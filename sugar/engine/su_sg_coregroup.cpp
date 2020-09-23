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
#include "su_sg_coregroup.h"
#include "su_sg_handle.h"
#include "su_sg_scenegraph.h"


using namespace su;
using namespace su::sg;



Group*	Group::Create		(	Handle*		inHandle,
								uint		inCapacity	)
{
	SU_ASSERT( inHandle );
	SU_ASSERT( inHandle->GetSceneGraph() );
	Pool* pool = inHandle->GetSceneGraph()->GetPool();
	SU_ASSERT( pool );

	Group* ng = pool->New<Group>( inCapacity * sizeof(Core*) );
	SU_ASSERT( ng );
	SU_REQUIRE_OR_NULL( ng );

	ng->refCount	= 1;
	ng->handle		= NULL;
	ng->pool		= pool;

	ng->capacity	= inCapacity;
	ng->childCount	= 0;
	ng->childA		= (Core**)(ng+1);
	util::Memset( ng->childA, 0, inCapacity * sizeof(Core*) );

	if( !ng->Init( inHandle ) )
	{
		pool->Delete( ng );
		return NULL;
	}

	SU_ASSERT( ng->handle == inHandle );
	return ng;
}
		
uint	Group::GetCapacity	(							)
{
	return capacity;
}

bool	Group::AddChild		(	Core*		inChild		)
{
	SU_ASSERT( inChild );

	SU_REQUIRE_OR_FALSE( childCount < capacity );
	SU_REQUIRE_OR_FALSE( inChild );
	childA[childCount] = inChild;
	childA[childCount]->Retain();
	childCount++;
	return true;
}

uint	Group::CountChild	(							)
{
	return childCount;
}

Core*	Group::EnumChild		(	uint		inIdx		)
{
	SU_REQUIRE_OR_NULL( inIdx<childCount );
	return childA[inIdx];
}

void	Group::ReleaseChildren	(							)
{
	SU_ASSERT( childCount <= capacity );
	for( uint i=0; i<childCount; i++ )
	{
		SU_ASSERT( childA[i] );
		childA[i]->Release();
		childA[i] = NULL;
	}
	childCount = 0;
}

void	Group::Shut			(							)
{
	ReleaseChildren();
}
