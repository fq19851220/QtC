#include "E2CppBase.h"
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef QT_CORE_LIB
#include <QtCore/QCoreApplication>
#include <QtCore/QEventLoop>
#include <QtCore/QLocale>
#endif

namespace fs = std::filesystem;

// cmdCategory: 18
// categoryCn: 其他

template <typename... Ts> struct E2CppAlwaysFalseOther : std::false_type {};

namespace {

static std::map<std::int32_t, E2CppByteArray> g_LoadedPics;
static std::int32_t g_NextPicId = 1;

static std::string g_DllRedirectPath;
static std::string g_DllRedirectCmd;
static std::string g_DllLoadPath;
static void *g_ErrorManager = nullptr;

inline std::string 值转普通文本_其他(const E2CppValue &v) {
  return std::visit(
      overloaded{
          [](std::monostate) -> std::string { return ""; },
          [](std::int32_t x) -> std::string { return std::to_string(x); },
          [](std::int64_t x) -> std::string { return std::to_string(x); },
          [](float x) -> std::string {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%g", x);
            return buf;
          },
          [](double x) -> std::string {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%g", x);
            return buf;
          },
          [](const QDateTime &x) -> std::string {
            return E2ToStdString(时间到文本(x, 1));
          },
          [](bool x) -> std::string { return x ? "真" : "假"; },
          [](const std::string &x) -> std::string { return x; },
          [](const E2CppByteArray &x) -> std::string {
            return "[字节集:" + std::to_string(x.size()) + "字节]";
          },
          [](void *p) -> std::string {
            char buf[64];
            std::snprintf(buf, sizeof(buf), "[指针:%p]", p);
            return buf;
          }},
      v.value);
}

inline void 安全写内存(const void *src, std::size_t src_size, std::intptr_t dst,
                       std::int32_t dst_size) {
  if (dst == 0 || src == nullptr || src_size == 0)
    return;

  std::size_t writable = src_size;
  if (dst_size >= 0) {
    writable = (std::min)(writable, static_cast<std::size_t>(dst_size));
  }

  if (writable > 0) {
    std::memcpy(reinterpret_cast<void *>(dst), src, writable);
  }
}

inline bool 运行控制台程序_内部(const std::string &cmd, std::string *outStd,
                                std::string *outErr, std::int32_t *retCode) {
  if (outStd)
    outStd->clear();
  if (outErr)
    outErr->clear();
  if (retCode)
    *retCode = -1;

  std::string mergedCmd = cmd;
#ifndef _WIN32
  mergedCmd += " 2>&1";
#else
  mergedCmd += " 2>&1";
#endif

#ifdef _WIN32
  FILE *pipe = _popen(mergedCmd.c_str(), "r");
#else
  FILE *pipe = popen(mergedCmd.c_str(), "r");
#endif
  if (!pipe)
    return false;

  char buf[4096];
  std::string output;
  while (std::fgets(buf, sizeof(buf), pipe)) {
    output += buf;
  }

#ifdef _WIN32
  int status = _pclose(pipe);
#else
  int status = pclose(pipe);
#endif

  if (outStd)
    *outStd = output;
  if (outErr)
    *outErr = "";
  if (retCode)
    *retCode = static_cast<std::int32_t>(status);

  return status != -1;
}

inline std::string 大写化(const std::string &s) {
  std::string out = s;
  for (auto &ch : out)
    ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
  return out;
}

} // namespace

/**
 * @brief 选择
 */
template <typename T>
inline T 选择(bool 用作选择的逻辑值, const T &待选择项一, const T &待选择项二) {
  return 用作选择的逻辑值 ? 待选择项一 : 待选择项二;
}

template <typename A, typename B,
          typename std::enable_if<
              !std::is_same_v<std::decay_t<A>, std::decay_t<B>> &&
                  !is_e2cpp_accessor<A>::value &&
                  !is_e2cpp_accessor<B>::value &&
                  (std::is_convertible_v<A, std::decay_t<A>> ||
                   std::is_convertible_v<B, std::decay_t<B>>),
              int>::type = 0>
inline auto 选择(bool 用作选择的逻辑值, const A &待选择项一, const B &待选择项二)
    -> std::common_type_t<std::decay_t<A>, std::decay_t<B>> {
  using R = std::common_type_t<std::decay_t<A>, std::decay_t<B>>;
  return 用作选择的逻辑值 ? static_cast<R>(待选择项一)
                        : static_cast<R>(待选择项二);
}

template <typename A, typename B,
          typename std::enable_if<is_e2cpp_accessor<A>::value &&
                                      !is_e2cpp_accessor<B>::value,
                                  int>::type = 0>
inline auto 选择(bool 用作选择的逻辑值, const A &待选择项一, const B &待选择项二)
    -> std::common_type_t<typename A::value_type, std::decay_t<B>> {
  using R = std::common_type_t<typename A::value_type, std::decay_t<B>>;
  return 用作选择的逻辑值 ? static_cast<R>(typename A::value_type(待选择项一))
                        : static_cast<R>(待选择项二);
}

template <typename A, typename B,
          typename std::enable_if<!is_e2cpp_accessor<A>::value &&
                                      is_e2cpp_accessor<B>::value,
                                  int>::type = 0>
inline auto 选择(bool 用作选择的逻辑值, const A &待选择项一, const B &待选择项二)
    -> std::common_type_t<std::decay_t<A>, typename B::value_type> {
  using R = std::common_type_t<std::decay_t<A>, typename B::value_type>;
  return 用作选择的逻辑值 ? static_cast<R>(待选择项一)
                        : static_cast<R>(typename B::value_type(待选择项二));
}

/**
 * @brief 多项选择
 */
template <typename T, typename... Rest,
          typename std::enable_if<(std::is_convertible_v<Rest, T> && ...),
                                  int>::type = 0>
inline T 多项选择(std::int32_t 索引值, const T &第1项, const Rest &...其余项) {
  std::vector<T> items{第1项, static_cast<T>(其余项)...};
  if (索引值 < 1 || static_cast<std::size_t>(索引值) > items.size()) {
    return T{};
  }
  return items[static_cast<std::size_t>(索引值 - 1)];
}

template <typename... Args>
inline E2CppValue 多项选择(std::int32_t 索引值, const Args &...待选择项数据) {
  std::vector<E2CppValue> items{E2CppValue(待选择项数据)...};
  if (索引值 < 1 || static_cast<std::size_t>(索引值) > items.size())
    return E2CppValue();
  return items[static_cast<std::size_t>(索引值 - 1)];
}

/**
 * @brief 是否为空
 */
inline bool 是否为空(const E2CppValue &欲测试的参数变量) {
  return std::holds_alternative<std::monostate>(欲测试的参数变量.value);
}

inline bool 是否为空(const std::string &欲测试的参数变量) {
  return 欲测试的参数变量.empty();
}

#ifdef QT_CORE_LIB
inline bool 是否为空(const QString &欲测试的参数变量) {
  return 欲测试的参数变量.isEmpty();
}
#endif

inline bool 是否为空(const E2CppByteArray &欲测试的参数变量) {
  return 欲测试的参数变量.size() == 0;
}

inline bool 是否为空(const E2CppDateTime &欲测试的参数变量) {
  return 欲测试的参数变量.is_empty;
}

template <typename T>
inline bool 是否为空(const E2CppArray<T> &欲测试的参数变量) {
  return 欲测试的参数变量.dims.empty();
}

template <typename T> inline bool 是否为空(const T &) { return false; }

/**
 * @brief 取数据类型尺寸
 */
inline std::int32_t 取数据类型尺寸(std::int32_t 欲取其尺寸的数据类型) {
  switch (欲取其尺寸的数据类型) {
  case DT_Byte:
    return 1;
  case DT_Short:
    return 2;
  case DT_Int:
    return 4;
  case DT_Int64:
    return 8;
  case DT_Float:
    return 4;
  case DT_Double:
    return 8;
  case DT_Bool:
    return 1;
  case DT_DateTime:
    return 8;
  case DT_SubPtr:
    return static_cast<std::int32_t>(sizeof(void *));
  case DT_Text:
    return static_cast<std::int32_t>(sizeof(char *));
  default:
    return -1;
  }
}

/**
 * @brief 复制内存
 */
inline void 复制内存(std::intptr_t 目标内存地址, std::intptr_t 源内存地址,
                     std::int32_t 复制长度) {
  if (目标内存地址 == 0 || 源内存地址 == 0 || 复制长度 <= 0)
    return;
  std::memcpy(reinterpret_cast<void *>(目标内存地址),
              reinterpret_cast<const void *>(源内存地址),
              static_cast<std::size_t>(复制长度));
}

/**
 * @brief 分配内存
 */
inline std::intptr_t 分配内存(std::int32_t 欲分配的字节数) {
  if (欲分配的字节数 <= 0)
    return 0;
  return reinterpret_cast<std::intptr_t>(std::malloc(欲分配的字节数));
}

/**
 * @brief 申请内存
 */
inline std::intptr_t 申请内存(std::int32_t 欲申请的内存字节数,
                              bool 是否清零 = false) {
  std::intptr_t p = 分配内存(欲申请的内存字节数);
  if (p != 0 && 是否清零 && 欲申请的内存字节数 > 0) {
    std::memset(reinterpret_cast<void *>(p), 0,
                static_cast<std::size_t>(欲申请的内存字节数));
  }
  return p;
}

/**
 * @brief 释放内存
 */
inline void 释放内存(std::intptr_t 这里的内存地址) {
  if (这里的内存地址 == 0)
    return;
  std::free(reinterpret_cast<void *>(这里的内存地址));
}

/**
 * @brief 取变量地址
 */
template <typename T> inline std::intptr_t 取变量地址(T &变量) {
  return reinterpret_cast<std::intptr_t>(&变量);
}

/**
 * @brief 取变量数据地址
 */
inline std::intptr_t 取变量数据地址(const std::string &变量) {
  return reinterpret_cast<std::intptr_t>(变量.c_str());
}

#ifdef QT_CORE_LIB
inline std::intptr_t 取变量数据地址(const QString &变量) {
  return reinterpret_cast<std::intptr_t>(变量.utf16());
}
#endif

inline std::intptr_t 取变量数据地址(const E2CppByteArray &变量) {
  return reinterpret_cast<std::intptr_t>(变量.data());
}

// RAII wrapper: adapts std::int32_t& to bool* for Qt ok-parameter pattern
// Destructor writes bool result back to the int32 variable
struct E2QtOkParam {
  std::int32_t &ref;
  bool ok = false;
  explicit E2QtOkParam(std::int32_t &v) : ref(v) {}
  ~E2QtOkParam() { ref = ok ? 1 : 0; }
  operator bool*() { return &ok; }
};

// std::int32_t 重载：供 Qt ok 参数使用，例如 字节数组.toInt(取变量数据地址(成功))
inline E2QtOkParam 取变量数据地址(std::int32_t &变量) {
  return E2QtOkParam{变量};
}

// 通用模板重载：返回 T*，供其他需要指针参数的 Qt API 使用
template <typename T,
          typename = std::enable_if_t<!std::is_same_v<T, std::string> &&
                                      !std::is_same_v<T, E2CppByteArray> &&
                                      !std::is_same_v<T, std::int32_t>>>
inline T* 取变量数据地址(T &变量) {
  return &变量;
}

/**
 * @brief 取颜色值 (RGB)
 */
inline std::int32_t 取颜色值(std::int32_t 红色值, std::int32_t 绿色值,
                             std::int32_t 蓝色值) {
  return (红色值 & 0xFF) | ((绿色值 & 0xFF) << 8) | ((蓝色值 & 0xFF) << 16);
}

inline std::int32_t 取红(std::int32_t 颜色值) { return 颜色值 & 0xFF; }
inline std::int32_t 取绿(std::int32_t 颜色值) { return (颜色值 >> 8) & 0xFF; }
inline std::int32_t 取蓝(std::int32_t 颜色值) { return (颜色值 >> 16) & 0xFF; }

/**
 * @brief 是否为双字节字符
 */
inline bool 是否为双字节字符(const std::string &欲检查的文本,
                             std::int32_t 待检查字符的位置 = 1) {
  if (待检查字符的位置 < 1 ||
      static_cast<std::size_t>(待检查字符的位置) > 欲检查的文本.length())
    return false;
  unsigned char c =
      static_cast<unsigned char>(欲检查的文本[待检查字符的位置 - 1]);
  return c > 127;
}

/**
 * @brief 文本指针到文本
 */
inline std::string 文本指针到文本(std::intptr_t 文本指针) {
  if (文本指针 == 0)
    return "";
  return std::string(reinterpret_cast<const char *>(文本指针));
}

#ifdef QT_CORE_LIB
inline QString 文本指针到文本Q(std::intptr_t 文本指针) {
  if (文本指针 == 0) {
    return QString();
  }
  return QString::fromUtf8(reinterpret_cast<const char *>(文本指针));
}

inline QString 指针到文本Q(std::intptr_t 文本指针) {
  return 文本指针到文本Q(文本指针);
}
#endif

/**
 * @brief 指针是否有效
 */
inline bool 指针是否有效(void *欲测试的指针) { return 欲测试的指针 != nullptr; }

/**
 * @brief 处理事件
 */
inline void 处理事件() {
#ifdef QT_CORE_LIB
  QCoreApplication::processEvents(QEventLoop::AllEvents);
#elif defined(_WIN32)
  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
#endif
}

/**
 * @brief 载入图片
 */
inline std::int32_t 载入图片(const E2CppByteArray &欲被载入的图片) {
  const std::int32_t 图片号 = g_NextPicId++;
  g_LoadedPics[图片号] = 欲被载入的图片;
  return 图片号;
}

inline std::int32_t 载入图片(const std::string &欲被载入的图片) {
  const E2CppByteArray 数据 = 读入文件(欲被载入的图片);
  if (数据.size() == 0) {
    return 0;
  }
  return 载入图片(数据);
}

#ifdef QT_CORE_LIB
inline std::int32_t 载入图片(const QString &欲被载入的图片) {
  return 载入图片(E2ToStdString(欲被载入的图片));
}
#endif

template <typename T> inline std::int32_t 载入图片(const T &欲被载入的图片) {
  (void)欲被载入的图片;
  static_assert(E2CppAlwaysFalseOther<T>::value,
                "E2Cpp: 命令“载入图片”当前不支持，请在源代码中移除此命令。");
  return 0;
}

/**
 * @brief 卸载图片
 */
inline void 卸载图片(std::int32_t 图片号) { g_LoadedPics.erase(图片号); }

/**
 * @brief 取硬盘特征字
 */
inline std::int32_t 取硬盘特征字() {
#ifdef _WIN32
  DWORD serial = 0;
  if (GetVolumeInformationA("C:\\", nullptr, 0, &serial, nullptr, nullptr,
                            nullptr, 0)) {
    return static_cast<std::int32_t>(serial);
  }
  return 0;
#else
  const char *host = std::getenv("HOSTNAME");
  if (!host)
    host = std::getenv("HOST");
  if (!host)
    return 0;
  std::size_t h = std::hash<std::string>{}(host);
  return static_cast<std::int32_t>(h & 0x7fffffff);
#endif
}

/**
 * @brief 取系统语言
 * @return 1=GBK中文,2=英文,3=BIG5中文,4=日文
 */
inline std::int32_t 取系统语言() {
#ifdef QT_CORE_LIB
  const QLocale locale = QLocale::system();
  const QLocale::Language lang = locale.language();
  const QLocale::Script script = locale.script();
  if (lang == QLocale::Chinese) {
    if (script == QLocale::TraditionalChineseScript)
      return 3; // BIG5 繁体
    return 1;   // GBK 简体
  }
  if (lang == QLocale::Japanese)
    return 4;
  return 2;
#elif defined(_WIN32)
  LANGID lid = GetUserDefaultUILanguage();
  WORD prim = PRIMARYLANGID(lid);
  WORD sub = SUBLANGID(lid);
  if (prim == LANG_CHINESE) {
    if (sub == SUBLANG_CHINESE_TRADITIONAL || sub == SUBLANG_CHINESE_HONGKONG ||
        sub == SUBLANG_CHINESE_MACAU)
      return 3;
    return 1;
  }
  if (prim == LANG_JAPANESE)
    return 4;
  return 2;
#else
  const char *lang = std::getenv("LANG");
  if (!lang)
    return 2;
  std::string s = 大写化(lang);
  if (s.find("ZH_TW") != std::string::npos || s.find("HANT") != std::string::npos)
    return 3;
  if (s.find("ZH") != std::string::npos)
    return 1;
  if (s.find("JA") != std::string::npos)
    return 4;
  return 2;
#endif
}

/**
 * @brief 写到内存
 */
inline void 写到内存(const E2CppValue &欲写到内存的数据,
                     std::intptr_t 内存区域指针,
                     std::int32_t 内存区域尺寸 = -1) {
  std::visit(
      overloaded{[&](std::monostate) {},
                 [&](std::int32_t v) {
                   安全写内存(&v, sizeof(v), 内存区域指针, 内存区域尺寸);
                 },
                 [&](std::int64_t v) {
                   安全写内存(&v, sizeof(v), 内存区域指针, 内存区域尺寸);
                 },
                 [&](float v) {
                   安全写内存(&v, sizeof(v), 内存区域指针, 内存区域尺寸);
                 },
                 [&](double v) {
                   安全写内存(&v, sizeof(v), 内存区域指针, 内存区域尺寸);
                 },
                 [&](const QDateTime &v) {
                   auto sec = std::chrono::duration_cast<std::chrono::seconds>(
                                  std::chrono::milliseconds(v.toMSecsSinceEpoch()))
                                  .count();
                   安全写内存(&sec, sizeof(sec), 内存区域指针, 内存区域尺寸);
                 },
                 [&](bool v) {
                   安全写内存(&v, sizeof(v), 内存区域指针, 内存区域尺寸);
                 },
                 [&](const std::string &v) {
                   std::size_t len = v.size() + 1; // 包含 \0
                   安全写内存(v.c_str(), len, 内存区域指针, 内存区域尺寸);
                 },
                 [&](const E2CppByteArray &v) {
                   安全写内存(v.data(), v.size(), 内存区域指针, 内存区域尺寸);
                 },
                 [&](void *v) {
                   auto p = reinterpret_cast<std::uintptr_t>(v);
                   安全写内存(&p, sizeof(p), 内存区域指针, 内存区域尺寸);
                 }},
      欲写到内存的数据.value);
}

/**
 * @brief DLL命令调用转向
 */
inline bool DLL命令调用转向(const std::string &DLL库文件名 = "",
                            const std::string &调用命令名 = "") {
  if (DLL库文件名.empty()) {
    g_DllRedirectPath.clear();
    g_DllRedirectCmd.clear();
    return true;
  }

  if (!fs::exists(DLL库文件名))
    return false;

  g_DllRedirectPath = DLL库文件名;
  g_DllRedirectCmd = 调用命令名;
  return true;
}

#ifdef QT_CORE_LIB
inline bool DLL命令调用转向(const QString &DLL库文件名 = QString(),
                            const QString &调用命令名 = QString()) {
  return DLL命令调用转向(E2ToStdString(DLL库文件名),
                      E2ToStdString(调用命令名));
}
#endif

/**
 * @brief 置错误提示管理
 */
inline void 置错误提示管理(void *用作进行错误提示的子程序 = nullptr) {
  g_ErrorManager = 用作进行错误提示的子程序;
}

/**
 * @brief 置DLL装载目录
 */
inline std::string 置DLL装载目录(const std::string &DLL装载目录 = "") {
  std::string old = g_DllLoadPath;
  g_DllLoadPath = DLL装载目录;
  return old;
}

#ifdef QT_CORE_LIB
inline QString 置DLL装载目录Q(const QString &DLL装载目录 = QString()) {
  return QString::fromStdString(置DLL装载目录(E2ToStdString(DLL装载目录)));
}
#endif

/**
 * @brief 置入代码
 */
inline void 置入代码(const E2CppValue &代码数据) {
  (void)代码数据;
  throw std::runtime_error(
      "E2Cpp: MachineCode must be handled during translation. "
      "Only Windows 32-bit MSVC (x86) __asm _emit is supported.");
}

/**
 * @brief 运行控制台程序
 */
inline bool 运行控制台程序(const std::string &欲运行的命令行,
                           std::string &标准输出内容存放变量,
                           std::string &错误输出内容存放变量,
                           std::int32_t &返回值存放变量) {
  return 运行控制台程序_内部(欲运行的命令行, &标准输出内容存放变量,
                             &错误输出内容存放变量, &返回值存放变量);
}

inline bool 运行控制台程序(const std::string &欲运行的命令行,
                           std::string &标准输出内容存放变量,
                           std::string &错误输出内容存放变量) {
  std::int32_t rc = -1;
  return 运行控制台程序_内部(欲运行的命令行, &标准输出内容存放变量,
                             &错误输出内容存放变量, &rc);
}

inline bool 运行控制台程序(const std::string &欲运行的命令行,
                           std::string &标准输出内容存放变量) {
  std::string err;
  std::int32_t rc = -1;
  return 运行控制台程序_内部(欲运行的命令行, &标准输出内容存放变量, &err, &rc);
}

inline bool 运行控制台程序(const std::string &欲运行的命令行) {
  std::string out;
  std::string err;
  std::int32_t rc = -1;
  return 运行控制台程序_内部(欲运行的命令行, &out, &err, &rc);
}

#ifdef QT_CORE_LIB
inline bool 运行控制台程序(const QString &欲运行的命令行,
                           QString &标准输出内容存放变量,
                           QString &错误输出内容存放变量,
                           std::int32_t &返回值存放变量) {
  std::string out;
  std::string err;
  const bool ok =
      运行控制台程序_内部(欲运行的命令行.toStdString(), &out, &err, &返回值存放变量);
  标准输出内容存放变量 = QString::fromStdString(out);
  错误输出内容存放变量 = QString::fromStdString(err);
  return ok;
}

inline bool 运行控制台程序(const QString &欲运行的命令行,
                           QString &标准输出内容存放变量,
                           QString &错误输出内容存放变量) {
  std::int32_t rc = -1;
  return 运行控制台程序(欲运行的命令行, 标准输出内容存放变量,
                        错误输出内容存放变量, rc);
}

inline bool 运行控制台程序(const QString &欲运行的命令行,
                           QString &标准输出内容存放变量) {
  QString err;
  std::int32_t rc = -1;
  return 运行控制台程序(欲运行的命令行, 标准输出内容存放变量, err, rc);
}

inline bool 运行控制台程序(const QString &欲运行的命令行) {
  QString out;
  QString err;
  std::int32_t rc = -1;
  return 运行控制台程序(欲运行的命令行, out, err, rc);
}
#endif

/**
 * @brief 条件宏是否存在
 */
template <typename T> inline bool 条件宏是否存在(const T &所欲检测的条件宏名) {
  (void)所欲检测的条件宏名;
  static_assert(
      E2CppAlwaysFalseOther<T>::value,
      "E2Cpp: 命令“条件宏是否存在”当前不支持，请在源代码中移除此命令。");
  return false;
}
