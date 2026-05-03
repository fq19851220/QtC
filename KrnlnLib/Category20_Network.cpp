#include "E2CppBase.h"

#include <QtCore/QProcess>
#include <QtCore/QRegularExpression>
#include <QtCore/QSysInfo>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

// cmdCategory: 20
// categoryCn: 网络通信

namespace {

#ifdef _WIN32
struct E2WinsockGuard {
  E2WinsockGuard() {
    WSADATA wsaData{};
    ok = (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);
  }
  ~E2WinsockGuard() {
    if (ok) {
      WSACleanup();
    }
  }
  bool ok = false;
};

inline bool E2EnsureSocketEnv() {
  static E2WinsockGuard guard;
  return guard.ok;
}
#else
inline bool E2EnsureSocketEnv() { return true; }
#endif

inline 逻辑型 主机名安全(const 文本型 &host) {
  if (host.isEmpty()) {
    return false;
  }
  for (QChar ch : host) {
    if (!(ch.isLetterOrNumber() || ch == QLatin1Char('.') ||
          ch == QLatin1Char('-') || ch == QLatin1Char(':') ||
          ch == QLatin1Char('_'))) {
      return false;
    }
  }
  return true;
}

inline 整数型 从Ping输出提取耗时(const 文本型 &out) {
  static const QRegularExpression kRe(
      QStringLiteral(R"((time|时间)\s*[=<]\s*([0-9]+(?:\.[0-9]+)?)\s*ms)"),
      QRegularExpression::CaseInsensitiveOption);
  const auto match = kRe.match(out);
  if (match.hasMatch()) {
    bool ok = false;
    const auto v = match.captured(2).toDouble(&ok);
    return ok ? static_cast<整数型>(qRound64(v)) : -1;
  }

  static const QRegularExpression kReAlt(
      QStringLiteral(R"(time\s*=\s*([0-9]+(?:\.[0-9]+)?)\s*ms)"),
      QRegularExpression::CaseInsensitiveOption);
  const auto matchAlt = kReAlt.match(out);
  if (matchAlt.hasMatch()) {
    bool ok = false;
    const auto v = matchAlt.captured(1).toDouble(&ok);
    return ok ? static_cast<整数型>(qRound64(v)) : -1;
  }
  return -1;
}

inline 文本型 E2SockaddrToText(const sockaddr *addr) {
  if (!addr) {
    return {};
  }
  char host[NI_MAXHOST]{};
  const socklen_t len =
      addr->sa_family == AF_INET ? static_cast<socklen_t>(sizeof(sockaddr_in))
                                 : static_cast<socklen_t>(sizeof(sockaddr_in6));
  const int rc = getnameinfo(addr, len, host, sizeof(host), nullptr, 0,
                             NI_NUMERICHOST);
  return rc == 0 ? QString::fromLatin1(host) : QString();
}

} // namespace

inline 文本型 取主机名() {
  const QString host = QSysInfo::machineHostName();
  if (!host.isEmpty()) {
    return host;
  }
  if (!E2EnsureSocketEnv()) {
    return {};
  }
  char buffer[256]{};
  if (gethostname(buffer, static_cast<int>(sizeof(buffer))) == 0) {
    return QString::fromLocal8Bit(buffer);
  }
  return {};
}

inline 文本型 取主机名Q() { return 取主机名(); }

inline 整数型 通信测试(const 文本型 &被测试主机地址,
                  整数型 最长等待时间 = 10 * 1000) {
  if (!主机名安全(被测试主机地址)) {
    return -1;
  }
  if (最长等待时间 <= 0) {
    最长等待时间 = 10 * 1000;
  }

  QProcess proc;
#ifdef _WIN32
  const QString program = QStringLiteral("ping");
  const QStringList args = {QStringLiteral("-n"), QStringLiteral("1"),
                            QStringLiteral("-w"),
                            QString::number(最长等待时间), 被测试主机地址};
#else
  const QString program = QStringLiteral("ping");
  const QStringList args = {QStringLiteral("-c"), QStringLiteral("1"),
                            QStringLiteral("-W"),
                            QString::number((最长等待时间 + 999) / 1000),
                            被测试主机地址};
#endif
  proc.start(program, args);
  if (!proc.waitForFinished(最长等待时间 + 1000)) {
    proc.kill();
    proc.waitForFinished();
    return -1;
  }

  const 文本型 out = QString::fromLocal8Bit(proc.readAllStandardOutput()) +
                  QString::fromLocal8Bit(proc.readAllStandardError());
  const 整数型 ms = 从Ping输出提取耗时(out);
  if (ms >= 0) {
    return ms;
  }
  return proc.exitCode() == 0 ? 0 : -1;
}

inline 文本型 转换为主机名(const 文本型 &欲转换IP地址) {
  if (欲转换IP地址.isEmpty() || !E2EnsureSocketEnv()) {
    return {};
  }

  sockaddr_storage storage{};
  socklen_t storageLen = 0;
  const QByteArray ipUtf8 = 欲转换IP地址.toUtf8();

  auto ipv4 = reinterpret_cast<sockaddr_in *>(&storage);
  if (inet_pton(AF_INET, ipUtf8.constData(), &ipv4->sin_addr) == 1) {
    ipv4->sin_family = AF_INET;
    storageLen = static_cast<socklen_t>(sizeof(sockaddr_in));
  } else {
    auto ipv6 = reinterpret_cast<sockaddr_in6 *>(&storage);
    if (inet_pton(AF_INET6, ipUtf8.constData(), &ipv6->sin6_addr) == 1) {
      ipv6->sin6_family = AF_INET6;
      storageLen = static_cast<socklen_t>(sizeof(sockaddr_in6));
    } else {
      return {};
    }
  }

  char host[NI_MAXHOST]{};
  const int rc = getnameinfo(reinterpret_cast<const sockaddr *>(&storage),
                             storageLen, host, sizeof(host), nullptr, 0, 0);
  return rc == 0 ? QString::fromLocal8Bit(host) : QString();
}

inline 文本型 转换为主机名Q(const 文本型 &欲转换IP地址) {
  return 转换为主机名(欲转换IP地址);
}

inline 文本型 转换为IP地址(const 文本型 &欲转换主机名) {
  if (欲转换主机名.isEmpty() || !E2EnsureSocketEnv()) {
    return {};
  }

  addrinfo hints{};
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  addrinfo *result = nullptr;
  const QByteArray hostUtf8 = 欲转换主机名.toUtf8();
  const int rc = getaddrinfo(hostUtf8.constData(), nullptr, &hints, &result);
  if (rc != 0 || !result) {
    return {};
  }

  文本型 ip;
  for (addrinfo *cur = result; cur != nullptr; cur = cur->ai_next) {
    if (cur->ai_family == AF_INET) {
      ip = E2SockaddrToText(cur->ai_addr);
      if (!ip.isEmpty()) {
        break;
      }
    }
  }
  if (ip.isEmpty()) {
    for (addrinfo *cur = result; cur != nullptr; cur = cur->ai_next) {
      ip = E2SockaddrToText(cur->ai_addr);
      if (!ip.isEmpty()) {
        break;
      }
    }
  }

  freeaddrinfo(result);
  return ip;
}

inline 文本型 转换为IP地址Q(const 文本型 &欲转换主机名) {
  return 转换为IP地址(欲转换主机名);
}
