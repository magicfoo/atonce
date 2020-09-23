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

#include "su_collection.h"

#include "su_sg_core.h"
#include "su_sg_coregeometry.h"
#include "su_sg_handle.h"
#include "su_sg_scenegraph.h"



using namespace su;
using namespace su::sg;




Geometry*			Geometry::Create	(	Handle*				inHandle	)
{
	SU_ASSERT( inHandle );
	SU_ASSERT( inHandle->GetSceneGraph() );
	Pool* pool = inHandle->GetSceneGraph()->GetPool();
	SU_ASSERT( pool );

	Geometry* ng = pool->New<Geometry>();
	SU_ASSERT( ng );
	SU_REQUIRE_OR_NULL( ng );

	ng->refCount = 1;
	ng->handle	 = NULL;
	ng->pool	 = pool;
	
	ng->mesh	 = NULL;
	ng->selected = false;
	ng->scale	 = 1.0f;

	if( !ng->Init( inHandle ) )
	{
		pool->Delete( ng );
		return NULL;
	}

	SU_ASSERT( ng->handle == inHandle );
	return ng;
}


void				Geometry::SetMesh	(	collection::Mesh*	inMesh		)
{
	if( mesh )
	{
		mesh->release();
		mesh = NULL;
	}
	if( inMesh )
	{
		mesh = inMesh;
		mesh->retain();
	}
}

collection::Mesh*	Geometry::GetMesh	(									)	{	return mesh;		}

void				Geometry::Select	(	bool				inOnOff		)	{	selected = inOnOff;	}
bool				Geometry::IsSelected(									)	{	return selected;	}

void				Geometry::SetScale	(	float				inScale		)	{	scale = inScale;	}
float				Geometry::GetScale	(									)	{	return scale;		}

void				Geometry::Shut		(									)
{
	if( mesh )
	{
		mesh->release();
		mesh = NULL;
	}
}
