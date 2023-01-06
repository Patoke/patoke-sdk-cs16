#include "../hooks.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall n_hooked::hk_wndproc(HWND hwnd, unsigned int msg, WPARAM wparam, LPARAM lparam) {
	const auto get_button_toggle = [msg, wparam](bool& button, int key) {
		if (wparam != key)
			return;

		if (msg == WM_KEYUP)
			button = !button;
	};

	get_button_toggle(ui_i.is_open, VK_INSERT);

	if (ui_i.is_open && ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		return true;

	return CallWindowProc(ofn_wndproc, hwnd, msg, wparam, lparam);
}