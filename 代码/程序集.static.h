#pragma once
#include "../KrnlnLib/KrnlnLib.h"
#include <optional>
#include "../自定义类型.h"
#include "../常量.h"
#include "../资源.h"
#include "../全局变量.h"

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

#ifndef E2CPP_BYTEARRAY_Q_BRIDGES_DEFINED
#define E2CPP_BYTEARRAY_Q_BRIDGES_DEFINED
inline E2CppByteArray 到字节集Q(const QByteArray &ba) { return E2CppByteArray(reinterpret_cast<const std::uint8_t *>(ba.constData()), reinterpret_cast<const std::uint8_t *>(ba.constData()) + ba.size()); }
inline QByteArray 到字节数组Q(const E2CppByteArray &ba) { return QByteArray(reinterpret_cast<const char *>(ba.data()), static_cast<qsizetype>(ba.size())); }
#endif

整数型 _启动子程序();
void 测试();
void 添加最近联系人(QListWidget * 最近联系列表框, 字节集& 头像 = _e2_empty, 文本型& 昵称 = _e2_empty, 文本型& 最近消息最近消息 = _e2_empty, 文本型& 时间 = _e2_empty, 逻辑型 是否未读 = _e2_empty, 整数型 位置 = _e2_empty);
template <typename BridgeArg2 = 字节集&, typename BridgeArg3 = 文本型&, typename BridgeArg4 = 文本型&, typename BridgeArg5 = 文本型&, typename = std::enable_if_t<(std::is_convertible_v<BridgeArg2, 字节集> && std::is_convertible_v<BridgeArg3, 文本型> && std::is_convertible_v<BridgeArg4, 文本型> && std::is_convertible_v<BridgeArg5, 文本型>) && (!(std::is_lvalue_reference_v<BridgeArg2> && std::is_same_v<std::remove_reference_t<BridgeArg2>, 字节集>) || !(std::is_lvalue_reference_v<BridgeArg3> && std::is_same_v<std::remove_reference_t<BridgeArg3>, 文本型>) || !(std::is_lvalue_reference_v<BridgeArg4> && std::is_same_v<std::remove_reference_t<BridgeArg4>, 文本型>) || !(std::is_lvalue_reference_v<BridgeArg5> && std::is_same_v<std::remove_reference_t<BridgeArg5>, 文本型>))>>
inline void 添加最近联系人(QListWidget * 最近联系列表框, BridgeArg2&& __e2_bridge_arg_2 = _e2_empty, BridgeArg3&& __e2_bridge_arg_3 = _e2_empty, BridgeArg4&& __e2_bridge_arg_4 = _e2_empty, BridgeArg5&& __e2_bridge_arg_5 = _e2_empty, 逻辑型 是否未读 = _e2_empty, 整数型 位置 = _e2_empty) {
    字节集 __e2_bridge_头像 = static_cast<字节集>(__e2_bridge_arg_2);
    文本型 __e2_bridge_昵称 = static_cast<文本型>(__e2_bridge_arg_3);
    文本型 __e2_bridge_最近消息最近消息 = static_cast<文本型>(__e2_bridge_arg_4);
    文本型 __e2_bridge_时间 = static_cast<文本型>(__e2_bridge_arg_5);
    添加最近联系人(最近联系列表框, __e2_bridge_头像, __e2_bridge_昵称, __e2_bridge_最近消息最近消息, __e2_bridge_时间, 是否未读, 位置);
}
void 添加最近联系人2(QListWidget * 最近联系列表框);
void _主窗口_ui_btnNewChat_按钮点击();
void _主窗口_ui_messageInput_插入自MIME数据(QMimeData& 源);
template <typename BridgeArg1 = QMimeData&, typename = std::enable_if_t<(std::is_convertible_v<BridgeArg1, QMimeData>) && (!(std::is_lvalue_reference_v<BridgeArg1> && std::is_same_v<std::remove_reference_t<BridgeArg1>, QMimeData>))>>
inline void _主窗口_ui_messageInput_插入自MIME数据(BridgeArg1&& __e2_bridge_arg_1) {
    QMimeData __e2_bridge_源 = static_cast<QMimeData>(__e2_bridge_arg_1);
    _主窗口_ui_messageInput_插入自MIME数据(__e2_bridge_源);
}
void _主窗口_ui_messageInput_按键按下事件(QKeyEvent& 事件);
template <typename BridgeArg1 = QKeyEvent&, typename = std::enable_if_t<(std::is_convertible_v<BridgeArg1, QKeyEvent>) && (!(std::is_lvalue_reference_v<BridgeArg1> && std::is_same_v<std::remove_reference_t<BridgeArg1>, QKeyEvent>))>>
inline void _主窗口_ui_messageInput_按键按下事件(BridgeArg1&& __e2_bridge_arg_1) {
    QKeyEvent __e2_bridge_事件 = static_cast<QKeyEvent>(__e2_bridge_arg_1);
    _主窗口_ui_messageInput_按键按下事件(__e2_bridge_事件);
}

