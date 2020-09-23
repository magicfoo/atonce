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
#include <sufw/su_uids.h>
using namespace su;



#define BLOC_BSIZE						(64*1024)
#define RM_AUTO_THR						(64*1024)
#define AUTO_COST_THR					(1024)
#define AUTO_ALLOC_COST_FCT(pool,inbs)	(inbs)




namespace
{

	struct memnode;
	struct mempool;
	struct membloc;

	template <typename T> membloc*	alloc_bloc			(					);
	template <typename T> T*		alloc_unit			( T*& iofirst		);
	template <typename T> void		chain_unit			( T*& iofirst, T* t	);
	template <typename T> void		doublechain_unit	( T*& iofirst, T* t	);


	membloc*		bloc_l	 = NULL;
	mempool*		pool_l   = NULL;


	struct membloc
	{
		void*		addr;
		uint		bsize;
		uint		count;
		void*		first;
		void*		last;
		membloc*	next;
	};


	struct memnode
	{
		mempool*	pool;
		memnode*	next;
	};


	struct mempool : public Pool
	{
		Pool::RecyclingMode	rmode;
		IAllocator*			ator;
		pcstr				tag;

		mempool*			par;
		mempool*			prev;			// sibbling
		mempool*			next;			// sibbling
		mempool*			sub_l;			// sub pool

		memnode*			node_l;			// node list
		uint				node_u;			// node in use (not freed) in the list

		union
		{
			mempool*		auto_pool;		// cur pool
			uint			auto_cost;		// cost scalar for the node list
		};


		void init ( mempool* inparent, IAllocator* inator, Pool::RecyclingMode inrmode )
		{
			SU_ASSERT( inator );
			ator = inator;
			ator->Retain();

			rmode		= inrmode;
			tag			= NULL;
			par			= inparent;
			prev		= NULL;
			next		= NULL;
			sub_l		= NULL;
			node_l		= NULL;
			node_u		= 0;
			auto_cost	= 0;
			auto_pool	= NULL;
		}


		mempool* create_sub ( IAllocator* inator, Pool::RecyclingMode inrmode )
		{
			mempool* p = alloc_unit<mempool>( pool_l );
			SU_ASSERT( p );
			SU_REQUIRE_OR_NULL( p );

			if( !inator )
				inator = ator;
			SU_ASSERT( inator );

			p->init( this, inator, inrmode );
			doublechain_unit( sub_l, p );

			return p;
		}


		Pool* CreateSub ( )
		{
			RecyclingMode rm = rmode;
			if( rm == Pool::RM_Immediate )
				rm = Pool::RM_Retained;
			return create_sub( ator, rm );
		}


		Pool* CreateSub ( Pool::RecyclingMode inrmode )
		{
			return create_sub( ator, inrmode );
		}


		void Clear ( )
		{
			Recycle();

			// free subs
			while( sub_l )
			{
				sub_l->Release();
			}

			// free allocs
			SU_ASSERT( ator );
			memnode *n, *nn;
			n = node_l;
			while( n )
			{
				nn = n->next;
				ator->Free( n );
				n = nn;
			}

			node_l		= NULL;
			node_u		= 0;
			auto_cost	= 0;
			auto_pool	= NULL;
		}


		void Release ( )
		{
			Clear();

			if( par )
			{
				if( prev )
					prev->next = next;

				if( next )
					next->prev = prev;

				if( par->sub_l==this )
					par->sub_l = next;

				par = NULL;
			}

			if( ator )
			{
				ator->Release();
				ator = NULL;
			}

			chain_unit( pool_l, this );
		}


		IAllocator* GetAllocator ( )
		{
			SU_ASSERT( ator );
			return ator;
		}


		Pool* GetParent ( )
		{
			return par;
		}


		bool IsAncestorOf ( Pool* inChild )
		{
			SU_REQUIRE_OR_FALSE( inChild );

			Pool* p = inChild->GetParent();
			return (p==this) || IsAncestorOf(p);
		}


		void* Alloc ( uint inBSize )
		{
			SU_REQUIRE_OR_NULL( inBSize );

			if( rmode == Pool::RM_Immediate )
			{
				return ator->Alloc( inBSize );
			}

			else if( rmode == Pool::RM_Retained )
			{
				memnode* n = (memnode*) ator->Alloc( inBSize+sizeof(memnode) );
				SU_REQUIRE_OR_NULL( n );
				SU_ASSERT_A32( n );

				chain_unit( node_l, n );
				n->pool = this;
				node_u++;

				return (n+1);
			}

			else if( rmode == Pool::RM_Auto )
			{
				if( !auto_pool || auto_pool->auto_cost > AUTO_COST_THR )
					auto_pool = create_sub( ator, Pool::RM_Retained );
				SU_REQUIRE_OR_NULL( auto_pool );

				void* p = auto_pool->Alloc( inBSize );
				SU_REQUIRE_OR_NULL( p );

				auto_pool->auto_cost += AUTO_ALLOC_COST_FCT(auto_pool,inBSize);

				return p;
			}

			return NULL;
		}


		void Free ( void* inAddr )
		{
			SU_REQUIRE_OR_RETURN( inAddr );

			if( rmode == Pool::RM_Immediate )
			{
				SU_ASSERT( ator );
				ator->Free( inAddr );
			}

			else if( rmode == Pool::RM_Retained )
			{
				SU_ASSERT_A32( inAddr );
				memnode* n = ((memnode*)inAddr)-1;
				SU_ASSERT_A32( n );

				SU_ASSERT( n->pool == this );
				n->pool = NULL;

				SU_ASSERT( node_u > 0 );
				node_u--;

				if( node_u==0 && auto_cost )
				{
					SU_ASSERT( par );
					SU_ASSERT( par->rmode == Pool::RM_Auto );
					if( par->auto_pool == this )
						par->auto_pool = NULL;
					Release();
				}
			}

			else if( rmode == Pool::RM_Auto )
			{
				SU_ASSERT_A32( inAddr );
				memnode* n = ((memnode*)inAddr)-1;
				SU_ASSERT_A32( n );

				SU_ASSERT( n->pool );
				SU_ASSERT( n->pool != this );
				SU_ASSERT( n->pool->par );
				SU_ASSERT( n->pool->par == this );

				n->pool->Free( inAddr );
			}
		}


		void* AllocZero ( uint inBSize )
		{
			void* p = Alloc( inBSize );
			if( p )	util::Zero( p, inBSize );
			return p;
		}


		void* AllocMore ( void* inOldAddr, uint inOldBSize, uint inNewBSize )
		{
			SU_ASSERT( inNewBSize >= inOldBSize );
			if( inOldAddr == NULL )
			{
				SU_ASSERT( inOldBSize == 0 );
				if( inNewBSize == 0 )
				{
					return NULL;
				}
				else
				{
					return Alloc( inNewBSize );
				}
			}
			else
			{
				if( inNewBSize == inOldBSize )
				{
					return inOldAddr;
				}
				else
				{
					void* p = Alloc( inNewBSize );
					if( p )
						util::Memcopy( p, inOldAddr, inOldBSize );
					return p;
				}
			}
		}


		void* AllocLess ( void* inOldAddr, uint inOldBSize, uint inNewBSize )
		{
			SU_ASSERT( inNewBSize <= inOldBSize );
			SU_ASSERT( inNewBSize > 0 );
			if( inOldAddr == NULL )
			{
				SU_ASSERT( inOldBSize == 0 );
				if( inNewBSize == 0 )
				{
					return NULL;
				}
				else
				{
					return Alloc( inNewBSize );
				}
			}
			else
			{
				if( inNewBSize == inOldBSize )
				{
					return inOldAddr;
				}
				else
				{
					void* p = Alloc( inNewBSize );
					if( p )
						util::Memcopy( p, inOldAddr, inNewBSize );
					return p;
				}
			}
		}


		void* DumpMemory ( void* inAddr, uint inBSize, uint inBSupply )
		{
			uint bs = inBSupply + inBSize;
			if( bs )
			{
				void* a = Alloc( bs );
				if( a ) 
				{
					util::Memcopy( a, inAddr, inBSize );
					return a;
				}
			}
			return NULL;
		}


		void Recycle ( )
		{
			if( rmode == Pool::RM_Retained )
			{
				SU_ASSERT( ator );

				memnode* n;
				memnode* nn;

				n = node_l;

				node_l = NULL;
				node_u = 0;

				while( n )
				{
					nn = n->next;
					if( n->pool )
					{
						// in use
						chain_unit( node_l, n );
						node_u++;
					}
					else
					{
						// freed
						SU_ASSERT( ator );
						ator->Free( n );
					}
					n = nn;
				}
			}
		}


		void RecycleDeep ( )
		{
			Recycle();

			mempool* s = sub_l;
			while( s )
			{
				s->RecycleDeep();
				s = s->next;
			}
		}


		void SetTag ( pcstr inTag )
		{
			tag = inTag;
		}

	};



	mempool		rootpool;



	template < typename T >
	membloc* alloc_bloc ( )
	{
		byte *mlo, *mhi;

		mlo = (byte*) rootpool.Alloc( BLOC_BSIZE );
		SU_REQUIRE_OR_NULL( mlo );
		mhi = mlo + BLOC_BSIZE;
		
		membloc* b;
		b = (membloc*) mlo;
		b->addr  = (void*)mlo;
		b->bsize = BLOC_BSIZE;
		b->count = (BLOC_BSIZE - sizeof(membloc)) / sizeof(T);
		b->first = (void*)( b+1 );
		b->last  = (void*)( (T*)b->first + b->count );
		b->next  = bloc_l;
		bloc_l    = b;

		SU_ASSERT( (byte*)(b->last) <= mhi );
		return b;
	}
	
	
	template < typename T >
	T* alloc_unit ( T*& iofirst )
	{
		if( !iofirst )
		{
			membloc* b = alloc_bloc<T>();
			SU_ASSERT( b );
			
			bool needcdtor;
			needcdtor = support_yes_or_undefined< has_user_cdtor<T> >::value;
			
			T *t0, *t1, *t;
			t0 = (T*) b->first;
			t1 = (T*) b->last;
			for( t=t0 ; t<t1 ; t++ )
			{
				if( needcdtor )
					SU_CC_CALL_CTOR( t );
				t->next = t+1;
			}
			
			(t1-1)->next = iofirst;
			iofirst = t0;
		}
		
		T* t = iofirst;
		iofirst = t->next;
		return t;
	}


	template <typename T>
	void chain_unit ( T*& iofirst, T* t )
	{
		SU_ASSERT( t );
		t->next = iofirst;
		iofirst = t;
	}


	template <typename T>
	void doublechain_unit ( T*& iofirst, T* t )
	{
		SU_ASSERT( t );
		t->prev = NULL;
		t->next = iofirst;
		if( iofirst )	iofirst->prev = t;
		iofirst = t;
	}

}



su::Pool*
su::Pool::Create ( IAllocator* inator, RecyclingMode inrmode )
{
	Pool* p = rootpool.create_sub( inator, inrmode );
	SU_REQUIRE_OR_NULL( p );

	return p;
}


void
su::Pool::Destroy ( Pool*& ioPool )
{
	SU_REQUIRE_OR_RETURN( ioPool );
	ioPool->Release();
	ioPool = NULL;
}


su::Pool*
su::Pool::Root ( )
{
	return &rootpool;
}


void
su::Pool::RecycleAll ( )
{
	rootpool.RecycleDeep();
}










#ifdef __cplusplus
extern "C" 
{
#endif


void* su_langc_malloc ( unsigned int s )
{
	SU_REQUIRE_OR_NULL( globals::syspool );
	return globals::syspool->Alloc( s );
}


void su_langc_free ( void* a )
{
	SU_REQUIRE_OR_RETURN( a );
	SU_REQUIRE_OR_RETURN( globals::syspool );
	return globals::syspool->Free( a );
}


void* su_langc_realloc ( void* a, unsigned int b )
{
	if( !b ) {
		su_langc_free( a );
		return NULL;
	}

	byte* p = (byte*) su_langc_malloc( b );
	if( !a )
		return p;

	for( uint i=0 ; i<b ; i++ )
		p[i] = ((byte*)a)[i];

	su_langc_free( a );
	return p;
}


#ifdef __cplusplus
}
#endif







SU_BEGIN_STARTER( CorePool )


void OnStage ( Stage stage )
{
	if( stage == STAGE_INITIALIZE )
	{
		SU_COMPILE_TIME_ASSERT( sizeof(memnode)==8 );

		// init the root allocator

		IAllocator* ator = factory::CreateObject<IAllocator>( uid::MAllocatorPool, NULL );
		if( ator )
		{
			rootpool.init( NULL, ator, Pool::RM_Immediate );
			SafeRelease( ator );
		}

		// create a system pool

		if( !globals::syspool )
		{
			globals::syspool = Pool::Create( NULL, Pool::RM_Immediate );
			SU_ASSERT( globals::syspool );
		}

		// create the obstack

		if( !globals::obspool && globals::obspoolbs )
		{
			IAllocatorObstack* obs;
			obs = factory::CreateObject<IAllocatorObstack>( uid::MAllocatorObstack, globals::syspool );
			SU_ASSERT( obs );

			void* obs_mem = globals::syspool->Alloc( globals::obspoolbs );
			SU_ASSERT( obs_mem );

			bool rlz = obs->Realize( obs_mem, globals::obspoolbs );
			SU_ASSERT( rlz );

			globals::obspool = Pool::Create( obs, Pool::RM_Immediate );
			SU_ASSERT( globals::obspool );
		}

		// create the authoring pool

		if( !globals::authpool && globals::authmode>0 )
		{
			globals::authpool = Pool::Create( NULL, Pool::RM_Immediate );
			SU_ASSERT( globals::authpool );
		}
	}
	else if( stage == STAGE_SHUT )
	{
		SafeRelease( globals::obspool );
		SafeRelease( globals::syspool );

		rootpool.Release();
	}
}


SU_END_STARTER( CorePool )


