/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2009 AtOnce Technologies
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



using namespace su;




namespace
{

	struct SIDL_STUB_CLASSNAME
	{

		void**	chk_freelist;
		void**	chk_freehead;
		uint	chk_bs;
		uint	chk_max;
		byte*	chk_addr_low;
		byte*	chk_addr_high;


		bool Init ( Pool* p, AnyList* o )
		{
			chk_freelist = NULL;
			return true;
		}


		void Shut ( )
		{
			Unrealize();
		}


		bool IsRealized ( )
		{
			return (chk_freelist != NULL);
		}


		bool Realize (  void*	inMemAddr,
						uint	inMemBSize,
						uint	inChunkBSize	)
		{
			SU_REQUIRE_OR_FALSE( !IsRealized() );
			SU_REQUIRE_OR_FALSE( inMemAddr );
			SU_REQUIRE_OR_FALSE( (uint(inMemAddr)%4)==0 );
			SU_REQUIRE_OR_FALSE( inMemBSize>=16 );
			SU_REQUIRE_OR_FALSE( inChunkBSize>=4 );
			SU_REQUIRE_OR_FALSE( (inChunkBSize%4)==0 );

			int n = inMemBSize / (inChunkBSize+4);
			if( n <= 0 )
				return false;

			chk_freelist	= (void**) inMemAddr;
			chk_bs			= inChunkBSize;
			chk_max			= n;
			chk_addr_low	= (byte*)inMemAddr + n*4;
			chk_addr_high	= chk_addr_low     + n*chk_bs;
			SU_ASSERT( chk_addr_high <= ((byte*)inMemAddr+inMemBSize) )

			Clear();

			return true;
		}


		bool Clear ( )
		{
			SU_REQUIRE_OR_FALSE( IsRealized() );

			chk_freehead = chk_freelist;

			byte* chk = chk_addr_high;
			while( chk >= chk_addr_low )
			{
				*chk_freehead++ = chk;
				chk -= chk_bs;
			}

			return true;
		}


		uint CountChunk ( )
		{
			SU_REQUIRE_OR_RETURN_V( IsRealized(), 0 );
			return chk_max;
		}


		int GetChunkId ( void* inAddr )
		{
			SU_REQUIRE_OR_RETURN_V( IsRealized(), -1 );
			SU_REQUIRE_OR_RETURN_V( inAddr, -1 );

			byte* p = (byte*) inAddr;
			if( p>=chk_addr_low && p<chk_addr_high )
			{
				int cid = (p-chk_addr_low) / chk_bs;
				SU_ASSERT( cid < (int)chk_max );
				return cid;
			}
			else
			{
				return -1;
			}
		}


		int GetChunkOffset ( void* inAddr )
		{
			SU_REQUIRE_OR_RETURN_V( IsRealized(), -1 );
			SU_REQUIRE_OR_RETURN_V( inAddr, -1 );

			byte* p = (byte*) inAddr;
			if( p>=chk_addr_low && p<chk_addr_high )
				return (p-chk_addr_low) % chk_bs;
			else
				return -1;
		}


		void* GetChunkAddr ( uint inIndex )
		{
			SU_REQUIRE_OR_NULL( IsRealized() );
			SU_REQUIRE_OR_NULL( inIndex<chk_max );

			byte* a = chk_addr_low + inIndex*chk_bs;
			SU_ASSERT( a >= chk_addr_low );
			SU_ASSERT( a <  chk_addr_high );
			return (void*)a;
		}


		void Unrealize ( )
		{
			chk_freelist = NULL;
		}


		void* AllocInPlace ( void* inBAddr, uint inBSize )
		{
			return NULL;
		}


		void* Alloc ( uint inBSize )
		{
			SU_REQUIRE_OR_NULL( IsRealized() );
			SU_REQUIRE_OR_NULL( inBSize>0 );
			SU_REQUIRE_OR_NULL( inBSize<=chk_bs );

			if( chk_freehead == chk_freelist )
				return NULL;

			void* p = *--chk_freehead;
			SU_ASSERT( GetChunkOffset(p)==0 );

			return p;
		}


		void Free ( void* inPointer )
		{
			SU_REQUIRE_OR_RETURN( IsRealized() );
			SU_REQUIRE_OR_RETURN( inPointer );

			SU_ASSERT( GetChunkOffset(inPointer)==0 );
			SU_ASSERT( (byte*)chk_freehead < chk_addr_low );
			*chk_freehead++ = inPointer;
		}

	};


}


