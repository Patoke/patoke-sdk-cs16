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

typedef struct studio_model_renderer_s
{
	// Construction/Destruction
	void				(*CStudioModelRenderer)			(void);

	// Initialization
	void				(*Init)							(void);

	// Public Interfaces
	int					(*StudioDrawModel)				(int flags);
	int					(*StudioDrawPlayer)				(int flags, entity_state_s* pplayer);

	// Look up animation data for sequence
	mstudioanim_t* (*StudioGetAnim)				(model_t* m_pSubModel, mstudioseqdesc_t* pseqdesc);

	// Interpolate model position and angles and set up matrices
	void				(*StudioSetUpTransform)			(int trivial_accept);

	// Set up model bone positions
	void				(*StudioSetupBones)				(void);

	// Find final attachment points
	void				(*StudioCalcAttachments)		(void);

	// Save bone matrices and names
	void				(*StudioSaveBones)				(void);

	// Merge cached bones with current bones for model
	void				(*StudioMergeBones)				(model_t* m_pSubModel);

	// Determine interpolation fraction
	float				(*StudioEstimateInterpolant)	(void);

	// Determine current frame for rendering
	float				(*StudioEstimateFrame)			(mstudioseqdesc_t* pseqdesc);

	// Apply special effects to transform matrix
	void				(*StudioFxTransform)			(cl_entity_t* ent, float transform[3][4]);

	// Spherical interpolation of bones
	void				(*StudioSlerpBones)				(s_rect q1[], float pos1[][3], s_rect q2[], float pos2[][3], float s);

	// Compute bone adjustments(bone controllers)
	void				(*StudioCalcBoneAdj)			(float dadt, float* adj, const byte* pcontroller1, const byte* pcontroller2, byte mouthopen);

	// Get bone quaternions
	void				(*StudioCalcBoneQuaterion)		(int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* q);

	// Get bone positions
	void				(*StudioCalcBonePosition)		(int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* pos);

	// Compute rotations
	void				(*StudioCalcRotations)			(float pos[][3], s_rect* q, mstudioseqdesc_t* pseqdesc, mstudioanim_t* panim, float f);

	// Send bones and verts to renderer
	void				(*StudioRenderModel)			(void);

	// Finalize rendering
	void				(*StudioRenderFinal)			(void);

	// GL&D3D vs. Software renderer finishing functions
	void				(*StudioRenderFinal_Software)	(void);
	void				(*StudioRenderFinal_Hardware)	(void);

	// Player specific data
	// Determine pitch and blending amounts for players
	void				(*StudioPlayerBlend)			(mstudioseqdesc_t* pseqdesc, int* pBlend, float* pPitch);

	// Estimate gait frame for player
	void				(*StudioEstimateGait)			(entity_state_t* pplayer);

	// Process movement of player
	void				(*StudioProcessGait)			(entity_state_t* pplayer);

	int					(*_StudioDrawPlayer)			(int flags, entity_state_s* pplayer);

	int					(*CalculateYawBlend)			(entity_state_s* pplayer);

	void				(*CalculatePitchBlend)			(entity_state_t* pplayer);
} studio_model_renderer_t;