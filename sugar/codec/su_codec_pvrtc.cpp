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


#include <sufw/core/su_core.h>
#include <sufw/su_mapping.h>
#include "su_icodec.h"
#include "su_codec.h"

using namespace su;


#if defined( SU_HW_WINDOWS )

#include <PVRTexLib.h>
using namespace pvrtexlib;

bool	su::decompress_pvrtc	(	void*		ioRGBA32,
									byte*		inPVRTCData,
									uint		inWidth,
									uint		inHeight,
									bool		inAlpha,
									uint		inBpp		)			// 2 || 4
{
	SU_ASSERT( inBpp == 2 || inBpp == 4 );
	SU_REQUIRE_OR_FALSE( inBpp == 2 || inBpp == 4 );

	PixelType pt = (inBpp == 2) ? OGL_PVRTC2 : OGL_PVRTC4;
	try
	{
		PVRTextureUtilities* pvrUtil = PVRTextureUtilities::getPointer();

		CPVRTexture compTex(	inWidth,
								inHeight,
								0,				// MipMapCount,
								1,				// NumSurfaces,
								false,			// Border,
								false,			// Twiddled,
								false,			// CubeMap,
								false,			// Volume,
								false,			// FalseMips,
								inAlpha,
								false,			// VerticallyFlipped
								pt,
								0.0f,			// fNormalMap,
								(pvrtexlib::uint8*)inPVRTCData	);

		// create texture to decode
		CPVRTexture decompTex;

		pvrUtil->DecompressPVR( compTex, decompTex );
		
		CPVRTextureHeader& hd = decompTex.getHeader();
		CPVRTextureData& data = decompTex.getData();

		pvrtexlib::uint8* rgbaD = data.getData();
		SU_REQUIRE_OR_FALSE( rgbaD );

	//	uint hbs = hd.getFileHeaderSize();
		uint dbs = data.getDataSize();
		SU_ASSERT( dbs == inWidth*inHeight*4 );
		SU_REQUIRE_OR_FALSE( rgbaD );

		util::Memcopy( ioRGBA32, rgbaD, dbs );
		return true;
	}
	catch( PVRException& inExcept )
	{
		SU_STDOUT( "Could not decompress texture:\n" );
		SU_STDOUT( inExcept.what() );
	}
	return false;
}

#endif


