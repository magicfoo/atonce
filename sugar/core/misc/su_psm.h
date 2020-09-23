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



#ifndef _su_psm_h_
#define _su_psm_h_




namespace su
{


	//
	// Pixel Storage Format

	enum Psm
	{
		PSM_UNKNOWN=0,

		// in memory storage

		PSM_R8_G8_B8,
		PSM_B8_G8_R8,
		PSM_A8_R8_G8_B8,
		PSM_A8_B8_G8_R8,
		PSM_R8_G8_B8_A8,
		PSM_B8_G8_R8_A8,


		// 16bpp

		PSM_R5G6B5,
		PSM_B5G6R5,
		PSM_A1R5G5B5,
		PSM_A1B5G5R5,
		PSM_A4R4G4B4,
		PSM_A4B4G4R4,
		PSM_R5G5B5A1,
		PSM_B5G5R5A1,
		PSM_R4G4B4A4,
		PSM_B4G4R4A4,

		// misc

		PSM_L4,
		PSM_L8,
		PSM_A4L4,
	    PSM_A8L8,
		PSM_A16B16G16R16,

		PSM_L8A8,


		// 32bpp

		#ifdef SU_HW_LSB
		PSM_A8R8G8B8 = PSM_B8_G8_R8_A8,
		PSM_A8B8G8R8 = PSM_R8_G8_B8_A8,
		PSM_R8G8B8A8 = PSM_A8_B8_G8_R8,
		PSM_B8G8R8A8 = PSM_A8_R8_G8_B8,
		#else
		PSM_A8R8G8B8 = PSM_A8_R8_G8_B8,
		PSM_A8B8G8R8 = PSM_A8_B8_G8_R8,
		PSM_R8G8B8A8 = PSM_R8_G8_B8_A8,
		PSM_B8G8R8A8 = PSM_B8_G8_R8_A8,
		#endif

		// 24bpp

		#ifdef SU_HW_LSB
		PSM_R8G8B8 = PSM_B8_G8_R8,
		PSM_B8G8R8 = PSM_R8_G8_B8,
		#else
		PSM_R8G8B8 = PSM_R8_G8_B8,
		PSM_B8G8R8 = PSM_B8_G8_R8,
		#endif
	};



	su_ie uint GetBPP			(	Psm			inPsm		);

	su_ie bool ConvertPixels	(	byte*&		outp,
									Psm			outpsm,
									byte*&		inp,
									Psm			inpsm,
									uint		incount		);


}




#endif // _su_psm_h_


