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

#ifndef E2CPP_PAINT_WIDGET_CONTEXT_DECLARED
#define E2CPP_PAINT_WIDGET_CONTEXT_DECLARED
extern thread_local QWidget *E2CurrentPaintWidget;
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

    载入窗口Q(E2Res_最近联系人_ui_字节集(), false);
    载入窗口Q(E2Res_聊天框_ui_字节集(), false);
    添加最近联系人2(recentList_最近联系人);
    添加最近联系人2(recentList_最近联系人);
    添加最近联系人2(recentList_最近联系人);
    添加最近联系人2(recentList_最近联系人);
    添加最近联系人2(recentList_最近联系人);
    载入窗口Q(E2Res_主窗口_ui_字节集(), true);
    // ' pushButton_主窗口.置文本 (“主窗口按钮”)
    // 像素图.从数据加载_字节数组 (到字节数组Q (#微信头像), , )
    // 头像图标.构造_QIcon_从像素图 (像素图)
    // 按钮头像_主窗口.置图标 (头像图标)
    // 尺寸.构造_带宽高 (按钮头像_主窗口.取宽度 (), 按钮头像_主窗口.取高度 ())
    // 按钮头像_主窗口.置图标大小 (尺寸)
    // 调试输出 (“啥也不是”)
}

void 添加最近联系人(QListWidget * 最近联系列表框, 字节集& 头像, 文本型& 昵称, 文本型& 最近消息最近消息, 文本型& 时间, 逻辑型 是否未读, 整数型 位置) {
    QWidget *单个联系人 = nullptr;
    QListWidgetItem *列表控件项 = nullptr;
    QSize 尺寸{};

    // ' 单个联系人.调整大小_宽高 (300, 100)
    // 单个联系人.置样式表 (“border: none; outline: none;”)
    // 单个联系人.置内容边距_各边 (0, 0, 0, 0)
    // 单个联系人.置焦点策略 (#Qt_焦点策略_无焦点)
    // 单个联系人.置窗口标志集 (位或 (#Qt_窗口类型_无边框窗口提示, #Qt_窗口类型_工具窗口))
    // 列表控件项.构造函数_列表控件和类型 (最近联系列表框, )
    // 尺寸.构造_带宽高 (300, 100)
    // 列表控件项.置大小提示 (尺寸)
    // 最近联系列表框.置项目部件 (列表控件项, 单个联系人)
    // 单个联系人.显示 ()
}

void 添加最近联系人2(QListWidget * 最近联系列表框) {
    QWidget *最近联系人项ui = nullptr;
    QListWidgetItem *列表控件项 = nullptr;
    QSize 尺寸{};
    QPixmap 像素图{};
    QTime 时间{};
    QFont 局字体{};

    最近联系人项ui = 载入窗口Q(E2Res_最近联系人项_ui_字节集(), false);
    // 单个联系人.调整大小_宽高 (300, 100)
    最近联系人项ui->setStyleSheet("border: none; outline: none;");
    // QLabel {color: #f3f6ff;}
    局字体.setPointSize(18);
    if (label最近联系人昵称_最近联系人项) label最近联系人昵称_最近联系人项->setFont(局字体);
    最近联系人项ui->setContentsMargins(0, 0, 0, 0);
    最近联系人项ui->setFocusPolicy(Qt::NoFocus);
    // 最近联系人ui.置窗口标志集 (位或 (#Qt_窗口类型_无边框窗口提示, #Qt_窗口类型_工具窗口))
    像素图.loadFromData(到字节数组Q(E2Res_微信头像()), _e2_empty, _e2_empty);
    if (label最近联系人头像_最近联系人项) label最近联系人头像_最近联系人项->setPixmap(像素图);
    if (label最近联系人头像_最近联系人项) label最近联系人头像_最近联系人项->setScaledContents(true);
    if (label最近联系人头像_最近联系人项) label最近联系人头像_最近联系人项->setFixedSize(50, 50);
    if (label最近联系人昵称_最近联系人项) label最近联系人昵称_最近联系人项->setText("文件传输助手");
    if (label最近联系人消息_最近联系人项) label最近联系人消息_最近联系人项->setText("您最近传输了一个文件,是否还要继续传输");
    if (label最近联系人消息_最近联系人项) label最近联系人消息_最近联系人项->setStyleSheet("color: #999999;");
    if (label最近联系人时间_最近联系人项) label最近联系人时间_最近联系人项->setStyleSheet("color: #b2b2b2;");
    if (label最近联系人时间_最近联系人项) label最近联系人时间_最近联系人项->setText(时间.currentTime().toString());
    列表控件项 = new QListWidgetItem(最近联系列表框, _e2_empty);
    尺寸 = QSize(300, 80);
    列表控件项->setSizeHint(尺寸);
    最近联系列表框->setItemWidget(列表控件项, 最近联系人项ui);
    最近联系人项ui->show();
}

void 添加消息(QListWidget * 聊天框, 文本型& 消息内容, 逻辑型 本人, 字节集& 头像) {
    QWidget *消息控件 = nullptr;
    QPixmap 像素图{};
    QListWidgetItem *列表控件项 = nullptr;
    QSize 尺寸{};
    QFont 字体_var{};

    
    消息控件 = 载入窗口Q(E2Res_聊天消息_ui_字节集(), false);
    // 置入文本 (“messageLabel_聊天消息->setWordWrap(true);”)
    // 置入文本 (“messageLabel_聊天消息->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);”)
    // 置入文本 (“messageLabel_聊天消息->setTextInteractionFlags(Qt::TextSelectableByMouse);”)
    // 置入文本 (“bubbleFrame_聊天消息->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);”)
    // 置入文本 (“bubbleFrame_聊天消息->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);”)
    // 置入文本 (“messageLabel_聊天消息->setMinimumHeight(0);”)
    // 置入文本 (“messageLabel_聊天消息->setMaximumHeight(QWIDGETSIZE_MAX);”)
    // 置入文本 (“bubbleFrame_聊天消息->setMinimumHeight(0);”)
    // 置入文本 (“bubbleFrame_聊天消息->setMaximumHeight(QWIDGETSIZE_MAX);”)
    字体_var = QFont("Microsoft YaHei", 14, _e2_empty, _e2_empty);
    字体_var.setStyleStrategy(QFont::PreferAntialias);
    if (本人) {
        像素图.loadFromData(到字节数组Q(E2Res_微信头像()), _e2_empty, _e2_empty);
        if (leftArrowLabel_聊天消息) leftArrowLabel_聊天消息->hide();
        if (leftAvatarLabel_聊天消息) leftAvatarLabel_聊天消息->hide();
        if (rightAvatarLabel_聊天消息) rightAvatarLabel_聊天消息->setPixmap(像素图);
        if (messageLabel_聊天消息) messageLabel_聊天消息->setFont(字体_var);
        if (messageLabel_聊天消息) messageLabel_聊天消息->setText(消息内容);
        if (messageLabel_聊天消息) messageLabel_聊天消息->setWordWrap(true);
        消息控件->layout()->update();
        消息控件->adjustSize();
        if (bubbleFrame_聊天消息) bubbleFrame_聊天消息->setStyleSheet("background:#95EC69;border-radius:5px;");
        if (rightArrowLabel_聊天消息) rightArrowLabel_聊天消息->setStyleSheet("color:#95EC69;background:transparent;font-size:18px;");
        列表控件项 = new QListWidgetItem(聊天框, _e2_empty);
        // 尺寸.构造_带宽高 (300, 50)
        聊天框->setSelectionMode(QAbstractItemView::NoSelection);
        聊天框->setItemWidget(列表控件项, 消息控件);
        列表控件项->setSizeHint(消息控件->sizeHint());
    }
    
}

void _主窗口_ui_btnNewChat_按钮点击() {
    // ' 添加最近联系人 (recentList_主窗口)
    // 添加最近联系人2 (recentList_主窗口)
}

void _主窗口_ui_messageInput_插入自MIME数据(QMimeData& 源) {
    // 调试输出 (“包含图像:”, 源.包含图像 ())
    // 调试输出 (“包含图像:”, 源.包含链接 ())
}

void _主窗口_ui_messageInput_按键按下事件(QKeyEvent& 事件) {
    QKeySequence 键序列{};

    // 键序列.构造函数_标准键 (事件.取按键代码 ())
    // 调试输出 (键序列.转字符串 ())
}

void _主窗口_ui_MainWindow_调整大小事件(QResizeEvent& 事件) {
    if (事件.size().width() >= E2Const_桌面宽度  &&  桌面模式) {
        return;
    }
    if (事件.size().width() < E2Const_桌面宽度  &&  手机模式) {
        return;
    }
    if (事件.size().width() >= E2Const_桌面宽度) {
        桌面模式 = true;
        手机模式 = false;
        调试输出("pc");
        if (modeSwitcherStack_主窗口) modeSwitcherStack_主窗口->setCurrentIndex(0);
        if (mainSplitter_主窗口) mainSplitter_主窗口->addWidget(RecentListWidget_最近联系人);
        if (mainSplitter_主窗口) mainSplitter_主窗口->addWidget(ChatPanelWidget_聊天框);
    }
    else if (事件.size().width() < E2Const_桌面宽度) {
        桌面模式 = false;
        手机模式 = true;
        if (modeSwitcherStack_主窗口) modeSwitcherStack_主窗口->setCurrentIndex(1);
        if (mobileStack_主窗口) mobileStack_主窗口->addWidget(RecentListWidget_最近联系人);
        if (mobileStack_主窗口) mobileStack_主窗口->addWidget(ChatPanelWidget_聊天框);
        调试输出("mobile");
    }
}

void _最近联系人_ui_recentList_项目点击(QListWidgetItem& 项目) {
    调试输出(123);
    // 添加消息 (messageList_聊天框, label最近联系人消息_最近联系人项.取文本 (), 真, )
    添加消息(messageList_聊天框, "转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串转字符串", true, _e2_empty);
}

void _聊天框_ui_messageList_绘制事件(QPaintEvent& 事件) {
    QPainter 绘制器(E2CurrentPaintWidget);
    QColor 颜色{};

    // messageList_聊天框.绘制事件 (事件)
    // 绘制器.置渲染提示 (#绘制器_抗锯齿, )
    // 颜色.构造_QColor_从字符串名称 (“#95ec69”)
    // 绘制器.置画刷_颜色对象 (颜色)
    // 绘制器.置画笔_画笔样式 (#Qt_无画笔)
    // 绘制器.绘制椭圆_矩形 (QRect(100,100,110,110))
}


