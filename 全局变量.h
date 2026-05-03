#pragma once
#ifdef QT_WIDGETS_LIB
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QKeySequenceEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDial>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QToolBar>
#include <QtGui/QAction>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QCalendarWidget>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#endif
#include "KrnlnLib/E2CppBase.h"
#include <optional>
#include "自定义类型.h"
#include "常量.h"
#include "资源.h"

#ifdef _WIN32
#include "KrnlnLib/E2CppComObject.h"
#include "KrnlnLib/E2CppVariant.h"
#endif
#include <QtWidgets/QtWidgets>

#ifndef E2CPP_BYTEARRAY_Q_BRIDGES_DEFINED
#define E2CPP_BYTEARRAY_Q_BRIDGES_DEFINED
inline E2CppByteArray 到字节集Q(const QByteArray &ba) { return E2CppByteArray(reinterpret_cast<const std::uint8_t *>(ba.constData()), reinterpret_cast<const std::uint8_t *>(ba.constData()) + ba.size()); }
inline QByteArray 到字节数组Q(const E2CppByteArray &ba) { return QByteArray(reinterpret_cast<const char *>(ba.data()), static_cast<qsizetype>(ba.size())); }
#endif

inline 逻辑型 移动系统{};
// [UI控件] QWidget
inline QWidget *MainWindow_主窗口 = nullptr;
// [UI控件] QVBoxLayout
inline QVBoxLayout *verticalLayout_主窗口 = nullptr;
// [UI控件] QStackedWidget
inline QStackedWidget *modeSwitcherStack_主窗口 = nullptr;
// [UI控件] QHBoxLayout
inline QHBoxLayout *horizontalLayoutPc_主窗口 = nullptr;
// [UI控件] QFrame
inline QFrame *leftRail_主窗口 = nullptr;
// [UI控件] QVBoxLayout
inline QVBoxLayout *verticalLayoutRail_主窗口 = nullptr;
// [UI控件] QToolButton
inline QToolButton *按钮头像_主窗口 = nullptr;
// [UI控件] QToolButton
inline QToolButton *消息按钮_主窗口 = nullptr;
// [UI控件] QToolButton
inline QToolButton *联系人按钮_主窗口 = nullptr;
// [UI控件] QToolButton
inline QToolButton *收藏按钮_主窗口 = nullptr;
// [UI控件] QToolButton
inline QToolButton *朋友圈按钮_主窗口 = nullptr;
// [UI控件] QToolButton
inline QToolButton *视频号按钮_主窗口 = nullptr;
// [UI控件] QToolButton
inline QToolButton *搜一搜按钮_主窗口 = nullptr;
// [UI控件] QToolButton
inline QToolButton *小程序按钮_主窗口 = nullptr;
// [UI控件] QToolButton
inline QToolButton *手机按钮_主窗口 = nullptr;
// [UI控件] QToolButton
inline QToolButton *更多按钮_主窗口 = nullptr;
// [UI控件] QSplitter
inline QSplitter *mainSplitter_主窗口 = nullptr;
// [UI控件] QVBoxLayout
inline QVBoxLayout *verticalLayoutMobile_主窗口 = nullptr;
// [UI控件] QStackedWidget
inline QStackedWidget *mobileStack_主窗口 = nullptr;
// [UI控件] QWidget
inline QWidget *ChatPanelWidget_聊天框 = nullptr;
// [UI控件] QVBoxLayout
inline QVBoxLayout *verticalLayoutChatPanel_聊天框 = nullptr;
// [UI控件] QFrame
inline QFrame *chatHeader_聊天框 = nullptr;
// [UI控件] QHBoxLayout
inline QHBoxLayout *horizontalLayoutChatHeader_聊天框 = nullptr;
// [UI控件] QVBoxLayout
inline QVBoxLayout *verticalLayoutChatTitle_聊天框 = nullptr;
// [UI控件] QLabel
inline QLabel *contactNameLabel_聊天框 = nullptr;
// [UI控件] QLabel
inline QLabel *contactSubLabel_聊天框 = nullptr;
// [UI控件] QToolButton
inline QToolButton *btnMore_聊天框 = nullptr;
// [UI控件] QFrame
inline QFrame *lineH2_聊天框 = nullptr;
// [UI控件] QSplitter
inline QSplitter *chatVerticalSplitter_聊天框 = nullptr;
// [UI控件] QListWidget
inline QListWidget *messageList_聊天框 = nullptr;
// [UI控件] QFrame
inline QFrame *chatComposer_聊天框 = nullptr;
// [UI控件] QVBoxLayout
inline QVBoxLayout *verticalLayoutComposer_聊天框 = nullptr;
// [UI控件] QHBoxLayout
inline QHBoxLayout *horizontalLayoutComposerTools_聊天框 = nullptr;
// [UI控件] QToolButton
inline QToolButton *btnEmoji_聊天框 = nullptr;
// [UI控件] QToolButton
inline QToolButton *btnfavorite_聊天框 = nullptr;
// [UI控件] QToolButton
inline QToolButton *btnFolder_聊天框 = nullptr;
// [UI控件] QToolButton
inline QToolButton *btnCut_聊天框 = nullptr;
// [UI控件] QToolButton
inline QToolButton *btnVoice_聊天框 = nullptr;
// [UI控件] QLabel
inline QLabel *hintLabel_聊天框 = nullptr;
// [UI控件] QTextEdit
inline QTextEdit *messageInput_聊天框 = nullptr;
// [UI控件] QHBoxLayout
inline QHBoxLayout *horizontalLayoutSendRow_聊天框 = nullptr;
// [UI控件] QPushButton
inline QPushButton *sendButton_聊天框 = nullptr;
// [UI控件] QWidget
inline QWidget *RecentListWidget_最近联系人 = nullptr;
// [UI控件] QVBoxLayout
inline QVBoxLayout *verticalLayoutSidebar_最近联系人 = nullptr;
// [UI控件] QFrame
inline QFrame *recentHeader_最近联系人 = nullptr;
// [UI控件] QHBoxLayout
inline QHBoxLayout *horizontalLayoutRecentHeader_最近联系人 = nullptr;
// [UI控件] QLineEdit
inline QLineEdit *searchEdit_最近联系人 = nullptr;
// [UI控件] QToolButton
inline QToolButton *btnNewChat_最近联系人 = nullptr;
// [UI控件] QFrame
inline QFrame *lineH1_最近联系人 = nullptr;
// [UI控件] QListWidget
inline QListWidget *recentList_最近联系人 = nullptr;
// [UI控件] QWidget
inline QWidget *ChatMessageItem_聊天消息 = nullptr;
// [UI控件] QHBoxLayout
inline QHBoxLayout *rootHorizontalLayout_聊天消息 = nullptr;
// [UI控件] QLabel
inline QLabel *leftAvatarLabel_聊天消息 = nullptr;
// [UI控件] QLabel
inline QLabel *leftArrowLabel_聊天消息 = nullptr;
// [UI控件] QFrame
inline QFrame *bubbleFrame_聊天消息 = nullptr;
// [UI控件] QHBoxLayout
inline QHBoxLayout *bubbleLayout_聊天消息 = nullptr;
// [UI控件] QLabel
inline QLabel *messageLabel_聊天消息 = nullptr;
// [UI控件] QLabel
inline QLabel *rightArrowLabel_聊天消息 = nullptr;
// [UI控件] QLabel
inline QLabel *rightAvatarLabel_聊天消息 = nullptr;
// [UI控件] QWidget
inline QWidget *Form_最近联系人项 = nullptr;
// [UI控件] QHBoxLayout
inline QHBoxLayout *horizontalLayout_最近联系人项 = nullptr;
// [UI控件] QLabel
inline QLabel *label最近联系人头像_最近联系人项 = nullptr;
// [UI控件] QVBoxLayout
inline QVBoxLayout *verticalLayout_最近联系人项 = nullptr;
// [UI控件] QHBoxLayout
inline QHBoxLayout *horizontalLayout_2_最近联系人项 = nullptr;
// [UI控件] QLabel
inline QLabel *label最近联系人昵称_最近联系人项 = nullptr;
// [UI控件] QLabel
inline QLabel *label最近联系人时间_最近联系人项 = nullptr;
// [UI控件] QLabel
inline QLabel *label最近联系人消息_最近联系人项 = nullptr;
inline 逻辑型 桌面系统{};
inline 逻辑型 桌面模式{};
inline 逻辑型 手机模式{};
