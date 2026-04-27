#include "E2CppBase.h"
#include <algorithm>
#include <cmath>
#include <random>


// cmdCategory: 2
// categoryCn: 算术运算

/**
 * @brief 取符号
 */
inline std::int32_t 取符号(double 欲取其符号的数值) {
  if (欲取其符号的数值 < 0)
    return -1;
  if (欲取其符号的数值 > 0)
    return 1;
  return 0;
}

/**
 * @brief 取绝对值
 */
inline double 取绝对值(double 欲取其绝对值的数值) {
  return std::abs(欲取其绝对值的数值);
}

/**
 * @brief 取整 (返回小于或等于该小数的第一个负整数 if negative)
 * 易语言：取整 (-7.8) -> -8
 * C++: std::floor(-7.8) -> -8.0
 */
inline std::int32_t 取整(double 欲取整的小数) {
  return static_cast<std::int32_t>(std::floor(欲取整的小数));
}

/**
 * @brief 绝对取整 (返回大于或等于该小数的第一个负整数 if negative)
 * 易语言：绝对取整 (-7.8) -> -7
 * C++: std::trunc(-7.8) -> -7.0
 */
inline std::int32_t 绝对取整(double 欲取整的小数) {
  return static_cast<std::int32_t>(std::trunc(欲取整的小数));
}

/**
 * @brief 四舍五入
 */
inline double 四舍五入(double 欲被四舍五入的数值,
                       std::int32_t 被舍入的位置 = 0) {
  double factor = std::pow(10.0, 被舍入的位置);
  return std::round(欲被四舍五入的数值 * factor) / factor;
}

/**
 * @brief 求次方
 */
inline double 求次方(double 欲求次方数值, double 次方数) {
  return std::pow(欲求次方数值, 次方数);
}

/**
 * @brief 求平方根
 */
inline double 求平方根(double 欲求其平方根的数值) {
  return std::sqrt(欲求其平方根的数值);
}

/**
 * @brief 求正弦
 */
inline double 求正弦(double 欲进行计算的角) { return std::sin(欲进行计算的角); }

/**
 * @brief 求余弦
 */
inline double 求余弦(double 欲进行计算的角) { return std::cos(欲进行计算的角); }

/**
 * @brief 求正切
 */
inline double 求正切(double 欲进行计算的角) { return std::tan(欲进行计算的角); }

/**
 * @brief 求反正切
 */
inline double 求反正切(double 欲求其反正切值的数值) {
  return std::atan(欲求其反正切值的数值);
}

/**
 * @brief 求自然对数
 */
inline double 求自然对数(double 欲求其自然对数的数值) {
  return std::log(欲求其自然对数的数值);
}

/**
 * @brief 求反对数 (exp)
 */
inline double 求反对数(double 欲求其反对数的数值) {
  return std::exp(欲求其反对数的数值);
}

/**
 * @brief 是否运算正确
 */
inline bool 是否运算正确(double 欲校验的计算结果) {
  return std::isfinite(欲校验的计算结果);
}

static std::mt19937 g_e2cpp_rng;

/**
 * @brief 置随机数种子
 */
inline void 置随机数种子(std::int32_t 欲置入的种子数值 = 0) {
  if (欲置入的种子数值 == 0) {
    g_e2cpp_rng.seed(static_cast<std::uint32_t>(
        std::chrono::system_clock::now().time_since_epoch().count()));
  } else {
    g_e2cpp_rng.seed(static_cast<std::uint32_t>(欲置入的种子数值));
  }
}

/**
 * @brief 取随机数
 */
inline std::int32_t 取随机数(std::int32_t 欲取随机数的最小值 = 0,
                             std::int32_t 欲取随机数的最大值 = 2147483647) {
  if (欲取随机数的最小值 > 欲取随机数的最大值)
    std::swap(欲取随机数的最小值, 欲取随机数的最大值);
  std::uniform_int_distribution<std::int32_t> dist(欲取随机数的最小值,
                                                   欲取随机数的最大值);
  return dist(g_e2cpp_rng);
}
