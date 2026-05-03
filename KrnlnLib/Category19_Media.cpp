#pragma once
#include "E2CppBase.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define MA_API static
#define MINIAUDIO_IMPLEMENTATION
#if defined(__has_include)
#if __has_include("../3rd/miniaudio/miniaudio.h")
#include "../3rd/miniaudio/miniaudio.h"
#elif __has_include("../../3rd/miniaudio/miniaudio.h")
#include "../../3rd/miniaudio/miniaudio.h"
#elif __has_include("miniaudio/miniaudio.h")
#include "miniaudio/miniaudio.h"
#elif __has_include("miniaudio.h")
#include "miniaudio.h"
#else
#error "miniaudio.h not found"
#endif
#else
#include "../3rd/miniaudio/miniaudio.h"
#endif

#include <atomic>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace {
struct E2MediaRuntime {
  std::mutex mutex;
  ma_engine engine{};
  bool engineReady = false;
  ma_sound sound{};
  bool soundReady = false;
  std::wstring tempFile;

  std::thread playlistThread;
  std::atomic<int> mp3State{0}; // 0 stop, -1 pause, 1 play
  std::atomic<bool> stopRequested{false};
  std::atomic<bool> pauseRequested{false};
  std::wstring mciAlias = L"e2cpp_mp3";
};

inline E2MediaRuntime &E2Media() {
  static E2MediaRuntime rt;
  return rt;
}

inline bool E2MediaEnsureEngine() {
  auto &rt = E2Media();
  std::lock_guard<std::mutex> lock(rt.mutex);
  if (rt.engineReady) {
    return true;
  }
  if (ma_engine_init(nullptr, &rt.engine) != MA_SUCCESS) {
    return false;
  }
  rt.engineReady = true;
  return true;
}

inline void E2MediaStopMiniaudioUnlocked(E2MediaRuntime &rt) {
  if (rt.soundReady) {
    ma_sound_stop(&rt.sound);
    ma_sound_uninit(&rt.sound);
    rt.soundReady = false;
  }
  if (!rt.tempFile.empty()) {
    std::error_code ec;
    std::filesystem::remove(rt.tempFile, ec);
    rt.tempFile.clear();
  }
}

inline void E2MediaCloseMciAlias(const std::wstring &alias) {
  if (alias.empty()) {
    return;
  }
  std::wstring cmd = L"close " + alias;
  ::mciSendStringW(cmd.c_str(), nullptr, 0, nullptr);
}

inline std::wstring E2ToWide(const std::string &text) {
  if (text.empty()) {
    return std::wstring();
  }
  int len = ::MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
  if (len <= 1) {
    len = ::MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, nullptr, 0);
    if (len <= 1) {
      return std::wstring(text.begin(), text.end());
    }
    std::wstring out(static_cast<size_t>(len - 1), L'\0');
    ::MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, out.data(), len);
    return out;
  }
  std::wstring out(static_cast<size_t>(len - 1), L'\0');
  ::MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, out.data(), len);
  return out;
}

inline std::string E2ToLowerAscii(std::string s) {
  for (char &ch : s) {
    if (ch >= 'A' && ch <= 'Z') {
      ch = static_cast<char>(ch - 'A' + 'a');
    }
  }
  return s;
}

inline bool E2IsMidiBytes(const E2CppByteArray &bytes) {
  return bytes.m_data.size() >= 4 && bytes.m_data[0] == 'M' &&
         bytes.m_data[1] == 'T' && bytes.m_data[2] == 'h' &&
         bytes.m_data[3] == 'd';
}

inline std::wstring E2WriteTempAudioFile(const E2CppByteArray &bytes,
                                         const wchar_t *suffix) {
  wchar_t dir[MAX_PATH]{};
  wchar_t file[MAX_PATH]{};
  ::GetTempPathW(MAX_PATH, dir);
  ::GetTempFileNameW(dir, L"e2m", 0, file);
  std::wstring path = file;
  if (suffix != nullptr && *suffix != L'\0') {
    path += suffix;
  }
  std::ofstream out(path, std::ios::binary);
  if (!out) {
    return std::wstring();
  }
  if (!bytes.m_data.empty()) {
    out.write(reinterpret_cast<const char *>(bytes.m_data.data()),
              static_cast<std::streamsize>(bytes.m_data.size()));
  }
  out.close();
  return path;
}

inline bool E2PlayMciFile(const std::wstring &alias, const std::wstring &path,
                          bool loop) {
  E2MediaCloseMciAlias(alias);
  std::wstring openCmd =
      L"open \"" + path + L"\" alias " + alias;
  if (::mciSendStringW(openCmd.c_str(), nullptr, 0, nullptr) != 0) {
    return false;
  }
  std::wstring playCmd = L"play " + alias;
  if (loop) {
    playCmd += L" repeat";
  }
  return ::mciSendStringW(playCmd.c_str(), nullptr, 0, nullptr) == 0;
}

inline bool E2PlayWithMiniaudio(const std::wstring &path, bool loop) {
  if (!E2MediaEnsureEngine()) {
    return false;
  }
  auto &rt = E2Media();
  std::lock_guard<std::mutex> lock(rt.mutex);
  E2MediaStopMiniaudioUnlocked(rt);
  const ma_result result =
      ma_sound_init_from_file_w(&rt.engine, path.c_str(), 0, nullptr, nullptr,
                                &rt.sound);
  if (result != MA_SUCCESS) {
    return false;
  }
  rt.soundReady = true;
  ma_sound_set_looping(&rt.sound, loop ? MA_TRUE : MA_FALSE);
  return ma_sound_start(&rt.sound) == MA_SUCCESS;
}

inline bool E2PlayMusicBytes(const E2CppByteArray &bytes, bool loop) {
  if (bytes.m_data.empty()) {
    return false;
  }
  if (E2IsMidiBytes(bytes)) {
    const std::wstring path = E2WriteTempAudioFile(bytes, L".mid");
    if (path.empty()) {
      return false;
    }
    E2Media().tempFile = path;
    return E2PlayMciFile(L"e2cpp_music", path, loop);
  }
  const std::wstring path = E2WriteTempAudioFile(bytes, L".bin");
  if (path.empty()) {
    return false;
  }
  E2Media().tempFile = path;
  return E2PlayWithMiniaudio(path, loop);
}

inline bool E2PlayMusicPath(const std::string &pathText, bool loop) {
  const std::wstring path = E2ToWide(pathText);
  const std::string lower = E2ToLowerAscii(pathText);
  if (lower.ends_with(".mid") || lower.ends_with(".midi")) {
    return E2PlayMciFile(L"e2cpp_music", path, loop);
  }
  return E2PlayWithMiniaudio(path, loop);
}

inline void E2StopPlaylistThread() {
  auto &rt = E2Media();
  rt.stopRequested = true;
  rt.pauseRequested = false;
  if (rt.playlistThread.joinable()) {
    rt.playlistThread.join();
  }
  rt.stopRequested = false;
  rt.mp3State = 0;
}

template <typename T>
inline void E2AppendMp3Arg(std::vector<std::string> &out, const T &) {}

inline void E2AppendMp3Arg(std::vector<std::string> &out, const std::string &v) {
  if (!v.empty()) {
    out.push_back(v);
  }
}

inline void E2AppendMp3Arg(std::vector<std::string> &out,
                           const E2CppArray<std::string> &arr) {
  for (const auto &v : arr.data) {
    if (!v.empty()) {
      out.push_back(v);
    }
  }
}

template <typename... Args>
inline std::vector<std::string> E2CollectMp3Files(const Args &...args) {
  std::vector<std::string> files;
  (E2AppendMp3Arg(files, args), ...);
  return files;
}

inline bool E2MciOpenAndPlayMp3(const std::wstring &alias,
                                const std::wstring &path,
                                int startPercent) {
  E2MediaCloseMciAlias(alias);
  std::wstring openCmd = L"open \"" + path + L"\" type mpegvideo alias " + alias;
  if (::mciSendStringW(openCmd.c_str(), nullptr, 0, nullptr) != 0) {
    return false;
  }
  if (startPercent > 0) {
    wchar_t lenBuf[64]{};
    std::wstring lenCmd = L"status " + alias + L" length";
    if (::mciSendStringW(lenCmd.c_str(), lenBuf, 64, nullptr) == 0) {
      const long totalMs = _wtol(lenBuf);
      const long startMs = (totalMs > 0) ? (totalMs * startPercent / 100) : 0;
      std::wstring seekCmd =
          L"seek " + alias + L" to " + std::to_wstring(startMs);
      ::mciSendStringW(seekCmd.c_str(), nullptr, 0, nullptr);
    }
  }
  std::wstring playCmd = L"play " + alias;
  return ::mciSendStringW(playCmd.c_str(), nullptr, 0, nullptr) == 0;
}
} // namespace

// --- 命令类别：媒体播放 ---

inline void 停止播放();

// 播放 .WAV/.MP3 文件或字节集。MIDI 走 MCI，其它音频优先走 miniaudio。
inline bool 播放音乐(const E2CppValue &欲播放的音乐,
                    bool 是否循环播放 = false) {
  停止播放();
  return std::visit(
      overloaded{
          [是否循环播放](const std::string &path) {
            return E2PlayMusicPath(path, 是否循环播放);
          },
          [是否循环播放](const E2CppByteArray &bytes) {
            return E2PlayMusicBytes(bytes, 是否循环播放);
          },
          [是否循环播放](const E2CppArray<std::uint8_t> &bytes) {
            return E2PlayMusicBytes(
                static_cast<E2CppByteArray>(到字节集(bytes)), 是否循环播放);
          },
          [](const auto &) { return false; },
      },
      欲播放的音乐.value);
}

// 停止所有由本运行库启动的音频播放。
inline void 停止播放() {
  auto &rt = E2Media();
  E2StopPlaylistThread();
  {
    std::lock_guard<std::mutex> lock(rt.mutex);
    E2MediaStopMiniaudioUnlocked(rt);
  }
  E2MediaCloseMciAlias(L"e2cpp_music");
  E2MediaCloseMciAlias(rt.mciAlias);
}

template <typename... Args>
inline void 播放MP3(std::int32_t 播放次数, const Args &...args) {
  auto &rt = E2Media();
  E2StopPlaylistThread();
  const auto files = E2CollectMp3Files(args...);
  if (files.empty()) {
    return;
  }
  rt.stopRequested = false;
  rt.pauseRequested = false;
  rt.mp3State = 1;
  rt.playlistThread = std::thread([files, 播放次数]() {
    auto &rt2 = E2Media();
    const bool loopForever = (播放次数 == -1);
    int remaining = 播放次数;
    while (!rt2.stopRequested && (loopForever || remaining > 0)) {
      for (const auto &file : files) {
        if (rt2.stopRequested) {
          break;
        }
        if (!E2MciOpenAndPlayMp3(rt2.mciAlias, E2ToWide(file), 0)) {
          continue;
        }
        while (!rt2.stopRequested) {
          if (rt2.pauseRequested) {
            ::mciSendStringW((L"pause " + rt2.mciAlias).c_str(), nullptr, 0,
                             nullptr);
            rt2.mp3State = -1;
            while (rt2.pauseRequested && !rt2.stopRequested) {
              std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            if (!rt2.stopRequested) {
              ::mciSendStringW((L"resume " + rt2.mciAlias).c_str(), nullptr, 0,
                               nullptr);
              rt2.mp3State = 1;
            }
          }
          wchar_t modeBuf[64]{};
          if (::mciSendStringW((L"status " + rt2.mciAlias + L" mode").c_str(),
                               modeBuf, 64, nullptr) != 0) {
            break;
          }
          if (std::wstring(modeBuf) == L"stopped") {
            break;
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        E2MediaCloseMciAlias(rt2.mciAlias);
      }
      if (!loopForever) {
        --remaining;
      }
    }
    rt2.mp3State = 0;
  });
}

template <typename... Args>
inline void 播放MP3(const Args &...args) {
  播放MP3(1, args...);
}

// 同步播放单个 MP3。反馈标签当前不处理，保留参数忽略。
inline void 同步播放MP3(std::string 欲播放的MP3文件名,
                        std::int32_t 起始播放位置 = 0,
                        std::int32_t 欲通知的反馈标签 = 0,
                        std::string 保留参数 = std::string()) {
  (void)欲通知的反馈标签;
  (void)保留参数;
  auto &rt = E2Media();
  E2StopPlaylistThread();
  if (!E2MciOpenAndPlayMp3(rt.mciAlias, E2ToWide(欲播放的MP3文件名),
                           (std::max)(0, (std::min)(100, 起始播放位置)))) {
    rt.mp3State = 0;
    return;
  }
  rt.mp3State = 1;
  while (true) {
    wchar_t modeBuf[64]{};
    if (::mciSendStringW((L"status " + rt.mciAlias + L" mode").c_str(), modeBuf,
                         64, nullptr) != 0) {
      break;
    }
    if (std::wstring(modeBuf) == L"stopped") {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  E2MediaCloseMciAlias(rt.mciAlias);
  rt.mp3State = 0;
}

inline void 暂停播放MP3() {
  auto &rt = E2Media();
  if (rt.mp3State == 1) {
    rt.pauseRequested = true;
  }
}

inline void 继续播放MP3() {
  auto &rt = E2Media();
  if (rt.mp3State == -1) {
    rt.pauseRequested = false;
  }
}

inline std::int32_t 取MP3播放状态() { return E2Media().mp3State.load(); }

#else

inline bool 播放音乐(const E2CppValue &, bool = false) { return false; }
inline void 停止播放() {}
template <typename... Args> inline void 播放MP3(Args &&...) {}
inline void 同步播放MP3(std::string, std::int32_t = 0, std::int32_t = 0,
                        std::string = std::string()) {}
inline void 暂停播放MP3() {}
inline void 继续播放MP3() {}
inline std::int32_t 取MP3播放状态() { return 0; }

#endif
