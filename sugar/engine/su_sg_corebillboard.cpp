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
#include "su_sg_corebillboard.h"
#include "su_sg_handle.h"
#include "su_sg_scenegraph.h"



using namespace su;
using namespace su::sg;


namespace
{

	void create_billboard	(	uint*			outVbo,
								IDirectDraw*	inDD	)
	{
		SU_ASSERT( inDD );

		uint vbo;
		uint crVboCmdA[] =
		{
			SU_DD_VBOCREATE_1( dd::VF_LOC2 | dd::VF_UV0, dd::PRIM_TRIANGLE, 0 ),
			SU_DD_VBOCREATE_2( 4 ),
			SU_DD_VBOCREATE_3( &vbo ),
			SU_DD_END()
		};

		inDD->StartAndWaitCmdList( crVboCmdA );

		*outVbo = vbo;
	}

	void update_billboard	(	uint			inVbo,
								const Vec2&		inSize,
								const Vec4&		inUV,
								IDirectDraw*	inDD	)
	{
		SU_ASSERT( inDD );


		struct Vertex_L2_UV0
		{
			Vec2 loc;
			Vec2 uv0;
		};

		
		float hw = inSize.x * 0.5f;
		float hh = inSize.y * 0.5f;

	//	float u0 = inUV.x;
	//	float v0 = inUV.y;
	//	float u1 = inUV.z;
	//	float v1 = inUV.w;

		Vertex_L2_UV0 vtxA[4] =
		{
			{ Vec2( -hw, -hh ),	Vec2( inUV.x, inUV.w ) },
			{ Vec2( -hw,  hh ),	Vec2( inUV.x, inUV.y ) },
			{ Vec2(  hw, -hh ),	Vec2( inUV.z, inUV.w ) },
			{ Vec2(  hw,  hh ),	Vec2( inUV.z, inUV.y ) }
		};

		uint upSrfCmdA[] =
		{
			SU_DD_VBOFILL_1( inVbo ),
			SU_DD_VBOFILL_2( dd::VF_LOC2 | dd::VF_UV0 ),
			SU_DD_VBOFILL_3( vtxA ),
			SU_DD_END()
		};

		inDD->StartAndWaitCmdList( upSrfCmdA );
	}
}



Billboard*	Billboard::Create	(	Handle*				inHandle,
									IDirectDraw*		inDD		)
{
	SU_ASSERT( inHandle );
	SU_ASSERT( inDD );
	SU_ASSERT( inHandle->GetSceneGraph() );
	Pool* pool = inHandle->GetSceneGraph()->GetPool();
	SU_ASSERT( pool );

	Billboard* nb = pool->New<Billboard>();
	SU_ASSERT( nb );
	SU_REQUIRE_OR_NULL( nb );

	nb->refCount	= 1;
	nb->handle		= NULL;
	nb->pool		= pool;
	
	nb->dd			= inDD;
	nb->size		= Vec2( 1.0f, 1.0f );
	nb->uv			= Vec4( 0.0f, 0.0f, 1.0f, 1.0f );
	nb->vbo			= dd::INVALID_IDX;
	nb->pbi			= dd::INVALID_IDX;
	nb->alphaTH		= false;
	nb->colorTH		= Color32( 255, 255, 255, 255 );
	nb->isInWorld	= true;

	if( !nb->Init( inHandle ) )
	{
		pool->Delete( nb );
		return NULL;
	}
	nb->dd->Retain();
	create_billboard( &(nb->vbo), nb->dd );
	update_billboard( nb->vbo, nb->size, nb->uv, nb->dd );

	SU_ASSERT( nb->handle == inHandle );
	return nb;
}



void		Billboard::SetPbi	(	uint				inPbi			)
{
	SU_ASSERT( dd );

	if( pbi != dd::INVALID_IDX )
	{
		dd->ResourceRelease( pbi );
	}
	pbi = inPbi;
	dd->ResourceAddRef( pbi );
}

void		Billboard::SetSize	(	const Vec2&			inSize		)
{
	if( size != inSize )
	{
		size = inSize;
		update_billboard( vbo, size, uv, dd );
	}
}

void		Billboard::SetUV	(	const Vec4&			inU0V0U1V1	)
{
	if( uv != inU0V0U1V1 )
	{
		uv = inU0V0U1V1;
		update_billboard( vbo, size, uv, dd );
	}
}
void		Billboard::SetParam	(	const Vec2&			inSize,
									const Vec4&			inU0V0U1V1	)
{
	if( size != inSize || uv != inU0V0U1V1 )
	{
		size = inSize;
		uv = inU0V0U1V1;
		update_billboard( vbo, size, uv, dd );
	}
}


void		Billboard::SetToScreenSpace			(									)
{
	isInWorld = false;
}
void		Billboard::SetToWorldSpace			(									)
{
	isInWorld = true;
}

bool		Billboard::IsInWorldspace			(									)
{
	return isInWorld;
}

uint		Billboard::GetVbo	(									)
{
	return vbo;
}	
uint		Billboard::GetPbi	(									)
{
	return pbi;
}

void		Billboard::Shut		(									)
{
	if( dd )
	{
		dd->ResourceRelease( vbo );
		dd->ResourceRelease( pbi );
		dd->Release();
	}
}



void		Billboard::EnableAlphaThreshold	(	float			inTh,
												const Color32&	inColorTh	)
{
	alphaTH = true;
	alphaTHV = inTh;
	colorTH = inColorTh;
}
bool		Billboard::GetAlphaThresholdParam(	float*		ioTH,
												Color32*	ioColor	)
{
	SU_REQUIRE_OR_FALSE( alphaTH );
	*ioTH = alphaTHV;
	*ioColor = colorTH;
	return true;
}