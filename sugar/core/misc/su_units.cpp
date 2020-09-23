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



bool
su::UnitToString	( char* outName/*[32]*/, Unit inU  )
{
	return FALSE;
}


bool
su::StringToUnit	( Unit& outU, char* inName/*[32]*/ )
{
	return FALSE;
}




su::Time
su::Time::toUnit	( Unit	inUnit	) const
{
	static float any_per_sec[] = {
		(1.f/86400.f),		// TU_Day
		(1.f/3600.f),		// TU_Hour
		(1.f/60.f),			// TU_Minute
		(1.f),				// TU_Second
		(1000.f),			// TU_Millisecond
		(24.f),				// TU_Movie
		(25.f),				// TU_PALFrame
		(30.f),				// TU_NTSCFrame
		(48.f),				// TU_ShowScan
		(50.f),				// TU_PALField
		(60.f),				// TU_NTSCField
		(2.f),				// TU_2FPS
		(3.f),				// TU_3FPS
		(4.f),				// TU_4FPS
		(5.f),				// TU_5FPS
		(6.f),				// TU_6FPS
		(8.f),				// TU_8FPS
		(10.f),				// TU_10FPS
		(12.f),				// TU_12FPS
		(16.f),				// TU_16FPS
		(20.f),				// TU_20FPS
		(40.f),				// TU_40FPS
		(75.f),				// TU_75FPS
		(100.f),			// TU_100FPS
		(120.f),			// TU_120FPS
		(125.f),			// TU_125FPS
		(150.f),			// TU_150FPS
		(175.f),			// TU_175FPS
		(180.f),			// TU_180FPS
		(200.f),			// TU_200FPS
	};

	if( inUnit==unit )
		return *this;

	Time ti;
	util::Zero( ti );

	if( unit>=TU_Day && unit<=TU_200FPS && inUnit>=TU_Day && inUnit<=TU_200FPS )
	{
		// t = (ipart+fpart) / any_per_sec[from_unit] * any_per_sec[to_unit]
		float to_unit = any_per_sec[inUnit-TU_Day] / any_per_sec[unit-TU_Day];
		float i_part  = ipart * to_unit;
		float f_part  = fpart * to_unit;
		ti.ipart = int(i_part);
		ti.fpart = f_part + (i_part - ti.ipart);
	}

	return ti;
}

su::Distance
su::Distance::toUnit	( Unit	inUnit) const
{
	static const float UnitToMeter [] = {
		0.0254f,				//DU_Inch,					// Inch
		0.3048f,				//DU_Foot,					// Foot
		0.9144f,				//DU_Yard,					// Yard
		1609.344f,				//DU_Mile,					// Mile
		0.001f,					//DU_Millimeter,			// Millimeter
		0.01f,					//DU_Centimeter,			// Centimeter
		1.0f,					//DU_Meter,					// Meter
		1000.0f,				//DU_Kilometer,				// Kilometer
	};

	if( inUnit==unit )
		return *this;

	if( inUnit>=DU_Inch && inUnit<=DU_Kilometer && unit>=DU_Inch && unit<=DU_Kilometer )
	{
		float dist = measure * UnitToMeter[unit-DU_Inch] / UnitToMeter[inUnit-DU_Inch] ;
		return Distance ( inUnit , dist );
	}
	else
	{
		return Distance();
	}
}

su::Angle			
su::Angle::toUnit	( Unit		inUnit	) const
{
	static const float UnitToDegree [] = {
		1.0f,		//AU_Degree
		180.0f/Pi,	//AU_Radian
	};

	if( inUnit==unit )
		return *this;

	if( inUnit>=AU_Degree && inUnit<=AU_Radian && unit>=AU_Degree && unit<=AU_Radian )
	{
		float angle = measure * UnitToDegree[unit-AU_Degree] / UnitToDegree[inUnit-AU_Degree] ;
		return Angle ( inUnit , angle );
	}
	else
	{
		return Angle();
	}
}


