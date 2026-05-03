#include "E2CppBase.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

inline std::string E2CppDebugToString(const std::string &v);
inline std::string E2CppDebugToString(const QString &v);
inline std::string E2CppDebugToString(const char *v);
inline std::string E2CppDebugToString(bool v);
inline std::string E2CppDebugToString(const E2CppDateTime &v);
inline std::string E2CppDebugToString(const QDateTime &v);
inline std::string E2CppDebugToString(const E2CppByteArray &v);
inline std::string E2CppDebugToString(const QByteArray &v);
template <typename T> std::string E2CppDebugToString(const T &v);
template <typename T> std::string E2CppDebugToString(const std::vector<T> &v);
template <typename T> std::string E2CppDebugToString(const E2CppArray<T> &v);
template <typename T> std::string E2CppDebugToString(const EQtArray<T> &v);

inline std::string E2CppDebugToString(const std::string &v) {
  std::string res;
  res.reserve(v.size() + 2);
  res += '"';
  res += v;
  res += '"';
  return res;
}

inline std::string E2CppDebugToString(const QString &v) {
  return E2CppDebugToString(E2ToStdString(v));
}

inline std::string E2CppDebugToString(const char *v) {
  if (!v) {
    return "\"\"";
  }
  std::string_view sv(v);
  std::string res;
  res.reserve(sv.size() + 2);
  res += '"';
  res += sv;
  res += '"';
  return res;
}

inline std::string E2CppDebugToString(bool v) { return v ? "真" : "假"; }

inline std::string E2CppDebugToString(const E2CppDateTime &v) {
  const std::tm tm = v.to_tm();
  char buf[64];
  const int len =
      std::snprintf(buf, sizeof(buf), "[%04d年%02d月%02d日 %02d时%02d分%02d秒]",
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
                    tm.tm_min, tm.tm_sec);
  return std::string(buf, static_cast<std::size_t>(len));
}

inline std::string E2CppDebugToString(const QDateTime &v) {
  return E2CppDebugToString(E2ToStdString(v.isValid()
                                              ? v.toString(QStringLiteral("yyyy年M月d日 H时m分s秒"))
                                              : QString()));
}

inline std::string E2CppDebugToString(const E2CppByteArray &v) {
  std::string res;
  res.reserve(16 + v.m_data.size() * 4);
  res += "字节集:";
  res += std::to_string(v.m_data.size());
  res += "{";
  for (std::size_t i = 0; i < v.m_data.size(); ++i) {
    if (i != 0) {
      res += ",";
    }
    res += std::to_string(static_cast<unsigned int>(v.m_data[i]));
  }
  res += "}";
  return res;
}

inline std::string E2CppDebugToString(const QByteArray &v) {
  return E2CppDebugToString(E2CppByteArray(
      reinterpret_cast<const std::uint8_t *>(v.constData()),
      reinterpret_cast<const std::uint8_t *>(v.constData()) + v.size()));
}

template <typename T> std::string E2CppDebugToString(const std::vector<T> &v) {
  std::string res;
  res += "数组:";
  res += std::to_string(v.size());
  res += "{";
  for (std::size_t i = 0; i < v.size(); ++i) {
    if (i != 0) {
      res += ",";
    }
    res += E2CppDebugToString(v[i]);
  }
  res += "}";
  return res;
}

template <typename T> std::string E2CppDebugToString(const E2CppArray<T> &v) {
  return E2CppDebugToString(v.data);
}

template <typename T> std::string E2CppDebugToString(const EQtArray<T> &v) {
  std::string res = "数组:";
  if (v.dims.isEmpty()) {
    res += "0";
  } else {
    for (qsizetype i = 0; i < v.dims.size(); ++i) {
      if (i != 0) {
        res += "*";
      }
      res += std::to_string(v.dims[i]);
    }
  }
  res += "{";
  for (qsizetype i = 0; i < v.data.size(); ++i) {
    if (i != 0) {
      res += ",";
    }
    res += E2CppDebugToString(v.data[i]);
  }
  res += "}";
  return res;
}

template <typename T> std::string E2CppDebugToString(const T &v) {
  if constexpr (std::is_same_v<T, E2CppValue>) {
    return std::visit(
        [](auto &&arg) -> std::string {
          using TI = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<TI, std::monostate>) {
            return "";
          } else {
            return E2CppDebugToString(arg);
          }
        },
        v.value);
  } else if constexpr (is_e2cpp_accessor<T>::value) {
    return E2CppDebugToString(static_cast<typename T::value_type>(v));
  } else if constexpr (std::is_same_v<T, E2CppByteArray>) {
    return E2CppDebugToString(v);
  } else if constexpr (std::is_same_v<T, QByteArray>) {
    return E2CppDebugToString(v);
  } else if constexpr (std::is_same_v<T, E2CppDateTime>) {
    return E2CppDebugToString(v);
  } else if constexpr (std::is_same_v<T, QDateTime>) {
    return E2CppDebugToString(v);
  } else if constexpr (std::is_same_v<T, QString>) {
    return E2CppDebugToString(v);
  } else if constexpr (is_e2cpp_array<T>::value) {
    return E2CppDebugToString(v);
  } else if constexpr (std::is_pointer_v<T>) {
    char buf[32];
    const int len = std::snprintf(buf, sizeof(buf), "%p", (void *)v);
    return std::string(buf, static_cast<std::size_t>(len));
  } else if constexpr (std::is_arithmetic_v<T>) {
    if constexpr (std::is_floating_point_v<T>) {
      char buf[64];
      const int len = std::snprintf(buf, sizeof(buf), "%g", (double)v);
      return std::string(buf, static_cast<std::size_t>(len));
    } else {
      return std::to_string(v);
    }
  } else {
    return "[Unknown Type]";
  }
}
