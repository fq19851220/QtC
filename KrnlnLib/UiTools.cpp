#include "UiTools.h"

#include <QtCore/QBuffer>
#include <QtCore/QCryptographicHash>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtCore/QIODevice>
#include <QtCore/QRegularExpression>
#include <QtCore/QStandardPaths>
#include <QtCore/QString>
#include <QtCore/QStringDecoder>
#include <QtCore/QUrl>

#if defined(QT_WIDGETS_LIB)
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#endif
#if defined(QT_WIDGETS_LIB) && defined(QT_UITOOLS_LIB)
#include <QtUiTools/QUiLoader>
#endif

namespace {

E2ResourceResolver g_e2ResourceResolver = nullptr;
E2UiWidgetFactory g_e2UiWidgetFactory = nullptr;

#if defined(QT_WIDGETS_LIB) && defined(QT_UITOOLS_LIB)
class E2UiLoader final : public QUiLoader {
public:
  using QUiLoader::QUiLoader;

  QWidget *createWidget(const QString &className, QWidget *parent,
                        const QString &name) override {
    if (parent == nullptr)
      m_currentWindowName = name;
    if (g_e2UiWidgetFactory != nullptr) {
      if (QWidget *widget =
              g_e2UiWidgetFactory(m_currentWindowName, className, parent, name)) {
        widget->setObjectName(name);
        return widget;
      }
    }
    return QUiLoader::createWidget(className, parent, name);
  }

private:
  QString m_currentWindowName;
};
#endif

QString E2ImageSuffixFromBytes(const QByteArray &data) {
  if (data.size() >= 8 && data.left(8) == QByteArray("\x89PNG\r\n\x1A\n", 8))
    return QStringLiteral(".png");
  if (data.size() >= 2 && data.left(2) == QByteArray("\xFF\xD8", 2))
    return QStringLiteral(".jpg");
  if (data.startsWith("GIF87a") || data.startsWith("GIF89a"))
    return QStringLiteral(".gif");
  if (data.startsWith("BM"))
    return QStringLiteral(".bmp");
  if (data.size() >= 4 && data.left(4) == QByteArray("\x00\x00\x01\x00", 4))
    return QStringLiteral(".ico");
  return QStringLiteral(".img");
}

QString E2WriteTempImageResourceFile(const QString &resourceName,
                                     const QByteArray &data) {
  if (resourceName.isEmpty() || data.isEmpty())
    return {};

  const QString root = QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
                           .isEmpty()
                       ? QDir::tempPath()
                       : QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
  QDir dir(root);
  if (!dir.mkpath(QStringLiteral("EQtImageResources")))
    return {};
  dir.cd(QStringLiteral("EQtImageResources"));

  const QByteArray hash =
      QCryptographicHash::hash(resourceName.toUtf8() + '\0' + data,
                               QCryptographicHash::Sha256)
          .toHex()
          .left(24);
  const QString filePath =
      dir.filePath(QString::fromLatin1(hash) + E2ImageSuffixFromBytes(data));

  QFile existing(filePath);
  if (existing.exists() && existing.size() == data.size())
    return QDir::fromNativeSeparators(filePath);

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    return {};
  if (file.write(data) != data.size())
    return {};
  return QDir::fromNativeSeparators(filePath);
}

QByteArray E2ResolveUiImageSchemes(const QByteArray &uiBytes) {
  if (uiBytes.isEmpty() || g_e2ResourceResolver == nullptr ||
      !uiBytes.contains("eqt-image:")) {
    return uiBytes;
  }

  QString xml = QString::fromUtf8(uiBytes);
  static const QRegularExpression imageRx(
      QStringLiteral(R"(eqt-image:([^<"'\s]+))"));
  QHash<QString, QString> replacements;

  auto it = imageRx.globalMatch(xml);
  while (it.hasNext()) {
    const QRegularExpressionMatch match = it.next();
    const QString marker = match.captured(0);
    if (replacements.contains(marker))
      continue;

    const QString encodedName = match.captured(1).trimmed();
    const QString decodedName =
        QUrl::fromPercentEncoding(encodedName.toUtf8()).trimmed();
    QByteArray data = g_e2ResourceResolver(decodedName);
    if (data.isEmpty() && decodedName != encodedName)
      data = g_e2ResourceResolver(encodedName);
    if (data.isEmpty()) {
      qWarning().noquote() << "载入窗口Q: image resource not found" << decodedName;
      continue;
    }

    const QString tempPath = E2WriteTempImageResourceFile(decodedName, data);
    if (!tempPath.isEmpty())
      replacements.insert(marker, tempPath);
  }

  for (auto itReplace = replacements.cbegin(); itReplace != replacements.cend();
       ++itReplace) {
    xml.replace(itReplace.key(), itReplace.value());
  }
  return xml.toUtf8();
}

QByteArray E2NormalizeUiXmlBytes(QByteArray uiBytes) {
  if (uiBytes.isEmpty()) {
    return uiBytes;
  }

  // Try UTF-8 first (may have BOM or not)
  QStringDecoder utf8Decoder(QStringDecoder::Utf8, QStringDecoder::Flag::Stateless);
  const QString utf8Text = utf8Decoder(uiBytes);
  if (!utf8Decoder.hasError()) {
    // Valid UTF-8 - ensure XML declaration matches
    static const QRegularExpression encodingRx(
        QStringLiteral(R"((<\?xml[^>]*encoding\s*=\s*["'])([^"']+)(["'][^>]*\?>))"),
        QRegularExpression::CaseInsensitiveOption);
    QString fixed = utf8Text;
    fixed.replace(encodingRx, QStringLiteral(R"(\1UTF-8\3)"));
    return fixed.toUtf8();
  }

  // UTF-8 failed - assume System encoding (GBK on Chinese Windows)
  // QString::fromLocal8Bit automatically uses the OS native code page
  QString localText = QString::fromLocal8Bit(uiBytes);

  if (localText.isEmpty()) {
    return uiBytes;
  }

  // Fix XML declaration to say UTF-8 and convert to UTF-8
  static const QRegularExpression encodingRx(
      QStringLiteral(R"((<\?xml[^>]*encoding\s*=\s*["'])([^"']+)(["'][^>]*\?>))"),
      QRegularExpression::CaseInsensitiveOption);
  localText.replace(encodingRx, QStringLiteral(R"(\1UTF-8\3)"));
  return localText.toUtf8();
}

QByteArray E2LoadWindowQToBytes(const E2CppValue &value) {
  return std::visit(
      overloaded{
          [](std::monostate) -> QByteArray { return {}; },
          [](std::int32_t v) -> QByteArray { return QByteArray::number(v); },
          [](std::int64_t v) -> QByteArray { return QByteArray::number(v); },
          [](float v) -> QByteArray { return QByteArray::number(v, 'g', 16); },
          [](double v) -> QByteArray { return QByteArray::number(v, 'g', 16); },
          [](const QDateTime &v) -> QByteArray {
            return v.toString(Qt::ISODateWithMs).toUtf8();
          },
          [](bool v) -> QByteArray {
            return v ? QByteArray("true") : QByteArray("false");
          },
          [](const std::string &v) -> QByteArray {
            return QByteArray(v.data(), static_cast<qsizetype>(v.size()));
          },
          [](const E2CppByteArray &v) -> QByteArray {
            return QByteArray(reinterpret_cast<const char *>(v.data()),
                              static_cast<qsizetype>(v.size()));
          },
          [](void *v) -> QByteArray {
            return QByteArray::number(
                static_cast<qulonglong>(reinterpret_cast<quintptr>(v)));
          }},
      value.value);
}

QWidget *E2LoadWindowQFromBytes(const QByteArray &uiBytes, bool AutoShow) {
  if (uiBytes.isEmpty()) {
    qWarning().noquote() << "载入窗口Q: empty ui data";
    return nullptr;
  }

#if !defined(QT_WIDGETS_LIB) || !defined(QT_UITOOLS_LIB)
  Q_UNUSED(uiBytes);
  Q_UNUSED(AutoShow);
  qWarning().noquote() << "载入窗口Q: QtWidgets/QtUiTools unavailable";
  return nullptr;
#else
  if (QApplication::instance() == nullptr) {
    qWarning().noquote() << "载入窗口Q: QApplication not initialized";
    return nullptr;
  }

  QBuffer buffer;
  buffer.setData(E2ResolveUiImageSchemes(E2NormalizeUiXmlBytes(uiBytes)));
  if (!buffer.open(QIODevice::ReadOnly)) {
    qWarning().noquote() << "载入窗口Q: failed to open buffer";
    return nullptr;
  }

  E2UiLoader loader;
  QWidget *widget = loader.load(&buffer, nullptr);
  if (!widget) {
    qWarning().noquote() << "载入窗口Q:" << loader.errorString();
    return nullptr;
  }

  widget->setAttribute(Qt::WA_DeleteOnClose, true);
  E2BindLoadedWindowEvents(widget, widget->objectName());
  if (AutoShow) {
    widget->show();
    widget->raise();
    widget->activateWindow();
  }
  return widget;
#endif
}

} // namespace

void E2SetResourceResolver(E2ResourceResolver resolver) {
  g_e2ResourceResolver = resolver;
}

void E2SetUiWidgetFactory(E2UiWidgetFactory factory) {
  g_e2UiWidgetFactory = factory;
}

QWidget *载入窗口Q(const E2CppValue &uiData, bool AutoShow) {
  return E2LoadWindowQFromBytes(E2LoadWindowQToBytes(uiData), AutoShow);
}

QWidget *载入窗口Q(const 文本型 &uiText, bool AutoShow) {
  return E2LoadWindowQFromBytes(uiText.toUtf8(), AutoShow);
}

QWidget *载入窗口Q(const 字节集 &uiData, bool AutoShow) {
  return E2LoadWindowQFromBytes(
      QByteArray(reinterpret_cast<const char *>(uiData.data()),
                 static_cast<qsizetype>(uiData.size())),
      AutoShow);
}
