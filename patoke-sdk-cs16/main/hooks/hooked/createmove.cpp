#include "../hooks.hpp"

void n_hooked::hk_createmove(float frametime, usercmd_s* cmd, int active) {
	static auto o_hook = n_detour::get<createmove_fn>(hk_createmove);
	
	o_hook(frametime, cmd, active);

	globals_i.local_player.update(interfaces_i.engine->GetLocalPlayer(), true);

	movement_i.bhop(cmd);
}