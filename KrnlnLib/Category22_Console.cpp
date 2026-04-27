#include "E2CppBase.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

// cmdCategory: 22
// categoryCn: 控制台操作

namespace {

inline std::string 控制台值到文本(const E2CppValue &v) {
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
            std::snprintf(buf, sizeof(buf), "%p", p);
            return buf;
          }},
      v.value);
}

template <typename T> inline std::string 控制台值到文本(const T &v) {
  return 控制台值到文本(E2CppValue(v));
}

inline void 标准输出到流(std::ostream &os) {}

template <typename T, typename... Rest>
inline void 标准输出到流(std::ostream &os, const T &first,
                    const Rest &...rest) {
  os << 控制台值到文本(first);
  if constexpr (sizeof...(rest) > 0) {
    标准输出到流(os, rest...);
  }
}

} // namespace

/**
 * @brief 标准输出
 */
template <typename... Args>
inline void 标准输出(std::int32_t 输出方向, const Args &...欲输出内容) {
  std::ostream &os = (输出方向 == 2) ? static_cast<std::ostream &>(std::cerr)
                                     : static_cast<std::ostream &>(std::cout);
  标准输出到流(os, 欲输出内容...);
  os.flush();
}

template <typename... Args>
inline void 标准输出(double 输出方向, const Args &...欲输出内容) {
  标准输出(static_cast<std::int32_t>(输出方向), 欲输出内容...);
}

template <typename... Args> inline void 标准输出(const Args &...欲输出内容) {
  标准输出(1, 欲输出内容...);
}

/**
 * @brief 标准输入
 */
inline std::string 标准输入(bool 是否回显 = true) {
  constexpr std::size_t kMaxChars = 2048;
  std::string line;

  if (是否回显) {
    std::getline(std::cin, line);
    if (line.size() > kMaxChars) {
      line.resize(kMaxChars);
    }
    return line;
  }

#ifdef _WIN32
  line.reserve(64);
  while (true) {
    const int ch = _getch();
    if (ch == '\r' || ch == '\n') {
      std::cout << std::endl;
      break;
    }
    if (ch == '\b' || ch == 127) {
      if (!line.empty()) {
        line.pop_back();
      }
      continue;
    }
    if (ch >= 0 && ch < 256 && line.size() < kMaxChars) {
      line.push_back(static_cast<char>(ch));
    }
  }
  return line;
#else
  termios oldt{};
  termios newt{};
  if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
    std::getline(std::cin, line);
    if (line.size() > kMaxChars) {
      line.resize(kMaxChars);
    }
    return line;
  }
  newt = oldt;
  newt.c_lflag &= static_cast<unsigned long>(~ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  std::getline(std::cin, line);
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  std::cout << std::endl;

  if (line.size() > kMaxChars) {
    line.resize(kMaxChars);
  }
  return line;
#endif
}
