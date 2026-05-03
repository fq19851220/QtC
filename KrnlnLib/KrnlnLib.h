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
#include "../3rd/monocypher/monocypher.h"
/**
 * Easy Language Core Library (krnln) C++ Implementation
 * Organized by cmdCategory
 */

// Foundations
#include "E2CppBase.h"
#include "UiTools.h"

// Internal helpers (must come before category files that use
// E2CppDebugToString)
#define E2CPP_KRNLN_HEADER_IMPL 1
#include "Internal_Debug.cpp"
#include "KrnlnConstants.h"

// Category implementations
#include "Category02_Arithmetic.cpp"
#include "Category04_Bitwise.cpp"
#include "Category05_Variable.cpp"
#include "Category06_Array.cpp"
#include "Category07_Environment.cpp"
#include "Category08_Pinyin.cpp"
#include "Category09_Text.cpp"
#include "Category10_ByteArray.cpp"
#include "Category11_Conversion.cpp"
#include "Category12_Time.cpp"
#include "Category13_Disk.cpp"
#include "Category14_FileIO.cpp"
#include "Category15_System.cpp"
#include "Category17_Debug.cpp"
#include "Category18_Other.cpp"
#include "Category19_Media.cpp"
#include "Category20_Network.cpp"
#include "Category22_Console.cpp"
#ifdef _WIN32
#include "E2CppComObject.cpp"
#include "E2CppVariant.cpp"
#endif
#include "Category21_Web.cpp"
#undef E2CPP_KRNLN_HEADER_IMPL
