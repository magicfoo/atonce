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




struct PrefetchResource
{
	uint32					rid;
	PakDesc					desc;
	StashResource*			rsc;
};



struct PrefetchList
{
	PrefetchList*			next;
	PrefetchList*			prev;

	StashList*				stash;
	PrefetchResource*		rlist;
	uint					rcount;
	uint32					rtoload;
	uint32					rloaded;

	IRscManager::PrefetchStatus	status;

	static PrefetchList*	create			( uint32* inList, uint32 inCount, Pool* inp );
	void					recycle			( Pool* inp );

	bool					lock			( StashList*, StashCloud*, PakCloud* );
	bool					lock_prepare	( StashCloud*, PakCloud* );
	void					lock_sort_byids	( );
	void					lock_sort_byofs	( );
	void					unlock			( );
};



struct PrefetchQueue
{
	PrefetchList*			first;

							PrefetchQueue	( );
	void					clear			( Pool* inp );

	PrefetchList*			enqueue			( uint32* inList, uint32 inCount, Pool* );
	bool					dequeue			( PrefetchList*, Pool* );		// pl is recycled !

	PrefetchList*			oldest_pending	( );
};



struct PrefetchLoader
{
	byte*					bufaddr;
	uint					bufbs;

	PrefetchList*			curr_pl;
	PrefetchResource*		curr_rsc;
	PakNode*				curr_node;
	TransferParams			curr_bunch;
	int						curr_stage;		// start-loading(0), waiting(1), stashing(2)

							PrefetchLoader	( );

	void					setbuffer		( byte*, uint );

	void					load_start		( PrefetchList* );
	void					load_update		( );

	PrefetchList*			getloading		( );

	void					abort			( );
};







//
// PrefetchList
//


PrefetchList* PrefetchList::create ( uint32* inList, uint32 inCount, Pool* inp )
{
	SU_ASSERT( inp );
	SU_REQUIRE_OR_NULL( inList );
	SU_REQUIRE_OR_NULL( inCount );

	uint supplybs = inCount * sizeof(PrefetchResource);

	PrefetchList* pl = inp->New<PrefetchList>( supplybs );
	SU_REQUIRE_OR_NULL( pl );

	pl->next = NULL;
	pl->prev = NULL;
	pl->stash = NULL;
	pl->rlist = (PrefetchResource*)(pl+1);
	pl->rcount = inCount;
	pl->rtoload = 0;
	pl->rloaded = 0;
	pl->status = IRscManager::PS_PENDING;

	for( uint i=0 ; i<inCount ; i++ )
	{
		SU_ASSERT( inList[i] > 0 );
		util::Zero( pl->rlist[i] );
		pl->rlist[i].rid = inList[i];
	}

	return pl;
}


void PrefetchList::recycle ( Pool* inp )
{
	inp->Delete( this );
}


bool PrefetchList::lock ( StashList* instash, StashCloud* scloud, PakCloud* pcloud )
{
	SU_ASSERT( !stash );
	SU_ASSERT( instash );
	SU_ASSERT( scloud );
	SU_ASSERT( pcloud );

	stash = instash;
	stash->retain();

	lock_sort_byids();

	if( lock_prepare(scloud,pcloud) )
	{
		lock_sort_byofs();
		return true;
	}

	// failed to prepare !
	unlock();
	return false;
}


void PrefetchList::lock_sort_byids ( )
{
	// sort rsc according to their ids
	// descending sort, from highest to lowest
	// duplicated rid is changed to rid 0 and will bubbled to the end

	uint zcount = 0;

	bool again = true;
	while( again )
	{
		again = false;
		for( uint i=1 ; i<rcount ; i++ )
		{
			PrefetchResource& r0 = rlist[i-1];
			PrefetchResource& r1 = rlist[i];
			if( (r0.rid == r1.rid) && (r0.rid > 0) )
			{
				r1.rid = 0;
				zcount++;
				again = true;
			}
			else if( r1.rid > r0.rid )
			{
				util::Swap( r0.rid, r1.rid );
				again = true;
			}
		}
	}

	// remove zero'ing desc bubbled at tail
	SU_ASSERT( zcount < rcount );
	rcount -= zcount;
}


bool PrefetchList::lock_prepare	( StashCloud* scloud, PakCloud* pcloud )
{
	for( uint i=0 ; i<rcount ; i++ )
	{
		PrefetchResource& r = rlist[i]; 

		// checks if the rsc is already fully stashed or in progress ?
		r.rsc = scloud->findRsc( r.rid );

		// load the pak-desc ?
		if( !r.rsc || !r.rsc->isloaded() )
		{
			if( !pcloud->findRsc(r.rid,r.desc) )
				return false;	// failed !
			SU_ASSERT( r.desc.node );
			r.desc.node->retain();
		}

		// stash the rsc
		if( !r.rsc )
		{
			r.rsc = stash->allocRsc( r.desc.contentbs );
			if( !r.rsc )
				return false;	// failed !
			r.rsc->rid = r.desc.rid;
			r.rsc->tid = r.desc.tid;
		}

		r.rsc->retain();
		rtoload += r.rsc->contentbs;
		rloaded += r.rsc->loadedbs;
	}

	return true;
}


void PrefetchList::lock_sort_byofs ( )
{
	// sort rsc according to this order :
	// - no pak desc (fully stashed rsc)
	// - by pak-node address
	// - by pak-offset

	bool again = true;
	while( again )
	{
		again = false;
		for( uint i=1 ; i<rcount ; i++ )
		{
			PrefetchResource& r0 = rlist[i-1];
			PrefetchResource& r1 = rlist[i];
			if( (r1.desc.node < r0.desc.node) || ((r1.desc.node == r0.desc.node) && (r1.desc.dataso < r0.desc.dataso)) )
			{
				util::Swap( r0, r1 );
				again = true;
			}
		}
	}
}


void PrefetchList::unlock ( )
{
	if( stash )
	{
		for( uint i=0 ; i<rcount ; i++ )
		{
			PrefetchResource& r = rlist[i];
			if( r.desc.node )	r.desc.node->release();
			if( r.rsc )			r.rsc->release();
			util::Zero( r.desc );
			util::Zero( r.rsc );
		}

		stash->squeeze();
		stash->release();
		stash = NULL;
	}
}






//
// PrefetchQueue
//


PrefetchQueue::PrefetchQueue ( )
{
	first = NULL;
}


void PrefetchQueue::clear ( Pool* inp )
{
	list_recycle_all( first, inp );
}


PrefetchList* PrefetchQueue::enqueue ( uint32* inlist, uint32 incount, Pool* inp )
{
	SU_REQUIRE_OR_NULL( inlist );
	SU_REQUIRE_OR_NULL( incount );
	SU_ASSERT( inp );

	// check the rid-list
	for( uint i=0 ; i<incount ; i++ )
		if( inlist[i]==0 )
			return NULL;

	PrefetchList* pl = PrefetchList::create( inlist, incount, inp );
	SU_REQUIRE_OR_NULL( pl );

	list_insert_front( first, pl );
	return pl;
}


bool PrefetchQueue::dequeue ( PrefetchList* inpl, Pool* inp )
{
	SU_ASSERT( inp );
	SU_REQUIRE_OR_FALSE( inpl );

	inpl->unlock();

	list_remove( first, inpl );
	inpl->recycle( inp );
	return true;
}


PrefetchList* PrefetchQueue::oldest_pending	( )
{
	PrefetchList* o = NULL;
	for( PrefetchList* i=first ; i ; i=i->next )
		if( i->status==IRscManager::PS_PENDING )
			o = i;
	return o;
}





//
// PrefetchLoader
//


PrefetchLoader::PrefetchLoader ( )
{
	util::Zero( *this );
}


void PrefetchLoader::setbuffer ( byte* inaddr, uint inbs )
{
	bufaddr = inaddr;
	bufbs = inbs;
}


PrefetchList* PrefetchLoader::getloading ( )
{
	return curr_pl;
}


void PrefetchLoader::load_start ( PrefetchList* inpl )
{
	SU_ASSERT( inpl );
	SU_ASSERT( inpl->status == IRscManager::PS_PENDING );
	SU_ASSERT( curr_pl == NULL );

	inpl->status = IRscManager::PS_LOADING;
	curr_pl = inpl;
	curr_rsc = curr_pl->rlist;
	curr_stage = 0;

	load_update();
}


void PrefetchLoader::load_update ( )
{
	SU_REQUIRE_OR_RETURN( curr_pl );
	SU_ASSERT( curr_pl->status == IRscManager::PS_LOADING );

	PrefetchResource* r = curr_rsc;
	PrefetchResource* rend = curr_pl->rlist + curr_pl->rcount;

	if( curr_stage == 0 )
	{
		// stage 0: select a next bunch of bytes to load

		for( ;; )
		{
			if( r==rend )
			{
				curr_pl->status = IRscManager::PS_STASHED;
				curr_pl = NULL;
				return;
			}
			else if( r->rsc->isloaded() )
			{
				r++;
			}
			else
			{
				curr_node = r->desc.node;
				SU_ASSERT( curr_node );
				TransferParams inp, outp;
				inp.baddr = bufaddr;
				inp.bsize = Min( (r->rsc->contentbs - r->rsc->loadedbs), bufbs );
				inp.boffset = r->desc.dataso + r->rsc->loadedbs;
				curr_node->startTransfer(inp,outp);
				SU_ASSERT( outp.boffset <= inp.boffset );
				SU_ASSERT( outp.baddr >= inp.baddr );
				SU_ASSERT( outp.bsize <= inp.bsize );
				curr_bunch = outp;
				curr_rsc = r;
				curr_stage = 1;
				return;
			}
		}
	}

	else if( curr_stage == 1 )
	{
		// stage 1: wait the async transfer to complete

		SU_ASSERT( curr_node );
		TransferState ts = curr_node->probeTransfer();
		if( ts==TSFR_COMPLETED )
		{
			curr_node = NULL;
			curr_stage = 2;
			return;
		}
		else if( ts==TSFR_FAILED )
		{
			curr_node = NULL;
			curr_pl->status = IRscManager::PS_LOAD_FAILED;
			curr_pl = NULL;
			return;
		}
	}
	else if( curr_stage == 2 )
	{
		// stage 2: stash the just transfered bunch of bytes

		uint  bo = (r->desc.dataso+r->rsc->loadedbs) - curr_bunch.boffset;
		uint  bs = curr_bunch.bsize - bo;
		byte* ba = curr_bunch.baddr + bo;
		byte* bp = r->rsc->contentaddr + r->rsc->loadedbs;
		util::Memcopy( bp, ba, bs );
		r->rsc->loadedbs += bs;
		SU_ASSERT( r->rsc->loadedbs <= r->rsc->contentbs );
		curr_pl->rloaded += bs;
		SU_ASSERT( curr_pl->rloaded <= curr_pl->rtoload );
		curr_stage = 0;
	}
}


void PrefetchLoader::abort ( )
{
	SU_REQUIRE_OR_RETURN( curr_pl );
	SU_ASSERT( curr_pl->status == IRscManager::PS_LOADING );

	if( curr_node )
		curr_node->abortTransfer();

	curr_pl->status = IRscManager::PS_ABORTED;
	curr_pl = NULL;
}




