#include "E2CppBase.h"

// cmdCategory: 4
// categoryCn: 位运算

/**
 * @brief 位取反
 */
inline std::int32_t 位取反(std::int32_t 欲取其反的代码) {
  return ~欲取其反的代码;
}

/**
 * @brief 位与
 */
template <typename T>
inline auto 位与(T value) {
  return value;
}

template <typename T, typename U, typename... Args>
inline auto 位与(T first, U second, Args... args) {
  auto res = (first & second);
  ((res = (res & args)), ...);
  return res;
}

/**
 * @brief 位或
 */
template <typename T>
inline auto 位或(T value) {
  return value;
}

template <typename T, typename U, typename... Args>
inline auto 位或(T first, U second, Args... args) {
  auto res = (first | second);
  ((res = (res | args)), ...);
  return res;
}

/**
 * @brief 位异或
 */
template <typename T, typename U>
inline auto 位异或(T 位型一, U 位型二) {
  return 位型一 ^ 位型二;
}

/**
 * @brief 左移
 */
inline std::int32_t 左移(std::int32_t 欲移动的整数, std::int32_t 欲移动的位数) {
  return 欲移动的整数 << 欲移动的位数;
}

/**
 * @brief 右移
 */
inline std::int32_t 右移(std::int32_t 欲移动的整数, std::int32_t 欲移动的位数) {
  return 欲移动的整数 >> 欲移动的位数;
}

/**
 * @brief 合并短整数 (MakeWord)
 */
inline std::int32_t 合并短整数(std::uint8_t 低8位, std::uint8_t 高8位) {
  return static_cast<std::int16_t>((static_cast<std::uint16_t>(高8位) << 8) |
                                   低8位);
}

/**
 * @brief 合并整数 (MakeLong)
 */
inline std::int32_t 合并整数(std::int16_t 低16位, std::int16_t 高16位) {
  return static_cast<std::int32_t>((static_cast<std::uint32_t>(高16位) << 16) |
                                   static_cast<std::uint16_t>(低16位));
}

/**
 * @brief 取反 (Logical NOT)
 */
inline bool 取反(bool 被反转的逻辑值) { return !被反转的逻辑值; }
