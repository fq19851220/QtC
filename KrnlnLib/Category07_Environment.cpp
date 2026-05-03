#include "E2CppBase.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QProcessEnvironment>

// cmdCategory: 7
// categoryCn: 环境存取

inline void 取命令行(EQtArray<文本型> &存放被取回命令行文本的数组变量) {
  存放被取回命令行文本的数组变量.data.clear();

  const QStringList args = QCoreApplication::arguments();
  for (int i = 1; i < args.size(); ++i) {
    存放被取回命令行文本的数组变量.data.push_back(args.at(i));
  }

  存放被取回命令行文本的数组变量.dims = {
      static_cast<整数型>(存放被取回命令行文本的数组变量.data.size())};
}

inline 文本型 取运行目录() {
  const QString filePath = QCoreApplication::applicationFilePath();
  if (filePath.isEmpty()) {
    return QDir::currentPath();
  }
  return QFileInfo(filePath).absolutePath();
}

inline 文本型 取运行目录Q() { return 取运行目录(); }

inline 文本型 取执行文件名() {
  const QString filePath = QCoreApplication::applicationFilePath();
  if (filePath.isEmpty()) {
    return QString();
  }
  return QFileInfo(filePath).fileName();
}

inline 文本型 取执行文件名Q() { return 取执行文件名(); }

inline 文本型 读环境变量(const 文本型 &环境变量名称) {
  if (环境变量名称.isEmpty()) {
    return {};
  }
  return qEnvironmentVariable(环境变量名称.toUtf8().constData());
}

inline 逻辑型 写环境变量(const 文本型 &环境变量名称,
                    const 文本型 &欲写入内容) {
  if (环境变量名称.isEmpty()) {
    return false;
  }
  const QByteArray 名称 = 环境变量名称.toUtf8();
  const QByteArray 内容 = 欲写入内容.toUtf8();
  return qputenv(名称.constData(), 内容);
}
