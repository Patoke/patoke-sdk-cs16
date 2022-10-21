#pragma once
#include <globals.hpp>

struct s_initialization {

	// init
	void init_main();

	// data init
	void init_interfaces();
	void init_offsets();
	void init_static_signatures();

	// cleanup
	void cleanup_main();

} extern initialization_i;