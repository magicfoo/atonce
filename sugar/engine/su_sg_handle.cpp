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
#include "su_sg_core.h"
#include "su_sg_coregroup.h"
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
	void		delete_shadow	(	dg::Shadow*&	ioDGShadow	)
	{
		SU_ASSERT( ioDGShadow );
		SU_ASSERT( ioDGShadow->node );

		// Delete dg ref
		dg::Shadow* sn = ioDGShadow->node->shadows;
		SU_ASSERT( sn );
		uint sc = 0;
		for( ;; )
		{
			sc++;
			sn = sn->next;
			if( sn == NULL || sn == ioDGShadow->node->shadows )
				break;
		}
		if( sc == 1 )	ioDGShadow->node->destroy();
		else			ioDGShadow->destroy();
		ioDGShadow = NULL;
	}
}


Handle*		Handle::Create			(	Handle*			inParent	)
{
	SU_ASSERT( inParent );
	SceneGraph* sg = inParent->GetSceneGraph();
	SU_ASSERT( sg );
	Pool* pool = sg->GetPool();
	SU_ASSERT( pool );

	Handle* nh = pool->New<Handle>();
	SU_ASSERT( nh );
	SU_REQUIRE_OR_NULL( nh );

	if( !nh->Init( inParent, pool ) )
	{
		pool->Delete( nh );
		nh = NULL;
	}

	return nh;
}

void		Handle::Retain			(								)
{
	refCount++;
}
uint		Handle::CountRef		(								)
{
	return refCount;
}
void		Handle::Release			(								)
{
	SU_ASSERT( refCount );
	refCount--;
	if( !refCount )
	{
		Shut();
	}
}
		
Handle*		Handle::GetParent		(								)
{
	SU_ASSERT( shadow );
	SU_REQUIRE_OR_NULL( shadow->outputs );
	SU_ASSERT( ( shadow->outputs == shadow->outputs->next ) || ( !shadow->outputs->next ) );	// parent uniqueness
	SU_ASSERT( shadow->outputs );
	SU_ASSERT( shadow->outputs->dual->shadow );
	SU_ASSERT( shadow->outputs->dual->shadow != shadow );
	SU_ASSERT( shadow->outputs->dual->shadow->node );
	SU_ASSERT( shadow->outputs->dual->shadow->node != shadow->node );
	SU_ASSERT( shadow->outputs->dual->shadow->node->udata );

	return (Handle*)(shadow->outputs->dual->shadow->node->udata);
}
uint		Handle::CountChild		(								)
{
	SU_ASSERT( shadow );
	SU_REQUIRE_OR_0( shadow->inputs );

	dg::Edge* f = shadow->inputs;
	dg::Edge* c = f;
	uint cc = 0;
	for( ;; )
	{
		cc++;
		if( c == c->next )	return cc;
		c = c->next;
		if( c == f || !c )	return cc;
	}
}
Handle*		Handle::EnumChild		(	uint			inIdx		)
{
	SU_ASSERT( shadow );
	SU_REQUIRE_OR_NULL( shadow->inputs );

	dg::Edge* f = shadow->inputs;
	dg::Edge* c = f;
	uint cc = 0;
	for( ;; )
	{
		SU_ASSERT( c );
		if( inIdx == cc )
		{
			SU_ASSERT( c->dual );
			SU_ASSERT( c->dual->shadow );
			SU_ASSERT( c->dual->shadow->node );
			return (Handle*)(c->dual->shadow->node->udata);
		}
		cc++;
		if( c == c->next )	return NULL;
		c = c->next;
		if( c == f || !c )	return NULL;
	}
}

void		Handle::ReleaseChildren	(								)
{
	dg::Edge* ce = shadow->inputs;
	while( ce )
	{
		SU_ASSERT( ce->dual );
		SU_ASSERT( ce->dual->shadow );
		SU_ASSERT( ce->dual->shadow->node );
		SU_ASSERT( ce->dual->shadow->node->udata );

		Handle* cChild = (Handle*)( ce->dual->shadow->node->udata );
		cChild->Release();

		if( ce == shadow->inputs )
		{
			ce->destroy();
		}
		ce = shadow->inputs;
	}

	SU_ASSERT( CountChild() == 0 );
}


bool		Handle::ReleaseChild	(	Handle*			inHandle	)
{
	uint cc = CountChild();
	dg::Edge* ce = shadow->inputs;
	while( true )
	{
		SU_ASSERT( ce->dual );
		SU_ASSERT( ce->dual->shadow );
		SU_ASSERT( ce->dual->shadow->node );
		SU_ASSERT( ce->dual->shadow->node->udata );

		Handle* cChild = (Handle*)( ce->dual->shadow->node->udata );
		if( cChild == inHandle )
		{
			bool dstCe = ( cChild->CountRef() > 1 );
			cChild->Release();
			if( dstCe )
				ce->destroy();
			break;
		}

		ce = ce->next;
		if( !ce || ce == shadow->inputs )
		{
			break;
		}
	}

	return ( CountChild() == ( cc - 1 ) );
}
		
SceneGraph*	Handle::GetSceneGraph	(								)
{
	Handle* p = GetParent();
	if( p )
		return p->GetSceneGraph();

	return NULL;
}

Core*		Handle::GetCore			(								)
{
	return core;
}

Pool*		Handle::GetPool			(								)
{
	return pool;
}


uint		Handle::GetAttributes	(								)
{
	return attr;
}
void		Handle::SetAttributes	(	uint			inAttr		)
{
	uint g;
	
	// Visibility
	g = inAttr & HA_Visibility_Mask;
	attr = g ? (attr & (~HA_Visibility_Mask)) | g : attr;
}


void		Handle::Show			(								)
{
	SetAttributes( HA_Visibility_Show );
}
void		Handle::Hide			(								)
{
	SetAttributes( HA_Visibility_Hide );
}

bool		Handle::SetCore			(	Core*			inCore		)
{
	if( !core )
	{
		core = inCore;
		core->Retain();
		return true;
	}
	if( core->GetType() == Group::Type )
	{
		Group* gp = (Group*)core;
		return gp->AddChild( inCore );
	}
	return false;
}

bool		Handle::AddChild		(	Handle*			inChild		)
{
	SU_ASSERT( shadow );
	SU_ASSERT( inChild );
	SU_ASSERT( inChild->shadow );

	dg::Edge* e = shadow->connectto( inChild->shadow );
	SU_ASSERT( e );
	SU_REQUIRE_OR_FALSE( e );

	inChild->Retain();

	SceneGraph* sg = GetSceneGraph();
	SU_ASSERT( sg );
	if( sg )
		sg->OnEvent( inChild, ER_New );

	return true;
}

bool		Handle::Init			(	Handle*			inParent,
										Pool*			inPool		)
{
	shadow	= create_shadow( this, inParent->GetSceneGraph() );
	SU_REQUIRE_OR_FALSE( shadow );

	core	= NULL;
	refCount= 1;
	pool	= inPool;
	
	attr	= HA_Visibility_Show;

	if( inParent->AddChild( this ) )
	{
		return true;
	}
	delete_shadow( shadow );
	return false;
}

void		Handle::Shut			(								)
{
	SU_ASSERT( !refCount );

	SceneGraph* sg = GetSceneGraph();
	if( sg )
		sg->OnEvent( this, ER_Delete );

	ReleaseChildren();

	if( core )
	{
		if( core->CountRef() > 1 )
			core->handle = NULL;

		core->Release();
		core = NULL;
	}
	delete_shadow( shadow );
	pool->Delete( this );
}