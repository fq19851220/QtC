#include "E2CppBase.h"

#include <QtCore/QDate>
#include <QtCore/QDateTime>
#include <QtCore/QTime>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

// cmdCategory: 12
// categoryCn: 时间操作

inline 日期时间型 取现行时间() { return QDateTime::currentDateTime(); }

inline 日期时间型 指定时间(整数型 年, 整数型 月 = 1, 整数型 日 = 1,
                    整数型 小时 = 0, 整数型 分钟 = 0,
                    整数型 秒 = 0) {
  const QDate date(年, 月, 日);
  const QTime time(小时, 分钟, 秒);
  if (!date.isValid() || !time.isValid()) {
    return {};
  }
  return QDateTime(date, time);
}

inline 日期时间型 到时间(const E2CppValue &v) {
  return std::visit(
      overloaded{[](const QDateTime &dt) { return dt; },
                 [](const std::string &s) {
                   return E2CppDateTime::fromEString(s).is_empty
                              ? QDateTime()
                              : static_cast<QDateTime>(
                                    E2CppValue(E2CppDateTime::fromEString(s)));
                 },
                 [](const auto &) { return QDateTime(); }},
      v.value);
}

inline 整数型 取年份(const 日期时间型 &欲取出其部分的日期时间) {
  return 欲取出其部分的日期时间.date().year();
}

inline 整数型 取月份(const 日期时间型 &欲取出其部分的日期时间) {
  return 欲取出其部分的日期时间.date().month();
}

inline 整数型 取日(const 日期时间型 &欲取出其部分的日期时间) {
  return 欲取出其部分的日期时间.date().day();
}

inline 整数型 取星期几(const 日期时间型 &欲取出其部分的日期时间) {
  const int iso = 欲取出其部分的日期时间.date().dayOfWeek();
  return iso == 7 ? 1 : (iso + 1);
}

inline 整数型 取小时(const 日期时间型 &欲取出其部分的日期时间) {
  return 欲取出其部分的日期时间.time().hour();
}

inline 整数型 取分钟(const 日期时间型 &欲取出其部分的日期时间) {
  return 欲取出其部分的日期时间.time().minute();
}

inline 整数型 取秒(const 日期时间型 &欲取出其部分的日期时间) {
  return 欲取出其部分的日期时间.time().second();
}

inline 日期时间型 增减时间(const 日期时间型 &欲增减的日期时间,
                    整数型 被增加部分, 整数型 增加值) {
  if (!欲增减的日期时间.isValid()) {
    return {};
  }
  switch (被增加部分) {
  case 1:
    return 欲增减的日期时间.addYears(增加值);
  case 2:
    return 欲增减的日期时间.addMonths(增加值 * 3);
  case 3:
    return 欲增减的日期时间.addMonths(增加值);
  case 4:
    return 欲增减的日期时间.addDays(增加值 * 7);
  case 5:
    return 欲增减的日期时间.addDays(增加值);
  case 6:
    return 欲增减的日期时间.addSecs(增加值 * 3600);
  case 7:
    return 欲增减的日期时间.addSecs(增加值 * 60);
  case 8:
    return 欲增减的日期时间.addSecs(增加值);
  default:
    return 欲增减的日期时间;
  }
}

inline 双精度小数型 取时间间隔(const 日期时间型 &时间1, const 日期时间型 &时间2,
                        整数型 取间隔部分) {
  const qint64 diff = 时间2.secsTo(时间1);
  switch (取间隔部分) {
  case 1:
    return diff / (365.25 * 24 * 3600.0);
  case 2:
    return diff / (91.31 * 24 * 3600.0);
  case 3:
    return diff / (30.44 * 24 * 3600.0);
  case 4:
    return diff / (7.0 * 24 * 3600.0);
  case 5:
    return diff / (24.0 * 3600.0);
  case 6:
    return diff / 3600.0;
  case 7:
    return diff / 60.0;
  case 8:
    return static_cast<双精度小数型>(diff);
  default:
    return 0.0;
  }
}

inline 整数型 取某月天数(整数型 年份, 整数型 月份) {
  const QDate date(年份, 月份, 1);
  return date.isValid() ? date.daysInMonth() : 0;
}

inline 文本型 时间到文本(const 日期时间型 &欲转换的时间,
                  整数型 转换部分 = 1) {
  if (!欲转换的时间.isValid()) {
    return {};
  }
  if (转换部分 == 2) {
    return 欲转换的时间.toString(QStringLiteral("yyyy年M月d日"));
  }
  if (转换部分 == 3) {
    return 欲转换的时间.toString(QStringLiteral("H时m分s秒"));
  }
  return 欲转换的时间.toString(QStringLiteral("yyyy年M月d日H时m分s秒"));
}

inline 整数型 取时间部分(const 日期时间型 &欲取其部分的时间,
                    整数型 欲取的时间部分) {
  const QDate d = 欲取其部分的时间.date();
  const QTime t = 欲取其部分的时间.time();
  switch (欲取的时间部分) {
  case 1:
    return d.year();
  case 2:
    return (d.month() - 1) / 3 + 1;
  case 3:
    return d.month();
  case 4:
    return d.dayOfYear() > 0 ? ((d.dayOfYear() - 1) / 7 + 1) : 0;
  case 5:
    return d.day();
  case 6:
    return t.hour();
  case 7:
    return t.minute();
  case 8:
    return t.second();
  case 9: {
    const int iso = d.dayOfWeek();
    return iso == 7 ? 1 : (iso + 1);
  }
  case 10:
    return d.dayOfYear();
  default:
    return 0;
  }
}

inline 逻辑型 置现行时间(const 日期时间型 &欲设置的时间) {
  if (!欲设置的时间.isValid()) {
    return false;
  }

#ifdef _WIN32
  const QDate d = 欲设置的时间.date();
  const QTime t = 欲设置的时间.time();
  SYSTEMTIME st;
  st.wYear = static_cast<WORD>(d.year());
  st.wMonth = static_cast<WORD>(d.month());
  st.wDay = static_cast<WORD>(d.day());
  st.wHour = static_cast<WORD>(t.hour());
  st.wMinute = static_cast<WORD>(t.minute());
  st.wSecond = static_cast<WORD>(t.second());
  st.wMilliseconds = 0;
  st.wDayOfWeek = static_cast<WORD>(d.dayOfWeek() % 7);
  return SetLocalTime(&st) != 0;
#elif defined(Q_OS_ANDROID)
#  warning "置现行时间: Android 不支持修改系统时间，将始终返回 false"
  return false;
#else
  const auto secs = 欲设置的时间.toSecsSinceEpoch();
  timeval tv;
  tv.tv_sec = static_cast<time_t>(secs);
  tv.tv_usec = 0;
  return settimeofday(&tv, nullptr) == 0;
#endif
}

inline 日期时间型 取日期(const 日期时间型 &时间) {
  return 时间.isValid() ? QDateTime(时间.date(), QTime(0, 0, 0)) : QDateTime();
}

inline 日期时间型 取时间(const 日期时间型 &时间) {
  return 时间.isValid() ? QDateTime(QDate(2000, 1, 1), 时间.time())
                        : QDateTime();
}

inline 文本型 时间到文本Q(const 日期时间型 &欲转换的时间,
                    整数型 转换部分 = 1) {
  return 时间到文本(欲转换的时间, 转换部分);
}

// Compatibility wrappers while the rest of the runtime is still migrating.
inline E2CppDateTime 取现行时间E() { return static_cast<E2CppDateTime>(E2CppValue(取现行时间())); }
inline E2CppDateTime 指定时间E(整数型 年, 整数型 月 = 1, 整数型 日 = 1,
                           整数型 小时 = 0, 整数型 分钟 = 0,
                           整数型 秒 = 0) {
  return static_cast<E2CppDateTime>(E2CppValue(指定时间(年, 月, 日, 小时, 分钟, 秒)));
}
inline E2CppDateTime 到时间E(const E2CppValue &v) {
  return static_cast<E2CppDateTime>(E2CppValue(到时间(v)));
}
inline 整数型 取年份(const E2CppDateTime &v) { return 取年份(E2ToQDateTime(v)); }
inline 整数型 取月份(const E2CppDateTime &v) { return 取月份(E2ToQDateTime(v)); }
inline 整数型 取日(const E2CppDateTime &v) { return 取日(E2ToQDateTime(v)); }
inline 整数型 取星期几(const E2CppDateTime &v) { return 取星期几(E2ToQDateTime(v)); }
inline 整数型 取小时(const E2CppDateTime &v) { return 取小时(E2ToQDateTime(v)); }
inline 整数型 取分钟(const E2CppDateTime &v) { return 取分钟(E2ToQDateTime(v)); }
inline 整数型 取秒(const E2CppDateTime &v) { return 取秒(E2ToQDateTime(v)); }
inline E2CppDateTime 增减时间(const E2CppDateTime &v, 整数型 part, 整数型 delta) {
  return static_cast<E2CppDateTime>(E2CppValue(增减时间(E2ToQDateTime(v), part, delta)));
}
inline 双精度小数型 取时间间隔(const E2CppDateTime &a, const E2CppDateTime &b, 整数型 p) {
  return 取时间间隔(E2ToQDateTime(a), E2ToQDateTime(b), p);
}
inline std::string 时间到文本(const E2CppDateTime &v, 整数型 p = 1) {
  return 时间到文本(E2ToQDateTime(v), p).toStdString();
}
inline 整数型 取时间部分(const E2CppDateTime &v, 整数型 p) {
  return 取时间部分(E2ToQDateTime(v), p);
}
inline 逻辑型 置现行时间(const E2CppDateTime &v) { return 置现行时间(E2ToQDateTime(v)); }
inline E2CppDateTime 取日期(const E2CppDateTime &v) {
  return static_cast<E2CppDateTime>(E2CppValue(取日期(E2ToQDateTime(v))));
}
inline E2CppDateTime 取时间(const E2CppDateTime &v) {
  return static_cast<E2CppDateTime>(E2CppValue(取时间(E2ToQDateTime(v))));
}
inline 文本型 时间到文本Q(const E2CppDateTime &欲转换的时间,
                    整数型 转换部分 = 1) {
  return 时间到文本(E2ToQDateTime(欲转换的时间), 转换部分);
}
