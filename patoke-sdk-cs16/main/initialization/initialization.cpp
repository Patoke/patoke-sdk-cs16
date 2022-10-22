#include "initialization.hpp"
#include "../hooks/hooks.hpp"
#include "../game/interfaces.hpp"

// create init variables
s_initialization initialization_i;
s_interfaces interfaces_i;
s_hooks hooks_i;

void s_initialization::init_main() {
	n_utilities::attach_console("patoke cs 1.6 sdk [debug console]");

	init_interfaces();
	//init_static_signatures();

	if (!hooks_i.initialize())
		; // do something
}

void s_initialization::cleanup_main() {
	hooks_i.cleanup();
	n_utilities::detach_console();
}

bool s_initialization::init_modules() {
	auto get_module_size = [](void* address) {
		return static_cast<uintptr_t>(PIMAGE_NT_HEADERS(uintptr_t(address) + PIMAGE_DOS_HEADER(uintptr_t(address))->e_lfanew)->OptionalHeader.SizeOfImage);
	};

	// get modules and fix their sizes and bases
	char hardware_type = 0;
	hardware_dll = GetModuleHandle("hw.dll");
	client_dll = GetModuleHandle("client.dll");

	if (!hardware_dll) {
		hardware_type = 1;
		hardware_dll = GetModuleHandle("sw.dll");
	}

	if (!hardware_dll) {
		hardware_type = 2;
		hardware_dll = GetModuleHandle(NULL);
	}

	// failed to grab any of the module types
	if (!hardware_dll)
		return false;

	hardware_size = get_module_size(hardware_dll);

	// if we fail to grab the module size
	if (!hardware_size) {
		// set it to values we already knew beforehand
		switch (hardware_type) {
		case 0: hardware_size = 0x122A000; break; // hardware
		case 1: hardware_size = 0xB53000; break; // software
		case 2: hardware_size = 0x2116000; break; // neither (wtf!)
		}
	}

	hl_exe = globals_i.get_game_window<HMODULE>();
	hl_size = get_module_size(hl_exe);

	if (client_dll)
		client_size = get_module_size(client_dll);
	else {
		client_dll = hardware_dll;
		client_size = hardware_size;
	}

	if (!hardware_dll || !client_dll)
		return false;

	return true;
}

void s_initialization::init_interfaces() {
	if (!init_modules())
		return;

	interfaces_i.client = reinterpret_cast<cl_clientfunc_t*>(n_utilities::find_str_ref(hardware_dll, hardware_size, client_str));
	interfaces_i.engine = reinterpret_cast<cl_enginefunc_t*>(n_utilities::find_str_ref(client_dll, client_size, engine_str));
	interfaces_i.pmove = reinterpret_cast<playermove_t*>(n_utilities::find_str_ref(hardware_dll, hardware_size, pmove_str));

	// fix for protected memory region
	RtlCopyMemory(&interfaces_i.client, interfaces_i.client, sizeof(interfaces_i.client));
	RtlCopyMemory(&interfaces_i.engine, interfaces_i.engine, sizeof(interfaces_i.engine));

	printf("client: 0x%x\n", interfaces_i.client);
	printf("engine: 0x%x\n", interfaces_i.engine);
}

void s_initialization::init_static_signatures() {

}