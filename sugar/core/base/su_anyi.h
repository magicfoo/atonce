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




#ifndef _su_anyi_h_
#define _su_anyi_h_




namespace su
{

	template <typename T> inline
	T*	Any::asObject ( ) const
	{
		IInterface* i  = asObject();
		IInterface* ii = i ? i->GetInterface(T::ClassId) : NULL;
		return (T*)ii;
	}


	template <typename T> inline
	T*	AnyList::asObject ( pcstr n, bool cs ) const
	{
		IInterface* i = asObject(n,cs);
		IInterface* ii = i ? i->GetInterface(T::ClassId) : NULL;
		return (T*)ii;
	}

	
	template <uint Bound> inline
	AnyArray<Bound>::AnyArray ( )
	{
		vfirst = &var[0];
		vend   = vfirst + Bound;
	}


	template <uint Bound> inline
	void AnyArray<Bound>::set (	int i, pcstr n, const Any& v )
	{
		SU_ASSERT( i >= 0 );
		SU_ASSERT( i < Bound );
		var[i].name = n;
		var[i].value.copy( v );
	}


	template <uint Bound> inline
	AnyList AnyArray<Bound>::list ( int nb, int first )
	{
		SU_ASSERT( nb >= 0 );
		SU_ASSERT( nb <= Bound );
		SU_ASSERT( first >= 0 );
		SU_ASSERT( first+nb <= Bound );
		if( nb==0 )		return AnyList();
		else			return AnyList( &var[first], &var[first+nb] );
	}


}




#endif // _su_anyi_h_



