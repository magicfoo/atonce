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

#include "su_sg_handle.h"
#include "su_sg_handleroot.h"
#include "su_sg_scenegraph.h"


using namespace su;
using namespace su::sg;



namespace
{
	dg::Shadow*	create_shadow	(	Handle*			inHandle,
									SceneGraph*		inSG		)
	{
		dg::Node* nd = dg::Node::create( inHandle );
		SU_ASSERT( nd );
		SU_REQUIRE_OR_FALSE( nd );
		SU_ASSERT( nd->shadow( inSG->GetLayerId() ) == NULL );

		dg::Shadow* shd = nd->cast( inSG->GetLayerId() );
		if( !shd )
		{
			nd->destroy();
		}
		return shd;
	}
}

Root*		Root::Create		(	SceneGraph*		inSG	)
{
	SU_ASSERT( inSG );
	SU_REQUIRE_OR_NULL( inSG );
	
	Pool* pool = inSG->GetPool();
	SU_ASSERT( pool );
	Root* nr = pool->New<Root>();
	SU_ASSERT( nr );
	SU_REQUIRE_OR_NULL( nr );

	nr->sg		= inSG;
	nr->shadow	= create_shadow( nr, inSG );
	nr->core	= NULL;
	nr->pool	= pool;
	nr->refCount= 1;
	
	nr->attr	= HA_Visibility_Show;
	
	inSG->OnEvent( nr, ER_New );

	if( !nr->shadow )
	{
		pool->Delete( nr );
		nr = NULL;
	}

	return nr;
}

SceneGraph*	Root::GetSceneGraph	(								)
{
	return sg;
}
