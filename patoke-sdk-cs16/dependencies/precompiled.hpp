#pragma once

// windows headers
#include <Windows.h>
#include <Psapi.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <intrin.h>

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

// defines
#define strong_inline __pragma(warning(suppress : 4714)) inline __forceinline
#define mem_likely(x) static_cast<bool>(x)
#define mem_unlikely(x) static_cast<bool>(x)