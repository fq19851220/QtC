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
template <typename BridgeArg2 = E2EmptyParam, typename BridgeArg3 = E2EmptyParam, typename BridgeArg4 = E2EmptyParam, typename BridgeArg5 = E2EmptyParam, typename = std::enable_if_t<((std::is_convertible_v<BridgeArg2, 字节集> || std::is_same_v<std::remove_cv_t<std::remove_reference_t<BridgeArg2>>, E2EmptyParam>) && (std::is_convertible_v<BridgeArg3, 文本型> || std::is_same_v<std::remove_cv_t<std::remove_reference_t<BridgeArg3>>, E2EmptyParam>) && (std::is_convertible_v<BridgeArg4, 文本型> || std::is_same_v<std::remove_cv_t<std::remove_reference_t<BridgeArg4>>, E2EmptyParam>) && (std::is_convertible_v<BridgeArg5, 文本型> || std::is_same_v<std::remove_cv_t<std::remove_reference_t<BridgeArg5>>, E2EmptyParam>)) && (!(std::is_lvalue_reference_v<BridgeArg2> && std::is_same_v<std::remove_reference_t<BridgeArg2>, 字节集>) || !(std::is_lvalue_reference_v<BridgeArg3> && std::is_same_v<std::remove_reference_t<BridgeArg3>, 文本型>) || !(std::is_lvalue_reference_v<BridgeArg4> && std::is_same_v<std::remove_reference_t<BridgeArg4>, 文本型>) || !(std::is_lvalue_reference_v<BridgeArg5> && std::is_same_v<std::remove_reference_t<BridgeArg5>, 文本型>))>>
inline void 添加最近联系人(QListWidget * 最近联系列表框, BridgeArg2&& __e2_bridge_arg_2 = _e2_empty, BridgeArg3&& __e2_bridge_arg_3 = _e2_empty, BridgeArg4&& __e2_bridge_arg_4 = _e2_empty, BridgeArg5&& __e2_bridge_arg_5 = _e2_empty, 逻辑型 是否未读 = _e2_empty, 整数型 位置 = _e2_empty) {
    字节集 __e2_bridge_头像{};
    if constexpr (!std::is_same_v<std::remove_cv_t<std::remove_reference_t<BridgeArg2>>, E2EmptyParam>) {
        __e2_bridge_头像 = static_cast<字节集>(__e2_bridge_arg_2);
    }
    文本型 __e2_bridge_昵称{};
    if constexpr (!std::is_same_v<std::remove_cv_t<std::remove_reference_t<BridgeArg3>>, E2EmptyParam>) {
        __e2_bridge_昵称 = static_cast<文本型>(__e2_bridge_arg_3);
    }
    文本型 __e2_bridge_最近消息最近消息{};
    if constexpr (!std::is_same_v<std::remove_cv_t<std::remove_reference_t<BridgeArg4>>, E2EmptyParam>) {
        __e2_bridge_最近消息最近消息 = static_cast<文本型>(__e2_bridge_arg_4);
    }
    文本型 __e2_bridge_时间{};
    if constexpr (!std::is_same_v<std::remove_cv_t<std::remove_reference_t<BridgeArg5>>, E2EmptyParam>) {
        __e2_bridge_时间 = static_cast<文本型>(__e2_bridge_arg_5);
    }
    添加最近联系人(最近联系列表框, __e2_bridge_头像, __e2_bridge_昵称, __e2_bridge_最近消息最近消息, __e2_bridge_时间, 是否未读, 位置);
}
void 添加最近联系人2(QListWidget * 最近联系列表框);
void 添加消息(QListWidget * 聊天框, 文本型& 消息内容, 逻辑型 本人, 字节集& 头像 = _e2_empty);
template <typename BridgeArg2 = 文本型&, typename BridgeArg4 = E2EmptyParam, typename = std::enable_if_t<(std::is_convertible_v<BridgeArg2, 文本型> && (std::is_convertible_v<BridgeArg4, 字节集> || std::is_same_v<std::remove_cv_t<std::remove_reference_t<BridgeArg4>>, E2EmptyParam>)) && (!(std::is_lvalue_reference_v<BridgeArg2> && std::is_same_v<std::remove_reference_t<BridgeArg2>, 文本型>) || !(std::is_lvalue_reference_v<BridgeArg4> && std::is_same_v<std::remove_reference_t<BridgeArg4>, 字节集>))>>
inline void 添加消息(QListWidget * 聊天框, BridgeArg2&& __e2_bridge_arg_2, 逻辑型 本人, BridgeArg4&& __e2_bridge_arg_4 = _e2_empty) {
    文本型 __e2_bridge_消息内容 = static_cast<文本型>(__e2_bridge_arg_2);
    字节集 __e2_bridge_头像{};
    if constexpr (!std::is_same_v<std::remove_cv_t<std::remove_reference_t<BridgeArg4>>, E2EmptyParam>) {
        __e2_bridge_头像 = static_cast<字节集>(__e2_bridge_arg_4);
    }
    添加消息(聊天框, __e2_bridge_消息内容, 本人, __e2_bridge_头像);
}
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
void _主窗口_ui_MainWindow_调整大小事件(QResizeEvent& 事件);
template <typename BridgeArg1 = QResizeEvent&, typename = std::enable_if_t<(std::is_convertible_v<BridgeArg1, QResizeEvent>) && (!(std::is_lvalue_reference_v<BridgeArg1> && std::is_same_v<std::remove_reference_t<BridgeArg1>, QResizeEvent>))>>
inline void _主窗口_ui_MainWindow_调整大小事件(BridgeArg1&& __e2_bridge_arg_1) {
    QResizeEvent __e2_bridge_事件 = static_cast<QResizeEvent>(__e2_bridge_arg_1);
    _主窗口_ui_MainWindow_调整大小事件(__e2_bridge_事件);
}
void _最近联系人_ui_recentList_项目点击(QListWidgetItem& 项目);
template <typename BridgeArg1 = QListWidgetItem&, typename = std::enable_if_t<(std::is_convertible_v<BridgeArg1, QListWidgetItem>) && (!(std::is_lvalue_reference_v<BridgeArg1> && std::is_same_v<std::remove_reference_t<BridgeArg1>, QListWidgetItem>))>>
inline void _最近联系人_ui_recentList_项目点击(BridgeArg1&& __e2_bridge_arg_1) {
    QListWidgetItem __e2_bridge_项目 = static_cast<QListWidgetItem>(__e2_bridge_arg_1);
    _最近联系人_ui_recentList_项目点击(__e2_bridge_项目);
}
void _聊天框_ui_messageList_绘制事件(QPaintEvent& 事件);
template <typename BridgeArg1 = QPaintEvent&, typename = std::enable_if_t<(std::is_convertible_v<BridgeArg1, QPaintEvent>) && (!(std::is_lvalue_reference_v<BridgeArg1> && std::is_same_v<std::remove_reference_t<BridgeArg1>, QPaintEvent>))>>
inline void _聊天框_ui_messageList_绘制事件(BridgeArg1&& __e2_bridge_arg_1) {
    QPaintEvent __e2_bridge_事件 = static_cast<QPaintEvent>(__e2_bridge_arg_1);
    _聊天框_ui_messageList_绘制事件(__e2_bridge_事件);
}

