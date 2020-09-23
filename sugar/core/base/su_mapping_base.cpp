/*LIC-HDR*****************************************************************
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


#define MAX_INT_BITS(n)			(0x3fffffffl>>(32-n))
#define MAX_UINT_BITS(n)		(0xffffffffl>>(32-n))
#define MAX_INT_SIZEOF(v)		MAX_INT_BITS(sizeof(v)*8)
#define MAX_UINT_SIZEOF(v)		MAX_UINT_BITS(sizeof(v)*8)





// si_attr

int su::map::si_attr::setup( byte* _p, int _s, int _w, int _n )
{
	SU_ASSERT( _s >= 0 );
	SU_ASSERT( _n >= 0 );
	SU_ASSERT( _w >= 0 );
	SU_ASSERT( _s <= MAX_INT_SIZEOF(s) );
	SU_ASSERT( _w <= MAX_INT_SIZEOF(w) );
	SU_ASSERT( _n <= MAX_INT_SIZEOF(n) );
	if( _p && _w && _n )
	{
	 	p  = _p + (_s>>3);
		s  = _s & 7;
		w  = _w;
		n  = _n;
		ro = false;
		return _w*_n;
	}
	else
	{
		p = NULL;
		return 0;
	}
}


int su::map::si_attr::get ( int i ) const
{
	SU_REQUIRE_OR_0( p );
	SU_ASSERT( i >= 0 );
	SU_ASSERT( i <  n );
	return util::si_read( p, s+i*w, w );
}


void su::map::si_attr::set ( int v, int i )
{
	SU_ASSERT( !ro );
	SU_REQUIRE_OR_RETURN( p );
	SU_REQUIRE_OR_RETURN( i >= 0 );
	SU_REQUIRE_OR_RETURN( i <  n );
	SU_ASSERT( v <= MAX_INT_BITS(w) );			// v is too big to be stored
	util::si_write( p, s+i*w, w, v );
}


void su::map::si_attr::blit ( byte* sp, int sn )
{
	SU_ASSERT( !ro );
	SU_REQUIRE_OR_RETURN( p );
	SU_REQUIRE_OR_RETURN( !ro );
	SU_REQUIRE_OR_RETURN( sp );
	SU_REQUIRE_OR_RETURN( sn );
	SU_ASSERT( sn >= 0 );
	SU_ASSERT( sn <= w*n );
	util::ub_copy( p, s, sp, 0, sn );
}


su::map::si_attr::operator int ( ) const
{
	return get();
}




// ui_attr

int su::map::ui_attr::setup( byte* _p, int _s, int _w, int _n )
{
	SU_ASSERT( _s >= 0 );
	SU_ASSERT( _n >= 0 );
	SU_ASSERT( _w >= 0 );
	SU_ASSERT( _s <= MAX_INT_SIZEOF(s) );
	SU_ASSERT( _w <= MAX_INT_SIZEOF(w) );
	SU_ASSERT( _n <= MAX_INT_SIZEOF(n) );
	if( _p && _w && _n )
	{
		p  = _p + (_s>>3);
		s  = _s & 7;
		w  = _w;
		n  = _n;
		ro = false;
		return _w*_n;
	}
	else
	{
		p = NULL;
		return 0;
	}
}


uint su::map::ui_attr::get ( int i ) const
{
	SU_REQUIRE_OR_0( p );
	SU_ASSERT( i >= 0 );
	SU_ASSERT( i <  n );
	return util::ui_read( p, s+i*w, w );
}


void su::map::ui_attr::set ( uint v, int i )
{
	SU_ASSERT( !ro );
	SU_REQUIRE_OR_RETURN( p );
	SU_REQUIRE_OR_RETURN( i >= 0 );
	SU_REQUIRE_OR_RETURN( i <  n );
	SU_ASSERT( v <= MAX_UINT_BITS(w) );		// v is too big to be stored
	util::ui_write( p, s+i*w, w, v );
}


void su::map::ui_attr::blit ( byte* sp, int sn )
{
	SU_ASSERT( !ro );
	SU_REQUIRE_OR_RETURN( p );
	SU_REQUIRE_OR_RETURN( sp );
	SU_REQUIRE_OR_RETURN( sn );
	SU_ASSERT( sn >= 0 );
	SU_ASSERT( sn <= w*n );
	util::ub_copy( p, s, sp, 0, sn );
}


su::map::ui_attr::operator uint ( ) const
{
	return get();
}






// fp_attr

int su::map::fp_attr::setup( byte* _p, int _s, int _w, int _n )
{
	// only 32bits IEEE at the moment !
	SU_ASSERT( _w == 32 );
	SU_ASSERT( _s >= 0 );
	SU_ASSERT( _n >= 0 );
	SU_ASSERT( _w >= 0 );
	SU_ASSERT( _s <= MAX_INT_SIZEOF(s) );
	SU_ASSERT( _w <= MAX_INT_SIZEOF(w) );
	SU_ASSERT( _n <= MAX_INT_SIZEOF(n) );
	if( _p && _w && _n )
	{
		p  = _p + (_s>>3);
		s  = _s & 7;
		w  = _w;
		n  = _n;
		ro = false;
		return _w*_n;
	}
	else
	{
		p = NULL;
		return 0;
	}
}


float su::map::fp_attr::get ( int i ) const
{
	SU_REQUIRE_OR_0( p );
	SU_ASSERT( i >= 0 );
	SU_ASSERT( i <  n );

	union
	{
		uint32	ui;
		float	fp;
	};

	ui = util::ui_read( p, s+i*w, w );
	return fp;
}


void su::map::fp_attr::set ( float v, int i )
{
	SU_ASSERT( !ro );
	SU_REQUIRE_OR_RETURN( p );
	SU_REQUIRE_OR_RETURN( i >= 0 );
	SU_REQUIRE_OR_RETURN( i <  n );

	union
	{
		uint32	ui;
		float	fp;
	};

	fp = v;
	util::ui_write( p, s+i*w, w, ui );
}


void su::map::fp_attr::blit ( byte* sp, int sn )
{
	SU_ASSERT( !ro );
	SU_REQUIRE_OR_RETURN( p );
	SU_REQUIRE_OR_RETURN( sp );
	SU_REQUIRE_OR_RETURN( sn );
	SU_ASSERT( sn >= 0 );
	SU_ASSERT( sn <= w*n );
	util::ub_copy( p, s, sp, 0, sn );
}


su::map::fp_attr::operator float ( ) const
{
	return get();
}






// fx_attr

int su::map::fx_attr::setup( byte* _p, int _s, int _w, int _n )
{
	SU_ASSERT( _s >= 0 );
	SU_ASSERT( _n >= 0 );
	SU_ASSERT( _w >= 0 );
	SU_ASSERT( _s <= MAX_INT_SIZEOF(s) );
	SU_ASSERT( _w <= MAX_INT_SIZEOF(w) );
	SU_ASSERT( _n <= MAX_INT_SIZEOF(n) );
	if( _p && _w && _n )
	{
		p  = _p + (_s>>3);
		s  = _s & 7;
		w  = _w;
		n  = _n;
		ro = false;
		return _w*_n;
	}
	else
	{
		p = NULL;
		return 0;
	}
}


float su::map::fx_attr::get ( int i ) const
{
	SU_REQUIRE_OR_0( p );
	SU_ASSERT( i >= 0 );
	SU_ASSERT( i <  n );

	float q, x;

	q = 1.0f / float(1<<(w/2));
	x = util::si_read( p, s+i*w, w );

	return float(x)*q;
}


void su::map::fx_attr::set ( float v, int i )
{
	SU_ASSERT( !ro );
	SU_REQUIRE_OR_RETURN( p );
	SU_REQUIRE_OR_RETURN( i >= 0 );
	SU_REQUIRE_OR_RETURN( i <  n );

	float q, x;

	q = 1.0f / float(1<<(w/2));
	x = float(v) * q;

	util::si_write( p, s+i*w, w, int(x) );
}


void su::map::fx_attr::blit ( byte* sp, int sn )
{
	SU_ASSERT( !ro );
	SU_REQUIRE_OR_RETURN( p );
	SU_REQUIRE_OR_RETURN( sp );
	SU_REQUIRE_OR_RETURN( sn );
	SU_ASSERT( sn >= 0 );
	SU_ASSERT( sn <= w*n );
	util::ub_copy( p, s, sp, 0, sn );
}


su::map::fx_attr::operator float ( ) const
{
	return get();
}









// the base-tag

int su::map::base_tag::getcode ( byte* p )
{
	base_tag t;
	SU_REQUIRE_OR_0( p );
	SU_REQUIRE_OR_0( t.load(p) );
	return t.code.get();
}

byte* su::map::base_tag::getnext ( byte* p )
{
	base_tag t;
	SU_REQUIRE_OR_NULL( p );
	SU_REQUIRE_OR_NULL( t.load(p) );
	return p+t.bsize();
}

int su::map::base_tag::create ( byte* p, bool forcelong )
{
	int s=0;
	s+=code.setup(p,s,10,1);
	s+=slen.setup(p,s,6,1);
	slen.set(forcelong?0x3f:0);
	if( slen.get()==0x3f ) s+=llen.setup(p,s,32,1);
	return s;
}

int su::map::base_tag::load ( byte* p )
{
	int s=0;
	s+=code.setup(p,s,10,1); 
	s+=slen.setup(p,s,6,1); 
	if( slen.get()==0x3f ) s+=llen.setup(p,s,32,1);
	return s;
}

byte* su::map::base_tag::addr ( )
{
	return code.p;
}

uint su::map::base_tag::bsize ( )
{
	uint bs = slen.get();
	if( bs==0x3f ) bs=4+llen.get();
	return 2+bs;
}







// the base-map

int su::map::base_map::create ( byte* p, uint inalign )
{
	SU_ASSERT( util::RoundX(p,(1<<inalign)) == p );
	int s=0;
	s+=csign.setup(p,s,32,1);
	s+=fsign.setup(p,s,32,1);
	s+=flen.setup(p,s,32,1);
	align = inalign;
	return s;
}


int su::map::base_map::load ( byte* p, uint inalign )
{
	int s=0;
	s+=csign.setup(p,s,32,1);
	s+=fsign.setup(p,s,32,1);
	s+=flen.setup(p,s,32,1);
	align = inalign;
	return s;
}


byte* su::map::base_map::addr ( )
{
	return csign.p;
}


uint su::map::base_map::bsize ( )
{
	return flen.get();
}


byte* su::map::base_map::data_begin ( )
{
	return addr() ? addr()+12 : NULL;
}


byte* su::map::base_map::data_end ( )
{
	return addr() ? addr()+bsize() : NULL;
}


bool su::map::base_map::data_check_addr ( byte* pstart, byte* pend )
{
	SU_ASSERT( pend >= pstart );
	byte *b, *e;
	b = data_begin();
	e = data_end();
	SU_REQUIRE_OR_FALSE( pstart>=b && pstart<e );
	SU_REQUIRE_OR_FALSE( pend>=b && pend<=e );
	return true;
}


int su::map::base_map::tag_code ( )
{
	return tag.code.get();
}


bool su::map::base_map::tag_isfirst ( )
{
	return tag.addr() == data_begin();
}


bool su::map::base_map::tag_islast ( )
{
	return tag.addr()+tag.bsize() >= data_end();
}


byte* su::map::base_map::tag_addr ( )
{
	return tag.addr();
}


uint su::map::base_map::tag_bsize ( )
{
	return tag.bsize();
}


int su::map::base_map::tag_seek ( byte* t )
{
	base_tag g;
	byte *t0, *t1;
	t0 = util::RoundX( t, (1<<align) );
	SU_REQUIRE_OR_0( data_check_addr(t0,t0+2) );
	g.load( t0 );
	t1 = g.addr() + g.bsize();
	SU_REQUIRE_OR_0( data_check_addr(t0,t1) );
	tag = g;
	return tag_code();
}


bool su::map::base_map::tag_seek ( int code )
{
	int co = tag_first();
	while( co )
	{
		if( co == code )
			return true;
		co = tag_next();
	}
	return false;
}


int su::map::base_map::tag_first ( )
{
	return tag_seek( data_begin() );
}


int su::map::base_map::tag_next ( )
{
	byte* p = tag.addr() + tag.bsize();
	return tag_seek( p );
}


