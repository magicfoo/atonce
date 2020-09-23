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
#include "su_sg_handle.h"
#include "su_sg_scenegraph.h"


using namespace su;
using namespace su::sg;




void			Core::Retain		(							)
{
	refCount++;
}

uint			Core::CountRef		(							)
{
	return refCount;
}

void			Core::Release		(							)
{
	SU_ASSERT( refCount );
	refCount--;
	if( !refCount )
	{
		if( handle )
		{
			SceneGraph* sg = handle->GetSceneGraph();
			SU_ASSERT( sg );
			if( sg )
				sg->OnEvent( this, ER_Delete );
		}
		Shut();
		pool->Delete( this );
	}
}

Handle*	Core::GetHandle	(							)
{
	return handle;
}

bool	Core::Init		(	Handle*		inHandle	)
{
	SU_ASSERT( inHandle );
	SU_ASSERT( !handle );
	SU_REQUIRE_OR_FALSE( inHandle->SetCore( this ) );

	handle = inHandle;

	SceneGraph* sg = handle->GetSceneGraph();
	SU_ASSERT( sg );
	if( sg )
		sg->OnEvent( this, ER_New );

	return true;
}
void	Core::Shut		(							)
{}
