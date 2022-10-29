#pragma once
#include <globals.hpp>
#include <libraries/minhook/minhook.h>

// feature includes
#include "../features/features.hpp"

struct s_hooks {

	// init
	bool initialize();

	// cleanup
	void cleanup();

} extern hooks_i;

namespace n_hooked {
	// hook definitions
	typedef bool(__stdcall* swap_buffers_fn)(HDC);
	typedef void(__cdecl* createmove_fn)(float, usercmd_s*, int);

	// ohook declarations
	inline swap_buffers_fn ofn_swap_buffers{};
	inline WNDPROC ofn_wndproc{};
	inline createmove_fn ofn_createmove{};

	// hook declarations
	bool __stdcall hk_swap_buffers(HDC hdc);
	LRESULT __stdcall hk_wndproc(HWND hwnd, unsigned int msg, WPARAM wparam, LPARAM lparam);
	void hk_createmove(float frametime, usercmd_s* cmd, int active);
}