#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#ifdef QT_CORE_LIB
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtGui/QFont>
#include <QtCore/QString>
#if defined(Q_OS_WIN) && defined(__has_include)
#if __has_include(<ActiveQt/QAxObject>)
#include <ActiveQt/QAxObject>
#endif
#endif
#endif

template <typename T> struct E2CppArray;
#ifdef QT_CORE_LIB
template <typename T> struct EQtArray;
#endif

// Type trait to detect E2CppArray accessors
template <typename T, typename = void>
struct is_e2cpp_accessor : std::false_type {};

template <typename T>
struct is_e2cpp_accessor<T, std::void_t<typename T::E2CPP_ARRAY_ACCESSOR_TAG>>
    : std::true_type {};

template <typename T> struct is_e2cpp_array : std::false_type {};
template <typename T> struct is_e2cpp_array<E2CppArray<T>> : std::true_type {};
#ifdef QT_CORE_LIB
template <typename T> struct is_e2cpp_array<EQtArray<T>> : std::true_type {};
#endif

// overloaded helper for std::visit
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

/**
 */
template <typename T> struct E2CppArray {
  std::vector<T> data;           
  std::vector<std::int32_t> dims; 

  E2CppArray() = default;

  explicit E2CppArray(const std::vector<T> &vec) : data(vec) {
    dims = {static_cast<std::int32_t>(data.size())};
  }

  explicit E2CppArray(std::vector<T> &&vec) noexcept : data(std::move(vec)) {
    dims = {static_cast<std::int32_t>(data.size())};
  }

  /**
   * @brief 支持易语言大括号初始化（如 E2CppArray<int> a = {1,2,
   * 3
   * })
   */
  E2CppArray(std::initializer_list<T> list) : data(list) {
    dims = {static_cast<std::int32_t>(data.size())};
  }

  /**
   * @brief
* 变长参数构造函数，支持任意维度的数组初始化
* @param d 每一维的大小列表
   */
  template <typename... Args> explicit E2CppArray(Args... d) {
    dims = {static_cast<std::int32_t>(d)...};
    std::size_t total = 0;
    if (!dims.empty()) {
      total = 1;
      for (auto v : dims) {
        if (v < 0) v = 0;
        total *= static_cast<std::size_t>(v);
      }
    }
    data.assign(total, T());
  }

  /**
   * @brief 支持易语言大括号赋值(如 数组 = { 1, 2,
   * 3
   * })
   */
  E2CppArray &operator=(std::initializer_list<T> list) {
    data.assign(list.begin(), list.end());
    // 赋值后自动更新为一维数组，维度大小为列表长度
    dims = {static_cast<std::int32_t>(data.size())};
    return *this;
  }

  E2CppArray &operator=(const std::vector<T> &vec) {
    data = vec;
    dims = {static_cast<std::int32_t>(data.size())};
    return *this;
  }

  E2CppArray &operator=(std::vector<T> &&vec) noexcept {
    data = std::move(vec);
    dims = {static_cast<std::int32_t>(data.size())};
    return *this;
  }

  struct ConstAccessor;

  /**
* @brief 多维数组访问代理类
* 实现逻辑：自动将易语言 1-base
* 下标转为 0-base，并计算多维偏移。
   */
  struct Accessor {
    E2CppArray *parent;
    std::size_t offset;
    std::size_t dim_idx;

    /**
* @brief 连续方括号访问
* @param idx 易语言风格下标 (1 到 N)
     */
    using value_type = T;
    using E2CPP_ARRAY_ACCESSOR_TAG = void;

    Accessor operator[](std::int32_t idx) {
      if (parent->dims.empty()) {
        throw std::out_of_range("鏁扮粍灏氭湭鍒濆鍖?绌虹淮搴?");
      }
      if (dim_idx >= parent->dims.size())
        throw std::out_of_range("缁村害瓒呭嚭鑼冨洿");
      // 允许在 1D
      // 数组或作为最后一个维度时进行安全的平均访问边界检查
      if (idx < 1 || idx > parent->dims[dim_idx]) {
        throw std::out_of_range("鏁扮粍涓嬫爣瓒婄晫");
      }
      std::size_t stride = 1;
      // 计算当前维度的步长
      for (std::size_t i = dim_idx + 1; i < parent->dims.size(); ++i) {
        stride *= parent->dims[i];
      }
      std::size_t new_offset = offset + (std::size_t)(idx - 1) * stride;
      return Accessor{parent, new_offset, dim_idx + 1};
    }

    /// 隐式转换为原类型的引用，支持 a[1]=1
    /// 赋值
    operator T &() const { return parent->data[offset]; }
    /// 赋值操作符
    T &operator=(const T &val) {

      parent->data[offset] = val;
      return parent->data[offset];
    }
    T &operator=(std::initializer_list<std::int32_t> list) {
      (void)list;
      parent->data[offset] = T{};
      return parent->data[offset];
    }
    /// 解决 Proxy = Proxy
    /// 导致默认拷贝的死循环问题
    Accessor &operator=(const Accessor &other) {
      parent->data[offset] = (const T &)
          other; // 强制读取右侧的真实数据，写给左侧
      return *this;
    }

    /// 解决 Proxy = ConstProxy 的赋值问题
    Accessor &operator=(const ConstAccessor &other) {
      parent->data[offset] = (const T &)other;
      return *this;
    }
    /// 取地址操作符，支持从 E2CppValue 转换
    T *operator&() const { return &parent->data[offset]; }
    /// 指针访问操作符，支持 a[1]->member 访问
    T *operator->() const { return &parent->data[offset]; }
    /// 显式引用获取
    T &get() const { return parent->data[offset]; }

    // 辅助拼接：解决 std::string + Accessor 无法自动推导
    // operator+ 的问题
    template <typename U, typename TT = T,
              std::enable_if_t<!std::is_same_v<TT, QString>, int> = 0>
    auto operator+(const U &other) const {
      return (T &)(*this) + other;
    }
    template <typename U, typename TT = T,
              std::enable_if_t<!std::is_same_v<TT, QString>, int> = 0>
    friend auto operator+(const U &left, const Accessor &right) {
      return left + (T &)right;
    }
    template <typename TT = T,
              std::enable_if_t<!std::is_same_v<TT, QString>, int> = 0>
    friend auto operator+(const Accessor &left, const Accessor &right) {
      return (T &)left + (T &)right;
    }
    template <typename TT = T,
              std::enable_if_t<!std::is_same_v<TT, QString>, int> = 0>
    friend auto operator+(const Accessor &left, const ConstAccessor &right) {
      return (T &)left + (const T &)right;
    }
    template <typename TT = T,
              std::enable_if_t<!std::is_same_v<TT, QString>, int> = 0>
    friend auto operator+(const ConstAccessor &left, const Accessor &right) {
      return (const T &)left + (T &)right;
    }
    template <typename U> T &operator+=(const U &other) {
      return (T &)(*this) += other;
    }
    // 比较运算符
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    bool operator==(const U &other) const {
      return (const T &)(*this) == other;
    }
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    bool operator!=(const U &other) const {
      return (const T &)(*this) != other;
    }
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    bool operator<(const U &other) const {
      return (const T &)(*this) < other;
    }
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    bool operator>(const U &other) const {
      return (const T &)(*this) > other;
    }
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    bool operator<=(const U &other) const {
      return (const T &)(*this) <= other;
    }
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    bool operator>=(const U &other) const {
      return (const T &)(*this) >= other;
    }

    // 反向比较 (C++17: U == Accessor, U != Accessor)
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    friend bool operator==(const U &left, const Accessor &right) {
      return left == (const T &)right;
    }
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    friend bool operator!=(const U &left, const Accessor &right) {
      return left != (const T &)right;
    }

    // 解决比较两个 Accessor/ConstAccessor 时的歧义
    bool operator==(const Accessor &other) const {
      return (const T &)(*this) == (const T &)other;
    }
    bool operator!=(const Accessor &other) const { return !(*this == other); }
    bool operator<(const Accessor &other) const {
      return (const T &)(*this) < (const T &)other;
    }
    bool operator>(const Accessor &other) const {
      return (const T &)(*this) > (const T &)other;
    }
    bool operator<=(const Accessor &other) const {
      return (const T &)(*this) <= (const T &)other;
    }
    bool operator>=(const Accessor &other) const {
      return (const T &)(*this) >= (const T &)other;
    }

    bool operator==(const ConstAccessor &other) const {
      return (const T &)(*this) == (const T &)other;
    }
    bool operator!=(const ConstAccessor &other) const {
      return !(*this == other);
    }
    bool operator<(const ConstAccessor &other) const {
      return (const T &)(*this) < (const T &)other;
    }
    bool operator>(const ConstAccessor &other) const {
      return (const T &)(*this) > (const T &)other;
    }
    bool operator<=(const ConstAccessor &other) const {
      return (const T &)(*this) <= (const T &)other;
    }
    bool operator>=(const ConstAccessor &other) const {
      return (const T &)(*this) >= (const T &)other;
    }
  };

  /**
   * @brief 常量数组访问代理类
   */
  struct ConstAccessor {
    using value_type = T;
    using E2CPP_ARRAY_ACCESSOR_TAG = void;

    const E2CppArray *parent;
    std::size_t offset;
    std::size_t dim_idx;

    ConstAccessor operator[](std::int32_t idx) const {
      if (parent->dims.empty()) {
        throw std::out_of_range("数组尚未初始化(空维度)");
      }
      if (dim_idx >= parent->dims.size())
        throw std::out_of_range("维度超出范围");
      if (idx < 1 || idx > parent->dims[dim_idx])
        throw std::out_of_range("数组下标越界");
      std::size_t stride = 1;
      for (std::size_t i = dim_idx + 1; i < parent->dims.size(); ++i) {
        stride *= parent->dims[i];
      }
      return ConstAccessor{parent, offset + (std::size_t)(idx - 1) * stride,
                           dim_idx + 1};
    }
    operator const T &() const { return parent->data[offset]; }

    template <typename U, typename TT = T,
              std::enable_if_t<!std::is_same_v<TT, QString>, int> = 0>
    auto operator+(const U &other) const {
      return (const T &)(*this) + other;
    }
    template <typename U, typename TT = T,
              std::enable_if_t<!std::is_same_v<TT, QString>, int> = 0>
    friend auto operator+(const U &left, const ConstAccessor &right) {
      return left + (const T &)right;
    }
    template <typename TT = T,
              std::enable_if_t<!std::is_same_v<TT, QString>, int> = 0>
    friend auto operator+(const ConstAccessor &left,
                          const ConstAccessor &right) {
      return (const T &)left + (const T &)right;
    }
    // 比较运算符
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    bool operator==(const U &other) const {
      return (const T &)(*this) == other;
    }
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    bool operator!=(const U &other) const {
      return (const T &)(*this) != other;
    }
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    bool operator<(const U &other) const {
      return (const T &)(*this) < other;
    }
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    bool operator>(const U &other) const {
      return (const T &)(*this) > other;
    }
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    bool operator<=(const U &other) const {
      return (const T &)(*this) <= other;
    }
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    bool operator>=(const U &other) const {
      return (const T &)(*this) >= other;
    }

    // 反向比较
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    friend bool operator==(const U &left, const ConstAccessor &right) {
      return left == (const T &)right;
    }
    template <typename U, typename std::enable_if<!is_e2cpp_accessor<U>::value,
                                                  int>::type = 0>
    friend bool operator!=(const U &left, const ConstAccessor &right) {
      return left != (const T &)right;
    }

    // 解决比较两个 Accessor/ConstAccessor 时的歧义
    bool operator==(const Accessor &other) const {
      return (const T &)(*this) == (const T &)other;
    }
    bool operator!=(const Accessor &other) const { return !(*this == other); }
    bool operator==(const ConstAccessor &other) const {
      return (const T &)(*this) == (const T &)other;
    }
    bool operator!=(const ConstAccessor &other) const {
      return !(*this == other);
    }

    // 不等号重载(ConstAccessor 版本)
    bool operator<(const Accessor &other) const {
      return (const T &)(*this) < (const T &)other;
    }
    bool operator>(const Accessor &other) const {
      return (const T &)(*this) > (const T &)other;
    }
    bool operator<=(const Accessor &other) const {
      return (const T &)(*this) <= (const T &)other;
    }
    bool operator>=(const Accessor &other) const {
      return (const T &)(*this) >= (const T &)other;
    }

    bool operator<(const ConstAccessor &other) const {
      return (const T &)(*this) < (const T &)other;
    }
    bool operator>(const ConstAccessor &other) const {
      return (const T &)(*this) > (const T &)other;
    }
    bool operator<=(const ConstAccessor &other) const {
      return (const T &)(*this) <= (const T &)other;
    }
    bool operator>=(const ConstAccessor &other) const {
      return (const T &)(*this) >= (const T &)other;
    }

    const T *operator->() const { return &parent->data[offset]; }
    const T &get() const { return parent->data[offset]; }
  };


  Accessor operator[](std::int32_t idx) { return Accessor{this, 0, 0}[idx]; }

  ConstAccessor operator[](std::int32_t idx) const {
    return ConstAccessor{this, 0, 0}[idx];
  }


  std::size_t size() const { return data.size(); }
};

// Specialization: std::vector<bool> uses a proxy reference, not bool&.
// Redirect E2CppArray<bool> to use uint8_t storage so Accessor::operator T&()
// and operator= return a real reference without the proxy issue.
template <> struct E2CppArray<bool> : public E2CppArray<std::uint8_t> {
  using E2CppArray<std::uint8_t>::E2CppArray;
  using E2CppArray<std::uint8_t>::operator=;
  using E2CppArray<std::uint8_t>::operator[];
};

#ifdef QT_CORE_LIB
template <typename T> struct EQtArray {
  QList<T> data;
  QList<std::int32_t> dims;

  EQtArray() = default;

  explicit EQtArray(const QList<T> &vec) : data(vec) {
    dims = {static_cast<std::int32_t>(data.size())};
  }

  explicit EQtArray(QList<T> &&vec) noexcept : data(std::move(vec)) {
    dims = {static_cast<std::int32_t>(data.size())};
  }

  EQtArray(const E2CppArray<T> &other) {
    data.reserve(static_cast<qsizetype>(other.data.size()));
    for (const auto &item : other.data) {
      data.push_back(item);
    }
    dims.clear();
    for (auto dim : other.dims) {
      dims.push_back(dim);
    }
    if (dims.isEmpty()) {
      dims = {static_cast<std::int32_t>(data.size())};
    }
  }

  EQtArray(std::initializer_list<T> list) : data(list.begin(), list.end()) {
    dims = {static_cast<std::int32_t>(data.size())};
  }

  EQtArray &operator=(std::initializer_list<T> list) {
    data = QList<T>(list.begin(), list.end());
    dims = {static_cast<std::int32_t>(data.size())};
    return *this;
  }

  EQtArray &operator=(const QList<T> &list) {
    data = list;
    dims = {static_cast<std::int32_t>(data.size())};
    return *this;
  }

  EQtArray &operator=(QList<T> &&list) noexcept {
    data = std::move(list);
    dims = {static_cast<std::int32_t>(data.size())};
    return *this;
  }

  EQtArray &operator=(const E2CppArray<T> &other) {
    data.clear();
    data.reserve(static_cast<qsizetype>(other.data.size()));
    for (const auto &item : other.data) {
      data.push_back(item);
    }
    dims.clear();
    for (auto dim : other.dims) {
      dims.push_back(dim);
    }
    if (dims.isEmpty()) {
      dims = {static_cast<std::int32_t>(data.size())};
    }
    return *this;
  }

  template <typename... Args> explicit EQtArray(Args... d) {
    dims = {static_cast<std::int32_t>(d)...};
    resizeFromDims(false);
  }

  void resizeFromDims(bool keepExisting) {
    std::size_t total = 0;
    if (!dims.isEmpty()) {
      total = 1;
      for (auto v : dims) {
        if (v < 0) {
          v = 0;
        }
        total *= static_cast<std::size_t>(v);
      }
    }

    QList<T> newData;
    newData.resize(static_cast<qsizetype>(total));
    if (keepExisting) {
      const qsizetype keepCount =
          (std::min)(newData.size(), data.size());
      for (qsizetype i = 0; i < keepCount; ++i) {
        newData[i] = data[i];
      }
    }
    data = std::move(newData);
  }

  void makeOneDim() { dims = {static_cast<std::int32_t>(data.size())}; }

  std::size_t size() const { return static_cast<std::size_t>(data.size()); }

  struct ConstAccessor;

  struct Accessor {
    using value_type = T;
    using E2CPP_ARRAY_ACCESSOR_TAG = void;

    EQtArray *parent;
    std::size_t offset;
    std::size_t dim_idx;

    Accessor operator[](std::int32_t idx) {
      if (parent->dims.isEmpty()) {
        throw std::out_of_range("数组尚未初始化(空维度)");
      }
      if (dim_idx >= static_cast<std::size_t>(parent->dims.size())) {
        throw std::out_of_range("维度超出范围");
      }
      if (idx < 1 || idx > parent->dims[static_cast<qsizetype>(dim_idx)]) {
        throw std::out_of_range("数组下标越界");
      }
      std::size_t stride = 1;
      for (std::size_t i = dim_idx + 1;
           i < static_cast<std::size_t>(parent->dims.size()); ++i) {
        stride *= static_cast<std::size_t>(parent->dims[static_cast<qsizetype>(i)]);
      }
      return Accessor{parent, offset + static_cast<std::size_t>(idx - 1) * stride,
                      dim_idx + 1};
    }

    operator T &() const { return parent->data[static_cast<qsizetype>(offset)]; }
    T &operator=(const T &val) {
      parent->data[static_cast<qsizetype>(offset)] = val;
      return parent->data[static_cast<qsizetype>(offset)];
    }
    T &operator=(std::initializer_list<std::int32_t> list) {
      if constexpr (std::is_assignable_v<T &, std::initializer_list<std::int32_t>>) {
        parent->data[static_cast<qsizetype>(offset)] = list;
      } else if constexpr (std::is_constructible_v<T, std::initializer_list<std::int32_t>>) {
        parent->data[static_cast<qsizetype>(offset)] = T(list);
      } else {
        parent->data[static_cast<qsizetype>(offset)] = T{};
      }
      return parent->data[static_cast<qsizetype>(offset)];
    }
    Accessor &operator=(const Accessor &other) {
      parent->data[static_cast<qsizetype>(offset)] = (const T &)other;
      return *this;
    }
    Accessor &operator=(const ConstAccessor &other) {
      parent->data[static_cast<qsizetype>(offset)] = (const T &)other;
      return *this;
    }
    T *operator&() const { return &parent->data[static_cast<qsizetype>(offset)]; }
    T *operator->() const { return &parent->data[static_cast<qsizetype>(offset)]; }
    T &get() const { return parent->data[static_cast<qsizetype>(offset)]; }
    template <typename U> bool operator<(const U &other) const { return get() < static_cast<T>(other); }
    template <typename U> bool operator>(const U &other) const { return get() > static_cast<T>(other); }
    template <typename U> bool operator<=(const U &other) const { return get() <= static_cast<T>(other); }
    template <typename U> bool operator>=(const U &other) const { return get() >= static_cast<T>(other); }
    template <typename U> bool operator==(const U &other) const { return get() == static_cast<T>(other); }
    template <typename U> bool operator!=(const U &other) const { return get() != static_cast<T>(other); }
    bool operator<(const Accessor &other) const { return get() < other.get(); }
    bool operator>(const Accessor &other) const { return get() > other.get(); }
    bool operator<=(const Accessor &other) const { return get() <= other.get(); }
    bool operator>=(const Accessor &other) const { return get() >= other.get(); }
    bool operator==(const Accessor &other) const { return get() == other.get(); }
    bool operator!=(const Accessor &other) const { return get() != other.get(); }
    bool operator<(const ConstAccessor &other) const { return get() < other.get(); }
    bool operator>(const ConstAccessor &other) const { return get() > other.get(); }
    bool operator<=(const ConstAccessor &other) const { return get() <= other.get(); }
    bool operator>=(const ConstAccessor &other) const { return get() >= other.get(); }
    bool operator==(const ConstAccessor &other) const { return get() == other.get(); }
    bool operator!=(const ConstAccessor &other) const { return get() != other.get(); }
  };

  struct ConstAccessor {
    using value_type = T;
    using E2CPP_ARRAY_ACCESSOR_TAG = void;

    const EQtArray *parent;
    std::size_t offset;
    std::size_t dim_idx;

    ConstAccessor operator[](std::int32_t idx) const {
      if (parent->dims.isEmpty()) {
        throw std::out_of_range("数组尚未初始化(空维度)");
      }
      if (dim_idx >= static_cast<std::size_t>(parent->dims.size())) {
        throw std::out_of_range("维度超出范围");
      }
      if (idx < 1 || idx > parent->dims[static_cast<qsizetype>(dim_idx)]) {
        throw std::out_of_range("数组下标越界");
      }
      std::size_t stride = 1;
      for (std::size_t i = dim_idx + 1;
           i < static_cast<std::size_t>(parent->dims.size()); ++i) {
        stride *= static_cast<std::size_t>(parent->dims[static_cast<qsizetype>(i)]);
      }
      return ConstAccessor{
          parent, offset + static_cast<std::size_t>(idx - 1) * stride, dim_idx + 1};
    }

    operator const T &() const {
      return parent->data[static_cast<qsizetype>(offset)];
    }
    const T *operator->() const {
      return &parent->data[static_cast<qsizetype>(offset)];
    }
    const T &get() const {
      return parent->data[static_cast<qsizetype>(offset)];
    }
    template <typename U> bool operator<(const U &other) const { return get() < static_cast<T>(other); }
    template <typename U> bool operator>(const U &other) const { return get() > static_cast<T>(other); }
    template <typename U> bool operator<=(const U &other) const { return get() <= static_cast<T>(other); }
    template <typename U> bool operator>=(const U &other) const { return get() >= static_cast<T>(other); }
    template <typename U> bool operator==(const U &other) const { return get() == static_cast<T>(other); }
    template <typename U> bool operator!=(const U &other) const { return get() != static_cast<T>(other); }
    bool operator<(const Accessor &other) const { return get() < other.get(); }
    bool operator>(const Accessor &other) const { return get() > other.get(); }
    bool operator<=(const Accessor &other) const { return get() <= other.get(); }
    bool operator>=(const Accessor &other) const { return get() >= other.get(); }
    bool operator==(const Accessor &other) const { return get() == other.get(); }
    bool operator!=(const Accessor &other) const { return get() != other.get(); }
    bool operator<(const ConstAccessor &other) const { return get() < other.get(); }
    bool operator>(const ConstAccessor &other) const { return get() > other.get(); }
    bool operator<=(const ConstAccessor &other) const { return get() <= other.get(); }
    bool operator>=(const ConstAccessor &other) const { return get() >= other.get(); }
    bool operator==(const ConstAccessor &other) const { return get() == other.get(); }
    bool operator!=(const ConstAccessor &other) const { return get() != other.get(); }
  };

  Accessor operator[](std::int32_t idx) { return Accessor{this, 0, 0}[idx]; }
  ConstAccessor operator[](std::int32_t idx) const {
    return ConstAccessor{this, 0, 0}[idx];
  }
};
#endif

struct E2CppByteArray {
  std::vector<std::uint8_t> m_data;

  E2CppByteArray() = default;
  E2CppByteArray(std::initializer_list<std::uint8_t> list) : m_data(list) {}
  template <typename Iter>
  E2CppByteArray(Iter first, Iter last) : m_data(first, last) {}

  const std::uint8_t *data() const { return m_data.data(); }
  std::uint8_t *data() { return m_data.data(); }
  const std::uint8_t *data_ptr() const { return m_data.data(); }
  std::uint8_t *data_ptr() { return m_data.data(); }
  std::size_t size() const { return m_data.size(); }

  std::uint8_t &operator[](std::int32_t idx) {
    if (idx < 1 || static_cast<std::size_t>(idx) > m_data.size()) {
      throw std::out_of_range("字节集下标越界");
    }
    return m_data[static_cast<std::size_t>(idx - 1)];
  }

  const std::uint8_t &operator[](std::int32_t idx) const {
    if (idx < 1 || static_cast<std::size_t>(idx) > m_data.size()) {
      throw std::out_of_range("字节集下标越界");
    }
    return m_data[static_cast<std::size_t>(idx - 1)];
  }

  E2CppByteArray operator+(const E2CppByteArray &other) const {
    E2CppByteArray res = *this;
    res.m_data.insert(res.m_data.end(), other.m_data.begin(),
                      other.m_data.end());
    return res;
  }


  E2CppByteArray operator+(std::initializer_list<std::uint8_t> list) const {
    E2CppByteArray res = *this;
    res.m_data.insert(res.m_data.end(), list.begin(), list.end());
    return res;
  }

  bool operator==(const E2CppByteArray &other) const {
    return m_data == other.m_data;
  }
  bool operator!=(const E2CppByteArray &other) const {
    return !(*this == other);
  }
};

/**
 */
struct E2CppLiteralArray {
  std::vector<std::int32_t> m_vals;
  E2CppLiteralArray() = default;
  E2CppLiteralArray(std::initializer_list<std::int32_t> list) : m_vals(list) {}

  E2CppLiteralArray operator+(const E2CppLiteralArray &other) const {
    E2CppLiteralArray res = *this;
    res.m_vals.insert(res.m_vals.end(), other.m_vals.begin(),
                      other.m_vals.end());
    return res;
  }


  E2CppLiteralArray operator+(std::initializer_list<std::int32_t> list) const {
    E2CppLiteralArray res = *this;
    res.m_vals.insert(res.m_vals.end(), list.begin(), list.end());
    return res;
  }

  E2CppByteArray operator+(const E2CppByteArray &other) const {
    return static_cast<E2CppByteArray>(*this) + other;
  }

  friend E2CppByteArray operator+(const E2CppByteArray &left,
                                  const E2CppLiteralArray &right) {
    return left + static_cast<E2CppByteArray>(right);
  }

  bool operator==(const E2CppLiteralArray &other) const {
    return m_vals == other.m_vals;
  }
  bool operator!=(const E2CppLiteralArray &other) const {
    return !(*this == other);
  }

  bool operator==(const E2CppByteArray &other) const {
    return static_cast<E2CppByteArray>(*this) == other;
  }
  bool operator!=(const E2CppByteArray &other) const {
    return !(*this == other);
  }

  friend bool operator==(const E2CppByteArray &left,
                         const E2CppLiteralArray &right) {
    return left == static_cast<E2CppByteArray>(right);
  }
  friend bool operator!=(const E2CppByteArray &left,
                         const E2CppLiteralArray &right) {
    return !(left == right);
  }


  operator E2CppByteArray() const {
    E2CppByteArray res;
    res.m_data.reserve(m_vals.size());
    for (auto v : m_vals)
      res.m_data.push_back(static_cast<std::uint8_t>(v));
    return res;
  }

#ifdef QT_CORE_LIB
  operator QByteArray() const {
    QByteArray res;
    res.reserve(static_cast<qsizetype>(m_vals.size()));
    for (auto v : m_vals)
      res.push_back(static_cast<char>(static_cast<std::uint8_t>(v)));
    return res;
  }
#endif


  template <typename T> operator E2CppArray<T>() const {
    E2CppArray<T> res;
    res.data.reserve(m_vals.size());
    for (auto v : m_vals)
      res.data.push_back(static_cast<T>(v));
    res.dims = {static_cast<std::int32_t>(m_vals.size())};
    return res;
  }

#ifdef QT_CORE_LIB
  template <typename T> operator EQtArray<T>() const {
    EQtArray<T> res;
    res.data.reserve(static_cast<qsizetype>(m_vals.size()));
    for (auto v : m_vals)
      res.data.push_back(static_cast<T>(v));
    res.dims = {static_cast<std::int32_t>(m_vals.size())};
    return res;
  }
#endif
};

// POD reading helper
template <typename T>
T Internal_ReadPOD(const E2CppByteArray &bin, std::int32_t offset1) {
  if (offset1 < 1)
    return T{};
  std::size_t offset0 = static_cast<std::size_t>(offset1 - 1);
  if (offset0 + sizeof(T) > bin.m_data.size())
    return T{};

  T val;
  std::memcpy(&val, bin.m_data.data() + offset0, sizeof(T));
  return val;
}

struct E2CppDateTime {
  std::chrono::system_clock::time_point tp;
  bool is_empty;
  // 默认构造必须表示“空日期时间”，不能再把 1970-01-01 当成空值。
  E2CppDateTime()
      : tp(std::chrono::system_clock::from_time_t(0)), is_empty(true) {}
  explicit E2CppDateTime(std::chrono::system_clock::time_point t)
      : tp(t), is_empty(false) {}

  std::tm to_tm() const {
    // 空日期时间给出稳定的零点 tm，调用方应优先配合 是否为空() 判断。
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_struct;
#ifdef _WIN32
    localtime_s(&tm_struct, &t);
#else
    localtime_r(&t, &tm_struct);
#endif
    return tm_struct;
  }

  static E2CppDateTime fromEString(const std::string &s) {
    if (s.empty())
      return E2CppDateTime();
    std::tm tm = {};
    int y = 0, m = 0, d = 0, h = 0, min = 0, sec = 0;
    // 避免依赖中文格式串；源码一旦编码受损，sscanf 的中文模板就会直接失效。
    // 这里统一提取文本中的整数序列，兼容：
    //   2026年03月04日04时12分37秒
    //   2026-03-04 04:12:37
    //   2026/03/04 04:12:37
    std::vector<int> parts;
    int current = 0;
    bool inNumber = false;
    for (unsigned char ch : s) {
      if (ch >= '0' && ch <= '9') {
        current = current * 10 + static_cast<int>(ch - '0');
        inNumber = true;
      } else if (inNumber) {
        parts.push_back(current);
        current = 0;
        inNumber = false;
      }
    }
    if (inNumber) {
      parts.push_back(current);
    }
    if (parts.size() >= 3) {
      y = parts[0];
      m = parts[1];
      d = parts[2];
      if (parts.size() >= 4)
        h = parts[3];
      if (parts.size() >= 5)
        min = parts[4];
      if (parts.size() >= 6)
        sec = parts[5];
      tm.tm_year = y - 1900;
      tm.tm_mon = m - 1;
      tm.tm_mday = d;
      tm.tm_hour = h;
      tm.tm_min = min;
      tm.tm_sec = sec;
      tm.tm_isdst = -1;
      std::time_t tt = std::mktime(&tm);
      if (tt != -1)
        return E2CppDateTime(std::chrono::system_clock::from_time_t(tt));
    }
    return E2CppDateTime();
  }

  // 易语言日期时间型在 DLL/OLE 交互里按 DATE(double) 传递。
  // 这里统一提供和 OLE Automation DATE 之间的双向转换，避免把
  // E2CppDateTime 对象本身直接按 ABI 传给系统 DLL。
  double toOleDate() const {
    if (is_empty) {
      return 0.0;
    }
    std::tm oleEpoch = {};
    oleEpoch.tm_year = 1899 - 1900;
    oleEpoch.tm_mon = 11;
    oleEpoch.tm_mday = 30;
    oleEpoch.tm_hour = 0;
    oleEpoch.tm_min = 0;
    oleEpoch.tm_sec = 0;
    oleEpoch.tm_isdst = -1;
    const std::time_t oleEpochTime = std::mktime(&oleEpoch);
    const auto oleEpochTp = std::chrono::system_clock::from_time_t(oleEpochTime);
    const auto delta = tp - oleEpochTp;
    const auto millis =
        std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
    return static_cast<double>(millis) / 86400000.0;
  }

  static E2CppDateTime fromOleDate(double value) {
    std::tm oleEpoch = {};
    oleEpoch.tm_year = 1899 - 1900;
    oleEpoch.tm_mon = 11;
    oleEpoch.tm_mday = 30;
    oleEpoch.tm_hour = 0;
    oleEpoch.tm_min = 0;
    oleEpoch.tm_sec = 0;
    oleEpoch.tm_isdst = -1;
    const std::time_t oleEpochTime = std::mktime(&oleEpoch);
    const auto oleEpochTp = std::chrono::system_clock::from_time_t(oleEpochTime);
    const auto millis = static_cast<std::int64_t>(std::llround(value * 86400000.0));
    return E2CppDateTime(oleEpochTp + std::chrono::milliseconds(millis));
  }

#ifdef QT_CORE_LIB
  operator QDateTime() const {
    if (is_empty) {
      return QDateTime();
    }
    return QDateTime::fromMSecsSinceEpoch(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            tp.time_since_epoch())
            .count());
  }
#endif
};

inline bool operator==(const E2CppDateTime &lhs, const E2CppDateTime &rhs) {
  return lhs.is_empty == rhs.is_empty && lhs.tp == rhs.tp;
}
inline bool operator!=(const E2CppDateTime &lhs, const E2CppDateTime &rhs) {
  return !(lhs == rhs);
}
inline bool operator<(const E2CppDateTime &lhs, const E2CppDateTime &rhs) {
  if (lhs.is_empty || rhs.is_empty) {
    return !lhs.is_empty && rhs.is_empty ? false : (!rhs.is_empty && lhs.is_empty);
  }
  return lhs.tp < rhs.tp;
}
inline bool operator>(const E2CppDateTime &lhs, const E2CppDateTime &rhs) {
  return rhs < lhs;
}
inline bool operator<=(const E2CppDateTime &lhs, const E2CppDateTime &rhs) {
  return !(rhs < lhs);
}
inline bool operator>=(const E2CppDateTime &lhs, const E2CppDateTime &rhs) {
  return !(lhs < rhs);
}

struct EType_字体 {
  std::int32_t 角度{};
  bool 加粗{};
  bool 倾斜{};
  bool 删除线{};
  bool 下划线{};
  std::int32_t 字体大小{9};
  QString 字体名称{QStringLiteral("宋体")};
};

enum E2CppDataTypeCode : std::int32_t {
  DT_Byte = 1,
  DT_Short = 2,
  DT_Int = 3,
  DT_Int64 = 4,
  DT_Float = 5,
  DT_Double = 6,
  DT_Bool = 7,
  DT_DateTime = 8,
  DT_SubPtr = 9,
  DT_Text = 10
};

/**
 * @brief 易语言通用返回类型封装
 */
struct E2CppValue {
  using ValueType =
      std::variant<std::monostate, std::int32_t, std::int64_t, float, double,
                   QDateTime, bool, std::string, E2CppByteArray, void *>;

  ValueType value;

  E2CppValue() = default;
  template <typename T,
            std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T> &&
                                 !std::is_same_v<std::remove_cv_t<T>, bool> &&
                                 (sizeof(T) <= sizeof(std::int32_t)),
                             int> = 0>
  E2CppValue(T v) : value(static_cast<std::int32_t>(v)) {}

  template <typename T,
            std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T> &&
                                 !std::is_same_v<std::remove_cv_t<T>, bool> &&
                                 (sizeof(T) > sizeof(std::int32_t)),
                             int> = 0>
  E2CppValue(T v) : value(static_cast<std::int64_t>(v)) {}

  template <typename T,
            std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> &&
                                 !std::is_same_v<std::remove_cv_t<T>, bool> &&
                                 (sizeof(T) < sizeof(std::int64_t)),
                             int> = 0>
  E2CppValue(T v) : value(static_cast<std::int64_t>(v)) {}

  template <typename T,
            std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> &&
                                 !std::is_same_v<std::remove_cv_t<T>, bool> &&
                                 (sizeof(T) >= sizeof(std::int64_t)),
                             int> = 0>
  E2CppValue(T v) : value(static_cast<double>(v)) {}
  E2CppValue(float v) : value(v) {}
  E2CppValue(double v) : value(v) {}
  E2CppValue(bool v) : value(v) {}
  E2CppValue(const std::string &v) : value(v) {}
  E2CppValue(std::string &&v) : value(std::move(v)) {}
  E2CppValue(const E2CppByteArray &v) : value(v) {}
  E2CppValue(E2CppByteArray &&v) : value(std::move(v)) {}
  E2CppValue(const E2CppDateTime &v)
      : value(v.is_empty
                  ? QDateTime()
                  : QDateTime::fromMSecsSinceEpoch(
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            v.tp.time_since_epoch())
                            .count())) {}
#ifdef QT_CORE_LIB
  E2CppValue(const QString &v) : value(v.toStdString()) {}
  E2CppValue(const QByteArray &v)
      : value(E2CppByteArray(
            reinterpret_cast<const std::uint8_t *>(v.constData()),
            reinterpret_cast<const std::uint8_t *>(v.constData()) +
                static_cast<std::size_t>(v.size()))) {}
  E2CppValue(const QDateTime &v) : value(v) {}
#endif
  E2CppValue(void *v) : value(v) {}
  E2CppValue(const E2CppLiteralArray &v)
      : value(static_cast<E2CppByteArray>(v)) {}
  E2CppValue(std::initializer_list<int> bytes) {
    E2CppByteArray data;
    data.m_data.reserve(bytes.size());
    for (int b : bytes) {
      data.m_data.push_back(static_cast<std::uint8_t>(b));
    }
    value = std::move(data);
  }

  // 特殊处理 const char* 到 std::string
  E2CppValue(const char *s) : value(std::string(s ? s : "")) {}

  template <typename T, std::enable_if_t<
                            is_e2cpp_accessor<std::decay_t<T>>::value, int> = 0>
  E2CppValue(const T &v)
      : E2CppValue(static_cast<typename std::decay_t<T>::value_type>(v)) {}

  // 隐式转换操作符，方便作为函数参数使用
  operator std::int32_t() const {
    return std::visit(
        overloaded{
            [](std::monostate) { return 0; }, [](std::int32_t v) { return v; },
            [](std::int64_t v) { return static_cast<std::int32_t>(v); },
            [](float v) { return static_cast<std::int32_t>(v); },
            [](double v) { return static_cast<std::int32_t>(v); },
            [](bool v) { return v ? 1 : 0; }, [](const auto &) { return 0; }},
        value);
  }
  operator std::int16_t() const {
    return static_cast<std::int16_t>(operator std::int32_t());
  }
  operator std::int64_t() const {
    return std::visit(
        overloaded{[](std::monostate) { return std::int64_t(0); },
                   [](std::int32_t v) { return static_cast<std::int64_t>(v); },
                   [](std::int64_t v) { return v; },
                   [](float v) { return static_cast<std::int64_t>(v); },
                   [](double v) { return static_cast<std::int64_t>(v); },
                   [](bool v) { return v ? std::int64_t(1) : std::int64_t(0); },
                   [](const auto &) { return std::int64_t(0); }},
        value);
  }
  operator std::uint8_t() const {
    return static_cast<std::uint8_t>(operator std::int32_t());
  }
  operator double() const {
    return std::visit(
        overloaded{[](std::monostate) { return 0.0; },
                   [](std::int32_t v) { return static_cast<double>(v); },
                   [](std::int64_t v) { return static_cast<double>(v); },
                   [](float v) { return static_cast<double>(v); },
                   [](double v) { return v; },
                   [](bool v) { return v ? 1.0 : 0.0; },
                   [](const auto &) { return 0.0; }},
        value);
  }
  operator E2CppDateTime() const {
    return std::visit(overloaded{[](const QDateTime &v) {
                                   return v.isValid()
                                              ? E2CppDateTime(
                                                    std::chrono::system_clock::time_point(
                                                        std::chrono::milliseconds(
                                                            v.toMSecsSinceEpoch())))
                                              : E2CppDateTime();
                                 },
                                 [](const std::string &v) {
                                   return E2CppDateTime::fromEString(v);
                                 },
                                 [](const auto &) { return E2CppDateTime(); }},
                      value);
  }
#ifdef QT_CORE_LIB
  operator QDateTime() const {
    return std::visit(overloaded{[](const QDateTime &v) { return v; },
                                 [](const std::string &v) {
                                   const E2CppDateTime dt =
                                       E2CppDateTime::fromEString(v);
                                   return dt.is_empty
                                              ? QDateTime()
                                              : QDateTime::fromMSecsSinceEpoch(
                                                    std::chrono::duration_cast<
                                                        std::chrono::milliseconds>(
                                                        dt.tp.time_since_epoch())
                                                        .count());
                                 },
                                 [](const auto &) { return QDateTime(); }},
                      value);
  }

  operator QString() const {
    return std::visit(
        overloaded{
            [](std::monostate) { return QString(); },
            [](const std::string &v) { return QString::fromStdString(v); },
            [](std::int32_t v) { return QString::number(v); },
            [](std::int64_t v) { return QString::number(v); },
            [](float v) { return QString::number(static_cast<double>(v), 'g', 7); },
            [](double v) { return QString::number(v, 'g', 15); },
            [](bool v) { return v ? QStringLiteral("真") : QStringLiteral("假"); },
            [](const QDateTime &v) {
              return v.isValid()
                         ? v.toString(QStringLiteral("yyyy年MM月dd日HH时mm分ss秒"))
                         : QString();
            },
            [](const E2CppByteArray &v) {
              return QString::fromUtf8(
                  reinterpret_cast<const char *>(v.data()),
                  static_cast<qsizetype>(v.size()));
            },
            [](void *v) { return QStringLiteral("%1").arg(reinterpret_cast<quintptr>(v), 0, 16); }},
        value);
  }
#endif
  operator float() const { return static_cast<float>(operator double()); }
  operator void *() const {
    return std::visit(overloaded{[](void *v) { return v; },
                                 [](std::int32_t v) {
                                   return reinterpret_cast<void *>(static_cast<std::intptr_t>(v));
                                 },
                                 [](std::int64_t v) {
                                   return reinterpret_cast<void *>(static_cast<std::intptr_t>(v));
                                 },
                                 [](const auto &) { return static_cast<void *>(nullptr); }},
                      value);
  }
  operator bool() const {
    return std::visit(
        overloaded{[](std::monostate) { return false; },
                   [](std::int32_t v) { return v != 0; },
                   [](std::int64_t v) { return v != 0; },
                   [](float v) { return v != 0.0f; },
                   [](double v) { return v != 0.0; }, [](bool v) { return v; },
                   [](const std::string &v) { return !v.empty(); },
                   [](const auto &) { return false; }},
        value);
  }
  operator std::string() const {
    return std::visit(
        overloaded{
            [](std::monostate) { return std::string(); },
            [](const std::string &v) { return v; },
            [](std::int32_t v) { return std::to_string(v); },
            [](std::int64_t v) { return std::to_string(v); },
            [](float v) {
              char buf[64];
              std::snprintf(buf, sizeof(buf), "%g", static_cast<double>(v));
              return std::string(buf);
            },
            [](double v) {
              char buf[64];
              std::snprintf(buf, sizeof(buf), "%g", v);
              return std::string(buf);
            },
            [](bool v) { return v ? std::string("真") : std::string("假"); },
            [](const QDateTime &v) {
              return v.isValid()
                         ? v.toString(QStringLiteral("yyyy年MM月dd日HH时mm分ss秒"))
                               .toStdString()
                         : std::string();
            },
            [](const E2CppByteArray &v) {
              return std::string(reinterpret_cast<const char *>(v.data()),
                                 v.size());
            },
            [](void *v) {
              char buf[32];
              std::snprintf(buf, sizeof(buf), "%p", v);
              return std::string(buf);
            }},
        value);
  }

  bool isFloatingNumeric() const {
    return std::holds_alternative<float>(value) ||
           std::holds_alternative<double>(value);
  }

  std::int64_t toInt64Lossy() const { return operator std::int64_t(); }
  double toDoubleLossy() const { return operator double(); }

  friend double operator+(const E2CppValue &lhs, const E2CppValue &rhs) {
    if (lhs.isFloatingNumeric() || rhs.isFloatingNumeric()) {
      return lhs.toDoubleLossy() + rhs.toDoubleLossy();
    }
    return static_cast<double>(lhs.toInt64Lossy() + rhs.toInt64Lossy());
  }

  friend double operator-(const E2CppValue &lhs, const E2CppValue &rhs) {
    if (lhs.isFloatingNumeric() || rhs.isFloatingNumeric()) {
      return lhs.toDoubleLossy() - rhs.toDoubleLossy();
    }
    return static_cast<double>(lhs.toInt64Lossy() - rhs.toInt64Lossy());
  }

  friend double operator*(const E2CppValue &lhs, const E2CppValue &rhs) {
    if (lhs.isFloatingNumeric() || rhs.isFloatingNumeric()) {
      return lhs.toDoubleLossy() * rhs.toDoubleLossy();
    }
    return static_cast<double>(lhs.toInt64Lossy() * rhs.toInt64Lossy());
  }

  friend double operator/(const E2CppValue &lhs, const E2CppValue &rhs) {
    return lhs.toDoubleLossy() / rhs.toDoubleLossy();
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend std::conditional_t<std::is_floating_point_v<T>, double, std::int64_t>
  operator*(const E2CppValue &lhs, T rhs) {
    if constexpr (std::is_floating_point_v<T>) {
      return lhs.toDoubleLossy() * static_cast<double>(rhs);
    } else {
      return lhs.isFloatingNumeric()
                 ? static_cast<std::int64_t>(lhs.toDoubleLossy() *
                                             static_cast<double>(rhs))
                 : lhs.toInt64Lossy() * static_cast<std::int64_t>(rhs);
    }
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend std::conditional_t<std::is_floating_point_v<T>, double, std::int64_t>
  operator*(T lhs, const E2CppValue &rhs) {
    if constexpr (std::is_floating_point_v<T>) {
      return static_cast<double>(lhs) * rhs.toDoubleLossy();
    } else {
      return rhs.isFloatingNumeric()
                 ? static_cast<std::int64_t>(static_cast<double>(lhs) *
                                             rhs.toDoubleLossy())
                 : static_cast<std::int64_t>(lhs) * rhs.toInt64Lossy();
    }
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend double operator/(const E2CppValue &lhs, T rhs) {
    return lhs.toDoubleLossy() / static_cast<double>(rhs);
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend double operator/(T lhs, const E2CppValue &rhs) {
    return static_cast<double>(lhs) / rhs.toDoubleLossy();
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend std::conditional_t<std::is_floating_point_v<T>, double, std::int64_t>
  operator+(const E2CppValue &lhs, T rhs) {
    if constexpr (std::is_floating_point_v<T>) {
      return lhs.toDoubleLossy() + static_cast<double>(rhs);
    } else {
      return lhs.isFloatingNumeric()
                 ? static_cast<std::int64_t>(lhs.toDoubleLossy() +
                                             static_cast<double>(rhs))
                 : lhs.toInt64Lossy() + static_cast<std::int64_t>(rhs);
    }
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend std::conditional_t<std::is_floating_point_v<T>, double, std::int64_t>
  operator+(T lhs, const E2CppValue &rhs) {
    if constexpr (std::is_floating_point_v<T>) {
      return static_cast<double>(lhs) + rhs.toDoubleLossy();
    } else {
      return rhs.isFloatingNumeric()
                 ? static_cast<std::int64_t>(static_cast<double>(lhs) +
                                             rhs.toDoubleLossy())
                 : static_cast<std::int64_t>(lhs) + rhs.toInt64Lossy();
    }
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend std::conditional_t<std::is_floating_point_v<T>, double, std::int64_t>
  operator-(const E2CppValue &lhs, T rhs) {
    if constexpr (std::is_floating_point_v<T>) {
      return lhs.toDoubleLossy() - static_cast<double>(rhs);
    } else {
      return lhs.isFloatingNumeric()
                 ? static_cast<std::int64_t>(lhs.toDoubleLossy() -
                                             static_cast<double>(rhs))
                 : lhs.toInt64Lossy() - static_cast<std::int64_t>(rhs);
    }
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend std::conditional_t<std::is_floating_point_v<T>, double, std::int64_t>
  operator-(T lhs, const E2CppValue &rhs) {
    if constexpr (std::is_floating_point_v<T>) {
      return static_cast<double>(lhs) - rhs.toDoubleLossy();
    } else {
      return rhs.isFloatingNumeric()
                 ? static_cast<std::int64_t>(static_cast<double>(lhs) -
                                             rhs.toDoubleLossy())
                 : static_cast<std::int64_t>(lhs) - rhs.toInt64Lossy();
    }
  }

  friend bool operator==(const E2CppValue &lhs, const E2CppValue &rhs) {
    if (lhs.isFloatingNumeric() || rhs.isFloatingNumeric()) {
      return lhs.toDoubleLossy() == rhs.toDoubleLossy();
    }
    return lhs.toInt64Lossy() == rhs.toInt64Lossy();
  }

  friend bool operator!=(const E2CppValue &lhs, const E2CppValue &rhs) {
    return !(lhs == rhs);
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend bool operator==(const E2CppValue &lhs, T rhs) {
    if (lhs.isFloatingNumeric() || std::is_floating_point_v<T>) {
      return lhs.toDoubleLossy() == static_cast<double>(rhs);
    }
    return lhs.toInt64Lossy() == static_cast<std::int64_t>(rhs);
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend bool operator==(T lhs, const E2CppValue &rhs) {
    return rhs == lhs;
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend bool operator!=(const E2CppValue &lhs, T rhs) {
    return !(lhs == rhs);
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend bool operator!=(T lhs, const E2CppValue &rhs) {
    return !(rhs == lhs);
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend bool operator<(const E2CppValue &lhs, T rhs) {
    if (lhs.isFloatingNumeric() || std::is_floating_point_v<T>) {
      return lhs.toDoubleLossy() < static_cast<double>(rhs);
    }
    return lhs.toInt64Lossy() < static_cast<std::int64_t>(rhs);
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend bool operator<(T lhs, const E2CppValue &rhs) {
    if (rhs.isFloatingNumeric() || std::is_floating_point_v<T>) {
      return static_cast<double>(lhs) < rhs.toDoubleLossy();
    }
    return static_cast<std::int64_t>(lhs) < rhs.toInt64Lossy();
  }

  friend bool operator<(const E2CppValue &lhs, const E2CppValue &rhs) {
    if (lhs.isFloatingNumeric() || rhs.isFloatingNumeric()) {
      return lhs.toDoubleLossy() < rhs.toDoubleLossy();
    }
    return lhs.toInt64Lossy() < rhs.toInt64Lossy();
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend bool operator>(const E2CppValue &lhs, T rhs) {
    return rhs < lhs;
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend bool operator>(T lhs, const E2CppValue &rhs) {
    return rhs < lhs;
  }

  friend bool operator>(const E2CppValue &lhs, const E2CppValue &rhs) {
    return rhs < lhs;
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend bool operator<=(const E2CppValue &lhs, T rhs) {
    return !(lhs > rhs);
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend bool operator<=(T lhs, const E2CppValue &rhs) {
    return !(lhs > rhs);
  }

  friend bool operator<=(const E2CppValue &lhs, const E2CppValue &rhs) {
    return !(lhs > rhs);
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend bool operator>=(const E2CppValue &lhs, T rhs) {
    return !(lhs < rhs);
  }

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  friend bool operator>=(T lhs, const E2CppValue &rhs) {
    return !(lhs < rhs);
  }

  friend bool operator>=(const E2CppValue &lhs, const E2CppValue &rhs) {
    return !(lhs < rhs);
  }
};

// Aliases for compatibility with old translator code
template <typename T> using E2CppArray1D = E2CppArray<T>;
template <typename T> using E2CppArray2D = E2CppArray<T>;

inline bool E2CppApproxEqual(const std::string &a, const std::string &b) {
  return a.compare(0, b.length(), b) == 0;
}

/// 获取一个用于填充“可空”引用参数的空数组指针。
/// 请注意：如果函数内部修改了此数组，由于它是静态的，可能会影响后续调用。
/// 但在易语言语义中，忽略的参数通常不应被期望持有状态。
template <typename T> inline E2CppArray<T> &Internal_GetEmptyArray() {
  struct Pool {
    std::array<E2CppArray<T>, 32> storage;
    std::size_t next = 0;
  };
  static thread_local Pool pool;
  const std::size_t idx = pool.next % pool.storage.size();
  ++pool.next;
  E2CppArray<T> &a = pool.storage[idx];
  a.data.clear();
  a.dims.clear();
  return a;
}

#ifdef QT_CORE_LIB
template <typename T> inline EQtArray<T> &Internal_GetEmptyQtArray() {
  struct Pool {
    std::array<EQtArray<T>, 32> storage;
    std::size_t next = 0;
  };
  static thread_local Pool pool;
  const std::size_t idx = pool.next % pool.storage.size();
  ++pool.next;
  EQtArray<T> &a = pool.storage[idx];
  a.data.clear();
  a.dims.clear();
  return a;
}
#endif

/// 获取一个用于填充“可空”引用参数的空变量指针。
template <typename T> inline T &Internal_GetEmptyScalar() {
  struct Pool {
    std::array<T, 32> storage;
    std::size_t next = 0;
  };
  static thread_local Pool pool;
  const std::size_t idx = pool.next % pool.storage.size();
  ++pool.next;
  T &v = pool.storage[idx];
  v = {};
  return v;
}

template <typename T> inline T &Internal_GetTempScalarCopy(const T &value) {
  struct Pool {
    std::array<T, 32> storage;
    std::size_t next = 0;
  };
  static thread_local Pool pool;
  const std::size_t idx = pool.next % pool.storage.size();
  ++pool.next;
  T &v = pool.storage[idx];
  v = value;
  return v;
}

template <typename T> struct E2EmptyHelper {
  static T &get() { return Internal_GetEmptyScalar<T>(); }
};
template <typename T> struct E2EmptyHelper<E2CppArray<T>> {
  static E2CppArray<T> &get() { return Internal_GetEmptyArray<T>(); }
};
#ifdef QT_CORE_LIB
template <typename T> struct E2EmptyHelper<EQtArray<T>> {
  static EQtArray<T> &get() { return Internal_GetEmptyQtArray<T>(); }
};
#endif

/**
* @brief 易语言空参数占位符类
* 利用模板转换操作符，自动根据目标参数类型返回对应的静态空对象左值引用。
 */
struct E2EmptyParam {
  template <typename T> operator T &() const { return E2EmptyHelper<T>::get(); }
  template <typename T> operator const T &() const {
    return E2EmptyHelper<T>::get();
  }
};
inline const E2EmptyParam _e2_empty{};

template <typename T>
inline std::int32_t 取数据_通用型_数组(E2CppArray<T> &数据) {
  return static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(
      数据.data.empty() ? nullptr : 数据.data.data()));
}

template <typename T>
inline std::int32_t 取数据_通用型_数组(const E2CppArray<T> &数据) {
  return static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(
      数据.data.empty() ? nullptr : 数据.data.data()));
}

template <typename T>
inline std::int32_t 取指针_通用型_数组(E2CppArray<T> &变量) {
  return static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(&变量));
}

template <typename T>
inline std::int32_t 取指针_通用型_数组(const E2CppArray<T> &变量) {
  return static_cast<std::int32_t>(
      reinterpret_cast<std::intptr_t>(&Internal_GetTempScalarCopy(变量)));
}

#ifdef QT_CORE_LIB
template <typename T>
inline std::int32_t 取数据_通用型_数组(EQtArray<T> &数据) {
  return static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(
      数据.data.isEmpty() ? nullptr : 数据.data.data()));
}

template <typename T>
inline std::int32_t 取数据_通用型_数组(const EQtArray<T> &数据) {
  return static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(
      数据.data.isEmpty() ? nullptr : 数据.data.constData()));
}

template <typename T>
inline std::int32_t 取指针_通用型_数组(EQtArray<T> &变量) {
  return static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(&变量));
}

template <typename T>
inline std::int32_t 取指针_通用型_数组(const EQtArray<T> &变量) {
  return static_cast<std::int32_t>(
      reinterpret_cast<std::intptr_t>(&Internal_GetTempScalarCopy(变量)));
}
#endif

template <typename T,
          std::enable_if_t<!std::is_same_v<std::decay_t<T>, E2CppValue> &&
                               !is_e2cpp_array<std::decay_t<T>>::value,
                           int> = 0>
inline std::intptr_t 取数据_通用型(T &数据) {
  return reinterpret_cast<std::intptr_t>(&数据);
}

template <typename T,
          std::enable_if_t<!std::is_same_v<std::decay_t<T>, E2CppValue> &&
                               !is_e2cpp_array<std::decay_t<T>>::value,
                           int> = 0>
inline std::intptr_t 取数据_通用型(const T &数据) {
  return reinterpret_cast<std::intptr_t>(&Internal_GetTempScalarCopy(数据));
}

template <typename T,
          std::enable_if_t<!std::is_same_v<std::decay_t<T>, E2CppValue> &&
                               !is_e2cpp_array<std::decay_t<T>>::value,
                           int> = 0>
inline std::intptr_t 取指针_通用型(T &变量) {
  return reinterpret_cast<std::intptr_t>(&变量);
}

template <typename T,
          std::enable_if_t<!std::is_same_v<std::decay_t<T>, E2CppValue> &&
                               !is_e2cpp_array<std::decay_t<T>>::value,
                           int> = 0>
inline std::intptr_t 取指针_通用型(const T &变量) {
  return reinterpret_cast<std::intptr_t>(&Internal_GetTempScalarCopy(变量));
}

template <
    typename T,
    std::enable_if_t<!std::is_same_v<std::decay_t<T>, E2CppValue>, int> = 0>
inline std::intptr_t 取变量地址_整数(T &var) {
  return reinterpret_cast<std::intptr_t>(&var);
}

template <
    typename T,
    std::enable_if_t<!std::is_same_v<std::decay_t<T>, E2CppValue>, int> = 0>
inline std::intptr_t 取变量地址_整数(const T &var) {
  return reinterpret_cast<std::intptr_t>(&Internal_GetTempScalarCopy(var));
}

template <
    typename T,
    std::enable_if_t<!std::is_same_v<std::decay_t<T>, E2CppValue>, int> = 0>
inline std::intptr_t 取变量地址_字节(T &var) {
  return reinterpret_cast<std::intptr_t>(&var);
}

template <
    typename T,
    std::enable_if_t<!std::is_same_v<std::decay_t<T>, E2CppValue>, int> = 0>
inline std::intptr_t 取变量地址_字节(const T &var) {
  return reinterpret_cast<std::intptr_t>(&Internal_GetTempScalarCopy(var));
}

template <
    typename T,
    std::enable_if_t<!std::is_same_v<std::decay_t<T>, E2CppValue>, int> = 0>
inline std::intptr_t 指针到变量(std::intptr_t 指针, T &变量) {
  if (指针 == 0) {
    变量 = T{};
    return 0;
  }
  变量 = *reinterpret_cast<T *>(指针);
  return 1;
}

inline std::intptr_t 取指针_字节集型(E2CppByteArray &字节集) {
  return reinterpret_cast<std::intptr_t>(字节集.data());
}

inline std::intptr_t 取指针_字节集型(const E2CppByteArray &字节集) {
  return reinterpret_cast<std::intptr_t>(字节集.data());
}

inline std::int32_t assignZero(E2CppByteArray &arg1, std::int32_t arg2,
                               bool arg3 = false) {
  if (arg3) {
    if (arg2 < 0) {
      arg2 = 0;
    }
    arg1.m_data.resize(static_cast<std::size_t>(arg2));
    return arg2;
  }
  if (arg2 >= 0 && static_cast<std::size_t>(arg2) < arg1.m_data.size()) {
    arg1.m_data[static_cast<std::size_t>(arg2)] = 0;
  }
  return arg2;
}

template <typename T>
inline std::int32_t assignZero(E2CppArray<T> &arg1, std::int32_t arg2,
                               bool arg3 = false) {
  if (arg3) {
    if (arg2 < 0) {
      arg2 = 0;
    }
    arg1.data.resize(static_cast<std::size_t>(arg2));
    if (!arg1.dims.empty()) {
      arg1.dims[0] = arg2;
    } else {
      arg1.dims.push_back(arg2);
    }
    return arg2;
  }
  if (arg2 >= 0 && static_cast<std::size_t>(arg2) < arg1.data.size()) {
    arg1.data[static_cast<std::size_t>(arg2)] = T{};
  }
  return arg2;
}

#ifdef QT_CORE_LIB
template <typename T>
inline std::int32_t assignZero(EQtArray<T> &arg1, std::int32_t arg2,
                               bool arg3 = false) {
  if (arg3) {
    if (arg2 < 0) {
      arg2 = 0;
    }
    arg1.data.resize(static_cast<qsizetype>(arg2));
    if (!arg1.dims.isEmpty()) {
      arg1.dims[0] = arg2;
    } else {
      arg1.dims.push_back(arg2);
    }
    return arg2;
  }
  if (arg2 >= 0 && arg2 < arg1.data.size()) {
    arg1.data[arg2] = T{};
  }
  return arg2;
}
#endif

inline std::int32_t assignZero(std::string &arg1, std::int32_t arg2,
                               bool arg3 = false) {
  if (arg3) {
    if (arg2 < 0) {
      arg2 = 0;
    }
    arg1.resize(static_cast<std::size_t>(arg2));
    return arg2;
  }
  if (arg2 >= 0 && static_cast<std::size_t>(arg2) < arg1.size()) {
    arg1[static_cast<std::size_t>(arg2)] = '\0';
    arg1.resize(std::char_traits<char>::length(arg1.c_str()));
  }
  return arg2;
}

#ifdef QT_CORE_LIB
inline std::int32_t assignZero(QString &arg1, std::int32_t arg2,
                               bool arg3 = false) {
  if (arg3) {
    if (arg2 < 0) {
      arg2 = 0;
    }
    arg1.resize(arg2);
    return arg2;
  }
  if (arg2 >= 0 && arg2 < arg1.size()) {
    arg1[arg2] = QChar(u'\0');
    const int nulPos = arg1.indexOf(QChar(u'\0'));
    if (nulPos >= 0) {
      arg1.truncate(nulPos);
    }
  }
  return arg2;
}
#endif

// Easy Language Type Aliases for Inline CPP

#ifdef QT_CORE_LIB
struct EQtByteArray : public QByteArray {
  using QByteArray::QByteArray;
  using QByteArray::operator=;

  EQtByteArray() = default;
  EQtByteArray(const QByteArray &other) : QByteArray(other) {}
  EQtByteArray(const E2CppByteArray &other)
      : QByteArray(reinterpret_cast<const char *>(other.data()),
                   static_cast<qsizetype>(other.size())) {}
  EQtByteArray(const E2CppLiteralArray &other) { *this = other; }

  EQtByteArray &operator=(const E2CppByteArray &other) {
    QByteArray::operator=(QByteArray(reinterpret_cast<const char *>(other.data()),
                                     static_cast<qsizetype>(other.size())));
    return *this;
  }

  EQtByteArray &operator=(const E2CppLiteralArray &other) {
    clear();
    reserve(static_cast<qsizetype>(other.m_vals.size()));
    for (std::int32_t v : other.m_vals) {
      append(static_cast<char>(static_cast<unsigned char>(v)));
    }
    return *this;
  }

  EQtByteArray &operator=(std::initializer_list<int> init) {
    clear();
    reserve(static_cast<qsizetype>(init.size()));
    for (int v : init) {
      append(static_cast<char>(static_cast<unsigned char>(v)));
    }
    return *this;
  }

  operator E2CppByteArray() const {
    E2CppByteArray out;
    out.m_data.reserve(static_cast<std::size_t>(size()));
    for (char ch : *this) {
      out.m_data.push_back(static_cast<std::uint8_t>(
          static_cast<unsigned char>(ch)));
    }
    return out;
  }

  EQtByteArray &operator+=(const EQtByteArray &other) {
    append(other);
    return *this;
  }

  EQtByteArray &operator+=(const E2CppLiteralArray &other) {
    append(EQtByteArray(other));
    return *this;
  }

  EQtByteArray operator+(const EQtByteArray &other) const {
    EQtByteArray result(*this);
    result += other;
    return result;
  }

  EQtByteArray operator+(const E2CppLiteralArray &other) const {
    EQtByteArray result(*this);
    result += other;
    return result;
  }

  bool operator==(const E2CppLiteralArray &other) const {
    return QByteArray(*this) == QByteArray(other);
  }

  bool operator!=(const E2CppLiteralArray &other) const {
    return !(*this == other);
  }
};

inline EQtByteArray operator+(const E2CppLiteralArray &left, const EQtByteArray &right) {
  EQtByteArray result(left);
  result += right;
  return result;
}

inline bool operator==(const E2CppLiteralArray &left, const EQtByteArray &right) {
  return QByteArray(left) == QByteArray(right);
}

inline bool operator!=(const E2CppLiteralArray &left, const EQtByteArray &right) {
  return !(left == right);
}

inline EQtByteArray operator+(const QByteArray &left, const E2CppLiteralArray &right) {
  EQtByteArray result(left);
  result += EQtByteArray(right);
  return result;
}

inline EQtByteArray operator+(const E2CppLiteralArray &left, const QByteArray &right) {
  EQtByteArray result(left);
  result += right;
  return result;
}

#include <QtCore/QDebug>
inline QDebug operator<<(QDebug dbg, const EQtByteArray &byteArray) {
  dbg.nospace() << static_cast<const QByteArray &>(byteArray);
  return dbg.space();
}
#endif

using 字节集 = EQtByteArray;
using 文本型 = QString;
using 整数型 = qint32;
using 逻辑型 = bool;
using 小数型 = float;
using 双精度小数型 = qreal;
using 字节型 = quint8;
using 短整数型 = qint16;
using 长整数型 = qint64;
using 日期时间型 = QDateTime;
using 子程序指针型 = void *;
using 子程序指针 = void *;
class EType_变体型;
using 变体型 = EType_变体型;
class EType_对象;
using 对象 = EType_对象;
using 字体 = EType_字体;
using 无符号字节型 = quint8;
using 无符号短整型 = quint16;
using 无符号整数型 = quint32;
using 无符号长整型 = quint64;

#ifdef QT_CORE_LIB
inline QString E2ToQString(const QString &s) { return s; }
inline QString E2ToQString(const std::string &s) {
  return QString::fromStdString(s);
}
inline QString E2ToQString(const char *s) {
  return QString::fromUtf8(s ? s : "");
}

inline QByteArray E2ToQByteArray(const QByteArray &v) { return v; }
inline QByteArray E2ToQByteArray(const E2CppByteArray &v) {
  return QByteArray(reinterpret_cast<const char *>(v.data()),
                    static_cast<qsizetype>(v.size()));
}
inline QByteArray E2ToQByteArray(const std::string &v) {
  return QByteArray(v.data(), static_cast<qsizetype>(v.size()));
}
inline QByteArray E2ToQByteArray(const char *v) {
  return QByteArray(v ? v : "");
}

inline QDateTime E2ToQDateTime(const QDateTime &v) { return v; }
inline QDateTime E2ToQDateTime(const E2CppDateTime &v) {
  if (v.is_empty) {
    return QDateTime();
  }
  const auto tp = v.tp;
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      tp.time_since_epoch())
                      .count();
  return QDateTime::fromMSecsSinceEpoch(ms);
}

inline QVariant E2ToQVariant(const QVariant &v) { return v; }
inline QVariant E2ToQVariant(const QString &v) { return v; }
inline QVariant E2ToQVariant(const QByteArray &v) { return v; }
inline QVariant E2ToQVariant(const QDateTime &v) { return v; }
inline QVariant E2ToQVariant(const std::string &v) {
  return QVariant(QString::fromStdString(v));
}
inline QVariant E2ToQVariant(const char *v) {
  return QVariant(QString::fromUtf8(v ? v : ""));
}

inline std::string E2ToStdString(const QString &s) { return s.toStdString(); }
inline const std::string &E2ToStdString(const std::string &s) { return s; }
inline QString E2TextLiteral(const char *s) {
  return QString::fromUtf8(s ? s : "");
}

// Keep a Qt-friendly overload available when generated code still emits
// CharUpperA/CharLowerA-style calls under 文本型=QString.
inline QString CharUpperA(const QString &s) { return s.toUpper(); }
inline QString CharLowerA(const QString &s) { return s.toLower(); }
#else
inline const std::string &E2ToStdString(const std::string &s) { return s; }
inline std::string E2TextLiteral(const char *s) {
  return std::string(s ? s : "");
}
#endif
