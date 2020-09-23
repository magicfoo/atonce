/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2010 AtOnce Technologies
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
using namespace su;




su::MCursor::MCursor ( )
{
	clear();
}


su::MCursor::MCursor ( void* inbstart, uint inbs, uint64 inpos )
{
	setup( inbstart, inbs, inpos );
}


su::MCursor::MCursor ( const MCursor& inref, uint64 inpos )
{
	setup( inref, inpos );
}


void su::MCursor::setup ( void* inbstart, uint inbs, uint64 inpos )
{
	SU_ASSERT( inbstart );
	SU_ASSERT( inbs );

	bstart	= (byte*) inbstart;
	bend	= bstart + inbs;
	by		= bstart;

	seek( inpos );
}


void su::MCursor::setup ( const MCursor& inref, uint64 inpos )
{
	setup( inref.bstart, inref.bend-inref.bstart );
	seek( inpos );
}


void su::MCursor::clear ( )
{
	bstart	= NULL;
	bend	= NULL;
	by		= NULL;
}


uint64 su::MCursor::backward ( ) const
{
	SU_ASSERT( checkintegrity() );
	return (by.ptr-bstart)*8 + by.bit;
}


bool su::MCursor::hasbackward ( uint64 inbits ) const
{
	return backward() >= inbits;
}


void su::MCursor::gobackward ( uint64 inbits )
{
	SU_ASSERT( checkintegrity() );
	uint64 p = pos();
	SU_ASSERT( p >= inbits );
	seek( p-inbits );
}


void su::MCursor::goforward ( uint64 inbits )
{
	SU_ASSERT( checkintegrity() );
	uint64 p = pos();
	SU_ASSERT( inbits <= forward() );
	seek( p+inbits );
}



bool su::MCursor::hasforward ( uint64 inbits ) const
{
	return forward() >= inbits;
}


uint64 su::MCursor::forward ( ) const
{
	SU_ASSERT( checkintegrity() );
	uint64 sz = size();
	return (sz - backward());
}


uint64 su::MCursor::size ( ) const
{
	SU_ASSERT( checkintegrity() );
	uint64 sz = bend-bstart;
	return sz<<3;
}


uint64 su::MCursor::pos ( ) const
{
	return backward();
}


uint64 su::MCursor::seek ( uint64 inbits )
{
	SU_ASSERT( checkintegrity() );
	SU_ASSERT( inbits <= size() );
	uint64 p = pos();
	by.ptr = bstart + (inbits>>3);
	by.bit = (inbits&7);
	return p;
}


void su::MCursor::align ( uint inbits )
{
	SU_ASSERT( checkintegrity() );
	SU_REQUIRE_OR_RETURN( inbits>1 );
	uint64 m = pos() % inbits;
	if( m )
		goforward( inbits-m );
}


void su::MCursor::fill ( uint inbits )
{
	SU_ASSERT( checkintegrity() );
	SU_REQUIRE_OR_RETURN( inbits );

	byte* bp = by.ptr;
	int   bb = by.bit;
	int   bc = inbits;

	int   n;
	byte  m;

	while( bc )
	{
		n = Min(bc,8-bb);
		m = ((1<<n)-1)<<(8-n-bb);

		*bp++ &= ~m;

		bc -= n;
		bb = 0;
	}

	goforward( inbits );
}


uint su::MCursor::uread ( uint inbits )
{
	SU_ASSERT( checkintegrity() );
	SU_REQUIRE_OR_RETURN_V( inbits, 0 );
	SU_ASSERT( inbits <= 32 );

	byte* bp = by.ptr;
	int   bb = by.bit;
	int   bc = inbits;

	uint  u = 0;

	while( bc>0 )
	{
		u = (u<<8) | (*bp++);
		bc -= 8-bb;
		bb = 0;
	}

	u >>= (-bc);
	u  &= (1<<inbits)-1;

	goforward( inbits );
	return u;
}


int su::MCursor::sread ( uint inbits )
{
	SU_ASSERT( checkintegrity() );
	SU_REQUIRE_OR_RETURN_V( inbits, 0 );
	SU_ASSERT( inbits <= 32 );
	uint u = uread( inbits );
	if( u & (1<<(inbits-1)) )
		u |= -1L << inbits;
	return u;
}


void su::MCursor::write ( byte* insrc, uint inbits )
{
	SU_ASSERT( checkintegrity() );
	SU_REQUIRE_OR_RETURN( insrc );
	SU_REQUIRE_OR_RETURN( inbits );

	byte* bp = by.ptr;
	int   bb = by.bit;
	int   bc = inbits;

	uint  u  = 0;
	int   bu = 0;
	byte* bs = insrc;

	int   n;
	byte  m;

	while( bc )
	{
		if( bu < 8 )
		{
			u = (u<<8) | (*bs++);
			bu += 8;
		}

		n = Min(bc,8-bb);
		m = ((1<<n)-1)<<(8-n-bb);

		*bp++ = ((*bp)&(~m)) | ((u>>(bu-8+bb))&m);
		bc -= n;
		bu -= n;
		bb  = 0;
	}

	goforward( inbits );
}


void su::MCursor::write ( uint invalue, uint inbits )
{
	SU_ASSERT( checkintegrity() );
	SU_REQUIRE_OR_RETURN( inbits );
	SU_ASSERT( inbits <= 32 );

	uint v = invalue << (32-inbits);

	byte b[4];
	b[0] = v>>24;
	b[1] = v>>16;
	b[2] = v>>8;
	b[3] = v>>0;

	write( b, inbits );
}





uint su::MCursor::sizeB ( ) const
{
	SU_ASSERT( checkintegrity() );
	return size()>>3;
}


byte* su::MCursor::addrB ( ) const
{
	SU_ASSERT( checkintegrity() );
	return by.ptr;
}


byte* su::MCursor::addrB ( uint64 inbits )
{
	SU_ASSERT( checkintegrity() );
	uint64 p = pos();
	seek( inbits );
	byte* b = by.ptr;
	seek( p );
	return b;
}


uint64 su::MCursor::posB ( byte* b ) const
{
	SU_ASSERT( checkintegrity() );
	SU_ASSERT( b );
	SU_ASSERT( b >= bstart );
	SU_ASSERT( b <= bend );
	return uint64(b-bstart) << 3;
}


uint64 su::MCursor::seekB ( byte* b )
{
	SU_ASSERT( checkintegrity() );
	SU_ASSERT( b );
	SU_ASSERT( b >= bstart );
	SU_ASSERT( b <= bend );
	uint64 p = pos();
	by = b;
	return p;
}


uint su::MCursor::usedB ( ) const
{
	SU_ASSERT( checkintegrity() );
	return backward()>>3;
}


bool su::MCursor::usedB ( uint inbytes ) const
{
	SU_ASSERT( checkintegrity() );
	return usedB() >= inbytes;
}


uint su::MCursor::leftB ( ) const
{
	SU_ASSERT( checkintegrity() );
	return forward()>>3;
}


bool su::MCursor::leftB ( uint inbytes ) const
{
	SU_ASSERT( checkintegrity() );
	return leftB() >= inbytes;
}


void su::MCursor::readB ( byte* outdst, uint incount )
{
	SU_ASSERT( checkintegrity() );
	SU_REQUIRE_OR_RETURN( outdst );
	SU_REQUIRE_OR_RETURN( incount );

	align( 8 );
	SU_ASSERT( leftB(incount) );

	util::Memcopy( outdst, by.ptr, incount );

	goforward( incount<<3 );
}


void su::MCursor::writeB ( byte* insrc, uint incount )
{
	SU_ASSERT( checkintegrity() );
	SU_REQUIRE_OR_RETURN( insrc );
	SU_REQUIRE_OR_RETURN( incount );

	align( 8 );
	SU_ASSERT( leftB(incount) );

	util::Memcopy( by.ptr, insrc, incount );

	goforward( incount<<3 );
}


bool su::MCursor::findB ( uint64& outpos, byte inValue )
{
	SU_ASSERT( checkintegrity() );
	uint64 p0 = pos();

	align( 8 );
	uint fw = forward() >> 3;

	while( fw )
	{
		uint rm = Min( fw, 1024U );

		for( uint i=0 ; i<rm ; i++ )
		{
			if( by.ptr[i]==inValue )
			{
				outpos = pos() + i*8;
				seek( p0 );
				return true;
			}
		}

		fw -= rm;
	}

	seek( p0 );
	return false;
}


byte* su::MCursor::findB ( byte inValue )
{
	SU_ASSERT( checkintegrity() );
	uint64 p;
	if( findB(p,inValue) )
		return addrB(p);
	else
		return NULL;
}


bool su::MCursor::checkintegrity( ) const
{
	return ( by.ptr && by.ptr >= bstart && by.ptr <= bend );
}



