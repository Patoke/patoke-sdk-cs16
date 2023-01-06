#pragma once
#include <globals.hpp>

struct s_initialization {

	// init
	void init_main();

	// data init
	void init_interfaces();
	void init_static();
	bool init_modules();

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
	s_sighelper pmove_str = s_sighelper(0x18, "ScreenFade");

} extern initialization_i;

// modules
extern std::map<unsigned int, s_module> g_module_list; // global accessor for module list (slow.)
extern std::vector<s_module> m;                        // global accessor for modules (fast.)

// modules to grab
enum e_modules {
	GAME,
	CLIENT,
	HARDWARE,
	OPENGL32,
	SOFTWARE
};

#define DEFINE_MODULE(idx, module_name) m.insert(m.begin() + idx, g_module_list.find(HASH(module_name))->second);