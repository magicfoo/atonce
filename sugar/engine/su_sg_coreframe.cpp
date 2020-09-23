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
#include "su_sg_coreframe.h"
#include "su_sg_handle.h"
#include "su_sg_scenegraph.h"


using namespace su;
using namespace su::sg;




Frame*	Frame::Create		(	Handle*			inHandle	)
{
	SU_ASSERT( inHandle );
	SU_ASSERT( inHandle->GetSceneGraph() );
	Pool* pool = inHandle->GetSceneGraph()->GetPool();
	SU_ASSERT( pool );

	Frame* nf = pool->New<Frame>();
	SU_ASSERT( nf );
	SU_REQUIRE_OR_NULL( nf );

	nf->refCount = 1;
	nf->handle	 = NULL;
	nf->pool	 = pool;

	if( !nf->Init( inHandle ) )
	{
		pool->Delete( nf );
		return NULL;
	}
	
	nf->SetTR( Matrix::UNIT );
	SU_ASSERT( nf->handle == inHandle );
	return nf;
}
		


void	Frame::SetPosition	(	const Vec3&		inPos		)
{
	loc = inPos;
	MatrixBuildTR( &tr, &loc, &rot, NULL );
}
void	Frame::SetRotation	(	const Quat&		inRot		)
{
	rot = inRot;
	MatrixBuildTR( &tr, &loc, &rot, NULL );
}
void	Frame::SetTR		(	const Matrix&	inTR		)
{
	tr = inTR;
	MatrixGetTR( &loc, &rot, NULL, &tr );
}

Vec3	Frame::GetPosition	(								)	{	return loc;		}
Quat	Frame::GetRotation	(								)	{	return rot;		}
Matrix	Frame::GetTR		(								)	{	return tr;		}
