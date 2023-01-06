#pragma once
#include "pattern/pattern.hpp"
#include <position/g_pos.hpp>
#include <memory.hpp>

// helper for signature scanning results
struct s_pattern {
    uintptr_t address;

    constexpr s_pattern() : address(0) {}

    constexpr s_pattern(uintptr_t address) : address(address) {}

    template <typename t>
    constexpr t cast() {
        return reinterpret_cast<t>(address);
    }

    template <typename t>
    constexpr t scast() {
        return static_cast<t>(address);
    }

    template <typename t>
    constexpr t ccast() {
        return const_cast<t>(address);
    }

    template <typename t>
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
    // helpers for virtual functions
    template <unsigned int idx, typename t, typename... args>
    constexpr t call_vfunc(void* thisptr, args... arg_list) {
        using fn = t(__thiscall*)(void*, decltype(arg_list)...);
        return (*static_cast<fn**>(thisptr))[idx](thisptr, arg_list...);
    }

    template <typename t>
    constexpr t get_vfunc(void* class_pointer, size_t index) {
        return (t)(*(t**)class_pointer)[index];
    }

    unsigned constexpr hash_str(char const* input) {
        constexpr unsigned int past_char_additive = 33;
        constexpr unsigned int default_output = 5381;

        return *input ? static_cast<unsigned int>(*input) + past_char_additive * hash_str(input + 1) : default_output;
    }

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

    static uintptr_t get_export_from_table(uintptr_t module_base, unsigned int export_hash) {
        auto dos_headers = reinterpret_cast<IMAGE_DOS_HEADER*>(module_base);
        auto nt_headers = reinterpret_cast<IMAGE_NT_HEADERS*>(module_base + dos_headers->e_lfanew);
        IMAGE_OPTIONAL_HEADER* optional_header = &nt_headers->OptionalHeader;

        uintptr_t exportdir_address = optional_header->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

        // we aint got no exports :c
        if (optional_header->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size <= 0U)
            return 0x0;

        auto export_directory = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(module_base + exportdir_address);
        auto names_rva = reinterpret_cast<uintptr_t*>(module_base + export_directory->AddressOfNames);
        auto functions_rva = reinterpret_cast<uintptr_t*>(module_base + export_directory->AddressOfFunctions);
        auto name_ordinals = reinterpret_cast<unsigned short*>(module_base + export_directory->AddressOfNameOrdinals);

        // perform binary search
        for (size_t i = 0; i < export_directory->AddressOfFunctions; i++) {
            const char* export_name = reinterpret_cast<const char*>(module_base + names_rva[i]);

            if (export_hash == hash_str(export_name))
                return module_base + functions_rva[name_ordinals[i]];
        }

        return 0x0;
    }
    template <typename type>
    strong_inline type get_export_from_table(uintptr_t module_base, unsigned int export_hash) {
        return reinterpret_cast<type>(get_export_from_table(module_base, export_hash));
    }

    strong_inline void attach_console(const char* name) {
#ifdef _DEBUG
        AllocConsole();
        freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        SetConsoleTitleA(name);
#endif
    }

    strong_inline void detach_console() {
#ifdef _DEBUG
        fclose(reinterpret_cast<FILE*>(stdin));
        fclose(reinterpret_cast<FILE*>(stdout));
        FreeConsole();
        PostMessageW(GetConsoleWindow(), WM_CLOSE, 0, 0);
#endif
    }
} // namespace n_utilities

// utility helpers
#define HASH(input) n_utilities::hash_str(input)