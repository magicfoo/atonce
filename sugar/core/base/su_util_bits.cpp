/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2009 AtOnce Technologies
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
using namespace su;





void su::util::ub_write ( byte* p, int s, int n, uchar i )
{
	SU_REQUIRE_OR_RETURN( p );
	SU_REQUIRE_OR_RETURN( n>0 );
	SU_ASSERT( s>=0 );
	SU_ASSERT( n<=8 );

	p += (s>>3);
	s &= 7;

	register uint u;
	u  = p[0]<<8;
	u |= p[1]<<0;

	register uint v=i;
	v <<= (16-n-s);

	register uint m=(1<<n)-1;
	m <<= (16-n-s);

	u = u^((u^v)&m); 

	p[0] = (u>>8);
	p[1] = (u>>0);
}


void su::util::sb_write ( byte* p, int s, int n, char i )
{
	ub_write( p, s, n, i );
}


uchar su::util::ub_read ( byte* p, int s, int n )
{
	SU_REQUIRE_OR_0( p );
	SU_REQUIRE_OR_0( n>0 );
	SU_ASSERT( s>=0 );
	SU_ASSERT( n<=8 );

	p += (s>>3);
	s &= 7;

	register uint u;
	u  = p[0]<<8;
	u |= p[1]<<0;

	u >>= (16-n-s);
	u  &= (1<<n)-1;
	return u;
}


char su::util::sb_read ( byte* p, int s, int n )
{
	SU_REQUIRE_OR_0( p );
	SU_REQUIRE_OR_0( n>0 );
	SU_ASSERT( s>=0 );
	SU_ASSERT( n<=8 );

	uint u = ub_read(p,s,n);
	if( (u<<1) & (1<<n) )
		u |= -1L<<n;
	return u;
}


int su::util::ub_align ( byte* p, int s, int a )
{
	SU_ASSERT( p );
	SU_ASSERT( s>=0 );
	SU_ASSERT( a>=0 );

	// a is the number of low-bits to zeroed in the targeted address
	uint p0 = uint(p) + (s>>3);
	uint s0 = s&7;
	uint p1 = p0 + (s0?1:0);
	if( a>0 )
	{
		uint m = (1<<a)-1;
		uint o = (p1&m);
		if( o )	p1 += (m+1)-o;
	}
	return ((p1-p0)<<3)-s0;
}


void su::util::ub_copy ( byte* to_p, int to_s, byte* from_p, int from_s, int n )
{
	SU_ASSERT( to_p );
	SU_ASSERT( from_p );
	SU_ASSERT( to_s>=0 );
	SU_ASSERT( from_s>=0 );
	SU_REQUIRE_OR_RETURN( n>0 );

	int k; uint v;
	while( n )
	{
		k = Min(n,8);
		v = ub_read( from_p, from_s, k );
		ub_write( to_p, to_s, k, v );
		from_s += k;
		to_s   += k;
		n -= k;
	}
}


void su::util::ub_zero ( byte* p, int s, int n )
{
	SU_ASSERT( p );
	SU_ASSERT( s>=0 );
	SU_REQUIRE_OR_RETURN( n>0 );

	int k;
	while( n )
	{
		k = Min(n,8);
		ub_write( p, s, k, 0U );
		s += k;
		n -= k;
	}
}




void util::ui_write ( byte* p, int s, int n, uint i )
{
	SU_ASSERT( p );
	SU_ASSERT( s>=0 );
	SU_REQUIRE_OR_RETURN( n>0 );
	SU_ASSERT( n<=32 );

	int r=n&7;

	if( r==0 )
	{
		#ifdef SU_HW_LSB
		ub_copy(p,s,(byte*)&i,0,n);
		#else
		ub_copy(p,s,(byte*)&i,32-n,n);
		#endif
	}
	else if( n>24 )
	{
		ub_write( p, s, r, i>>24 ); s+=r;
		ub_write( p, s, 8, i>>16 ); s+=8;
		ub_write( p, s, 8, i>>8  ); s+=8;
		ub_write( p, s, 8, i>>0  );
	}
	else if( n>16 )
	{
		ub_write( p, s, r, i>>16 ); s+=r;
		ub_write( p, s, 8, i>>8  ); s+=8;
		ub_write( p, s, 8, i>>0  );
	}
	else if( n>8 )
	{
		ub_write( p, s, r, i>>8 ); s+=r;
		ub_write( p, s, 8, i>>0  );
	}
	else
	{
		ub_write( p, s, r, i );
	}
}


void su::util::si_write ( byte* p, int s, int n, int i )
{
	ui_write( p, s, n, i );
}


uint su::util::ui_read ( byte* p, int s, int n )
{
	SU_ASSERT( p );
	SU_ASSERT( s>=0 );
	SU_REQUIRE_OR_0( n>0 );
	SU_ASSERT( n<=32 );

	uint i=0;
	int r=n&7;

	if( r==0 )
	{
		#ifdef SU_HW_LSB
		ub_copy((byte*)&i,0,p,s,n);
		#else
		ub_copy((byte*)&i,32-n,p,s,n);
		#endif
	}
	else if( n>24 )
	{
		i  = ub_read( p, s, r ) << 24; s+=r;
		i |= ub_read( p, s, 8 ) << 16; s+=8;
		i |= ub_read( p, s, 8 ) << 8;  s+=8;
		i |= ub_read( p, s, 8 ) << 0;
	}
	else if( n>16 )
	{
		i  = ub_read( p, s, r ) << 16; s+=r;
		i |= ub_read( p, s, 8 ) << 8;  s+=8;
		i |= ub_read( p, s, 8 ) << 0;
	}
	else if( n>8 )
	{
		i  = ub_read( p, s, r ) << 8;  s+=r;
		i |= ub_read( p, s, 8 ) << 0;
	}
	else
	{
		i = ub_read( p, s, r );
	}

	return i;
}


int su::util::si_read ( byte* p, int s, int n )
{
	uint u = ui_read(p,s,n);
	if( (u<<1) & (1<<n) )
		u |= -1L<<n;
	return u;
}



