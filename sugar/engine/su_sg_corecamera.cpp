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

#include "su_sg_core.h"
#include "su_sg_corecamera.h"
#include "su_sg_handle.h"
#include "su_sg_scenegraph.h"


using namespace su;
using namespace su::sg;




Camera*	Camera::Create		(	Handle*			inHandle	)
{
	SU_ASSERT( inHandle );
	SU_ASSERT( inHandle->GetSceneGraph() );
	Pool* pool = inHandle->GetSceneGraph()->GetPool();
	SU_ASSERT( pool );

	Camera* nc = pool->New<Camera>();
	SU_ASSERT( nc );
	SU_REQUIRE_OR_NULL( nc );

	nc->refCount	= 1;
	nc->handle		= NULL;
	nc->pool		= pool;
	nc->target		= NULL;
	nc->targetRoll	= 0.0f;

	if( !nc->Init( inHandle ) )
	{
		pool->Delete( nc );
		return NULL;
	}
	SU_ASSERT( nc->handle == inHandle );
	nc->SetPerspective( Pi*0.4f, Vec2( 0.1f, 10.0f ) );
	return nc;
}



void	Camera::SetPerspective	(	float			inFOV,
									const Vec2&		inRange		)
{
	fov = inFOV;
	range = inRange;
	fromMat = false;
}
void	Camera::SetPerspective	(	const Matrix&	inMat		)
{
	proj = inMat;
	fromMat = true;
}
Matrix	Camera::GetPerspective	(	float			inAspect	)
{
	if( fromMat )
	{
		return proj;
	}
	else
	{
		Matrix p;
		MatrixPerspectiveLH( &p, fov, inAspect, range.x, range.y );
		return p;
	}
}
void	Camera::SetFOV          (	float           inFOV       )
{
    fov = inFOV;
}
float	Camera::GetFOV          (								)
{
    return fov;
}

void	Camera::SetTarget		(	Handle*			inHandle	)
{
	if( target )	target->Release();
	target = inHandle;
	if( target )	target->Retain();
}

void	Camera::SetTargetRoll	(	float			inAngle		)	{	targetRoll = inAngle;	}
void	Camera::SetDOF			(	float			inDist		)	{	dofDist = inDist;		}
Matrix	Camera::GetLookatTR		(								)
{
	Matrix r;

	SceneGraph* sg;
	if( handle && ( sg = handle->GetSceneGraph() ) )
	{
		sg->BuildWorldTR( &r, handle );

		if( target )
		{
			Vec3 posCam = *(r.location());

			sg->BuildWorldTR( &r, target );
			Vec3 posTarg = *(r.location());

			Vec3 front = posTarg - posCam;
			Vec3Normalize( &front, &front );

			Vec3 up( Vec3::UNIT_Y ), right;
			{
				float d = Vec3Dot( &up, &front );
				if( d == 1.0f || d == -1.0f )
					up = Vec3::UNIT_Z * (-d);

				Vec3Normalize( Vec3Cross( &right, &up, &front ), &right );
				Vec3Normalize( Vec3Cross( &up, &front, &right ), &up );
			}
			MatrixTranslation( &r, &posCam );
			MatrixSetAxis( &r, &right, &up, &front );
		}
	}
	
	return r;
}