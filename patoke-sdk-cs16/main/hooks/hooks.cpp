#include "hooks.hpp"

bool s_hooks::initialize() {
	// hook addresses
	static void* adr_swap_buffers = reinterpret_cast<void*>(GetProcAddress(GetModuleHandle("opengl32.dll"), "wglSwapBuffers"));

	// initialize minhook
	if (MH_Initialize() != MH_OK)
		return false;

	// minhook hooks
	if (MH_CreateHook(adr_swap_buffers, &n_hooked::hk_swap_buffers, reinterpret_cast<void**>(&n_hooked::ofn_swap_buffers)) != MH_OK)
		return false;
	
	if (MH_CreateHook(interfaces_i.client->CL_CreateMove, &n_hooked::hk_createmove, reinterpret_cast<void**>(&n_hooked::ofn_createmove)) != MH_OK)
		return false;

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		return false;

	return true;
}

// @todo: fix crash on detach
void s_hooks::cleanup() {
	// unhook wndproc
	SetWindowLong(globals_i.get_game_window<HWND>(), GWL_WNDPROC, reinterpret_cast<long>(n_hooked::ofn_wndproc));

	// remove hooks and uninitialize minhook
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}