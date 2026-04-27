#include "E2CppBase.h"

#include <utility>

// Category 05: Variable Operations

template <typename T, typename U>
inline void 赋值(T &target, const U &source) {
  target = source;
}

template <typename V, typename... Args>
inline void 连续赋值(const V &val, Args &&...targets) {
  ((targets = val), ...);
}

inline void 连续赋值(const E2CppLiteralArray &val, E2CppByteArray &a) {
  a = static_cast<E2CppByteArray>(val);
}

template <typename... Args>
inline void 连续赋值(const E2CppLiteralArray &val, E2CppByteArray &a,
                 Args &&...targets) {
  a = static_cast<E2CppByteArray>(val);
  连续赋值(val, std::forward<Args>(targets)...);
}

template <typename... Args>
inline void 连续赋值(std::initializer_list<std::int32_t> list,
                 Args &&...targets) {
  连续赋值(E2CppLiteralArray(list), std::forward<Args>(targets)...);
}
