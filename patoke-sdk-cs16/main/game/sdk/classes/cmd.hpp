#pragma once

typedef int (*pfnUserMsgHook)(const char*, int, void*);
#define MAX_ALIAS_NAME	32

typedef struct ClientUserMsg
{
	int msg;
	int size;
	char name[16];
	struct ClientUserMsg* next;
	pfnUserMsgHook pfn;
} *PClientUserMsg;

typedef struct cmdalias_s
{
	struct cmdalias_s* next;
	char				name[MAX_ALIAS_NAME];
	char* value;
} cmdalias_t;

typedef struct usercmd_s
{
	short lerp_msec;		// Interpolation time on client
	byte msec;			// Duration in ms of command
	s_vec3 viewangles;		// Command view angles.

	// intended velocities
	float forwardmove;		// Forward velocity.
	float sidemove;			// Sideways velocity.
	float upmove;			// Upward velocity.
	byte lightlevel;		// Light level at spot where we are standing.
	unsigned short buttons;		// Attack buttons
	byte impulse;			// Impulse command issued.
	byte weaponselect;		// Current weapon id

	// Experimental player impact stuff.
	int impact_index;
	s_vec3 impact_position;

} usercmd_t;