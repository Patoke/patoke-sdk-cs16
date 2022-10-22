#pragma once
#include <globals.hpp>

struct s_ui {

	// main
	void draw();

	// containers
	void main_container();

	// variables
	bool is_open = true;
	int current_tab = 0;

} extern ui_i;