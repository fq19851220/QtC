#include "E2CppBase.h"

#include <QtCore/QLocale>
#include <QtCore/QStringList>

#include <algorithm>
#include <cmath>

// cmdCategory: 11
// categoryCn: 数值转换

namespace E2QtConversionInternal {

inline const QStringList &繁体数字() {
  static const QStringList v = {QStringLiteral("零"), QStringLiteral("壹"),
                                QStringLiteral("贰"), QStringLiteral("叁"),
                                QStringLiteral("肆"), QStringLiteral("伍"),
                                QStringLiteral("陆"), QStringLiteral("柒"),
                                QStringLiteral("捌"), QStringLiteral("玖")};
  return v;
}

inline const QStringList &简体数字() {
  static const QStringList v = {QStringLiteral("零"), QStringLiteral("一"),
                                QStringLiteral("二"), QStringLiteral("三"),
                                QStringLiteral("四"), QStringLiteral("五"),
                                QStringLiteral("六"), QStringLiteral("七"),
                                QStringLiteral("八"), QStringLiteral("九")};
  return v;
}

inline const QStringList &繁体单位() {
  static const QStringList v = {QString(), QStringLiteral("拾"),
                                QStringLiteral("佰"), QStringLiteral("仟")};
  return v;
}

inline const QStringList &简体单位() {
  static const QStringList v = {QString(), QStringLiteral("十"),
                                QStringLiteral("百"), QStringLiteral("千")};
  return v;
}

inline const QStringList &组单位() {
  static const QStringList v = {QString(), QStringLiteral("万"),
                                QStringLiteral("亿"), QStringLiteral("兆")};
  return v;
}

inline const QStringList &数字表(逻辑型 简体) {
  return 简体 ? 简体数字() : 繁体数字();
}

inline const QStringList &单位表(逻辑型 简体) {
  return 简体 ? 简体单位() : 繁体单位();
}

inline 文本型 格式化整数(const 文本型 &s, 逻辑型 简体) {
  if (s.isEmpty() || s == QStringLiteral("0")) {
    return 数字表(简体).at(0);
  }

  const QStringList &digits = 数字表(简体);
  const QStringList &units = 单位表(简体);
  const QStringList &groups = 组单位();

  文本型 res;
  const int len = s.size();
  bool zero = false;

  for (int i = 0; i < len; ++i) {
    const int p = len - 1 - i;
    const int digit = s.at(i).unicode() - u'0';
    const int pos = p % 4;
    const int group = p / 4;

    if (digit != 0) {
      if (zero) {
        res += digits.at(0);
        zero = false;
      }
      res += digits.at(digit);
      res += units.at(pos);
    } else {
      zero = true;
    }

    if (pos == 0 && group > 0) {
      bool hasNonZero = false;
      for (int j = (std::max)(0, i - 3); j <= i; ++j) {
        if (s.at(j) != QLatin1Char('0')) {
          hasNonZero = true;
          break;
        }
      }
      if (hasNonZero) {
        res += groups.at(group % groups.size());
        zero = false;
      }
    }
  }

  return res;
}

inline 文本型 浮点到规范文本(double value, int fixedPrecision = -1) {
  if (fixedPrecision >= 0) {
    return QString::number(value, 'f', fixedPrecision);
  }
  文本型 s = QString::number(value, 'f', 6);
  while (s.contains(QLatin1Char('.')) && s.endsWith(QLatin1Char('0'))) {
    s.chop(1);
  }
  if (s.endsWith(QLatin1Char('.'))) {
    s.chop(1);
  }
  return s;
}

} // namespace E2QtConversionInternal

inline 双精度小数型 到数值(const E2CppValue &待转换的文本或数值) {
  const auto &value = 待转换的文本或数值.value;
  if (std::holds_alternative<std::monostate>(value)) {
    return 0.0;
  }
  if (std::holds_alternative<std::int32_t>(value)) {
    return static_cast<双精度小数型>(std::get<std::int32_t>(value));
  }
  if (std::holds_alternative<std::int64_t>(value)) {
    return static_cast<双精度小数型>(std::get<std::int64_t>(value));
  }
  if (std::holds_alternative<float>(value)) {
    return static_cast<双精度小数型>(std::get<float>(value));
  }
  if (std::holds_alternative<double>(value)) {
    return static_cast<双精度小数型>(std::get<double>(value));
  }
  if (std::holds_alternative<QDateTime>(value)) {
    return static_cast<双精度小数型>(
        std::get<QDateTime>(value).toMSecsSinceEpoch());
  }
  if (std::holds_alternative<bool>(value)) {
    return std::get<bool>(value) ? 1.0 : 0.0;
  }
  if (std::holds_alternative<std::string>(value)) {
    bool ok = false;
    const auto v = QString::fromStdString(std::get<std::string>(value)).toDouble(&ok);
    return ok ? v : 0.0;
  }
  if (std::holds_alternative<E2CppByteArray>(value)) {
    const QByteArray bytes = E2ToQByteArray(std::get<E2CppByteArray>(value));
    bool ok = false;
    const auto v = QString::fromUtf8(bytes).toDouble(&ok);
    return ok ? v : 0.0;
  }
  return 0.0;
}

inline 文本型 数值到大写(double 欲转换形式的数值,
                    逻辑型 是否转换为简体 = false) {
  const 逻辑型 isNeg = 欲转换形式的数值 < 0;
  const double val = std::fabs(欲转换形式的数值);
  const 文本型 s = E2QtConversionInternal::浮点到规范文本(val);

  文本型 intPart = s;
  文本型 fracPart;
  const qsizetype dotPos = s.indexOf(QLatin1Char('.'));
  if (dotPos >= 0) {
    intPart = s.left(dotPos);
    fracPart = s.mid(dotPos + 1);
  }

  文本型 res =
      E2QtConversionInternal::格式化整数(intPart, 是否转换为简体);
  if (!fracPart.isEmpty()) {
    res += QStringLiteral("点");
    const QStringList &digits =
        E2QtConversionInternal::数字表(是否转换为简体);
    for (QChar c : fracPart) {
      res += digits.at(c.unicode() - u'0');
    }
  }

  return (isNeg ? QStringLiteral("负") : QString()) + res;
}

inline 文本型 数值到大写Q(double 欲转换形式的数值,
                     逻辑型 是否转换为简体 = false) {
  return 数值到大写(欲转换形式的数值, 是否转换为简体);
}

inline 文本型 数值到金额(double 欲转换形式的数值,
                    逻辑型 是否转换为简体 = false) {
  const 逻辑型 isNeg = 欲转换形式的数值 < 0;
  const double val = std::fabs(欲转换形式的数值);
  const 长整数型 cents =
      static_cast<长整数型>(std::llround(val * 100.0));
  const 长整数型 intPart = cents / 100;
  const int fracPart = static_cast<int>(cents % 100);
  const QStringList &digits =
      E2QtConversionInternal::数字表(是否转换为简体);

  if (cents == 0) {
    return digits.at(0) + QStringLiteral("元整");
  }

  文本型 res;
  if (intPart > 0) {
    res += E2QtConversionInternal::格式化整数(QString::number(intPart),
                                        是否转换为简体);
    res += QStringLiteral("元");
  }

  if (fracPart == 0) {
    res += QStringLiteral("整");
  } else {
    const int jiao = fracPart / 10;
    const int fen = fracPart % 10;

    bool needZero = false;
    if (intPart > 0) {
      if (jiao == 0 && fen > 0) {
        needZero = true;
      } else if (intPart % 10 == 0 && jiao > 0) {
        needZero = true;
      }
    }

    if (needZero) {
      res += digits.at(0);
    }
    if (jiao > 0) {
      res += digits.at(jiao) + QStringLiteral("角");
    }
    if (fen > 0) {
      res += digits.at(fen) + QStringLiteral("分");
    }
  }

  return (isNeg ? QStringLiteral("负") : QString()) + res;
}

inline 文本型 数值到金额Q(double 欲转换形式的数值,
                     逻辑型 是否转换为简体 = false) {
  return 数值到金额(欲转换形式的数值, 是否转换为简体);
}

inline 文本型 数值到格式文本(double 欲转换为文本的数值,
                      整数型 小数保留位数 = -1,
                      逻辑型 是否进行千分位分隔 = false) {
  文本型 s;
  if (小数保留位数 < 0) {
    s = QString::number(欲转换为文本的数值, 'g', 15);
  } else {
    s = QString::number(欲转换为文本的数值, 'f', 小数保留位数);
  }

  if (是否进行千分位分隔) {
    bool ok = false;
    const double value = s.toDouble(&ok);
    if (ok) {
      if (小数保留位数 < 0) {
        s = QLocale::system().toString(value, 'g', 15);
      } else {
        s = QLocale::system().toString(value, 'f', 小数保留位数);
      }
    }
  }
  return s;
}

inline 文本型 数值到格式文本Q(double 欲转换为文本的数值,
                        整数型 欲保留的小数位数 = -1,
                        逻辑型 是否使用千分位 = false) {
  return 数值到格式文本(欲转换为文本的数值, 欲保留的小数位数,
                  是否使用千分位);
}

inline 文本型 取十六进制文本(整数型 欲取进制文本的数值) {
  return QString::number(欲取进制文本的数值, 16).toUpper();
}

inline 文本型 取八进制文本(整数型 欲取进制文本的数值) {
  return QString::number(欲取进制文本的数值, 8);
}

inline 字节型 到字节(const E2CppValue &v) {
  return static_cast<字节型>(到数值(v));
}

inline 短整数型 到短整数(const E2CppValue &v) {
  return static_cast<短整数型>(到数值(v));
}

inline 整数型 到整数(const E2CppValue &v) {
  return static_cast<整数型>(到数值(v));
}

inline 长整数型 到长整数(const E2CppValue &v) {
  return static_cast<长整数型>(到数值(v));
}

inline 小数型 到小数(const E2CppValue &v) {
  return static_cast<小数型>(到数值(v));
}

inline 整数型 十六进制(const 文本型 &十六进制文本常量) {
  bool ok = false;
  const 整数型 value = 十六进制文本常量.toInt(&ok, 16);
  return ok ? value : 0;
}

inline 整数型 二进制(const 文本型 &二进制文本常量) {
  bool ok = false;
  const 整数型 value = 二进制文本常量.toInt(&ok, 2);
  return ok ? value : 0;
}

inline 整数型 反转整数字节序(整数型 待反转的整数值) {
  无符号整数型 v = static_cast<无符号整数型>(待反转的整数值);
  v = ((v & 0xff000000u) >> 24) | ((v & 0x00ff0000u) >> 8) |
      ((v & 0x0000ff00u) << 8) | ((v & 0x000000ffu) << 24);
  return static_cast<整数型>(v);
}
