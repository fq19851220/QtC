#include "E2CppBase.h"

#include <algorithm>
#include <vector>

// cmdCategory: 6
// categoryCn: 数组操作

template <typename T, typename... Dims>
inline void 重定义数组(EQtArray<T> &欲重定义的数组变量,
                  逻辑型 是否保留以前的内容 = false,
                  整数型 数组对应维的上限值 = 0,
                  Dims... 其余各维上限值) {
  欲重定义的数组变量.dims = {数组对应维的上限值,
                        static_cast<整数型>(其余各维上限值)...};
  for (auto &项 : 欲重定义的数组变量.dims) {
    if (项 < 0) {
      项 = 0;
    }
  }
  欲重定义的数组变量.resizeFromDims(是否保留以前的内容);
}

template <typename T>
inline 整数型 取数组成员数(const EQtArray<T> &欲检查的变量) {
  return static_cast<整数型>(欲检查的变量.data.size());
}

template <typename T>
inline 整数型 取数组成员数(const T &) {
  return -1;
}

template <typename T>
inline 整数型 取数组下标(const EQtArray<T> &欲取某维最大下标的数组变量,
                  整数型 欲取其最大下标的维 = 1) {
  if (欲取其最大下标的维 < 1 ||
      欲取其最大下标的维 > 欲取某维最大下标的数组变量.dims.size()) {
    return 0;
  }
  return 欲取某维最大下标的数组变量.dims[欲取其最大下标的维 - 1];
}

template <typename T>
inline 整数型 取数组下标(const T &, 整数型 = 1) {
  return 0;
}

template <typename T>
inline void 复制数组(EQtArray<T> &复制到的数组变量,
                const EQtArray<T> &待复制的数组数据) {
  复制到的数组变量 = 待复制的数组数据;
}

template <typename T>
inline void 加入成员(EQtArray<T> &欲加入成员的数组变量,
                const T &欲加入的成员数据) {
  欲加入成员的数组变量.data.push_back(欲加入的成员数据);
  欲加入成员的数组变量.makeOneDim();
}

template <typename T, typename U,
          typename = std::enable_if_t<
              std::is_convertible_v<U, T> &&
              !std::is_same_v<std::decay_t<U>, T> &&
              !std::is_same_v<std::decay_t<U>, EQtArray<T>> &&
              !std::is_same_v<std::decay_t<U>, QList<T>>>>
inline void 加入成员(EQtArray<T> &欲加入成员的数组变量,
                const U &欲加入的成员数据) {
  加入成员(欲加入成员的数组变量, static_cast<T>(欲加入的成员数据));
}

inline void 加入成员(EQtArray<文本型> &欲加入成员的数组变量,
                const std::string &欲加入的成员数据) {
  加入成员(欲加入成员的数组变量, QString::fromStdString(欲加入的成员数据));
}

template <typename T>
inline void 加入成员(EQtArray<T> &欲加入成员的数组变量,
                const EQtArray<T> &欲加入的成员数据) {
  欲加入成员的数组变量.data.append(欲加入的成员数据.data);
  欲加入成员的数组变量.makeOneDim();
}

template <typename T>
inline void 加入成员(EQtArray<T> &欲加入成员的数组变量,
                const QList<T> &欲加入的成员数据) {
  欲加入成员的数组变量.data.append(欲加入的成员数据);
  欲加入成员的数组变量.makeOneDim();
}

template <typename T>
inline void 加入成员(EQtArray<T> &欲加入成员的数组变量,
                std::initializer_list<T> 欲加入的成员数据) {
  加入成员(欲加入成员的数组变量, EQtArray<T>(欲加入的成员数据));
}

template <typename T>
inline void 插入成员(EQtArray<T> &欲插入成员的数组变量,
                整数型 欲插入的位置,
                const T &欲插入的成员数据) {
  if (欲插入的位置 < 1 ||
      欲插入的位置 > static_cast<整数型>(欲插入成员的数组变量.data.size()) + 1) {
    return;
  }
  欲插入成员的数组变量.data.insert(欲插入的位置 - 1, 欲插入的成员数据);
  欲插入成员的数组变量.makeOneDim();
}

template <typename T, typename U,
          typename = std::enable_if_t<
              std::is_convertible_v<U, T> &&
              !std::is_same_v<std::decay_t<U>, T> &&
              !std::is_same_v<std::decay_t<U>, EQtArray<T>> &&
              !std::is_same_v<std::decay_t<U>, QList<T>>>>
inline void 插入成员(EQtArray<T> &欲插入成员的数组变量,
                整数型 欲插入的位置,
                const U &欲插入的成员数据) {
  插入成员(欲插入成员的数组变量, 欲插入的位置,
       static_cast<T>(欲插入的成员数据));
}

inline void 插入成员(EQtArray<文本型> &欲插入成员的数组变量,
                整数型 欲插入的位置,
                const std::string &欲插入的成员数据) {
  插入成员(欲插入成员的数组变量, 欲插入的位置,
       QString::fromStdString(欲插入的成员数据));
}

template <typename T>
inline void 插入成员(EQtArray<T> &欲插入成员的数组变量,
                整数型 欲插入的位置,
                const EQtArray<T> &欲插入的成员数据) {
  if (欲插入的位置 < 1 ||
      欲插入的位置 > static_cast<整数型>(欲插入成员的数组变量.data.size()) + 1) {
    return;
  }
  欲插入成员的数组变量.data.insert(欲插入的位置 - 1,
                            欲插入的成员数据.data.begin(),
                            欲插入的成员数据.data.end());
  欲插入成员的数组变量.makeOneDim();
}

template <typename T>
inline void 插入成员(EQtArray<T> &欲插入成员的数组变量,
                整数型 欲插入的位置,
                const QList<T> &欲插入的成员数据) {
  插入成员(欲插入成员的数组变量, 欲插入的位置,
       EQtArray<T>(欲插入的成员数据));
}

template <typename T>
inline void 插入成员(EQtArray<T> &欲插入成员的数组变量,
                整数型 欲插入的位置,
                std::initializer_list<T> 欲插入的成员数据) {
  插入成员(欲插入成员的数组变量, 欲插入的位置,
       EQtArray<T>(欲插入的成员数据));
}

template <typename T>
inline void 插入成员(EQtArray<T> &欲插入成员的数组变量,
                整数型 欲插入的位置,
                const E2CppLiteralArray &欲插入的成员数据) {
  插入成员(欲插入成员的数组变量, 欲插入的位置,
       static_cast<EQtArray<T>>(欲插入的成员数据));
}

template <typename T>
inline 整数型 删除成员(EQtArray<T> &欲删除成员的数组变量,
                  整数型 欲删除的位置,
                  整数型 欲删除的成员数目 = 1) {
  if (欲删除的位置 < 1 ||
      欲删除的位置 > static_cast<整数型>(欲删除成员的数组变量.data.size()) ||
      欲删除的成员数目 <= 0) {
    return 0;
  }
  const 整数型 实际删除数 =
      (std::min)(欲删除的成员数目,
                 static_cast<整数型>(欲删除成员的数组变量.data.size()) -
                     欲删除的位置 + 1);
  欲删除成员的数组变量.data.erase(
      欲删除成员的数组变量.data.begin() + (欲删除的位置 - 1),
      欲删除成员的数组变量.data.begin() + (欲删除的位置 - 1 + 实际删除数));
  欲删除成员的数组变量.makeOneDim();
  return 实际删除数;
}

template <typename T>
inline void 清除数组(EQtArray<T> &欲删除成员的数组变量) {
  欲删除成员的数组变量.data.clear();
  欲删除成员的数组变量.dims = {0};
}

template <typename T>
inline void 数组排序(EQtArray<T> &数值数组变量,
                逻辑型 排序方向是否为从小到大 = true) {
  std::sort(数值数组变量.data.begin(), 数值数组变量.data.end());
  if (!排序方向是否为从小到大) {
    std::reverse(数值数组变量.data.begin(), 数值数组变量.data.end());
  }
}

template <typename T>
inline void 数组清零(EQtArray<T> &数值数组变量) {
  for (auto &项 : 数值数组变量.data) {
    项 = T{};
  }
}

template <typename T>
inline void 数组_反转(EQtArray<T> &数组变量) {
  std::reverse(数组变量.data.begin(), 数组变量.data.end());
}
