#include "hooks.hpp"

bool s_hooks::initialize() {
	// hook addresses
	auto adr_swap_buffers = reinterpret_cast<void*>(n_utilities::get_export_from_table(m[OPENGL32].get(), HASH("wglSwapBuffers")));
	auto adr_create_move = reinterpret_cast<void*>(interfaces_i.client->CL_CreateMove);

	if (!n_detour::init())
		return false;

	CREATE_HOOK(adr_swap_buffers, n_hooked::hk_swap_buffers);
	CREATE_HOOK(adr_create_move, n_hooked::hk_createmove);

	n_detour::enable();

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