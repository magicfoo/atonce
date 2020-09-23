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
using namespace su;




su::Color32::Color32	(	)
{
	//
}


su::Color32::Color32	(	uint32 inRGBA		)
{
	r = (inRGBA >> 24) & 0XFF;
	g = (inRGBA >> 16) & 0XFF;
	b = (inRGBA >> 8)  & 0XFF;
	a = (inRGBA >> 0)  & 0XFF;
}

su::Color32::Color32	(	uint8 inR, uint8 inG, uint8 inB, uint8 inA		)
{
	r = inR;
	g = inG;
	b = inB;
	a = inA;
}

su::Color32::Color32	(	const Color32 &		inref	)
{
	r = inref.r;
	g = inref.g;
	b = inref.b;
	a = inref.a;
}

su::Color32::Color32	(	const Color64 &		inref	)
{
	r = inref.r >> 8;
	g = inref.g >> 8;
	b = inref.b >> 8;
	a = inref.a >> 8;
}

su::Color32::Color32	(	const Color &		inref	)
{
	r = Clamp(inref.r,0.f,1.f) * 255.f;
	g = Clamp(inref.g,0.f,1.f) * 255.f;
	b = Clamp(inref.b,0.f,1.f) * 255.f;
	a = Clamp(inref.a,0.f,1.f) * 255.f;
}

su::Color32
su::Color32::operator *	(	const Color32 &		inref	) const
{
	Color32 c;
	c.r = ( int(r) * int(inref.r) ) >> 8;
	c.g = ( int(b) * int(inref.g) ) >> 8;
	c.b = ( int(g) * int(inref.b) ) >> 8;
	c.a = ( int(a) * int(inref.a) ) >> 8;
	return c;
}

uint32
su::Color32::getRGBA		(			) const
{
	uint32 rgba = 0;
	rgba |= uint32(r) << 24;
	rgba |= uint32(g) << 16;
	rgba |= uint32(b) << 8;
	rgba |= uint32(a) << 0;
	return rgba;
}

uint32
su::Color32::getABGR		(			) const
{
	uint32 abgr = 0;
	abgr |= uint32(a) << 24;
	abgr |= uint32(b) << 16;
	abgr |= uint32(g) << 8;
	abgr |= uint32(r) << 0;
	return abgr;
}

uint32
su::Color32::getARGB		(			) const
{
	uint32 argb = 0;
	argb |= uint32(a) << 24;
	argb |= uint32(r) << 16;
	argb |= uint32(g) << 8;
	argb |= uint32(b) << 0;
	return argb;
}






su::Color64::Color64	(		)
{
	//
}

su::Color64::Color64	(	uint64 inRGBA	)
{
	r = inRGBA >> 48;
	g = inRGBA >> 32;
	b = inRGBA >> 16;
	a = inRGBA >> 0;
}

su::Color64::Color64	(	uint16 inR, uint16 inG, uint16 inB, uint16 inA	)
{
	r = inR;
	g = inG;
	b = inB;
	a = inA;
}

su::Color64::Color64	(	const Color64&	inref	)
{
	r = inref.r;
	g = inref.g;
	b = inref.b;
	a = inref.a;
}

su::Color64::Color64	(	const Color32 &	inref	)
{
	r = uint16(inref.r) << 8;
	g = uint16(inref.g) << 8;
	b = uint16(inref.b) << 8;
	a = uint16(inref.a) << 8;
}

su::Color64::Color64	(	const Color &	inref	)
{
	r = Clamp(inref.r,0.f,1.f) * 65535.f;
	g = Clamp(inref.g,0.f,1.f) * 65535.f;
	b = Clamp(inref.b,0.f,1.f) * 65535.f;
	a = Clamp(inref.a,0.f,1.f) * 65535.f;
}

su::Color64
su::Color64::operator *	(	const Color64 &	inref	) const
{
	Color64 c;
	c.r = ( int(r) * int(inref.r) ) >> 8;
	c.g = ( int(b) * int(inref.g) ) >> 8;
	c.b = ( int(g) * int(inref.b) ) >> 8;
	c.a = ( int(a) * int(inref.a) ) >> 8;
	return c;
}

uint64
su::Color64::getRGBA	(			) const
{
	uint64 rgba = 0;
	rgba |= uint64(r) << 48;
	rgba |= uint64(g) << 32;
	rgba |= uint64(b) << 16;
	rgba |= uint64(a) << 0;
	return rgba;
}

uint64
su::Color64::getARGB	(			) const
{
	uint64 argb = 0;
	argb |= uint64(a) << 48;
	argb |= uint64(r) << 32;
	argb |= uint64(g) << 16;
	argb |= uint64(b) << 0;
	return argb;
}

uint64
su::Color64::getABGR	(			) const
{
	uint64 abgr = 0;
	abgr |= uint64(a) << 48;
	abgr |= uint64(b) << 32;
	abgr |= uint64(g) << 16;
	abgr |= uint64(r) << 0;
	return abgr;
}





su::Color::Color	(			)
{
	//
}

su::Color::Color	(	float inR, float inG, float inB, float inA	)
{
	r = Clamp( inR, 0.f, 1.f );
	g = Clamp( inG, 0.f, 1.f );
	b = Clamp( inB, 0.f, 1.f );
	a = Clamp( inA, 0.f, 1.f );
}

su::Color::Color	(	const Color &		inref		)
{
	r = inref.r;
	g = inref.g;
	b = inref.b;
	a = inref.a;
}

su::Color::Color	(	const Color32 &		inref		)
{
	float oos = 1.f / 255.f;
	r = float(inref.r) * oos;
	g = float(inref.g) * oos;
	b = float(inref.b) * oos;
	a = float(inref.a) * oos;
}

su::Color::Color	(	const Color64 &		inref		)
{
	float oos = 1.f / 65535.f;
	r = float(inref.r) * oos;
	g = float(inref.g) * oos;
	b = float(inref.b) * oos;
	a = float(inref.a) * oos;
}

su::Color::Color	(	const Vec4&		inref		)
{
	r = inref.x;
	g = inref.y;
	b = inref.z;
	a = inref.w;
}

su::Color::Color	(	const Vec3&		inref,		float inA		)
{
	r = inref.x;
	g = inref.y;
	b = inref.z;
	a = inA;
}


Vec4 su::Color::tovec4 ( ) const
{
	return Vec4( r, g, b, a );
}


su::Color su::Color::operator *	( const Color& inf ) const
{
	Color c;
	c.r = r * inf.r;
	c.g = g * inf.g;
	c.b = b * inf.b;
	c.a = a * inf.a;
	return c;
}


su::Color su::Color::operator *	( float inf ) const
{
	Color c;
	c.r = r * inf;
	c.g = g * inf;
	c.b = b * inf;
	c.a = a * inf;
	return c;
}


su::Color& su::Color::operator *= ( float inf )
{
	r *= inf;
	g *= inf;
	b *= inf;
	a *= inf;
	return *this;
}


su::Color su::Color::toclamp	(	float inMin, float inMax	) const
{
	return Color( Clamp(r,0.f,1.f), Clamp(g,0.f,1.f), Clamp(b,0.f,1.f), Clamp(a,0.f,1.f) );
}


su::Color & su::Color::clamp		(	float inMin, float inMax		)
{
	r = Clamp( r, 0.f, 1.f );
	g = Clamp( g, 0.f, 1.f );
	b = Clamp( b, 0.f, 1.f );
	a = Clamp( a, 0.f, 1.f );
	return *this;
}


uint32 su::Color::getRGBA ( ) const
{
	return Color32(*this).getRGBA();
}


uint32 su::Color::getARGB ( ) const
{
	return Color32(*this).getARGB();
}


uint32 su::Color::getABGR ( ) const
{
	return Color32(*this).getABGR();
}





su::ColorHSV::ColorHSV ( )
{
	//
}


su::ColorHSV::ColorHSV ( float inH, float inS, float inV )
{
	h = Clamp(inH,0.f,360.f);
	s = Clamp(inS,0.f,1.f);
	v = Clamp(inV,0.f,1.f);
	hsvToRgb();
}


su::ColorHSV::ColorHSV ( uint32 inRGBA )
{
	Color32 c32(inRGBA);
	Color c( c32 );
	r = c.r;
	g = c.g;
	b = c.b;
	rgbToHsv();
}


su::ColorHSV::ColorHSV ( const Color32& inrgb )
{
	Color c( inrgb );
	r = c.r;
	g = c.g;
	b = c.b;
	rgbToHsv();
}


uint32 su::ColorHSV::getRGBA ( ) const
{
	return Color(r,g,b).getRGBA();
}


uint32 su::ColorHSV::getARGB ( ) const
{
	return Color(r,g,b).getARGB();
}


uint32 su::ColorHSV::getABGR ( ) const
{
	return Color(r,g,b).getABGR();
}


su::ColorHSV su::ColorHSV::bright ( float f ) const
{
	return ColorHSV(h,s,v*f);
}


su::ColorHSV su::ColorHSV::operator * ( float f ) const
{
	return ColorHSV(h,s,v*f);
}


su::ColorHSV& su::ColorHSV::operator *= ( float f )
{
	v = Clamp(v*f,0.f,1.f);
	hsvToRgb();
	return *this;
}


void su::ColorHSV::rgbToHsv	( )
{
	float min, max, delta;

	min = Min( Min(r,g), b );
	max = Max( Max(r,g), b );
	v = max;

	delta = max - min;

	if( max != 0 )
	{
		s = delta / max;		// s
	}
	else
	{
		// r = g = b = 0		// s = 0, v is undefined
		s = 0;
		h = -1;
		return;
	}

	if( r == max )
		h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		h = 4 + ( r - g ) / delta;	// between magenta & cyan

	h *= 60;				// degrees
	while( h < 0 )
		h += 360;
}


void su::ColorHSV::hsvToRgb	( )
{
	int i;
	float f, p, q, t, hh;

	if( s == 0 )
	{
		// achromatic (grey)
		r = g = b = v;
		return;
	}

	hh = h / 60.f;		// sector 0 to 5
	i = Floor( hh );
	f = hh - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i )
	{
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:		// case 5:
			r = v;
			g = p;
			b = q;
			break;
	}
}



uint32 su::color_brightner ( uint32 inrgba, float f )
{
	ColorHSV hsv( inrgba );
	hsv *= f;
	return hsv.getRGBA();
}



uint32 su::color_lerp ( uint32 inrgba0, uint32 inrgba1, float f )
{
	f = Clamp( f, 0.f, 1.f );
	Color c0 = Color32(inrgba0);
	Color c1 = Color32(inrgba1);
	Color r;
	r.r = c0.r*f + c1.r*(1.f-f);
	r.g = c0.g*f + c1.g*(1.f-f);
	r.b = c0.b*f + c1.b*(1.f-f);
	r.a = c0.a*f + c1.a*(1.f-f);
	return r.getRGBA();
}





