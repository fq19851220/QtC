#include "E2CppBase.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QFileDevice>
#include <QtCore/QFileInfo>
#include <QtCore/QStorageInfo>
#include <QtCore/QTemporaryFile>

#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

// cmdCategory: 15
// categoryCn: 磁盘操作

namespace {

inline QString E2DiskRootPath(const QString &path) {
  QString root = path.isEmpty() ? QDir::currentPath() : path;
  if (root.endsWith(u'/') || root.endsWith(u'\\')) {
    return root;
  }
  if (root.size() == 2 && root[1] == u':') {
    return root + u'\\';
  }
  return root;
}

#ifdef _WIN32
inline FILETIME E2ToFileTime(const QDateTime &dt) {
  const qint64 msecs = dt.toUTC().toMSecsSinceEpoch();
  const qint64 ticks = (msecs + 11644473600000LL) * 10000LL;
  ULARGE_INTEGER value;
  value.QuadPart = static_cast<ULONGLONG>(ticks);
  FILETIME fileTime;
  fileTime.dwLowDateTime = value.LowPart;
  fileTime.dwHighDateTime = value.HighPart;
  return fileTime;
}
#endif

} // namespace

inline 文本型 取当前目录() { return QDir::currentPath(); }

inline 文本型 取当前目录Q() { return 取当前目录(); }

inline bool 改变目录(const 文本型 &欲改变到的目录) {
  return QDir::setCurrent(欲改变到的目录);
}

inline bool 创建目录(const 文本型 &欲创建的目录名称) {
  return QDir().mkpath(欲创建的目录名称);
}

inline bool 删除目录(const 文本型 &欲删除的目录名称) {
  QDir dir(欲删除的目录名称);
  if (!dir.exists()) {
    return false;
  }
  return dir.removeRecursively();
}

inline bool 复制文件(const 文本型 &被复制文件名, const 文本型 &复制到文件名) {
  if (被复制文件名 == 复制到文件名) {
    return true;
  }
  if (QFile::exists(复制到文件名) && !QFile::remove(复制到文件名)) {
    return false;
  }
  return QFile::copy(被复制文件名, 复制到文件名);
}

inline bool 移动文件(const 文本型 &被移动文件名, const 文本型 &移动到文件名) {
  if (被移动文件名 == 移动到文件名) {
    return true;
  }
  if (QFile::exists(移动到文件名) && !QFile::remove(移动到文件名)) {
    return false;
  }
  return QFile::rename(被移动文件名, 移动到文件名);
}

inline bool 删除文件(const 文本型 &欲删除的文件名) {
  return QFile::remove(欲删除的文件名);
}

inline bool 文件更名(const 文本型 &欲更名的原文件名, const 文本型 &欲更改为的现文件名) {
  return 移动文件(欲更名的原文件名, 欲更改为的现文件名);
}

inline bool 文件是否存在(const 文本型 &欲寻找的文件名) {
  return QFileInfo::exists(欲寻找的文件名);
}

inline 日期时间型 取文件时间(const 文本型 &文件名) {
  return QFileInfo(文件名).lastModified();
}

inline 整数型 取文件尺寸(const 文本型 &文件名) {
  QFileInfo info(文件名);
  return info.exists() ? static_cast<整数型>(info.size()) : -1;
}

inline 字节集 读入文件(const 文本型 &文件名) {
  QFile file(文件名);
  if (!file.open(QIODevice::ReadOnly)) {
    return {};
  }
  return file.readAll();
}

inline bool 写到文件(const 文本型 &文件名, const 字节集 &欲写入的数据) {
  QFile file(文件名);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    return false;
  }
  return file.write(欲写入的数据) == 欲写入的数据.size();
}

inline bool 写到文件(const 文本型 &文件名, const QByteArray &欲写入的数据) {
  return 写到文件(文件名, 字节集(欲写入的数据));
}

inline bool 写到文件(const 文本型 &文件名, const E2CppLiteralArray &欲写入的数据) {
  return 写到文件(文件名, 字节集(static_cast<E2CppByteArray>(欲写入的数据)));
}

inline bool 写到文件(const 文本型 &文件名, const 文本型 &欲写入的文本) {
  return 写到文件(文件名, 字节集(欲写入的文本.toUtf8()));
}

inline 文本型 取临时文件名(const 文本型 &目录名 = {}) {
  QString directory = 目录名.isEmpty() ? QDir::tempPath() : 目录名;
  if (!QDir().mkpath(directory)) {
    return {};
  }
  QTemporaryFile tempFile(QDir(directory).filePath(QStringLiteral("e2cXXXXXX.tmp")));
  tempFile.setAutoRemove(false);
  if (!tempFile.open()) {
    return {};
  }
  return tempFile.fileName();
}

inline 文本型 取临时文件名Q(const 文本型 &目录名 = {}) { return 取临时文件名(目录名); }

inline 双精度小数型 取磁盘总空间(const 文本型 &驱动器名) {
  const QStorageInfo storage(E2DiskRootPath(驱动器名));
  return storage.isValid() ? static_cast<双精度小数型>(storage.bytesTotal()) / (1024.0 * 1024.0)
                           : 0.0;
}

inline 双精度小数型 取磁盘剩余空间(const 文本型 &驱动器名) {
  const QStorageInfo storage(E2DiskRootPath(驱动器名));
  return storage.isValid() ? static_cast<双精度小数型>(storage.bytesFree()) / (1024.0 * 1024.0)
                           : 0.0;
}

inline 文本型 取磁盘卷标(const 文本型 &驱动器名) {
#ifdef _WIN32
  wchar_t volName[MAX_PATH] = {0};
  const QString root = E2DiskRootPath(驱动器名);
  if (GetVolumeInformationW(reinterpret_cast<LPCWSTR>(root.utf16()), volName, MAX_PATH, nullptr,
                            nullptr, nullptr, nullptr, 0)) {
    return QString::fromWCharArray(volName);
  }
#else
  const QStorageInfo storage(E2DiskRootPath(驱动器名));
  if (storage.isValid()) {
    return storage.displayName();
  }
#endif
  return {};
}

inline 文本型 取磁盘卷标Q(const 文本型 &驱动器名) { return 取磁盘卷标(驱动器名); }

inline bool 置磁盘卷标(const 文本型 &驱动器名, const 文本型 &新卷标) {
#ifdef _WIN32
  const QString root = E2DiskRootPath(驱动器名);
  return SetVolumeLabelW(reinterpret_cast<LPCWSTR>(root.utf16()),
                         reinterpret_cast<LPCWSTR>(新卷标.utf16()));
#else
  Q_UNUSED(驱动器名);
  Q_UNUSED(新卷标);
  return false;
#endif
}

inline bool 改变驱动器(const 文本型 &驱动器名) { return QDir::setCurrent(E2DiskRootPath(驱动器名)); }

inline 文本型 寻找文件(const 文本型 &欲寻找的文件名或目录名 = {}, 整数型 欲寻找文件的属性 = -1) {
  // 静态迭代器：首次调用（非空路径）初始化，后续调用（空路径）继续迭代
  static QDirIterator *s_it = nullptr;
  static 整数型 s_attr = -1;

  // 易语言文件属性标志映射（attr参数含义）：
  //   0   = 不匹配任何文件
  //  -1   = 普通文件（非目录）
  //  16   = 目录
  auto matchAttr = [](const QFileInfo &fi, 整数型 attr) -> bool {
    if (attr == 0) return false;
    if (attr == -1) return fi.isFile();
    if (attr & 16) return fi.isDir();
    return fi.isFile();
  };

  if (!欲寻找的文件名或目录名.isEmpty()) {
    delete s_it;
    s_it = nullptr;
    s_attr = 欲寻找文件的属性;

    // 分离目录和文件名过滤器（支持通配符如 "C:/dir/*.txt"）
    const QString pattern = 欲寻找的文件名或目录名;
    const QFileInfo patternFi(pattern);
    const QString dir = patternFi.absolutePath();
    const QString nameFilter = patternFi.fileName(); // 可含通配符

    QDir::Filters filters = QDir::NoDotAndDotDot;
    if (s_attr == -1 || (s_attr & 16) == 0) filters |= QDir::Files;
    if (s_attr & 16) filters |= QDir::Dirs;

    s_it = new QDirIterator(dir, QStringList{nameFilter}, filters);
    if (s_it->hasNext()) {
      return s_it->next(), QFileInfo(s_it->filePath()).fileName();
    }
    delete s_it;
    s_it = nullptr;
    return {};
  }

  if (!s_it) return {};
  if (s_it->hasNext()) {
    s_it->next();
    const QFileInfo fi(s_it->filePath());
    if (matchAttr(fi, s_attr)) {
      return fi.fileName();
    }
    // 继续找下一个匹配项
    while (s_it->hasNext()) {
      s_it->next();
      const QFileInfo fi2(s_it->filePath());
      if (matchAttr(fi2, s_attr)) return fi2.fileName();
    }
  }
  delete s_it;
  s_it = nullptr;
  return {};
}

inline 文本型 寻找文件(const E2EmptyParam &, 整数型 欲寻找文件的属性) {
  return 寻找文件(文本型{}, 欲寻找文件的属性);
}

inline 整数型 取文件属性(const 文本型 &文件名) {
#ifdef _WIN32
  const DWORD attr = GetFileAttributesW(reinterpret_cast<LPCWSTR>(文件名.utf16()));
  if (attr == INVALID_FILE_ATTRIBUTES) {
    return -1;
  }
  return static_cast<整数型>(attr);
#else
  QFile::Permissions permissions = QFile::permissions(文件名);
  return static_cast<整数型>(permissions);
#endif
}

inline bool 置文件属性(const 文本型 &文件名, 整数型 属性值) {
#ifdef _WIN32
  return SetFileAttributesW(reinterpret_cast<LPCWSTR>(文件名.utf16()), static_cast<DWORD>(属性值));
#else
  return QFile::setPermissions(文件名, static_cast<QFileDevice::Permissions>(属性值));
#endif
}

inline bool 置文件时间(const 文本型 &文件名, 日期时间型 创建时间 = {}, 日期时间型 修改时间 = {},
                       日期时间型 访问时间 = {}) {
#ifdef _WIN32
  HANDLE handle =
      CreateFileW(reinterpret_cast<LPCWSTR>(文件名.utf16()), FILE_WRITE_ATTRIBUTES,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
                  FILE_ATTRIBUTE_NORMAL, nullptr);
  if (handle == INVALID_HANDLE_VALUE) {
    return false;
  }

  FILETIME createTime = {};
  FILETIME modifyTime = {};
  FILETIME accessTime = {};

  const FILETIME *createPtr = 创建时间.isValid() ? &(createTime = E2ToFileTime(创建时间)) : nullptr;
  const FILETIME *modifyPtr = 修改时间.isValid() ? &(modifyTime = E2ToFileTime(修改时间)) : nullptr;
  const FILETIME *accessPtr = 访问时间.isValid() ? &(accessTime = E2ToFileTime(访问时间)) : nullptr;

  const bool ok = SetFileTime(handle, createPtr, accessPtr, modifyPtr) != FALSE;
  CloseHandle(handle);
  return ok;
#else
  QFile file(文件名);
  if (!file.open(QIODevice::ReadWrite)) {
    return false;
  }
  bool ok = true;
  if (创建时间.isValid()) {
    ok = file.setFileTime(创建时间, QFileDevice::FileBirthTime) && ok;
  }
  if (修改时间.isValid()) {
    ok = file.setFileTime(修改时间, QFileDevice::FileModificationTime) && ok;
  }
  if (访问时间.isValid()) {
    ok = file.setFileTime(访问时间, QFileDevice::FileAccessTime) && ok;
  }
  return ok;
#endif
}

#ifdef QT_CORE_LIB

inline std::string 取当前目录A() { return E2ToStdString(取当前目录()); }

inline bool 改变目录(const std::string &欲改变到的目录) { return 改变目录(QString::fromStdString(欲改变到的目录)); }

inline bool 创建目录(const std::string &欲创建的目录名称) {
  return 创建目录(QString::fromStdString(欲创建的目录名称));
}

inline bool 删除目录(const std::string &欲删除的目录名称) {
  return 删除目录(QString::fromStdString(欲删除的目录名称));
}

inline bool 复制文件(const std::string &被复制文件名, const std::string &复制到文件名) {
  return 复制文件(QString::fromStdString(被复制文件名), QString::fromStdString(复制到文件名));
}

inline bool 移动文件(const std::string &被移动文件名, const std::string &移动到文件名) {
  return 移动文件(QString::fromStdString(被移动文件名), QString::fromStdString(移动到文件名));
}

inline bool 删除文件(const std::string &欲删除的文件名) {
  return 删除文件(QString::fromStdString(欲删除的文件名));
}

inline bool 文件更名(const std::string &欲更名的原文件名, const std::string &欲更改为的现文件名) {
  return 文件更名(QString::fromStdString(欲更名的原文件名), QString::fromStdString(欲更改为的现文件名));
}

inline bool 文件是否存在(const std::string &欲寻找的文件名) {
  return 文件是否存在(QString::fromStdString(欲寻找的文件名));
}

inline E2CppDateTime 取文件时间(const std::string &文件名) {
  return static_cast<E2CppDateTime>(E2CppValue(取文件时间(QString::fromStdString(文件名))));
}

inline std::int32_t 取文件尺寸(const std::string &文件名) {
  return 取文件尺寸(QString::fromStdString(文件名));
}

inline E2CppByteArray 读入文件(const std::string &文件名) {
  const QByteArray data = 读入文件(QString::fromStdString(文件名));
  return E2CppByteArray(data.begin(), data.end());
}

inline bool 写到文件(const std::string &文件名, const E2CppByteArray &欲写入的数据) {
  return 写到文件(QString::fromStdString(文件名), 字节集(E2ToQByteArray(欲写入的数据)));
}

inline bool 写到文件(const std::string &文件名, const std::string &欲写入的文本) {
  return 写到文件(QString::fromStdString(文件名), QString::fromStdString(欲写入的文本));
}

inline std::string 取临时文件名(const std::string &目录名 = "") {
  return E2ToStdString(取临时文件名(QString::fromStdString(目录名)));
}

inline double 取磁盘总空间(const std::string &驱动器名) {
  return 取磁盘总空间(QString::fromStdString(驱动器名));
}

inline double 取磁盘剩余空间(const std::string &驱动器名) {
  return 取磁盘剩余空间(QString::fromStdString(驱动器名));
}

inline std::string 取磁盘卷标(const std::string &驱动器名) {
  return E2ToStdString(取磁盘卷标(QString::fromStdString(驱动器名)));
}

inline bool 置磁盘卷标(const std::string &驱动器名, const std::string &新卷标) {
  return 置磁盘卷标(QString::fromStdString(驱动器名), QString::fromStdString(新卷标));
}

inline bool 改变驱动器(const std::string &驱动器名) {
  return 改变驱动器(QString::fromStdString(驱动器名));
}

inline std::string 寻找文件(const std::string &欲寻找的文件名或目录名, std::int32_t 欲寻找文件的属性) {
  return E2ToStdString(寻找文件(QString::fromStdString(欲寻找的文件名或目录名), 欲寻找文件的属性));
}

inline std::int32_t 取文件属性(const std::string &文件名) {
  return 取文件属性(QString::fromStdString(文件名));
}

inline bool 置文件属性(const std::string &文件名, std::int32_t 属性值) {
  return 置文件属性(QString::fromStdString(文件名), 属性值);
}

inline bool 置文件时间(const std::string &文件名, E2CppDateTime 创建时间 = E2CppDateTime(),
                       E2CppDateTime 修改时间 = E2CppDateTime(), E2CppDateTime 访问时间 = E2CppDateTime()) {
  return 置文件时间(QString::fromStdString(文件名), static_cast<QDateTime>(创建时间),
                    static_cast<QDateTime>(修改时间), static_cast<QDateTime>(访问时间));
}

#endif
