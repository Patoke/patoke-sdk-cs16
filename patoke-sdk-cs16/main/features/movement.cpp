#include "movement.hpp"

void s_movement::bhop(usercmd_s* cmd) {
	bool is_bad_movetype = globals_i.local_player.movetype == MOVETYPE_NOCLIP || globals_i.local_player.movetype == MOVETYPE_FLY;
	bool in_water = globals_i.local_player.waterlevel >= 2;

	if (is_bad_movetype || in_water)
		return;

	if (cmd->buttons & IN_JUMP && !(globals_i.local_player.flags & FL_ONGROUND))
		cmd->buttons &= ~IN_JUMP;
}