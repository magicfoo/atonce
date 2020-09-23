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



#include "../su_base.h"
#include "su_forceref.h"
using namespace su;


#define BLIST_H			(4)

// based on a skip list data structure :
//		http://igoro.com/archive/skip-lists-are-fascinating/



namespace
{

	addin::Starter*		slist_first		= NULL;

	addin::Builder*		blist_first		= NULL;
	bool				blist_optmzd	= false;



	template < typename T >
	void optimize_skip_list ( T*& first )
	{
		// bubble sorting first !
		bool again = true;
		while( again )
		{
			again = false;

			T *p, *t0, *t1;
			p = NULL;
			t0 = first;

			for( ;; )
			{
				if( !t0 )
					break;

				t1 = t0->next[0];
				if( !t1 )
					break;

				uint32 m0 = t0->GetMUID();
				uint32 m1 = t1->GetMUID();
				SU_ASSERT( m0 );
				SU_ASSERT( m1 );
				if( m1 < m0 )
				{
					if( p )	p->next[0] = t1;
					else	first = t1;
					t0->next[0] = t1->next[0];
					t1->next[0] = t0;
					util::Swap( t0, t1 );
					again = true;
				}
				p = t0;
				t0 = t1;
			}
		}
	}


	void optimize_blist ( )
	{
		if( !blist_optmzd )
		{
			optimize_skip_list( blist_first );
			blist_optmzd = true;
		}
	}


	bool has_interface ( addin::Builder* b, classid cuid )
	{
		SU_ASSERT( b );
		uint n = b->CountInterface();
		classid c;
		for( uint i=0 ; i<n ; i++ )
			if( b->EnumInterface(i,&c,NULL) && c==cuid )
				return true;
		return false;
	}

}





addin::Builder::Builder ( )
{
	SU_COMPILE_TIME_ASSERT( sizeof(next)==(BLIST_H*sizeof(this)) );
	util::Zero( next );
	next[0] = blist_first;
	blist_first = this;
	blist_optmzd = false;
}


addin::Builder* addin::Builder::Find ( classid muid )
{
	optimize_blist();

	Builder* b = blist_first;

	while( b )
	{
		if( b->GetMUID() == muid )
			return b;

		b = b->next[0];
	}

	return NULL;
}


addin::Builder* addin::Builder::First ( )
{
	optimize_blist();

	if( !blist_first )
		return NULL;
	else
		return blist_first;
}


addin::Builder* addin::Builder::Next ( Builder* b )
{
	optimize_blist();

	if( !b )
		return NULL;
	else if( !b->next[0] )
		return NULL;
	else
		return b->next[0];
}


addin::Builder* addin::Builder::FirstConcern ( classid cuid )
{
	optimize_blist();

	Builder* b = blist_first;

	while( b )
	{
		if( has_interface (b,cuid) )
			return b;

		b = b->next[0];
	}

	return NULL;
}


addin::Builder* addin::Builder::NextConcern ( classid cuid, Builder* from )
{
	optimize_blist();

	Builder* b = Next(from);

	while( b )
	{
		if( has_interface (b,cuid) )
			return b;

		b = b->next[0];
	}

	return NULL;
}







addin::Starter::Starter ( )
{
	next = slist_first;
	slist_first = this;
}


addin::Starter* addin::Starter::First ( )
{
	if( !slist_first )
		return NULL;
	else
		return slist_first;
}


addin::Starter* addin::Starter::Next ( Starter* s )
{
	if( !s )
		return NULL;
	else if( !s->next )
		return NULL;
	else
		return s->next;
}


void addin::Starter::Setup ( Stage stage )
{
	addin::Starter* s = slist_first;

	while( s )
	{
		s->OnStage( stage );
		s = s->next;
	}
}





