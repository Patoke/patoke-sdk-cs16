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
	s_matrix3x4 bone_matrix[MAXSTUDIOBONES];
	s_vec3 hitboxes[HITBOX_MAX];

	void update_bones() {
		if (!this->ent || !this->ent->model || this->ent->index < 0 || this->ent->index > MAX_CLIENTS)
			return;

		auto studio_header = reinterpret_cast<studiohdr_t*>(interfaces_i.studio->Mod_Extradata(this->ent->model));

		// i'm sorry.
		auto bone_matrix = *interfaces_i.studio->StudioGetBoneTransform();
		for (int i = 0; i < MAXSTUDIOBONES; i++)
			this->bone_matrix[i] = bone_matrix[i];

		auto hitbox = (mstudiobbox_t*)((uintptr_t)(studio_header) + studio_header->hitboxindex);

		if (!hitbox)
			return;

		s_vec3 bbox_mins, bbox_maxs;

		for (int i = 0; i < studio_header->numhitboxes; i++) {
			math_i.vector_transform(hitbox[i].bbmin, this->bone_matrix[hitbox[i].bone], bbox_mins);
			math_i.vector_transform(hitbox[i].bbmax, this->bone_matrix[hitbox[i].bone], bbox_maxs);

			this->hitboxes[i] = bbox_maxs + bbox_mins;
			this->hitboxes[i] *= 0.5f;
		}
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
	}

	void update(cl_entity_s* ent, bool is_local) {
		if (!ent)
			return;

		this->ent = ent;
		this->index = this->ent->index;

		// don't update the local player over 1 time
		if (!is_local && this->index == interfaces_i.engine->GetLocalPlayer()->index)
			return;

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

		this->use_hull = this->ent->curstate.usehull;
		this->origin = this->ent->origin;
		// calculate their velocity (i don't think it's networked?)
		this->velocity = (this->ent->curstate.origin - this->ent->prevstate.origin) / (this->ent->curstate.animtime - this->ent->prevstate.animtime);
		this->eye_pos = this->origin + s_vec3(0, 0, this->use_hull == DUCKED_HULL ? PM_VEC_DUCK_VIEW : PM_VEC_VIEW);
	
		//update_bones();
	}
};

struct s_globals {

	// don't use this raw, please use the get_game_window() wrapper
	void* game_window;
	unsigned long game_pid;
	
	// game specific
	s_entity local_player;
	std::vector<s_entity> entity_list;

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