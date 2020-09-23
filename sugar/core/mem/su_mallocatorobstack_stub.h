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

		struct blockh
		{
			blockh*	prevbh;
			uint	bsize;
		};


		byte*		memlow;
		byte*		memhi;
		blockh*		lastbh;
		bool		fatal;



		bool Init ( Pool* p, AnyList* o )
		{
			memlow  = NULL;
			memhi   = NULL;
			lastbh  = NULL;
			return true;
		}


		void Shut ( )
		{
			Unrealize();
		}


		bool IsRealized ( )
		{
			return (memlow != NULL);
		}


		bool Realize ( void* inMemAddr, uint inMemBSize )
		{
			SU_REQUIRE_OR_FALSE( !IsRealized() );
			SU_REQUIRE_OR_FALSE( inMemAddr );
			SU_REQUIRE_OR_FALSE( inMemBSize>sizeof(blockh) );

			memlow  = (byte*) inMemAddr;
			memhi   = memlow + inMemBSize;
			lastbh  = NULL;
			fatal	= true;

			return true;
		}


		void Unrealize (  )
		{
			SU_REQUIRE_OR_RETURN( IsRealized() );
			memlow  = NULL;
			memhi   = NULL;
			lastbh  = NULL;
		}


		void* AllocInPlace ( void* inBAddr, uint inBSize )
		{
			return NULL;
		}


		void* Alloc ( uint inBSize )
		{
			SU_REQUIRE_OR_NULL( inBSize );
			SU_REQUIRE_OR_NULL( IsRealized() );

			byte* mlo;
			mlo = util::Round4( lastbh ? ((byte*)(lastbh+1))+lastbh->bsize : (byte*)memlow );

			// full ?
			byte* mhi;
			mhi = mlo + sizeof(blockh) + inBSize;
			if( mhi > memhi )
				return NULL;

			blockh* bh = (blockh*) mlo;
			bh->prevbh = lastbh;
			bh->bsize  = inBSize;

			lastbh = bh;

			return (bh+1);
		}


		void Free ( void* inPointer )
		{
			SU_REQUIRE_OR_RETURN( IsRealized() );
			SU_REQUIRE_OR_RETURN( inPointer );

			SU_ASSERT_IF( fatal, lastbh );
			if( lastbh )
			{
				void* m = (lastbh+1);

				SU_ASSERT_IF( fatal, inPointer==m );
				if( inPointer==m )
				{
					lastbh = lastbh->prevbh;
				}
			}
		}


		bool Clear (  )
		{
			SU_REQUIRE_OR_FALSE( IsRealized() );
			lastbh  = NULL;
			return true;
		}


		void* GetTop ( )
		{
			return lastbh;
		}


		bool SetTop ( void* top )
		{
			bool toped = false;

			blockh* b = lastbh;
			blockh* t = (blockh*)top;

			for( ;; )
			{
				if( b == t )
				{
					lastbh = t;
					toped = true;
					break;
				}
				else if( b )
				{
					b = b->prevbh;
				}
				else
				{
					break;
				}
			}

			SU_ASSERT_IF( fatal, toped );
			return toped;
		}


		void SetFatal ( bool inOnOff )
		{
			fatal = inOnOff;
		}

	};


}


