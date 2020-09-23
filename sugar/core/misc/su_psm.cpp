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



namespace
{

	typedef byte* (*convert_func) ( byte*, uint* );

	struct convert_psm
	{
		Psm				psm;
		convert_func	func;
	};




	byte* dec_A8R8G8B8 ( byte* p, uint* c )
	{
		uint32 x = *((uint32*)p);
		c[0] = (x>>16)&0xff;
		c[1] = (x>>8 )&0xff;
		c[2] = (x>>0 )&0xff;
		c[3] = (x>>24)&0xff;
		return p+4;
	}

	byte* enc_A8R8G8B8 ( byte* p, uint* c )
	{
		uint32 x = (c[3]<<24) | (c[0]<<16) | (c[1]<<8) | (c[2]<<0);
		*((uint32*)p) = x;
		return p+4;
	}

	byte* dec_A8B8G8R8 ( byte* p, uint* c )
	{
		uint32 x = *((uint32*)p);
		c[0] = (x>>0 )&0xff;
		c[1] = (x>>8 )&0xff;
		c[2] = (x>>16)&0xff;
		c[3] = (x>>24)&0xff;
		return p+4;
	}

	byte* enc_A8B8G8R8 ( byte* p, uint* c )
	{
		uint32 x = (c[3]<<24) | (c[2]<<16) | (c[1]<<8) | (c[0]<<0);
		*((uint32*)p) = x;
		return p+4;
	}

	byte* dec_R8G8B8A8 ( byte* p, uint* c )
	{
		uint32 x = *((uint32*)p);
		c[0] = (x>>24)&0xff;
		c[1] = (x>>16)&0xff;
		c[2] = (x>>8 )&0xff;
		c[3] = (x>>0 )&0xff;
		return p+4;
	}

	byte* enc_R8G8B8A8 ( byte* p, uint* c )
	{
		uint32 x = (c[0]<<24) | (c[1]<<16) | (c[2]<<8) | (c[3]<<0);
		*((uint32*)p) = x;
		return p+4;
	}

	byte* dec_B8G8R8A8 ( byte* p, uint* c )
	{
		uint32 x = *((uint32*)p);
		c[0] = (x>>8 )&0xff;
		c[1] = (x>>16)&0xff;
		c[2] = (x>>24)&0xff;
		c[3] = (x>>0 )&0xff;
		return p+4;
	}

	byte* enc_B8G8R8A8 ( byte* p, uint* c )
	{
		uint32 x = (c[2]<<24) | (c[1]<<16) | (c[0]<<8) | (c[3]<<0);
		*((uint32*)p) = x;
		return p+4;
	}

	byte* dec_R8G8B8 ( byte* p, uint* c )
	{
		#ifdef SU_HW_LSB
			c[0] = p[2];
			c[1] = p[1];
			c[2] = p[0];
		#else
			c[0] = p[0];
			c[1] = p[1];
			c[2] = p[2];
		#endif
			c[3] = 255;
			return p+3;
	}

	byte* enc_R8G8B8 ( byte* p, uint* c )
	{
		#ifdef SU_HW_LSB
			p[0] = c[2];
			p[1] = c[1];
			p[2] = c[0];
		#else
			p[0] = c[0];
			p[1] = c[1];
			p[2] = c[2];
		#endif
			return p+3;
	}

	byte* dec_B8G8R8 ( byte* p, uint* c )
	{
		#ifdef SU_HW_LSB
			c[0] = p[0];
			c[1] = p[1];
			c[2] = p[2];
		#else
			c[0] = p[2];
			c[1] = p[1];
			c[2] = p[0];
		#endif
			c[3] = 255;
			return p+3;
	}

	byte* enc_B8G8R8 ( byte* p, uint* c )
	{
		#ifdef SU_HW_LSB
			p[0] = c[0];
			p[1] = c[1];
			p[2] = c[2];
		#else
			p[0] = c[2];
			p[1] = c[1];
			p[2] = c[0];
		#endif
			return p+3;
	}





	convert_psm decA[] =
	{
		{	PSM_A8R8G8B8,		dec_A8R8G8B8	},
		{	PSM_A8B8G8R8,		dec_A8B8G8R8	},
		{	PSM_R8G8B8A8,		dec_R8G8B8A8	},
		{	PSM_B8G8R8A8,		dec_B8G8R8A8	},
		{	PSM_R8G8B8,			dec_R8G8B8		},
		{	PSM_B8G8R8,			dec_B8G8R8		},
	};

	convert_psm encA[] =
	{
		{	PSM_A8R8G8B8,		enc_A8R8G8B8	},
		{	PSM_A8B8G8R8,		enc_A8B8G8R8	},
		{	PSM_R8G8B8A8,		enc_R8G8B8A8	},
		{	PSM_B8G8R8A8,		enc_B8G8R8A8	},
		{	PSM_R8G8B8,			enc_R8G8B8		},
		{	PSM_B8G8R8,			enc_B8G8R8		},
	};

}



uint su::GetBPP	(	Psm		inPsm	)
{
	switch( inPsm )
	{
	case PSM_R8_G8_B8 :
	case PSM_B8_G8_R8 :
		return 24;

	case PSM_A8_R8_G8_B8:
	case PSM_A8_B8_G8_R8:
	case PSM_R8_G8_B8_A8:
	case PSM_B8_G8_R8_A8:
		return 32;

	case PSM_R5G6B5 :
	case PSM_B5G6R5 :
	case PSM_A1R5G5B5 :
	case PSM_A1B5G5R5 :
	case PSM_A4R4G4B4 :
	case PSM_A4B4G4R4 :
	case PSM_R5G5B5A1 :
	case PSM_B5G5R5A1 :
	case PSM_R4G4B4A4 :
	case PSM_B4G4R4A4 :
	case PSM_A8L8 :
	case PSM_L8A8 :
		return 16;

	case PSM_L4 :
		return 4;
			
	case PSM_L8 :
	case PSM_A4L4 :
		return 8;

	case PSM_A16B16G16R16 :
		return 64;
	}
	return 0;
}

bool su::ConvertPixels	(	byte*&		outp,
							Psm			outpsm,
							byte*&		inp,
							Psm			inpsm,
							uint		incount		)
{
	SU_REQUIRE_OR_FALSE( outp );
	SU_REQUIRE_OR_FALSE( inp );
	SU_RETURN_TRUE_IF( outpsm==inpsm );
	SU_RETURN_TRUE_IF( incount==0 );

	convert_psm* dec = NULL;
	convert_psm* enc = NULL;

	for( uint i=0 ; i<sizeof(decA)/sizeof(convert_psm) ; i++ )
		if( decA[i].psm==inpsm )
			dec = &decA[i];

	for( uint i=0 ; i<sizeof(encA)/sizeof(convert_psm) ; i++ )
		if( encA[i].psm==outpsm )
			enc = &encA[i];

	SU_ASSERT( dec );		// else must add the function
	SU_ASSERT( enc );		// else must add the function

	if( enc && dec )
	{
		register byte* ip = inp;
		register byte* op = outp;
		register uint c[4];

		while( incount-- )
		{
			ip = dec->func( ip, c );
			op = enc->func( op, c );
		}

		outp = op;
		inp  = ip;
		return true;
	}

	return false;
}


