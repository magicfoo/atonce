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



#ifndef _su_colors_h_
#define _su_colors_h_




namespace su
{

	struct Color;
	struct Color32;
	struct Color64;
	struct ColorHLS;



	//
	// color in [0,255]^4

	struct_ie Color32
	{
		union {
			uint8		array[4];
			uint32		all;
			struct {
				uint8	a;
				uint8	b;
				uint8	g;
				uint8	r;
			};
		};

						Color32			(														);
						Color32			(	uint32 inRGBA										);
						Color32			(	uint8 inR, uint8 inG, uint8 inB, uint8 inA=0xFF		);
						Color32			(	const Color32 &										);
						Color32			(	const Color64 &										);
						Color32			(	const Color &										);
		Color32			operator *		(	const Color32 &										) const;
		uint32			getRGBA			(														) const;
		uint32			getARGB			(														) const;
		uint32			getABGR			(														) const;
	};



	//
	// color in [0,65535]^4

	struct_ie Color64
	{
		union {
			uint16		array[4];
			uint64		all;
			struct {
				uint16	a;
				uint16	b;
				uint16	g;
				uint16	r;
			};
		};

						Color64			(															);
						Color64			(	uint64 inRGBA											);
						Color64			(	uint16 inR, uint16 inG, uint16 inB, uint16 inA=0xFFFF	);
						Color64			(	const Color64 &											);
						Color64			(	const Color32 &											);
						Color64			(	const Color &											);
		Color64			operator *		(	const Color64 &											) const;
		uint64			getRGBA			(															) const;
		uint64			getARGB			(															) const;
		uint64			getABGR			(															) const;
	};



	//
	// color in [0,1]^4

	struct_ie Color
	{
		union {
			float		array[4];
			uint128		all;
			struct {
				float	a;
				float	b;
				float	g;
				float	r;
			};
		};

						Color			(														);
						Color			(	float inR, float inG, float inB, float inA=1.f		);
						Color			(	const Color &										);
						Color			(	const Color32 &										);
						Color			(	const Color64 &										);
						Color			(	const Vec4&											);
						Color			(	const Vec3&	, float inA=1.f							);
		Vec4			tovec4			(														) const;
		Color			operator *		(	const Color &										) const;
		Color			operator *		(	float												) const;
		Color&			operator *=		(	float												);
		Color			toclamp			(	float inMin = 0.0f, float inMax = 1.0f				) const;
		Color&			clamp			(	float inMin = 0.0f, float inMax = 1.0f				);
		uint32			getRGBA			(														) const;
		uint32			getARGB			(														) const;
		uint32			getABGR			(														) const;
	};



	//
	// HSV (or HSB)
	//
	// r,g,b values are from 0 to 1
	// h takes value over [0,360]
	// s takes value over [0,1]
	// v takes value over [0,1]
	// if s==0, then h=-1 (undefined)
	//
	// The coordinate system is cylindrical, and the colors are defined inside a hexcone.
	// The hue value H runs from 0 to 360º. The saturation S is the degree of strength or purity and is from 0 to 1.
	// Purity is how much white is added to the color, so S=1 makes the purest color (no white).
	// Brightness V also ranges from 0 to 1, where 0 is the black.

	struct_ie ColorHSV
	{
		float			r, g, b;	// rgb
		float			h, s, v;	// hsv

						ColorHSV		(														);
						ColorHSV		(	float inH, float inS, float inV						);
						ColorHSV		(	uint32 inRGBA										);
						ColorHSV		(	const Color32&										);
		void			rgbToHsv		(														);
		void			hsvToRgb		(														);
		ColorHSV		bright			(	float												) const;	// Brightness factor
		ColorHSV		operator *		(	float												) const;	// Brightness factor
		ColorHSV&		operator *=		(	float												);			// Brightness factor
		uint32			getRGBA			(														) const;
		uint32			getARGB			(														) const;
		uint32			getABGR			(														) const;
	};


	su_ie uint32		color_brightner	( uint32 inrgba, float f );
	su_ie uint32		color_lerp		( uint32 inrgba0, uint32 inrgba1, float f );			// c0*f + c1*(1-f)

}




#endif // _su_colors_h_


