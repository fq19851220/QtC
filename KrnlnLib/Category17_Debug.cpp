#include "E2CppBase.h"
#include <cassert>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#endif
#ifdef QT_CORE_LIB
#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#endif

// cmdCategory: 17
// categoryCn: 程序调试

inline void E2WriteStdout(const char *data, int len) {
#ifdef _WIN32
  HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
  if (h && h != INVALID_HANDLE_VALUE) {
    DWORD written = 0;
    WriteFile(h, data, (DWORD)len, &written, nullptr);
  }
#else
  (void)::write(1, data, (size_t)len);
#endif
}

#ifdef QT_CORE_LIB
#include <type_traits>

template <typename T>
inline auto E2QtDebugDisplayValue(const T &item) {
  if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T> ||
                std::is_pointer_v<T>) {
    return item;
  } else {
    return QString::fromStdString(E2CppDebugToString(item));
  }
}

inline const QString &E2QtDebugDisplayValue(const QString &item) {
  return item;
}

inline QString E2QtDebugDisplayValue(const std::string &item) {
  return QString::fromStdString(item);
}

inline const QByteArray &E2QtDebugDisplayValue(const QByteArray &item) {
  return item;
}

inline const QDateTime &E2QtDebugDisplayValue(const QDateTime &item) {
  return item;
}

inline QString E2QtDebugDisplayValue(const E2CppDateTime &item) {
  return 时间到文本Q(item);
}

inline QByteArray E2QtDebugDisplayValue(const E2CppByteArray &item) {
  return QByteArray(reinterpret_cast<const char *>(item.data()),
                    static_cast<qsizetype>(item.size()));
}
#endif

template <typename... Args>
inline void 调试输出(const Args &...args) {
#ifdef QT_CORE_LIB
  QDebug dbg = qDebug().noquote().nospace();
  bool first = true;
  auto emitOne = [&](const auto &item) {
    if (!first) {
      dbg << ' ';
    }
    dbg << E2QtDebugDisplayValue(item);
    first = false;
  };
  (emitOne(args), ...);
#else
  std::string buf;
  bool first = true;
  auto emitOne = [&](const auto &item) {
    if (!first) {
      buf += " | ";
    }
    buf += E2CppDebugToString(item);
    first = false;
  };
  (emitOne(args), ...);
  buf += '\n';
  E2WriteStdout(buf.data(), (int)buf.size());
#endif
}

template <typename... Args>
inline void 输出调试文本(const Args &...args) {
  调试输出(args...);
}

inline void 暂停() {
#ifdef _WIN32
  if (IsDebuggerPresent()) {
    DebugBreak();
    return;
  }
#endif
  std::cout << "[PAUSE] Program paused. Press Enter to continue..." << std::endl;
  std::cin.get();
}

inline void 检查(bool 被校验的条件) {
  if (!被校验的条件) {
    std::cerr << "[ASSERT FAILED] Condition is false." << std::endl;
    暂停();
  }
}

inline bool 是否为调试版() {
#ifdef _DEBUG
  return true;
#else
  return false;
#endif
}
