#define _CRT_SECURE_NO_WARNINGS
#include "E2CppVariant.h"
#include "E2CppComObject.h"
#include <comdef.h>
#include <vector>

// KrnlnLib.h 会把本实现 include 到多个翻译单元，成员定义必须是 inline。
#ifdef E2CPP_KRNLN_HEADER_IMPL
#define E2CPP_KRNLN_INLINE inline
#else
#define E2CPP_KRNLN_INLINE
#endif

// 静态转换辅助，由 EType_对象 内部重复使用
static BSTR ToVariantBSTR(const QString &s) {
  return SysAllocStringLen(reinterpret_cast<const OLECHAR *>(s.utf16()),
                           static_cast<UINT>(s.size()));
}

static QString FromVariantBSTR(BSTR bstr) {
  return bstr ? QString::fromWCharArray(bstr) : QString();
}

E2CPP_KRNLN_INLINE EType_变体型::EType_变体型() {
  VariantInit(&m_var);
  // 易语言里未赋值变体更接近“数值型 0”，不是 VT_EMPTY。
  m_var.vt = VT_R8;
  m_var.dblVal = 0.0;
}

E2CPP_KRNLN_INLINE EType_变体型::~EType_变体型() { 清除(); }

E2CPP_KRNLN_INLINE EType_变体型::EType_变体型(const EType_变体型 &other) {
  VariantInit(&m_var);
  VariantCopy(&m_var, (VARIANT *)&other.m_var);
}

E2CPP_KRNLN_INLINE EType_变体型 &EType_变体型::operator=(const EType_变体型 &other) {
  if (this != &other) {
    VariantCopy(&m_var, (VARIANT *)&other.m_var);
  }
  return *this;
}

E2CPP_KRNLN_INLINE void EType_变体型::清除() { VariantClear(&m_var); }

E2CPP_KRNLN_INLINE int32_t EType_变体型::取类型() const {
  switch (m_var.vt) {
  case VT_EMPTY:
    return 变体类型.空;
  case VT_I2:
  case VT_I4:
  case VT_R4:
  case VT_R8:
  case VT_CY:
    return 变体类型.数值型;
  case VT_BSTR:
    return 变体类型.文本型;
  case VT_BOOL:
    return 变体类型.逻辑型;
  case VT_DATE:
    return 变体类型.日期型;
  case VT_DISPATCH:
  case VT_UNKNOWN:
    return 变体类型.对象型;
  case VT_ERROR:
    return 变体类型.错误值型;
  default:
    if (m_var.vt & VT_ARRAY) {
      VARTYPE base = m_var.vt & VT_TYPEMASK;
      if (base == VT_VARIANT)
        return 变体类型.变体型数组;
      if (base == VT_DISPATCH || base == VT_UNKNOWN)
        return 变体类型.对象型数组;
      if (base == VT_BSTR)
        return 变体类型.文本型数组;
      return 变体类型.数值型数组; // 简化
    }
    return 变体类型.未知;
  }
}

E2CPP_KRNLN_INLINE int32_t EType_变体型::取数组成员数() const {
  if (!(m_var.vt & VT_ARRAY))
    return -1;
  long lBound, uBound;
  SafeArrayGetLBound(m_var.parray, 1, &lBound);
  SafeArrayGetUBound(m_var.parray, 1, &uBound);
  return (int32_t)(uBound - lBound + 1);
}

E2CPP_KRNLN_INLINE 文本型 EType_变体型::取文本(int32_t 成员索引) const {
  VARIANT varTemp;
  VariantInit(&varTemp);
  if (成员索引 > 0 && (m_var.vt & VT_ARRAY)) {
    long idx = 成员索引 - 1;
    SafeArrayGetElement(m_var.parray, &idx, &varTemp);
  } else {
    VariantCopy(&varTemp, (VARIANT *)&m_var);
  }
  VariantChangeType(&varTemp, &varTemp, 0, VT_BSTR);
  QString s = FromVariantBSTR(varTemp.bstrVal);
  VariantClear(&varTemp);
  return s;
}

E2CPP_KRNLN_INLINE double EType_变体型::取数值(int32_t 成员索引) const {
  VARIANT varTemp;
  VariantInit(&varTemp);
  if (成员索引 > 0 && (m_var.vt & VT_ARRAY)) {
    long idx = 成员索引 - 1;
    SafeArrayGetElement(m_var.parray, &idx, &varTemp);
  } else {
    VariantCopy(&varTemp, (VARIANT *)&m_var);
  }
  double res = 0.0;
  if (SUCCEEDED(VariantChangeType(&varTemp, &varTemp, 0, VT_R8)))
    res = varTemp.dblVal;
  VariantClear(&varTemp);
  return res;
}

E2CPP_KRNLN_INLINE bool EType_变体型::取逻辑值(int32_t 成员索引) const {
  return 取数值(成员索引) != 0;
}

E2CPP_KRNLN_INLINE E2CppDateTime EType_变体型::取日期(int32_t 成员索引) const {
  // 逻辑类似于取数值但目标是 VT_DATE
  return E2CppDateTime();
}

E2CPP_KRNLN_INLINE EType_对象 EType_变体型::取对象(int32_t 成员索引) const {
  VARIANT varTemp;
  VariantInit(&varTemp);
  if (成员索引 > 0 && (m_var.vt & VT_ARRAY)) {
    long idx = 成员索引 - 1;
    SafeArrayGetElement(m_var.parray, &idx, &varTemp);
  } else {
    VariantCopy(&varTemp, (VARIANT *)&m_var);
  }
  EType_对象 res;
  // 这里需完善对象类的赋值逻辑
  VariantClear(&varTemp);
  return res;
}

E2CPP_KRNLN_INLINE EType_变体型 EType_变体型::取变体型(int32_t 成员索引) const {
  EType_变体型 res;
  if (成员索引 > 0 && (m_var.vt & VT_ARRAY)) {
    SafeArrayGetElement(m_var.parray, (long *)&成员索引,
                        (VARIANT *)res.GetInternalVariant());
  }
  return res;
}

E2CPP_KRNLN_INLINE E2CppByteArray EType_变体型::取字节集(int32_t 成员索引) const {
  return E2CppByteArray();
}

E2CPP_KRNLN_INLINE bool EType_变体型::赋值(const E2CppValue &欲写入数据, int32_t 成员索引) {
  if (成员索引 > 0) {
    if (!(m_var.vt & VT_ARRAY) || m_var.parray == nullptr) {
      return false;
    }
    VARIANT elem;
    VariantInit(&elem);
    std::visit(
        [&elem](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, bool>) {
            elem.vt = VT_BOOL;
            elem.boolVal = arg ? VARIANT_TRUE : VARIANT_FALSE;
          } else if constexpr (std::is_same_v<T, double> ||
                               std::is_same_v<T, float> ||
                               std::is_same_v<T, std::int32_t> ||
                               std::is_same_v<T, std::int64_t>) {
            elem.vt = VT_R8;
            elem.dblVal = (double)arg;
          } else if constexpr (std::is_same_v<T, std::string>) {
            elem.vt = VT_BSTR;
            elem.bstrVal = ToVariantBSTR(QString::fromStdString(arg));
          } else if constexpr (std::is_same_v<T, QString>) {
            elem.vt = VT_BSTR;
            elem.bstrVal = ToVariantBSTR(arg);
          } else if constexpr (std::is_same_v<T, E2CppDateTime>) {
            elem.vt = VT_DATE;
            elem.date = 0;
          } else if constexpr (std::is_same_v<T, void *>) {
            elem.vt = VT_BYREF | VT_I1;
            elem.byref = arg;
          } else if constexpr (std::is_same_v<T, E2CppByteArray>) {
            elem.vt = VT_BSTR;
            elem.bstrVal = ToVariantBSTR(QString::fromUtf8(
                reinterpret_cast<const char *>(arg.data()),
                static_cast<qsizetype>(arg.size())));
          }
        },
        欲写入数据.value);
    long idx = 成员索引 - 1;
    HRESULT hr = SafeArrayPutElement(m_var.parray, &idx, &elem);
    VariantClear(&elem);
    return SUCCEEDED(hr);
  }
  清除();
  if (欲写入数据.value.index() == 0) { // monostate
    m_var.vt = VT_EMPTY;
    return true;
  }

  // 使用 std::visit 将 E2CppValue (std::variant) 转换为 VARIANT
  std::visit(
      [this](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
          m_var.vt = VT_BOOL;
          m_var.boolVal = arg ? VARIANT_TRUE : VARIANT_FALSE;
        } else if constexpr (std::is_same_v<T, double> ||
                             std::is_same_v<T, float> ||
                             std::is_same_v<T, std::int32_t> ||
                             std::is_same_v<T, std::int64_t>) {
          m_var.vt = VT_R8;
          m_var.dblVal = (double)arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
          m_var.vt = VT_BSTR;
          m_var.bstrVal = ToVariantBSTR(QString::fromStdString(arg));
        } else if constexpr (std::is_same_v<T, QString>) {
          m_var.vt = VT_BSTR;
          m_var.bstrVal = ToVariantBSTR(arg);
        } else if constexpr (std::is_same_v<T, E2CppDateTime>) {
          m_var.vt = VT_DATE;
          // 易语言日期转换逻辑
          m_var.date = 0;
        } else if constexpr (std::is_same_v<T, void *>) {
          m_var.vt = VT_BYREF | VT_I1; // 仅示意，通常为指针
          m_var.byref = arg;
        }
      },
      欲写入数据.value);
  return true;
}

E2CPP_KRNLN_INLINE bool EType_变体型::赋值(const EType_对象 &欲写入对象, int32_t 成员索引) {
  return 赋值(E2CppValue(欲写入对象.取原始接口()), 成员索引);
}

E2CPP_KRNLN_INLINE bool EType_变体型::创建数组(int32_t 成员类型, int32_t 成员数目) {
  清除();
  SAFEARRAYBOUND sab;
  sab.lLbound = 0;
  sab.cElements = (ULONG)成员数目;
  m_var.vt = VT_VARIANT | VT_ARRAY;
  m_var.parray = SafeArrayCreate(VT_VARIANT, 1, &sab);
  return m_var.parray != nullptr;
}

E2CPP_KRNLN_INLINE bool EType_变体型::置类型(int32_t 欲设置的类型) {
  清除();
  // 根据易语言变体类型，映射回 VARTYPE
  return true;
}

#undef E2CPP_KRNLN_INLINE

