#pragma once
#include "const.hpp"
#include "cmd.hpp"

#define MAX_PHYSENTS 600 		  		// Must have room for all entities in the world.
#define MAX_MOVEENTS 64
#define MAX_CLIP_PLANES	5

#define PM_NORMAL			0x00000000
#define PM_STUDIO_IGNORE		0x00000001	// Skip studio models
#define PM_STUDIO_BOX			0x00000002	// Use boxes for non-complex studio models (even in traceline)
#define PM_GLASS_IGNORE			0x00000004	// Ignore entities with non-normal rendermode
#define PM_WORLD_ONLY			0x00000008	// Only trace against the world

#define PM_TRACELINE_PHYSENTSONLY	0
#define PM_TRACELINE_ANYVISIBLE		1

#define MAX_PHYSINFO_STRING 256

#define CTEXTURESMAX		1024	// max number of textures loaded
#define CBTEXTURENAMEMAX	17	// only load first n chars of name

#define CHAR_TEX_CONCRETE	'C'	// texture types
#define CHAR_TEX_METAL		'M'
#define CHAR_TEX_DIRT		'D'
#define CHAR_TEX_VENT		'V'
#define CHAR_TEX_GRATE		'G'
#define CHAR_TEX_TILE		'T'
#define CHAR_TEX_SLOSH		'S'
#define CHAR_TEX_WOOD		'W'
#define CHAR_TEX_COMPUTER	'P'
#define CHAR_TEX_GRASS		'X'
#define CHAR_TEX_GLASS		'Y'
#define CHAR_TEX_FLESH		'F'
#define CHAR_TEX_SNOW		'N'

#define PM_DEAD_VIEWHEIGHT	-8

#define OBS_NONE			0
#define OBS_CHASE_LOCKED	1
#define OBS_CHASE_FREE		2
#define OBS_ROAMING			3
#define OBS_IN_EYE			4
#define OBS_MAP_FREE		5
#define OBS_MAP_CHASE		6

#define STEP_CONCRETE		0
#define STEP_METAL			1
#define STEP_DIRT			2
#define STEP_VENT			3
#define STEP_GRATE			4
#define STEP_TILE			5
#define STEP_SLOSH			6
#define STEP_WADE			7
#define STEP_LADDER			8
#define STEP_SNOW			9

#define WJ_HEIGHT			8
#define STOP_EPSILON				0.1
#define MAX_CLIMB_SPEED				200
#define PLAYER_DUCKING_MULTIPLIER		0.333
#define PM_CHECKSTUCK_MINTIME			0.05	// Don't check again too quickly.

#define PLAYER_LONGJUMP_SPEED			350.0f	// how fast we longjump

// Ducking time
#define TIME_TO_DUCK				0.4
#define STUCK_MOVEUP				1

#define PM_VEC_DUCK_HULL_MIN			-18.f
#define PM_VEC_HULL_MIN				-36.f
#define PM_VEC_DUCK_VIEW			12.f
#define PM_VEC_VIEW				17.f

#define PM_PLAYER_MAX_SAFE_FALL_SPEED		504.8F	// approx 20 feet
#define PM_PLAYER_MIN_BOUNCE_SPEED		350
#define PM_PLAYER_FALL_PUNCH_THRESHHOLD		250	// won't punch player's screen/make scrape noise unless player falling at least this fast.
#define PM_PLAYER_FATAL_FALL_SPEED		980.0F
#define PM_DAMAGE_FOR_FALL_SPEED	100.0F / (PM_PLAYER_FATAL_FALL_SPEED - PM_PLAYER_MAX_SAFE_FALL_SPEED);

// Only allow bunny jumping up to 1.2x server / player maxspeed setting
#define BUNNYJUMP_MAX_SPEED_FACTOR		1.2f

typedef struct
{
	s_vec3	normal;
	float	dist;
} pmplane_t;

typedef struct pmtrace_s pmtrace_t;

struct pmtrace_s
{
	int	allsolid;	      // if true, plane is not valid
	int	startsolid;	      // if true, the initial point was in a solid area
	int	inopen, inwater;  // End point is in empty space or in water
	float		fraction;		  // time completed, 1.0 = didn't hit anything
	s_vec3		endpos;			  // final position
	pmplane_t	plane;		      // surface normal at impact
	int			ent;			  // entity at impact
	s_vec3      deltavelocity;    // Change in player's velocity caused by impact.  
								  // Only run on server.
	int         hitgroup;
};

typedef struct movevars_s
{
	float gravity;			// Gravity for map
	float stopspeed;		// Deceleration when not moving
	float maxspeed;			// Max allowed speed
	float spectatormaxspeed;
	float accelerate;		// Acceleration factor
	float airaccelerate;		// Same for when in open air
	float wateraccelerate;		// Same for when in water
	float friction;
	float edgefriction;		// Extra friction near dropofs
	float waterfriction;		// Less in water
	float entgravity;		// 1.0
	float bounce;			// Wall bounce value. 1.0
	float stepsize;			// sv_stepsize;
	float maxvelocity;		// maximum server velocity.
	float zmax;			// Max z-buffer range (for GL)
	float waveHeight;		// Water wave height (for GL)
	int footsteps;		// Play footstep sounds
	char skyName[32];		// Name of the sky map
	float rollangle;
	float rollspeed;
	float skycolor_r;		// Sky color
	float skycolor_g;
	float skycolor_b;
	float skyvec_x;			// Sky s_vec3
	float skyvec_y;
	float skyvec_z;

} movevars_t;

typedef struct physent_s
{
	char name[32];					// Name of model, or "player" or "world".
	int player;
	s_vec3 origin;					// Model's origin in world coordinates.
	struct model_s* model;				// only for bsp models
	struct model_s* studiomodel;			// SOLID_BBOX, but studio clip intersections.
	s_vec3 mins, maxs;				// only for non-bsp models
	int info;					// For client or server to use to identify (index into edicts or cl_entities)
	s_angle angles;					// rotated entities need this info for hull testing to work.

	int solid;					// Triggers and func_door type WATER brushes are SOLID_NOT
	int skin;					// BSP Contents for such things like fun_door water brushes.
	int rendermode;					// So we can ignore glass

	float frame;
	int sequence;
	byte controller[4];
	byte blending[2];

	int movetype;
	int takedamage;
	int blooddecal;
	int team;
	int classnumber;

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

} physent_t;

typedef struct playermove_s
{
	int player_index;				// So we don't try to run the PM_CheckStuck nudging too quickly.
	int server;				// For debugging, are we running physics code on server side?
	int multiplayer;				// 1 == multiplayer server
	float time;					// realtime on host, for reckoning duck timing
	float frametime;				// Duration of this frame
	s_vec3 forward, right, up;			// s_vec3s for angles
	s_vec3 origin;					// Movement origin.
	s_angle angles;					// Movement view angles.
	s_angle oldangles;				// Angles before movement view angles were looked at.
	s_vec3 velocity;				// Current movement direction.
	s_vec3 movedir;					// For waterjumping, a forced forward velocity so we can fly over lip of ledge.
	s_vec3 basevelocity;				// Velocity of the conveyor we are standing, e.g.
	s_vec3 view_ofs;				// For ducking/dead
							// Our eye position.
	float flDuckTime;				// Time we started duck
	int bInDuck;				// In process of ducking or ducked already?
	int flTimeStepSound;				// For walking/falling
							// Next time we can play a step sound
	int iStepLeft;
	float flFallVelocity;
	s_angle punchangle;
	float flSwimTime;
	float flNextPrimaryAttack;
	int effects;					// MUZZLE FLASH, e.g.
	int flags;					// FL_ONGROUND, FL_DUCKING, etc.
	int usehull;					// 0 = regular player hull, 1 = ducked player hull, 2 = point hull
	float gravity;					// Our current gravity and friction.
	float friction;
	int oldbuttons;					// Buttons last usercmd
	float waterjumptime;				// Amount of time left in jumping out of water cycle.
	int dead;					// Are we a dead player?
	int deadflag;
	int spectator;					// Should we use spectator physics model?
	int movetype;					// Our movement type, NOCLIP, WALK, FLY
	int onground;					// -1 = in air, else pmove entity number
	int waterlevel;
	int watertype;
	int oldwaterlevel;
	char sztexturename[256];
	char chtexturetype;
	float maxspeed;
	float clientmaxspeed;
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
	int numphysent;					// world state
							// Number of entities to clip against.
	physent_t physents[MAX_PHYSENTS];
	int nummoveent;					// Number of momvement entities (ladders)
	physent_t moveents[MAX_MOVEENTS];		// just a list of ladders
	int numvisent;					// All things being rendered, for tracing against things you don't actually collide with
	physent_t visents[MAX_PHYSENTS];
	usercmd_t cmd;					// input to run through physics.
	int numtouch;					// Trace results for objects we collided with.
	pmtrace_t touchindex[MAX_PHYSENTS];
	char physinfo[MAX_PHYSINFO_STRING];		// Physics info string
	struct movevars_s* movevars;
	float player_mins[4][3];
	float player_maxs[4][3];

	const char* (*PM_Info_ValueForKey)(const char* s, const char* key);
	void (*PM_Particle)(float* origin, int color, float life, int zpos, int zvel);
	int (*PM_TestPlayerPosition)(float* pos, pmtrace_t* ptrace);
	void (*Con_NPrintf)(int idx, char* fmt, ...);
	void (*Con_DPrintf)(char* fmt, ...);
	void (*Con_Printf)(char* fmt, ...);
	double (*Sys_FloatTime)();
	void (*PM_StuckTouch)(int hitent, pmtrace_t* ptraceresult);
	int (*PM_PointContents)(float* p, int* truecontents);
	int (*PM_TruePointContents)(float* p);
	int (*PM_HullPointContents)(struct hull_s* hull, int num, float* p);
	pmtrace_t(*PM_PlayerTrace)(float* start, float* end, int traceFlags, int ignore_pe);
	struct pmtrace_s* (*PM_TraceLine)(float* start, float* end, int flags, int usehulll, int ignore_pe);
	int(*RandomLong)(int lLow, int lHigh);
	float (*RandomFloat)(float flLow, float flHigh);
	int (*PM_GetModelType)(struct model_s* mod);
	void (*PM_GetModelBounds)(struct model_s* mod, float* mins, float* maxs);
	void* (*PM_HullForBsp)(physent_t* pe, float* offset);
	float (*PM_TraceModel)(physent_t* pEnt, float* start, float* end, trace_t* trace);
	int (*COM_FileSize)(char* filename);
	byte* (*COM_LoadFile)(char* path, int usehunk, int* pLength);
	void (*COM_FreeFile)(void* buffer);
	char* (*memfgets)(byte* pMemFile, int fileSize, int* pFilePos, char* pBuffer, int bufferSize);
	int runfuncs;
	void (*PM_PlaySound)(int channel, const char* sample, float volume, float attenuation, int fFlags, int pitch);
	const char* (*PM_TraceTexture)(int ground, float* vstart, float* vend);
	void (*PM_PlaybackEventFull)(int flags, int clientindex, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);

	pmtrace_t(*PM_PlayerTraceEx)(float* start, float* end, int traceFlags, int (*pfnIgnore)(physent_t* pe));
	int (*PM_TestPlayerPositionEx)(float* pos, pmtrace_t* ptrace, int (*pfnIgnore)(physent_t* pe));
	struct pmtrace_s* (*PM_TraceLineEx)(float* start, float* end, int flags, int usehulll, int (*pfnIgnore)(physent_t* pe));

} playermove_t;