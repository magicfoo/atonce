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



#ifndef _su_bstringi_h_
#define _su_bstringi_h_




namespace su
{

	//
	// iterator

	template <uint Bound> inline
	BString<Bound>::iterator::iterator(	)
	: ptr(NULL)
	{
		//
	}

	template <uint Bound> inline
	BString<Bound>::iterator::iterator( pointer inPtr )
	: ptr(inPtr)
	{
		//
	}

	template <uint Bound> inline
	BString<Bound>::iterator::iterator( pointer inPtr, int inLine, int inCol )
	: ptr(inPtr)
	{
		//
	}

	template <uint Bound> inline
	BString<Bound>::iterator::iterator( const iterator& it )
	: ptr(it.ptr)
	{
		//
	}

	template <uint Bound> inline
	typename BString<Bound>::iterator
	BString<Bound>::iterator::operator + (	int inN ) const
	{
		return iterator( ptr + inN );
	}

	template <uint Bound> inline
	typename BString<Bound>::iterator
	BString<Bound>::iterator::operator - (	int inN	) const
	{
		return iterator( ptr - inN );
	}

	template <uint Bound> inline
	typename BString<Bound>::iterator &
	BString<Bound>::iterator::operator += ( int inN	)
	{
		ptr += inN;
		return *this;
	}

	template <uint Bound> inline
	typename BString<Bound>::iterator &
	BString<Bound>::iterator::operator -= (	int inN	)
	{
		ptr -= inN;
		return *this;
	}

	template <uint Bound> inline
	typename BString<Bound>::iterator &
	BString<Bound>::iterator::operator ++ ( )
	{
		if( ptr )
		{
			int c = *ptr++;
		}
		return *this;
	}

	template <uint Bound> inline
	typename BString<Bound>::iterator &
	BString<Bound>::iterator::operator -- ( )
	{
		if( ptr )
		{
			int c = *--ptr;
		}
		return *this;
	}

	template <uint Bound> inline
	typename BString<Bound>::iterator
	BString<Bound>::iterator::operator ++ ( int )
	{
		iterator it( *this );
		ptr++;
		return it;
	}

	template <uint Bound> inline
	typename BString<Bound>::iterator
	BString<Bound>::iterator::operator -- ( int )
	{
		iterator it( *this );
		ptr--;
		return it;
	}

	template <uint Bound> inline
	typename BString<Bound>::size_type
	BString<Bound>::iterator::operator -	( iterator inRef ) const
	{
		SU_ASSERT( ptr >= inRef.ptr );
		return (ptr - inRef.ptr);
	}

	template <uint Bound> inline
	typename BString<Bound>::iterator &
	BString<Bound>::iterator::operator =	( iterator inRef )
	{
		ptr = inRef.ptr;
		return *this;
	}

	template <uint Bound> inline
	bool
	BString<Bound>::iterator::operator == ( iterator inRef ) const
	{
		return ( ptr == inRef.ptr );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::iterator::operator != ( iterator inRef ) const
	{
		return ( ptr != inRef.ptr );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::iterator::operator <	( iterator inRef ) const
	{
		return ( ptr < inRef.ptr );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::iterator::operator >	( iterator inRef ) const
	{
		return ( ptr > inRef.ptr );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::iterator::operator <= ( iterator inRef ) const
	{
		return ( ptr <= inRef.ptr );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::iterator::operator >= ( iterator inRef ) const
	{
		return ( ptr >= inRef.ptr );
	}

	template <uint Bound> inline
	typename BString<Bound>::reference
	BString<Bound>::iterator::operator * ( ) const
	{
		return *ptr;
	}

	template <uint Bound> inline
	typename BString<Bound>::pointer
	BString<Bound>::iterator::operator &	( ) const
	{
		return ptr;
	}

	template <uint Bound> inline
	BString<Bound>::iterator::operator const char * ( ) const
	{
		return ptr;
	}


	//
	// Bstring

	template <uint Bound> inline
	BString<Bound>::BString ( )
	{
		//
	}

	template <uint Bound> inline
	BString<Bound>::BString( const BString& r )
	{
		copy( r );
	}

	template <uint Bound> inline
	BString<Bound>::BString( pcstr r )
	{
		copy( r );
	}

	template <uint Bound> inline
	BString<Bound>::~BString()
	{
		//
	}

	template <uint Bound> inline
	bool
		BString<Bound>::isnull ( ) const
	{
		return ( this == &String::Null );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::empty (	) const
	{
		return data.empty();
	}

	template <uint Bound> inline
	typename BString<Bound>::size_type
	BString<Bound>::size	(	) const
	{
		return data.size();
	}

	template <uint Bound> inline
	typename BString<Bound>::size_type
	BString<Bound>::len	(	) const
	{
		return data.size();
	}

	template <uint Bound> inline
	typename BString<Bound>::reference
	BString<Bound>::at	(	size_type pos	) const
	{
		return data.at( pos );
	}

	template <uint Bound> inline
	typename BString<Bound>::iterator
	BString<Bound>::begin	(	) const
	{
		return iterator( &data.begin(), 0, 0 );
	}

	template <uint Bound> inline
	typename BString<Bound>::iterator
	BString<Bound>::end	(	) const
	{
		return iterator( &data.end() );
	}

	template <uint Bound> inline
	typename BString<Bound>::size_type
	BString<Bound>::capacity	(	)	const
	{
		return data.capacity();
	}

	template <uint Bound> inline
	bool BString<Bound>::matchstring( pcstr instring, bool cs ) const
	{
		uint flags = cs ? util::FNM_IGNORECASE : 0;
		return util::Fnmatch(cstr(),instring,flags);
	}

	template <uint Bound> inline
	bool BString<Bound>::matchpattern( pcstr inpattern, bool cs ) const
	{
		uint flags = cs ? util::FNM_IGNORECASE : 0;
		return util::Fnmatch(inpattern,cstr(),flags);
	}

	template <uint Bound> inline
	typename BString<Bound>::cpointer
	BString<Bound>::cstr ( cpointer inDef ) const
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

	template <uint Bound> inline
	typename BString<Bound>::pointer
	BString<Bound>::str ( pointer inDef ) const
	{
		return (pointer) cstr( inDef );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::isspace		(		) const
	{
		return util::Strisspace( cstr() );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::isalpha		(			) const
	{
		return util::Strisalpha( cstr() );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::isdigit		(			) const
	{
		return util::Strisdigit( cstr() );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::isalnum		(			) const
	{
		return util::Strisalnum( cstr() );
	}

	template <uint Bound> inline
	int
	BString<Bound>::compare		(	char_type c, bool cs	) const
	{
		return util::Strcompare( cstr(), c, cs );
	}

	template <uint Bound> inline
	int
	BString<Bound>::compare		(	pcstr s, bool cs	) const
	{
		return util::Strcompare( cstr(), s, cs );
	}

	template <uint Bound> inline
	int
	BString<Bound>::compare		(	const BString& s, bool cs	) const
	{
		return util::Strcompare( cstr(), s.cstr(), cs );
	}

	template <uint Bound> inline
	int
	BString<Bound>::compare		(	const BString& s, uint len, bool cs	) const
	{
		return util::Strcompare( cstr(), s.cstr(), len, cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::equals	(	char_type c, bool cs	) const
	{
		return util::Strequals( cstr(), c, cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::equals	(	pcstr s, bool cs	) const
	{
		return util::Strequals( cstr(), s, cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::equals	(	const BString& s, bool cs	) const
	{
		return util::Strequals( cstr(), s.cstr(), cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::equals	(	const BString& s, uint len, bool cs	) const
	{
		return util::Strequals( cstr(), s.cstr(), len, cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::startswith	(	char_type c, bool cs	) const
	{
		return util::Strstartswith( cstr(), (int)c, cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::startswith	(	pcstr s, bool cs	) const
	{
		return util::Strstartswith( cstr(), s, cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::startswith	(	const BString& s, bool cs	) const
	{
		return util::Strstartswith( cstr(), s.cstr(), cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::startswithany	(	pcstr charset, bool cs	) const
	{
		return util::Strstartswithany( cstr(), charset, cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::startswithany	(	const BString& charset, bool cs	) const
	{
		return util::Strstartswithany( cstr(), charset.cstr(), cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::endswith ( char_type c, bool cs ) const
	{
		return util::Strendswith( cstr(), (int)c, cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::endswith ( pcstr s, bool cs ) const
	{
		return util::Strendswith( cstr(), s, cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::endswith ( const BString& s, bool cs ) const
	{
		return util::Strendswith( cstr(), s.cstr(), cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::endswithany	( pcstr charset, bool cs ) const
	{
		return util::Strendswithany( cstr(), charset, cs );
	}

	template <uint Bound> inline
	bool
	BString<Bound>::endswithany	( const BString& charset, bool cs ) const
	{
		return util::Strendswithany( cstr(), charset.cstr(), cs );
	}

	template <uint Bound> inline
	int
	BString<Bound>::offsetof ( pcstr p ) const
	{
		pcstr b = begin();
		pcstr e = end();
		SU_ASSERT( p>=b && p<e );
		if( p>=b && p<e )	return p-b;
		else				return -1;
	}

	template <uint Bound> inline
	int
	BString<Bound>::offsetof ( iterator p ) const
	{
		pcstr b = begin();
		pcstr e = end();
		SU_ASSERT( p>=b && p<e );
		if( p>=b && p<e )	return p-b;
		else				return -1;
	}

	template <uint Bound> inline
	int
	BString<Bound>::indexof	(	char_type c, size_type pos, bool cs, bool rev ) const
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

	template <uint Bound> inline
	int
	BString<Bound>::indexof ( const BString& r, size_type pos, bool cs, bool rev ) const
	{
		return indexof( r.cstr(), pos, cs, rev );
	}

	template <uint Bound> inline
	int
	BString<Bound>::indexof ( pcstr r, size_type pos, bool cs, bool rev ) const
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

	template <uint Bound> inline
	int
	BString<Bound>::indexofany ( const BString& charset, size_type pos, bool cs, bool rev ) const
	{
		return indexofany( charset.cstr(), pos, cs, rev );
	}

	template <uint Bound> inline
	int
	BString<Bound>::indexofany ( pcstr charset, size_type pos, bool cs, bool rev ) const
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

	template <uint Bound> inline
	typename BString<Bound>::size_type
	BString<Bound>::tonumber (	long& a, int base, size_type pos	) const
	{
		pstr endp = NULL;
		pcstr nptr = cstr();
		a = util::Strtolong( nptr, &endp, base );
		if( !endp || endp==nptr )
			return 0;
		return (endp - nptr);
	}

	template <uint Bound> inline
	typename BString<Bound>::size_type
	BString<Bound>::tonumber ( ulong& a, int base, size_type pos	) const
	{
		pstr endp = NULL;
		pcstr nptr = cstr();
		a = util::Strtoulong( nptr, &endp, base );
		if( !endp || endp==nptr )
			return 0;
		return (endp - nptr);
	}

	template <uint Bound> inline
	typename BString<Bound>::size_type
	BString<Bound>::tonumber ( float& a, size_type pos ) const
	{
		pstr endp = NULL;
		pcstr nptr = cstr();
		a = util::Strtofloat( nptr, &endp );
		if( !endp || endp==nptr )
			return 0;
		return (endp - nptr);
	}

	template <uint Bound> inline
	void
	BString<Bound>::split ( size_type pos, BString& outl, BString& outr ) const
	{
		BString l, r;
		l.copy( *this, 0, pos );
		r.copy( *this, pos, size() );
		outl.copy( l );
		outr.copy( r );
	}

	template <uint Bound> inline
	void
	BString<Bound>::splitsep ( pcstr seplist, BString& outl, BString& outr ) const
	{
		int pos = indexofany( seplist );
		if( pos < 0 )
		{
			outl.clear();
			outr.clear();
		}
		else
		{
			BString l, r;
			l.copy( *this, 0, pos );
			r.copy( *this, pos+1, size() );
			outl.copy( l );
			outr.copy( r );
		}
	}

	template <uint Bound> inline
	void
	BString<Bound>::splitsep ( const BString& seplist, BString& outl, BString& outr ) const
	{
		splitsep( seplist.cstr(), outl, outr );
	}

	template <uint Bound> inline
	uint32
	BString<Bound>::hash32 ( bool innormalized ) const
	{
        return util::hmac_crc32_w( cstr(), innormalized );
	}

	template <uint Bound> inline
	uint128
	BString<Bound>::hash128 ( ) const
	{
        return util::hmac_md5_w( data.data(), data.size() );
	}


	// modifiers

	template <uint Bound> inline
	void
	BString<Bound>::reserve ( size_type n )
	{
		data.reserve( n );
		assert_cstr();
	}

	template <uint Bound> inline
	void
	BString<Bound>::resize ( size_type n, char_type c )
	{
		data.resize( n, c );
		assert_cstr();
	}

	template <uint Bound> inline
	void
	BString<Bound>::squeeze ( )
	{
		// keep reserve of 1 byte for \0 terminator
		data.reserve( data.size()+1 );
		assert_cstr();
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::clear ( )
	{
		data.clear();
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::strip ( )
	{
		while( startswithany(" \n\t\r") )		erase(0,1);
		while( endswithany(" \n\t\r") )			erasend(1);
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::erase ( size_type pos, size_type len )
	{
		if( pos<size() && len>0 )
		{
			typename vector_type::iterator it0 = data.begin() + pos;
			typename vector_type::iterator it1 = it0 + len;
			if( it1 > data.end() )
				it1 = data.end();
			data.erase( it0, it1 );
			assert_cstr();
		}
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::insert ( size_type pos, const BString& r )
	{
		data.insert( data.begin()+pos, r.data.begin(), r.data.end() );
		assert_cstr();
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::insert ( size_type pos, cpointer r )
	{
		if( r )
		{
			int l = util::Strlen( r );
			data.insert( data.begin()+pos, vector_iterator(r), vector_iterator(r)+l );
			assert_cstr();
		}
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::copy ( const BString& r )
	{
		data.copy( r.data );
		assert_cstr();
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::copy ( cpointer r )
	{
		data.clear();
		if( r )
		{
			int l = util::Strlen( r );
			data.copy( vector_iterator(r), vector_iterator(r)+l );
		}
		assert_cstr();
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::copy ( cpointer r, size_type pos, size_type len )
	{
		data.clear();
		if( r && len>0 )
		{
			int l = util::Strlen( r );
			if( (int)pos < l )
			{
				int n = l - pos;
				n = (n>(int)len ? len : n);
				if( n > 0 )
				{
					data.copy( vector_iterator(r)+pos, vector_iterator(r)+pos+n );
				}
			}
		}
		assert_cstr();
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::copy ( const BString& r, size_type pos, size_type len )
	{
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

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::tolower ( )
	{
		util::Strtolower( (pstr)cstr() );
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::toupper ( )
	{
		util::Strtoupper( (pstr)cstr() );
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::append ( const BString& s0 )
	{
		data.insert( data.end(), s0.data.begin(), s0.data.end() );
		assert_cstr();
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::append ( pcstr s0 )
	{
		if( s0 )
		{
			int l = util::Strlen( s0 );
			data.insert( data.end(), vector_iterator(s0), vector_iterator(s0)+l );
			assert_cstr();
		}
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::append ( char_type c )
	{
		if( c )
		{
			data.insert( data.end(), c );
			assert_cstr();
		}
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::prepend ( const BString& s0 )
	{
		data.insert( data.begin(), s0.data.begin(), s0.data.end() );
		assert_cstr();
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::prepend ( pcstr s0 )
	{
		if( s0 )
		{
			int l = util::Strlen( s0 );
			data.insert( data.begin(), vector_iterator(s0), vector_iterator(s0)+l );
			assert_cstr();
		}
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::erasend ( size_type len )
	{
		uint l = data.size();
		uint n = len>l ? 0 : l-len;
		data.resize( n );
		assert_cstr();
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::replace ( size_type pos, size_type len, cpointer r )
	{
		erase( pos, len );
		insert( pos, r );
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::replace ( size_type pos, size_type len, const BString& r )
	{
		erase( pos, len );
		insert( pos, r );
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::replace ( const BString& src, const BString& dst, bool cs )
	{
		int pos = indexof( src, 0, cs );
		if( pos >= 0 )
			replace( pos, src.size(), dst );
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::replace ( pcstr src, pcstr dst, bool cs )
	{
		int pos = indexof( src, 0, cs );
		int len = util::Strlen( src );
		if( pos >= 0 )
			replace( pos, len, dst );
		return *this;
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::replaceall ( const BString& src, const BString& dst, bool cs )
	{
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

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::replaceall ( pcstr src, pcstr dst, bool cs )
	{
		size_type i = 0;
		int srclen = util::Strlen( src );
		int dstlen = util::Strlen( dst );
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

	template <uint Bound> inline
	void
	BString<Bound>::assert_cstr ( )
	{
		if( data.size() )
		{
			data.push_back( 0 );
			data.pop_back();
		}
	}


	// creators

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::dup ( ) const
	{
		return BString(*this);
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::left ( size_type pos ) const
	{
		BString r;
		r.copy( *this, 0, pos );
		return BString(r);
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::right ( size_type pos ) const
	{
		BString r;
		r.copy( *this, pos, size() );
		return BString(r);
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::mid ( size_type pos, size_type len ) const
	{
		BString r;
		r.copy( *this, pos, len );
		return BString(r);
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::front ( size_type len ) const
	{
		return left( len );
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::back ( size_type len ) const
	{
		size_type l, i;
		l = size();
		i = ( len>=l ? 0 : l-len );
		return right( i );
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::lower ( ) const
	{
		BString r = *this;
		r.tolower();
		return BString( r );
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::upper ( ) const
	{
		BString r = *this;
		r.toupper();
		return BString( r );
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::subst ( size_type pos, size_type len, const BString& s0 ) const
	{
		BString r( *this );
		r.replace( pos, len, s0 );
		return BString(r);
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::subst ( size_type pos, size_type len, pcstr s0 ) const
	{
		BString r( *this );
		r.replace( pos, len, s0 );
		return BString(r);
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::subst ( const BString& src, const BString& dst, bool cs ) const
	{
		BString r( *this );
		r.replace( src, dst, cs );
		return BString(r);
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::subst ( pcstr src, pcstr dst, bool cs ) const
	{
		BString r( *this );
		r.replace( src, dst, cs );
		return BString(r);
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::substall ( const BString& src, const BString& dst, bool cs ) const
	{
		BString r( *this );
		r.replaceall( src, dst, cs );
		return BString(r);
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::substall ( pcstr src, pcstr dst, bool cs ) const
	{
		BString r( *this );
		r.replaceall( src, dst, cs );
		return BString(r);
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::arg ( char_type c, int rep ) const
	{
		SU_ASSERT( rep < 256 );
		if( rep < 256 )
		{
			char tmp[256];
			for( uint i=0 ; i<rep ; i++ )
				tmp[i] = c;
			tmp[rep] = 0;
			return subst( "%%", tmp, false );
		}
		else
		{
			return BString( *this );
		}
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::arg ( pcstr s0 ) const
	{
		return subst( "%%", s0, false );
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::arg ( const BString& s0 ) const
	{
		return subst( "%%", s0.cstr(), false );
	}


	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::arg ( long a, int base ) const
	{
		char tmp[64];
		util::Strformat( tmp, a, base );
		return subst( "%%", tmp, false );
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::arg ( ulong a, int base ) const
	{
		char tmp[64];
		util::Strformat( tmp, a, base );
		return subst( "%%", tmp, false );
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::arg ( float a, char fmt, int prec ) const
	{
		char tmp[64];
		util::Strformat( tmp, a, fmt, prec );
		return subst( "%%", tmp, false );
	}


	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::argl ( long a, int base ) const
	{
		return arg( a, base );
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::argul ( ulong a, int base ) const
	{
		return arg( a, base );
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::argf ( float a, char fmt, int prec ) const
	{
		return arg( a, fmt, prec );
	}


	template <uint Bound> inline
	typename BString<Bound>::reference
	BString<Bound>::operator []	(	size_type	pos	)
	{
		return data.at( pos );
	}

	template <uint Bound> inline
	BString<Bound>::operator const char * ( ) const
	{
		return cstr();
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::operator = ( const BString& s0 )
	{
		return copy( s0 );
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::operator = ( pcstr s0 )
	{
		return copy( s0 );
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::operator += ( const BString& s0 )
	{
		return append( s0 );
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::operator += ( pcstr s0 )
	{
		return append( s0 );
	}

	template <uint Bound> inline
	BString<Bound> &
	BString<Bound>::operator += ( char_type c )
	{
		return append( c );
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::operator + ( const BString& s0 ) const
	{
		BString r( *this );
		r.append( s0 );
		return BString( r );
	}

	template <uint Bound> inline
	BString<Bound>
	BString<Bound>::operator + ( char_type c ) const
	{
		BString r( *this );
		r.append( c );
		return BString( r );
	}

}



#endif // _su_bstringi_h_


