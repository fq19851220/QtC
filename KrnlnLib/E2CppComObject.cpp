#include "E2CppComObject.h"
#include "E2CppVariant.h"
#include <comdef.h>

#ifdef _WIN32

static BSTR ToBSTR(const QString &text) {
  return SysAllocStringLen(reinterpret_cast<const OLECHAR *>(text.utf16()),
                           static_cast<UINT>(text.size()));
}

static QString FromBSTR(BSTR bstr) {
  return bstr ? QString::fromWCharArray(bstr) : QString();
}

inline EType_对象::EType_对象() = default;

inline EType_对象::~EType_对象() { 清除(); }

inline EType_对象::EType_对象(const EType_对象 &other) : m_pDisp(other.m_pDisp) {
  if (m_pDisp) {
    m_pDisp->AddRef();
  }
  m_lastError = other.m_lastError;
}

inline EType_对象 &EType_对象::operator=(const EType_对象 &other) {
  if (this != &other) {
    清除();
    m_pDisp = other.m_pDisp;
    if (m_pDisp) {
      m_pDisp->AddRef();
    }
    m_lastError = other.m_lastError;
  }
  return *this;
}

inline bool EType_对象::创建(const QString &对象类型, const QString &) {
  清除();
  CLSID clsid{};
  BSTR progId = ToBSTR(对象类型);
  const HRESULT hrProgId = CLSIDFromProgID(progId, &clsid);
  SysFreeString(progId);
  if (FAILED(hrProgId)) {
    SetLastError(hrProgId);
    return false;
  }

  const HRESULT hrCreate =
      CoCreateInstance(clsid, nullptr, CLSCTX_ALL, IID_IDispatch,
                       reinterpret_cast<void **>(&m_pDisp));
  if (FAILED(hrCreate)) {
    SetLastError(hrCreate);
    return false;
  }
  m_lastError.clear();
  return true;
}

inline bool EType_对象::获取(const QString &对象类型) {
  清除();
  CLSID clsid{};
  BSTR progId = ToBSTR(对象类型);
  const HRESULT hrProgId = CLSIDFromProgID(progId, &clsid);
  SysFreeString(progId);
  if (FAILED(hrProgId)) {
    SetLastError(hrProgId);
    return false;
  }

  IUnknown *unknown = nullptr;
  HRESULT hr = GetActiveObject(clsid, nullptr, &unknown);
  if (SUCCEEDED(hr) && unknown) {
    hr = unknown->QueryInterface(IID_IDispatch,
                                 reinterpret_cast<void **>(&m_pDisp));
    unknown->Release();
  }
  if (FAILED(hr)) {
    SetLastError(hr);
    return false;
  }
  m_lastError.clear();
  return true;
}

inline void EType_对象::清除() {
  if (m_pDisp) {
    m_pDisp->Release();
    m_pDisp = nullptr;
  }
  m_lastError.clear();
}

inline bool EType_对象::是否为空() const { return m_pDisp == nullptr; }

inline bool EType_对象::是否相等(const EType_对象 &欲检查的对象) const {
  if (m_pDisp == 欲检查的对象.m_pDisp) {
    return true;
  }
  if (!m_pDisp || !欲检查的对象.m_pDisp) {
    return false;
  }

  IUnknown *unk1 = nullptr;
  IUnknown *unk2 = nullptr;
  m_pDisp->QueryInterface(IID_IUnknown, reinterpret_cast<void **>(&unk1));
  欲检查的对象.m_pDisp->QueryInterface(IID_IUnknown,
                                         reinterpret_cast<void **>(&unk2));
  const bool same = (unk1 == unk2);
  if (unk1) {
    unk1->Release();
  }
  if (unk2) {
    unk2->Release();
  }
  return same;
}

inline 文本型 EType_对象::读文本属性(const QString &属性名称, ...) {
  VARIANT value;
  VariantInit(&value);
  const HRESULT hr = InvokeHelper(DISPATCH_PROPERTYGET, &value, 属性名称, 0, nullptr);
  if (FAILED(hr)) {
    return {};
  }

  VARIANT converted;
  VariantInit(&converted);
  QString result;
  if (SUCCEEDED(VariantChangeType(&converted, &value, 0, VT_BSTR))) {
    result = FromBSTR(converted.bstrVal);
  }
  VariantClear(&converted);
  VariantClear(&value);
  return result;
}

inline 双精度小数型 EType_对象::读数值属性(const QString &属性名称, ...) {
  VARIANT value;
  VariantInit(&value);
  const HRESULT hr = InvokeHelper(DISPATCH_PROPERTYGET, &value, 属性名称, 0, nullptr);
  if (FAILED(hr)) {
    return 0.0;
  }

  VARIANT converted;
  VariantInit(&converted);
  double result = 0.0;
  if (SUCCEEDED(VariantChangeType(&converted, &value, 0, VT_R8))) {
    result = converted.dblVal;
  }
  VariantClear(&converted);
  VariantClear(&value);
  return result;
}

inline 逻辑型 EType_对象::读逻辑属性(const QString &属性名称, ...) {
  VARIANT value;
  VariantInit(&value);
  const HRESULT hr = InvokeHelper(DISPATCH_PROPERTYGET, &value, 属性名称, 0, nullptr);
  if (FAILED(hr)) {
    return false;
  }

  VARIANT converted;
  VariantInit(&converted);
  bool result = false;
  if (SUCCEEDED(VariantChangeType(&converted, &value, 0, VT_BOOL))) {
    result = converted.boolVal != VARIANT_FALSE;
  }
  VariantClear(&converted);
  VariantClear(&value);
  return result;
}

inline E2CppDateTime EType_对象::读日期属性(const QString &属性名称, ...) {
  VARIANT value;
  VariantInit(&value);
  const HRESULT hr = InvokeHelper(DISPATCH_PROPERTYGET, &value, 属性名称, 0, nullptr);
  if (FAILED(hr)) {
    return E2CppDateTime();
  }

  VARIANT converted;
  VariantInit(&converted);
  E2CppDateTime result;
  if (SUCCEEDED(VariantChangeType(&converted, &value, 0, VT_DATE))) {
    result = E2CppDateTime::fromOleDate(converted.date);
  }
  VariantClear(&converted);
  VariantClear(&value);
  return result;
}

inline EType_对象 EType_对象::读对象型属性(const QString &属性名称, ...) {
  VARIANT value;
  VariantInit(&value);
  InvokeHelper(DISPATCH_PROPERTYGET, &value, 属性名称, 0, nullptr);

  EType_对象 result;
  if (value.vt == VT_DISPATCH && value.pdispVal) {
    result.m_pDisp = value.pdispVal;
    result.m_pDisp->AddRef();
  }
  VariantClear(&value);
  return result;
}

inline EType_变体型 EType_对象::读属性(const QString &属性名称, ...) {
  VARIANT value;
  VariantInit(&value);
  InvokeHelper(DISPATCH_PROPERTYGET, &value, 属性名称, 0, nullptr);

  EType_变体型 result;
  VariantClear(result.GetInternalVariant());
  VariantCopy(result.GetInternalVariant(), &value);
  VariantClear(&value);
  return result;
}

inline 逻辑型 EType_对象::写属性(const QString &属性名称, ...) {
  return SUCCEEDED(
      InvokeHelper(DISPATCH_PROPERTYPUT, nullptr, 属性名称, 0, nullptr));
}

inline void EType_对象::方法(const QString &方法名称, ...) {
  InvokeHelper(DISPATCH_METHOD, nullptr, 方法名称, 0, nullptr);
}

inline EType_变体型 EType_对象::通用方法(const QString &方法名称, ...) {
  VARIANT value;
  VariantInit(&value);
  InvokeHelper(DISPATCH_METHOD, &value, 方法名称, 0, nullptr);

  EType_变体型 result;
  VariantClear(result.GetInternalVariant());
  VariantCopy(result.GetInternalVariant(), &value);
  VariantClear(&value);
  return result;
}

inline 文本型 EType_对象::文本方法(const QString &方法名称, ...) {
  VARIANT value;
  VariantInit(&value);
  InvokeHelper(DISPATCH_METHOD, &value, 方法名称, 0, nullptr);

  VARIANT converted;
  VariantInit(&converted);
  QString result;
  if (SUCCEEDED(VariantChangeType(&converted, &value, 0, VT_BSTR))) {
    result = FromBSTR(converted.bstrVal);
  }
  VariantClear(&converted);
  VariantClear(&value);
  return result;
}

inline 双精度小数型 EType_对象::数值方法(const QString &方法名称, ...) {
  VARIANT value;
  VariantInit(&value);
  InvokeHelper(DISPATCH_METHOD, &value, 方法名称, 0, nullptr);

  VARIANT converted;
  VariantInit(&converted);
  double result = 0.0;
  if (SUCCEEDED(VariantChangeType(&converted, &value, 0, VT_R8))) {
    result = converted.dblVal;
  }
  VariantClear(&converted);
  VariantClear(&value);
  return result;
}

inline 逻辑型 EType_对象::逻辑方法(const QString &方法名称, ...) {
  VARIANT value;
  VariantInit(&value);
  InvokeHelper(DISPATCH_METHOD, &value, 方法名称, 0, nullptr);

  VARIANT converted;
  VariantInit(&converted);
  bool result = false;
  if (SUCCEEDED(VariantChangeType(&converted, &value, 0, VT_BOOL))) {
    result = converted.boolVal != VARIANT_FALSE;
  }
  VariantClear(&converted);
  VariantClear(&value);
  return result;
}

inline E2CppDateTime EType_对象::日期方法(const QString &方法名称, ...) {
  VARIANT value;
  VariantInit(&value);
  InvokeHelper(DISPATCH_METHOD, &value, 方法名称, 0, nullptr);

  VARIANT converted;
  VariantInit(&converted);
  E2CppDateTime result;
  if (SUCCEEDED(VariantChangeType(&converted, &value, 0, VT_DATE))) {
    result = E2CppDateTime::fromOleDate(converted.date);
  }
  VariantClear(&converted);
  VariantClear(&value);
  return result;
}

inline EType_对象 EType_对象::对象型方法(const QString &方法名称, ...) {
  VARIANT value;
  VariantInit(&value);
  InvokeHelper(DISPATCH_METHOD, &value, 方法名称, 0, nullptr);

  EType_对象 result;
  if (value.vt == VT_DISPATCH && value.pdispVal) {
    result.m_pDisp = value.pdispVal;
    result.m_pDisp->AddRef();
  }
  VariantClear(&value);
  return result;
}

inline 逻辑型 EType_对象::创建图片对象(const E2CppByteArray &) { return false; }
inline 逻辑型 EType_对象::创建字体对象(void *) { return false; }
inline E2CppByteArray EType_对象::取回图片() { return E2CppByteArray(); }
inline void *EType_对象::取回字体() { return nullptr; }

inline 文本型 EType_对象::取错误() const { return m_lastError; }

inline void EType_对象::查看() {}

inline bool EType_对象::取接口(void *, const QString &) { return false; }

inline void EType_对象::SetLastError(HRESULT hr) const {
  if (SUCCEEDED(hr)) {
    m_lastError.clear();
    return;
  }
  const _com_error err(hr);
  m_lastError = QString::fromWCharArray(err.ErrorMessage());
}

inline HRESULT EType_对象::InvokeHelper(WORD wFlags, VARIANT *pVarResult,
                                         const QString &name, int argCount,
                                         VARIANT *pArgs) {
  if (!m_pDisp) {
    return E_POINTER;
  }

  BSTR bstrName = ToBSTR(name);
  OLECHAR *oleName = bstrName;
  DISPID dispid = DISPID_UNKNOWN;
  HRESULT hr = m_pDisp->GetIDsOfNames(IID_NULL, &oleName, 1,
                                      GetUserDefaultLCID(), &dispid);
  SysFreeString(bstrName);
  if (FAILED(hr)) {
    SetLastError(hr);
    return hr;
  }

  DISPPARAMS params = {pArgs, nullptr, static_cast<UINT>(argCount), 0};
  DISPID dispidPut = DISPID_PROPERTYPUT;
  if (wFlags & DISPATCH_PROPERTYPUT) {
    params.cNamedArgs = 1;
    params.rgdispidNamedArgs = &dispidPut;
  }

  hr = m_pDisp->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), wFlags, &params,
                       pVarResult, nullptr, nullptr);
  SetLastError(hr);
  return hr;
}

#endif
