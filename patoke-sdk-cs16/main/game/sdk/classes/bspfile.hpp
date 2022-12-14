#pragma once

#define BSPVERSION			30
#define MAX_MAP_HULLS			4

#define CONTENTS_ORIGIN			-7		// removed at csg time
#define CONTENTS_CLIP			-8		// changed to contents_solid
#define CONTENTS_CURRENT_0		-9
#define CONTENTS_CURRENT_90		-10
#define CONTENTS_CURRENT_180		-11
#define CONTENTS_CURRENT_270		-12
#define CONTENTS_CURRENT_UP		-13
#define CONTENTS_CURRENT_DOWN		-14

#define CONTENTS_TRANSLUCENT		-15

#define LUMP_ENTITIES			0
#define LUMP_PLANES			1
#define LUMP_TEXTURES			2
#define LUMP_VERTEXES			3
#define LUMP_VISIBILITY			4
#define LUMP_NODES			5
#define LUMP_TEXINFO			6
#define LUMP_FACES			7
#define LUMP_LIGHTING			8
#define LUMP_CLIPNODES			9
#define LUMP_LEAFS			10
#define LUMP_MARKSURFACES		11
#define LUMP_EDGES			12
#define LUMP_SURFEDGES			13
#define LUMP_MODELS			14

#define HEADER_LUMPS			15

/* <a1fc> ../engine/bspfile.h:41 */
typedef struct lump_s
{
	int fileofs;
	int filelen;

} lump_t;

/* <a22c> ../engine/bspfile.h:64 */
typedef struct dmodel_s
{
	float mins[3], maxs[3];
	float origin[3];
	int headnode[MAX_MAP_HULLS];
	int visleafs;			// not including the solid leaf 0
	int firstface, numfaces;

} dmodel_t;

/* <a2c2> ../engine/bspfile.h:73 */
typedef struct dheader_s
{
	int version;
	lump_t lumps[15];

} dheader_t;

/* <485b2> ../engine/bspfile.h:79 */
typedef struct dmiptexlump_s
{
	int _nummiptex;
	int dataofs[4];

} dmiptexlump_t;

/* <1ce18> ../engine/bspfile.h:86 */
typedef struct miptex_s
{
	char name[16];
	unsigned width;
	unsigned height;
	unsigned offsets[4];

} miptex_t;

/* <48652> ../engine/bspfile.h:94 */
typedef struct dvertex_s
{
	float point[3];

} dvertex_t;

/* <48674> ../engine/bspfile.h:110 */
typedef struct dplane_s
{
	float normal[3];
	float dist;
	int type;

} dplane_t;

/* <486b2> ../engine/bspfile.h:132 */
typedef struct dnode_s
{
	int planenum;
	short children[2];
	short mins[3];
	short maxs[3];
	unsigned short firstface;
	unsigned short numfaces;

} dnode_t;

/* <a332> ../engine/bspfile.h:142 */
typedef struct dclipnode_s
{
	int planenum;
	short children[2];	// negative numbers are contents

} dclipnode_t;

/* <4876a> ../engine/bspfile.h:149 */
typedef struct texinfo_s
{
	float vecs[2][4];
	int _miptex;
	int flags;

} texinfo_t;

/* <487c2> ../engine/bspfile.h:159 */
typedef struct dedge_s
{
	unsigned short v[2];

} dedge_t;

/* <487f2> ../engine/bspfile.h:165 */
typedef struct dface_s
{
	short planenum;
	short side;
	int firstedge;
	short numedges;
	short texinfo;
	byte styles[4];
	int lightofs;

} dface_t;

typedef struct dleaf_s
{
	int contents;
	int visofs;
	short mins[3];
	short maxs[3];
	unsigned short firstmarksurface;
	unsigned short nummarksurfaces;
	byte ambient_level[4];

} dleaf_t;