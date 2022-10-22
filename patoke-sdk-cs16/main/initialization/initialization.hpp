#pragma once
#include <globals.hpp>

struct s_initialization {

	// init
	void init_main();

	// data init
	bool init_modules();
	void init_interfaces();
	void init_static_signatures();

	// cleanup
	void cleanup_main();

	// modules
	HMODULE hl_exe;
	HMODULE hardware_dll;
	HMODULE client_dll;
	uintptr_t hl_size;
	uintptr_t hardware_size;
	uintptr_t client_size;

	// signatures
	s_sighelper client_str = s_sighelper(0x13, "ScreenFade");
	s_sighelper engine_str = s_sighelper(0x12, "sprites/%s.spr");

} extern initialization_i;