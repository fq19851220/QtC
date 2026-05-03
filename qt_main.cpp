#include <QApplication>
#include <QByteArray>
#include <QLibraryInfo>
#include <QLocale>
#include <QEvent>
#include <QIcon>
#include <QCloseEvent>
#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEnterEvent>
#include <QFocusEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QMoveEvent>
#include <QObject>
#include <QPaintEvent>
#include <QPixmap>
#include <QPointer>
#include <QResizeEvent>
#include <QShowEvent>
#include <QStyleFactory>
#include <QTranslator>
#include <QWheelEvent>
#include <QWidget>
#include <QTextEdit>
#include <QWidget>
#include <QFrame>
#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QListWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QToolButton>
#undef signals
#undef slots
#undef emit
#include "全局变量.h"
#include "代码/程序集.static.h"
#include <functional>
#include <utility>

#ifdef _WIN32
#include <windows.h>
static void e2WriteStdout(const char *data, int len) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h && h != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteFile(h, data, (DWORD)len, &written, nullptr);
    }
}
#else
#include <unistd.h>
static void e2WriteStdout(const char *data, int len) {
    (void)::write(1, data, (size_t)len);
}
#endif

static void e2QtMsgHandler(QtMsgType, const QMessageLogContext &, const QString &msg) {
    const QByteArray utf8 = msg.toUtf8();
    e2WriteStdout(utf8.constData(), utf8.size());
    e2WriteStdout("\n", 1);
#ifdef _WIN32
    OutputDebugStringW((msg + QChar('\n')).toStdWString().c_str());
#endif
}

static bool e2IsWidgetChangeEventType(QEvent::Type type) {
    switch (type) {
    case QEvent::ActivationChange:
    case QEvent::EnabledChange:
    case QEvent::FontChange:
    case QEvent::StyleChange:
    case QEvent::PaletteChange:
    case QEvent::ThemeChange:
    case QEvent::LanguageChange:
    case QEvent::LayoutDirectionChange:
    case QEvent::ReadOnlyChange:
    case QEvent::WindowStateChange:
        return true;
    default:
        return false;
    }
}

static QPixmap E2PixmapFromResourceBytes(const E2CppByteArray &bytes) {
    QPixmap pixmap;
    pixmap.loadFromData(到字节数组Q(bytes));
    return pixmap;
}

thread_local QWidget *E2CurrentPaintWidget = nullptr;

class E2ScopedPaintWidget final {
public:
    explicit E2ScopedPaintWidget(QWidget *widget)
        : m_previous(E2CurrentPaintWidget) { E2CurrentPaintWidget = widget; }
    ~E2ScopedPaintWidget() { E2CurrentPaintWidget = m_previous; }
private:
    QWidget *m_previous = nullptr;
};

class E2Wrapped_QWidget final : public QWidget {
public:
    explicit E2Wrapped_QWidget(const QString &windowName, QWidget *parent = nullptr)
        : QWidget(parent), m_windowName(windowName) {}

protected:
    void resizeEvent(QResizeEvent * event) override {
        if (event) {
            if ((m_windowName == QStringLiteral("MainWindow") || m_windowName == QStringLiteral("主窗口_ui")) && objectName() == QStringLiteral("MainWindow")) {
            _主窗口_ui_MainWindow_调整大小事件(*event);
            return;
            }
        }
        QWidget::resizeEvent(event);
    }

private:
    QString m_windowName;
};

class E2Wrapped_QListWidget final : public QListWidget {
public:
    explicit E2Wrapped_QListWidget(const QString &windowName, QWidget *parent = nullptr)
        : QListWidget(parent), m_windowName(windowName) {}

protected:
    void paintEvent(QPaintEvent * e) override {
        QListWidget::paintEvent(e);
        if (!e)
            return;
        QWidget *__e2PaintTarget = this;
        if (auto *__e2ScrollArea = qobject_cast<QAbstractScrollArea*>(this))
            __e2PaintTarget = __e2ScrollArea->viewport();
        E2ScopedPaintWidget __e2PaintWidget(__e2PaintTarget);
        if ((m_windowName == QStringLiteral("ChatPanelWidget") || m_windowName == QStringLiteral("聊天框_ui")) && objectName() == QStringLiteral("messageList")) {
            _聊天框_ui_messageList_绘制事件(*e);
        }
        return;
    }

private:
    QString m_windowName;
};

static QWidget *E2CreateWrappedWidget(const QString &windowName,
                                   const QString &className,
                                   QWidget *parent,
                                   const QString &name) {
    Q_UNUSED(name);
    if (className == QStringLiteral("QWidget"))
        return new E2Wrapped_QWidget(windowName, parent);
    if (className == QStringLiteral("QListWidget"))
        return new E2Wrapped_QListWidget(windowName, parent);
    return nullptr;
}

class E2WidgetEventFilter final : public QObject {
public:
    E2WidgetEventFilter(QWidget *root, const QString &windowName)
        : QObject(root), m_windowName(windowName) {}

protected:
    bool eventFilter(QObject *watched, QEvent *event) override {
        if (watched == nullptr || event == nullptr)
            return QObject::eventFilter(watched, event);
        const QString objectName = watched->objectName();
        if (m_windowName == QStringLiteral("MainWindow") || m_windowName == QStringLiteral("主窗口_ui")) {
            if (objectName == QStringLiteral("MainWindow") && event->type() == QEvent::Resize) {
                _主窗口_ui_MainWindow_调整大小事件(*static_cast<QResizeEvent*>(event));
            }
        }
        if (m_windowName == QStringLiteral("RecentListWidget") || m_windowName == QStringLiteral("最近联系人_ui")) {
        }
        if (m_windowName == QStringLiteral("Form") || m_windowName == QStringLiteral("最近联系人项_ui")) {
        }
        if (m_windowName == QStringLiteral("ChatPanelWidget") || m_windowName == QStringLiteral("聊天框_ui")) {
        }
        if (m_windowName == QStringLiteral("ChatMessageItem") || m_windowName == QStringLiteral("聊天消息_ui")) {
        }
        return QObject::eventFilter(watched, event);
    }

private:
    QString m_windowName;
};

void E2BindLoadedWindowEvents(QWidget *root, const QString &windowName) {
    if (!root)
        return;
    if (windowName == QStringLiteral("MainWindow") || windowName == QStringLiteral("主窗口_ui")) {
        MainWindow_主窗口 = qobject_cast<QWidget*>(root);
        verticalLayout_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayout") );
        modeSwitcherStack_主窗口 = root->findChild<QStackedWidget*>( QStringLiteral("modeSwitcherStack") );
        horizontalLayoutPc_主窗口 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayoutPc") );
        leftRail_主窗口 = root->findChild<QFrame*>( QStringLiteral("leftRail") );
        verticalLayoutRail_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutRail") );
        按钮头像_主窗口 = root->findChild<QToolButton*>( QStringLiteral("按钮头像") );
        消息按钮_主窗口 = root->findChild<QToolButton*>( QStringLiteral("消息按钮") );
        联系人按钮_主窗口 = root->findChild<QToolButton*>( QStringLiteral("联系人按钮") );
        收藏按钮_主窗口 = root->findChild<QToolButton*>( QStringLiteral("收藏按钮") );
        朋友圈按钮_主窗口 = root->findChild<QToolButton*>( QStringLiteral("朋友圈按钮") );
        视频号按钮_主窗口 = root->findChild<QToolButton*>( QStringLiteral("视频号按钮") );
        搜一搜按钮_主窗口 = root->findChild<QToolButton*>( QStringLiteral("搜一搜按钮") );
        小程序按钮_主窗口 = root->findChild<QToolButton*>( QStringLiteral("小程序按钮") );
        手机按钮_主窗口 = root->findChild<QToolButton*>( QStringLiteral("手机按钮") );
        更多按钮_主窗口 = root->findChild<QToolButton*>( QStringLiteral("更多按钮") );
        mainSplitter_主窗口 = root->findChild<QSplitter*>( QStringLiteral("mainSplitter") );
        verticalLayoutMobile_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutMobile") );
        mobileStack_主窗口 = root->findChild<QStackedWidget*>( QStringLiteral("mobileStack") );
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("按钮头像") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信头像());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("消息按钮") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信消息());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("联系人按钮") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信联系人());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("收藏按钮") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信收藏());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("朋友圈按钮") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信朋友圈());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("视频号按钮") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信视频号());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("搜一搜按钮") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信搜一搜());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("小程序按钮") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信小程序());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("手机按钮") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信手机());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("更多按钮") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信更多());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
    }
    if (windowName == QStringLiteral("RecentListWidget") || windowName == QStringLiteral("最近联系人_ui")) {
        RecentListWidget_最近联系人 = qobject_cast<QWidget*>(root);
        verticalLayoutSidebar_最近联系人 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutSidebar") );
        recentHeader_最近联系人 = root->findChild<QFrame*>( QStringLiteral("recentHeader") );
        horizontalLayoutRecentHeader_最近联系人 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayoutRecentHeader") );
        searchEdit_最近联系人 = root->findChild<QLineEdit*>( QStringLiteral("searchEdit") );
        btnNewChat_最近联系人 = root->findChild<QToolButton*>( QStringLiteral("btnNewChat") );
        lineH1_最近联系人 = root->findChild<QFrame*>( QStringLiteral("lineH1") );
        recentList_最近联系人 = root->findChild<QListWidget*>( QStringLiteral("recentList") );
        if (auto *sender = root->findChild<QListWidget*>( QStringLiteral("recentList") )) {
            QObject::connect(sender, qOverload<QListWidgetItem*>(&QListWidget::itemClicked), root, [](QListWidgetItem* __arg0) { _最近联系人_ui_recentList_项目点击(*__arg0); });
        }
    }
    if (windowName == QStringLiteral("Form") || windowName == QStringLiteral("最近联系人项_ui")) {
        Form_最近联系人项 = qobject_cast<QWidget*>(root);
        horizontalLayout_最近联系人项 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayout") );
        label最近联系人头像_最近联系人项 = root->findChild<QLabel*>( QStringLiteral("label最近联系人头像") );
        verticalLayout_最近联系人项 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayout") );
        horizontalLayout_2_最近联系人项 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayout_2") );
        label最近联系人昵称_最近联系人项 = root->findChild<QLabel*>( QStringLiteral("label最近联系人昵称") );
        label最近联系人时间_最近联系人项 = root->findChild<QLabel*>( QStringLiteral("label最近联系人时间") );
        label最近联系人消息_最近联系人项 = root->findChild<QLabel*>( QStringLiteral("label最近联系人消息") );
    }
    if (windowName == QStringLiteral("ChatPanelWidget") || windowName == QStringLiteral("聊天框_ui")) {
        ChatPanelWidget_聊天框 = qobject_cast<QWidget*>(root);
        verticalLayoutChatPanel_聊天框 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutChatPanel") );
        chatHeader_聊天框 = root->findChild<QFrame*>( QStringLiteral("chatHeader") );
        horizontalLayoutChatHeader_聊天框 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayoutChatHeader") );
        verticalLayoutChatTitle_聊天框 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutChatTitle") );
        contactNameLabel_聊天框 = root->findChild<QLabel*>( QStringLiteral("contactNameLabel") );
        contactSubLabel_聊天框 = root->findChild<QLabel*>( QStringLiteral("contactSubLabel") );
        btnMore_聊天框 = root->findChild<QToolButton*>( QStringLiteral("btnMore") );
        lineH2_聊天框 = root->findChild<QFrame*>( QStringLiteral("lineH2") );
        chatVerticalSplitter_聊天框 = root->findChild<QSplitter*>( QStringLiteral("chatVerticalSplitter") );
        messageList_聊天框 = root->findChild<QListWidget*>( QStringLiteral("messageList") );
        chatComposer_聊天框 = root->findChild<QFrame*>( QStringLiteral("chatComposer") );
        verticalLayoutComposer_聊天框 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutComposer") );
        horizontalLayoutComposerTools_聊天框 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayoutComposerTools") );
        btnEmoji_聊天框 = root->findChild<QToolButton*>( QStringLiteral("btnEmoji") );
        btnfavorite_聊天框 = root->findChild<QToolButton*>( QStringLiteral("btnfavorite") );
        btnFolder_聊天框 = root->findChild<QToolButton*>( QStringLiteral("btnFolder") );
        btnCut_聊天框 = root->findChild<QToolButton*>( QStringLiteral("btnCut") );
        btnVoice_聊天框 = root->findChild<QToolButton*>( QStringLiteral("btnVoice") );
        hintLabel_聊天框 = root->findChild<QLabel*>( QStringLiteral("hintLabel") );
        messageInput_聊天框 = root->findChild<QTextEdit*>( QStringLiteral("messageInput") );
        horizontalLayoutSendRow_聊天框 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayoutSendRow") );
        sendButton_聊天框 = root->findChild<QPushButton*>( QStringLiteral("sendButton") );
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("btnEmoji") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信表情());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("btnfavorite") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信收藏());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("btnFolder") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信文件());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("btnCut") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信截图());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
        if (auto *target = root->findChild<QToolButton*>( QStringLiteral("btnVoice") )) {
            const QPixmap pixmap = E2PixmapFromResourceBytes(E2Res_微信麦克风());
            if (!pixmap.isNull())
                target->setIcon(QIcon(pixmap));
        }
    }
    if (windowName == QStringLiteral("ChatMessageItem") || windowName == QStringLiteral("聊天消息_ui")) {
        ChatMessageItem_聊天消息 = qobject_cast<QWidget*>(root);
        rootHorizontalLayout_聊天消息 = root->findChild<QHBoxLayout*>( QStringLiteral("rootHorizontalLayout") );
        leftAvatarLabel_聊天消息 = root->findChild<QLabel*>( QStringLiteral("leftAvatarLabel") );
        leftArrowLabel_聊天消息 = root->findChild<QLabel*>( QStringLiteral("leftArrowLabel") );
        bubbleFrame_聊天消息 = root->findChild<QFrame*>( QStringLiteral("bubbleFrame") );
        bubbleLayout_聊天消息 = root->findChild<QHBoxLayout*>( QStringLiteral("bubbleLayout") );
        messageLabel_聊天消息 = root->findChild<QLabel*>( QStringLiteral("messageLabel") );
        rightArrowLabel_聊天消息 = root->findChild<QLabel*>( QStringLiteral("rightArrowLabel") );
        rightAvatarLabel_聊天消息 = root->findChild<QLabel*>( QStringLiteral("rightAvatarLabel") );
    }
}

int main(int argc, char *argv[]) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    qInstallMessageHandler(e2QtMsgHandler);
    QApplication app(argc, argv);
    E2SetUiWidgetFactory(E2CreateWrappedWidget);
    QTranslator qtTranslator;
    const QString qtTranslationsPath = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
    const QLocale systemLocale = QLocale::system();
    if (qtTranslator.load(systemLocale, QStringLiteral("qtbase"), QStringLiteral("_"), qtTranslationsPath)
        || qtTranslator.load(QStringLiteral("qtbase_zh_CN"), qtTranslationsPath)) {
        app.installTranslator(&qtTranslator);
    }
#ifdef Q_OS_WIN
    app.setStyle(QStyleFactory::create("windowsvista"));
#else
    app.setStyle(QStyleFactory::create("Fusion"));
#endif

    _启动子程序();
    return app.exec();
}
