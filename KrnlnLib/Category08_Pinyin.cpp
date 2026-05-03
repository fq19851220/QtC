#include "Category08_PinyinData.h"
#include "E2CppBase.h"

#include <algorithm>

// cmdCategory: 21
// categoryCn: 拼音处理

namespace {

const char *LookupByUnicode(char16_t code) {
  auto it = std::lower_bound(
      std::begin(kE2PinyinByUnicode), std::end(kE2PinyinByUnicode), code,
      [](const E2PinyinUnicodeEntry &entry, std::uint16_t value) {
        return entry.code < value;
      });
  if (it != std::end(kE2PinyinByUnicode) && it->code == code) {
    return it->pinyin;
  }
  return nullptr;
}

inline QList<文本型> SplitPinyins(const char *joined) {
  QList<文本型> result;
  if (!joined || !*joined) {
    return result;
  }

  const QStringList parts =
      QString::fromLatin1(joined).split(QLatin1Char('|'), Qt::SkipEmptyParts);
  for (const QString &part : parts) {
    result.push_back(part);
  }
  return result;
}

inline QList<文本型> GetAllPYInternal(const 文本型 &text) {
  if (text.isEmpty()) {
    return {};
  }
  if (const char *joined = LookupByUnicode(text.at(0).unicode())) {
    return SplitPinyins(joined);
  }
  return {};
}

inline 文本型 Internal_GetSM(const 文本型 &py) {
  static const char *sms[] = {"zh", "ch", "sh", "b", "p", "m", "f", "d",
                              "t",  "n",  "l",  "g", "k", "h", "j", "q",
                              "x",  "r",  "z",  "c", "s", "y", "w"};
  for (const char *sm : sms) {
    const QString prefix = QString::fromLatin1(sm);
    if (py.startsWith(prefix)) {
      return prefix;
    }
  }
  return {};
}

inline 文本型 Internal_GetYM(const 文本型 &py) {
  return py.mid(Internal_GetSM(py).size());
}

inline 文本型 ToSouthern(文本型 py) {
  if (py.startsWith(QStringLiteral("zh"))) {
    py.replace(0, 2, QStringLiteral("z"));
  } else if (py.startsWith(QStringLiteral("ch"))) {
    py.replace(0, 2, QStringLiteral("c"));
  } else if (py.startsWith(QStringLiteral("sh"))) {
    py.replace(0, 2, QStringLiteral("s"));
  }

  if (!py.isEmpty() && py.at(0) == QLatin1Char('n')) {
    py[0] = QLatin1Char('l');
  }

  const auto replaceEnd = [&](const QString &from, const QString &to) {
    if (py.endsWith(from)) {
      py.chop(from.size());
      py += to;
    }
  };
  replaceEnd(QStringLiteral("ing"), QStringLiteral("in"));
  replaceEnd(QStringLiteral("eng"), QStringLiteral("en"));
  replaceEnd(QStringLiteral("ang"), QStringLiteral("an"));
  return py;
}

inline QList<文本型> TokenizeFullPY(const 文本型 &text) {
  QList<文本型> res;
  for (QChar ch : text) {
    if (ch.unicode() < 0x80) {
      res.push_back(文本型(ch));
      continue;
    }
    const QList<文本型> py = GetAllPYInternal(文本型(ch));
    res.push_back(py.isEmpty() ? 文本型(ch) : py.first());
  }
  return res;
}

} // namespace

inline EQtArray<文本型> 取所有发音(const 文本型 &欲取其拼音的汉字) {
  EQtArray<文本型> res;
  res.data = GetAllPYInternal(欲取其拼音的汉字);
  res.dims = {static_cast<整数型>(res.data.size())};
  return res;
}

inline 整数型 取发音数目(const 文本型 &欲取其发音数目的汉字) {
  return static_cast<整数型>(GetAllPYInternal(欲取其发音数目的汉字).size());
}

inline 文本型 取拼音(const 文本型 &欲取其拼音编码的汉字,
                整数型 欲取拼音编码的索引 = 1) {
  const QList<文本型> all = GetAllPYInternal(欲取其拼音编码的汉字);
  if (欲取拼音编码的索引 >= 1 && 欲取拼音编码的索引 <= all.size()) {
    return all.at(欲取拼音编码的索引 - 1);
  }
  return {};
}

inline 文本型 取声母(const 文本型 &欲取其拼音编码的汉字,
                整数型 欲取拼音编码的索引 = 1) {
  return Internal_GetSM(取拼音(欲取其拼音编码的汉字, 欲取拼音编码的索引));
}

inline 文本型 取韵母(const 文本型 &欲取其拼音编码的汉字,
                整数型 欲取拼音编码的索引 = 1) {
  return Internal_GetYM(取拼音(欲取其拼音编码的汉字, 欲取拼音编码的索引));
}

inline 逻辑型 发音比较(const 文本型 &待比较文本一,
                  const 文本型 &待比较文本二,
                  逻辑型 是否支持南方音,
                  逻辑型 是否为模糊比较 = false) {
  const QList<文本型> tokens1 = TokenizeFullPY(待比较文本一);
  const QList<文本型> tokens2 = TokenizeFullPY(待比较文本二);

  const auto equal = [&](const 文本型 &a, const 文本型 &b) {
    if (a == b) {
      return true;
    }
    if (是否支持南方音) {
      return ToSouthern(a) == ToSouthern(b);
    }
    return false;
  };

  if (!是否为模糊比较) {
    if (tokens1.size() != tokens2.size()) {
      return false;
    }
    for (qsizetype i = 0; i < tokens1.size(); ++i) {
      if (!equal(tokens1.at(i), tokens2.at(i))) {
        return false;
      }
    }
    return true;
  }

  if (tokens1.size() > tokens2.size()) {
    return false;
  }
  for (qsizetype i = 0; i < tokens1.size(); ++i) {
    if (!equal(tokens1.at(i), tokens2.at(i))) {
      return false;
    }
  }
  return true;
}

inline 逻辑型 输入字比较(const 文本型 &待比较拼音输入字,
                  const 文本型 &待比较普通文本,
                  逻辑型 是否支持南方音,
                  整数型 拼音输入字类别 = 3) {
  if (待比较拼音输入字.isEmpty() || 待比较普通文本.isEmpty()) {
    return false;
  }

  文本型 input = 待比较拼音输入字;
  整数型 mode = 拼音输入字类别;
  if (mode == 3) {
    if (input.startsWith(QLatin1Char(';'))) {
      mode = 1;
      input.remove(0, 1);
    } else if (input.startsWith(QLatin1Char(':'))) {
      mode = 2;
      input.remove(0, 1);
    } else {
      mode = 1;
    }
  }

  const QList<文本型> targets = TokenizeFullPY(待比较普通文本);
  if (input.isEmpty() || targets.isEmpty()) {
    return false;
  }
  mode = 1;
  if (mode == 1) {
    qsizetype ti = 0;
    for (qsizetype ii = 0; ii < input.size();) {
      if (ti >= targets.size()) {
        return false;
      }
      文本型 py = targets.at(ti++);
      if (是否支持南方音) {
        py = ToSouthern(py);
      }
      if (py.isEmpty()) {
        return false;
      }
      const QChar ch = input.at(ii).toLower();
      if (!py.isEmpty() && py.at(0).toLower() == ch) {
        ++ii;
        continue;
      }
      if (input.mid(ii).startsWith(py, Qt::CaseInsensitive)) {
        ii += py.size();
        continue;
      }
      return false;
    }
    return true;
  }

  // stub: 其他输入字类别暂未实现
  return false;
}
