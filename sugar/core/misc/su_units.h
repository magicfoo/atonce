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



#ifndef _su_units_h_
#define _su_units_h_




namespace su
{

	enum Unit
	{
		U_Undefined = 0,

		// Time units

		TU_Day,					// 86400 s
		TU_Hour,				// 3600 s
		TU_Minute,				// 60 s
		TU_Second,				// 1 s
		TU_Millisecond,			// 1/1000 s
		TU_Movie,				// 24 frames per s
		TU_PALFrame,			// 25 frames per s (approx)
		TU_NTSCFrame,			// 30 frames per s (approx)
		TU_ShowScan,			// twice the speed of movie (48 frames per second)
		TU_PALField,			// twice the speed of PAL (50 frames per second)
		TU_NTSCField,			// twice the speed of NTSC (60 frames per second)
		TU_2FPS,				// 2 frames per second
		TU_3FPS,				// 3 frames per second
		TU_4FPS,				// ... frames per second
		TU_5FPS,
		TU_6FPS,
		TU_8FPS,
		TU_10FPS,
		TU_12FPS,
		TU_16FPS,
		TU_20FPS,
		TU_40FPS,
		TU_75FPS,
		TU_100FPS,
		TU_120FPS,
		TU_125FPS,
		TU_150FPS,
		TU_175FPS,
		TU_180FPS,
		TU_200FPS,
		TU_SI = TU_Second,		// International system

		// Distance units

		DU_Inch,				// Inch
		DU_Foot,				// Foot
		DU_Yard,				// Yard
		DU_Mile,				// Mile
		DU_Millimeter,			// Millimeter
		DU_Centimeter,			// Centimeter
		DU_Meter,				// Meter
		DU_Kilometer,			// Kilometer
		DU_SI = DU_Meter,		// International system

		// Angle units

		AU_Degree,
		AU_Radian,
		AU_SI = AU_Radian,		// International system
	};


	su_ie bool			UnitToString	( char* outName/*[32]*/, Unit inU  );
	su_ie bool			StringToUnit	( Unit& outU, char* inName/*[32]*/ );


	struct_ie Time
	{
						Time			(						);
						Time			( const Time&			);
						Time			( Unit, float			);		// ipart only
						Time			( Unit, float, float	);		// ipart & fpart

		Unit			unit;			// the unit in effect for this instance
		float			ipart;			// integer part of time
		float			fpart;			// fractionnal part of time

		bool			operator <		( const Time&			) const;
		bool			operator >		( const Time&			) const;
		bool			operator ==		( const Time&			) const;
		bool			operator !=		( const Time&			) const;
		float			operator -		( const Time&			) const;
						operator float	(						) const;
						operator int	(						) const;

		Time			toUnit			( Unit					) const;
		Time			toSecond		(						) const;

		float			asUnit			( Unit					) const;
		float			asDay			(						) const;
		float			asHour			(						) const;
		float			asMinute		(						) const;
		float			asSecond		(						) const;
		float			asMillisecond	(						) const;
		float			asMovie			(						) const;
		float			asPALFrame		(						) const;
		float			asNTSCFrame		(						) const;
		float			asShowScan		(						) const;
		float			asPALField		(						) const;
		float			asNTSCField		(						) const;
		float			asFPS			( uint					) const;

		int				Compare			( const Time&, float e	) const;
	};


	struct_ie Date
	{
		int				dsec;			// seconds after the minute - [0,59] - undefined if < 0
		int				dmin;			// minutes after the hour - [0,59] - undefined if < 0
		int				dhour;			// hours since midnight - [0,23] - undefined if < 0
		int				mday;			// day of the month - [1,31] - undefined if < 0
		int				wday;			// day of the week - [1(monday),7(sunday)] - undefined if < 0
		int				mon;			// months since January - [1,12] - undefined if < 0
		int				year;			// years - [0..] - undefined if < 0

		bool			operator <		( const Date&			) const;
		bool			operator >		( const Date&			) const;
		bool			operator ==		( const Date&			) const;
		bool			operator !=		( const Date&			) const;
	};



	struct_ie Distance
	{
						Distance		(							);
						Distance		( const Distance&			);
						Distance		( Unit, float				);

		Unit			unit;			// the unit
		float			measure;		// the value

		bool			operator <		( const Distance&			) const;
		bool			operator >		( const Distance&			) const;
		bool			operator ==		( const Distance&			) const;
		bool			operator !=		( const Distance&			) const;
		float			operator -		( const Distance&			) const;

		Distance		toUnit			( Unit						) const;
		Distance		toMeter			(							) const;
		Distance		toMillimeter	(							) const;
		Distance		toInch			(							) const;

		float			asUnit			( Unit						) const;
		float			asInch			(							) const;
		float			asFoot			(							) const;
		float			asYard			(							) const;
		float			asMile			(							) const;
		float			asMillimeter	(							) const;
		float			asCentimeter	(							) const;
		float			asMeter			(							) const;
		float			asKilometer		(							) const;

		int				Compare			( const Distance&, float e	) const;
	};



	struct_ie Angle
	{
						Angle			(							);
						Angle			( const Angle&				);
						Angle			( Unit, float				);

		Unit			unit;			// the unit
		float			measure;		// the value

		bool			operator <		( const Angle&				) const;
		bool			operator >		( const Angle&				) const;
		bool			operator ==		( const Angle&				) const;
		bool			operator !=		( const Angle&				) const;
		float			operator -		( const Angle&				) const;

		Angle			toUnit			( Unit						) const;
		Angle			toRadian		(							) const;
		Angle			toDegree		(							) const;

		float			asUnit			( Unit						) const;
		float			asRadian		(							) const;
		float			asDegree		(							) const;

		int				Compare			( const Angle&, float e		) const;
	};

}




#include "su_unitsi.h"
#endif // _su_units_h_


