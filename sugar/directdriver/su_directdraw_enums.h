/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2008 AtOnce Technologies
**
** This file is part of the Sugar Framework.
**
** This file and the associated product may be used and distributed
** under the terms of a current License as defined by AtOnce Technologies
** and appearing in the file LICENSE.TXT included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.atonce-technologies.com or email info@atonce-technologies.com
** for informations about Sugar Framework and AtOnce Technologies others products
** License Agreements.
**
** Contact info@atonce-technologies.com if any conditions of this license
** are not clear to you.
**
*****************************************************************LIC-HDR*/



#ifndef _su_ddraw_enums_h_
#define _su_ddraw_enums_h_




namespace su { namespace dd {


enum
{
	INVALID_IDX				= 0,
	NB_CONSTANT				= 256,
};





enum DynUsage
{
	DUSAGE_STATIC			= 0,
	DUSAGE_DYNAMIC			= 1,
};

struct CameraDescriptor
{
	void*	data;
	Psm		psm;
	uint	width;
	uint	height;
	byte	origin;			// 0: bottom-left, 1: bottom-right, 2: top-left, 3: top-right
	bool	yFlipped;
};

enum PixelUsage
{
	PUSAGE_LOSSLESS_RGBA	= 0,
	PUSAGE_LOSSLESS_RGB		= 1,
	PUSAGE_LUMINANCE		= 5,
};


enum CameraMode
{
	CAM_OFF					= 0,
	CAM_PREPARE				= 1,
	CAM_GRAB				= 2,
};


enum SwitchMode
{
	SW_OFF					= 0,
	SW_ON					= 1,
};


enum CompUsage
{
	CUSAGE_NONE				= 0,
	CUSAGE_POS				= 1,
	CUSAGE_BLENDW			= 2,
	CUSAGE_BLENDI			= 3,
	CUSAGE_NORMAL			= 4,
	CUSAGE_PSIZE			= 5,
	CUSAGE_TEXCOORD			= 6,
	CUSAGE_TANGENT			= 7,
	CUSAGE_BINORMAL			= 8,
	CUSAGE_TESSFACTOR		= 9,
	CUSAGE_POSITIONT		= 10,
	CUSAGE_COLOR			= 11,
	CUSAGE_FOG				= 12,
	CUSAGE_DEPTH			= 13,
	CUSAGE_SAMPLE			= 14,
};

enum VertexFormat
{
	VF_LOC2		= (1<<0),
	VF_LOC3		= (1<<1),
	VF_UV0		= (1<<2),
	VF_UV1		= (1<<3),
	VF_UV2		= (1<<4),
	VF_COL		= (1<<5),
	VF_NRM		= (1<<6),
	VF_TAN		= (1<<7),
	VF_BIN		= (1<<8),
	VF_KICK		= (1<<9)
};


enum DefaultProgram
{
							//							Constantes				Vertex format needed		Tex stage used		Final Color
							// For all program :		Matrix "g_wvp"			VF_LOC2 or VF_LOC3

	DP_Position							= 1,		//	------------			------------				------------		a gray color
	DP_VertexColor						= 2,		//	------------			VF_COL						------------		VF_COL
	DP_Color							= 3,		//	Vec4  "color"			------------				------------		"color"
	DP_Texture							= 4,		//	------------			VF_UV0						s0					(s0,UV0)
	
	DP_MultiTex_Additive				= 5,		//	------------			VF_UV0 | VF_UV1				s0, s1				(s0,uv0) + (s1,uv1)
	DP_MultiTex_Multiplicative			= 6,		//	------------			VF_UV0 | VF_UV1				s0, s1				(s0,uv0) * (s1,uv1)
	DP_MultiTex_Additive_OnUV0			= 7,		//	------------			VF_UV0						s0, s1				(s0,uv0) + (s1,uv0)
	DP_MultiTex_Multiplicative_OnUV0	= 8,		//	------------			VF_UV0						s0, s1				(s0,uv0) + (s1,uv0)
	
	DP_TextureSetAlpha					= 9,		//	float "alpha"			VF_UV0						s0					((s0,uv0).xyz, alpha)
	DP_TextureScale						= 10,		//	float "scale"			VF_UV0						s0					(s0,uv0) * scale
	DP_TextureSubstractColor			= 11,		//	Vec4  "color"			VF_UV0						s0					(s0,uv0) - color
	DP_VertexColorSetAlpha				= 12,		//	float "alpha"			VF_COL						------------		( VF_COL.xyz, alpha )
	
	DP_Outlining						= 13,		//	Vec4  "color"			VF_UV0						s0, s1				outline s0 when (s1,uv0) == color
	DP_AlphaThreshold					= 14		//	float "alphaTH",		VF_UV0						s0					
													//	Vec4  "color"
};


enum CstClass
{
	CCLASS_SCALAR,
	CCLASS_VECTOR,
	CCLASS_MATRIX,
	CCLASS_OBJECT
};

enum CstType
{
	CTYPE_VOID,
	CTYPE_BOOL,
	CTYPE_INT,
	CTYPE_FLOAT,
	CTYPE_SAMPLER
};


enum CompStorage
{
	CSM_FLOAT1				=  0,  // 1D float expanded to (value, 0., 0., 1.)
	CSM_FLOAT2				=  1,  // 2D float expanded to (value, value, 0., 1.)
	CSM_FLOAT3				=  2,  // 3D float expanded to (value, value, value, 1.)
	CSM_FLOAT4				=  3,  // 4D float
	CSM_COLOR				=  4,  // 4D packed unsigned bytes (ARGB) mapped to 0. to 1. range (R, G, B, A)
	CSM_UBYTE4				=  5,  // 4D unsigned byte
	CSM_SHORT2				=  6,  // 2D signed short expanded to (value, value, 0., 1.)
	CSM_SHORT4				=  7,  // 4D signed short
	CSM_UBYTE4N				=  8,  // Each of 4 bytes is normalized by dividing to 255.0
	CSM_SHORT2N				=  9,  // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
	CSM_SHORT4N				= 10,  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
	CSM_USHORT2N			= 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
	CSM_USHORT4N			= 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
	CSM_FLOAT16_2			= 13,  // Two 16-bit floating point values, expanded to (value, value, 0, 1)
	CSM_FLOAT16_4			= 14,  // Four 16-bit floating point values
};



enum PrimType
{
	PRIM_POINT		= 0,
	PRIM_LINE		= 1,
	PRIM_TRIANGLE	= 2
};


enum BlendMode
{
	// Sc	: source color
	// Sa	: source alpha
	// Dc	: dest color
	// Da	: dest alpha
	
	// Operation Without alpha
	BM_Sc_x_One_add_Dc_x_Zero		= 0,
	BM_Sc_x_One_add_Dc_x_One		= 1,
	BM_Sc_x_One_sub_Dc_x_One		= 2,
	
	// With src alpha
	BM_Sc_x_Sa_add_Dc_x_ReverseSa		= 3,
	BM_Sc_x_Sa_add_Dc_x_One				= 4,
	BM_Sc_x_ReverseSa_add_Dc_x_Sa		= 5,
	BM_Sc_x_Sa_add_Dc_x_Zero			= 6,
	
	// With dst alpha
	BM_Sc_x_ReverseDa_add_Dc_x_Da		= 7,
	BM_Sc_x_Da_add_Dc_x_ReverseDa		= 8,
	BM_Sc_x_ReverseDa_add_Dc_x_One		= 9,
	
	
	BM_Disabled				= BM_Sc_x_One_add_Dc_x_Zero,
	BM_Additive				= BM_Sc_x_One_add_Dc_x_One,
	BM_Substractive			= BM_Sc_x_One_sub_Dc_x_One,
	BM_StdAphaBlend			= BM_Sc_x_Sa_add_Dc_x_ReverseSa
};

enum CullMode
{
	CM_DISABLE	= 0,
	CM_FRONT	= 1,
	CM_BACK		= 2
};

enum CompFunc
{
	CF_UNDEF	= 0,		// Don't make change
	CF_NEVER	= 1,
	CF_LESS		= 2,
	CF_EQUAL	= 3,
	CF_LEQUAL	= 4,
	CF_GREATER	= 5,
	CF_NOTEQUAL	= 6,
	CF_GEQUAL	= 7,
	CF_ALWAYS	= 8
};

enum TileGroupDirective
{
	TGD_Dirty	= 1,
	TGD_Reserve	= 2,
	TGD_Reset	= 3,
    TGD_Locate  = 4,
};



struct Vertex_L3
{
	static const uint fvf = VF_LOC3;

	Vec3 loc;
};

struct Vertex_L3_K
{
	static const uint fvf = VF_LOC3 | VF_KICK;

	Vec3 loc;
	bool kick;
};

struct Vertex_L3_COL
{
	static const uint fvf = VF_LOC3 | VF_COL;

	Vec3 loc;
	uint c;
};

struct Vertex_L3_COL_K
{
	static const uint fvf = VF_LOC3 | VF_COL | VF_KICK;

	Vec3 loc;
	uint c;
	bool kick;
};

struct Vertex_L3_UV0
{
	static const uint fvf = VF_LOC3 | VF_UV0;

	Vec3 loc;
	Vec2 uv0;
};

struct Vertex_L3_UV0_K
{
	static const uint fvf = VF_LOC3 | VF_UV0 | VF_KICK;

	Vec3 loc;
	Vec2 uv0;
	bool kick;
};

struct Vertex_L3_UV0_UV1
{
	static const uint fvf = VF_LOC3 | VF_UV0 | VF_UV1;

	Vec3 loc;
	Vec2 uv0, uv1;
};

struct Vertex_L3_UV0_UV1_K
{
	static const uint fvf = VF_LOC3 | VF_UV0 | VF_UV1 | VF_KICK;

	Vec3 loc;
	Vec2 uv0, uv1;
	bool kick;
};



} }


#endif // _su_ddraw_enums_h_



