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
#include <QFrame>
#include <QSplitter>
#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QListWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
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

class E2Wrapped_QTextEdit final : public QTextEdit {
public:
    explicit E2Wrapped_QTextEdit(const QString &windowName, QWidget *parent = nullptr)
        : QTextEdit(parent), m_windowName(windowName) {}

protected:
    void insertFromMimeData(const QMimeData * source) override {
        if ((m_windowName == QStringLiteral("WeChatAdaptiveWindow") || m_windowName == QStringLiteral("主窗口_ui")) && objectName() == QStringLiteral("messageInput")) {
            _主窗口_ui_messageInput_插入自MIME数据(const_cast<QMimeData &>(*source));
            return;
        }
        QTextEdit::insertFromMimeData(source);
    }

    void keyPressEvent(QKeyEvent * e) override {
        if (e) {
            if ((m_windowName == QStringLiteral("WeChatAdaptiveWindow") || m_windowName == QStringLiteral("主窗口_ui")) && objectName() == QStringLiteral("messageInput")) {
            _主窗口_ui_messageInput_按键按下事件(*e);
            return;
            }
        }
        QTextEdit::keyPressEvent(e);
    }

private:
    QString m_windowName;
};

static QWidget *E2CreateWrappedWidget(const QString &windowName,
                                   const QString &className,
                                   QWidget *parent,
                                   const QString &name) {
    Q_UNUSED(name);
    if (className == QStringLiteral("QTextEdit"))
        return new E2Wrapped_QTextEdit(windowName, parent);
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
        if (m_windowName == QStringLiteral("WeChatAdaptiveWindow") || m_windowName == QStringLiteral("主窗口_ui")) {
            if (objectName == QStringLiteral("messageInput") && event->type() == QEvent::KeyPress) {
                _主窗口_ui_messageInput_按键按下事件(*static_cast<QKeyEvent*>(event));
            }
        }
        if (m_windowName == QStringLiteral("Form") || m_windowName == QStringLiteral("最近联系人_ui")) {
        }
        return QObject::eventFilter(watched, event);
    }

private:
    QString m_windowName;
};

void E2BindLoadedWindowEvents(QWidget *root, const QString &windowName) {
    if (!root)
        return;
    if (windowName == QStringLiteral("WeChatAdaptiveWindow") || windowName == QStringLiteral("主窗口_ui")) {
        WeChatAdaptiveWindow_主窗口 = qobject_cast<QWidget*>(root);
        verticalLayoutRoot_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutRoot") );
        adaptiveStack_主窗口 = root->findChild<QStackedWidget*>( QStringLiteral("adaptiveStack") );
        horizontalLayoutDesktopRoot_主窗口 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayoutDesktopRoot") );
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
        lineV1_主窗口 = root->findChild<QFrame*>( QStringLiteral("lineV1") );
        mainSplitter_主窗口 = root->findChild<QSplitter*>( QStringLiteral("mainSplitter") );
        desktopSidebar_主窗口 = root->findChild<QFrame*>( QStringLiteral("desktopSidebar") );
        verticalLayoutSidebar_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutSidebar") );
        recentHeader_主窗口 = root->findChild<QFrame*>( QStringLiteral("recentHeader") );
        horizontalLayoutRecentHeader_主窗口 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayoutRecentHeader") );
        searchEdit_主窗口 = root->findChild<QLineEdit*>( QStringLiteral("searchEdit") );
        btnNewChat_主窗口 = root->findChild<QToolButton*>( QStringLiteral("btnNewChat") );
        lineH1_主窗口 = root->findChild<QFrame*>( QStringLiteral("lineH1") );
        recentList_主窗口 = root->findChild<QListWidget*>( QStringLiteral("recentList") );
        chatPanel_主窗口 = root->findChild<QFrame*>( QStringLiteral("chatPanel") );
        verticalLayoutChatPanel_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutChatPanel") );
        chatHeader_主窗口 = root->findChild<QFrame*>( QStringLiteral("chatHeader") );
        horizontalLayoutChatHeader_主窗口 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayoutChatHeader") );
        verticalLayoutChatTitle_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutChatTitle") );
        contactNameLabel_主窗口 = root->findChild<QLabel*>( QStringLiteral("contactNameLabel") );
        contactSubLabel_主窗口 = root->findChild<QLabel*>( QStringLiteral("contactSubLabel") );
        btnMore_主窗口 = root->findChild<QToolButton*>( QStringLiteral("btnMore") );
        lineH2_主窗口 = root->findChild<QFrame*>( QStringLiteral("lineH2") );
        chatVerticalSplitter_主窗口 = root->findChild<QSplitter*>( QStringLiteral("chatVerticalSplitter") );
        messageList_主窗口 = root->findChild<QListWidget*>( QStringLiteral("messageList") );
        chatComposer_主窗口 = root->findChild<QFrame*>( QStringLiteral("chatComposer") );
        verticalLayoutComposer_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutComposer") );
        horizontalLayoutComposerTools_主窗口 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayoutComposerTools") );
        btnEmoji_主窗口 = root->findChild<QToolButton*>( QStringLiteral("btnEmoji") );
        btnfavorite_主窗口 = root->findChild<QToolButton*>( QStringLiteral("btnfavorite") );
        btnFolder_主窗口 = root->findChild<QToolButton*>( QStringLiteral("btnFolder") );
        btnCut_主窗口 = root->findChild<QToolButton*>( QStringLiteral("btnCut") );
        btnVoice_主窗口 = root->findChild<QToolButton*>( QStringLiteral("btnVoice") );
        hintLabel_主窗口 = root->findChild<QLabel*>( QStringLiteral("hintLabel") );
        messageInput_主窗口 = root->findChild<QTextEdit*>( QStringLiteral("messageInput") );
        horizontalLayoutSendRow_主窗口 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayoutSendRow") );
        sendButton_主窗口 = root->findChild<QPushButton*>( QStringLiteral("sendButton") );
        verticalLayoutMobileRoot_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutMobileRoot") );
        mobileHeader_主窗口 = root->findChild<QFrame*>( QStringLiteral("mobileHeader") );
        horizontalLayoutMobileHeader_主窗口 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayoutMobileHeader") );
        mobileBackButton_主窗口 = root->findChild<QToolButton*>( QStringLiteral("mobileBackButton") );
        mobileTitleLabel_主窗口 = root->findChild<QLabel*>( QStringLiteral("mobileTitleLabel") );
        mobileMoreButton_主窗口 = root->findChild<QToolButton*>( QStringLiteral("mobileMoreButton") );
        mobileContentStack_主窗口 = root->findChild<QStackedWidget*>( QStringLiteral("mobileContentStack") );
        verticalLayoutMobileContacts_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutMobileContacts") );
        mobileSearchEdit_主窗口 = root->findChild<QLineEdit*>( QStringLiteral("mobileSearchEdit") );
        mobileRecentList_主窗口 = root->findChild<QListWidget*>( QStringLiteral("mobileRecentList") );
        verticalLayoutMobileChat_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutMobileChat") );
        chatPage_主窗口 = root->findChild<QFrame*>( QStringLiteral("chatPage") );
        verticalLayoutMobileChatInner_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutMobileChatInner") );
        verticalLayoutMobileContactInfo_主窗口 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayoutMobileContactInfo") );
        mobileContactNameLabel_主窗口 = root->findChild<QLabel*>( QStringLiteral("mobileContactNameLabel") );
        mobileContactSubLabel_主窗口 = root->findChild<QLabel*>( QStringLiteral("mobileContactSubLabel") );
        mobileMessageList_主窗口 = root->findChild<QListWidget*>( QStringLiteral("mobileMessageList") );
        mobileHintLabel_主窗口 = root->findChild<QLabel*>( QStringLiteral("mobileHintLabel") );
        mobileMessageInput_主窗口 = root->findChild<QTextEdit*>( QStringLiteral("mobileMessageInput") );
        horizontalLayoutMobileSend_主窗口 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayoutMobileSend") );
        mobileAttachImageButton_主窗口 = root->findChild<QPushButton*>( QStringLiteral("mobileAttachImageButton") );
        mobileSendButton_主窗口 = root->findChild<QPushButton*>( QStringLiteral("mobileSendButton") );
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
        if (auto *sender = root->findChild<QToolButton*>( QStringLiteral("btnNewChat") )) {
            QObject::connect(sender, &QToolButton::clicked, root, []() { _主窗口_ui_btnNewChat_按钮点击(); });
        }
    }
    if (windowName == QStringLiteral("Form") || windowName == QStringLiteral("最近联系人_ui")) {
        Form_最近联系人 = qobject_cast<QWidget*>(root);
        horizontalLayout_最近联系人 = root->findChild<QHBoxLayout*>( QStringLiteral("horizontalLayout") );
        label最近联系人头像_最近联系人 = root->findChild<QLabel*>( QStringLiteral("label最近联系人头像") );
        verticalLayout_最近联系人 = root->findChild<QVBoxLayout*>( QStringLiteral("verticalLayout") );
        label最近联系人昵称_最近联系人 = root->findChild<QLabel*>( QStringLiteral("label最近联系人昵称") );
        label最近联系人时间_最近联系人 = root->findChild<QLabel*>( QStringLiteral("label最近联系人时间") );
        label最近联系人消息_最近联系人 = root->findChild<QLabel*>( QStringLiteral("label最近联系人消息") );
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
