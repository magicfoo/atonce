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


#include <sufw/precompiled/supch_fw.h>
#include "su_mswfreader_structs.h"


bool su::swfRECT::read ( MCursor& mc )
{
	mc.align(8);

	uint nbits = mc.uread(5);
	SU_ASSERT( nbits );

	const float fromtwip = 1.f / 20.0f;
	xmin = mc.sread( nbits ) * fromtwip;
	xmax = mc.sread( nbits ) * fromtwip;
	ymin = mc.sread( nbits ) * fromtwip;
	ymax = mc.sread( nbits ) * fromtwip;

	mc.align(8);
	return true;
}



bool su::swfHEADER::read ( MCursor& mc )
{
	util::Zero( *this );

	mc.seek(0);

	sig[0]     = mc.readlsb<uint8>();
	sig[1]     = mc.readlsb<uint8>();
	sig[2]     = mc.readlsb<uint8>();
	version    = mc.readlsb<uint8>();
	filelength = mc.readlsb<uint32>();

	SU_REQUIRE_OR_FALSE( sig[0]=='F' || sig[0]=='C' );
	SU_REQUIRE_OR_FALSE( sig[1]=='W' );
	SU_REQUIRE_OR_FALSE( sig[2]=='S' );

	if( sig[0]=='F' )
	{
		// extract full header
		mc.seek( 8*8 );
		framesize.read( mc );
		framerate  = float( mc.readlsb<uint16>() ) / 256.0;
		framecount = mc.readlsb<uint16>();

		mc.align(8);
		firsttag   = mc.pos();
		lasttag    = filelength * 8;

		SU_ASSERT( lasttag <= mc.size() );
	}

	return true;
}



bool su::swfFrameLabelTag::read ( MCursor& mc, uint64 begin, uint64 end )
{
	mc.seek( begin );

	mc.readstring( label );

	if( mc.pos()<end )
		isanchor = mc.readlsb<uint8>() == 1;
	else
		isanchor =false;

	SU_ASSERT( mc.pos()<=end );
	return ( mc.pos()<=end );
}


