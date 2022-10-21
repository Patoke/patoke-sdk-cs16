#include "initialization.hpp"

// create init variables
s_initialization initialization_i;
//s_hooks hooks_i;

void s_initialization::init_main() {
	n_utilities::attach_console("patoke cs 1.6 sdk [debug console]");

	//init_interfaces();
	//init_offsets();
	//init_static_signatures();

	//if (!hooks_i.initialize())
	//	; // do something
}

void s_initialization::cleanup_main() {
	//hooks_i.cleanup();
	n_utilities::detach_console();
}