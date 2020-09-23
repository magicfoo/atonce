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



#ifndef _su_unitsi_h_
#define _su_unitsi_h_



namespace su
{

	inline
	Time::Time	(	) : unit( TU_SI )
	{
	}

	inline
	Time::Time	( const Time&	ref	)
	{
		unit  = ref.unit;
		ipart = ref.ipart;
		fpart = ref.fpart;
	}

	inline
	Time::Time	( Unit u, float ip )
	{
		unit  = u;
		ipart = ip;
		fpart = 0.f;
	}

	inline
	Time::Time	( Unit u, float ip, float fp )
	{
		unit  = u;
		ipart = ip;
		fpart = fp;
	}

	inline
	bool
	Time::operator < ( const Time&	inref ) const
	{
		return Compare(inref,0.0001f) < 0;
	}

	inline
	bool
	Time::operator > ( const Time&	inref ) const
	{
		return Compare(inref,0.0001f) > 0;
	}

	inline
	bool
	Time::operator == ( const Time&	inref ) const
	{
		return Compare(inref,0.0001f) == 0;
	}

	inline
	bool
	Time::operator != ( const Time&	inref ) const
	{
		return Compare(inref,0.0001f) != 0;
	}

	inline
	float
	Time::operator -	( const Time & inref ) const
	{
		Time r = inref.toUnit( unit );
		return (ipart-r.ipart)+(fpart-r.fpart);
	}

	inline
	Time::operator float	(		) const
	{
		return (ipart+fpart);
	}

	inline
	Time::operator int	(		) const
	{
		return int(ipart);
	}

	inline
	Time
	Time::toSecond	(		) const
	{
		return toUnit( TU_Second );
	}

	inline
	float
	Time::asUnit	( Unit		u	) const
	{
		Time t = toUnit(u);
		return float(t);
	}

	inline
	float
	Time::asDay		(		) const
	{
		return asUnit( TU_Day );
	}


	inline
	float
	Time::asHour		(		) const
	{
		return asUnit( TU_Hour );
	}

	inline
	float
	Time::asMinute		(		) const
	{
		return asUnit( TU_Minute );
	}

	inline
	float
	Time::asSecond		(		) const
	{
		return asUnit( TU_Second );
	}

	inline
	float
	Time::asMillisecond		(		) const
	{
		return asUnit( TU_Millisecond );
	}

	inline
	float
	Time::asMovie		(		) const
	{
		return asUnit( TU_Movie );
	}

	inline
	float
	Time::asPALFrame		(		) const
	{
		return asUnit( TU_PALFrame );
	}

	inline
	float
	Time::asNTSCFrame		(		) const
	{
		return asUnit( TU_NTSCFrame );
	}

	inline
	float
	Time::asShowScan		(		) const
	{
		return asUnit( TU_ShowScan );
	}

	inline
	float
	Time::asPALField		(		) const
	{
		return asUnit( TU_PALField );
	}

	inline
	float
	Time::asNTSCField		(		) const
	{
		return asUnit( TU_NTSCField );
	}

	inline
	float
	Time::asFPS		(	uint howmany	) const
	{
		return asSecond() * float(int(howmany));
	}

	inline
	int
	Time::Compare	( const Time& inref, float e	) const
	{
		Time  r		= inref.toUnit( unit );
		float di	= ipart - r.ipart;
		float df	= fpart - r.fpart;
		float p_di	= (di<0.f) ? -di : di;
		float p_df	= (df<0.f) ? -df : df;
		if( p_di<e && p_df<e )		return 0;	// equal
		if( di>0.f )				return +1;	// greater
		if( di<0.f )				return -1;	// lower
		return (df>0.f) ? +1 : -1;
	}

	inline
	bool
	Date::operator < ( const Date& inref ) const
	{
		if( year < inref.year )		return TRUE;
		if( year > inref.year )		return FALSE;
		if( mon < inref.mon )		return TRUE;
		if( mon > inref.mon )		return FALSE;
		if( mday < inref.mday )		return TRUE;
		if( mday > inref.mday )		return FALSE;
		if( dhour < inref.dhour )	return TRUE;
		if( dhour > inref.dhour )	return FALSE;
		if( dmin < inref.dmin )		return TRUE;
		if( dmin > inref.dmin )		return FALSE;
		if( dsec < inref.dsec )		return TRUE;
		if( dsec > inref.dsec )		return FALSE;
		return FALSE;
	}

	inline
	bool
	Date::operator > ( const Date& inref ) const
	{
		if( year > inref.year )		return TRUE;
		if( year < inref.year )		return FALSE;
		if( mon > inref.mon )		return TRUE;
		if( mon < inref.mon )		return FALSE;
		if( mday > inref.mday )		return TRUE;
		if( mday < inref.mday )		return FALSE;
		if( dhour > inref.dhour )	return TRUE;
		if( dhour < inref.dhour )	return FALSE;
		if( dmin > inref.dmin )		return TRUE;
		if( dmin < inref.dmin )		return FALSE;
		if( dsec > inref.dsec )		return TRUE;
		if( dsec < inref.dsec )		return FALSE;
		return FALSE;
	}

	inline
	bool
	Date::operator == ( const Date& inref ) const
	{
		if( year != inref.year )	return FALSE;
		if( mon != inref.mon )		return FALSE;
		if( mday != inref.mday )	return FALSE;
		if( dhour != inref.dhour )	return FALSE;
		if( dmin != inref.dmin )	return FALSE;
		if( dsec != inref.dsec )	return FALSE;
		return FALSE;
	}

	inline
	bool
	Date::operator != ( const Date& inref ) const
	{
		if( year != inref.year )	return TRUE;
		if( mon != inref.mon )		return TRUE;
		if( mday != inref.mday )	return TRUE;
		if( dhour != inref.dhour )	return TRUE;
		if( dmin != inref.dmin )	return TRUE;
		if( dsec != inref.dsec )	return TRUE;
		return FALSE;
	}

	inline 
	Distance::Distance() : unit(DU_SI),measure(0.0f)
	{

	}

	inline 
	Distance::Distance		( const Distance&	inD)
	{
		unit	= inD.unit;
		measure = inD.measure;
	}

	inline 
	Distance::Distance		( Unit inUnit, float	inD		)
	{
		unit	= inUnit;
		measure = inD;
	}


	inline
	bool
	Distance::operator < ( const Distance&	inD ) const
	{
		return Compare(inD,0.0001f) < 0;
	}

	inline
	bool
	Distance::operator > ( const Distance&	inD ) const
	{
		return Compare(inD,0.0001f) > 0;
	}

	inline
	bool
	Distance::operator == ( const Distance&	inD ) const
	{
		return Compare(inD,0.0001f) == 0;
	}

	inline
	bool
	Distance::operator != ( const Distance&	inD ) const
	{
		return Compare(inD,0.0001f) != 0;
	}

	inline
	float
	Distance::operator -	( const Distance&	inD ) const
	{
		return measure - inD.toUnit(unit).measure ;
	}

	inline
	Distance
	Distance::toMeter	(	) const
	{
		return toUnit(DU_Meter);
	}

	inline
	Distance
	Distance::toMillimeter	(	) const
	{
		return toUnit(DU_Millimeter);
	}

	inline
	Distance
	Distance::toInch	(	) const
	{
		return toUnit(DU_Inch);
	}

	inline
	float
	Distance::asUnit	( Unit	inUnit	) const
	{
		return toUnit(inUnit).measure;
	}

	inline
	float
	Distance::asInch	(	) const
	{
		return asUnit(DU_Inch);
	}

	inline
	float
	Distance::asFoot	(	) const
	{
		return asUnit(DU_Foot);
	}

	inline
	float
	Distance::asYard	(	) const
	{
		return asUnit(DU_Yard);
	}

	inline
	float
	Distance::asMile	(	) const
	{
		return asUnit(DU_Mile);
	}

	inline
	float
	Distance::asMillimeter	(	) const
	{
		return asUnit(DU_Millimeter);
	}

	inline
	float
	Distance::asCentimeter	(	) const
	{
		return asUnit(DU_Centimeter);
	}

	inline
	float
	Distance::asMeter	(	) const
	{
		return asUnit(DU_Meter);
	}

	inline
	float
	Distance::asKilometer	(	) const
	{
		return asUnit(DU_Kilometer);
	}

	inline
	int
	Distance::Compare	( const Distance& inD, float e	) const
	{
		
		float dist = inD.asUnit(unit);
		float diff = measure - dist;

		if (diff >= 0.0f) {			// dist >= measure 
			if (diff < e) return 0;	// equal
			return +1;				// greater
		}
		else {						// dist < measure 
			if (diff > -e) return 0; // equal
			return -1;				// lower
		}
	}

	inline
	Angle::Angle	(							)
	{
		unit = AU_SI;
		measure = 0.0f;
	}

	inline
	Angle::Angle	( const Angle&	 inA			)
	{
		unit =inA.unit;
		measure = inA.measure;
	}

	inline
	Angle::Angle	( Unit inU , float	inA)
	{
		unit = inU;
		measure = inA;
	}

	inline
	bool
	Angle::operator <	( const Angle&		inA		) const
	{
		return Compare(inA,0.0001f) < 0;
	}

	inline
	bool			
	Angle::operator >	( const Angle&		inA		) const
	{
		return Compare(inA,0.0001f) > 0;
	}

	inline
	bool			
	Angle::operator ==	( const Angle&		inA		) const
	{
		return Compare(inA,0.0001f) == 0;
	}

	inline
	bool
	Angle::operator !=	( const Angle&		inA		) const
	{
		return Compare(inA,0.0001f) != 0;
	}

	inline
	float
	Angle::operator -	( const Angle&		inA		) const
	{
		Angle a = inA.toUnit(unit);
		return measure - a.measure;
	}

	inline
	Angle
	Angle::toRadian	(	) const
	{
		return toUnit(AU_Radian);
	}

	inline
	Angle
	Angle::toDegree	(	) const
	{
		return toUnit(AU_Degree);
	}

	inline
	float
	Angle::asUnit	( Unit	inU	 ) const
	{
		return toUnit(inU).measure;
	}

	inline
	float
	Angle::asRadian	(	) const
	{
		return toUnit(AU_Radian).measure;
	}

	inline
	float
	Angle::asDegree	(	) const
	{
		return toUnit(AU_Degree).measure;
	}

	inline
	int	
	Angle::Compare	( const Angle& inA , float e	) const
	{
		float angle = inA.asUnit(unit);
		float diff = measure - angle;

		if (diff >= 0.0f)
		{
			// dist >= measure 
			if (diff < e) return 0;	// equal
			return +1;				// greater
		}
		else
		{							// dist < measure 
			if (diff > -e) return 0;// equal
			return -1;				// lower
		}
	}
}



#endif // _su_unitsi_h_


