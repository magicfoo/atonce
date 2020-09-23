/*LIC-HDR********************************************************************
**
** Copyright (C) 2005-2010 AtOnce Technologies
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




struct StashResource;
struct StashList;
struct StashCloud;





struct StashResource
{
	StashResource*				next;
	StashResource*				prev;
	uint						refcnt;

	StashList*					stash;
	uint32						rid;
	uint32						tid;
	byte*						contentaddr;
	uint						contentbs;
	uint						loadedbs;		// fully stashed when loadedbs==contentbs

	static StashResource*		create			( uint inbsize, Pool* );
	void						recycle			( Pool* );
	void						retain			( );
	void						release			( );

	bool						isempty			( );
	bool						isloading		( );
	bool						isloaded		( );
};



struct StashList
{
	StashList*					next;
	StashList*					prev;
	uint						refcnt;

	Pool*						pool;
	StashResource*				first;

	static StashList*			create			( Pool* instashpool, Pool* inp );
	void						recycle			( Pool* inp );
	void						retain			( );
	void						release			( );

	bool						squeeze_one		( );
	void						squeeze			( );

	StashResource*				allocRsc		( uint inbsize );
	StashResource*				findRsc			( uint32 inRID );
};



struct StashCloud
{
	StashList*					first;

								StashCloud	( );
	void						clear			( Pool* inp );

	StashList*					newStash		( Pool* inStashPool, Pool* inp );
	bool						freeStash		( StashList* inStash, Pool* inp );		// stash is recycled !

	StashResource*				findRsc			( uint32 inRID );

	void						squeeze			( );
};









//
// StashedResource
//

StashResource* StashResource::create ( uint inbsize, Pool* inp )
{
	SU_REQUIRE_OR_NULL( inbsize );
	SU_REQUIRE_OR_NULL( inp );

	StashResource* r = inp->New<StashResource>( inbsize );
	SU_REQUIRE_OR_NULL( r );

	r->stash = NULL;
	r->next = NULL;
	r->prev = NULL;
	r->refcnt = 0;
	r->rid = 0;
	r->tid = 0;
	r->contentaddr = (byte*)(r+1);
	r->contentbs = inbsize;
	r->loadedbs = 0;
	return r;
}

void StashResource::recycle ( Pool* inp )
{
	inp->Delete( this );
}

void StashResource::retain ( )
{
	SU_ASSERT( stash );
	if( refcnt==0 )	stash->retain();
	refcnt++;
}

void StashResource::release ( )
{
	SU_ASSERT( stash );
	SU_ASSERT( refcnt );
	if( refcnt==1 )	stash->release();
	refcnt--;
}

bool StashResource::isempty ( )
{
	return (loadedbs == 0);
}

bool StashResource::isloading ( )
{
	return (loadedbs>0 && loadedbs<contentbs);
}

bool StashResource::isloaded ( )
{
	return (loadedbs == contentbs);
}






//
// StashList
//


StashList* StashList::create ( Pool* instashpool, Pool* inp )
{
	SU_REQUIRE_OR_NULL( instashpool );

	StashList* ps = inp->New<StashList>( 0 );
	ps->pool = instashpool;
	ps->prev = NULL;
	ps->next = NULL;
	ps->refcnt = 0;
	ps->first = NULL;
	return ps;
}


void StashList::recycle ( Pool* inp )
{
	inp->Delete( this );
}


void StashList::retain ( )
{
	refcnt++;
}


void StashList::release ( )
{
	SU_ASSERT( refcnt );
	refcnt--;
}


StashResource* StashList::allocRsc ( uint inbsize )
{
	StashResource* r = StashResource::create( inbsize, pool );
	SU_REQUIRE_OR_NULL( r );
	r->stash = this;
	list_insert_front( first, r );
	return r;
}


StashResource* StashList::findRsc ( uint32 inRID )
{
	SU_REQUIRE_OR_NULL( inRID );
	for( StashResource* i=first ; i ; i=i->next )
		if( i->rid==inRID )	return i;
	return NULL;
}


bool StashList::squeeze_one ( )
{
	for( StashResource* i=first ; i ; i=i->next )
	{
		if( i->refcnt==0 )
		{
			list_remove( first, i );
			i->recycle( pool );
			return true;
		}
	}

	return false;
}


void StashList::squeeze ( )
{
	while( squeeze_one() );
}





//
// StashCloud
//


StashCloud::StashCloud ( )
{
	first = NULL;
}


void StashCloud::clear ( Pool* inp )
{
	list_recycle_all( first, inp );
}


StashList* StashCloud::newStash ( Pool* inStashPool, Pool* inp )
{
	StashList* s = StashList::create( inStashPool, inp );
	SU_REQUIRE_OR_NULL( s );
	list_insert_front( first, s );
	return s;
}


bool StashCloud::freeStash ( StashList* instash, Pool* inp )
{
	SU_REQUIRE_OR_FALSE( instash );
	SU_REQUIRE_OR_FALSE( inp );

	if( instash->refcnt > 0 )
		return false;	// busy

	list_remove( first, instash );
	instash->recycle( inp );
	return true;
}


StashResource* StashCloud::findRsc ( uint32 inRID )
{
	StashResource* r = NULL;
	for( StashList* i=first ; i && !r ; i=i->next )
		r = i->findRsc(inRID);
	return r;
}


void StashCloud::squeeze ( )
{
	for( StashList* i=first ; i ; i=i->next )
		i->squeeze();
}



