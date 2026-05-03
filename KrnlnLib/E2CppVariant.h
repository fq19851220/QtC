#pragma once

#ifndef _WIN32
#error "变体型 (Variant) 仅支持 Windows 操作系统。"
#endif

#include "E2CppBase.h"
#include <ole2.h>
#include <windows.h>

/**
 * @brief 变体类型 (VariantType) 枚举常量集合
 */
struct E2Const_变体类型_S {
  const int32_t 未知 = -1;
  const int32_t 空 = 0;
  const int32_t 数值型 = 1;
  const int32_t 文本型 = 2;
  const int32_t 逻辑型 = 3;
  const int32_t 日期型 = 4;
  const int32_t 对象型 = 5;
  const int32_t 错误值型 = 6;
  const int32_t 数值型数组 = 7;
  const int32_t 文本型数组 = 8;
  const int32_t 逻辑型数组 = 9;
  const int32_t 日期型数组 = 10;
  const int32_t 对象型数组 = 11;
  const int32_t 错误值型数组 = 12;
  const int32_t 变体型数组 = 13;
};
inline const E2Const_变体类型_S 变体类型;
inline const E2Const_变体类型_S
    E2Const_变体类型; // 针对转译器生成的 E2Const_前缀

// 声明 EType_对象 以便互相引用
class EType_对象;

/**
 * @brief 易语言核心支持库：变体型 (Variant) 类型封装
 */
class EType_变体型 {
public:
  EType_变体型();
  ~EType_变体型();

  EType_变体型(const EType_变体型 &other);
  EType_变体型 &operator=(const EType_变体型 &other);

  // --- 成员命令 ---
  void 清除();
  int32_t 取类型() const;
  int32_t 取数组成员数() const;

  文本型 取文本(int32_t 成员索引 = 0) const;
  double 取数值(int32_t 成员索引 = 0) const;
  bool 取逻辑值(int32_t 成员索引 = 0) const;
  E2CppDateTime 取日期(int32_t 成员索引 = 0) const;
  EType_对象 取对象(int32_t 成员索引 = 0) const;
  EType_变体型 取变体型(int32_t 成员索引 = 0) const;
  E2CppByteArray 取字节集(int32_t 成员索引 = 0) const;

  bool 赋值(const E2CppValue &欲写入数据 = E2CppValue(), int32_t 成员索引 = 0);
  bool 赋值(const EType_对象 &欲写入对象, int32_t 成员索引 = 0);
  template <typename T>
  bool 赋值(const EQtArray<T> &欲写入数组, int32_t 成员索引 = 0) {
    if (成员索引 != 0) {
      return false;
    }
    if (!创建数组(变体类型.数值型, static_cast<int32_t>(欲写入数组.data.size()))) {
      return false;
    }
    for (std::size_t i = 0; i < 欲写入数组.data.size(); ++i) {
      if (!赋值(E2CppValue(欲写入数组.data[i]), static_cast<int32_t>(i + 1))) {
        return false;
      }
    }
    return true;
  }
  bool 创建数组(int32_t 成员类型, int32_t 成员数目);
  bool 置类型(int32_t 欲设置的类型);

  // 内部接口，供对象类内部调用
  VARIANT *GetInternalVariant() { return &m_var; }
  const VARIANT *GetInternalVariant() const { return &m_var; }

private:
  VARIANT m_var;
};
