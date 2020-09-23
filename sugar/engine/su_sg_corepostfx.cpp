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
#include "su_sg_corepostfx.h"
#include "su_sg_handle.h"
#include "su_sg_scenegraph.h"


using namespace su;
using namespace su::sg;




PostFx*	PostFx::Create		(	Handle*			inHandle	)
{
	SU_ASSERT( inHandle );
	SU_ASSERT( inHandle->GetSceneGraph() );
	Pool* pool = inHandle->GetSceneGraph()->GetPool();
	SU_ASSERT( pool );

	PostFx* np = pool->New<PostFx>();
	SU_ASSERT( np );
	SU_REQUIRE_OR_NULL( np );

	np->refCount = 1;
	np->handle	 = NULL;
	np->pool	 = pool;

	if( !np->Init( inHandle ) )
	{
		pool->Delete( np );
		return NULL;
	}
	
	SU_ASSERT( np->handle == inHandle );
	return np;
}

