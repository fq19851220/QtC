#include "E2CppBase.h"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#ifdef QT_GUI_LIB
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QCursor>
#include <QtGui/QClipboard>
#endif
#ifdef QT_WIDGETS_LIB
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFileDialog>
#endif
#ifdef QT_CORE_LIB
#include <QtCore/QSettings>
#endif

#ifdef _WIN32
#include <windows.h>

// 声明 ntdll 中的 RtlGetVersion 函数指针类型
typedef LONG(WINAPI *RtlGetVersion_FUNC)(OSVERSIONINFOW *);
#endif

// cmdCategory: 15
// categoryCn: 系统处理

/**
 * @brief 运行 (Run)
 */
inline bool 运行(const std::string &欲运行的命令行,
                 bool 是否等待程序运行完毕 = false,
                 std::int32_t 窗口显示方式 = 1) {
#ifdef QT_CORE_LIB
  QStringList args = QProcess::splitCommand(QString::fromStdString(欲运行的命令行));
  if (args.isEmpty())
    return false;
  QString program = args.takeFirst();

#ifdef _WIN32
  // Windows 上传递窗口显示方式
  QProcess proc;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  proc.setProgram(program);
  proc.setArguments(args);
  // QProcess 没有直接的窗口显示方式控制，在 Windows 上使用 CREATE_NO_WINDOW 等
  // 此处通过 startDetached 或 start 模拟
  if (窗口显示方式 == 1) {
    proc.setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args) {
      args->flags |= CREATE_NO_WINDOW;
    });
  }
  proc.start();
  if (!proc.waitForStarted(3000))
    return false;
  if (是否等待程序运行完毕)
    proc.waitForFinished(-1);
  return true;
#else
  if (是否等待程序运行完毕) {
    return QProcess::execute(program, args) >= 0;
  } else {
    return QProcess::startDetached(program, args);
  }
#endif

#else // 无 Qt，退回 Win32
#ifdef _WIN32
  STARTUPINFOA si = {sizeof(si)};
  PROCESS_INFORMATION pi;

  if (窗口显示方式 == 1)
    si.wShowWindow = SW_HIDE;
  else if (窗口显示方式 == 2)
    si.wShowWindow = SW_SHOWNORMAL;
  else if (窗口显示方式 == 3)
    si.wShowWindow = SW_SHOWMINIMIZED;
  else if (窗口显示方式 == 4)
    si.wShowWindow = SW_SHOWMAXIMIZED;
  else if (窗口显示方式 == 5)
    si.wShowWindow = SW_SHOWNOACTIVATE;
  else if (窗口显示方式 == 6)
    si.wShowWindow = SW_SHOWMINNOACTIVE;
  si.dwFlags = STARTF_USESHOWWINDOW;

  if (CreateProcessA(NULL, (char *)欲运行的命令行.c_str(), NULL, NULL, FALSE, 0,
                     NULL, NULL, &si, &pi)) {
    if (是否等待程序运行完毕)
      WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
  }
#endif
  return false;
#endif
}

inline bool 运行(const QString &欲运行的命令行,
                 bool 是否等待程序运行完毕 = false,
                 std::int32_t 窗口显示方式 = 1) {
  return 运行(E2ToStdString(欲运行的命令行), 是否等待程序运行完毕, 窗口显示方式);
}

/**
 * @brief 结束
 */
[[noreturn]] inline void 结束() { std::exit(0); }

/**
 * @brief 取剪辑板文本 (GetClipboardText)
 */
#ifdef QT_GUI_LIB
inline QString 取剪辑板文本() {
  QClipboard *cb = QGuiApplication::clipboard();
  return cb ? cb->text() : QString();
}
#elif defined(QT_CORE_LIB)
inline QString 取剪辑板文本() { return QString(); }
#else
inline std::string 取剪辑板文本() {
#ifdef _WIN32
  if (!OpenClipboard(nullptr))
    return "";
  HANDLE hData = GetClipboardData(CF_TEXT);
  std::string text;
  if (hData) {
    if (char *pszText = static_cast<char *>(GlobalLock(hData))) {
      text = pszText;
      GlobalUnlock(hData);
    }
  }
  CloseClipboard();
  return text;
#endif
  return "";
}
#endif

/**
 * @brief 置剪辑板文本 (SetClipboardText)
 */
#ifdef QT_GUI_LIB
inline bool 置剪辑板文本(const QString &准备置入剪辑板的文本) {
  QClipboard *cb = QGuiApplication::clipboard();
  if (!cb)
    return false;
  cb->setText(准备置入剪辑板的文本);
  return true;
}
inline bool 置剪辑板文本(const std::string &准备置入剪辑板的文本) {
  return 置剪辑板文本(QString::fromStdString(准备置入剪辑板的文本));
}
#else
inline bool 置剪辑板文本(const std::string &准备置入剪辑板的文本) {
#ifdef _WIN32
  if (!OpenClipboard(nullptr))
    return false;
  EmptyClipboard();
  size_t len = 准备置入剪辑板的文本.size();
  HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, len + 1);
  if (!hGlob) {
    CloseClipboard();
    return false;
  }
  void *pLock = GlobalLock(hGlob);
  if (pLock) {
    memcpy(pLock, 准备置入剪辑板的文本.c_str(), len + 1);
    GlobalUnlock(hGlob);
    SetClipboardData(CF_TEXT, hGlob);
  } else {
    GlobalFree(hGlob);
  }
  CloseClipboard();
  return true;
#else
  return false;
#endif
}
#endif

/**
 * @brief 剪辑板中可有文本 (HasClipboardText)
 */
inline bool 剪辑板中可有文本() {
#ifdef QT_GUI_LIB
  QClipboard *cb = QGuiApplication::clipboard();
  return cb && !cb->text().isEmpty();
#elif defined(_WIN32)
  return (IsClipboardFormatAvailable(CF_TEXT) ||
          IsClipboardFormatAvailable(CF_UNICODETEXT));
#else
  return false;
#endif
}

/**
 * @brief 清除剪辑板 (ClearClipboard)
 */
inline void 清除剪辑板() {
#ifdef QT_GUI_LIB
  if (QClipboard *cb = QGuiApplication::clipboard())
    cb->clear();
#elif defined(_WIN32)
  if (OpenClipboard(nullptr)) {
    EmptyClipboard();
    CloseClipboard();
  }
#endif
}

/**
 * @brief 取屏幕宽度 (GetScreenWidth)
 */
inline std::int32_t 取屏幕宽度() {
#ifdef QT_GUI_LIB
  if (QScreen *screen = QGuiApplication::primaryScreen())
    return screen->geometry().width();
  return 1920;
#elif defined(_WIN32)
  return GetSystemMetrics(SM_CXSCREEN);
#else
  return 1920;
#endif
}

/**
 * @brief 取屏幕高度 (GetScreenHeight)
 */
inline std::int32_t 取屏幕高度() {
#ifdef QT_GUI_LIB
  if (QScreen *screen = QGuiApplication::primaryScreen())
    return screen->geometry().height();
  return 1080;
#elif defined(_WIN32)
  return GetSystemMetrics(SM_CYSCREEN);
#else
  return 1080;
#endif
}

/**
 * @brief 取鼠标水平位置 (GetMouseX)
 */
inline std::int32_t 取鼠标水平位置() {
#ifdef QT_GUI_LIB
  return QCursor::pos().x();
#elif defined(_WIN32)
  POINT pt;
  if (GetCursorPos(&pt))
    return pt.x;
  return 0;
#else
  return 0;
#endif
}

/**
 * @brief 取鼠标垂直位置 (GetMouseY)
 */
inline std::int32_t 取鼠标垂直位置() {
#ifdef QT_GUI_LIB
  return QCursor::pos().y();
#elif defined(_WIN32)
  POINT pt;
  if (GetCursorPos(&pt))
    return pt.y;
  return 0;
#else
  return 0;
#endif
}

/**
 * @brief 取颜色数 (GetColorDepth)
 */
inline std::int32_t 取颜色数() {
#ifdef QT_GUI_LIB
  if (QScreen *screen = QGuiApplication::primaryScreen())
    return screen->depth();
  return 32;
#elif defined(_WIN32)
  HDC hdc = GetDC(NULL);
  int bitspixel = GetDeviceCaps(hdc, BITSPIXEL);
  ReleaseDC(NULL, hdc);
  if (bitspixel >= 32)
    return 32;
  return bitspixel;
#else
  return 32;
#endif
}

/**
 * @brief 辅助函数：将易语言的根目录映射为 Windows HKEY
 */
#ifdef _WIN32
inline HKEY _map_e_hkey(int root) {
  switch (root) {
  case 1:
    return HKEY_CLASSES_ROOT;
  case 2:
    return HKEY_CURRENT_USER;
  case 3:
    return HKEY_CURRENT_USER;
  case 4:
    return HKEY_LOCAL_MACHINE;
  case 5:
    return HKEY_USERS;
  default:
    return HKEY_CURRENT_USER;
  }
}
#endif

/**
 * @brief 取文本注册项 (GetTextRegItem)
 * @note 注册表为 Windows 专属，非 Windows 返回默认值
 */
inline std::string 取文本注册项(std::int32_t 根目录,
                                const std::string &全路径注册项名,
                                const std::string &默认文本 = "") {
#ifdef _WIN32
  HKEY hRoot = _map_e_hkey(根目录);
  std::string keyPath = 全路径注册项名;
  std::string valueName = "";

  if (!keyPath.empty() && keyPath.back() == '\\') {
    keyPath.pop_back();
  } else {
    size_t lastSlash = keyPath.find_last_of('\\');
    if (lastSlash != std::string::npos) {
      valueName = keyPath.substr(lastSlash + 1);
      keyPath = keyPath.substr(0, lastSlash);
    } else {
      valueName = keyPath;
      keyPath = "";
    }
  }

  HKEY hKey;
  if (RegOpenKeyExA(hRoot, keyPath.c_str(), 0, KEY_READ, &hKey) ==
      ERROR_SUCCESS) {
    char buffer[2048];
    DWORD bufferSize = sizeof(buffer);
    LSTATUS status =
        RegQueryValueExA(hKey, valueName.empty() ? NULL : valueName.c_str(),
                         NULL, NULL, (LPBYTE)buffer, &bufferSize);
    RegCloseKey(hKey);
    if (status == ERROR_SUCCESS && bufferSize > 0) {
      return std::string(buffer, bufferSize - 1);
    }
  }
#elif defined(Q_OS_ANDROID)
#  warning "取文本注册项: Android 不支持注册表，将返回默认文本"
#endif
  return 默认文本;
}

/**
 * @brief 取数值注册项 (GetNumRegItem)
 * @note 注册表为 Windows 专属
 */
inline std::int32_t 取数值注册项(std::int32_t 根目录,
                                 const std::string &全路径注册项名,
                                 std::int32_t 默认数值 = 0) {
#ifdef _WIN32
  HKEY hRoot = _map_e_hkey(根目录);
  std::string keyPath = 全路径注册项名;
  std::string valueName = "";
  if (!keyPath.empty() && keyPath.back() == '\\') {
    keyPath.pop_back();
  } else {
    size_t lastSlash = keyPath.find_last_of('\\');
    if (lastSlash != std::string::npos) {
      valueName = keyPath.substr(lastSlash + 1);
      keyPath = keyPath.substr(0, lastSlash);
    } else {
      valueName = keyPath;
      keyPath = "";
    }
  }

  HKEY hKey;
  if (RegOpenKeyExA(hRoot, keyPath.c_str(), 0, KEY_READ, &hKey) ==
      ERROR_SUCCESS) {
    DWORD value = 0;
    DWORD bufferSize = sizeof(value);
    LSTATUS status =
        RegQueryValueExA(hKey, valueName.empty() ? NULL : valueName.c_str(),
                         NULL, NULL, (LPBYTE)&value, &bufferSize);
    RegCloseKey(hKey);
    if (status == ERROR_SUCCESS) {
      return static_cast<std::int32_t>(value);
    }
  }
#elif defined(Q_OS_ANDROID)
#  warning "取数值注册项: Android 不支持注册表，将返回默认数值"
#endif
  return 默认数值;
}

/**
 * @brief 取字节集注册项 (GetBinRegItem)
 */
inline E2CppByteArray
取字节集注册项(std::int32_t 根目录, const std::string &全路径注册项名,
               const E2CppByteArray &默认字节集 = E2CppByteArray{}) {
#ifdef _WIN32
  HKEY hRoot = _map_e_hkey(根目录);
  std::string keyPath = 全路径注册项名;
  std::string valueName = "";
  if (!keyPath.empty() && keyPath.back() == '\\') {
    keyPath.pop_back();
  } else {
    size_t lastSlash = keyPath.find_last_of('\\');
    if (lastSlash != std::string::npos) {
      valueName = keyPath.substr(lastSlash + 1);
      keyPath = keyPath.substr(0, lastSlash);
    } else {
      valueName = keyPath;
      keyPath = "";
    }
  }

  HKEY hKey;
  if (RegOpenKeyExA(hRoot, keyPath.c_str(), 0, KEY_READ, &hKey) ==
      ERROR_SUCCESS) {
    DWORD bufferSize = 0;
    if (RegQueryValueExA(hKey, valueName.empty() ? NULL : valueName.c_str(),
                         NULL, NULL, NULL, &bufferSize) == ERROR_SUCCESS) {
      std::vector<uint8_t> buffer(bufferSize);
      if (RegQueryValueExA(hKey, valueName.empty() ? NULL : valueName.c_str(),
                           NULL, NULL, buffer.data(),
                           &bufferSize) == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return E2CppByteArray(buffer.begin(), buffer.end());
      }
    }
    RegCloseKey(hKey);
  }
#elif defined(Q_OS_ANDROID)
#  warning "取字节集注册项: Android 不支持注册表，将返回默认字节集"
#endif
  return 默认字节集;
}

/**
 * @brief 写注册项 (SetRegItem)
 * @note 注册表为 Windows 专属
 */
inline bool 写注册项(std::int32_t 根目录, const std::string &全路径注册项名,
                     const E2CppValue &欲写入值) {
#ifdef _WIN32
  HKEY hRoot = _map_e_hkey(根目录);
  std::string keyPath = 全路径注册项名;
  std::string valueName = "";
  if (!keyPath.empty() && keyPath.back() == '\\') {
    keyPath.pop_back();
  } else {
    size_t lastSlash = keyPath.find_last_of('\\');
    if (lastSlash != std::string::npos) {
      valueName = keyPath.substr(lastSlash + 1);
      keyPath = keyPath.substr(0, lastSlash);
    } else {
      valueName = keyPath;
      keyPath = "";
    }
  }

  HKEY hKey;
  if (RegCreateKeyExA(hRoot, keyPath.c_str(), 0, NULL, 0, KEY_WRITE, NULL,
                      &hKey, NULL) != ERROR_SUCCESS) {
    return false;
  }

  bool ok = false;
  std::visit(
      [&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::int32_t> ||
                      std::is_same_v<T, std::int64_t>) {
          DWORD val = static_cast<DWORD>(arg);
          if (RegSetValueExA(hKey, valueName.empty() ? NULL : valueName.c_str(),
                             0, REG_DWORD, (const BYTE *)&val,
                             sizeof(val)) == ERROR_SUCCESS)
            ok = true;
        } else if constexpr (std::is_same_v<T, std::string>) {
          if (RegSetValueExA(hKey, valueName.empty() ? NULL : valueName.c_str(),
                             0, REG_SZ, (const BYTE *)arg.c_str(),
                             static_cast<DWORD>(arg.length() + 1)) ==
              ERROR_SUCCESS)
            ok = true;
        } else if constexpr (std::is_same_v<T, E2CppByteArray>) {
          if (RegSetValueExA(hKey, valueName.empty() ? NULL : valueName.c_str(),
                             0, REG_BINARY, arg.data(),
                             static_cast<DWORD>(arg.size())) == ERROR_SUCCESS)
            ok = true;
        } else if constexpr (std::is_same_v<T, void *>) {
          DWORD_PTR val = (DWORD_PTR)arg;
          if (RegSetValueExA(hKey, valueName.empty() ? NULL : valueName.c_str(),
                             0, REG_DWORD, (const BYTE *)&val,
                             sizeof(DWORD)) == ERROR_SUCCESS)
            ok = true;
        } else {
          auto str = E2CppDebugToString(arg);
          if (RegSetValueExA(hKey, valueName.empty() ? NULL : valueName.c_str(),
                             0, REG_SZ, (const BYTE *)str.c_str(),
                             static_cast<DWORD>(str.length() + 1)) ==
              ERROR_SUCCESS)
            ok = true;
        }
      },
      欲写入值.value);

  RegCloseKey(hKey);
  return ok;
#elif defined(Q_OS_ANDROID)
#  warning "写注册项: Android 不支持注册表，将始终返回 false"
  return false;
#else
  return false;
#endif
}

/**
 * @brief 删除注册项 (DeleteRegItem)
 */
inline bool 删除注册项(std::int32_t 根目录, const std::string &全路径注册项名) {
#ifdef _WIN32
  HKEY hRoot = _map_e_hkey(根目录);
  std::string path = 全路径注册项名;
  while (!path.empty() && (path.back() == '\\' || path.back() == '/'))
    path.pop_back();

  if (RegDeleteTreeA(hRoot, path.c_str()) == ERROR_SUCCESS)
    return true;

  size_t lastSlash = path.find_last_of("\\/");
  if (lastSlash != std::string::npos) {
    std::string keyPath = path.substr(0, lastSlash);
    std::string valueName = path.substr(lastSlash + 1);
    HKEY hKey;
    if (RegOpenKeyExA(hRoot, keyPath.c_str(), 0, KEY_WRITE, &hKey) ==
        ERROR_SUCCESS) {
      bool ok = (RegDeleteValueA(hKey, valueName.c_str()) == ERROR_SUCCESS);
      RegCloseKey(hKey);
      return ok;
    }
  } else {
    return RegDeleteValueA(hRoot, path.c_str()) == ERROR_SUCCESS;
  }
#elif defined(Q_OS_ANDROID)
#  warning "删除注册项: Android 不支持注册表，将始终返回 false"
#endif
  return false;
}

/**
 * @brief 注册项是否存在 (IsRegItemExist)
 */
inline bool 注册项是否存在(std::int32_t 根目录,
                           const std::string &全路径注册项名) {
#ifdef _WIN32
  HKEY hRoot = _map_e_hkey(根目录);
  std::string keyPath = 全路径注册项名;
  std::string valueName = "";
  bool isKey = false;
  if (!keyPath.empty() && keyPath.back() == '\\') {
    keyPath.pop_back();
    isKey = true;
  } else {
    size_t lastSlash = keyPath.find_last_of('\\');
    if (lastSlash != std::string::npos) {
      valueName = keyPath.substr(lastSlash + 1);
      keyPath = keyPath.substr(0, lastSlash);
    } else {
      valueName = keyPath;
      keyPath = "";
    }
  }

  HKEY hKey;
  if (RegOpenKeyExA(hRoot, keyPath.c_str(), 0, KEY_READ, &hKey) ==
      ERROR_SUCCESS) {
    if (isKey) {
      RegCloseKey(hKey);
      return true;
    } else {
      LSTATUS status =
          RegQueryValueExA(hKey, valueName.c_str(), NULL, NULL, NULL, NULL);
      RegCloseKey(hKey);
      return status == ERROR_SUCCESS;
    }
  }
#elif defined(Q_OS_ANDROID)
#  warning "注册项是否存在: Android 不支持注册表，将始终返回 false"
#endif
  return false;
}

/**
 * @brief 取文本注册表项
 */
inline std::string 取文本注册表项(const std::string &程序名称,
                                  const std::string &节名称,
                                  const std::string &注册表项名称,
                                  const std::string &默认文本 = "") {
  std::string path =
      "Software\\" + 程序名称 + "\\" + 节名称 + "\\" + 注册表项名称;
  return 取文本注册项(2, path, 默认文本);
}

/**
 * @brief 取数值注册表项
 */
inline std::int32_t 取数值注册表项(const std::string &程序名称,
                                   const std::string &节名称,
                                   const std::string &注册表项名称,
                                   std::int32_t 默认数值 = 0) {
  std::string path =
      "Software\\" + 程序名称 + "\\" + 节名称 + "\\" + 注册表项名称;
  return 取数值注册项(2, path, 默认数值);
}

/**
 * @brief 取字节集注册表项
 */
inline E2CppByteArray
取字节集注册表项(const std::string &程序名称, const std::string &节名称,
                 const std::string &注册表项名称,
                 const E2CppByteArray &默认字节集 = E2CppByteArray{}) {
  std::string path =
      "Software\\" + 程序名称 + "\\" + 节名称 + "\\" + 注册表项名称;
  return 取字节集注册项(2, path, 默认字节集);
}

/**
 * @brief 写注册表项
 */
inline bool 写注册表项(const std::string &程序名称, const std::string &节名称,
                       const std::string &注册表项名称,
                       const E2CppValue &欲写入值) {
  std::string path =
      "Software\\" + 程序名称 + "\\" + 节名称 + "\\" + 注册表项名称;
  return 写注册项(2, path, 欲写入值);
}

/**
 * @brief 删除注册表项
 */
inline bool 删除注册表项(const std::string &程序名称, const std::string &节名称,
                         const std::string &注册表项名称) {
  std::string path =
      "Software\\" + 程序名称 + "\\" + 节名称 + "\\" + 注册表项名称;
  return 删除注册项(2, path);
}

/**
 * @brief 取启动时间 (自系统启动以来所经过的时间，单位为毫秒)
 */
inline std::int32_t 取启动时间() {
#ifdef QT_CORE_LIB
  return static_cast<std::int32_t>(QDateTime::currentMSecsSinceEpoch() & 0x7FFFFFFF);
#elif defined(_WIN32)
  return static_cast<std::int32_t>(GetTickCount());
#else
  auto now = std::chrono::steady_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      now.time_since_epoch());
  return static_cast<std::int32_t>(ms.count());
#endif
}

/**
 * @brief 延时
 */
inline void 延时(std::int32_t 欲等待的时间) {
  if (欲等待的时间 <= 0)
    return;
  std::this_thread::sleep_for(std::chrono::milliseconds(欲等待的时间));
}

#ifdef QT_CORE_LIB

#include <QCoreApplication>
#include <QTime>
#include <QEventLoop>

inline void 延迟(int ms) {
    if (ms <= 0) return;
    QTime dieTime = QTime::currentTime().addMSecs(ms);
    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

#else

#include <chrono>
#include <thread>

inline void 延迟(int ms) {
    if (ms <= 0) return;
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

#endif

// 辅助函数：将通用型安全地转为文本
inline std::string E2CppValueToString2(const E2CppValue &val) {
  return std::visit(
      overloaded{
          [](std::monostate) -> std::string { return ""; },
          [](std::int32_t v) -> std::string { return std::to_string(v); },
          [](std::int64_t v) -> std::string { return std::to_string(v); },
          [](float v) -> std::string {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%g", v);
            return buf;
          },
          [](double v) -> std::string {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%g", v);
            return buf;
          },
          [](const QDateTime &v) -> std::string {
            return E2ToStdString(时间到文本(v, 1));
          },
          [](bool v) -> std::string { return v ? "真" : "假"; },
          [](const std::string &v) -> std::string { return v; },
          [](const E2CppByteArray &v) -> std::string {
            return "[字节集:" + std::to_string(v.size()) + "字节]";
          },
          [](void *v) -> std::string {
            char buf[64];
            std::snprintf(buf, sizeof(buf), "[指针:%p]", v);
            return buf;
          }},
      val.value);
}

/**
 * @brief 信息框 (MsgBox)
 */
inline std::int32_t 信息框(const E2CppValue &提示信息, std::int32_t 按钮 = 0,
                           const std::string &窗口标题 = "信息：",
                           void *父窗口 = nullptr) {
  std::string strPrompt = E2CppValueToString2(提示信息);

#ifdef QT_WIDGETS_LIB
  QMessageBox::StandardButton defaultBtn = QMessageBox::Ok;
  QMessageBox::StandardButtons btns = QMessageBox::Ok;

  // 易语言按钮常量映射
  switch (按钮 & 0x0F) {
  case 0: btns = QMessageBox::Ok; break;
  case 1: btns = QMessageBox::Ok | QMessageBox::Cancel; break;
  case 2: btns = QMessageBox::Abort | QMessageBox::Retry | QMessageBox::Ignore; break;
  case 3: btns = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel; break;
  case 4: btns = QMessageBox::Yes | QMessageBox::No; break;
  case 5: btns = QMessageBox::Retry | QMessageBox::Cancel; break;
  }

  QMessageBox::Icon icon = QMessageBox::NoIcon;
  switch ((按钮 >> 4) & 0x0F) {
  case 1: icon = QMessageBox::Critical; break;
  case 2: icon = QMessageBox::Question; break;
  case 3: icon = QMessageBox::Warning; break;
  case 4: icon = QMessageBox::Information; break;
  }

  QWidget *parent = reinterpret_cast<QWidget *>(父窗口);
  QMessageBox mb(icon,
                 QString::fromStdString(窗口标题),
                 QString::fromStdString(strPrompt),
                 btns, parent);
  int ret = mb.exec();

  // 映射 QMessageBox 返回值到易语言返回值 (result - 1)
  switch (ret) {
  case QMessageBox::Ok:      return 0;
  case QMessageBox::Cancel:  return 1;
  case QMessageBox::Abort:   return 2;
  case QMessageBox::Retry:   return 3;
  case QMessageBox::Ignore:  return 4;
  case QMessageBox::Yes:     return 5;
  case QMessageBox::No:      return 6;
  default:                   return -1;
  }
#elif defined(_WIN32)
  int result = ::MessageBoxA((HWND)父窗口, strPrompt.c_str(), 窗口标题.c_str(),
                             static_cast<UINT>(按钮));
  if (result > 0)
    return static_cast<std::int32_t>(result - 1);
  return -1;
#else
  std::cout << 窗口标题 << ": " << strPrompt << std::endl;
  return 0;
#endif
}

inline std::int32_t 信息框(const E2CppValue &提示信息, std::int32_t 按钮,
                           const QString &窗口标题,
                           void *父窗口 = nullptr) {
  return 信息框(提示信息, 按钮, E2ToStdString(窗口标题), 父窗口);
}

inline std::int32_t 信息框(const E2CppValue &提示信息, std::int32_t 按钮,
                           const char *窗口标题,
                           void *父窗口 = nullptr) {
  return 信息框(提示信息, 按钮,
                窗口标题 ? std::string(窗口标题) : std::string(),
                父窗口);
}

/**
 * @brief 输入框 (InputBox)
 */
template <typename V>
inline bool 输入框(const std::string &提示信息 = "",
                   const std::string &窗口标题 = "请输入：",
                   const std::string &初始文本 = "",
                   V &存放输入内容的变量 = V{},
                   std::int32_t 输入方式 = 1,
                   void *父窗口 = nullptr) {
#ifdef QT_WIDGETS_LIB
  QWidget *parent = reinterpret_cast<QWidget *>(父窗口);
  bool ok = false;
  QString result;

  if (输入方式 == 4) {
    // 密码输入
    result = QInputDialog::getText(
        parent,
        QString::fromStdString(窗口标题),
        QString::fromStdString(提示信息),
        QLineEdit::Password,
        QString::fromStdString(初始文本),
        &ok);
  } else if (输入方式 == 2) {
    // 整数输入
    int val = QInputDialog::getInt(
        parent,
        QString::fromStdString(窗口标题),
        QString::fromStdString(提示信息),
        初始文本.empty() ? 0 : std::stoi(初始文本),
        INT_MIN, INT_MAX, 1, &ok);
    if (ok) {
      if constexpr (std::is_arithmetic_v<V>)
        存放输入内容的变量 = static_cast<V>(val);
      else if constexpr (std::is_same_v<V, std::string>)
        存放输入内容的变量 = std::to_string(val);
    }
    return ok;
  } else if (输入方式 == 3) {
    // 小数输入
    double val = QInputDialog::getDouble(
        parent,
        QString::fromStdString(窗口标题),
        QString::fromStdString(提示信息),
        初始文本.empty() ? 0.0 : std::stod(初始文本),
        -2147483647.0, 2147483647.0, 6, &ok);
    if (ok) {
      if constexpr (std::is_arithmetic_v<V>)
        存放输入内容的变量 = static_cast<V>(val);
      else if constexpr (std::is_same_v<V, std::string>)
        存放输入内容的变量 = std::to_string(val);
    }
    return ok;
  } else {
    result = QInputDialog::getText(
        parent,
        QString::fromStdString(窗口标题),
        QString::fromStdString(提示信息),
        QLineEdit::Normal,
        QString::fromStdString(初始文本),
        &ok);
  }

  if (ok) {
    std::string text = result.toStdString();
    if constexpr (std::is_same_v<V, std::string>)
      存放输入内容的变量 = text;
    else if constexpr (std::is_same_v<V, QString>)
      存放输入内容的变量 = result;
    else if constexpr (std::is_arithmetic_v<V>) {
      try {
        if constexpr (std::is_floating_point_v<V>)
          存放输入内容的变量 = static_cast<V>(std::stod(text));
        else
          存放输入内容的变量 = static_cast<V>(std::stoll(text));
      } catch (...) {
        存放输入内容的变量 = 0;
      }
    }
  }
  return ok;

#elif defined(_WIN32)
  // Windows fallback: Win32 InputBox
  HWND hWndParent = reinterpret_cast<HWND>(父窗口);
  HINSTANCE hInstance = ::GetModuleHandleA(nullptr);
  const char *className = "E2CppInputBoxClass";

  struct InputBoxState {
    std::string resultText;
    bool isConfirmed = false;
    HWND hEdit = nullptr;
    HWND hBtnOk = nullptr;
    HWND hBtnCancel = nullptr;
  };

  struct Procs {
    static LRESULT CALLBACK EditProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam, UINT_PTR, DWORD_PTR dwRefData) {
      InputBoxState *state = reinterpret_cast<InputBoxState *>(dwRefData);
      if (uMsg == WM_KEYDOWN) {
        if (wParam == VK_RETURN) {
          ::SendMessageA(::GetParent(hWnd), WM_COMMAND, MAKEWPARAM(1, BN_CLICKED), (LPARAM)state->hBtnOk);
          return 0;
        } else if (wParam == VK_ESCAPE) {
          ::SendMessageA(::GetParent(hWnd), WM_COMMAND, MAKEWPARAM(2, BN_CLICKED), (LPARAM)state->hBtnCancel);
          return 0;
        }
      }
      return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
      InputBoxState *state = reinterpret_cast<InputBoxState *>(::GetWindowLongPtrA(hWnd, GWLP_USERDATA));
      switch (message) {
      case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
          char buf[4096] = {0};
          ::GetWindowTextA(state->hEdit, buf, sizeof(buf));
          state->resultText = buf;
          state->isConfirmed = true;
          ::DestroyWindow(hWnd);
        } else if (LOWORD(wParam) == 2) {
          state->isConfirmed = false;
          ::DestroyWindow(hWnd);
        }
        break;
      case WM_CLOSE:
        state->isConfirmed = false;
        ::DestroyWindow(hWnd);
        break;
      case WM_DESTROY:
        ::PostQuitMessage(0);
        break;
      default:
        return ::DefWindowProcA(hWnd, message, wParam, lParam);
      }
      return 0;
    }
  };

  static bool classRegistered = false;
  if (!classRegistered) {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = Procs::WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = ::LoadCursorA(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszClassName = className;
    ::RegisterClassA(&wc);
    classRegistered = true;
  }

  int screenW = ::GetSystemMetrics(SM_CXSCREEN);
  int screenH = ::GetSystemMetrics(SM_CYSCREEN);
  int winW = 350, winH = 180;
  HWND hMainWnd = ::CreateWindowExA(
      WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, className, 窗口标题.c_str(),
      WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
      (screenW - winW) / 2, (screenH - winH) / 2, winW, winH,
      hWndParent, nullptr, hInstance, nullptr);

  InputBoxState state;
  ::SetWindowLongPtrA(hMainWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&state));

  HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
  HWND hStatic = ::CreateWindowExA(0, "STATIC", 提示信息.c_str(),
                                   WS_CHILD | WS_VISIBLE | SS_LEFT,
                                   15, 15, 305, 40, hMainWnd, nullptr, hInstance, nullptr);
  ::SendMessageA(hStatic, WM_SETFONT, (WPARAM)hFont, FALSE);

  DWORD editStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL;
  if (输入方式 == 2) editStyle |= ES_NUMBER;
  if (输入方式 == 4) editStyle |= ES_PASSWORD;

  state.hEdit = ::CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", 初始文本.c_str(),
                                  editStyle, 15, 60, 305, 25,
                                  hMainWnd, nullptr, hInstance, nullptr);
  ::SendMessageA(state.hEdit, WM_SETFONT, (WPARAM)hFont, FALSE);

  state.hBtnOk = ::CreateWindowExA(0, "BUTTON", "确认",
                                   WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                                   155, 100, 75, 28, hMainWnd, (HMENU)1, hInstance, nullptr);
  ::SendMessageA(state.hBtnOk, WM_SETFONT, (WPARAM)hFont, FALSE);

  state.hBtnCancel = ::CreateWindowExA(0, "BUTTON", "取消",
                                       WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                       240, 100, 75, 28, hMainWnd, (HMENU)2, hInstance, nullptr);
  ::SendMessageA(state.hBtnCancel, WM_SETFONT, (WPARAM)hFont, FALSE);

  ::SetWindowSubclass(state.hEdit, Procs::EditProc, 1, reinterpret_cast<DWORD_PTR>(&state));

  if (hWndParent) ::EnableWindow(hWndParent, FALSE);
  ::SetFocus(state.hEdit);
  ::SendMessageA(state.hEdit, EM_SETSEL, 0, -1);

  MSG msg;
  while (::GetMessageA(&msg, nullptr, 0, 0) > 0) {
    if (!::IsDialogMessageA(hMainWnd, &msg)) {
      ::TranslateMessage(&msg);
      ::DispatchMessageA(&msg);
    }
  }

  if (hWndParent) {
    ::EnableWindow(hWndParent, TRUE);
    ::SetForegroundWindow(hWndParent);
  }

  if (state.isConfirmed) {
    if constexpr (std::is_same_v<V, std::string>)
      存放输入内容的变量 = state.resultText;
    else if constexpr (std::is_same_v<V, QString>)
      存放输入内容的变量 = QString::fromStdString(state.resultText);
    else if constexpr (std::is_arithmetic_v<V>) {
      try {
        if constexpr (std::is_floating_point_v<V>)
          存放输入内容的变量 = static_cast<V>(std::stod(state.resultText));
        else
          存放输入内容的变量 = static_cast<V>(std::stoll(state.resultText));
      } catch (...) {
        存放输入内容的变量 = 0;
      }
    }
  }
  return state.isConfirmed;

#else
  std::cout << 窗口标题 << " - " << 提示信息 << " [" << 初始文本 << "]: ";
  std::string input;
  if (std::getline(std::cin, input)) {
    if constexpr (std::is_same_v<V, std::string>)
      存放输入内容的变量 = input;
    else if constexpr (std::is_same_v<V, QString>)
      存放输入内容的变量 = QString::fromStdString(input);
    return true;
  }
  return false;
#endif
}

template <typename V>
inline bool 输入框(const std::string &提示信息,
                   const QString &窗口标题,
                   const std::string &初始文本,
                   V &存放输入内容的变量,
                   std::int32_t 输入方式 = 1,
                   void *父窗口 = nullptr) {
  return 输入框(提示信息, E2ToStdString(窗口标题), 初始文本,
              存放输入内容的变量, 输入方式, 父窗口);
}

template <typename V>
inline bool 输入框(const char *提示信息,
                   const QString &窗口标题,
                   const char *初始文本,
                   V &存放输入内容的变量,
                   std::int32_t 输入方式 = 1,
                   void *父窗口 = nullptr) {
  return 输入框(提示信息 ? std::string(提示信息) : std::string(),
              窗口标题,
              初始文本 ? std::string(初始文本) : std::string(),
              存放输入内容的变量, 输入方式, 父窗口);
}

template <typename V>
inline bool 输入框(const char *提示信息,
                   const char *窗口标题,
                   const char *初始文本,
                   V &存放输入内容的变量,
                   std::int32_t 输入方式 = 1,
                   void *父窗口 = nullptr) {
  return 输入框(提示信息 ? std::string(提示信息) : std::string(),
              窗口标题 ? std::string(窗口标题) : std::string(),
              初始文本 ? std::string(初始文本) : std::string(),
              存放输入内容的变量, 输入方式, 父窗口);
}

/**
 * @brief 鸣叫
 */
inline void 鸣叫() {
#ifdef QT_WIDGETS_LIB
  QApplication::beep();
#elif defined(QT_GUI_LIB)
  QGuiApplication::beep();
#elif defined(_WIN32)
  Beep(750, 300);
#else
  std::cout << '\a' << std::flush;
#endif
}

/**
 * @brief 置等待鼠标
 */
inline void 置等待鼠标() {
#ifdef QT_GUI_LIB
  QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#elif defined(_WIN32)
  SetCursor(LoadCursor(NULL, IDC_WAIT));
#endif
}

/**
 * @brief 恢复鼠标
 */
inline void 恢复鼠标() {
#ifdef QT_GUI_LIB
  QGuiApplication::restoreOverrideCursor();
#elif defined(_WIN32)
  SetCursor(LoadCursor(NULL, IDC_ARROW));
#endif
}

/**
 * @brief 取操作系统类别
 */
inline int 取操作系统类别() {
#ifdef _WIN32
  return 3;
#else
  return 4;
#endif
}

/**
 * @brief 取操作系统类别2
 * @note 返回值: 0-Unknown, 1-Win9x, 2-WinNT, 3-Win2000, 4-WinXP,
 *              5-Win2003, 6-Vista, 7-Win7, 8-Win8, 9-Win10+, 10-Linux
 */
inline int 取操作系统类别2() {
#ifdef _WIN32
  HMODULE hMod = ::GetModuleHandleA("ntdll.dll");
  if (hMod) {
    RtlGetVersion_FUNC func =
        (RtlGetVersion_FUNC)::GetProcAddress(hMod, "RtlGetVersion");
    if (func) {
      OSVERSIONINFOW rovi = {0};
      rovi.dwOSVersionInfoSize = sizeof(rovi);
      if (func(&rovi) == 0) {
        DWORD major = rovi.dwMajorVersion;
        DWORD minor = rovi.dwMinorVersion;
        if (major == 10) return 9;
        if (major == 6) {
          if (minor == 2 || minor == 3) return 8;
          if (minor == 1) return 7;
          if (minor == 0) return 6;
        }
        if (major == 5) {
          if (minor == 2) return 5;
          if (minor == 1) return 4;
          if (minor == 0) return 3;
        }
        if (major <= 4) return 2;
      }
    }
  }
  OSVERSIONINFOA osvi = {0};
  osvi.dwOSVersionInfoSize = sizeof(osvi);
#pragma warning(suppress : 4996)
  if (::GetVersionExA(&osvi)) {
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
      return 1;
  }
  return 0;
#elif defined(__linux__)
  return 10;
#else
  return 0;
#endif
}

/**
 * @brief 读配置项 — 使用 QSettings 实现跨平台 INI 读取
 */
#ifdef QT_CORE_LIB
inline QString 读配置项(const QString &配置文件名, const QString &节名称,
                        const QString &配置项名称,
                        const QString &默认文本 = QString()) {
  if (配置项名称.isEmpty())
    return 默认文本;
  QSettings settings(配置文件名, QSettings::IniFormat);
  settings.beginGroup(节名称);
  QString val = settings.value(配置项名称, 默认文本).toString();
  settings.endGroup();
  return val;
}
#else
inline std::string 读配置项(const std::string &配置文件名,
                            const std::string &节名称,
                            const std::string &配置项名称,
                            const std::string &默认文本 = "") {
#ifdef _WIN32
  char buffer[2048];
  char absPath[MAX_PATH];
  if (GetFullPathNameA(配置文件名.c_str(), MAX_PATH, absPath, NULL) == 0)
    return 默认文本;
  DWORD ret = GetPrivateProfileStringA(
      节名称.c_str(), 配置项名称.empty() ? NULL : 配置项名称.c_str(),
      默认文本.c_str(), buffer, sizeof(buffer), absPath);
  if (ret > 0)
    return std::string(buffer, ret);
#endif
  return 默认文本;
}
#endif

/**
 * @brief 写配置项 — 使用 QSettings 实现跨平台 INI 写入
 */
#ifdef QT_CORE_LIB
inline bool 写配置项(const QString &配置文件名, const QString &节名称,
                     const QString &配置项名称, const QString &欲写入值) {
  QSettings settings(配置文件名, QSettings::IniFormat);
  settings.beginGroup(节名称);
  settings.setValue(配置项名称, 欲写入值);
  settings.endGroup();
  settings.sync();
  return settings.status() == QSettings::NoError;
}

inline bool 写配置项(const QString &配置文件名, const QString &节名称) {
  return 写配置项(配置文件名, 节名称, QString(), QString());
}

inline bool 写配置项(const QString &配置文件名, const QString &节名称,
                     const QString &配置项名称) {
  return 写配置项(配置文件名, 节名称, 配置项名称, QString());
}

inline bool 写配置项(const std::string &配置文件名, const std::string &节名称,
                     const std::string &配置项名称, const std::string &欲写入值) {
  return 写配置项(QString::fromStdString(配置文件名),
                  QString::fromStdString(节名称),
                  QString::fromStdString(配置项名称),
                  QString::fromStdString(欲写入值));
}
#else
inline bool 写配置项(const std::string &配置文件名, const std::string &节名称,
                     const std::string &配置项名称, const std::string &欲写入值) {
#ifdef _WIN32
  char absPath[MAX_PATH];
  if (GetFullPathNameA(配置文件名.c_str(), MAX_PATH, absPath, NULL) == 0)
    return false;
  return WritePrivateProfileStringA(节名称.empty() ? NULL : 节名称.c_str(),
                                    配置项名称.empty() ? NULL : 配置项名称.c_str(),
                                    欲写入值.c_str(), absPath) != 0;
#else
  return false;
#endif
}
#endif

inline bool 写配置项(const std::string &配置文件名, const std::string &节名称) {
  return 写配置项(配置文件名, 节名称, std::string(), std::string());
}

inline bool 写配置项(const std::string &配置文件名, const std::string &节名称,
                     const std::string &配置项名称) {
  return 写配置项(配置文件名, 节名称, 配置项名称, std::string());
}

/**
 * @brief 取配置节名 — 使用 QSettings 实现跨平台 INI 节枚举
 */
#ifdef QT_CORE_LIB
inline E2CppArray1D<QString> 取配置节名(const QString &配置文件名) {
  E2CppArray1D<QString> res;
  QSettings settings(配置文件名, QSettings::IniFormat);
  for (const QString &group : settings.childGroups())
    res.data.push_back(group);
  res.dims = {static_cast<std::int32_t>(res.data.size())};
  return res;
}
#else
inline E2CppArray1D<std::string> 取配置节名(const std::string &配置文件名) {
  E2CppArray1D<std::string> res;
#ifdef _WIN32
  char absPath[MAX_PATH];
  if (GetFullPathNameA(配置文件名.c_str(), MAX_PATH, absPath, NULL) == 0)
    return res;
  char buffer[4096];
  DWORD ret = GetPrivateProfileStringA(NULL, NULL, "", buffer, sizeof(buffer), absPath);
  if (ret > 0) {
    for (DWORD i = 0; i < ret;) {
      std::string sectionName = &buffer[i];
      if (!sectionName.empty())
        res.data.push_back(sectionName);
      i += sectionName.length() + 1;
    }
  }
#endif
  res.dims = {static_cast<std::int32_t>(res.data.size())};
  return res;
}
#endif

/**
 * @brief 多文件对话框
 */
inline std::string
多文件对话框(const std::string &标题, const std::string &过滤器,
             std::int32_t 初始过滤器 = 0, const std::string &初始目录 = "",
             bool 不改变目录 = true, void *父窗口 = nullptr) {
#ifdef QT_WIDGETS_LIB
  QWidget *parent = reinterpret_cast<QWidget *>(父窗口);
  QString filter = QString::fromStdString(过滤器);
  QString dir = QString::fromStdString(初始目录);
  QStringList files = QFileDialog::getOpenFileNames(
      parent, QString::fromStdString(标题), dir, filter);
  return files.join(QLatin1Char('\n')).toStdString();
#else
  return "";
#endif
}

/**
 * @brief 取程序名称
 */
inline std::string 取程序名称(std::int32_t 欲取的程序名称类型) {
#ifdef QT_CORE_LIB
  QString appPath = QCoreApplication::applicationFilePath();
  if (欲取的程序名称类型 == 1)
    return appPath.toStdString();
  QString name = QFileInfo(appPath).fileName();
  if (欲取的程序名称类型 == 3)
    return name.toStdString();
  if (欲取的程序名称类型 == 2)
    return QFileInfo(appPath).baseName().toStdString();
#elif defined(_WIN32)
  char szPath[MAX_PATH];
  GetModuleFileNameA(NULL, szPath, MAX_PATH);
  std::string path(szPath);
  if (欲取的程序名称类型 == 1)
    return path;
  size_t lastSlash = path.find_last_of("\\/");
  std::string name = (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);
  if (欲取的程序名称类型 == 3)
    return name;
  if (欲取的程序名称类型 == 2) {
    size_t lastDot = name.find_last_of('.');
    if (lastDot != std::string::npos)
      return name.substr(0, lastDot);
    return name;
  }
#endif
  return "";
}

/**
 * @brief 交换变量
 */
template <typename T> inline void 交换变量(T &变量1, T &变量2) {
  std::swap(变量1, 变量2);
}

/**
 * @brief 强制交换变量
 */
template <typename T> inline void 强制交换变量(T &变量1, T &变量2) {
  std::swap(变量1, 变量2);
}

/**
 * @brief 取运行时数据类型
 */
inline std::int32_t 取运行时数据类型(const E2CppValue &欲取其数据类型的数据) {
  return static_cast<std::int32_t>(欲取其数据类型的数据.value.index());
}

/**
 * @brief 宏
 */
inline void 宏(const std::string &编译插件名称,
               const E2CppValue &相关常量参数 = E2CppValue{}) {
}

/**
 * @brief 类自身
 */
inline E2CppValue 类自身() { return E2CppValue{}; }

/**
 * @brief 取最后错误 (Windows 专属)
 */
inline std::int32_t 取最后错误() {
#ifdef _WIN32
  return static_cast<std::int32_t>(GetLastError());
#else
  return 0;
#endif
}

/**
 * @brief 取统一文本
 * @note 易语言原始行为：GBK↔UTF-16LE 转换
 */
inline E2CppByteArray 取统一文本(const std::string &待转换常量文本,
                                 bool 转换到宽文本 = true,
                                 bool 添加结束零字符 = false) {
#ifdef _WIN32
  if (转换到宽文本) {
    int wlen = MultiByteToWideChar(CP_ACP, 0, 待转换常量文本.c_str(),
                                   static_cast<int>(待转换常量文本.size()),
                                   nullptr, 0);
    if (wlen <= 0)
      return E2CppByteArray{};
    std::vector<wchar_t> wbuf(wlen);
    MultiByteToWideChar(CP_ACP, 0, 待转换常量文本.c_str(),
                        static_cast<int>(待转换常量文本.size()), wbuf.data(), wlen);
    std::vector<uint8_t> raw;
    raw.reserve(wlen * 2 + (添加结束零字符 ? 2 : 0));
    for (int i = 0; i < wlen; ++i) {
      raw.push_back(static_cast<uint8_t>(wbuf[i] & 0xFF));
      raw.push_back(static_cast<uint8_t>((wbuf[i] >> 8) & 0xFF));
    }
    if (添加结束零字符) { raw.push_back(0); raw.push_back(0); }
    return E2CppByteArray(raw.begin(), raw.end());
  } else {
    const wchar_t *wptr = reinterpret_cast<const wchar_t *>(待转换常量文本.data());
    int wlen = static_cast<int>(待转换常量文本.size() / 2);
    int alen = WideCharToMultiByte(CP_ACP, 0, wptr, wlen, nullptr, 0, nullptr, nullptr);
    if (alen <= 0)
      return E2CppByteArray{};
    std::vector<char> abuf(alen);
    WideCharToMultiByte(CP_ACP, 0, wptr, wlen, abuf.data(), alen, nullptr, nullptr);
    std::vector<uint8_t> raw(abuf.begin(), abuf.end());
    if (添加结束零字符) raw.push_back(0);
    return E2CppByteArray(raw.begin(), raw.end());
  }
#else
  std::vector<uint8_t> raw(待转换常量文本.begin(), 待转换常量文本.end());
  if (添加结束零字符) raw.push_back(0);
  return E2CppByteArray(raw.begin(), raw.end());
#endif
}

#ifdef QT_CORE_LIB
inline QString 取文本注册项(std::int32_t 根目录, const QString &全路径注册项名,
                        const QString &默认文本 = QString()) {
  return QString::fromStdString(
      取文本注册项(根目录, E2ToStdString(全路径注册项名), E2ToStdString(默认文本)));
}

inline std::int32_t 取数值注册项(std::int32_t 根目录, const QString &全路径注册项名,
                             std::int32_t 默认数值 = 0) {
  return 取数值注册项(根目录, E2ToStdString(全路径注册项名), 默认数值);
}
#endif
