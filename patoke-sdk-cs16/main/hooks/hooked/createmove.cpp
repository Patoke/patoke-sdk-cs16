#include "../hooks.hpp"

void n_hooked::hk_createmove(float frametime, usercmd_s* cmd, int active) {
	ofn_createmove(frametime, cmd, active);

	printf("%.2f\n", cmd->sidemove);
}