/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2010 AtOnce Technologies
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



#ifndef _su_mcursori_h_
#define _su_mcursori_h_




namespace su
{

	template <typename T> inline 
	T* MCursor::tpointer<T>::operator () ( )
	{
		SU_ASSERT( ptr );
		return ptr;
	}

	template <typename T> inline 
	T& MCursor::tpointer<T>::operator * ( )
	{
		SU_ASSERT( ptr );
		return *ptr;
	}

	template <typename T> inline 
	T& MCursor::tpointer<T>::operator [] ( int i )
	{
		SU_ASSERT( ptr );
		return ptr[i];
	}

	template <typename T> inline 
	MCursor::tpointer<T>& MCursor::tpointer<T>::operator << ( const T& v )
	{
		SU_ASSERT( ptr );
		*ptr++ = v;
		return *this;
	}

	template <typename T> inline 
	MCursor::tpointer<T>& MCursor::tpointer<T>::operator >> ( T& v )
	{
		SU_ASSERT( ptr );
		v = *ptr++;
		return *this;
	}

	template <typename T> inline 
	MCursor::tpointer<T>& MCursor::tpointer<T>::operator = ( T* v )
	{
		ptr = v;
		bit = 0;
		return *this;
	}

	template <typename T> inline 
	bool MCursor::tpointer<T>::operator == ( T* v )
	{
		return (ptr==v);
	}

	template <typename T> inline 
	MCursor::tpointer<T>& MCursor::tpointer<T>::operator ++ ( )
	{
		SU_ASSERT( ptr );
		ptr++;
		return *this;
	}

	template <typename T> inline 
	MCursor::tpointer<T> MCursor::tpointer<T>::operator ++ ( int )
	{
		SU_ASSERT( ptr );
		MCursor::tpointer<T> p = *this;
		p.ptr++;
		return p;
	}

	template <typename T> inline 
	MCursor::tpointer<T>& MCursor::tpointer<T>::operator -- ( )
	{
		SU_ASSERT( ptr );
		ptr--;
		return *this;
	}

	template <typename T> inline 
	MCursor::tpointer<T> MCursor::tpointer<T>::operator -- ( int )
	{
		SU_ASSERT( ptr );
		MCursor::tpointer<T> p = *this;
		p.ptr--;
		return p;
	}

	template <typename T> inline 
	MCursor::tpointer<T> MCursor::tpointer<T>::operator + ( int i ) const
	{
		SU_ASSERT( ptr );
		MCursor::tpointer<T> p = *this;
		p.ptr += i;
		return p;
	}

	template <typename T> inline 
	MCursor::tpointer<T> MCursor::tpointer<T>::operator - ( int i ) const
	{
		SU_ASSERT( ptr );
		MCursor::tpointer<T> p = *this;
		p.ptr -= i;
		return p;
	}

	template <typename T> inline 
	MCursor::tpointer<T>& MCursor::tpointer<T>::operator += ( int i )
	{
		SU_ASSERT( ptr );
		ptr += i;
		return *this;
	}

	template <typename T> inline 
	MCursor::tpointer<T>& MCursor::tpointer<T>::operator -= ( int i )
	{
		SU_ASSERT( ptr );
		ptr -= i;
		return *this;
	}

	template <typename T> inline 
	int MCursor::tpointer<T>::operator - ( T* r )
	{
		SU_ASSERT( ptr );
		return ptr - r;
	}

	template <typename T> inline 
	MCursor::tpointer<T>::operator T* ( )
	{
		SU_ASSERT( ptr );
		return ptr;
	}





	template <typename T> inline
	void MCursor::align ( )
	{
		align( sizeof(T)*8 );
	}


	template <typename T> inline
	T MCursor::readmsb ( )
	{
		T t;
		readB( (byte*)&t, sizeof(t) );
		return util::MSBToNative(t);
	}


	template <typename T> inline
	T MCursor::readlsb ( )
	{
		T t;
		readB( (byte*)&t, sizeof(t) );
		return util::LSBToNative(t);
	}

	
	template <typename T> inline
	void MCursor::writemsb ( const T& t )
	{
		T tt = util::NativeToMSB(t);
		writeB( (byte*)&tt, sizeof(tt) );
	}


	template <typename T> inline
	void MCursor::writelsb ( const T& t )
	{
		T tt = util::NativeToLSB(t);
		writeB( (byte*)&tt, sizeof(tt) );
	}


	template <typename T> inline
	T MCursor::readB ( )
	{
		T t;
		readB( (byte*)&t, sizeof(t) );
	}


	template <typename T> inline
	void MCursor::writeB ( const T& t )
	{
		writeB( (byte*)&t, sizeof(t) );
	}


	template <typename T> inline
	void MCursor::readstring ( T& outString )
	{
		outString.clear();

		align( 8 );

		uint64 p, eos;
		p = pos();
		if( findB(eos,0) )
		{
			SU_ASSERT( eos >= p );
			outString.copy( (char*)by() );
			seek( eos+8 );
		}
	}


	template <typename T> inline
	void MCursor::readstring ( T& outString, uint incapacity )
	{
		SU_ASSERT( incapacity < 256 );

		outString.clear();

		char tmp[256];
		if( incapacity < 256 )
		{
			readB( (byte*)tmp, incapacity );
			tmp[ incapacity ] = 0;
			outString.copy( tmp );
		}
	}

}




#endif // _su_mcursori_h_


