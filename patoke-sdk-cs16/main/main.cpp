#include <globals.hpp>
#include "initialization/initialization.hpp"

// create globals instance
s_globals globals_i;

unsigned long __stdcall main_thread(void* hmodule) {
    while (!globals_i.initialize_window()) // wait until the game window is valid
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    initialization_i.init_main();

    // detach if the end key is pressed
    while (!GetAsyncKeyState(VK_HOME))
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(hmodule), TRUE);
}

void __stdcall on_detach_module() {
    initialization_i.cleanup_main();
}

int __stdcall DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved) {
    DisableThreadLibraryCalls(hmodule);

    switch (reason) {
    case DLL_PROCESS_ATTACH: {
        HANDLE thread_instance = CreateThread(nullptr, NULL, main_thread, hmodule, NULL, nullptr);

        if (thread_instance)
            CloseHandle(thread_instance);
        else
            return FALSE;

    } break;
    case DLL_PROCESS_DETACH:
        if (!reserved) // don't do cleanup when the app is in a closing scenario
            on_detach_module();
        break;
    }

    return TRUE;
}