#pragma once

#include "E2CppBase.h"
#include <type_traits>

#ifdef _WIN32
#include <ocidl.h>
#include <ole2.h>
#include <windows.h>
#endif

class EType_变体型;
#ifndef QT_CORE_LIB
using 变体型 = EType_变体型;
#endif

class EType_对象;
#ifndef QT_CORE_LIB
using 对象 = EType_对象;
#endif

#ifdef _WIN32
class EType_对象 {
public:
  EType_对象();
  ~EType_对象();

  EType_对象(const EType_对象 &other);
  EType_对象 &operator=(const EType_对象 &other);

  bool 创建(const QString &对象类型, const QString &类型库文件名 = QString());
  bool 获取(const QString &对象类型);
  void 清除();
  bool 是否为空() const;
  bool 是否相等(const EType_对象 &欲检查的对象) const;

  文本型 读文本属性(const QString &属性名称, ...);
  双精度小数型 读数值属性(const QString &属性名称, ...);
  逻辑型 读逻辑属性(const QString &属性名称, ...);
  E2CppDateTime 读日期属性(const QString &属性名称, ...);
  EType_对象 读对象型属性(const QString &属性名称, ...);
  EType_变体型 读属性(const QString &属性名称, ...);

  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  文本型 读文本属性(const QString &属性名称, Args &&...) {
    return 读文本属性(属性名称);
  }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  双精度小数型 读数值属性(const QString &属性名称, Args &&...) {
    return 读数值属性(属性名称);
  }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  逻辑型 读逻辑属性(const QString &属性名称, Args &&...) {
    return 读逻辑属性(属性名称);
  }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  E2CppDateTime 读日期属性(const QString &属性名称, Args &&...) {
    return 读日期属性(属性名称);
  }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  EType_对象 读对象型属性(const QString &属性名称, Args &&...) {
    return 读对象型属性(属性名称);
  }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  EType_变体型 读属性(const QString &属性名称, Args &&...) {
    return 读属性(属性名称);
  }

  逻辑型 写属性(const QString &属性名称, ...);
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  逻辑型 写属性(const QString &属性名称, Args &&...) {
    return 写属性(属性名称);
  }

  void 方法(const QString &方法名称, ...);
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  void 方法(const QString &方法名称, Args &&...) {
    方法(方法名称);
  }

  EType_变体型 通用方法(const QString &方法名称, ...);
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  EType_变体型 通用方法(const QString &方法名称, Args &&...) {
    return 通用方法(方法名称);
  }

  文本型 文本方法(const QString &方法名称, ...);
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  文本型 文本方法(const QString &方法名称, Args &&...) {
    return 文本方法(方法名称);
  }

  双精度小数型 数值方法(const QString &方法名称, ...);
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  双精度小数型 数值方法(const QString &方法名称, Args &&...) {
    return 数值方法(方法名称);
  }

  逻辑型 逻辑方法(const QString &方法名称, ...);
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  逻辑型 逻辑方法(const QString &方法名称, Args &&...) {
    return 逻辑方法(方法名称);
  }

  E2CppDateTime 日期方法(const QString &方法名称, ...);
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  E2CppDateTime 日期方法(const QString &方法名称, Args &&...) {
    return 日期方法(方法名称);
  }

  EType_对象 对象型方法(const QString &方法名称, ...);
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  EType_对象 对象型方法(const QString &方法名称, Args &&...) {
    return 对象型方法(方法名称);
  }

  逻辑型 创建图片对象(const E2CppByteArray &图片数据);
  逻辑型 创建字体对象(void *字体数据);
  E2CppByteArray 取回图片();
  void *取回字体();

  文本型 取错误() const;
  void 查看();
  逻辑型 取接口(void *对象或窗口组件, const QString &接口标志符 = QString());
  void *取原始接口() const { return m_pDisp; }

private:
  IDispatch *m_pDisp = nullptr;
  mutable QString m_lastError;

  void SetLastError(HRESULT hr) const;
  HRESULT InvokeHelper(WORD wFlags, VARIANT *pVarResult,
                       const QString &name, int argCount, VARIANT *pArgs);
};
#else
#  if defined(Q_OS_ANDROID)
#    warning "对象 (ComObject) 类型: Android 不支持 COM，对象型命令将返回默认值"
#  endif
class EType_对象 {
public:
  EType_对象() = default;
  ~EType_对象() = default;
  EType_对象(const EType_对象 &) = default;
  EType_对象 &operator=(const EType_对象 &) = default;

  bool 创建(const QString &, const QString & = QString()) { return false; }
  bool 获取(const QString &) { return false; }
  void 清除() {}
  bool 是否为空() const { return true; }
  bool 是否相等(const EType_对象 &) const { return false; }

  文本型 读文本属性(const QString &, ...) { return {}; }
  双精度小数型 读数值属性(const QString &, ...) { return 0.0; }
  逻辑型 读逻辑属性(const QString &, ...) { return false; }
  E2CppDateTime 读日期属性(const QString &, ...) { return {}; }
  EType_对象 读对象型属性(const QString &, ...) { return {}; }
  EType_变体型 读属性(const QString &, ...);

  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  文本型 读文本属性(const QString &属性名称, Args &&...) {
    return 读文本属性(属性名称);
  }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  双精度小数型 读数值属性(const QString &属性名称, Args &&...) {
    return 读数值属性(属性名称);
  }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  逻辑型 读逻辑属性(const QString &属性名称, Args &&...) {
    return 读逻辑属性(属性名称);
  }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  E2CppDateTime 读日期属性(const QString &属性名称, Args &&...) {
    return 读日期属性(属性名称);
  }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  EType_对象 读对象型属性(const QString &属性名称, Args &&...) {
    return 读对象型属性(属性名称);
  }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  EType_变体型 读属性(const QString &, Args &&...);

  逻辑型 写属性(const QString &, ...) { return false; }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  逻辑型 写属性(const QString &属性名称, Args &&...) {
    return 写属性(属性名称);
  }

  void 方法(const QString &, ...) {}
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  void 方法(const QString &方法名称, Args &&...) {
    方法(方法名称);
  }

  EType_变体型 通用方法(const QString &, ...);
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  EType_变体型 通用方法(const QString &, Args &&...);

  文本型 文本方法(const QString &, ...) { return {}; }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  文本型 文本方法(const QString &方法名称, Args &&...) {
    return 文本方法(方法名称);
  }

  双精度小数型 数值方法(const QString &, ...) { return 0.0; }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  双精度小数型 数值方法(const QString &方法名称, Args &&...) {
    return 数值方法(方法名称);
  }

  逻辑型 逻辑方法(const QString &, ...) { return false; }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  逻辑型 逻辑方法(const QString &方法名称, Args &&...) {
    return 逻辑方法(方法名称);
  }

  E2CppDateTime 日期方法(const QString &, ...) { return {}; }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  E2CppDateTime 日期方法(const QString &方法名称, Args &&...) {
    return 日期方法(方法名称);
  }

  EType_对象 对象型方法(const QString &, ...) { return {}; }
  template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 0)>>
  EType_对象 对象型方法(const QString &方法名称, Args &&...) {
    return 对象型方法(方法名称);
  }

  逻辑型 创建图片对象(const E2CppByteArray &) { return false; }
  逻辑型 创建字体对象(void *) { return false; }
  E2CppByteArray 取回图片() { return {}; }
  void *取回字体() { return nullptr; }

  文本型 取错误() const { return QStringLiteral("ComObject is not supported on this platform."); }
  void 查看() {}
  逻辑型 取接口(void *, const QString & = QString()) { return false; }
  void *取原始接口() const { return nullptr; }
};
#endif
