#pragma once

#include "E2CppBase.h"

#if defined(QT_WIDGETS_LIB)
class QWidget;
class QString;
class QByteArray;
void E2BindLoadedWindowEvents(QWidget *root, const QString &windowName);
using E2ResourceResolver = QByteArray (*)(const QString &name);
using E2UiWidgetFactory = QWidget *(*)(const QString &windowName,
                                       const QString &className,
                                       QWidget *parent,
                                       const QString &name);
void E2SetResourceResolver(E2ResourceResolver resolver);
void E2SetUiWidgetFactory(E2UiWidgetFactory factory);
QWidget *载入窗口Q(const E2CppValue &uiData, bool AutoShow = true);
QWidget *载入窗口Q(const 文本型 &uiText, bool AutoShow = true);
QWidget *载入窗口Q(const 字节集 &uiData, bool AutoShow = true);
#endif

template <typename T>
inline auto \u53d6\u503cQ(const T &value) -> std::enable_if_t<!std::is_pointer_v<T>, T> {
  return value;
}

template <typename T>
inline auto \u53d6\u503cQ(T *value) -> std::remove_cv_t<T> {
  using ValueType = std::remove_cv_t<T>;
  return value ? static_cast<ValueType>(*value) : ValueType{};
}

template <typename T>
inline auto \u53d6\u6307\u9488Q(T &value) -> T * {
  return &value;
}

template <typename T>
inline auto \u53d6\u6307\u9488Q(T *value) -> T * {
  return value;
}
