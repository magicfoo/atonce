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


#ifndef _su_mapping_basei_h_
#define _su_mapping_basei_h_



namespace su { namespace map
{

	template<typename T> inline
	void si_attr::copy ( const T& inref )
	{
		SU_ASSERT( !ro );
		SU_REQUIRE_OR_RETURN( p );
		SU_ASSERT( sizeof(T)*8 == w*n );
		util::ub_copy( p,s,(byte*)&inref,0,w*n );
	}


	template<typename T> inline
	void si_attr::load ( T& outref )
	{
		SU_REQUIRE_OR_RETURN( p );
		SU_ASSERT( sizeof(T)*8 == w*n );
		util::ub_copy( (byte*)&outref,0,p,s,w*n );
	}


	template<typename T> inline
	void si_attr::setarray ( T* inref )
	{
		SU_ASSERT( !ro );
		SU_REQUIRE_OR_RETURN( inref );
		SU_REQUIRE_OR_RETURN( p );
		for( int i=0 ; i<n ; i++ )
			set( (value_type)inref[i], i );
	}


	template<typename T> inline
	void ui_attr::copy ( const T& inref )
	{
		SU_ASSERT( !ro );
		SU_REQUIRE_OR_RETURN( p );
		SU_ASSERT( sizeof(T)*8 == w*n );
		util::ub_copy( p,s,(byte*)&inref,0,w*n );
	}


	template<typename T> inline
	void ui_attr::load ( T& outref )
	{
		SU_REQUIRE_OR_RETURN( p );
		SU_ASSERT( sizeof(T)*8 == w*n );
		util::ub_copy( (byte*)&outref,0,p,s,w*n );
	}


	template<typename T> inline
	void ui_attr::setarray ( T* inref )
	{
		SU_ASSERT( !ro );
		SU_REQUIRE_OR_RETURN( inref );
		SU_REQUIRE_OR_RETURN( p );
		for( int i=0 ; i<n ; i++ )
			set( (value_type)inref[i], i );
	}


	template<typename T> inline
	void fx_attr::copy ( const T& inref )
	{
		SU_ASSERT( !ro );
		SU_REQUIRE_OR_RETURN( p );
		SU_ASSERT( sizeof(T)*8 == w*n );
		util::ub_copy( p,s,(byte*)&inref,0,w*n );
	}


	template<typename T> inline
	void fx_attr::load ( T& outref )
	{
		SU_REQUIRE_OR_RETURN( p );
		SU_ASSERT( sizeof(T)*8 == w*n );
		util::ub_copy( (byte*)&outref,0,p,s,w*n );
	}


	template<typename T> inline
	void fx_attr::setarray ( T* inref )
	{
		SU_ASSERT( !ro );
		SU_REQUIRE_OR_RETURN( inref );
		SU_REQUIRE_OR_RETURN( p );
		for( int i=0 ; i<n ; i++ )
			set( (value_type)inref[i], i );
	}


	template<typename T> inline
	void fp_attr::copy ( const T& inref )
	{
		SU_ASSERT( !ro );
		SU_REQUIRE_OR_RETURN( p );
		SU_ASSERT( sizeof(T)*8 == w*n );
		util::ub_copy( p,s,(byte*)&inref,0,w*n );
	}


	template<typename T> inline
	void fp_attr::load ( T& outref )
	{
		SU_REQUIRE_OR_RETURN( p );
		SU_ASSERT( sizeof(T)*8 == w*n );
		util::ub_copy( (byte*)&outref,0,p,s,w*n );
	}


	template<typename T> inline
	void fp_attr::setarray ( T* inref )
	{
		SU_ASSERT( !ro );
		SU_REQUIRE_OR_RETURN( inref );
		SU_REQUIRE_OR_RETURN( p );
		for( int i=0 ; i<n ; i++ )
			set( (value_type)inref[i], i );
	}




	template <typename T> inline
	int base_map::tag_load ( T& t )
	{
		SU_REQUIRE_OR_0( tag_code() == T::CODE );
		return t.load( tag_addr() );
	}
	

	template <typename T> inline
	bool base_map::tag_seek ( T& t )
	{
		return tag_seek(int(T::CODE)) && tag_load(t);
	}
	

	template <typename T> inline
	int base_map::tag_add ( T& t )
	{
		byte* e = data_end();
		byte* p = util::RoundX( e, (1<<align) );
		int   s = t.create( p );
		flen.set( flen.get()+(p-e)+((s+7)>>3) );
		return s;
	}
	

	template <typename T> inline
	int base_map::tag_add ( T& t, typename T::requirements &r )
	{
		byte* e = data_end();
		byte* p = util::RoundX( e, (1<<align) );
		int   s = t.create( p, r );
		flen.set( flen.get()+(p-e)+((s+7)>>3) );
		return s;
	}

} }



#endif // _su_mapping_basei_h_