#pragma once
// this file shall only be included for addresses grabbed in the s_initialization::init_static function

// helper for making signature scanning more convenient
struct s_sighelper {
    int offset{};
    const char* sig{};

    constexpr s_sighelper() : offset(0x0), sig("") {}
    constexpr s_sighelper(const char* sig) : offset(0x0), sig(sig) {}
    constexpr s_sighelper(int offset, const char* sig) : offset(offset), sig(sig) {}
};

#define MEMORY_INIT(name, sig_pattern)                                                                                                               \
    uintptr_t adr_##name = 0x0;                                                                                                                      \
    s_sighelper name##_sig = s_sighelper(sig_pattern);

#define OMEMORY_INIT(name, sig_pattern, sig_offset)                                                                                                  \
    uintptr_t adr_##name = 0x0;                                                                                                                      \
    s_sighelper name##_sig = s_sighelper(sig_offset, sig_pattern);

struct s_memory {

    // examples (source engine):
    //OMEMORY_INIT(prediction_player, "A3 ? ? ? ? 66 ? ? 86", 0x1);
    //OMEMORY_INIT(prediction_random_seed, "89 35 ? ? ? ? F3 0F 10 48", 0x2);



} extern memory_i;