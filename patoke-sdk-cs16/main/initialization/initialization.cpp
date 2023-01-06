#include "initialization.hpp"
#include "../hooks/hooks.hpp"
#include "../game/interfaces.hpp"

// create init variables
s_initialization initialization_i;
s_interfaces interfaces_i;
s_hooks hooks_i;
s_render render_i;

// initialize module list
std::map<unsigned int, s_module> g_module_list; // global accessor for module list (slow.)
std::vector<s_module> m;                        // global accessor for modules (fast.)

void s_initialization::init_main() {

	init_interfaces();
	//init_static();

	if (!hooks_i.initialize())
		; // do something
}

void s_initialization::cleanup_main() {
	hooks_i.cleanup();
	n_utilities::detach_console();
}

bool s_initialization::init_modules() {
	// the size of an int is 4 in 32 bits, 8 in 64 bits, and so on, this will work for any build arch
	constexpr unsigned int peb_offset = 0x30 * (sizeof(unsigned int) / 4);
	auto peb = reinterpret_cast<PEB*>(__readfsdword(peb_offset));

	LIST_ENTRY list_head = peb->Ldr->InMemoryOrderModuleList;
	LIST_ENTRY list_curr = list_head;

	// get modules every time we know the game hasn't fully loaded
	while (g_module_list.find(HASH("serverbrowser.dll")) == g_module_list.end()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // don't kill pc pls

		for (auto list_curr = list_head; list_curr.Flink != &peb->Ldr->InMemoryOrderModuleList; list_curr = *list_curr.Flink) {
			auto mod = CONTAINING_RECORD(list_curr.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

			// module has a valid name
			if (mod->BaseDllName.Buffer) {
				// convert unicode to multibyte
				char mod_name[128];
				wcstombs(mod_name, mod->BaseDllName.Buffer, mod->BaseDllName.Length);

				auto mod_name_hash = HASH(mod_name);

				// if this module already exists in our map
				if (g_module_list.find(mod_name_hash) != g_module_list.end())
					continue;

				s_module new_module{};
#ifdef _DEBUG
				strcpy(new_module.module_name, mod_name);
#endif
				new_module.module_address = reinterpret_cast<uintptr_t>(mod->DllBase);

#if IS_CS16
				new_module.module_size = mod->SizeOfImage;
#endif

				g_module_list.insert({ mod_name_hash, new_module });
			}
		}
	}

	// add new modules here (please wait until i figure out a way to get them all in 1 shot)
	DEFINE_MODULE(GAME, "hl.exe"); // get this using the first grabbed module index, some anticheats change the module name :P
	DEFINE_MODULE(CLIENT, "client.dll");
	DEFINE_MODULE(HARDWARE, "hw.dll"); // probably call this engine?
	DEFINE_MODULE(OPENGL32, "OPENGL32.dll");

	// do this another way if possible, this is only a placeholder until i fix crash on module definition
	if (!m[HARDWARE].get())
		DEFINE_MODULE(SOFTWARE, "sw.dll"); // this isn't ever going to be used as hardware replaces all these

	// fix modules
	char hardware_fallback = 0; // (for hardcoded sizes) 0 = hardware, 1 = software, 2 = neither (under version 3k)
	if (!m[HARDWARE].get() && m[SOFTWARE].get())
		m[HARDWARE] = m[SOFTWARE]; // for cross compatibility

	if (!m[HARDWARE].get())
		m[HARDWARE] = m[GAME]; // for cross compatibility

	if (!m[HARDWARE].module_size) {
		// set it to values we already knew beforehand
		switch (hardware_fallback) {
		case 0: m[HARDWARE].module_size = 0x122A000; break; // hardware
		case 1: m[HARDWARE].module_size = 0xB53000; break; // software
		case 2: m[HARDWARE].module_size = 0x2116000; break; // neither
		}
	}

	if (!m[CLIENT].get())
		m[CLIENT] = m[HARDWARE]; // for cross compatibility

	bool failed_to_grab_modules = false;
	for (auto& hmodule : m) {
		if (!hmodule.get()) {
			failed_to_grab_modules = true;
#ifdef _DEBUG
#if IS_CS16
			printf("[-] failed to grab %s (size: 0x%X)\n", hmodule.module_name, hmodule.module_size);
		}
		else {
			printf("[+] %s: 0x%X (size: 0x%X)\n", hmodule.module_name, hmodule.get(), hmodule.module_size);
#else
			printf("[-] failed to grab %s\n", hmodule.module_name);
		}
		else {
			printf("[+] %s: 0x%X\n", hmodule.module_name, hmodule.get());
#endif
#endif
		}
	}

	return !failed_to_grab_modules;
}

void s_initialization::init_interfaces() {
	if (!init_modules())
		return;

	interfaces_i.client = s_interfaces::grab<cl_clientfunc_t*>(n_utilities::find_str_ref(m[HARDWARE].get<HMODULE>(), m[HARDWARE].module_size, client_str));
	interfaces_i.engine = s_interfaces::grab<cl_enginefunc_t*>(n_utilities::find_str_ref(m[CLIENT].get<HMODULE>(), m[HARDWARE].module_size, engine_str));
	interfaces_i.pmove = s_interfaces::grab<playermove_t*>(n_utilities::find_str_ref(m[HARDWARE].get<HMODULE>(), m[HARDWARE].module_size, pmove_str));

	// always do these in order from oldest build to latest
	interfaces_i.studio = s_interfaces::grab<engine_studio_api_t*, 1>(m[CLIENT], (uintptr_t)interfaces_i.client->HUD_GetStudioModelInterface, { 0x30, 0x1A });
	
	// @todo: fix these
	//interfaces_i.studio_api = s_interfaces::grab<r_studio_interface_t*>(m[CLIENT], (uintptr_t)interfaces_i.client->HUD_GetStudioModelInterface, { 0x36, 0x20 });
	//interfaces_i.studio_model = s_interfaces::grab<studio_model_renderer_t*>(m[CLIENT], (uintptr_t)interfaces_i.studio_api->StudioDrawModel, { 0x5 });

	printf("[+] interfaces_i.client: 0x%X\n", interfaces_i.client);
	printf("[+] interfaces_i.engine: 0x%X\n", interfaces_i.engine);
	printf("[+] interfaces_i.pmove: 0x%X\n", interfaces_i.pmove);
	printf("[+] interfaces_i.studio: 0x%X\n", interfaces_i.studio);
	//printf("[+] interfaces_i.studio_api: 0x%X | %i\n", interfaces_i.studio_api, interfaces_i.studio_api->version);
	//printf("[+] interfaces_i.studio_model: 0x%X\n", interfaces_i.studio_model);
}

void s_initialization::init_static() {

}