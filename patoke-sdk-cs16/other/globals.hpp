#pragma once
#include <precompiled.hpp>
#include <utilities/utilities.hpp>
#include <libraries/wrappers/imgui_wrapper.hpp>
#include <../main/ui/ui.hpp>

struct s_globals {

	// don't use this raw, please use the get_game_window() wrapper
	void* game_window;
	unsigned long game_pid;

	bool initialize_window() {
		game_window = GetModuleHandle(NULL);

		if (!game_window)
			return false;

		game_pid = _getpid();

		if (!game_pid)
			return false;

		return true;
	}

	template<typename type>
	constexpr type get_game_window() {
		return reinterpret_cast<type>(game_window);
	}

} extern globals_i;