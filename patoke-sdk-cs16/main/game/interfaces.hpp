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

#if IS_CS16
    template<typename type>
    type get_interface(const char* interface_name) {
        auto create_interface_fn = n_utilities::get_export_from_table<void*(*)(const char*, int*)>(this->module_address, HASH("CreateInterface"));
        return create_interface_fn(interface_name, NULL);
    }
#endif

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
    c_studiomodelrenderer* studio_model;

    template<typename type, unsigned int dereference_count = 0>
    static type grab(uintptr_t address) {
#ifdef _DEBUG
        const char* type_name = typeid(type).name();
#endif

        auto iface = reinterpret_cast<type>(address);

        for (unsigned int i = 0; i < dereference_count; i++)
            iface = *reinterpret_cast<type*>(iface);
        
#ifdef _DEBUG
        printf("[+] %s: 0x%X\n", type_name, uintptr_t(iface));
#endif

        // fix for protected regions
        memcpy(&iface, iface, sizeof(iface));

        return iface;
    }

    template<typename type, unsigned int dereference_count = 0>
    static type grab(s_module mod, uintptr_t start_addr, std::vector<uintptr_t> addresses) {
#ifdef _DEBUG
        const char* type_name = typeid(type).name();
#endif

        type iface = reinterpret_cast<type>(start_addr);
        for (size_t i = 0; i < addresses.size(); i++) {
            // make sure we point to the right thing
            type iface_copy = *reinterpret_cast<type*>(start_addr + addresses.at(i));;

            // we found the interface
            if (mod.address_in_module((uintptr_t)iface_copy)) {
                iface = reinterpret_cast<type>(start_addr + addresses.at(i));
                break;
            }
#ifdef _DEBUG
            else 
                printf("[!] didn't find %s (offset: 0x%X)\n", type_name, addresses.at(i));
#endif
        }

        for (unsigned int i = 0; i < dereference_count; i++)
            iface = *reinterpret_cast<type*>(iface);

#ifdef _DEBUG
        printf("[+] %s: 0x%X\n", type_name, uintptr_t(iface));
#endif

        // fix for protected regions
        memcpy(&iface, iface, sizeof(iface));

        return iface;
    }

} extern interfaces_i;