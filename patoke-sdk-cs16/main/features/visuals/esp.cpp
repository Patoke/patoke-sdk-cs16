#include "esp.hpp"

void s_esp::tracers() {
	if (!interfaces_i.engine || !globals_i.local_player.is_valid)
		return;

	for (int i = 1; i <= interfaces_i.engine->GetMaxClients(); i++) {
		s_entity entity;
		entity.update(interfaces_i.engine->GetEntityByIndex(i), false);

		if (!entity.is_valid || !entity.is_alive)
			continue;

		s_vec2 screen_pos;
		if (!interfaces_i.engine->world_to_screen(entity.origin, screen_pos))
			continue;

		ImGui::GetBackgroundDrawList()->AddLine(s_vec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y), screen_pos, ImColor(1.f, 1.f, 1.f));
	}
}