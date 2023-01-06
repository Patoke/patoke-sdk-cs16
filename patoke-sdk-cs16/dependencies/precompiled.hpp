#pragma once

// windows headers
#include <Windows.h>
#include <Psapi.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <intrin.h>
#include <libraries/wrappers/ntdll.hpp> // needed for undocumented functions

// std headers
#include <array>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <format>
#include <numbers>

// sdk includes
#include <math/math.hpp>

// defines
#define strong_inline __pragma(warning(suppress : 4714)) inline __forceinline
#define mem_likely(x) static_cast<bool>(x)
#define mem_unlikely(x) static_cast<bool>(x)

#define IS_CS16 1 // ofc it's gonna be cs 1.6 in this project, it's just for compatibility :P