#include "E2CppBase.h"

#include <QtCore/QList>
#include <QtCore/QStringList>
#include <type_traits>

// cmdCategory: 9
// categoryCn: 文本操作

namespace {

inline Qt::CaseSensitivity E2Case(逻辑型 是否区分大小写) {
  return 是否区分大小写 ? Qt::CaseSensitive : Qt::CaseInsensitive;
}

inline bool E2IsBlankChar(QChar ch) {
  return ch == QChar(u' ') || ch == QChar(0x3000);
}

// 易语言文本位置相关命令基本按“双字节宽度”处理：
// ASCII 字符按 1 计，其余字符按 2 计。
inline qsizetype E2TextUnitWidth(QChar ch) {
  return ch.unicode() <= 0x7F ? 1 : 2;
}

inline 整数型 E2QStringIndexToTextPos(const 文本型 &文本, qsizetype index) {
  if (index <= 0) {
    return 1;
  }
  index = (std::min)(index, 文本.size());
  整数型 logicalPos = 1;
  for (qsizetype i = 0; i < index; ++i) {
    logicalPos += static_cast<整数型>(E2TextUnitWidth(文本.at(i)));
  }
  return logicalPos;
}

inline 整数型 E2TextLogicalLength(const 文本型 &文本) {
  整数型 total = 0;
  for (QChar ch : 文本) {
    total += static_cast<整数型>(E2TextUnitWidth(ch));
  }
  return total;
}

inline qsizetype E2TextPosToQStringIndex(const 文本型 &文本, 整数型 易位置) {
  if (易位置 <= 1) {
    return 0;
  }
  qsizetype logicalPos = 1;
  for (qsizetype i = 0; i < 文本.size(); ++i) {
    if (logicalPos >= 易位置) {
      return i;
    }
    logicalPos += E2TextUnitWidth(文本.at(i));
  }
  return 文本.size();
}

inline qsizetype E2TextSpanToQStringLength(const 文本型 &文本, qsizetype startIndex,
                                           整数型 易长度) {
  if (易长度 <= 0 || startIndex < 0 || startIndex >= 文本.size()) {
    return 0;
  }
  qsizetype consumed = 0;
  qsizetype i = startIndex;
  while (i < 文本.size() && consumed < 易长度) {
    consumed += E2TextUnitWidth(文本.at(i));
    ++i;
  }
  return i - startIndex;
}

template <typename T>
文本型 E2ToTextValue(const T &值);

template <typename T>
文本型 E2JoinListText(const QList<T> &值) {
  QStringList parts;
  parts.reserve(值.size());
  for (const auto &item : 值) {
    parts.push_back(E2ToTextValue(item));
  }
  return QStringLiteral("[") + parts.join(QStringLiteral(", ")) +
         QStringLiteral("]");
}

template <typename T>
文本型 E2ToTextValue(const QList<T> &值) {
  return E2JoinListText(值);
}

template <typename T>
文本型 E2ToTextValue(const T &值) {
  using U = std::decay_t<T>;

  if constexpr (std::is_same_v<U, 文本型>) {
    return 值;
  } else if constexpr (std::is_same_v<U, const char *> ||
                       std::is_same_v<U, char *>) {
    return QString::fromUtf8(值 ? 值 : "");
  } else if constexpr (std::is_same_v<U, 字节集>) {
    const qsizetype nul = 值.indexOf('\0');
    return nul >= 0 ? QString::fromUtf8(值.constData(), nul)
                    : QString::fromUtf8(值);
  } else if constexpr (std::is_same_v<U, 逻辑型>) {
    return 值 ? QStringLiteral("真") : QStringLiteral("假");
  } else if constexpr (std::is_same_v<U, 整数型> ||
                       std::is_same_v<U, 短整数型> ||
                       std::is_same_v<U, 长整数型> ||
                       std::is_same_v<U, 字节型>) {
    return QString::number(static_cast<qlonglong>(值));
  } else if constexpr (std::is_same_v<U, 小数型> ||
                       std::is_same_v<U, 双精度小数型>) {
    return QString::number(static_cast<double>(值), 'g', 15);
  } else if constexpr (std::is_same_v<U, 日期时间型>) {
    return 值.isValid()
               ? 值.toString(QStringLiteral("yyyy年M月d日 H时m分s秒"))
               : QString();
  } else if constexpr (std::is_same_v<U, 变体型>) {
    if (!值.isValid() || 值.isNull()) {
      return QString();
    }
    if (值.metaType().id() == QMetaType::QString) {
      return 值.toString();
    }
    if (值.metaType().id() == QMetaType::QByteArray) {
      return E2ToTextValue(字节集(值.toByteArray()));
    }
    if (值.metaType().id() == QMetaType::QDateTime) {
      return E2ToTextValue(值.toDateTime());
    }
    if (值.metaType().id() == QMetaType::Bool) {
      return E2ToTextValue(值.toBool());
    }
    return 值.toString();
  } else if constexpr (std::is_same_v<U, 子程序指针型> ||
                       std::is_same_v<U, 子程序指针>) {
    return QStringLiteral("0x") +
           QString::number(reinterpret_cast<quintptr>(值), 16).toUpper();
  } else if constexpr (std::is_same_v<U, E2CppValue>) {
    return std::visit([](const auto &item) { return E2ToTextValue(item); },
                      值.value);
  } else if constexpr (std::is_arithmetic_v<U>) {
    return QString::number(值);
  } else {
    return QString();
  }
}

inline 文本型 E2TrimLeft(const 文本型 &文本) {
  qsizetype i = 0;
  while (i < 文本.size() && E2IsBlankChar(文本.at(i))) {
    ++i;
  }
  return 文本.mid(i);
}

inline 文本型 E2TrimRight(const 文本型 &文本) {
  qsizetype i = 文本.size();
  while (i > 0 && E2IsBlankChar(文本.at(i - 1))) {
    --i;
  }
  return 文本.left(i);
}

} // namespace

inline 整数型 取文本长度(const 文本型 &文本数据) {
  return E2TextLogicalLength(文本数据);
}

inline 文本型 取文本左边(const 文本型 &欲取其部分的文本,
                    整数型 欲取出字符的数目) {
  if (欲取出字符的数目 <= 0) {
    return {};
  }
  const qsizetype endIndex =
      E2TextPosToQStringIndex(欲取其部分的文本, 欲取出字符的数目 + 1);
  return 欲取其部分的文本.left(endIndex);
}

inline 文本型 取文本右边(const 文本型 &欲取其部分的文本,
                    整数型 欲取出字符的数目) {
  if (欲取出字符的数目 <= 0) {
    return {};
  }
  const 整数型 totalLen = E2TextLogicalLength(欲取其部分的文本);
  if (欲取出字符的数目 >= totalLen) {
    return 欲取其部分的文本;
  }
  const 整数型 startPos = totalLen - 欲取出字符的数目 + 1;
  const qsizetype startIndex =
      E2TextPosToQStringIndex(欲取其部分的文本, startPos);
  return 欲取其部分的文本.mid(startIndex);
}

inline 文本型 取文本中间(const 文本型 &欲取其部分的文本,
                    整数型 起始取出位置,
                    整数型 欲取出字符的数目) {
  if (起始取出位置 < 1 || 欲取出字符的数目 <= 0) {
    return {};
  }
  const qsizetype startIndex =
      E2TextPosToQStringIndex(欲取其部分的文本, 起始取出位置);
  const qsizetype span =
      E2TextSpanToQStringLength(欲取其部分的文本, startIndex, 欲取出字符的数目);
  return 欲取其部分的文本.mid(startIndex, span);
}

inline 文本型 字符(字节型 欲取其字符的字符代码) {
  return 文本型(QChar(static_cast<char16_t>(欲取其字符的字符代码)));
}

inline 整数型 取代码(const 文本型 &欲取字符代码的文本,
                 整数型 欲取其代码的字符位置 = 1) {
  if (欲取其代码的字符位置 < 1 || 欲取其代码的字符位置 > 欲取字符代码的文本.size()) {
    return 0;
  }
  return static_cast<整数型>(
      欲取字符代码的文本.at(欲取其代码的字符位置 - 1).unicode());
}

inline 整数型 寻找文本(const 文本型 &被搜寻的文本,
                 const 文本型 &欲寻找的文本,
                 整数型 起始搜寻位置 = 1,
                 逻辑型 是否不区分大小写 = false) {
  const qsizetype from = E2TextPosToQStringIndex(
      被搜寻的文本, (std::max)(1, 起始搜寻位置));
  const qsizetype pos =
      被搜寻的文本.indexOf(欲寻找的文本, from,
                      是否不区分大小写 ? Qt::CaseInsensitive
                                       : Qt::CaseSensitive);
  return pos >= 0 ? E2QStringIndexToTextPos(被搜寻的文本, pos) : -1;
}

inline 整数型 寻找文本(const std::string &被搜寻的文本,
                 const 文本型 &欲寻找的文本,
                 整数型 起始搜寻位置 = 1,
                 逻辑型 是否不区分大小写 = false) {
  return 寻找文本(QString::fromStdString(被搜寻的文本), 欲寻找的文本,
              起始搜寻位置, 是否不区分大小写);
}

inline 整数型 寻找文本(const std::string &被搜寻的文本,
                 const std::string &欲寻找的文本,
                 整数型 起始搜寻位置 = 1,
                 逻辑型 是否不区分大小写 = false) {
  return 寻找文本(QString::fromStdString(被搜寻的文本),
              QString::fromStdString(欲寻找的文本), 起始搜寻位置,
              是否不区分大小写);
}

inline 整数型 倒找文本(const 文本型 &被搜寻的文本,
                 const 文本型 &欲寻找的文本,
                 整数型 起始搜寻位置,
                 逻辑型 是否不区分大小写) {
  if (被搜寻的文本.isEmpty()) {
    return -1;
  }
  qsizetype from = 被搜寻的文本.size() - 1;
  if (起始搜寻位置 >= 1) {
    const qsizetype mapped =
        E2TextPosToQStringIndex(被搜寻的文本, 起始搜寻位置);
    from = mapped >= 被搜寻的文本.size()
               ? 被搜寻的文本.size() - 1
               : mapped;
  }
  const qsizetype pos =
      被搜寻的文本.lastIndexOf(欲寻找的文本, from,
                          是否不区分大小写 ? Qt::CaseInsensitive
                                           : Qt::CaseSensitive);
  return pos >= 0 ? E2QStringIndexToTextPos(被搜寻的文本, pos) : -1;
}

inline 整数型 倒找文本(const std::string &被搜寻的文本,
                 const 文本型 &欲寻找的文本,
                 整数型 起始搜寻位置,
                 逻辑型 是否不区分大小写) {
  return 倒找文本(QString::fromStdString(被搜寻的文本), 欲寻找的文本,
              起始搜寻位置, 是否不区分大小写);
}

inline 整数型 倒找文本(const 文本型 &被搜寻的文本,
                 const 文本型 &欲寻找的文本) {
  return 倒找文本(被搜寻的文本, 欲寻找的文本,
              取文本长度(被搜寻的文本), false);
}

inline 整数型 倒找文本(const 文本型 &被搜寻的文本,
                 const 文本型 &欲寻找的文本,
                 整数型 起始搜寻位置) {
  return 倒找文本(被搜寻的文本, 欲寻找的文本, 起始搜寻位置, false);
}

inline 文本型 到大写(const 文本型 &欲变换的文本) {
  return 欲变换的文本.toUpper();
}

inline 文本型 到小写(const 文本型 &欲变换的文本) {
  return 欲变换的文本.toLower();
}

inline 文本型 到全角(const 文本型 &欲变换的文本) {
  文本型 结果;
  结果.reserve(欲变换的文本.size());
  for (QChar ch : 欲变换的文本) {
    const ushort code = ch.unicode();
    if (code == 0x20) {
      结果 += QChar(0x3000);
    } else if (code >= 0x21 && code <= 0x7E) {
      结果 += QChar(code + 0xFEE0);
    } else {
      结果 += ch;
    }
  }
  return 结果;
}

inline 文本型 到半角(const 文本型 &欲变换的文本) {
  文本型 结果;
  结果.reserve(欲变换的文本.size());
  for (QChar ch : 欲变换的文本) {
    const ushort code = ch.unicode();
    if (code == 0x3000) {
      结果 += QChar(0x20);
    } else if (code >= 0xFF01 && code <= 0xFF5E) {
      结果 += QChar(code - 0xFEE0);
    } else {
      结果 += ch;
    }
  }
  return 结果;
}

template <typename T>
inline 文本型 到文本(const T &待转换的数据) {
  return E2ToTextValue(待转换的数据);
}

template <typename T>
inline 文本型 到文本Q(const T &待转换的数据) {
  return E2ToTextValue(待转换的数据);
}

inline 文本型 删首空(const 文本型 &欲删除空格的文本) {
  return E2TrimLeft(欲删除空格的文本);
}

inline 文本型 删尾空(const 文本型 &欲删除空格的文本) {
  return E2TrimRight(欲删除空格的文本);
}

inline 文本型 删首尾空(const 文本型 &欲删除空格的文本) {
  return E2TrimRight(E2TrimLeft(欲删除空格的文本));
}

inline 文本型 删全部空(const 文本型 &欲删除空格的文本) {
  文本型 结果;
  结果.reserve(欲删除空格的文本.size());
  for (QChar ch : 欲删除空格的文本) {
    if (!E2IsBlankChar(ch)) {
      结果 += ch;
    }
  }
  return 结果;
}

inline 文本型 文本替换(const 文本型 &欲被替换的文本,
                  整数型 起始替换位置,
                  整数型 替换长度,
                  const 文本型 &用作替换的文本 = 文本型()) {
  文本型 结果 = 欲被替换的文本;
  const qsizetype pos = E2TextPosToQStringIndex(结果, 起始替换位置);
  const qsizetype len = E2TextSpanToQStringLength(结果, pos, 替换长度);
  结果.replace(pos, len, 用作替换的文本);
  return 结果;
}

inline 文本型 子文本替换(const 文本型 &欲被替换的文本,
                    const 文本型 &欲被替换的子文本,
                    const 文本型 &用作替换的子文本 = 文本型(),
                    整数型 进行替换的起始位置 = 1,
                    整数型 替换进行的次数 = -1,
                    逻辑型 是否区分大小写 = true) {
  if (欲被替换的子文本.isEmpty() || 替换进行的次数 == 0) {
    return 欲被替换的文本;
  }

  文本型 结果 = 欲被替换的文本;
  qsizetype 搜索位置 = E2TextPosToQStringIndex(
      结果, (std::max)(1, 进行替换的起始位置));
  整数型 已替换次数 = 0;

  while (搜索位置 <= 结果.size()) {
    const qsizetype pos =
        结果.indexOf(欲被替换的子文本, 搜索位置, E2Case(是否区分大小写));
    if (pos < 0) {
      break;
    }
    结果.replace(pos, 欲被替换的子文本.size(), 用作替换的子文本);
    搜索位置 = pos + 用作替换的子文本.size();
    ++已替换次数;
    if (替换进行的次数 > 0 && 已替换次数 >= 替换进行的次数) {
      break;
    }
  }

  return 结果;
}

inline 文本型 取空白文本(整数型 重复次数) {
  if (重复次数 <= 0) {
    return {};
  }
  return 文本型(重复次数, QChar(u' '));
}

inline 文本型 取空白文本Q(整数型 重复次数) {
  return 取空白文本(重复次数);
}

inline 文本型 取重复文本(整数型 重复次数, const 文本型 &待重复文本) {
  if (重复次数 <= 0 || 待重复文本.isEmpty()) {
    return {};
  }
  文本型 结果;
  结果.reserve(待重复文本.size() * 重复次数);
  for (整数型 i = 0; i < 重复次数; ++i) {
    结果 += 待重复文本;
  }
  return 结果;
}

inline 文本型 取重复文本Q(整数型 重复次数, const 文本型 &待重复文本) {
  return 取重复文本(重复次数, 待重复文本);
}

inline 整数型 文本比较(const 文本型 &待比较文本一,
                  const 文本型 &待比较文本二,
                  逻辑型 是否区分大小写 = true) {
  return 待比较文本一.compare(待比较文本二, E2Case(是否区分大小写));
}

inline QList<文本型> 分割文本(const 文本型 &待分割文本,
                        const 文本型 &用作分割的文本 = QStringLiteral(","),
                        整数型 要返回的子文本数目 = -1) {
  QList<文本型> 结果;
  if (待分割文本.isEmpty()) {
    return 结果;
  }
  if (用作分割的文本.isEmpty()) {
    结果.push_back(待分割文本);
    return 结果;
  }
  if (要返回的子文本数目 == 1) {
    结果.push_back(待分割文本);
    return 结果;
  }

  qsizetype start = 0;
  整数型 已加入 = 0;
  while (start <= 待分割文本.size()) {
    if (要返回的子文本数目 > 0 && 已加入 == 要返回的子文本数目 - 1) {
      结果.push_back(待分割文本.mid(start));
      return 结果;
    }
    const qsizetype pos = 待分割文本.indexOf(用作分割的文本, start);
    if (pos < 0) {
      结果.push_back(待分割文本.mid(start));
      return 结果;
    }
    结果.push_back(待分割文本.mid(start, pos - start));
    ++已加入;
    start = pos + 用作分割的文本.size();
  }
  return 结果;
}

inline 文本型 指针到文本(std::intptr_t 内存文本指针) {
  if (内存文本指针 == 0) {
    return {};
  }
  return QString::fromUtf8(reinterpret_cast<const char *>(内存文本指针));
}

inline 字节集 文本到UTF8(const 文本型 &待转换的文本) {
  字节集 结果 = 待转换的文本.toUtf8();
  结果.append('\0');
  return 结果;
}

inline 文本型 UTF8到文本(const 字节集 &待转换的UTF8文本数据) {
  const qsizetype nul = 待转换的UTF8文本数据.indexOf('\0');
  if (nul >= 0) {
    return QString::fromUtf8(待转换的UTF8文本数据.constData(), nul);
  }
  return QString::fromUtf8(待转换的UTF8文本数据);
}

inline 文本型 UTF8到文本Q(const 字节集 &待转换的UTF8文本数据) {
  return UTF8到文本(待转换的UTF8文本数据);
}

inline bool e2_like(const 文本型 &被比较文本, const 文本型 &比较文本) {
  return 被比较文本.contains(比较文本);
}

template <typename T, std::enable_if_t<is_e2cpp_accessor<T>::value, int> = 0>
inline 文本型 到大写(const T &值) {
  return 到大写(static_cast<typename T::value_type>(值));
}

template <typename T, std::enable_if_t<is_e2cpp_accessor<T>::value, int> = 0>
inline 文本型 到小写(const T &值) {
  return 到小写(static_cast<typename T::value_type>(值));
}

template <typename TA, typename TB,
          std::enable_if_t<is_e2cpp_accessor<TA>::value &&
                               !is_e2cpp_accessor<TB>::value,
                           int> = 0>
inline 整数型 文本比较(const TA &a, const TB &b,
                  逻辑型 是否区分大小写 = true) {
  return 文本比较(到文本Q(static_cast<typename TA::value_type>(a)), 到文本Q(b),
              是否区分大小写);
}

template <typename TA, typename TB,
          std::enable_if_t<!is_e2cpp_accessor<TA>::value &&
                               is_e2cpp_accessor<TB>::value,
                           int> = 0>
inline 整数型 文本比较(const TA &a, const TB &b,
                  逻辑型 是否区分大小写 = true) {
  return 文本比较(到文本Q(a),
              到文本Q(static_cast<typename TB::value_type>(b)),
              是否区分大小写);
}

template <typename TA, typename TB,
          std::enable_if_t<is_e2cpp_accessor<TA>::value &&
                               is_e2cpp_accessor<TB>::value,
                           int> = 0>
inline 整数型 文本比较(const TA &a, const TB &b,
                  逻辑型 是否区分大小写 = true) {
  return 文本比较(到文本Q(static_cast<typename TA::value_type>(a)),
              到文本Q(static_cast<typename TB::value_type>(b)),
              是否区分大小写);
}
