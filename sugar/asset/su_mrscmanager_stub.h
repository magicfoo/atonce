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



#include <sufw/asset/su_asset.h>
using namespace su;



#define	MAX_HANDLE		(128)




namespace
{

	#include "su_mrscmanager_pak.h"
	#include "su_mrscmanager_stash.h"
	#include "su_mrscmanager_plist.h"
	#include "su_mrscmanager_handle.h"




	struct SIDL_STUB_CLASSNAME
	{

		Pool*			pool;
		PakCloud		pakcloud;
		StashCloud		stashcloud;
		PrefetchQueue	prefetchq;
		PrefetchLoader	loader;
		HandleMan		hman;


		int				register_pak	( PakNode* o )			{ return hman.registerObj(o,1); }
		int				register_stash	( StashList* o )		{ return hman.registerObj(o,2); }
		int				register_plist	( PrefetchList* o )		{ return hman.registerObj(o,3); }
		PakNode*		retrieve_pak	( int i )				{ return (PakNode*)hman.asObj(i,1); }
		StashList*		retrieve_stash	( int i )				{ return (StashList*)hman.asObj(i,2); }
		PrefetchList*	retrieve_plist	( int i )				{ return (PrefetchList*)hman.asObj(i,3); }
		Time			gettime			(		)				{ Time t; clock::GetTime(&t); return t; }


		bool Init ( Pool* p, AnyList* o )
		{
			if( globals::rscmanbufferbs < 1024 )
				return false;

			pool = p->CreateSub();

			uint  bufbs   = globals::rscmanbufferbs;
			SU_ASSERT( bufbs );
			byte* bufaddr = (byte*) pool->Alloc( bufbs );
			SU_ASSERT( bufaddr );
			loader.setbuffer( bufaddr, bufbs );

			return true;
		}


		void Shut ( )
		{
			loader.abort();
			pakcloud.clear( pool );
			stashcloud.clear( pool );
			prefetchq.clear( pool );

			Pool::Destroy( pool );
		}


		int AttachPak ( pcstr inFilename )
		{
			PakNode* pkn = pakcloud.attachFile( inFilename, pool );
			if( pkn )
			{
				int h = register_pak( pkn );
				if( h>=0 )
					return h;
				pakcloud.detach( pkn, pool );
			}
			return -1;
		}


		bool DetachPak ( int inPakid )
		{
			PakNode* pkn = retrieve_pak( inPakid );
			SU_REQUIRE_OR_FALSE( pkn );

			if( !pakcloud.detach(pkn,pool) )
				return false;	// busy !

			hman.unregisterObj( inPakid );
			return true;
		}

	
		int CreateStash ( Pool* inPool )
		{
			StashList* s = stashcloud.newStash( inPool, pool );
			if( s )
			{
				int h = register_stash( s );
				if( h>=0 )
					return h;
				stashcloud.freeStash( s, pool );
			}
			return -1;
		}


		bool DestroyStash ( int inStashid )
		{
			StashList* s = retrieve_stash( inStashid );
			SU_REQUIRE_OR_FALSE( s );

			if( !stashcloud.freeStash(s,pool) )
				return false;	// busy !

			hman.unregisterObj( inStashid );
			return true;
		}

		
		bool SqueezeStash ( int inStashid )
		{
			StashList* s = retrieve_stash( inStashid );
			SU_REQUIRE_OR_FALSE( s );
			s->squeeze();
			return true;
		}


		int  EnqueuePrefetchList ( int inStashid, uint32* inList, uint32 inCount )
		{
			SU_REQUIRE_OR_NEG( inList );
			SU_REQUIRE_OR_NEG( inCount );

			StashList* s = retrieve_stash( inStashid );
			SU_REQUIRE_OR_NEG( s );

			PrefetchList* pl = prefetchq.enqueue( inList, inCount, pool );
			if( pl )
			{
				if( pl->lock(s,&stashcloud,&pakcloud) )
				{
					int h = register_plist( pl );
					if( h >= 0 )
						return h;
				}
				prefetchq.dequeue( pl, pool );
			}
			return -1;
		}


		IRscManager::PrefetchStatus ProbePrefetchList ( int inListid, uint32* outToLoadBS=NULL, uint32* outLoadedBS=NULL )
		{
			if( outToLoadBS )	*outToLoadBS = 0;
			if( outLoadedBS )	*outLoadedBS = 0;
		
			PrefetchList* pl = retrieve_plist( inListid );
			SU_REQUIRE_OR_RETURN_V( pl, IRscManager::PS_INVALID );
			
			if( outToLoadBS )	*outToLoadBS = pl->rtoload;
			if( outLoadedBS )	*outLoadedBS = pl->rloaded;
			return pl->status;
		}

		
		bool IsValidPrefetchList ( int inListid )
		{
			IRscManager::PrefetchStatus ps = ProbePrefetchList(inListid);
			return (ps != IRscManager::PS_INVALID);
		}


		bool IsPendingPrefetchList ( int inListid )
		{
			IRscManager::PrefetchStatus ps = ProbePrefetchList(inListid);
			return (ps == IRscManager::PS_PENDING);
		}


		bool IsBusyPrefetchList ( int inListid )
		{
			IRscManager::PrefetchStatus ps = ProbePrefetchList(inListid);
			return (ps>=IRscManager::PS_PENDING && ps<=IRscManager::PS_LOADING );
		}


		bool IsIdlePrefetchList ( int inListid )
		{
			return !IsBusyPrefetchList(inListid);
		}

		
		bool AbortPrefetchList ( int inListid )
		{
			if( IsBusyPrefetchList(inListid) )
			{
				PrefetchList* pl = retrieve_plist( inListid );
				SU_ASSERT( pl );
			
				if( loader.getloading() == pl )
					loader.abort();

				pl->status = IRscManager::PS_ABORTED;
				return true;
			}
			return false;
		}


		bool DequeuePrefetchList ( int inListid )
		{
			if( IsIdlePrefetchList(inListid) )
			{
				PrefetchList* pl = retrieve_plist( inListid );
				SU_ASSERT( pl );

				if( prefetchq.dequeue(pl,pool) )
				{
					hman.unregisterObj( inListid );
					return true;
				}
			}
			return false;
		}


		uint GetLoResource ( )
		{
			return pakcloud.getLoRID();
		}


		uint GetHiResource ( )
		{
			return pakcloud.getHiRID();
		}


		uint GetTypeResource ( uint32 inRID )
		{
			PakDesc rdesc;
			if( pakcloud.findRsc(inRID,rdesc) )
				return rdesc.tid;
			return 0;
		}

	
		IRscManager::ResourceStatus ProbeResource ( uint32 inRID )
		{
			StashResource* r = stashcloud.findRsc(inRID);
			if( r && r->isloaded() )
				return IRscManager::RS_STASHED;

			PakDesc rdesc;
			if( pakcloud.findRsc(inRID,rdesc) )
				return IRscManager::RS_PAKED;

			return IRscManager::RS_NOTFOUND;
		}


		void RetainResource ( uint32 inRID )
		{
			StashResource* r = stashcloud.findRsc( inRID );
			SU_REQUIRE_OR_RETURN( r );
			SU_REQUIRE_OR_RETURN( r->isloaded() );
			r->retain();
		}


		void ReleaseResource ( uint32 inRID )
		{
			StashResource* r = stashcloud.findRsc( inRID );
			SU_REQUIRE_OR_RETURN( r );
			SU_REQUIRE_OR_RETURN( r->isloaded() );
			r->release();
		}


		bool FetchResource ( uint32 inRID, IRscManager::Resource& outRsc )
		{
			StashResource* r = stashcloud.findRsc( inRID );
			SU_REQUIRE_OR_FALSE( r );
			SU_REQUIRE_OR_FALSE( r->isloaded() );
			outRsc.rid = r->rid;
			outRsc.tid = r->tid;
			outRsc.bytes = r->contentaddr;
			outRsc.bsize = r->contentbs;
			return true;
		}


		void Update ( )
		{
			if( loader.getloading() )
				loader.load_update();
			else
				loader.load_start( prefetchq.oldest_pending() );

			pool->RecycleDeep();
		}


		void Squeeze ( )
		{
			stashcloud.squeeze();
			pool->RecycleDeep();
		}
	};


}




