#pragma once

#define MAXSTUDIOTRIANGLES	20000	// TODO: tune this
#define MAXSTUDIOVERTS		2048	// TODO: tune this
#define MAXSTUDIOSEQUENCES	2048	// total animation sequences
#define MAXSTUDIOSKINS		100		// total textures
#define MAXSTUDIOSRCBONES	512		// bones allowed at source movement
#define MAXSTUDIOBONES		128		// total bones actually used
#define MAXSTUDIOMODELS		32		// sub-models per model
#define MAXSTUDIOBODYPARTS	32
#define MAXSTUDIOGROUPS		16
#define MAXSTUDIOANIMATIONS	2048	// per sequence
#define MAXSTUDIOMESHES		256
#define MAXSTUDIOEVENTS		1024
#define MAXSTUDIOPIVOTS		256
#define MAXSTUDIOCONTROLLERS 8

#define STUDIO_DYNAMIC_LIGHT		0x0100	// dynamically get lighting from floor or ceil (flying monsters)
#define STUDIO_TRACE_HITBOX		0x0200	// always use hitbox trace instead of bbox

// lighting options
#define STUDIO_NF_FLATSHADE		0x0001
#define STUDIO_NF_CHROME		0x0002
#define STUDIO_NF_FULLBRIGHT		0x0004
#define STUDIO_NF_NOMIPS		0x0008
#define STUDIO_NF_ALPHA			0x0010
#define STUDIO_NF_ADDITIVE		0x0020
#define STUDIO_NF_MASKED		0x0040

// motion flags
#define STUDIO_X		0x0001
#define STUDIO_Y		0x0002
#define STUDIO_Z		0x0004
#define STUDIO_XR		0x0008
#define STUDIO_YR		0x0010
#define STUDIO_ZR		0x0020
#define STUDIO_LX		0x0040
#define STUDIO_LY		0x0080
#define STUDIO_LZ		0x0100
#define STUDIO_AX		0x0200
#define STUDIO_AY		0x0400
#define STUDIO_AZ		0x0800
#define STUDIO_AXR		0x1000
#define STUDIO_AYR		0x2000
#define STUDIO_AZR		0x4000
#define STUDIO_TYPES	0x7FFF
#define STUDIO_RLOOP	0x8000	// controller that wraps shortest distance

// sequence flags
#define STUDIO_LOOPING	0x0001

// bone flags
#define STUDIO_HAS_NORMALS	0x0001
#define STUDIO_HAS_VERTICES 0x0002
#define STUDIO_HAS_BBOX		0x0004
#define STUDIO_HAS_CHROME	0x0008	// if any of the textures have chrome on them

#define RAD_TO_STUDIO		(32768.0/M_PI)
#define STUDIO_TO_RAD		(M_PI/32768.0)


#define STUDIO_NUM_HULLS 128
#define STUDIO_NUM_PLANES (STUDIO_NUM_HULLS * 6)
#define STUDIO_CACHE_SIZE 16

typedef struct
{
	int					id;
	int					version;

	char				name[64];
	int					length;

	s_vec3				eyeposition;	// ideal eye position
	s_vec3				min;			// ideal movement hull size
	s_vec3				max;

	s_vec3				bbmin;			// clipping bounding box
	s_vec3				bbmax;

	int					flags;

	int					numbones;			// bones
	int					boneindex;

	int					numbonecontrollers;		// bone controllers
	int					bonecontrollerindex;

	int					numhitboxes;			// complex bounding boxes
	int					hitboxindex;

	int					numseq;				// animation sequences
	int					seqindex;

	int					numseqgroups;		// demand loaded sequences
	int					seqgroupindex;

	int					numtextures;		// raw textures
	int					textureindex;
	int					texturedataindex;

	int					numskinref;			// replaceable textures
	int					numskinfamilies;
	int					skinindex;

	int					numbodyparts;
	int					bodypartindex;

	int					numattachments;		// queryable attachable points
	int					attachmentindex;

	int					soundtable;
	int					soundindex;
	int					soundgroups;
	int					soundgroupindex;

	int					numtransitions;		// animation node to animation node transition graph
	int					transitionindex;
} studiohdr_t;

// header for demand loaded sequence group data
typedef struct
{
	int					id;
	int					version;

	char				name[64];
	int					length;
} studioseqhdr_t;

// bones
typedef struct
{
	char				name[32];	// bone name for symbolic links
	int		 			parent;		// parent bone
	int					flags;		// ??
	int					bonecontroller[6];	// bone controller index, -1 == none
	float				value[6];	// default DoF values
	float				scale[6];   // scale for delta DoF values
} mstudiobone_t;


// bone controllers
typedef struct
{
	int					bone;	// -1 == 0
	int					type;	// X, Y, Z, XR, YR, ZR, M
	float				start;
	float				end;
	int					rest;	// byte index value at rest
	int					index;	// 0-3 user set controller, 4 mouth
} mstudiobonecontroller_t;

// intersection boxes
typedef struct
{
	int					bone;
	int					group;			// intersection group
	s_vec3				bbmin;		// bounding box
	s_vec3				bbmax;
} mstudiobbox_t;

// demand loaded sequence groups
typedef struct
{
	char				label[32];	// textual name
	char				name[64];	// file name
	int32_t				unused1;    // was "cache"  - index pointer
	int					unused2;    // was "data" -  hack for group 0
} mstudioseqgroup_t;

// sequence descriptions
typedef struct
{
	char				label[32];	// sequence label

	float				fps;		// frames per second	
	int					flags;		// looping/non-looping flags

	int					activity;
	int					actweight;

	int					numevents;
	int					eventindex;

	int					numframes;	// number of frames per sequence

	int					numpivots;	// number of foot pivots
	int					pivotindex;

	int					motiontype;
	int					motionbone;
	s_vec3				linearmovement;
	int					automoveposindex;
	int					automoveangleindex;

	s_vec3				bbmin;		// per sequence bounding box
	s_vec3				bbmax;

	int					numblends;
	int					animindex;		// mstudioanim_t pointer relative to start of sequence group data
	// [blend][bone][X, Y, Z, XR, YR, ZR]

	int					blendtype[2];	// X, Y, Z, XR, YR, ZR
	float				blendstart[2];	// starting value
	float				blendend[2];	// ending value
	int					blendparent;

	int					seqgroup;		// sequence group for demand loading

	int					entrynode;		// transition node at entry
	int					exitnode;		// transition node at exit
	int					nodeflags;		// transition rules

	int					nextseq;		// auto advancing sequences
} mstudioseqdesc_t;

// events
//#include "studio_event.h"
/*
typedef struct
{
	int 				frame;
	int					event;
	int					type;
	char				options[64];
} mstudioevent_t;
*/

// pivots
typedef struct
{
	s_vec3				org;	// pivot point
	int					start;
	int					end;
} mstudiopivot_t;

// attachment
typedef struct
{
	char				name[32];
	int					type;
	int					bone;
	s_vec3				org;	// attachment point
	s_vec3				s_vec3s[3];
} mstudioattachment_t;

typedef struct
{
	unsigned short	offset[6];
} mstudioanim_t;

// animation frames
typedef union
{
	struct {
		byte	valid;
		byte	total;
	} num;
	short		value;
} mstudioanimvalue_t;



// body part index
typedef struct
{
	char				name[64];
	int					nummodels;
	int					base;
	int					modelindex; // index into models array
} mstudiobodyparts_t;



// skin info
typedef struct
{
	char					name[64];
	int						flags;
	int						width;
	int						height;
	int						index;
} mstudiotexture_t;


// skin families
// short	index[skinfamilies][skinref]

// studio models
typedef struct
{
	char				name[64];

	int					type;

	float				boundingradius;

	int					nummesh;
	int					meshindex;

	int					numverts;		// number of unique vertices
	int					vertinfoindex;	// vertex bone info
	int					vertindex;		// vertex vec3_t
	int					numnorms;		// number of unique surface normals
	int					norminfoindex;	// normal bone info
	int					normindex;		// normal vec3_t

	int					numgroups;		// deformation groups
	int					groupindex;
} mstudiomodel_t;


// vec3_t	boundingbox[model][bone][2];	// complex intersection info


// meshes
typedef struct
{
	int					numtris;
	int					triindex;
	int					skinref;
	int					numnorms;		// per mesh normals
	int					normindex;		// normal vec3_t
} mstudiomesh_t;

// triangles
#if 0
typedef struct
{
	short				vertindex;		// index into vertex array
	short				normindex;		// index into normal array
	short				s, t;			// s,t position on skin
} mstudiotrivert_t;
#endif

class c_studiomodelrenderer {
public:
	// Construction/Destruction
	c_studiomodelrenderer(void);
	virtual ~c_studiomodelrenderer(void) = 0;

	// Initialization
	virtual void Init(void) = 0;

public:
	// Public Interfaces
	virtual int StudioDrawModel(int flags) = 0;
	virtual int StudioDrawPlayer(int flags, struct entity_state_s* pplayer) = 0;

public:
	// Local interfaces
	//

	// Look up animation data for sequence
	virtual mstudioanim_t* StudioGetAnim(model_t* m_pSubModel, mstudioseqdesc_t* pseqdesc) = 0;

	// Interpolate model position and angles and set up matrices
	virtual void StudioSetUpTransform(int trivial_accept) = 0;

	// Set up model bone positions
	virtual void StudioSetupBones(void) = 0;

	// Find final attachment points
	virtual void StudioCalcAttachments(void) = 0;

	// Save bone matrices and names
	virtual void StudioSaveBones(void) = 0;

	// Merge cached bones with current bones for model
	virtual void StudioMergeBones(model_t* m_pSubModel) = 0;

	// Determine interpolation fraction
	virtual float StudioEstimateInterpolant(void) = 0;

	// Determine current frame for rendering
	virtual float StudioEstimateFrame(mstudioseqdesc_t* pseqdesc) = 0;

	// Apply special effects to transform matrix
	virtual void StudioFxTransform(cl_entity_t* ent, float transform[3][4]) = 0;

	// Spherical interpolation of bones
	virtual void StudioSlerpBones(s_rect q1[], float pos1[][3], s_rect q2[], float pos2[][3], float s) = 0;

	// Compute bone adjustments ( bone controllers )
	virtual void StudioCalcBoneAdj(float dadt, float* adj, const byte* pcontroller1, const byte* pcontroller2, byte mouthopen) = 0;

	// Get bone quaternions
	virtual void StudioCalcBoneQuaterion(int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* q) = 0;

	// Get bone positions
	virtual void StudioCalcBonePosition(int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* pos) = 0;

	// Compute rotations
	virtual void StudioCalcRotations(float pos[][3], s_rect* q, mstudioseqdesc_t* pseqdesc, mstudioanim_t* panim, float f) = 0;

	// Send bones and verts to renderer
	virtual void StudioRenderModel(void) = 0;

	// Finalize rendering
	virtual void StudioRenderFinal(void) = 0;

	// GL&D3D vs. Software renderer finishing functions
	virtual void StudioRenderFinal_Software(void) = 0;
	virtual void StudioRenderFinal_Hardware(void) = 0;

	// Player specific data
	// Determine pitch and blending amounts for players
	virtual void StudioPlayerBlend(mstudioseqdesc_t* pseqdesc, int* pBlend, float* pPitch) = 0;

	// Estimate gait frame for player
	virtual void StudioEstimateGait(entity_state_t* pplayer) = 0;

	// Process movement of player
	virtual void StudioProcessGait(entity_state_t* pplayer) = 0;

public:

	// Client clock
	double			m_clTime;
	// Old Client clock
	double			m_clOldTime;

	// Do interpolation?
	int				m_fDoInterp;
	// Do gait estimation?
	int				m_fGaitEstimation;

	// Current render frame #
	int				m_nFrameCount;

	// Cvars that studio model code needs to reference
	//
	// Use high quality models?
	cvar_t* m_pCvarHiModels;
	// Developer debug output desired?
	cvar_t* m_pCvarDeveloper;
	// Draw entities bone hit boxes, etc?
	cvar_t* m_pCvarDrawEntities;

	// The entity which we are currently rendering.
	cl_entity_t* m_pCurrentEntity;

	// The model for the entity being rendered
	model_t* m_pRenderModel;

	// Player info for current player, if drawing a player
	player_info_t* m_pPlayerInfo;

	// The index of the player being drawn
	int				m_nPlayerIndex;

	// The player's gait movement
	float			m_flGaitMovement;

	// Pointer to header block for studio model data
	studiohdr_t* m_pStudioHeader;

	// Pointers to current body part and submodel
	mstudiobodyparts_t* m_pBodyPart;
	mstudiomodel_t* m_pSubModel;

	// Palette substition for top and bottom of model
	int				m_nTopColor;
	int				m_nBottomColor;

	//
	// Sprite model used for drawing studio model chrome
	model_t* m_pChromeSprite;

	// Caching
	// Number of bones in bone cache
	int				m_nCachedBones;
	// Names of cached bones
	char			m_nCachedBoneNames[MAXSTUDIOBONES][32];
	// Cached bone & light transformation matrices
	float			m_rgCachedBoneTransform[MAXSTUDIOBONES][3][4];
	float			m_rgCachedLightTransform[MAXSTUDIOBONES][3][4];

	// Software renderer scale factors
	float			m_fSoftwareXScale, m_fSoftwareYScale;

	// Current view vectors and render origin
	float			m_vUp[3];
	float			m_vRight[3];
	float			m_vNormal[3];

	float			m_vRenderOrigin[3];

	// Model render counters ( from engine )
	int* m_pStudioModelCount;
	int* m_pModelsDrawn;

	// Matrices
	// Model to world transformation
	float(*m_protationmatrix)[3][4];
	// Model to view transformation
	float(*m_paliastransform)[3][4];

	// Concatenated bone and light transforms
	float(*m_pbonetransform)[MAXSTUDIOBONES][3][4];
	float(*m_plighttransform)[MAXSTUDIOBONES][3][4];
};