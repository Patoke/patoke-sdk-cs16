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

	s_vec3 origin;
	s_vec3 velocity;
	s_vec3 eye_pos;

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
	}

	void update(cl_entity_s* ent, bool is_local) {
		if (!ent)
			return;

		this->ent = ent;
		this->is_valid = this->ent && this->ent->model && this->ent->player && this->ent->index;

		if (!this->is_valid)
			return;

		this->index = this->ent->index;
		this->message_num = this->ent->curstate.messagenum;
		this->observer_state = this->ent->curstate.iuser1;
		this->observer_index = this->ent->curstate.iuser2;

		this->is_alive = !this->observer_state;

		this->is_local = is_local;
		if (this->is_local)
			return update_local();

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

	bool initialize_window() {
		game_window = GetModuleHandle(NULL);

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