#pragma once
#include <libraries/minhook/minhook.h>

// little helper to make the hook function take up only 1 line and less chars
#ifdef _DEBUG
#define CREATE_HOOK(adr, hk_fn)                                                                                                                      \
    if (!n_detour::hook(adr, hk_fn)) {                                                                                                               \
        printf("[x] failed to hook %s: 0x%X\n", #hk_fn, uintptr_t(##adr));                                                                                         \
        return false;                                                                                                                                \
    } else                                                                                                                                           \
        printf("[+] hooked %s: 0x%X\n", #hk_fn, uintptr_t(##adr));
#else
#define CREATE_HOOK(adr, hk_fn)                                                                                                                      \
    if (!n_detour::hook(adr, hk_fn))                                                                                                                 \
        return false
#endif

namespace n_detour {
    struct s_hook {
        void *function;
        void *original;
    };

    inline std::unordered_map<void *, s_hook> detours;

    static bool hook(void *target, void *fn) {
        if (!target || !fn)
            return false;

        detours[fn].function = target;

        return MH_CreateHook(target, fn, &detours[fn].original) == MH_OK;
    }

    strong_inline bool init() {
        return MH_Initialize() == MH_OK;
    }

    strong_inline void enable() {
        for (auto &detour : detours)
            MH_EnableHook(detour.second.function);
    }

    strong_inline void restore() {
        for (auto &detour : detours) {
            MH_DisableHook(detour.second.function);
            MH_RemoveHook(detour.second.function);
        }

        MH_Uninitialize();
    }

    template <typename t>
    inline t get(void *fn) {
        return static_cast<t>(detours[fn].original);
    }
} // namespace n_detour