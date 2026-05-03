#include "E2CppBase.h"

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QRegularExpression>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCore/QVariant>

// cmdCategory: 21
// categoryCn: 网页交互

struct 精易_IP归属地 {
    文本型 IP;
    文本型 国家;
    文本型 省份;
    文本型 城市;
    文本型 运营商;
};

inline 精易_IP归属地 网页_取IP地址() {
    精易_IP归属地 归属地;
    return 归属地;
}

inline void 网页_IE浏览器网页跳转静音(逻辑型 是否静音 = true) {
    // Qt WebEngine / WebKit 中可单独设置，此类纯系统设置将被忽略
    (void)是否静音;
}

inline 文本型 网页_协议头_取信息(const 文本型& 参_源协议头, const 文本型& 参_键名) {
    const QStringList lines = 参_源协议头.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        if (line.startsWith(参_键名, Qt::CaseInsensitive)) {
            int idx = line.indexOf(QLatin1Char(':'));
            if (idx != -1) {
                return line.mid(idx + 1).trimmed();
            }
        }
    }
    return 文本型();
}

inline 文本型 网页_JS格式化_EX(const 文本型& 参数1) {
    // 占位函数：Qt中并没有内置的JS格式化工具，如需实现可引入第三方库或暴露QJSEngine
    return 参数1;
}

inline 文本型 网页_文字翻译(const 文本型& 参_待翻译) {
    // 占位函数：通过网络请求翻译API
    (void)参_待翻译;
    return 文本型();
}

inline 逻辑型 网页_更改IE版本(整数型 类型 = 0, 逻辑型 是否使用所有用户 = false) {
    (void)类型;
    (void)是否使用所有用户;
    return false;
}

inline 文本型 网页_eval解密(const 文本型& 原JS文本) {
    return 原JS文本;
}

inline 文本型 网页_eval加密(const 文本型& 原JS文本) {
    return 原JS文本;
}

inline 文本型 网页_Cookie合并更新ex(文本型& 旧Cookie, const 文本型& 新Cookie) {
    if (新Cookie.isEmpty()) return 旧Cookie;
    if (旧Cookie.isEmpty()) return 新Cookie;
    旧Cookie = 旧Cookie + QStringLiteral("; ") + 新Cookie;
    return 旧Cookie;
}

inline 文本型 网页_Cookie合并更新(文本型& 旧Cookie, const 文本型& 新Cookie) {
    return 网页_Cookie合并更新ex(旧Cookie, 新Cookie);
}

inline 文本型 网页_处理协议头(const 文本型& 参_原始协议头) {
    return 参_原始协议头;
}

inline 文本型 网页_处理协议头ex(const 文本型& 参_原始协yi头) {
    return 参_原始协yi头;
}

inline 字节集 网页_访问_对象(
    const 文本型& 网址,
    整数型 访问方式 = 0,
    const 文本型& 提交信息 = 文本型(),
    文本型* 提交Cookies = nullptr,
    文本型* 返回Cookies = nullptr,
    const 文本型& 附加协议头 = 文本型(),
    文本型* 返回协议头 = nullptr,
    整数型* 返回状态代码 = nullptr,
    逻辑型 禁止重定向 = false,
    const 字节集& 字节集提交 = 字节集(),
    const 文本型& 代理地址 = 文本型(),
    整数型 超时 = 15,
    const 文本型& 代理用户名 = 文本型(),
    const 文本型& 代理密码 = 文本型(),
    整数型 代理标识 = 1,
    void* 对象继承 = nullptr,
    逻辑型 是否自动合并更新Cookie = true,
    逻辑型 是否补全必要协议头 = true,
    逻辑型 是否处理协议头大小写 = true
) {
    QNetworkAccessManager manager;
    QNetworkRequest request;
    request.setUrl(QUrl(网址));

    if (!代理地址.isEmpty()) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        QStringList parts = 代理地址.split(QLatin1Char(':'));
        if (parts.size() >= 2) {
            proxy.setHostName(parts[0]);
            bool ok;
            quint16 port = parts[1].toUShort(&ok);
            if (ok) proxy.setPort(port);
        } else {
            proxy.setHostName(代理地址);
        }
        if (!代理用户名.isEmpty()) {
            proxy.setUser(代理用户名);
            proxy.setPassword(代理密码);
        }
        manager.setProxy(proxy);
    }

    if (提交Cookies && !提交Cookies->isEmpty()) {
        request.setRawHeader("Cookie", 提交Cookies->toUtf8());
    }
    
    if (!附加协议头.isEmpty()) {
        QStringList headers = 附加协议头.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
        for (const QString& header : headers) {
            int idx = header.indexOf(QLatin1Char(':'));
            if (idx != -1) {
                request.setRawHeader(header.left(idx).trimmed().toUtf8(), header.mid(idx + 1).trimmed().toUtf8());
            }
        }
    }

    if (是否补全必要协议头) {
        if (!request.hasRawHeader("User-Agent")) {
            request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36");
        }
        if (!request.hasRawHeader("Accept")) {
            request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
        }
        if (!request.hasRawHeader("Accept-Language")) {
            request.setRawHeader("Accept-Language", "zh-CN,zh;q=0.9");
        }
        if (访问方式 == 1 && !request.hasRawHeader("Content-Type")) {
            request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
        }
    }

    if (禁止重定向) {
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::UserVerifiedRedirectPolicy);
    } else {
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    }

    qint64 size = 0; // fallback usage logic
    QByteArray postData;
    // We assume 字节集 has isEmpty() and provides to QByteArray easily mapping or is QByteArray.
    // If not, it can be constructed using its data. 
    // Usually EQt 字节集 is mapped directly to QByteArray
#if defined(QT_CORE_LIB)
    if (!字节集提交.isEmpty()) {
        postData = QByteArray(字节集提交.constData(), 字节集提交.size());
    } else if (!提交信息.isEmpty()) {
        postData = 提交信息.toUtf8();
    }
#endif

    QNetworkReply* reply = nullptr;
    if (访问方式 == 0) { // GET
        reply = manager.get(request);
    } else if (访问方式 == 1) { // POST
        reply = manager.post(request, postData);
    } else if (访问方式 == 2) { // HEAD
        reply = manager.head(request);
    } else if (访问方式 == 3) { // PUT
        reply = manager.put(request, postData);
    } else if (访问方式 == 5) { // DELETE
        reply = manager.deleteResource(request);
    } else {
        reply = manager.sendCustomRequest(request, "GET", postData);
    }

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    
    QTimer timer;
    if (超时 > 0) {
        timer.setSingleShot(true);
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start(超时 * 1000);
    }

    loop.exec();

    if (timer.isActive()) {
        timer.stop();
    } else if (超时 > 0 && !reply->isFinished()) {
        reply->abort();
    }

    字节集 result;
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray tmp = reply->readAll();
        result = 字节集(tmp.constData(), static_cast<qsizetype>(tmp.size()));
    } else {
        qDebug().noquote() << "\n网页_访问 发生网络错误:" << reply->errorString();
    }

    if (返回状态代码) {
        *返回状态代码 = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    }

    if (返回协议头) {
        *返回协议头 = QStringLiteral("");
        const auto headers = reply->rawHeaderList();
        for (const auto& header : headers) {
            *返回协议头 += QString::fromUtf8(header) + QStringLiteral(": ") + QString::fromUtf8(reply->rawHeader(header)) + QStringLiteral("\n");
        }
    }

    if (返回Cookies) {
        *返回Cookies = QStringLiteral("");
        QVariant var = reply->header(QNetworkRequest::SetCookieHeader);
        if (var.isValid()) {
            *返回Cookies = var.toString();
        }
    }

    reply->deleteLater();
    return result;
}

inline 字节集 网页_访问(
    const 文本型& 网址,
    整数型 访问方式 = 0,
    const 文本型& 提交信息 = 文本型(),
    文本型* 提交Cookies = nullptr,
    文本型* 返回Cookies = nullptr,
    const 文本型& 附加协议头 = 文本型(),
    文本型* 返回协议头 = nullptr,
    逻辑型 禁止重定向 = false,
    const 字节集& 字节集提交 = 字节集(),
    const 文本型& 代理地址 = 文本型(),
    逻辑型 是否自动合并更新Cookie = true,
    逻辑型 是否补全必要协议头 = true,
    逻辑型 是否处理协议头大小写 = true,
    逻辑型 是否使用socks4代理 = false
) {
    (void)是否使用socks4代理; // Qt Proxy could be updated to SOCKS5 if needed
    return 网页_访问_对象(
        网址, 访问方式, 提交信息, 提交Cookies, 返回Cookies, 附加协议头, 返回协议头,
        nullptr, 禁止重定向, 字节集提交, 代理地址, 15, 文本型(), 文本型(), 1, nullptr,
        是否自动合并更新Cookie, 是否补全必要协议头, 是否处理协议头大小写
    );
}

inline 文本型 网页_访问S(
    const 文本型& 网址,
    整数型 访问方式 = 0,
    const 文本型& 提交信息 = 文本型(),
    文本型* 提交Cookies = nullptr,
    文本型* 返回Cookies = nullptr,
    const 文本型& 附加协议头 = 文本型(),
    文本型* 返回协议头 = nullptr,
    逻辑型 禁止重定向 = false,
    const 字节集& 字节集提交 = 字节集(),
    const 文本型& 代理地址 = 文本型(),
    逻辑型 是否自动合并更新Cookie = true,
    逻辑型 是否补全必要协议头 = true,
    逻辑型 是否处理协议头大小写 = true,
    逻辑型 是否使用socks4代理 = false
) {
    字节集 ret = 网页_访问(
        网址, 访问方式, 提交信息, 提交Cookies, 返回Cookies, 附加协议头, 返回协议头,
        禁止重定向, 字节集提交, 代理地址, 是否自动合并更新Cookie, 是否补全必要协议头, 
        是否处理协议头大小写, 是否使用socks4代理
    );
    if (ret.isEmpty()) {
        return 文本型();
    }
    return QString::fromUtf8(ret.constData(), ret.size());
}

