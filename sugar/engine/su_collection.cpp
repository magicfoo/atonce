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
#include <sufw/asset/su_asset.h>
#include <sufw/directdriver/su_directdriver.h>
#include <sufw/su_mapping.h>

#include "su_engine.h"

using namespace su;
using namespace su::map;
using namespace su::collection;




namespace
{


	collection::Tag		null_tag = { 0, 0, 0 };
	collection::Define	null_def = { {0,0,0}, 0 };
	collection::Export	null_exp = { {0,0,0}, 0, NULL };
	collection::List	null_li  = { {0,0,0}, 0, NULL };



	struct MyResource : public collection::Resource
	{
		Pool*						po;
		uint						refcnt;
		IRscManager*				rscman;
		IRscManager::Resource		rsc;

		short						tcnt;
		byte**						taddr;

		short						defcnt;
		short*						deftags;

		short						expcnt;
		short*						exptags;

		short						licnt;
		short*						litags;

		Pool*						pool		( );
		IRscManager*				rscmanager	( );
		uint32						rid			( );

		void						retain		( );
		void						release		( );
		uint						countref	( );

		uint						counttag	( );
		Tag							enumtag		( uint );
		Tag							firsttag	( );
		Tag							nexttag		( const Tag& );
		Tag							findtag		( int code );

		uint						countdef	( );
		Define						enumdef		( uint );
		Define						finddef		( int id );

		uint						countexport	( );
		Export						enumexport	( uint );
		Export						findexport	( pcstr inname );

		uint						countlist	( );
		List						enumlist	( uint );
		Tag							iterlist	( uint li, uint idx );
	};



	void MyResource::retain ( )
	{
		refcnt++;
	}


	void MyResource::release ( )
	{
		SU_ASSERT( refcnt );
		refcnt--;
		if( refcnt ) return;
		rscman->ReleaseResource( rsc.rid );
		rscman->Release();
		po->Delete( this );
	}
	
	uint MyResource::countref ()
	{
		return refcnt;
	}


	Pool* MyResource::pool ( )
	{
		return po;
	}


	IRscManager* MyResource::rscmanager	( )
	{
		return rscman;
	}


	uint32 MyResource::rid ( )
	{
		return rsc.rid;
	}


	uint MyResource::counttag	( )
	{
		return tcnt;
	}


	collection::Tag MyResource::enumtag ( uint i )
	{
		if( i < tcnt )
		{
			Tag t;
			t.addr = taddr[i];
			t.index = i;
			t.code = base_tag::getcode( t.addr );
			return t;
		}
		else
		{
			return null_tag;
		}
	}


	collection::Tag	MyResource::firsttag ( )
	{
		Tag t;
		t.index = 0;
		t.addr = taddr[0];
		t.code = base_tag::getcode( t.addr );
		return t;
	}


	collection::Tag MyResource::nexttag ( const Tag& intag )
	{
		return enumtag( intag.index+1 );
	}


	collection::Tag MyResource::findtag ( int incode )
	{
		for( uint i=0 ; i<tcnt ; i++ )
		{
			if( base_tag::getcode(taddr[i]) == incode )
			{
				Tag t;
				t.index = i;
				t.addr = taddr[i];
				t.code = incode;
				return t;
			}
		}

		return null_tag;
	}


	uint MyResource::countdef	( )
	{
		return defcnt;
	}


	collection::Define MyResource::enumdef ( uint i )
	{
		if( i<defcnt )
		{
			rsc_collection_define def;
			Tag t = enumtag( deftags[i] );
			t.load( def );

			Define d;
			d.tag = t;
			d.id = def.id.get();
			return d;
		}
		else
		{
			return null_def;
		}
	}


	collection::Define MyResource::finddef ( int inid )
	{
		for( uint i=0 ; i<defcnt ; i++ )
		{
			Define d = enumdef(i);
			if( d.id == inid )
				return d;
		}

		return null_def;
	}


	uint MyResource::countexport	( )
	{
		return expcnt;
	}


	collection::Export MyResource::enumexport	( uint i )
	{
		if( i < expcnt )
		{
			rsc_collection_litem_export exp;
			Tag t = enumtag( exptags[i] );
			t.load( exp );

			Export e;
			e.tag = t;
			e.objectid = exp.objectid.get();
			e.name = (pcstr)exp.name.text.p;
			return e;
		}
		else
		{
			return null_exp;
		}
	}


	collection::Export MyResource::findexport	( pcstr inname )
	{
		for( uint i=0 ; i<expcnt ; i++ )
		{
			Export e = enumexport(i);
			if( util::Strequals(inname,e.name) )
				return e;
		}

		return null_exp;
	}


	uint MyResource::countlist ( )
	{
		return licnt;
	}


	collection::List MyResource::enumlist	( uint i )
	{
		if( i < licnt )
		{
			rsc_collection_list li;
			Tag t = enumtag( litags[i] );
			t.load( li );

			List l;
			l.tag = t;
			l.len = li.count.get();
			l.name = (pcstr)li.name.text.p;
			return l;
		}
		else
		{
			return null_li;
		}
	}


	collection::Tag MyResource::iterlist	( uint i, uint idx )
	{
		if( i < licnt )
		{
			return enumtag( litags[i]+1+idx );
		}
		else
		{
			return null_tag;
		}
	}



}




collection::Resource* collection::create ( IRscManager* inrscman, uint32 inrid, Pool* inp )
{
	SU_REQUIRE_OR_NULL( inrscman );
	SU_REQUIRE_OR_NULL( inp );

	IRscManager::Resource r;
	if( !inrscman->FetchResource(inrid,r) )
		return NULL;

	if( r.tid != rsc_collection::CSIGN )
		return NULL;

	rsc_collection co;
	co.load( r.bytes );

	if( co.bsize() == 0 )
		return NULL;

	int tcnt=0, defcnt=0, expcnt=0, licnt=0;

	co.tag_first();
	do {
																		tcnt++;
		if     ( co.tag_code() == rsc_collection_define::CODE )			defcnt++;
		else if( co.tag_code() == rsc_collection_litem_export::CODE )	expcnt++;
		else if( co.tag_code() == rsc_collection_list::CODE )			licnt++;
	} while( co.tag_next() );

	uint supplybs = tcnt*sizeof(byte*) + (defcnt+expcnt+licnt)*sizeof(short);

	MyResource* c = inp->New<MyResource>( supplybs );
	SU_ASSERT( c );

	byte** tl = (byte**)(c+1);
	short* sl = (short*)(tl+tcnt);

	c->refcnt	= 1;
	c->po		= inp;
	c->rscman	= inrscman;
	c->rsc		= r;

	c->tcnt		= 0;	c->taddr	= tl;
	c->defcnt	= 0;	c->deftags	= defcnt?sl:NULL;	sl += defcnt;	
	c->expcnt	= 0;	c->exptags	= expcnt?sl:NULL;	sl += expcnt;
	c->licnt	= 0;	c->litags	= licnt?sl:NULL;	sl += licnt;

	co.tag_first();
	do {
		if     ( co.tag_code() == rsc_collection_define::CODE )			c->deftags[ c->defcnt++ ] = c->tcnt;
		else if( co.tag_code() == rsc_collection_litem_export::CODE )	c->exptags[ c->expcnt++ ] = c->tcnt;
		else if( co.tag_code() == rsc_collection_list::CODE )			c->litags[ c->licnt++ ] = c->tcnt;
		c->taddr[ c->tcnt++ ] = co.tag_addr();
	} while( co.tag_next() );

	inrscman->Retain();
	inrscman->RetainResource( inrid );
	return c;
}

