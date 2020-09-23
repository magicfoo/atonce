/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2008 AtOnce Technologies
**
** This file is part of the Sugar Framework.
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


#include <sufw/core/su_core.h>
#include "su_dg.h"


#define POOL_UNIT	(32*1024)

using namespace su;
using namespace su::dg;


namespace
{
	Pool*				pool = NULL;

	vector<byte*>		pools;
	vector<Node*>		nodeA;
	vector<Shadow*>		shadowA;
	vector<Edge*>		edgeA;

	uint				shadowCountA[256];


	bool
	check_ptr (	void* inPtr )
	{
		byte* p = (byte*) inPtr;
		for( uint i=0; i<pools.size(); i++ )
		{
			byte* p0 = pools[i];
			byte* p1 = pools[i] + POOL_UNIT;
			if( p>=p0 && p<p1 )
				return TRUE;
		}
		return FALSE;
	}


	template < typename T >
	bool
	check_pool (	vector<T*>&		items,
					uint			count	)
	{
		SU_ASSERT( pool );

		if( items.size() >= count )
			return TRUE;

		byte* bp = (byte*) pool->Alloc( POOL_UNIT );
		if( !bp )
			return FALSE;
		pools.push_back( bp );
		T* i0 = (T*) bp;
		T* i1 = i0 + (POOL_UNIT/sizeof(T));
		while( i0 < i1 )
			items.push_back( i0++ );
		SU_ASSERT( items.size() >= count );
		return TRUE;
	}


	template < typename T >
	T*
	find_prev ( T* t )
	{
		SU_ASSERT( check_ptr(t) );
		SU_ASSERT( check_ptr(t->next) );
		T* prev = t->next;
		while( prev->next!=t )
			prev = prev->next;
		return prev;
	}


	void
	_release ( Node* n )
	{
		SU_ASSERT( check_ptr(n) );
		SU_ASSERT( n->shadows==NULL );
		nodeA.push_back( n );
	}


	void
	_release ( Shadow* s )
	{
		SU_ASSERT( check_ptr(s) );
		SU_ASSERT( check_ptr(s->node) );
		SU_ASSERT( check_ptr(s->next) );
		SU_ASSERT( s->inputs==NULL );
		SU_ASSERT( s->outputs==NULL );

		// update head in node
		if( s->next == s )
			s->node->shadows  = NULL;		// no more levels
		else if( s->node->shadows == s )
			s->node->shadows = s->next;		// set new head

		// unlink
		Shadow* prev = find_prev( s );
		prev->next = s->next;

		SU_ASSERT( shadowCountA[s->layer] > 0 );
		shadowCountA[s->layer]--;

		shadowA.push_back( s );
	}


	void
	_release	(	Edge* e	)
	{
		SU_ASSERT( check_ptr( e ) );
		SU_ASSERT( check_ptr( e->shadow ) );
		SU_ASSERT( check_ptr( e->shadow->node ) );
		SU_ASSERT( check_ptr( e->next ) );
		SU_ASSERT( check_ptr( e->dual ) );
		
		// update head in level
		if( e->input )
		{
			if( e->next == e )
				e->shadow->inputs  = NULL;		// no more edges
			else if( e->shadow->inputs == e )
				e->shadow->inputs = e->next;	// set new head
		}
		else
		{
			if( e->next == e )
				e->shadow->outputs = NULL;		// no more edges
			else if( e->shadow->outputs == e )
				e->shadow->outputs = e->next;	// set new head
		}

		// unlink
		Edge* prev = find_prev( e );
		prev->next = e->next;

		edgeA.push_back( e );
	}


	Shadow*
	link_shadow	(	Node*		n,
					byte		layer		)
	{
		SU_ASSERT( check_ptr(n) );

		
		Shadow* s = n->shadow( layer );
		if( s )		return s;

		check_pool( shadowA, 1 );
		s = shadowA.back();		shadowA.pop_back();
		
		shadowCountA[layer]++;
		s->node		= n;
		s->layer	= layer;
		s->inputs	= NULL;
		s->outputs	= NULL;

		if( n->shadows )
		{
			s->next = n->shadows->next;
			n->shadows->next = s;
		}
		else
		{
			s->next = s;
			n->shadows = s;
		}
		return s;
	}


	Edge*	last_input( Edge*	inFInput )
	{
		Edge* it = inFInput;

		while( it->next && ( it->next != inFInput ) )
			it = it->next;
		return it;
	}
	void
	link_edge	(	Shadow*		l,
					Edge*		p		)
	{
		SU_ASSERT( check_ptr(l) );
		SU_ASSERT( check_ptr(l->next) );
		SU_ASSERT( check_ptr(l->node) );
		SU_ASSERT( check_ptr(p) );

		if( p->input )
		{
			if( l->inputs )
			{
				Edge* li = last_input( l->inputs );

				p->next = li->next;
				li->next = p;
			//	p->next = l->inputs->next;
			//	l->inputs->next = p;
			}
			else
			{
				p->next = p;
				l->inputs = p;
			}
		}
		else
		{
			if( l->outputs )
			{
				p->next = l->outputs->next;
				l->outputs->next = p;
			}
			else
			{
				p->next = p;
				l->outputs = p;
			}
		}
	}

}



//
// --- Node

Node*	Node::create	(	void*	inUserData	)
{
	check_pool( nodeA, 1 );
	Node* n = nodeA.back();
	nodeA.pop_back();

	n->udata	= inUserData;
	n->shadows	= NULL;

	return n;
}
void	Node::destroy	(						)
{
	SU_ASSERT( check_ptr( this ) );
	
	udata = NULL;
	if( shadows != NULL )
	{
		Shadow* f = shadows;
		Shadow* c = f;
		for( ;; )
		{
			Shadow* n = c->next;
			c->destroy();
			if( !n || ( n == f ) || ( n == c ) )
				break;
			c = n;
		}
	}
	_release( this );
}
Shadow*	Node::cast		(	byte	inLayer		)		// create or get
{
	SU_ASSERT( check_ptr( this ) );
	
	Shadow* s = link_shadow( this,  inLayer );
	SU_ASSERT( check_ptr( s ) );
	return s;
}
Shadow*	Node::shadow	(	byte	inLayer		)		// NULL if not existing
{
	SU_ASSERT( check_ptr( this ) );
	SU_REQUIRE_OR_NULL( shadows );

	Shadow* c = shadows;
	while( c->layer != inLayer )
	{
		c = c->next;
		if( c == shadows )	return NULL;
		if( !c )			return NULL;
	}
	return c;
}



//
// --- Shadow

void	Shadow::destroy		(						)
{
	SU_ASSERT( check_ptr( this ) );
	while( inputs )
	{
		inputs->destroy();
	}
	while( outputs )
	{
		outputs->destroy();
	}

	_release( this );
}
Edge*	Shadow::connectto	(	Shadow*	inTarget	)
{
	return Edge::create( this, inTarget );
}
void	Shadow::disconnect	(	Shadow*	inTarget	)
{
	if( inputs )
	{
		Edge* ci = inputs;
		for( ;; )
		{
			if( ci->shadow == inTarget )
			{
				ci->destroy();
				break;
			}
			ci = ci->next;
			if( !ci )			break;
			if( ci == inputs )	break;
		}
	}
	if( outputs )
	{
		Edge* co = outputs;
		for( ;; )
		{
			if( co->shadow == inTarget )
			{
				co->destroy();
				break;
			}
			co = co->next;
			if( !co )			break;
			if( co == outputs )	break;
		}
	}
}



//
// --- Edge


Edge*	Edge::create	(	Shadow*	inStart,
							Shadow*	inEnd		)
{
	SU_ASSERT( check_ptr( inStart ) );
	SU_ASSERT( check_ptr( inEnd ) );
	SU_ASSERT( inStart->layer == inEnd->layer );

	check_pool( edgeA, 2 );
	uint ec = edgeA.size();
	Edge* inE = edgeA.back();	edgeA.pop_back();
	Edge* ouE = edgeA.back();	edgeA.pop_back();

	inE->input  = 1;
	inE->shadow = inStart;
	inE->dual	= ouE;
	inE->next	= inE;

	ouE->input  = 0;
	ouE->shadow = inEnd;
	ouE->dual	= inE;
	ouE->next	= ouE;

	link_edge( inStart,	 inE );
	link_edge( inEnd, ouE );

	return inE;
}
void	Edge::destroy	(						)
{
	SU_ASSERT( check_ptr( this ) );
	SU_ASSERT( check_ptr( this->dual ) );

	_release( this->dual );
	_release( this );
}




//
// --- Gobal

bool su::dg::IsLayerExist	(	byte			inLayer			)
{
	return ( shadowCountA[inLayer] > 0 );
}

uint su::dg::CountLayer		(									)
{
	uint c = 0;
	for( uint i=0; i<256; i++ )
	{
		if( shadowCountA[i] > 0 )
			c++;
	}
	return c;
}

byte su::dg::EnumLayer		(	uint			inIdx			)
{
	uint c = 0;
	for( uint i=0; i<256; i++ )
	{
		if( shadowCountA[i] > 0 )
		{
			if( inIdx == c )
				return i;
			c++;
		}
	}
	return 0;
}




SU_BEGIN_STARTER( CoreDG )

void OnStage ( Stage stage )
{
	if( stage == STAGE_START )
	{
		Pool* p = globals::syspool;
		SU_ASSERT( p );

		pools.relocate( p );
		nodeA.relocate( p );
		shadowA.relocate( p );
		edgeA.relocate( p );

		util::Zero( shadowCountA, sizeof(shadowCountA) );
		pool = p;
	}
	else if( stage == STAGE_TERMINATE )
	{
		SU_ASSERT( pool );

		for( uint i=0; i<pools.size(); i++ )
			pool->Free( pools[i] );

		pools.relocate( NULL );
		nodeA.relocate( NULL );
		edgeA.relocate( NULL );
		shadowA.relocate( NULL );

		pool = NULL;
	}
}

SU_END_STARTER( CoreDG )



