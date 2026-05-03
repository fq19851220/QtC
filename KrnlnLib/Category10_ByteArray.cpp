#include "E2CppBase.h"
#ifdef _WIN32
#include "E2CppVariant.h"
#endif

#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QVariant>

#include <algorithm>
#include <cstring>
#include <type_traits>

// cmdCategory: 10
// categoryCn: 字节集操作

namespace {

template <typename T>
inline void E2AppendRaw(字节集 &输出, const T &值) {
  输出.append(reinterpret_cast<const char *>(&值), static_cast<qsizetype>(sizeof(T)));
}

template <typename T>
inline T E2ReadRawOrDefault(const 字节集 &数据, qsizetype 偏移) {
  T 值{};
  if (偏移 < 0 || 偏移 + static_cast<qsizetype>(sizeof(T)) > 数据.size()) {
    return 值;
  }
  std::memcpy(&值, 数据.constData() + 偏移, sizeof(T));
  return 值;
}

inline 整数型 E2ClampCount(整数型 值) {
  return (std::max)(0, 值);
}

inline 整数型 E2FindBin(const 字节集 &被搜寻的字节集,
                    const 字节集 &欲寻找的字节集,
                    整数型 起始搜寻位置) {
  if (欲寻找的字节集.isEmpty()) {
    return 起始搜寻位置 >= 1 && 起始搜寻位置 <= 被搜寻的字节集.size() + 1
               ? 起始搜寻位置
               : -1;
  }
  const qsizetype from = (std::max)(0, 起始搜寻位置 - 1);
  const qsizetype pos = 被搜寻的字节集.indexOf(欲寻找的字节集, from);
  return pos >= 0 ? static_cast<整数型>(pos + 1) : -1;
}

inline 整数型 E2ReverseFindBin(const 字节集 &被搜寻的字节集,
                           const 字节集 &欲寻找的字节集,
                           整数型 起始搜寻位置) {
  if (被搜寻的字节集.isEmpty()) {
    return -1;
  }
  qsizetype from = 被搜寻的字节集.size() - 1;
  if (起始搜寻位置 >= 1) {
    from = (std::min)(static_cast<qsizetype>(起始搜寻位置 - 1),
                      被搜寻的字节集.size() - 1);
  }
  if (欲寻找的字节集.isEmpty()) {
    return static_cast<整数型>(from + 1);
  }
  const qsizetype pos = 被搜寻的字节集.lastIndexOf(欲寻找的字节集, from);
  return pos >= 0 ? static_cast<整数型>(pos + 1) : -1;
}

inline 字节集 E2ReverseBytes(const char *数据, qsizetype 长度) {
  字节集 结果(数据, 长度);
  std::reverse(结果.begin(), 结果.end());
  return 结果;
}

template <typename T>
inline 字节集 E2ToBinNumberArray(const QList<T> &数组) {
  字节集 结果;
  结果.reserve(static_cast<qsizetype>(数组.size() * static_cast<int>(sizeof(T))));
  for (const auto &项 : 数组) {
    E2AppendRaw(结果, 项);
  }
  return 结果;
}

template <typename T>
inline std::enable_if_t<std::is_arithmetic_v<T>, 字节集> E2ToBinScalar(const T &值) {
  字节集 结果;
  E2AppendRaw(结果, 值);
  return 结果;
}

} // namespace

inline 整数型 取字节集长度(const 字节集 &字节集数据) {
  return static_cast<整数型>(字节集数据.size());
}

inline 字节集 到字节集(const 字节集 &欲转换为字节集的数据) {
  return 欲转换为字节集的数据;
}

inline 字节集 到字节集(const E2CppLiteralArray &欲转换为字节集的数据) {
  return 字节集(欲转换为字节集的数据);
}

inline 字节集 到字节集(const 文本型 &欲转换为字节集的数据) {
  return 欲转换为字节集的数据.toUtf8();
}

inline 字节集 到字节集(const std::string &欲转换为字节集的数据) {
  return QByteArray(欲转换为字节集的数据.data(),
                    static_cast<qsizetype>(欲转换为字节集的数据.size()));
}

inline 字节集 到字节集(const 逻辑型 &欲转换为字节集的数据) {
  return E2ToBinScalar(欲转换为字节集的数据);
}

inline 字节集 到字节集(const 字节型 &欲转换为字节集的数据) {
  return E2ToBinScalar(欲转换为字节集的数据);
}

inline 字节集 到字节集(const 短整数型 &欲转换为字节集的数据) {
  return E2ToBinScalar(欲转换为字节集的数据);
}

inline 字节集 到字节集(const 整数型 &欲转换为字节集的数据) {
  return E2ToBinScalar(欲转换为字节集的数据);
}

inline 字节集 到字节集(const 长整数型 &欲转换为字节集的数据) {
  return E2ToBinScalar(欲转换为字节集的数据);
}

inline 字节集 到字节集(const 小数型 &欲转换为字节集的数据) {
  return E2ToBinScalar(欲转换为字节集的数据);
}

inline 字节集 到字节集(const 双精度小数型 &欲转换为字节集的数据) {
  return E2ToBinScalar(欲转换为字节集的数据);
}

inline 字节集 到字节集(const 日期时间型 &欲转换为字节集的数据) {
  return 到字节集(static_cast<长整数型>(欲转换为字节集的数据.toMSecsSinceEpoch()));
}

inline 字节集 到字节集(const 子程序指针型 &欲转换为字节集的数据) {
  字节集 结果;
  const quintptr 值 = reinterpret_cast<quintptr>(欲转换为字节集的数据);
  E2AppendRaw(结果, 值);
  return 结果;
}

inline 字节集 到字节集(std::monostate) {
  return {};
}

#ifdef _WIN32
inline 字节集 到字节集(const 变体型 &欲转换为字节集的数据) {
  const auto type = 欲转换为字节集的数据.取类型();
  if (type == E2Const_变体类型.空 || type == E2Const_变体类型.未知) {
    return {};
  }
  if (type == E2Const_变体类型.文本型) {
    return 到字节集(欲转换为字节集的数据.取文本());
  }
  if (type == E2Const_变体类型.逻辑型) {
    return 到字节集(欲转换为字节集的数据.取逻辑值());
  }
  if (type == E2Const_变体类型.日期型) {
    return 到字节集(欲转换为字节集的数据.取日期());
  }
  return 到字节集(static_cast<双精度小数型>(欲转换为字节集的数据.取数值()));
}
#endif

inline 字节集 到字节集(const QList<字节型> &欲转换为字节集的数据) {
  return E2ToBinNumberArray(欲转换为字节集的数据);
}

inline 字节集 到字节集(const QList<短整数型> &欲转换为字节集的数据) {
  return E2ToBinNumberArray(欲转换为字节集的数据);
}

inline 字节集 到字节集(const QList<整数型> &欲转换为字节集的数据) {
  return E2ToBinNumberArray(欲转换为字节集的数据);
}

inline 字节集 到字节集(const QList<长整数型> &欲转换为字节集的数据) {
  return E2ToBinNumberArray(欲转换为字节集的数据);
}

inline 字节集 到字节集(const QList<小数型> &欲转换为字节集的数据) {
  return E2ToBinNumberArray(欲转换为字节集的数据);
}

inline 字节集 到字节集(const QList<双精度小数型> &欲转换为字节集的数据) {
  return E2ToBinNumberArray(欲转换为字节集的数据);
}

inline 字节集 到字节集(const QList<逻辑型> &欲转换为字节集的数据) {
  字节集 结果;
  结果.reserve(欲转换为字节集的数据.size());
  for (逻辑型 项 : 欲转换为字节集的数据) {
    结果.append(项 ? '\x01' : '\x00');
  }
  return 结果;
}

template <typename T>
inline std::enable_if_t<std::is_arithmetic_v<T>, 字节集>
到字节集(const E2CppArray<T> &欲转换为字节集的数据) {
  字节集 结果;
  if constexpr (sizeof(T) == 1) {
    结果.reserve(欲转换为字节集的数据.size());
    for (std::int32_t i = 1; i <= 欲转换为字节集的数据.size(); ++i) {
      结果.append(static_cast<char>(static_cast<unsigned char>(
          static_cast<T>(欲转换为字节集的数据[i]))));
    }
  } else {
    结果.reserve(static_cast<qsizetype>(欲转换为字节集的数据.size()) *
                 static_cast<qsizetype>(sizeof(T)));
    for (std::int32_t i = 1; i <= 欲转换为字节集的数据.size(); ++i) {
      const T 值 = static_cast<T>(欲转换为字节集的数据[i]);
      E2AppendRaw(结果, 值);
    }
  }
  return 结果;
}

template <typename T>
inline std::enable_if_t<std::is_arithmetic_v<T>, 字节集>
到字节集(const EQtArray<T> &欲转换为字节集的数据) {
  字节集 结果;
  if constexpr (sizeof(T) == 1) {
    结果.reserve(欲转换为字节集的数据.size());
    for (std::int32_t i = 1; i <= 欲转换为字节集的数据.size(); ++i) {
      结果.append(static_cast<char>(static_cast<unsigned char>(
          static_cast<T>(欲转换为字节集的数据[i]))));
    }
  } else {
    结果.reserve(static_cast<qsizetype>(欲转换为字节集的数据.size()) *
               static_cast<qsizetype>(sizeof(T)));
    for (std::int32_t i = 1; i <= 欲转换为字节集的数据.size(); ++i) {
      const T 值 = static_cast<T>(欲转换为字节集的数据[i]);
      E2AppendRaw(结果, 值);
    }
  }
  return 结果;
}

template <typename T>
inline 字节集 到字节集(const QList<T> &) {
  return {};
}

template <typename Accessor>
inline std::enable_if_t<
    std::is_same_v<typename Accessor::E2CPP_ARRAY_ACCESSOR_TAG, void>,
    字节集>
到字节集(const Accessor &欲转换为字节集的数据) {
  using ValueType = typename Accessor::value_type;
  return 到字节集(static_cast<ValueType>(欲转换为字节集的数据));
}

inline 字节集 到字节集(const E2CppValue &欲转换为字节集的数据) {
  const auto &value = 欲转换为字节集的数据.value;
  if (std::holds_alternative<std::monostate>(value)) {
    return {};
  }
  if (std::holds_alternative<std::int32_t>(value)) {
    return 到字节集(std::get<std::int32_t>(value));
  }
  if (std::holds_alternative<std::int64_t>(value)) {
    // Cast to long long to get an exact match for 到字节集(const 长整数型&)
    // on both Windows (int64_t==long long) and Linux (int64_t==long).
    return E2ToBinScalar(static_cast<long long>(std::get<std::int64_t>(value)));
  }
  if (std::holds_alternative<float>(value)) {
    return 到字节集(std::get<float>(value));
  }
  if (std::holds_alternative<double>(value)) {
    return 到字节集(std::get<double>(value));
  }
  if (std::holds_alternative<QDateTime>(value)) {
    return 到字节集(std::get<QDateTime>(value));
  }
  if (std::holds_alternative<bool>(value)) {
    return 到字节集(std::get<bool>(value));
  }
  if (std::holds_alternative<std::string>(value)) {
    const auto &text = std::get<std::string>(value);
    return 字节集(QByteArray(text.data(), static_cast<qsizetype>(text.size())));
  }
  if (std::holds_alternative<E2CppByteArray>(value)) {
    return 字节集(std::get<E2CppByteArray>(value));
  }
  if (std::holds_alternative<void *>(value)) {
    return 到字节集(std::get<void *>(value));
  }
  return {};
}

inline E2CppValue 取字节集数据(const 字节集 &欲取出其中数据的字节集,
                        整数型 欲取出数据的类型,
                        整数型 起始索引位置 = 1) {
  const qsizetype 偏移 = 起始索引位置 < 1 ? 0 : 起始索引位置 - 1;
  switch (欲取出数据的类型) {
  case 1:
    return E2ReadRawOrDefault<字节型>(欲取出其中数据的字节集, 偏移);
  case 2:
    return E2ReadRawOrDefault<短整数型>(欲取出其中数据的字节集, 偏移);
  case 3:
    return E2ReadRawOrDefault<整数型>(欲取出其中数据的字节集, 偏移);
  case 4:
    return E2ReadRawOrDefault<长整数型>(欲取出其中数据的字节集, 偏移);
  case 5:
    return E2ReadRawOrDefault<小数型>(欲取出其中数据的字节集, 偏移);
  case 6:
    return E2ReadRawOrDefault<双精度小数型>(欲取出其中数据的字节集, 偏移);
  case 7:
    return static_cast<逻辑型>(E2ReadRawOrDefault<字节型>(欲取出其中数据的字节集, 偏移) != 0);
  case 8:
    return 日期时间型::fromMSecsSinceEpoch(
        E2ReadRawOrDefault<长整数型>(欲取出其中数据的字节集, 偏移));
  case 9: {
    const quintptr 指针值 = E2ReadRawOrDefault<quintptr>(欲取出其中数据的字节集, 偏移);
    return reinterpret_cast<void *>(指针值);
  }
  case 10: {
    if (偏移 < 0 || 偏移 >= 欲取出其中数据的字节集.size()) {
      return 文本型();
    }
    const char *起点 = 欲取出其中数据的字节集.constData() + 偏移;
    const qsizetype 剩余 = 欲取出其中数据的字节集.size() - 偏移;
    qsizetype 长度 = 0;
    while (长度 < 剩余 && 起点[长度] != '\0') {
      ++长度;
    }
    return QString::fromUtf8(起点, 长度);
  }
  default:
    return E2CppValue();
  }
}

inline 字节集 取字节集左边(const 字节集 &欲取其部分的字节集,
                    整数型 欲取出字节的数目) {
  return 欲取其部分的字节集.left(E2ClampCount(欲取出字节的数目));
}

inline 字节集 取字节集右边(const 字节集 &欲取其部分的字节集,
                    整数型 欲取出字节的数目) {
  return 欲取其部分的字节集.right(E2ClampCount(欲取出字节的数目));
}

inline 字节集 取字节集中间(const 字节集 &欲取其部分的字节集,
                     整数型 起始取出位置,
                     整数型 欲取出字节的数目) {
  if (起始取出位置 < 1 || 欲取出字节的数目 <= 0) {
    return {};
  }
  return 欲取其部分的字节集.mid(起始取出位置 - 1, 欲取出字节的数目);
}

inline 整数型 寻找字节集(const 字节集 &被搜寻的字节集,
                    const 字节集 &欲寻找的字节集,
                    整数型 起始搜寻位置 = 1) {
  return E2FindBin(被搜寻的字节集, 欲寻找的字节集, 起始搜寻位置);
}

inline 整数型 寻找字节集(const 字节集 &被搜寻的字节集,
                    const E2CppLiteralArray &欲寻找的字节集,
                    整数型 起始搜寻位置 = 1) {
  return 寻找字节集(被搜寻的字节集, 字节集(欲寻找的字节集), 起始搜寻位置);
}

inline 整数型 倒找字节集(const 字节集 &被搜寻的字节集,
                    const 字节集 &欲寻找的字节集,
                    整数型 起始搜寻位置) {
  return E2ReverseFindBin(被搜寻的字节集, 欲寻找的字节集, 起始搜寻位置);
}

inline 整数型 倒找字节集(const 字节集 &被搜寻的字节集,
                    const E2CppLiteralArray &欲寻找的字节集,
                    整数型 起始搜寻位置) {
  return 倒找字节集(被搜寻的字节集, 字节集(欲寻找的字节集), 起始搜寻位置);
}

inline 整数型 倒找字节集(const 字节集 &被搜寻的字节集,
                    const 字节集 &欲寻找的字节集) {
  return E2ReverseFindBin(
      被搜寻的字节集, 欲寻找的字节集, static_cast<整数型>(被搜寻的字节集.size()));
}

inline 整数型 倒找字节集(const 字节集 &被搜寻的字节集,
                    const E2CppLiteralArray &欲寻找的字节集) {
  return 倒找字节集(被搜寻的字节集, 字节集(欲寻找的字节集));
}

inline 字节集 字节集替换(const 字节集 &欲替换其部分的字节集,
                  整数型 起始替换位置,
                  整数型 替换长度,
                  const 字节集 &用作替换的字节集 = 字节集()) {
  字节集 结果 = 欲替换其部分的字节集;
  qsizetype 位置 = static_cast<qsizetype>(起始替换位置 - 1);
  if (位置 < 0) {
    位置 = 0;
  } else if (位置 > 结果.size()) {
    位置 = 结果.size();
  }
  qsizetype 长度 = static_cast<qsizetype>(替换长度);
  if (长度 < 0) {
    长度 = 0;
  }
  结果.replace(位置, 长度, 用作替换的字节集);
  return 结果;
}

inline 字节集 字节集替换(const 字节集 &欲替换其部分的字节集,
                  整数型 起始替换位置,
                  整数型 替换长度,
                  const E2CppLiteralArray &用作替换的字节集) {
  return 字节集替换(欲替换其部分的字节集, 起始替换位置, 替换长度,
               字节集(用作替换的字节集));
}

inline 字节集 子字节集替换(const 字节集 &欲被替换的字节集,
                    const 字节集 &欲被替换的子字节集,
                    const 字节集 &用作替换的子字节集 = 字节集(),
                    整数型 进行替换的起始位置 = 1,
                    整数型 替换进行的次数 = -1) {
  if (欲被替换的子字节集.isEmpty() || 替换进行的次数 == 0) {
    return 欲被替换的字节集;
  }

  字节集 结果 = 欲被替换的字节集;
  qsizetype 搜索位置 = (std::max)(0, 进行替换的起始位置 - 1);
  整数型 已替换次数 = 0;

  while (搜索位置 <= 结果.size()) {
    const qsizetype 位置 = 结果.indexOf(欲被替换的子字节集, 搜索位置);
    if (位置 < 0) {
      break;
    }
    结果.replace(位置, 欲被替换的子字节集.size(), 用作替换的子字节集);
    搜索位置 = 位置 + 用作替换的子字节集.size();
    ++已替换次数;
    if (替换进行的次数 > 0 && 已替换次数 >= 替换进行的次数) {
      break;
    }
  }

  return 结果;
}

inline 字节集 子字节集替换(const 字节集 &欲被替换的字节集,
                    const E2CppLiteralArray &欲被替换的子字节集,
                    const E2CppLiteralArray &用作替换的子字节集 = E2CppLiteralArray{},
                    整数型 进行替换的起始位置 = 1,
                    整数型 替换进行的次数 = -1) {
  return 子字节集替换(欲被替换的字节集, 字节集(欲被替换的子字节集),
                 字节集(用作替换的子字节集), 进行替换的起始位置, 替换进行的次数);
}

inline 字节集 取空白字节集(整数型 零字节数目) {
  if (零字节数目 <= 0) {
    return {};
  }
  return 字节集(零字节数目, '\0');
}

inline 字节集 取重复字节集(整数型 重复次数, const 字节集 &待重复的字节集) {
  if (重复次数 <= 0 || 待重复的字节集.isEmpty()) {
    return {};
  }
  字节集 结果;
  结果.reserve(待重复的字节集.size() * 重复次数);
  for (整数型 i = 0; i < 重复次数; ++i) {
    结果 += 待重复的字节集;
  }
  return 结果;
}

inline QList<字节集> 分割字节集(const 字节集 &待分割字节集,
                        const 字节集 &用作分割的字节集 = 字节集(1, '\0'),
                        整数型 要返回的子字节集数目 = -1) {
  QList<字节集> 结果;
  if (待分割字节集.isEmpty()) {
    return 结果;
  }
  if (用作分割的字节集.isEmpty()) {
    结果.push_back(待分割字节集);
    return 结果;
  }
  if (要返回的子字节集数目 == 1) {
    结果.push_back(待分割字节集);
    return 结果;
  }

  qsizetype 起点 = 0;
  整数型 已返回 = 0;
  while (起点 <= 待分割字节集.size()) {
    if (要返回的子字节集数目 > 0 && 已返回 == 要返回的子字节集数目 - 1) {
      结果.push_back(待分割字节集.mid(起点));
      return 结果;
    }
    const qsizetype 位置 = 待分割字节集.indexOf(用作分割的字节集, 起点);
    if (位置 < 0) {
      结果.push_back(待分割字节集.mid(起点));
      return 结果;
    }
    结果.push_back(待分割字节集.mid(起点, 位置 - 起点));
    ++已返回;
    起点 = 位置 + 用作分割的字节集.size();
  }
  return 结果;
}

inline 字节集 指针到字节集(整数型 内存数据指针, 整数型 内存数据长度) {
  if (内存数据指针 == 0 || 内存数据长度 <= 0) {
    return {};
  }
  return 字节集(reinterpret_cast<const char *>(static_cast<std::intptr_t>(内存数据指针)),
              内存数据长度);
}

inline 整数型 指针到整数(整数型 内存数据指针) {
  if (内存数据指针 == 0) {
    return 0;
  }
  return *reinterpret_cast<const 整数型 *>(static_cast<std::intptr_t>(内存数据指针));
}

inline 小数型 指针到小数(整数型 内存数据指针) {
  if (内存数据指针 == 0) {
    return 0.0f;
  }
  return *reinterpret_cast<const 小数型 *>(static_cast<std::intptr_t>(内存数据指针));
}

inline 双精度小数型 指针到双精度小数(整数型 内存数据指针) {
  if (内存数据指针 == 0) {
    return 0.0;
  }
  return *reinterpret_cast<const 双精度小数型 *>(
      static_cast<std::intptr_t>(内存数据指针));
}

inline 整数型 取字节集内整数(const 字节集 &待处理的字节集,
                      整数型 欲获取整数所处偏移,
                      逻辑型 是否反转字节序 = false) {
  qsizetype 偏移 = static_cast<qsizetype>(欲获取整数所处偏移);
  if (偏移 < 0) {
    偏移 = 0;
  }
  if (偏移 + static_cast<qsizetype>(sizeof(整数型)) > 待处理的字节集.size()) {
    return 0;
  }
  if (!是否反转字节序) {
    return E2ReadRawOrDefault<整数型>(待处理的字节集, 偏移);
  }
  const 字节集 反转 = E2ReverseBytes(待处理的字节集.constData() + 偏移, sizeof(整数型));
  return E2ReadRawOrDefault<整数型>(反转, 0);
}

inline void 置字节集内整数(字节集 &待处理的字节集,
                    整数型 欲设置整数所处偏移,
                    整数型 欲设置的整数值,
                    逻辑型 是否反转字节序 = false) {
  qsizetype 偏移 = static_cast<qsizetype>(欲设置整数所处偏移);
  if (偏移 < 0) {
    偏移 = 0;
  }
  const qsizetype 需要长度 = 偏移 + static_cast<qsizetype>(sizeof(整数型));
  if (待处理的字节集.size() < 需要长度) {
    待处理的字节集.resize(需要长度);
  }

  if (!是否反转字节序) {
    std::memcpy(待处理的字节集.data() + 偏移, &欲设置的整数值, sizeof(整数型));
    return;
  }

  字节集 临时;
  E2AppendRaw(临时, 欲设置的整数值);
  std::reverse(临时.begin(), 临时.end());
  std::memcpy(待处理的字节集.data() + 偏移, 临时.constData(), sizeof(整数型));
}
