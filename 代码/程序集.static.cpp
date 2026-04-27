#define _CRT_SECURE_NO_WARNINGS
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#include "../KrnlnLib/KrnlnLib.h"
#include <optional>
#include "程序集.static.h"
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

#ifndef E2CPP_BYTEARRAY_Q_BRIDGES_DEFINED
#define E2CPP_BYTEARRAY_Q_BRIDGES_DEFINED
inline E2CppByteArray 到字节集Q(const QByteArray &ba) { return E2CppByteArray(reinterpret_cast<const std::uint8_t *>(ba.constData()), reinterpret_cast<const std::uint8_t *>(ba.constData()) + ba.size()); }
inline QByteArray 到字节数组Q(const E2CppByteArray &ba) { return QByteArray(reinterpret_cast<const char *>(ba.data()), static_cast<qsizetype>(ba.size())); }
#endif

/**
 * @brief 程序入口
 */
整数型 _启动子程序() {
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
    桌面系统 = true;
#elif defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    移动系统 = true;
#else
    桌面系统 = false;
    移动系统 = false;
#endif
    测试();
    return 0;
}

void 测试() {
    QIcon 头像图标{};
    QPixmap 像素图{};
    QSize 尺寸{};

    载入窗口Q(E2Res_主窗口_ui_字节集(), true);
    // pushButton_主窗口.置文本 (“主窗口按钮”)
    像素图.loadFromData(到字节数组Q(E2Res_微信头像()), _e2_empty, _e2_empty);
    头像图标 = QIcon(像素图);
    if (按钮头像_主窗口) 按钮头像_主窗口->setIcon(头像图标);
    尺寸 = QSize(按钮头像_主窗口->width(), 按钮头像_主窗口->height());
    if (按钮头像_主窗口) 按钮头像_主窗口->setIconSize(尺寸);
    if (桌面系统) {
    }
    else if (移动系统) {
    }
    else {
        调试输出("啥也不是");
    }
}

void 添加最近联系人(QListWidget * 最近联系列表框, 字节集& 头像, 文本型& 昵称, 文本型& 最近消息最近消息, 文本型& 时间, 逻辑型 是否未读, 整数型 位置) {
    QWidget *单个联系人 = nullptr;
    QListWidgetItem *列表控件项 = nullptr;
    QSize 尺寸{};

    // 单个联系人.调整大小_宽高 (300, 100)
    单个联系人->setStyleSheet("border: none; outline: none;");
    单个联系人->setContentsMargins(0, 0, 0, 0);
    单个联系人->setFocusPolicy(Qt::NoFocus);
    单个联系人->setWindowFlags(位或(Qt::FramelessWindowHint, Qt::Tool));
    列表控件项 = new QListWidgetItem(最近联系列表框, _e2_empty);
    尺寸 = QSize(300, 100);
    列表控件项->setSizeHint(尺寸);
    最近联系列表框->setItemWidget(列表控件项, 单个联系人);
    单个联系人->show();
}

void 添加最近联系人2(QListWidget * 最近联系列表框) {
    QWidget *最近联系人ui = nullptr;
    QListWidgetItem *列表控件项 = nullptr;
    QSize 尺寸{};
    QPixmap 像素图{};
    QTime 时间{};

    最近联系人ui = 载入窗口Q(E2Res_最近联系人_ui_字节集(), false);
    // 单个联系人.调整大小_宽高 (300, 100)
    最近联系人ui->setStyleSheet("border: none; outline: none;");
    // QLabel {color: #f3f6ff;}
    最近联系人ui->setContentsMargins(0, 0, 0, 0);
    最近联系人ui->setFocusPolicy(Qt::NoFocus);
    // 最近联系人ui.置窗口标志集 (位或 (#Qt_窗口类型_无边框窗口提示, #Qt_窗口类型_工具窗口))
    像素图.loadFromData(到字节数组Q(E2Res_微信头像()), _e2_empty, _e2_empty);
    if (label最近联系人头像_最近联系人) label最近联系人头像_最近联系人->setPixmap(像素图);
    if (label最近联系人头像_最近联系人) label最近联系人头像_最近联系人->setScaledContents(true);
    if (label最近联系人头像_最近联系人) label最近联系人头像_最近联系人->setFixedSize(50, 50);
    if (label最近联系人昵称_最近联系人) label最近联系人昵称_最近联系人->setText("文件传输助手");
    if (label最近联系人消息_最近联系人) label最近联系人消息_最近联系人->setText("您最近传输了一个文件,是否还要继续传输");
    if (label最近联系人消息_最近联系人) label最近联系人消息_最近联系人->setStyleSheet("color: #999999;");
    if (label最近联系人时间_最近联系人) label最近联系人时间_最近联系人->setStyleSheet("color: #b2b2b2;");
    if (label最近联系人时间_最近联系人) label最近联系人时间_最近联系人->setText(时间.currentTime().toString());
    列表控件项 = new QListWidgetItem(最近联系列表框, _e2_empty);
    尺寸 = QSize(300, 80);
    列表控件项->setSizeHint(尺寸);
    最近联系列表框->setItemWidget(列表控件项, 最近联系人ui);
    最近联系人ui->show();
}

void _主窗口_ui_btnNewChat_按钮点击() {
    // 添加最近联系人 (recentList_主窗口)
    添加最近联系人2(recentList_主窗口);
}

void _主窗口_ui_messageInput_插入自MIME数据(QMimeData& 源) {
    调试输出("包含图像:", 源.hasImage());
    调试输出("包含图像:", 源.hasUrls());
}

void _主窗口_ui_messageInput_按键按下事件(QKeyEvent& 事件) {
    QKeySequence 键序列{};

    键序列 = QKeySequence(事件.key());
    调试输出(键序列.toString());
}


