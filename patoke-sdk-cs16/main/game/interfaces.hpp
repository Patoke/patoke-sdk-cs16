#pragma once
#include "sdk/hlsdk.hpp"

struct s_module {
    template <typename type>
    constexpr type get() {
        return reinterpret_cast<type>(module_address);
    }
    constexpr uintptr_t get() {
        return module_address;
    }

    template <bool relative = false>
    void* find_signature(s_sighelper& sig) {
        if (relative)
            return n_utilities::find_signature((HMODULE)this->module_address, sig).relative<void*>();
        else
            return n_utilities::find_signature((HMODULE)this->module_address, sig).cast<void*>();
    }

    template <typename type, bool relative = false>
    strong_inline type find_signature(s_sighelper& sig) {
        return reinterpret_cast<type>(this->find_signature<relative>(sig));
    }

    strong_inline bool address_in_module(uintptr_t addr) {
        return addr > this->module_address 
#if IS_CS16
            && addr < (this->module_address + this->module_size)
#endif
            ;
    }

#ifdef _DEBUG
    char module_name[64]; // just make it 64 bytes, we don't need more...
#endif
    uintptr_t module_address;
#if IS_CS16
    uintptr_t module_size;
#endif
};

struct s_interfaces {

	cl_clientfunc_t* client;
	cl_enginefunc_t* engine;
	playermove_t* pmove;
    engine_studio_api_t* studio;
    r_studio_interface_t* studio_api;
    studio_model_renderer_t* studio_model;

    template<typename type, unsigned int dereference_count = 0>
    static type grab(uintptr_t address) {
        auto iface = reinterpret_cast<type>(address);

        for (unsigned int i = 0; i < dereference_count; i++)
            iface = *reinterpret_cast<type*>(iface);

        // fix for protected regions (this counts as a dereference!!!)
        memcpy(&iface, iface, sizeof(iface));

        return iface;
    }

    template<typename type, unsigned int dereference_count = 0>
    static type grab(s_module mod, uintptr_t start_addr, std::vector<uintptr_t> addresses) {
        type iface = reinterpret_cast<type>(start_addr);
        for (size_t i = 0; i < addresses.size(); i++) {
            iface = reinterpret_cast<type>(start_addr + addresses.at(i));

            for (unsigned int i = 0; i < dereference_count; i++)
                iface = *reinterpret_cast<type*>(iface);

            // we found the interface
            if (mod.address_in_module((uintptr_t)iface))
                break;
        }

        // epic fail
        if (!mod.address_in_module((uintptr_t)iface))
            return nullptr;

        // fix for protected regions (this counts as a dereference!!!)
        memcpy(&iface, iface, sizeof(iface));

        return iface;
    }

} extern interfaces_i;