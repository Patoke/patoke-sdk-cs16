#pragma once
#include "const.hpp"
#include "playermove.hpp"
#include "model.hpp"

#define HISTORY_MAX		64  // Must be power of 2
#define HISTORY_MASK	( HISTORY_MAX - 1 )

#define ENTITY_NORMAL	(1<<0)
#define ENTITY_BEAM	(1<<1)

#define REGULAR_HULL 0
#define DUCKED_HULL 1
#define POINT_HULL 2

typedef struct entity_state_s
{
	int entityType;
	int number;			// Index into cl_entities array for this entity.
	float msg_time;
	int messagenum;			// Message number last time the player/entity state was updated.
	s_vec3 origin;			// Fields which can be transitted and reconstructed over the network stream
	s_angle angles;
	int modelindex;
	int sequence;
	float frame;
	int colormap;
	short skin;
	short solid;
	int effects;
	float scale;
	byte eflags;
	int rendermode;			// Render information
	int renderamt;
	color24 rendercolor;
	int renderfx;
	int movetype;
	float animtime;
	float framerate;
	int body;
	byte controller[4];
	byte blending[4];
	s_vec3 velocity;
	s_vec3 mins;			// Send bbox down to client for use during prediction.
	s_vec3 maxs;
	int aiment;
	int owner; 			// If owned by a player, the index of that player ( for projectiles ).
	float friction;			// Friction, for prediction.
	float gravity;			// Gravity multiplier
	int team;
	int playerclass;
	int health;
	int spectator;
	int weaponmodel;
	int gaitsequence;
	s_vec3 basevelocity;		// If standing on conveyor, e.g.
	int usehull;			// Use the crouched hull, or the regular player hull.
	int oldbuttons;			// Latched buttons last time state updated.
	int onground;			// -1 = in air, else pmove entity number
	int iStepLeft;
	float flFallVelocity;		// How fast we are falling
	float fov;
	int weaponanim;
	s_vec3 startpos;		// Parametric movement overrides
	s_vec3 endpos;
	float impacttime;
	float starttime;
	int iuser1;
	int iuser2;
	int iuser3;
	int iuser4;
	float fuser1;
	float fuser2;
	float fuser3;
	float fuser4;
	s_vec3 vuser1;
	s_vec3 vuser2;
	s_vec3 vuser3;
	s_vec3 vuser4;
} entity_state_t;

typedef struct clientdata_s
{
	s_vec3 origin;
	s_vec3 velocity;
	int viewmodel;
	s_angle punchangle;
	int flags;
	int waterlevel;
	int watertype;
	s_vec3 view_ofs;
	float health;
	int bInDuck;
	int weapons;
	int flTimeStepSound;
	int flDuckTime;
	int flSwimTime;
	int waterjumptime;
	float maxspeed;
	float fov;
	int weaponanim;
	int m_iId;
	int ammo_shells;
	int ammo_nails;
	int ammo_cells;
	int ammo_rockets;
	float m_flNextAttack;
	int tfstate;
	int pushmsec;
	int deadflag;
	char physinfo[MAX_PHYSINFO_STRING];
	int iuser1;
	int iuser2;
	int iuser3;
	int iuser4;
	float fuser1;
	float fuser2;
	float fuser3;
	float fuser4;
	s_vec3 vuser1;
	s_vec3 vuser2;
	s_vec3 vuser3;
	s_vec3 vuser4;

} clientdata_t;

typedef struct efrag_s
{
	struct mleaf_s* leaf;
	struct efrag_s* leafnext;
	struct cl_entity_s* entity;
	struct efrag_s* entnext;
} efrag_t;

typedef struct
{
	byte					mouthopen;		// 0 = mouth closed, 255 = mouth agape
	byte					sndcount;		// counter for running average
	int						sndavg;			// running average
} mouth_t;

typedef struct
{
	float					prevanimtime;
	float					sequencetime;
	byte					prevseqblending[2];
	s_vec3					prevorigin;
	s_angle					prevangles;

	int						prevsequence;
	float					prevframe;

	byte					prevcontroller[4];
	byte					prevblending[2];
} latchedvars_t;

typedef struct
{
	// Time stamp for this movement
	float					animtime;

	s_vec3					origin;
	s_angle					angles;
} position_history_t;

typedef struct cl_entity_s
{
	int						index;      // Index into cl_entities ( should match actual slot, but not necessarily )

	int				player;     // True if this entity is a "player"

	entity_state_t			baseline;   // The original state from which to delta during an uncompressed message
	entity_state_t			prevstate;  // The state information from the penultimate message received from the server
	entity_state_t			curstate;   // The state information from the last message received from server

	int						current_position;  // Last received history update index
	position_history_t		ph[HISTORY_MAX];   // History of position and angle updates for this player

	mouth_t					mouth;			// For synchronizing mouth movements.

	latchedvars_t			latched;		// Variables used by studio model rendering routines

	// Information based on interplocation, extrapolation, prediction, or just copied from last msg received.
	//
	float					lastmove;

	// Actual render position and angles
	s_vec3					origin;
	s_angle					angles;

	// Attachment points
	s_vec3					attachment[4];

	// Other entity local information
	int						trivial_accept;

	struct model_s* model;			// cl.model_precache[ curstate.modelindes ];  all visible entities have a model
	struct efrag_s* efrag;			// linked list of efrags
	struct mnode_s* topnode;		// for bmodels, first world node that splits bmodel, or NULL if not split

	float					syncbase;		// for client-side animations -- used by obsolete alias animation system, remove?
	int						visframe;		// last frame this entity was found in an active leaf
	colorVec				cvFloorColor;
} cl_entity_t;