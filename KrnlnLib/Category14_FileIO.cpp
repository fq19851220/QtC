#include "E2CppBase.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <random>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

// cmdCategory: 14
// categoryCn: 文件读写

namespace {

struct EFileHandle {
  std::fstream stream;
  std::string path;
  std::int32_t mode = 3;
  bool is_memory_file = false;
  bool is_encrypted_file = false;
  std::string encrypted_real_path;
  std::string encrypted_password;
  std::int32_t plain_prefix_len = 0;
};

static std::map<std::int32_t, EFileHandle> g_FileHandles;
static std::int32_t g_NextFileHandle = 1;

static constexpr std::size_t kEncSaltSize = 16;
static constexpr std::size_t kEncNonceSize = 24;
static constexpr std::size_t kEncMacSize = 16;
static constexpr std::size_t kEncHeaderSize = 8 + 1 + 3 + 4 + 8 + kEncSaltSize +
                                              kEncNonceSize + kEncMacSize;
static constexpr std::array<std::uint8_t, 8> kEncMagic = {'E', '2', 'C', 'E',
                                                           'N', 'C', '2', 0};

#ifdef _WIN32
struct FileLockEntry {
  std::int32_t file_no;
  std::int64_t pos;
  std::int64_t len;
  HANDLE lock_handle;
};
static std::vector<FileLockEntry> g_FileLocks;
#elif defined(__linux__) || defined(__APPLE__)
#include <fcntl.h>
#include <unistd.h>
struct FileLockEntry {
  std::int32_t file_no;
  std::int64_t pos;
  std::int64_t len;
  int fd; // dup'd fd kept open to hold the lock
};
static std::vector<FileLockEntry> g_FileLocks;
#endif

inline bool 是否可写模式(std::int32_t 打开方式) {
  return 打开方式 != 1;
}

inline bool 是否可读模式(std::int32_t 打开方式) {
  return 打开方式 == 1 || 打开方式 == 3 || 打开方式 == 6;
}

inline std::string 生成临时文件路径(const std::string &prefix) {
#ifdef _WIN32
  char temp_dir[MAX_PATH] = {0};
  char temp_file[MAX_PATH] = {0};
  if (GetTempPathA(MAX_PATH, temp_dir) == 0)
    return "";
  if (GetTempFileNameA(temp_dir, prefix.c_str(), 0, temp_file) == 0)
    return "";
  return std::string(temp_file);
#else
  try {
    auto p = fs::temp_directory_path() /
             (prefix + std::to_string(std::chrono::steady_clock::now()
                                          .time_since_epoch()
                                          .count()) +
              ".tmp");
    return p.string();
  } catch (...) {
    return "";
  }
#endif
}

inline void 写入LE32(std::vector<std::uint8_t> &out, std::uint32_t v) {
  out.push_back(static_cast<std::uint8_t>(v & 0xFF));
  out.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFF));
  out.push_back(static_cast<std::uint8_t>((v >> 16) & 0xFF));
  out.push_back(static_cast<std::uint8_t>((v >> 24) & 0xFF));
}

inline void 写入LE64(std::vector<std::uint8_t> &out, std::uint64_t v) {
  for (int i = 0; i < 8; ++i)
    out.push_back(static_cast<std::uint8_t>((v >> (8 * i)) & 0xFF));
}

inline std::uint32_t 读取LE32(const std::uint8_t *p) {
  return static_cast<std::uint32_t>(p[0]) |
         (static_cast<std::uint32_t>(p[1]) << 8) |
         (static_cast<std::uint32_t>(p[2]) << 16) |
         (static_cast<std::uint32_t>(p[3]) << 24);
}

inline std::uint64_t 读取LE64(const std::uint8_t *p) {
  std::uint64_t v = 0;
  for (int i = 0; i < 8; ++i)
    v |= (static_cast<std::uint64_t>(p[i]) << (8 * i));
  return v;
}

inline void 填充随机(std::uint8_t *buf, std::size_t size) {
  std::random_device rd;
  for (std::size_t i = 0; i < size; ++i) {
    buf[i] = static_cast<std::uint8_t>(rd() & 0xFF);
  }
}

inline void 派生密钥(const std::string &password, const std::uint8_t salt[16],
                 std::uint8_t key[32]) {
  const std::string kCtx = "E2Cpp.FileEnc.V2.Key";
  std::vector<std::uint8_t> msg(kCtx.begin(), kCtx.end());
  msg.insert(msg.end(), salt, salt + kEncSaltSize);

  const std::uint8_t fallback = 0;
  const std::uint8_t *pass_ptr = password.empty()
                                     ? &fallback
                                     : reinterpret_cast<const std::uint8_t *>(
                                           password.data());
  const std::size_t pass_size = password.empty() ? 1 : password.size();

  crypto_blake2b_keyed(key, 32, pass_ptr, pass_size, msg.data(), msg.size());
}

inline void 计算校验标签(const std::uint8_t key[32], const std::uint8_t nonce[24],
                    std::uint32_t plain_prefix_len, const std::uint8_t *cipher,
                    std::size_t cipher_size, std::uint8_t mac[16]) {
  std::vector<std::uint8_t> payload;
  payload.reserve(4 + kEncNonceSize + cipher_size);
  写入LE32(payload, plain_prefix_len);
  payload.insert(payload.end(), nonce, nonce + kEncNonceSize);
  payload.insert(payload.end(), cipher, cipher + cipher_size);
  crypto_blake2b_keyed(mac, kEncMacSize, key, 32, payload.data(),
                       payload.size());
}

inline std::ios::openmode 取重开模式(std::int32_t 打开方式) {
  switch (打开方式) {
  case 1:
    return std::ios::binary | std::ios::in;
  default:
    // 对非 #读入 模式统一使用 in|out，避免重开时触发截断。
    return std::ios::binary | std::ios::in | std::ios::out;
  }
}

inline std::int64_t 取当前位置(std::fstream &s) {
  auto p = s.tellp();
  if (p >= 0)
    return static_cast<std::int64_t>(p);
  auto g = s.tellg();
  if (g >= 0)
    return static_cast<std::int64_t>(g);
  return 0;
}

inline void 重新定位读写位置(std::fstream &s, std::int64_t pos) {
  s.clear();
  s.seekg(pos, std::ios::beg);
  s.seekp(pos, std::ios::beg);
}

inline bool 重新打开文件流(EFileHandle &h, std::int64_t pos) {
  h.stream.close();
  h.stream.clear();
  h.stream.open(h.path, 取重开模式(h.mode));
  if (!h.stream.is_open())
    return false;
  重新定位读写位置(h.stream, pos);
  return true;
}

inline bool 打开文件流(EFileHandle &h, const std::string &path,
                     std::int32_t 打开方式) {
  std::error_code ec;
  const bool exists = fs::exists(path, ec) && !ec;

  switch (打开方式) {
  case 1: // #读入：不存在失败
    h.stream.open(path, std::ios::binary | std::ios::in);
    return h.stream.is_open();

  case 2: // #写出：不存在失败；存在时覆盖写，不清空
    if (!exists)
      return false;
    h.stream.open(path, std::ios::binary | std::ios::in | std::ios::out);
    return h.stream.is_open();

  case 3: // #读写：不存在失败
    if (!exists)
      return false;
    h.stream.open(path, std::ios::binary | std::ios::in | std::ios::out);
    return h.stream.is_open();

  case 4: // #重写：不存在创建；存在清空
    h.stream.open(path, std::ios::binary | std::ios::out | std::ios::trunc);
    return h.stream.is_open();

  case 5: // #改写：不存在创建；存在时不清空
  case 6: // #改读：不存在创建；存在时不清空
    if (!exists) {
      std::ofstream creator(path, std::ios::binary);
      if (!creator)
        return false;
      creator.close();
    }
    h.stream.open(path, std::ios::binary | std::ios::in | std::ios::out);
    return h.stream.is_open();

  default:
    return false;
  }
}

inline EFileHandle *取文件句柄(std::int32_t 文件号) {
  auto it = g_FileHandles.find(文件号);
  if (it == g_FileHandles.end())
    return nullptr;
  return &it->second;
}

inline std::string 值到文本(const E2CppValue &值) {
  return std::visit(
      overloaded{
          [](std::monostate) -> std::string { return ""; },
          [](std::int32_t v) -> std::string { return std::to_string(v); },
          [](std::int64_t v) -> std::string { return std::to_string(v); },
          [](float v) -> std::string {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%g", v);
            return buf;
          },
          [](double v) -> std::string {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%g", v);
            return buf;
          },
          [](const QDateTime &v) -> std::string {
            return E2ToStdString(时间到文本(v, 1));
          },
          [](bool v) -> std::string { return v ? "真" : "假"; },
          [](const std::string &v) -> std::string { return v; },
          [](const E2CppByteArray &v) -> std::string {
            return "[字节集:" + std::to_string(v.size()) + "字节]";
          },
          [](void *v) -> std::string {
            char buf[64];
            std::snprintf(buf, sizeof(buf), "[指针:%p]", v);
            return buf;
          }},
      值.value);
}

inline bool 覆盖写回文件(const std::string &path, const std::vector<std::uint8_t> &data) {
  std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
  if (!ofs)
    return false;
  if (!data.empty()) {
    ofs.write(reinterpret_cast<const char *>(data.data()),
              static_cast<std::streamsize>(data.size()));
  }
  return ofs.good();
}

inline bool 读取整个文件(const std::string &path, std::vector<std::uint8_t> &out) {
  out.clear();
  std::ifstream ifs(path, std::ios::binary | std::ios::ate);
  if (!ifs)
    return false;

  std::streamsize size = ifs.tellg();
  if (size < 0)
    return false;

  ifs.seekg(0, std::ios::beg);
  out.resize(static_cast<std::size_t>(size));
  if (size > 0) {
    ifs.read(reinterpret_cast<char *>(out.data()), size);
    if (!ifs.good() && !ifs.eof())
      return false;
  }
  return true;
}

inline bool 加密打包数据(const std::vector<std::uint8_t> &plain,
                    const std::string &password, std::int32_t plain_prefix_len,
                    std::vector<std::uint8_t> &out) {
  out.clear();
  const std::size_t prefix_len = static_cast<std::size_t>(
      (std::max)(0, (std::min)(plain_prefix_len,
                               static_cast<std::int32_t>(plain.size()))));
  const std::size_t tail_len = plain.size() - prefix_len;

  std::array<std::uint8_t, 16> salt = {};
  std::array<std::uint8_t, 24> nonce = {};
  std::array<std::uint8_t, 32> key = {};
  std::array<std::uint8_t, 16> mac = {};
  std::vector<std::uint8_t> cipher(tail_len);

  填充随机(salt.data(), salt.size());
  填充随机(nonce.data(), nonce.size());
  派生密钥(password, salt.data(), key.data());

  if (tail_len > 0) {
    crypto_chacha20_x(cipher.data(), plain.data() + prefix_len, tail_len,
                      key.data(), nonce.data(), 0);
  }
  计算校验标签(key.data(), nonce.data(), static_cast<std::uint32_t>(prefix_len),
             cipher.data(), cipher.size(), mac.data());

  out.reserve(kEncHeaderSize + plain.size());
  out.insert(out.end(), kEncMagic.begin(), kEncMagic.end());
  out.push_back(1); // version
  out.push_back(0);
  out.push_back(0);
  out.push_back(0);
  写入LE32(out, static_cast<std::uint32_t>(prefix_len));
  写入LE64(out, static_cast<std::uint64_t>(tail_len));
  out.insert(out.end(), salt.begin(), salt.end());
  out.insert(out.end(), nonce.begin(), nonce.end());
  out.insert(out.end(), mac.begin(), mac.end());
  out.insert(out.end(), plain.begin(), plain.begin() + static_cast<std::ptrdiff_t>(prefix_len));
  out.insert(out.end(), cipher.begin(), cipher.end());

  crypto_wipe(key.data(), key.size());
  return true;
}

inline bool 解包解密数据(const std::vector<std::uint8_t> &in,
                    const std::string &password, std::vector<std::uint8_t> &plain,
                    std::int32_t &plain_prefix_len, bool &is_new_format) {
  plain.clear();
  plain_prefix_len = 0;
  is_new_format = false;

  if (in.size() < kEncHeaderSize || !std::equal(kEncMagic.begin(), kEncMagic.end(), in.begin())) {
    plain = in;
    return true;
  }

  is_new_format = true;
  const std::uint8_t *p = in.data();
  const std::uint8_t version = p[8];
  if (version != 1) {
    return false;
  }

  const std::uint32_t prefix_len_u32 = 读取LE32(p + 12);
  const std::uint64_t tail_len_u64 = 读取LE64(p + 16);
  const std::size_t prefix_len = static_cast<std::size_t>(prefix_len_u32);
  const std::size_t tail_len = static_cast<std::size_t>(tail_len_u64);
  if (kEncHeaderSize + prefix_len + tail_len != in.size()) {
    return false;
  }

  const std::uint8_t *salt = p + 24;
  const std::uint8_t *nonce = p + 24 + kEncSaltSize;
  const std::uint8_t *stored_mac = p + 24 + kEncSaltSize + kEncNonceSize;
  const std::uint8_t *plain_prefix = p + kEncHeaderSize;
  const std::uint8_t *cipher = plain_prefix + prefix_len;

  std::array<std::uint8_t, 32> key = {};
  std::array<std::uint8_t, 16> calc_mac = {};
  派生密钥(password, salt, key.data());
  计算校验标签(key.data(), nonce, prefix_len_u32, cipher, tail_len, calc_mac.data());

  std::vector<std::uint8_t> tail_plain(tail_len);
  if (tail_len > 0) {
    crypto_chacha20_x(tail_plain.data(), cipher, tail_len, key.data(), nonce, 0);
  }

  // 为兼容易语言行为，即使密码错误（校验不一致）也继续返回解密结果。
  (void)stored_mac;

  plain.reserve(prefix_len + tail_len);
  plain.insert(plain.end(), plain_prefix, plain_prefix + prefix_len);
  plain.insert(plain.end(), tail_plain.begin(), tail_plain.end());
  plain_prefix_len = static_cast<std::int32_t>(prefix_len_u32);

  crypto_wipe(key.data(), key.size());
  return true;
}

inline void 内部关闭文件(std::int32_t 文件号) {
  auto it = g_FileHandles.find(文件号);
  if (it == g_FileHandles.end())
    return;
  auto &fh = it->second;

#ifdef _WIN32
  for (std::size_t i = 0; i < g_FileLocks.size();) {
    if (g_FileLocks[i].file_no == 文件号) {
      OVERLAPPED ov = {};
      ov.Offset = static_cast<DWORD>(g_FileLocks[i].pos & 0xFFFFFFFFULL);
      ov.OffsetHigh = static_cast<DWORD>((g_FileLocks[i].pos >> 32) & 0xFFFFFFFFULL);
      UnlockFileEx(g_FileLocks[i].lock_handle, 0,
                   static_cast<DWORD>(g_FileLocks[i].len & 0xFFFFFFFFULL),
                   static_cast<DWORD>((g_FileLocks[i].len >> 32) & 0xFFFFFFFFULL),
                   &ov);
      CloseHandle(g_FileLocks[i].lock_handle);
      g_FileLocks.erase(g_FileLocks.begin() + static_cast<std::ptrdiff_t>(i));
      continue;
    }
    ++i;
  }
#endif

  if (fh.is_encrypted_file) {
    fh.stream.flush();
    fh.stream.close();

    if (是否可写模式(fh.mode) && !fh.encrypted_real_path.empty()) {
      std::vector<std::uint8_t> plain;
      if (读取整个文件(fh.path, plain)) {
        std::vector<std::uint8_t> packed;
        if (加密打包数据(plain, fh.encrypted_password, fh.plain_prefix_len, packed)) {
          覆盖写回文件(fh.encrypted_real_path, packed);
        }
      }
    }
  } else {
    fh.stream.close();
  }

  if ((fh.is_memory_file || fh.is_encrypted_file) && !fh.path.empty()) {
    std::error_code ec;
    fs::remove(fh.path, ec);
  }

  g_FileHandles.erase(it);
}

} // namespace

/**
 * @brief 打开文件
 */
inline std::int32_t 打开文件(const std::string &欲打开的文件名称,
                            std::int32_t 打开方式 = 3,
                            std::int32_t 共享方式 = 1) {
  (void)共享方式;

  EFileHandle h;
  h.path = 欲打开的文件名称;
  h.mode = 打开方式;

  if (!打开文件流(h, 欲打开的文件名称, 打开方式))
    return 0;

  std::int32_t id = g_NextFileHandle++;
  g_FileHandles.emplace(id, std::move(h));
  return id;
}

/**
 * @brief 打开内存文件（以临时文件模拟，跨平台）
 */
inline std::int32_t 打开内存文件() {
  const std::string path = 生成临时文件路径("e2m");
  if (path.empty())
    return 0;

  EFileHandle h;
  h.path = path;
  h.mode = 3;
  h.is_memory_file = true;

  if (!打开文件流(h, h.path, h.mode)) {
    std::error_code ec;
    fs::remove(h.path, ec);
    return 0;
  }

  std::int32_t id = g_NextFileHandle++;
  g_FileHandles.emplace(id, std::move(h));
  return id;
}

/**
 * @brief 关闭文件
 */
inline void 关闭文件(std::int32_t 欲关闭的文件号) { 内部关闭文件(欲关闭的文件号); }

/**
 * @brief 关闭所有文件
 */
inline void 关闭所有文件() {
  std::vector<std::int32_t> ids;
  ids.reserve(g_FileHandles.size());
  for (const auto &kv : g_FileHandles) {
    ids.push_back(kv.first);
  }
  for (auto id : ids) {
    内部关闭文件(id);
  }
}

/**
 * @brief 锁住文件（字节范围独占锁，跨平台）
 */
inline bool 锁住文件(std::int32_t 欲加锁或解锁的文件号,
                     std::int32_t 欲加锁或解锁的位置,
                     std::int32_t 欲加锁或解锁的长度,
                     std::int32_t 加锁重试时间 = 0) {
  EFileHandle *h = 取文件句柄(欲加锁或解锁的文件号);
  if (!h || h->path.empty() || 欲加锁或解锁的长度 <= 0)
    return false;

  const auto deadline = std::chrono::steady_clock::now() +
                        std::chrono::milliseconds((std::max)(0, 加锁重试时间));

#ifdef _WIN32
  do {
    HANDLE fh = CreateFileA(h->path.c_str(), GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fh == INVALID_HANDLE_VALUE) {
      if (std::chrono::steady_clock::now() >= deadline) return false;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }
    OVERLAPPED ov = {};
    ov.Offset     = static_cast<DWORD>(static_cast<std::uint32_t>(欲加锁或解锁的位置));
    ov.OffsetHigh = static_cast<DWORD>(static_cast<std::uint64_t>(
                        static_cast<std::uint32_t>(欲加锁或解锁的位置)) >> 32);
    BOOL ok = LockFileEx(fh, LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0,
                         static_cast<DWORD>(static_cast<std::uint32_t>(欲加锁或解锁的长度)),
                         static_cast<DWORD>(static_cast<std::uint64_t>(
                             static_cast<std::uint32_t>(欲加锁或解锁的长度)) >> 32),
                         &ov);
    if (ok) {
      g_FileLocks.push_back({欲加锁或解锁的文件号, 欲加锁或解锁的位置,
                             欲加锁或解锁的长度, fh});
      return true;
    }
    CloseHandle(fh);
    if (std::chrono::steady_clock::now() >= deadline) return false;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  } while (true);

#elif defined(__linux__) || defined(__APPLE__)
  do {
    int fd = ::open(h->path.c_str(), O_RDWR);
    if (fd < 0) {
      if (std::chrono::steady_clock::now() >= deadline) return false;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }
    struct ::flock fl{};
    fl.l_type   = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = static_cast<off_t>(欲加锁或解锁的位置);
    fl.l_len    = static_cast<off_t>(欲加锁或解锁的长度);
    if (::fcntl(fd, F_SETLK, &fl) == 0) {
      g_FileLocks.push_back({欲加锁或解锁的文件号, 欲加锁或解锁的位置,
                             欲加锁或解锁的长度, fd});
      return true;
    }
    ::close(fd);
    if (std::chrono::steady_clock::now() >= deadline) return false;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  } while (true);
#else
  (void)deadline;
  return false;
#endif
}

/**
 * @brief 解锁文件（跨平台）
 */
inline bool 解锁文件(std::int32_t 欲加锁或解锁的文件号,
                     std::int32_t 欲加锁或解锁的位置,
                     std::int32_t 欲加锁或解锁的长度) {
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
  for (std::size_t i = 0; i < g_FileLocks.size(); ++i) {
    auto &entry = g_FileLocks[i];
    if (entry.file_no != 欲加锁或解锁的文件号 || entry.pos != 欲加锁或解锁的位置 ||
        entry.len != 欲加锁或解锁的长度)
      continue;
#ifdef _WIN32
    OVERLAPPED ov = {};
    ov.Offset     = static_cast<DWORD>(entry.pos & 0xFFFFFFFFULL);
    ov.OffsetHigh = static_cast<DWORD>((entry.pos >> 32) & 0xFFFFFFFFULL);
    BOOL ok = UnlockFileEx(entry.lock_handle, 0,
                           static_cast<DWORD>(entry.len & 0xFFFFFFFFULL),
                           static_cast<DWORD>((entry.len >> 32) & 0xFFFFFFFFULL),
                           &ov);
    CloseHandle(entry.lock_handle);
    g_FileLocks.erase(g_FileLocks.begin() + static_cast<std::ptrdiff_t>(i));
    return ok != FALSE;
#else
    struct ::flock fl{};
    fl.l_type   = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = static_cast<off_t>(entry.pos);
    fl.l_len    = static_cast<off_t>(entry.len);
    ::fcntl(entry.fd, F_SETLK, &fl);
    ::close(entry.fd);
    g_FileLocks.erase(g_FileLocks.begin() + static_cast<std::ptrdiff_t>(i));
    return true;
#endif
  }
  return false;
#else
  return false;
#endif
}

/**
 * @brief 移动读写位置
 * @param 起始移动位置 1=文件首, 2=当前位置, 3=文件尾
 */
inline bool 移动读写位置(std::int32_t 欲进行操作的文件号,
                         std::int32_t 起始移动位置,
                         std::int32_t 移动距离) {
  EFileHandle *h = 取文件句柄(欲进行操作的文件号);
  if (!h)
    return false;

  std::ios::seekdir dir = std::ios::beg;
  switch (起始移动位置) {
  case 2:
    dir = std::ios::cur;
    break;
  case 3:
    dir = std::ios::end;
    break;
  default:
    dir = std::ios::beg;
    break;
  }

  h->stream.clear();
  h->stream.seekg(移动距离, dir);
  h->stream.seekp(移动距离, dir);
  return h->stream.good();
}

/**
 * @brief 移到文件首
 */
inline bool 移到文件首(std::int32_t 欲进行操作的文件号) {
  return 移动读写位置(欲进行操作的文件号, 1, 0);
}

/**
 * @brief 移到文件尾
 */
inline bool 移到文件尾(std::int32_t 欲进行操作的文件号) {
  return 移动读写位置(欲进行操作的文件号, 3, 0);
}

/**
 * @brief 读入字节集
 */
inline E2CppByteArray 读入字节集(std::int32_t 欲读入数据的文件号,
                                std::int32_t 欲读入数据的长度) {
  EFileHandle *h = 取文件句柄(欲读入数据的文件号);
  E2CppByteArray out;
  if (!h || !是否可读模式(h->mode) || 欲读入数据的长度 <= 0)
    return out;

  out.m_data.resize(static_cast<std::size_t>(欲读入数据的长度));
  h->stream.read(reinterpret_cast<char *>(out.m_data.data()), 欲读入数据的长度);
  out.m_data.resize(static_cast<std::size_t>(h->stream.gcount()));
  return out;
}

/**
 * @brief 写出字节集
 */
inline bool 写出字节集(std::int32_t 欲写出数据的文件号,
                     const E2CppByteArray &欲写出的字节集数据) {
  EFileHandle *h = 取文件句柄(欲写出数据的文件号);
  if (!h || !是否可写模式(h->mode))
    return false;

  if (!欲写出的字节集数据.m_data.empty()) {
    h->stream.write(reinterpret_cast<const char *>(欲写出的字节集数据.data()),
                    static_cast<std::streamsize>(欲写出的字节集数据.size()));
  }
  return h->stream.good();
}

/**
 * @brief 读入文本
 */
inline std::string 读入文本(std::int32_t 欲读入文本数据的文件号,
                          std::int32_t 欲读入文本数据的长度 = -1) {
  EFileHandle *h = 取文件句柄(欲读入文本数据的文件号);
  if (!h || !是否可读模式(h->mode))
    return "";

  std::string out;
  if (欲读入文本数据的长度 < 0) {
    char ch = 0;
    while (h->stream.get(ch)) {
      if (ch == 0 || static_cast<unsigned char>(ch) == 26)
        break;
      out.push_back(ch);
    }
    return out;
  }

  out.resize(static_cast<std::size_t>(欲读入文本数据的长度));
  if (!out.empty()) {
    h->stream.read(&out[0], 欲读入文本数据的长度);
    out.resize(static_cast<std::size_t>(h->stream.gcount()));
  }

  auto pos0 = out.find('\0');
  auto pos26 = out.find(static_cast<char>(26));
  std::size_t cut = std::string::npos;
  if (pos0 != std::string::npos)
    cut = pos0;
  if (pos26 != std::string::npos)
    cut = (cut == std::string::npos) ? pos26 : (std::min)(cut, pos26);
  if (cut != std::string::npos)
    out.resize(cut);

  return out;
}

/**
 * @brief 写出文本
 */
inline bool 写出文本(std::int32_t 欲写出文本的文件号, const E2CppValue &欲写出的文本) {
  EFileHandle *h = 取文件句柄(欲写出文本的文件号);
  if (!h || !是否可写模式(h->mode))
    return false;

  std::string text = 值到文本(欲写出的文本);
  if (!text.empty()) {
    h->stream.write(text.data(), static_cast<std::streamsize>(text.size()));
  }
  return h->stream.good();
}

template <typename... Args>
inline bool 写出文本(std::int32_t 欲写出文本的文件号,
                 const E2CppValue &欲写出的文本,
                 const Args &...其余欲写出的文本) {
  std::string text = 值到文本(欲写出的文本);
  ((text += 值到文本(其余欲写出的文本)), ...);
  return 写出文本(欲写出文本的文件号, E2CppValue(text));
}

/**
 * @brief 读入一行
 */
inline std::string 读入一行(std::int32_t 欲读入文本数据的文件号) {
  EFileHandle *h = 取文件句柄(欲读入文本数据的文件号);
  if (!h || !是否可读模式(h->mode))
    return "";

  std::string line;
  if (!std::getline(h->stream, line))
    return "";
  if (!line.empty() && line.back() == '\r')
    line.pop_back();
  return line;
}

/**
 * @brief 写文本行
 */
inline bool 写文本行(std::int32_t 欲写出文本的文件号, const E2CppValue &欲写出的文本) {
  EFileHandle *h = 取文件句柄(欲写出文本的文件号);
  if (!h || !是否可写模式(h->mode))
    return false;

  std::string text = 值到文本(欲写出的文本);
  h->stream.write(text.data(), static_cast<std::streamsize>(text.size()));
  h->stream.write("\r\n", 2);
  return h->stream.good();
}

template <typename... Ts> struct E2CppAlwaysFalse : std::false_type {};

namespace {
template <typename T>
struct is_rw_supported_scalar
    : std::bool_constant<
          std::is_same_v<T, std::int32_t> || std::is_same_v<T, std::int16_t> ||
          std::is_same_v<T, std::int64_t> || std::is_same_v<T, std::uint8_t> ||
          std::is_same_v<T, float> || std::is_same_v<T, double> ||
          std::is_same_v<T, bool> || std::is_same_v<T, E2CppDateTime> ||
          std::is_same_v<T, 子程序指针型> || std::is_same_v<T, void *> ||
          std::is_same_v<T, std::string> ||
          std::is_same_v<T, E2CppByteArray>> {};

template <typename T> struct is_rw_supported_value : is_rw_supported_scalar<T> {};
template <typename T>
struct is_rw_supported_value<E2CppArray<T>>
    : std::bool_constant<is_rw_supported_scalar<T>::value> {};

inline bool 读取精确字节(std::fstream &stream, void *dst, std::size_t size) {
  if (size == 0) {
    return true;
  }
  stream.read(reinterpret_cast<char *>(dst), static_cast<std::streamsize>(size));
  return stream.good() || stream.gcount() == static_cast<std::streamsize>(size);
}

inline bool 写出精确字节(std::fstream &stream, const void *src, std::size_t size) {
  if (size == 0) {
    return true;
  }
  stream.write(reinterpret_cast<const char *>(src),
               static_cast<std::streamsize>(size));
  return stream.good();
}

template <typename T>
inline std::enable_if_t<std::is_same_v<T, E2CppDateTime>, bool>
写出单个格式数据(std::fstream &stream, const T &value) {
  // 日期时间按 E 运行时常用的 VariantTime 双精度格式写入，读写配对稳定。
  const double raw = static_cast<double>(value);
  return 写出精确字节(stream, &raw, sizeof(raw));
}

template <typename T>
inline std::enable_if_t<std::is_same_v<T, bool>, bool>
写出单个格式数据(std::fstream &stream, const T &value) {
  const std::uint8_t raw = value ? 1 : 0;
  return 写出精确字节(stream, &raw, sizeof(raw));
}

template <typename T>
inline std::enable_if_t<std::is_same_v<T, std::string>, bool>
写出单个格式数据(std::fstream &stream, const T &value) {
  // 文本格式：原始文本字节 + 0 结束符。
  if (!写出精确字节(stream, value.data(), value.size())) {
    return false;
  }
  const char terminator = 0;
  return 写出精确字节(stream, &terminator, 1);
}

template <typename T>
inline std::enable_if_t<std::is_same_v<T, E2CppByteArray>, bool>
写出单个格式数据(std::fstream &stream, const T &value) {
  // 字节集格式：长度(int32) + 实际数据。
  const std::int32_t len = static_cast<std::int32_t>(value.size());
  if (!写出精确字节(stream, &len, sizeof(len))) {
    return false;
  }
  return 写出精确字节(stream, value.data(), value.size());
}

template <typename T>
inline std::enable_if_t<
    !std::is_same_v<T, E2CppDateTime> && !std::is_same_v<T, bool> &&
        !std::is_same_v<T, std::string> && !std::is_same_v<T, E2CppByteArray>,
    bool>
写出单个格式数据(std::fstream &stream, const T &value) {
  return 写出精确字节(stream, &value, sizeof(value));
}

template <typename T>
inline std::enable_if_t<std::is_same_v<T, E2CppDateTime>, bool>
读入单个格式数据(std::fstream &stream, T &value) {
  double raw = 0.0;
  if (!读取精确字节(stream, &raw, sizeof(raw))) {
    return false;
  }
  value = E2CppDateTime::fromEString(std::to_string(raw));
  value = static_cast<E2CppDateTime>(E2CppValue(raw));
  return true;
}

template <typename T>
inline std::enable_if_t<std::is_same_v<T, bool>, bool>
读入单个格式数据(std::fstream &stream, T &value) {
  std::uint8_t raw = 0;
  if (!读取精确字节(stream, &raw, sizeof(raw))) {
    return false;
  }
  value = (raw != 0);
  return true;
}

template <typename T>
inline std::enable_if_t<std::is_same_v<T, std::string>, bool>
读入单个格式数据(std::fstream &stream, T &value) {
  value.clear();
  char ch = 0;
  while (stream.get(ch)) {
    if (ch == 0) {
      return true;
    }
    value.push_back(ch);
  }
  return false;
}

template <typename T>
inline std::enable_if_t<std::is_same_v<T, E2CppByteArray>, bool>
读入单个格式数据(std::fstream &stream, T &value) {
  std::int32_t len = 0;
  if (!读取精确字节(stream, &len, sizeof(len)) || len < 0) {
    return false;
  }
  value.m_data.resize(static_cast<std::size_t>(len));
  return 读取精确字节(stream, value.data(), value.size());
}

template <typename T>
inline std::enable_if_t<
    !std::is_same_v<T, E2CppDateTime> && !std::is_same_v<T, bool> &&
        !std::is_same_v<T, std::string> && !std::is_same_v<T, E2CppByteArray>,
    bool>
读入单个格式数据(std::fstream &stream, T &value) {
  return 读取精确字节(stream, &value, sizeof(value));
}

template <typename T>
inline std::enable_if_t<is_rw_supported_scalar<T>::value, bool>
写出格式数据值(std::fstream &stream, const T &value) {
  return 写出单个格式数据(stream, value);
}

template <typename T>
inline std::enable_if_t<is_rw_supported_scalar<T>::value, bool>
读入格式数据值(std::fstream &stream, T &value) {
  return 读入单个格式数据(stream, value);
}

template <typename T>
inline std::enable_if_t<is_rw_supported_scalar<T>::value, bool>
写出格式数据值(std::fstream &stream, const E2CppArray<T> &value) {
  // 数组不写长度；格式为每个成员的顺序拼接，与易语言 read/write 配对一致。
  for (const auto &item : value.data) {
    if (!写出单个格式数据(stream, item)) {
      return false;
    }
  }
  return true;
}

template <typename T>
inline std::enable_if_t<is_rw_supported_scalar<T>::value, bool>
读入格式数据值(std::fstream &stream, E2CppArray<T> &value) {
  for (auto &item : value.data) {
    if (!读入单个格式数据(stream, item)) {
      return false;
    }
  }
  return true;
}

template <typename T>
inline std::enable_if_t<!is_rw_supported_value<std::decay_t<T>>::value, bool>
写出格式数据值(std::fstream &, const T &) {
  return false;
}

template <typename T>
inline std::enable_if_t<!is_rw_supported_value<std::decay_t<T>>::value, bool>
读入格式数据值(std::fstream &, T &) {
  return false;
}
} // namespace

template <typename T>
inline bool 读入数据(std::int32_t 欲读入数据的文件号, T &存放所读入数据的变量) {
  EFileHandle *h = 取文件句柄(欲读入数据的文件号);
  if (!h || !是否可读模式(h->mode)) {
    return false;
  }
  return 读入格式数据值(h->stream, 存放所读入数据的变量);
}

template <typename T, typename... Args>
inline bool 读入数据(std::int32_t 欲读入数据的文件号, T &存放所读入数据的变量,
                 Args &...其余变量) {
  return 读入数据(欲读入数据的文件号, 存放所读入数据的变量) &&
         (... && 读入数据(欲读入数据的文件号, 其余变量));
}

template <typename T>
inline bool 写出数据(std::int32_t 欲写出数据的文件号, const T &欲写出的数据) {
  EFileHandle *h = 取文件句柄(欲写出数据的文件号);
  if (!h || !是否可写模式(h->mode)) {
    return false;
  }
  return 写出格式数据值(h->stream, 欲写出的数据);
}

template <typename T, typename... Args>
inline bool 写出数据(std::int32_t 欲写出数据的文件号, const T &欲写出的数据,
                 const Args &...其余欲写出的数据) {
  return 写出数据(欲写出数据的文件号, 欲写出的数据) &&
         (... && 写出数据(欲写出数据的文件号, 其余欲写出的数据));
}

/**
 * @brief 是否在文件尾
 */
inline bool 是否在文件尾(std::int32_t 文件号, bool 是否为判断文本已读完 = false) {
  EFileHandle *h = 取文件句柄(文件号);
  if (!h || !是否可读模式(h->mode))
    return true;

  if (h->stream.eof())
    return true;

  auto cur = 取当前位置(h->stream);
  h->stream.clear();
  h->stream.seekg(0, std::ios::end);
  auto end = static_cast<std::int64_t>(h->stream.tellg());
  重新定位读写位置(h->stream, cur);

  if (cur >= end)
    return true;

  if (是否为判断文本已读完) {
    int next = h->stream.peek();
    if (next == EOF || next == 0 || next == 26)
      return true;
  }

  return false;
}

/**
 * @brief 取读写位置
 */
inline std::int32_t 取读写位置(std::int32_t 文件号) {
  EFileHandle *h = 取文件句柄(文件号);
  if (!h)
    return -1;
  return static_cast<std::int32_t>(取当前位置(h->stream));
}

/**
 * @brief 取文件长度
 */
inline std::int32_t 取文件长度(std::int32_t 文件号) {
  EFileHandle *h = 取文件句柄(文件号);
  if (!h)
    return -1;

  std::error_code ec;
  auto size = fs::file_size(h->path, ec);
  if (ec)
    return -1;
  return static_cast<std::int32_t>(size);
}

/**
 * @brief 插入字节集
 */
inline bool 插入字节集(std::int32_t 欲写出数据的文件号,
                     const E2CppByteArray &欲写出的字节集数据) {
  EFileHandle *h = 取文件句柄(欲写出数据的文件号);
  if (!h || !是否可写模式(h->mode))
    return false;

  std::int64_t pos = (std::max)(std::int64_t(0), 取当前位置(h->stream));

  std::vector<std::uint8_t> file_data;
  if (!读取整个文件(h->path, file_data))
    return false;

  std::size_t insert_pos = static_cast<std::size_t>((std::min)(
      pos, static_cast<std::int64_t>(file_data.size())));

  file_data.insert(file_data.begin() + static_cast<std::ptrdiff_t>(insert_pos),
                   欲写出的字节集数据.m_data.begin(), 欲写出的字节集数据.m_data.end());

  if (!覆盖写回文件(h->path, file_data))
    return false;

  return 重新打开文件流(*h, static_cast<std::int64_t>(insert_pos + 欲写出的字节集数据.size()));
}

/**
 * @brief 插入文本
 */
inline bool 插入文本(std::int32_t 欲写出文本的文件号, const E2CppValue &欲写出的文本) {
  std::string text = 值到文本(欲写出的文本);
  E2CppByteArray bytes(text.begin(), text.end());
  return 插入字节集(欲写出文本的文件号, bytes);
}

/**
 * @brief 插入文本行
 */
inline bool 插入文本行(std::int32_t 欲写出文本的文件号,
                     const E2CppValue &欲写出的文本) {
  std::string text = 值到文本(欲写出的文本);
  text += "\r\n";
  E2CppByteArray bytes(text.begin(), text.end());
  return 插入字节集(欲写出文本的文件号, bytes);
}

/**
 * @brief 删除数据
 */
inline bool 删除数据(std::int32_t 文件号, std::int32_t 欲删除数据的字节数) {
  EFileHandle *h = 取文件句柄(文件号);
  if (!h || !是否可写模式(h->mode))
    return false;
  if (欲删除数据的字节数 <= 0)
    return true;

  std::int64_t pos = (std::max)(std::int64_t(0), 取当前位置(h->stream));

  std::vector<std::uint8_t> file_data;
  if (!读取整个文件(h->path, file_data))
    return false;

  std::size_t start = static_cast<std::size_t>(
      (std::min)(pos, static_cast<std::int64_t>(file_data.size())));
  std::size_t end = (std::min)(file_data.size(),
                               start + static_cast<std::size_t>(欲删除数据的字节数));

  file_data.erase(file_data.begin() + static_cast<std::ptrdiff_t>(start),
                  file_data.begin() + static_cast<std::ptrdiff_t>(end));

  if (!覆盖写回文件(h->path, file_data))
    return false;

  return 重新打开文件流(*h, static_cast<std::int64_t>(start));
}

/**
 * @brief 打开加密文件（仅 Windows）
 * @note 采用 XChaCha20(BLAKE2b 派生密钥) 文件容器格式，命令签名保持不变。
 */
inline std::int32_t 打开加密文件(const std::string &欲打开的文件名称,
                                std::int32_t 打开方式 = 3,
                                std::int32_t 共享方式 = 1,
                                const std::string &文件密码 = "",
                                std::int32_t 明文区长度 = 0) {
  (void)共享方式;
#ifdef _WIN32
  if (打开方式 < 1 || 打开方式 > 6)
    return 0;

  std::error_code ec;
  const bool exists = fs::exists(欲打开的文件名称, ec) && !ec;

  if ((打开方式 == 1 || 打开方式 == 2 || 打开方式 == 3) && !exists) {
    return 0;
  }

  std::vector<std::uint8_t> plain_data;
  std::int32_t effective_plain_prefix_len = (std::max)(0, 明文区长度);
  if (exists && 打开方式 != 4) {
    std::vector<std::uint8_t> raw;
    if (!读取整个文件(欲打开的文件名称, raw))
      return 0;
    bool is_new_format = false;
    if (!解包解密数据(raw, 文件密码, plain_data, effective_plain_prefix_len,
                 is_new_format)) {
      return 0;
    }
    if (!is_new_format) {
      effective_plain_prefix_len = (std::max)(0, 明文区长度);
    }
  }

  if (打开方式 == 4) {
    plain_data.clear();
    effective_plain_prefix_len = (std::max)(0, 明文区长度);
  }

  std::string temp_path = 生成临时文件路径("e2e");
  if (temp_path.empty())
    return 0;

  if (!覆盖写回文件(temp_path, plain_data)) {
    std::error_code ec2;
    fs::remove(temp_path, ec2);
    return 0;
  }

  EFileHandle h;
  h.path = temp_path;
  h.mode = 打开方式;
  h.is_encrypted_file = true;
  h.encrypted_real_path = 欲打开的文件名称;
  h.encrypted_password = 文件密码;
  h.plain_prefix_len = effective_plain_prefix_len;

  if (!打开文件流(h, h.path, 打开方式)) {
    std::error_code ec3;
    fs::remove(temp_path, ec3);
    return 0;
  }

  std::int32_t id = g_NextFileHandle++;
  g_FileHandles.emplace(id, std::move(h));
  return id;
#else
  return 0;
#endif
}

#ifdef QT_CORE_LIB

inline std::int32_t 打开文件(const QString &欲打开的文件名称, std::int32_t 打开方式 = 3,
                         std::int32_t 共享方式 = 1) {
  return 打开文件(E2ToStdString(欲打开的文件名称), 打开方式, 共享方式);
}

inline std::int32_t 打开加密文件(const QString &欲打开的文件名称, std::int32_t 打开方式 = 3,
                           std::int32_t 共享方式 = 1, const QString &文件密码 = QString(),
                           std::int32_t 明文区长度 = 0) {
  return 打开加密文件(E2ToStdString(欲打开的文件名称), 打开方式, 共享方式,
                  E2ToStdString(文件密码), 明文区长度);
}

inline bool 写出文本(std::int32_t 欲写出文本的文件号, const QString &欲写出的文本) {
  return 写出文本(欲写出文本的文件号, E2CppValue(E2ToStdString(欲写出的文本)));
}

template <typename... Args>
inline bool 写出文本(std::int32_t 欲写出文本的文件号, const QString &欲写出的文本,
                 const Args &...其余欲写出的文本) {
  QString text = 欲写出的文本;
  ((text += 到文本Q(其余欲写出的文本)), ...);
  return 写出文本(欲写出文本的文件号, text);
}

inline QString 读入一行Q(std::int32_t 欲读入文本数据的文件号) {
  return QString::fromStdString(读入一行(欲读入文本数据的文件号));
}

inline QString 读入文本Q(std::int32_t 欲读入文本数据的文件号) {
  return QString::fromStdString(读入文本(欲读入文本数据的文件号));
}

inline QString 读入文本Q(std::int32_t 欲读入文本数据的文件号,
                     std::int32_t 欲读入的文本长度) {
  return QString::fromStdString(读入文本(欲读入文本数据的文件号, 欲读入的文本长度));
}

#endif
