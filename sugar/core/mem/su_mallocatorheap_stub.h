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



using namespace su;




namespace
{

	struct SIDL_STUB_CLASSNAME
	{

		byte*		memlow;
		byte*		memhi;
		byte*		memtop;


		bool Init ( Pool* p, AnyList* o )
		{
			memlow  = NULL;
			memhi   = NULL;
			memtop  = NULL;
			return true;
		}


		void Shut ( )
		{
			Unrealize();
		}


		bool Realize ( void* inMemAddr, uint inMemBSize)
		{
			SU_REQUIRE_OR_FALSE( !IsRealized() );
			SU_REQUIRE_OR_FALSE( inMemAddr );
			SU_REQUIRE_OR_FALSE( inMemBSize>16 );

			memlow  = (byte*) inMemAddr;
			memhi   = memlow + inMemBSize;
			memtop  = memlow;
			return true;
		}


		void Unrealize ( )
		{
			SU_REQUIRE_OR_RETURN( IsRealized() );
			memlow = NULL;
			memhi  = NULL;
			memtop = NULL;
		}


		bool IsRealized ( )
		{
			return (memlow != NULL);
		}


		void * AllocInPlace ( ::su::pointer inBAddr, ::uint inBSize)
		{
			return NULL;
		}


		void * Alloc ( uint inBSize )
		{
			SU_REQUIRE_OR_NULL( inBSize );
			SU_REQUIRE_OR_NULL( IsRealized() );

			byte *m0, *m1;
			m0 = util::Round4( memtop );
			m1 = m0 + inBSize;

			SU_REQUIRE_OR_NULL( m0 < memhi );
			SU_REQUIRE_OR_NULL( m1 <= memhi );

			memtop = m1;
			return m0;
		}


		void Free ( void * inPointer)
		{
			// no effect
		}


		bool Clear ( )
		{
			SU_REQUIRE_OR_FALSE( IsRealized() );
			memtop = memlow;
			return true;
		}


	};


}


