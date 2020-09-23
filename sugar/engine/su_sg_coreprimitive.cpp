/*LIC-HDR********************************************************************
**
** Copyright (C) 2005-2011 AtOnce Technologies
**
** This file is part of the Sugar core-system framework.
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

#include <sufw/su_core.h>
#include <sufw/directdriver/su_directdriver.h>

#include "su_collection.h"

#include "su_sg_core.h"
#include "su_sg_coreprimitive.h"
#include "su_sg_handle.h"
#include "su_sg_scenegraph.h"



using namespace su;
using namespace su::sg;


namespace
{

	uint	create_surface	(	void*			inVtxData,
								uint			inVtxCount,
								uint			inVtxFmt,
								uint			inProgId,
								dd::PrimType	inPType,
								IDirectDraw*	inDD			)
	{
		int vbo = dd::INVALID_IDX;
		uint cmdVbo[] = 
		{
			SU_DD_VBOCREATE_1( inVtxFmt, inPType, 0 ),
			SU_DD_VBOCREATE_2( inVtxCount ),
			SU_DD_VBOCREATE_3( &vbo ),
			SU_DD_END()
		};

		inDD->StartAndWaitCmdList( cmdVbo );
		SU_REQUIRE_OR_FALSE( vbo != dd::INVALID_IDX );

		int sf = dd::INVALID_IDX;
		
		uint cmdSrf[] = 
		{
			SU_DD_VBOFILL_1( vbo ),
			SU_DD_VBOFILL_2( inVtxFmt ),
			SU_DD_VBOFILL_3( inVtxData ),

			SU_DD_SURFACE_1( 1, inProgId ),
			SU_DD_SURFACE_2( &sf ),
			SU_DD_SURFACE_n( vbo, 0 ),

			SU_DD_END()
		};

		inDD->StartAndWaitCmdList( cmdSrf );
		inDD->ResourceRelease( vbo );

		return sf;
	}

	uint	create_sphere	(	float			inRad,
								uint			inRes,
								const Color32&	inTopCol,
								const Color32&	inBotCol,
								Pool*			inPool,
								IDirectDraw*	inDD			)
	{
		SU_ASSERT( inDD );
		
		const float r = inRad;
		const int res = inRes;

		const int vtxCount = (res-2) * res + 2;
		const int triCount = (res-1) * res * 2;
		const int idxCount = triCount * 3;

		void* mem0 = inPool->Alloc( vtxCount * sizeof( dd::Vertex_L3_COL ) );
		dd::Vertex_L3_COL* vtxA = (dd::Vertex_L3_COL*)mem0;

		const float yThresold = r*0.15f;
		float incA = Pi / res;
		float incB = 2.0f * Pi / res;
		uint topCol = inDD->GetPackedColor( inTopCol.r, inTopCol.g, inTopCol.b );
		uint botCol = inDD->GetPackedColor( inBotCol.r, inBotCol.g, inBotCol.b );

		
		Vec3 tc3( inTopCol.r, inTopCol.g, inTopCol.b );
		Vec3 bc3( inBotCol.r, inBotCol.g, inBotCol.b );
		
		float o255 = 1.0f/255.0f;
		tc3 *= o255;
		bc3 *= o255;

		vtxA[0].loc = Vec3( 0, r, 0 );
		vtxA[0].c = topCol;
		int ci = 1;
		for( uint j=1; j<res-1; j++ )
		{
			float a = Pi*0.5f - j*incA;
			float sinA = Sin( a );
			float cosA = Cos( a );
		
			float co = ( sinA + 1.0f ) * 0.5f;
			Vec3 cc = bc3 + ( co * ( tc3 - bc3 ) );
			uint col = inDD->GetPackedColor( cc.x, cc.y, cc.z );

			float y = sinA * r;
			for( uint i=0; i<res; i++ )
			{
				float b = i*incB;
				float x = cosA * Cos( b ) * r;
				float z = cosA * Sin( b ) * r;

				vtxA[ci].loc = Vec3( x, y, z );
				vtxA[ci].c = col;
				ci++;
			}
		}
		vtxA[ci].loc = Vec3( 0, -r, 0 );
		vtxA[ci].c = botCol;
		ci++;
		SU_ASSERT( ci == vtxCount );

		ci = 0;
		const uint quadCount = idxCount/6;
		const uint vtxStripCount = quadCount*4;

		
		void* mem1 = inPool->Alloc( vtxStripCount * sizeof( dd::Vertex_L3_COL_K ) );
		dd::Vertex_L3_COL_K* vtxStrA = (dd::Vertex_L3_COL_K*)mem1;

		for( uint j=0; j<res-1; j++ )
		{
			for( uint i=0; i<res; i++ )
			{
				short tplA[4];
				if( j==0 )
				{
					tplA[1] = i+1;
					tplA[0] = 0;
					tplA[3] = ( i == res-1 ) ? 1 : i+2;
					tplA[2] = 0;
				}
				else
				{
					short incNextX = ( i == res-1 ) ? -(res-1): 1;
					short i0 = 1 + ((j-1) * res) + i;
					tplA[1] = ( j == res-2 ) ? vtxCount-1 : i0+res;
					tplA[0] = i0;
					tplA[3] = ( j == res-2 ) ? vtxCount-1 : i0+res+incNextX;
					tplA[2] = i0+incNextX;
				}

				// i0, i1, i3, i2;
				for( uint k=0; k<4; k++ )
				{
					vtxStrA[ci+k].loc	= vtxA[tplA[k]].loc;
					vtxStrA[ci+k].c		= vtxA[tplA[k]].c;
					vtxStrA[ci+k].kick	= (k > 1);
				}
				ci += 4;
			}
		}
		SU_ASSERT( ci == vtxStripCount );
	
		uint sf = create_surface(	vtxStrA,
									vtxStripCount,
									dd::Vertex_L3_COL_K::fvf,
									dd::DP_VertexColor,
									dd::PRIM_TRIANGLE,
									inDD					);

		inPool->Free( mem0 );
		inPool->Free( mem1 );
		return sf;
	}



	uint	create_plan		(	float			inWidth,
								float			inLength,
								const Color32&	inCol,
								IDirectDraw*	inDD			)
	{
		SU_ASSERT( inDD );

		const float hw = inWidth * 0.5f;
		const float hl = inLength * 0.5f;

		uint col = inDD->GetPackedColor( inCol.r, inCol.g, inCol.b );

		dd::Vertex_L3_COL_K vtxA[4] =
		{
			{	Vec3(-hw, 0.0f,-hl ), col, true	},
			{	Vec3(-hw, 0.0f, hl ), col, true	},
			{	Vec3( hw, 0.0f,-hl ), col, true	},
			{	Vec3( hw, 0.0f, hl ), col, true	}
		};

		uint sf = create_surface(	vtxA,
									4,
									dd::Vertex_L3_COL_K::fvf,
									dd::DP_VertexColor,
									dd::PRIM_TRIANGLE,
									inDD					);
		return sf;
	}
}


Primitive*	Primitive::CreateAsSphere	(	Handle*				inHandle,
											float				inRadius,
											const Color32&		inTopCol,
											const Color32&		inBotCol,
											uint				inRes,
											IDirectDraw*		inDD		)
{
	SU_ASSERT( inHandle );
	SU_ASSERT( inDD );
	SU_ASSERT( inHandle->GetSceneGraph() );
	Pool* pool = inHandle->GetSceneGraph()->GetPool();
	SU_ASSERT( pool );

	Primitive* np = pool->New<Primitive>();
	SU_ASSERT( np );
	SU_REQUIRE_OR_NULL( np );

	np->refCount = 1;
	np->handle	 = NULL;
	np->pool	 = pool;
	
	np->dd		= inDD;
	np->srf		= dd::INVALID_IDX;


	if( !np->Init( inHandle ) )
	{
		pool->Delete( np );
		return NULL;
	}
	np->dd->Retain();
	np->srf = create_sphere( inRadius, inRes, inTopCol, inBotCol, np->pool, np->dd );

	SU_ASSERT( np->handle == inHandle );
	return np;
}


Primitive*	Primitive::CreateAsPlan	(	Handle*				inHandle,
										float				inWidth,
										float				inLength,
										const Color32&		inCol,
										IDirectDraw*		inDD		)
{
	SU_ASSERT( inHandle );
	SU_ASSERT( inDD );
	SU_ASSERT( inHandle->GetSceneGraph() );
	Pool* pool = inHandle->GetSceneGraph()->GetPool();
	SU_ASSERT( pool );

	Primitive* np = pool->New<Primitive>();
	SU_ASSERT( np );
	SU_REQUIRE_OR_NULL( np );

	np->refCount = 1;
	np->handle	 = NULL;
	np->pool	 = pool;
	
	np->dd		= inDD;
	np->srf		= dd::INVALID_IDX;


	if( !np->Init( inHandle ) )
	{
		pool->Delete( np );
		return NULL;
	}
	np->dd->Retain();
	np->srf = create_plan( inWidth, inLength, inCol, np->dd );

	SU_ASSERT( np->handle == inHandle );
	return np;
}


uint		Primitive::GetSrf		(									)
{
	return srf;
}

void		Primitive::Shut			(									)
{
	if( dd )
	{
		dd->ResourceRelease( srf );
		dd->Release();
	}
}
