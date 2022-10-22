#include "ui.hpp"

// ui instance
s_ui ui_i;

void s_ui::draw() {
	if (!is_open)
		return;

	ImGui::SetNextWindowSize({ 300, 200 }, ImGuiCond_Once);
	ImGui::Begin("patoke-sdk", 0); {

		// draw containers (tabs)
		switch (current_tab) {
		case 0: main_container(); break;
		default: break;
		}

	} ImGui::End();
}

// containers
void s_ui::main_container() {

}