#include "../hooks.hpp"

void n_hooked::hk_createmove(float frametime, usercmd_s* cmd, int active) {
	ofn_createmove(frametime, cmd, active);

	globals_i.local_player.update(interfaces_i.engine->GetLocalPlayer(), true);

	movement_i.bhop(cmd);
}