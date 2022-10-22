#include "../hooks.hpp"

bool __stdcall n_hooked::hk_swap_buffers(HDC hdc) {
	if (!imgui_wrapper_i.did_initialize_imgui) {
		// hook wndproc
		auto game_window_dc = WindowFromDC(hdc);
		ofn_wndproc = reinterpret_cast<WNDPROC>(SetWindowLong(game_window_dc, GWL_WNDPROC, reinterpret_cast<long>(hk_wndproc)));

		// initialize imgui
		imgui_wrapper_i.setup_imgui(hdc);

		imgui_wrapper_i.did_initialize_imgui = true;
	}

	imgui_wrapper_i.create_frame(); {

		// do imgui stuff here
		ui_i.draw();

	} imgui_wrapper_i.end_frame();

	return ofn_swap_buffers(hdc);
}