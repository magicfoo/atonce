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



#ifndef _su_stringi_h_
#define _su_stringi_h_




namespace su
{

	//
	// iterator

	inline
	String::iterator::iterator ( )
		: ptr(NULL)
	{
		//
	}

	inline
	String::iterator::iterator ( cpointer inPtr )
		: ptr((pointer)inPtr)
	{
		//
	}

	inline
	String::iterator::iterator ( pointer inPtr )
		: ptr(inPtr)
	{
		//
	}

	inline
	String::iterator::iterator ( const iterator& it )
		: ptr(it.ptr)
	{
		//
	}

	inline
	String::iterator
	String::iterator::operator + (	int inN ) const
	{
		return iterator( ptr + inN );
	}

	inline
	String::iterator
	String::iterator::operator - (	int inN	) const
	{
		return iterator( ptr - inN );
	}

	inline
	String::iterator &
	String::iterator::operator += ( int inN	)
	{
		ptr += inN;
		return *this;
	}

	inline
	String::iterator &
	String::iterator::operator -= ( int inN	)
	{
		ptr -= inN;
		return *this;
	}

	inline
	String::iterator &
	String::iterator::operator ++ ( )
	{
		if( ptr )
		{
			*ptr++;
		}
		return *this;
	}

	inline
	String::iterator &
	String::iterator::operator -- ( )
	{
		if( ptr )
		{
			*--ptr;
		}
		return *this;
	}

	inline
	String::iterator
	String::iterator::operator ++ ( int )
	{
		iterator it( *this );
		ptr++;
		return it;
	}

	inline
	String::iterator
	String::iterator::operator -- ( int )
	{
		iterator it( *this );
		ptr--;
		return it;
	}

	inline
	String::size_type
	String::iterator::operator -	( iterator inRef ) const
	{
		SU_ASSERT( ptr >= inRef.ptr );
		return (ptr - inRef.ptr);
	}

	inline
	String::iterator &
	String::iterator::operator =	( iterator inRef )
	{
		ptr = inRef.ptr;
		return *this;
	}

	inline
	bool
	String::iterator::operator == ( iterator inRef ) const
	{
		return ( ptr == inRef.ptr );
	}

	inline
	bool
	String::iterator::operator != ( iterator inRef ) const
	{
		return ( ptr != inRef.ptr );
	}

	inline
	bool
	String::iterator::operator <	( iterator inRef ) const
	{
		return ( ptr < inRef.ptr );
	}

	inline
	bool
	String::iterator::operator >	( iterator inRef ) const
	{
		return ( ptr > inRef.ptr );
	}

	inline
	bool
	String::iterator::operator <= ( iterator inRef ) const
	{
		return ( ptr <= inRef.ptr );
	}

	inline
	bool
	String::iterator::operator >= ( iterator inRef ) const
	{
		return ( ptr >= inRef.ptr );
	}

	inline
	String::char_type
	String::iterator::operator * ( ) const
	{
		return *ptr;
	}

	inline
	String::pointer
	String::iterator::operator &	( ) const
	{
		return ptr;
	}

	inline
	String::iterator::operator const char * ( ) const
	{
		return ptr;
	}


	// string

	inline
	String::String ( Pool* p )
	{
		relocate( p );
	}

	inline
	String::String( Pool* p, const String& r )
	{
		relocate( p );
		copy( r );
	}

	inline
	String::String( Pool* p, pcstr r )
	{
		relocate( p );
		copy( r );
	}

	inline
	String::~String()
	{
		//
	}

	inline
	void
	String::relocate ( Pool* p )
	{
		data.relocate( p );
	}

	inline
	Pool*
	String::pool ( ) const
	{
		return data.pool();
	}

	inline
	bool
	String::isnull ( ) const
	{
		return ( this == &Null );
	}

	inline
	bool
	String::empty (	) const
	{
		return data.empty();
	}

	inline
	String::size_type
	String::size ( ) const
	{
		return data.size();
	}

	inline
	String::size_type
	String::len	( ) const
	{
		return data.size();
	}

	inline
	String::reference
	String::at ( size_type pos ) const
	{
		return data.at( pos );
	}

	inline
	String::iterator
	String::begin ( ) const
	{
		return iterator( &data.begin() );
	}

	inline
	String::iterator
	String::end ( ) const
	{
		return iterator( &data.end() );
	}

	inline
	String::size_type
	String::capacity ( )	const
	{
		return data.capacity();
	}

	inline
	String::cpointer
	String::cstr ( cpointer inDef ) const
	{
		if( data.empty() )
		{
			return inDef;
		}
		else
		{
			cpointer cs = data.data();
			SU_ASSERT( cs[size()] == 0 );	// check \0 terminated !
			return cs;
		}
	}

	inline
	String::pointer
	String::str ( pointer inDef ) const
	{
		return (pointer) cstr( inDef );
	}

	inline
	bool
	String::isspace ( ) const
	{
		return util::Strisspace( cstr() );
	}

	inline
	bool
	String::isalpha ( ) const
	{
		return util::Strisalpha( cstr() );
	}

	inline
	bool
	String::isdigit ( ) const
	{
		return util::Strisdigit( cstr() );
	}

	inline
	bool
	String::isalnum ( ) const
	{
		return util::Strisalnum( cstr() );
	}

	inline
	int
	String::compare ( char_type c, bool cs ) const
	{
		return util::Strcompare( cstr(), c, cs );
	}

	inline
	int
	String::compare ( pcstr s, bool cs ) const
	{
		return util::Strcompare( cstr(), s, cs );
	}

	inline
	int
	String::compare ( const String& s, bool cs ) const
	{
		return util::Strcompare( cstr(), s.cstr(), cs );
	}

	inline
	int
	String::compare ( const String& s, uint len, bool cs ) const
	{
		return util::Strcompare( cstr(), s.cstr(), len, cs );
	}

	inline
	bool
	String::equals ( char_type c, bool cs ) const
	{
		return util::Strequals( cstr(), c, cs );
	}

	inline
	bool
	String::equals ( pcstr s, bool cs ) const
	{
		return util::Strequals( cstr(), s, cs );
	}

	inline
	bool
	String::equals (	const String& s, bool cs	) const
	{
		return util::Strequals( cstr(), s.cstr(), cs );
	}

	inline
	bool
	String::equals	(	const String& s, uint len, bool cs	) const
	{
		return util::Strequals( cstr(), s.cstr(), len, cs );
	}
	
	inline
	bool
	String::startswith	(	char_type c, bool cs	) const
	{
		return util::Strstartswith( cstr(), (int)c, cs );
	}
	
	inline
	bool
	String::startswith	(	pcstr s, bool cs	) const
	{
		return util::Strstartswith( cstr(), s, cs );
	}

	inline
	bool
	String::startswith	(	const String& s, bool cs	) const
	{
		return util::Strstartswith( cstr(), s.cstr(), cs );
	}

	inline
	bool
	String::startswithany	(	const String& charset, bool cs	) const
	{
		return util::Strstartswithany( cstr(), charset.cstr(), cs );
	}

	inline
	bool
	String::endswith	(	char_type c, bool cs	) const
	{
		return util::Strendswith( cstr(), (int)c, cs );
	}

	inline
	bool
	String::endswith	(	pcstr s, bool cs	) const
	{
		return util::Strendswith( cstr(), s, cs );
	}

	inline
	bool
	String::endswith	(	const String& s, bool cs	) const
	{
		return util::Strendswith( cstr(), s.cstr(), cs );
	}

	inline
	bool
	String::endswithany	(	const String& charset, bool cs	) const
	{
		return util::Strendswithany( cstr(), charset.cstr(), cs );
	}

	inline
	int
	String::offsetof ( pcstr p ) const
	{
		pcstr b = begin();
		pcstr e = end();
		SU_ASSERT( p>=b && p<e );
		if( p>=b && p<e )	return p-b;
		else				return -1;
	}

	inline
	int
	String::offsetof ( iterator p ) const
	{
		pcstr b = begin();
		pcstr e = end();
		SU_ASSERT( p>=b && p<e );
		if( p>=b && p<e )	return p-b;
		else				return -1;
	}

	inline
	int
	String::indexof	(	char_type c, size_type pos, bool cs, bool rev ) const
	{
		if( pos >= size() )
		{
			return -1;
		}
		else
		{
			int idx = util::Strindexof( cstr()+pos, c, cs, rev );
			return idx<0 ? -1 : int(idx+pos);
		}
	}

	inline
	int
	String::indexof ( const String& r, size_type pos, bool cs, bool rev ) const
	{
		return indexof( r.cstr(), pos, cs, rev );
	}

	inline
	int
	String::indexof ( pcstr r, size_type pos, bool cs, bool rev ) const
	{
		if( pos>=size() )
		{
			return -1;
		}
		else
		{
			int idx = util::Strindexof( cstr()+pos, r, cs, rev );
			return idx<0 ? -1 : int(idx+pos);
		}
	}

	inline
	int
	String::indexofany ( const String& charset, size_type pos, bool cs, bool rev ) const
	{
		return indexofany( charset.cstr(), pos, cs, rev );
	}

	inline
	int
	String::indexofany ( pcstr charset, size_type pos, bool cs, bool rev ) const
	{
		if( pos >= size() )
		{
			return -1;
		}
		else
		{
			int idx = util::Strindexofany( cstr()+pos, charset, cs, rev );
			return idx<0 ? -1 : int(idx+pos);
		}
	}

	inline
	String::size_type
	String::tonumber	(	long& a, int base, size_type pos	) const
	{
		pstr endp = NULL;
		pcstr nptr = cstr();
		a = util::Strtolong( nptr, &endp, base );
		if( !endp || endp==nptr )
			return 0;
		return (endp - nptr);
	}

	inline
	String::size_type
	String::tonumber	( ulong& a, int base, size_type pos	) const
	{
		pstr endp = NULL;
		pcstr nptr = cstr();
		a = util::Strtoulong( nptr, &endp, base );
		if( !endp || endp==nptr )
			return 0;
		return (endp - nptr);
	}

	inline
	String::size_type
	String::tonumber	( float& a, size_type pos ) const
	{
		pstr endp = NULL;
		pcstr nptr = cstr();
		a = util::Strtofloat( nptr, &endp );
		if( !endp || endp==nptr )
			return 0;
		return (endp - nptr);
	}

	inline
	void
	String::split ( size_type pos, String& outl, String& outr ) const
	{
		outl.copy( *this, 0, pos );
		outr.copy( *this, pos, size() );
	}

	inline
	void
	String::splitsep ( pcstr seplist, String& outl, String& outr ) const
	{
		int pos = indexofany( seplist );
		if( pos < 0 )
		{
			outl.clear();
			outr.clear();
		}
		else
		{
			String l, r;
			l.relocate( pool() );
			r.relocate( pool() );
			l.copy( *this, 0, pos );
			r.copy( *this, pos+1, size() );
			outl.copy( l );
			outr.copy( r );
		}
	}

	inline
	void
	String::splitsep ( const String& seplist, String& outl, String& outr ) const
	{
		splitsep( seplist.cstr(), outl, outr );
	}

	inline
	uint32
	String::hash32 ( bool innormalized ) const
	{
		return util::hmac_crc32_w( cstr(), innormalized );
	}

	inline
	uint128
	String::hash128 ( ) const
	{
		uint128 res;
		byte*   md5;
		md5 = util::hmac_md5( data.data(), data.size() );
		SU_ASSERT( md5 );
		util::Memcopy( &res, md5, 16 );
		return res;
	}

	inline
	String &
	String::tolower ( )
	{
		util::Strtolower( (pstr)cstr() );
		return *this;
	}

	inline
	String &
	String::toupper ( )
	{
		util::Strtoupper( (pstr)cstr() );
		return *this;
	}



	// modifiers

	inline
	void
	String::reserve ( size_type n )
	{
		assert_pool();
		data.reserve( n );
		assert_cstr();
	}

	inline
	void
	String::resize ( size_type n, char_type c )
	{
		assert_pool();
		data.resize( n, c );
		assert_cstr();
	}

	inline
	void
	String::squeeze ( )
	{
		// keep reserve of 1 byte for \0 terminator
		assert_pool();
		data.reserve( data.size()+1 );
		assert_cstr();
	}

	inline
	String&
	String::strip ( )
	{
		while( startswithany(" \n\t") )		erase(0,1);
		while( endswithany(" \n\t") )		erasend(1);
		return *this;
	}

	inline
	String &
	String::clear ( )
	{
		assert_pool();
		data.clear();
		return *this;
	}

	inline
	String &
	String::erase ( size_type pos, size_type len )
	{
		assert_pool();
		if( pos<size() && len>0 )
		{
			vector_type::iterator it0 = data.begin() + pos;
			vector_type::iterator it1 = it0 + len;
			if( it1 > data.end() )
				it1 = data.end();
			data.erase( it0, it1 );
			assert_cstr();
		}
		return *this;
	}

	inline
	String &
	String::insert ( size_type pos, const String& r )
	{
		assert_pool();
		data.insert( data.begin()+pos, r.data.begin(), r.data.end() );
		assert_cstr();
		return *this;
	}

	inline
	String &
	String::insert ( size_type pos, cpointer r )
	{
		assert_pool();
		if( r )
		{
			int l = util::Strlen( r );
			data.insert( data.begin()+pos, vector_type::iterator(r), vector_type::iterator(r)+l );
			assert_cstr();
		}
		return *this;
	}

	inline
	String &
	String::copy ( const String& r )
	{
		data.copy( r.data );
		assert_cstr();
		return *this;
	}

	inline
	String &
	String::copy ( cpointer r )
	{
		assert_pool();
		data.clear();
		if( r )
		{
			uint l = util::Strlen( r );
			data.copy( vector_type::iterator(r), vector_type::iterator(r)+l );
		}
		assert_cstr();
		return *this;
	}

	inline
	String &
	String::copy ( cpointer r, size_type pos, size_type len )
	{
		assert_pool();
		data.clear();
		if( r && len>0 )
		{
			uint l = util::Strlen( r );
			if( pos < l )
			{
				uint n = l - pos;
				n = (n>len ? len : n);
				if( n > 0 )
				{
					data.copy( vector_type::iterator(r)+pos, vector_type::iterator(r)+pos+n );
				}
			}
		}
		assert_cstr();
		return *this;
	}

	inline
	String &
	String::copy ( const String& r, size_type pos, size_type len )
	{
		assert_pool();
		data.clear();
		if( len>0 )
		{
			uint l = r.size();
			if( pos < l )
			{
				uint n = l - pos;
				n = (n>len ? len : n);
				if( n > 0 )
				{
					data.copy( r.data.begin()+pos, r.data.begin()+pos+n );
				}
			}
		}
		assert_cstr();
		return *this;
	}

	inline
	String &
	String::append ( const String& s0 )
	{
		assert_pool();
		data.insert( data.end(), s0.data.begin(), s0.data.end() );
		assert_cstr();
		return *this;
	}

	inline
	String &
	String::append ( pcstr s0 )
	{
		assert_pool();
		if( s0 )
		{
			uint l = util::Strlen( s0 );
			data.insert( data.end(), vector_type::iterator(s0), vector_type::iterator(s0)+l );
			assert_cstr();
		}
		return *this;
	}

	inline
	String &
	String::append ( char_type c )
	{
		assert_pool();
		if( c )
		{
			data.insert( data.end(), c );
			assert_cstr();
		}
		return *this;
	}

	inline
	String &
	String::prepend ( const String& s0 )
	{
		assert_pool();
		data.insert( data.begin(), s0.data.begin(), s0.data.end() );
		assert_cstr();
		return *this;
	}

	inline
	String &
	String::prepend ( pcstr s0 )
	{
		assert_pool();
		if( s0 )
		{
			uint l = util::Strlen( s0 );
			data.insert( data.begin(), vector_type::iterator(s0), vector_type::iterator(s0)+l );
			assert_cstr();
		}
		return *this;
	}

	inline
	String &
	String::erasend ( size_type len )
	{
		assert_pool();
		uint l = data.size();
		uint n = len>l ? 0 : l-len;
		data.resize( n );
		assert_cstr();
		return *this;
	}

	inline
	String &
	String::replace ( size_type pos, size_type len, cpointer r )
	{
		assert_pool();
		erase( pos, len );
		insert( pos, r );
		return *this;
	}

	inline
	String &
	String::replace ( size_type pos, size_type len, const String& r )
	{
		assert_pool();
		erase( pos, len );
		insert( pos, r );
		return *this;
	}

	inline
	String &
	String::replace ( const String& src, const String& dst, bool cs )
	{
		assert_pool();
		int pos = indexof( src, 0, cs );
		if( pos >= 0 )
			replace( pos, src.size(), dst );
		return *this;
	}

	inline
	String &
	String::replace ( pcstr src, pcstr dst, bool cs )
	{
		assert_pool();
		int pos = indexof( src, 0, cs );
		int len = util::Strlen( src );
		if( pos >= 0 )
			replace( pos, len, dst );
		return *this;
	}

	inline
	String &
	String::replaceall ( const String& src, const String& dst, bool cs )
	{
		assert_pool();
		size_type i = 0;
		for( ;; )
		{
			int pos = indexof( src, i, cs );
			if( pos < 0 )
				break;
			replace( pos, src.size(), dst );
			i = pos + dst.size();
		}
		return *this;
	}

	inline
	String &
	String::replaceall ( pcstr src, pcstr dst, bool cs )
	{
		assert_pool();
		size_type i = 0;
		uint srclen = util::Strlen( src );
		uint dstlen = util::Strlen( dst );
		for( ;; )
		{
			int pos = indexof( src, i, cs );
			if( pos < 0 )
				break;
			replace( pos, srclen, dst );
			i = pos + dstlen;
		}
		return *this;
	}

	inline
	void
	String::assert_cstr ( )
	{
		if( data.size() )
		{
			data.push_back( 0 );
			data.pop_back();
		}
	}

	inline
	void
	String::assert_pool ( ) const
	{
		SU_ASSERT( data.pool() );
	}

	inline
	String &
	String::operator = ( const String& s0 )
	{
		return copy( s0 );
	}

	inline
	String &
	String::operator = ( pcstr s0 )
	{
		assert_pool();
		return copy( s0 );
	}

	inline
	String &
	String::operator += ( const String& s0 )
	{
		assert_pool();
		return append( s0 );
	}

	inline
	String &
	String::operator += ( pcstr s0 )
	{
		assert_pool();
		return append( s0 );
	}

	inline
	String &
	String::operator += ( char_type c )
	{
		return append( c );
	}

	inline
	String &
	String::operator = ( const creator<String> & c )
	{
		assert_pool();
		return copy( c.value() );
	}

	inline
	String::reference
	String::operator []	(	size_type	pos	)
	{
		return data.at( pos );
	}

	inline
	String::operator const char * ( ) const
	{
		assert_pool();
		return cstr();
	}

	inline
	String::creator<String>
	String::operator ()	( Pool* p ) const
	{
		return creator<String>( p, *this );
	}


	// creators

	template < typename S >
	inline
	String::creator<S>::creator ( Pool* inp, const S& invalue ) : s( inp, invalue )
	{
		SU_ASSERT( inp );
	}

	template < typename S >
	inline
	String::creator<S> &
	String::creator<S>::relocate ( Pool* inp ) const
	{
		s.relocate( inp );
		return *this;
	}

	template < typename S >
	inline
	Pool*
	String::creator<S>::pool ( ) const
	{
		return s.pool();
	}

	template < typename S >
	inline
	const S &
	String::creator<S>::value ( ) const
	{
		return s;
	}

	template < typename S > inline
	String::cpointer
	String::creator<S>::cstr ( cpointer inDef ) const
	{
		return s.cstr( inDef );
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::dup ( ) const
	{
		Pool* p = pool();
		String r(p);
		r.copy( s );
		return creator(p,r);
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::left ( size_type pos ) const
	{
		Pool* p = pool();
		String r(p);
		r.copy( s, 0, pos );
		return creator(p,r);
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::right ( size_type pos ) const
	{
		Pool* p = pool();
		String r(p);
		r.copy( s, pos, s.size() );
		return creator(p,r);
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::mid ( size_type pos, size_type len ) const
	{
		Pool* p = pool();
		String r(p);
		r.copy( s, pos, len );
		return creator(p,r);
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::front ( size_type len ) const
	{
		return left( len );
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::back ( size_type len ) const
	{
		size_type l, i;
		l = s.size();
		i = ( len>=l ? 0 : l-len );
		return right( i );
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::lower ( ) const
	{
		Pool* p = pool();
		String r(p,s);
		r.tolower();
		return creator(p,r);
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::upper ( ) const
	{
		Pool* p = pool();
		String r(p,s);
		r.toupper();
		return creator(p,r);
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::subst ( size_type pos, size_type len, const String& s0 ) const
	{
		Pool* p = pool();
		String r(p,s);
		r.replace( pos, len, s0 );
		return creator(p,r);
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::subst ( size_type pos, size_type len, pcstr s0 ) const
	{
		Pool* p = pool();
		String r(p,s);
		r.replace( pos, len, s0 );
		return creator(p,r);
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::subst ( const String& src, const String& dst, bool cs ) const
	{
		Pool* p = pool();
		String r(p,s);
		r.replace( src, dst, cs );
		return creator(p,r);
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::subst ( pcstr src, pcstr dst, bool cs ) const
	{
		Pool* p = pool();
		String r(p,s);
		r.replace( src, dst, cs );
		return creator(p,r);
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::substall ( const String& src, const String& dst, bool cs ) const
	{
		Pool* p = pool();
		String r(p,s);
		r.replaceall( src, dst, cs );
		return creator(p,r);
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::substall ( pcstr src, pcstr dst, bool cs ) const
	{
		Pool* p = pool();
		String r(p,s);
		r.replaceall( src, dst, cs );
		return creator(p,r);
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::arg ( char_type c ) const
	{
		char tmp[2];
		tmp[0] = c;
		tmp[1] = 0;
		return subst( "%%", tmp, false );
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::arg ( pcstr s0 ) const
	{
		return subst( "%%", s0, false );
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::arg ( const String& s0 ) const
	{
		return subst( "%%", s0.cstr(), false );
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::arg ( long a, int base ) const
	{
		char tmp[64];
		util::Strformat( tmp, a, base );
		return subst( "%%", tmp, false );
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::arg ( ulong a, int base ) const
	{
		char tmp[64];
		util::Strformat( tmp, a, base );
		return subst( "%%", tmp, false );
	}

	template < typename S >
	inline
	String::creator<S>
	String::creator<S>::arg ( float a, char fmt, int prec ) const
	{
		char tmp[64];
		util::Strformat( tmp, a, fmt, prec );
		return subst( "%%", tmp, false );
	}

	template < typename S >
	inline
	String::creator<S>::operator const char * ( ) const
	{
		return s.cstr();
	}

}



#endif // _su_stringi_h_


