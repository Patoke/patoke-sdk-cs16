#pragma once
#include <precompiled.hpp>
#include <utilities/utilities.hpp>
#include <libraries/wrappers/imgui_wrapper.hpp>
#include <../main/ui/ui.hpp>
#include <../main/game/interfaces.hpp>

struct s_entity {
	cl_entity_s* ent;

	int index;
	int message_num;
	int	observer_state;
	int	observer_index;
	int use_hull;
	int flags;
	int movetype;
	int waterlevel;

	bool is_valid;
	bool is_local;
	bool is_alive;
	bool is_dormant; // in pvs

	s_vec3 origin;
	s_vec3 velocity;
	s_vec3 eye_pos;
	s_matrix3x4 bone_matrix[MAXSTUDIOBONES];
	s_vec3 hitboxes[HITBOX_MAX];

	void update_bones() {
		// @todo: reimplement this
		// do better entity list stuff and get bones and all that's possible in StudioDrawModel
	}

	void update_local() {
		if (!interfaces_i.pmove)
			return;

		this->use_hull = interfaces_i.pmove->usehull;
		this->origin = interfaces_i.pmove->origin;
		this->velocity = interfaces_i.pmove->velocity;
		this->eye_pos = this->origin + interfaces_i.pmove->view_ofs;
		this->flags = interfaces_i.pmove->flags;
		this->movetype = interfaces_i.pmove->movetype;
		this->waterlevel = interfaces_i.pmove->waterlevel;

		// the local player can't go dormant, we assume it's dormant when we die but this is incorrect
		if (this->is_alive)
			this->is_dormant = true;
		else
			this->is_dormant = false;
	}

	void update(cl_entity_s* ent, bool is_local) {
		if (!ent)
			return;

		this->ent = ent;
		this->index = this->ent->index;

		// don't update the local player over 1 time
		cl_entity_s* local = interfaces_i.engine->GetLocalPlayer(); // only used for some stuff
		if (!is_local && this->index == local->index) {
			this->is_valid = false;
			return;
		}

		this->is_valid = this->ent && this->ent->model && this->ent->player && this->index && this->index > 0 && this->index < MAX_CLIENTS;

		if (!this->is_valid)
			return;

		this->message_num = this->ent->curstate.messagenum;
		this->observer_state = this->ent->curstate.iuser1;
		this->observer_index = this->ent->curstate.iuser2;

		this->is_alive = !this->observer_state;

		this->is_local = is_local;
		if (this->is_local)
			return update_local();

		// player is dormant, don't update
		if (this->message_num < local->curstate.messagenum) {
			this->is_valid = false;
			this->is_dormant = true;
			return;
		}

		this->is_dormant = false;
		this->use_hull = this->ent->curstate.usehull;
		this->origin = this->ent->origin;
		// calculate their velocity (i don't think it's networked?)
		this->velocity = (this->ent->curstate.origin - this->ent->prevstate.origin) / (this->ent->curstate.animtime - this->ent->prevstate.animtime);
		this->eye_pos = this->origin + s_vec3(0, 0, this->use_hull == DUCKED_HULL ? PM_VEC_DUCK_VIEW : PM_VEC_VIEW);
	}
};

struct s_globals {

	// don't use this raw, please use the get_game_window() wrapper
	void* game_window;
	unsigned long game_pid;
	
	// game specific
	s_entity local_player;
	std::array<s_entity, MAX_CLIENTS> entity_list;

	bool initialize_window(uintptr_t game_base) {
		game_window = reinterpret_cast<void*>(game_base);

		if (!game_window)
			return false;

		game_pid = _getpid();

		if (!game_pid)
			return false;

		return true;
	}

	template<typename type>
	constexpr type get_game_window() {
		return reinterpret_cast<type>(game_window);
	}

} extern globals_i;