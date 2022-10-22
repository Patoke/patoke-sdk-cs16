#pragma once
#include <position/g_pos.hpp>
#include "pattern/pattern.hpp"

// helper for making signature scanning more convenient
struct s_sighelper {
	int offset{};
	const char* sig{};

	constexpr s_sighelper() : offset(0x0), sig("") { }
	constexpr s_sighelper(const char* sig) : offset(0x0), sig(sig) { }
	constexpr s_sighelper(int offset, const char* sig) : offset(offset), sig(sig) { }
};

// helper for signature scanning results
struct s_pattern {
	uintptr_t address;

	constexpr s_pattern() : address(0) { }

	constexpr s_pattern(uintptr_t address) : address(address) { }

	template<typename t>
	constexpr t cast() { return reinterpret_cast<t>(address); }

	template<typename t> 
	constexpr t scast() { return static_cast<t>(address); }

	template<typename t> 
	constexpr t ccast() { return const_cast<t>(address); }

	template< typename t >
	t relative(size_t offset = 0x1) {
		if (!address)
			return t();

		size_t new_address = address + offset;
		int32_t relative_offset = *reinterpret_cast<int32_t*>(new_address);

		if (!relative_offset)
			return t();

		return reinterpret_cast<t>(new_address + sizeof(size_t) + relative_offset);
	}
};

// helper for memory region data
struct s_memoryregion {
	uintptr_t base;
	size_t region_size;
	uintptr_t state;
	uintptr_t protect;
	uintptr_t type;

	s_memoryregion(MEMORY_BASIC_INFORMATION32 mbi) {
		base = mbi.BaseAddress;
		region_size = mbi.RegionSize;
		state = mbi.State;
		protect = mbi.Protect;
		type = mbi.Type;
	}
};

namespace n_utilities {
	static s_pattern find_signature(uintptr_t start, uintptr_t length, s_sighelper& signature) {
		mem::region range;
		range.start = start;
		range.size = length;

		mem::pattern pattern_to_search(signature.sig);
		mem::default_scanner scanner(pattern_to_search);
		uintptr_t return_value{};

		scanner(range, [&](mem::pointer address) {
			return_value = address.add(signature.offset).as<uintptr_t>();

			return false;
		});

		return return_value;
	}

	static s_pattern find_signature(HMODULE hmodule, s_sighelper& signature) {
		MODULEINFO mod_info{};
		GetModuleInformation(GetCurrentProcess(), hmodule, &mod_info, sizeof(MODULEINFO));

		return find_signature(reinterpret_cast<uintptr_t>(hmodule), mod_info.SizeOfImage, signature);
	}

	static s_pattern find_signature(s_sighelper& signature) {
		std::list<s_memoryregion> memory_regions{};
		uintptr_t return_value{};

		// dump memory regions
		[&memory_regions]() -> void {
			memory_regions.clear();

			MEMORY_BASIC_INFORMATION32 mbi{};
			LPCVOID address = 0;

			while (VirtualQueryEx(GetCurrentProcess(), address, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&mbi), sizeof(mbi)) != 0) {
				if (mbi.State == MEM_COMMIT && mbi.Protect >= 0x10 && mbi.Protect <= 0x80)
					memory_regions.push_back(*new s_memoryregion(mbi));

				address = reinterpret_cast<LPCVOID>(mbi.BaseAddress + mbi.RegionSize);
			}
		}();

		for (const auto& region : memory_regions) {
			if (return_value)
				break;

			return_value = find_signature(region.base, region.region_size, signature).address;
		}

		return return_value;
	}

	strong_inline s_pattern find_signature(const char* hmodule, s_sighelper& signature) {
		return find_signature(GetModuleHandle(hmodule), signature);
	}

	static uintptr_t find_pattern_down(HMODULE hmodule, uintptr_t size, s_sighelper& signature) {
		if (!hmodule)
			return 0x0;

		const auto pattern_length = strlen(signature.sig);

		for (auto i = reinterpret_cast<uintptr_t>(hmodule); i < (reinterpret_cast<uintptr_t>(hmodule) + size) - pattern_length; i++) {
			bool found = true;

			for (size_t idx = 0; idx < pattern_length; idx++) {
				if (signature.sig[idx] != *(char*)(i + idx)) {
					found = false;
					break;
				}
			}

			if (found)
				return i + signature.offset;
		}

		return 0x0;
	}

	strong_inline uintptr_t find_str_ref(HMODULE hmodule, uintptr_t size, s_sighelper& signature) {
		// push null_address
		char push_address[] = { 0x68, 0x00, 0x00, 0x00, 0x00, 0x00 };

		s_sighelper txt(signature.sig);
		uintptr_t str_def = find_pattern_down(hmodule, size, txt);

		if (!str_def)
			return 0x0;

		// push str_def
		*(uintptr_t*)&push_address[1] = str_def;

		txt.sig = push_address;
		uintptr_t str_ref = find_pattern_down(hmodule, size, txt);

		return str_ref ? str_ref + signature.offset : 0x0;
	}

	__forceinline void attach_console(const char* name) {
#ifdef _DEBUG
		AllocConsole();
		freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
		SetConsoleTitleA(name);
#endif
	}

	__forceinline void detach_console() {
#ifdef _DEBUG
		fclose(reinterpret_cast<FILE*>(stdin));
		fclose(reinterpret_cast<FILE*>(stdout));
		FreeConsole();
		PostMessageW(GetConsoleWindow(), WM_CLOSE, 0, 0);
#endif
	}
}