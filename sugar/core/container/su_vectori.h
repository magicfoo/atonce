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




#ifndef _su_vectori_h_
#define _su_vectori_h_



namespace su
{

	// iterator

	template <typename T, uint B> inline
	vector<T,B>::iterator::iterator ( ) : ptr( NULL )
	{
		//
	}

	template <typename T, uint B> inline
	vector<T,B>::iterator::iterator ( pointer inPtr ) : ptr( inPtr )
	{
		//
	}

	template <typename T, uint B> inline
	vector<T,B>::iterator::iterator ( cpointer inPtr ) : ptr( (pointer)inPtr )
	{
		//
	}

	template <typename T, uint B> inline
	vector<T,B>::iterator::iterator ( const iterator& it ) : ptr( it.ptr )
	{
		//
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator
	vector<T,B>::iterator::operator + (	int n ) const
	{
		return iterator( ptr + n );
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator
	vector<T,B>::iterator::operator - (	int n ) const
	{
		return iterator( ptr - n );
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator &
	vector<T,B>::iterator::operator += ( int n )
	{
		ptr += n;
		return *this;
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator &
	vector<T,B>::iterator::operator -= ( int inN )
	{
		ptr -= inN;
		return *this;
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator &
	vector<T,B>::iterator::operator ++ ( )
	{
		ptr++;
		return *this;
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator
	vector<T,B>::iterator::operator ++ ( int )
	{
		pointer p = ptr++;
		return iterator(p);
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator &
	vector<T,B>::iterator::operator -- ( )
	{
		ptr--;
		return *this;
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator
	vector<T,B>::iterator::operator -- ( int )
	{
		pointer p = ptr--;
		return iterator(p);
	}

	template <typename T, uint B> inline
	typename vector<T,B>::size_type
	vector<T,B>::iterator::operator - ( iterator inRef ) const
	{
		SU_ASSERT( ptr >= inRef.ptr );
		return (ptr - inRef.ptr );
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator &
	vector<T,B>::iterator::operator = ( iterator inRef )
	{
		ptr = inRef.ptr;
		return *this;
	}

	template <typename T, uint B> inline
	bool
	vector<T,B>::iterator::operator == ( iterator inRef ) const
	{
		return ( ptr == inRef.ptr );
	}

	template <typename T, uint B> inline
	bool
	vector<T,B>::iterator::operator != ( iterator inRef ) const
	{
		return ( ptr != inRef.ptr );
	}

	template <typename T, uint B> inline
	bool
	vector<T,B>::iterator::operator < ( iterator inRef ) const
	{
		return ( ptr < inRef.ptr );
	}

	template <typename T, uint B> inline
	bool
	vector<T,B>::iterator::operator > ( iterator inRef ) const
	{
		return ( ptr > inRef.ptr );
	}

	template <typename T, uint B> inline
	bool
	vector<T,B>::iterator::operator <= ( iterator inRef ) const
	{
		return ( ptr <= inRef.ptr );
	}

	template <typename T, uint B> inline
	bool
	vector<T,B>::iterator::operator >= ( iterator inRef ) const
	{
		return ( ptr >= inRef.ptr );
	}

	template <typename T, uint B> inline
	typename vector<T,B>::pointer
	vector<T,B>::iterator::operator -> ( ) const
	{
		return ptr;
	}

	template <typename T, uint B> inline
	typename vector<T,B>::reference
	vector<T,B>::iterator::operator * ( ) 
	{
		return *ptr;
	}

	template <typename T, uint B> inline
	typename vector<T,B>::pointer
	vector<T,B>::iterator::operator & ( ) 
	{
		return ptr;
	}


	// vector

	template <typename T, uint B> inline
	vector<T,B>::vector ( Pool* p )
	{
		vfirst = vlast = vend = NULL;
		vp = p;
	}

	template <typename T, uint B> inline
	vector<T,B>::vector ( const vector& inref )
	{
		vfirst = vlast = vend = NULL;
		vp = inref.vp;
		copy( inref );
	}

	template <typename T, uint B> inline
	vector<T,B>::vector ( Pool* p, const vector& inref )
	{
		vfirst = vlast = vend = NULL;
		vp = p;
		copy( inref );
	}

	template <typename T, uint B> inline
	vector<T,B>::~vector ( )
	{
		reserve(0);
	}

	template <typename T, uint B> inline
	void
	vector<T,B>::relocate ( Pool* p )
	{
		if( p )
		{
			if( p!=vp )
			{
				uint cap = capacity() ;
				uint sz  = size();

				if( cap )
				{
					T* ra = util::Objectnew<T>( p, cap );
					SU_ASSERT( ra );

					util::Objectcopy( ra, vfirst, cap );
					util::Objectdtor( vfirst, cap );
					vp->Free( vfirst );

					vfirst = ra;
					vlast  = ra + sz;
					vend   = ra + cap;
				}

				vp = p;
			}
		}
		else
		{
			reserve( 0 );
			vfirst = NULL;
			vlast  = NULL;
			vend   = NULL;
			vp     = NULL;
		}
	}

	template <typename T, uint B> inline
	Pool*
	vector<T,B>::pool ( ) const
	{
		return vp;
	}

	template <typename T, uint B> inline
	bool
	vector<T,B>::hascdtor ( ) const
	{
		return support_yes_or_undefined< has_user_cdtor<T> >::value;
	}

	template <typename T, uint B> inline
	typename vector<T,B>::reference
	vector<T,B>::at ( size_type inPos ) const
	{
		SU_ASSERT( vfirst );
		SU_ASSERT( inPos < size() );
		return vfirst[inPos];
	}

	template <typename T, uint B> inline
	typename vector<T,B>::pointer
	vector<T,B>::addr ( size_type inPos ) const
	{
		SU_ASSERT( vfirst );
		if( inPos < size() )	return vfirst+inPos;
		else					return NULL;
	}

	template <typename T, uint B> inline
	typename vector<T,B>::pointer
	vector<T,B>::data ( ) const
	{
		return empty() ? NULL : vfirst;
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator
	vector<T,B>::begin ( ) const
	{
		return iterator( vfirst );
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator
	vector<T,B>::end ( ) const
	{
		return iterator( vlast );
	}

	template <typename T, uint B> inline
	typename vector<T,B>::size_type
	vector<T,B>::size ( ) const
	{
		SU_ASSERT( (vfirst==NULL) || (vlast>=vfirst) );
		size_type sz = 0;
		if( vfirst )
			sz = vlast-vfirst;
		SU_ASSERT( (B==0) || (sz<=B) );
		return sz;
	}

	template <typename T, uint B> inline
	typename vector<T,B>::size_type
	vector<T,B>::bsize ( ) const
	{
		return size() * sizeof(T);
	}

	template <typename T, uint B> inline
	typename vector<T,B>::size_type
	vector<T,B>::capacity ( ) const
	{
		SU_ASSERT( (vfirst==NULL) || (vend>=vfirst) );
		size_type cap = 0;
		if( vfirst )
			cap = vend-vfirst;
		SU_ASSERT( (B==0) || (cap<=B) );
		return cap;
	}

	template <typename T, uint B> inline
	bool
	vector<T,B>::empty ( ) const
	{
		return (size()==0);
	}

	template <typename T, uint B> inline
	typename vector<T,B>::reference
	vector<T,B>::front ( ) const
	{
		SU_ASSERT( vfirst );
		return *vfirst;
	}

	template <typename T, uint B> inline
	typename vector<T,B>::reference
	vector<T,B>::back ( ) const
	{
		SU_ASSERT( vfirst );
		SU_ASSERT( vlast );
		SU_ASSERT( vlast >= vfirst );
		return empty() ? *vfirst : *(vlast-1);
	}


	// modifiers

	template <typename T, uint B> inline
	void
	vector<T,B>::reserve ( size_type inCap )
	{
		size_type sz  = size();
		size_type cap = capacity();
		SU_ASSERT( sz <= cap );

		if( inCap==cap )
			return;

		if( inCap<sz )
		{
			resize( inCap );
			sz = size();
		}

		if( inCap==0 )
		{
			// free !
			if( B==0 )
			{
				SU_ASSERT( vp );
				vp->Free( vfirst );
			}
			vfirst = vlast = vend = NULL;
		}
		else if( B>0 )
		{
			// bounded !
			SU_ASSERT( inCap <= B );
			vfirst = (T*) vbytes;
			vlast = vfirst + sz;
			vend = vfirst + inCap;
		}
		else
		{
			// mem alloc from pool
			SU_ASSERT( vp );
			T* ra = util::Objectnew<T>( vp, inCap );
			SU_ASSERT( ra );
			util::Objectcopy( ra, vfirst, sz );
			util::Objectdtor( vfirst, sz );
			vp->Free( vfirst );
			vfirst = ra;
			vlast = vfirst + sz;
			vend = vfirst + inCap;
		}
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator
	vector<T,B>::insert ( iterator inPos, iterator inFirst, iterator inEnd )
	{
		SU_ASSERT( &inPos >= vfirst );
		SU_ASSERT( &inPos <= vlast );

		size_type sz = size();
		size_type nb = inEnd - inFirst;
		size_type po = vfirst ? (&inPos-vfirst) : 0;
		SU_ASSERT( po <= sz );

		if( nb==0 )
			return inPos;

		if( sz+nb > capacity() )
		{
			size_type ncaplo = sz+nb;
			size_type ncaphi = ncaplo*2;
			if( B>0 )
			{
				SU_ASSERT( ncaplo <= B );
				if( ncaphi>B )	ncaphi = B;
			}
			reserve( ncaphi );
		}

		pointer i0 = vfirst + po;
		pointer i1 = i0 + nb;
		pointer s0 = &inFirst;

		util::Objectctor( vlast, nb );
		util::Objectmove( i1, i0, sz-po );
		util::Objectcopy( i0, s0, nb );

		vlast += nb;
		return iterator( vfirst+po );
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator
	vector<T,B>::insert ( iterator inPos, const value_type & inValue, size_type inSize )
	{
		SU_ASSERT( &inPos >= vfirst );
		SU_ASSERT( &inPos <= vlast );

		size_type sz = size();
		size_type nb = inSize;
		size_type po = vfirst ? (&inPos-vfirst) : 0;
		SU_ASSERT( po <= sz );

		if( nb==0 )
			return inPos;

		if( sz+nb > capacity() )
		{
			size_type ncaplo = sz+nb;
			size_type ncaphi = ncaplo*2;
			if( B>0 )
			{
				SU_ASSERT( ncaplo <= B );
				if( ncaphi>B )	ncaphi = B;
			}
			reserve( ncaphi );
		}

		pointer i0 = vfirst + po;
		pointer i1 = i0 + nb;
		pointer s0 = (T*) &inValue;

		util::Objectctor( vlast, nb );
		util::Objectmove( i1, i0, sz-po );
		util::Objectncopy( i0, s0, nb );

		vlast += nb;
		return iterator( vfirst+po );
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator
	vector<T,B>::erase ( iterator inFirst, iterator inEnd )
	{
		if( inFirst == inEnd )
			return inEnd;

		SU_ASSERT( capacity() );
		SU_ASSERT( &inFirst >= vfirst );
		SU_ASSERT( &inFirst <= vlast );
		SU_ASSERT( &inEnd >= vfirst );
		SU_ASSERT( &inEnd <= vlast );
		SU_ASSERT( &inFirst <= &inEnd );

		pointer   e0 = &inFirst;
		pointer   e1 = &inEnd;
		size_type nb = e1 - e0;
		size_type po = e0 - vfirst;
		size_type ct = vlast - e1;

		util::Objectmove( e0, e1, ct );
		util::Objectdtor( vlast-nb, nb );

		vlast -= nb;
		return iterator( vfirst+po );
	}

	template <typename T, uint B> inline
	typename vector<T,B>::iterator
	vector<T,B>::erase ( iterator inPos )
	{
		return erase( inPos, inPos+1 );
	}

	template <typename T, uint B> inline
	void
	vector<T,B>::resize ( size_type inSize, const value_type& inValue )
	{
		size_type sz = size();
		if( inSize<sz )
			erase( begin()+inSize, end() );
		else if( inSize>sz )
			insert( end(), inValue, (inSize-sz) );
	}

	template <typename T, uint B> inline
	void
	vector<T,B>::squeeze ( )
	{
		size_type sz = size();
		reserve( sz );
	}

	template <typename T, uint B> inline
	void
	vector<T,B>::clear ( )
	{
		resize( 0 );
	}

	template <typename T, uint B> inline
	void
	vector<T,B>::copy ( const vector & inRef )
	{
		if( B==0 && vp==NULL )
		{
			// uses the ref pool !
			relocate( inRef.vp );
		}

		clear();
		insert( begin(), inRef.begin(), inRef.end() );
	}

	template <typename T, uint B> inline
	void
	vector<T,B>::copy (	iterator inFirst, iterator inEnd )
	{
		clear();
		insert( begin(), inFirst, inEnd );
	}

	template <typename T, uint B> inline
	typename vector<T,B>::reference
	vector<T,B>::alloc_back ( size_type inSize )
	{
		return *insert( end(), T(), inSize );
	}

	template <typename T, uint B> inline
	void
	vector<T,B>::push_back ( const value_type &	inT )
	{
		insert( end(), inT, 1 );
	}

	template <typename T, uint B> inline
	void
	vector<T,B>::pop_back ( )
	{
		if( size() )
		{
			iterator e = end();
			erase( e-1, e );
		}
	}

	template <typename T, uint B> inline
	typename vector<T,B>::reference
	vector<T,B>::operator [] ( size_type inPos ) const
	{
		return at(inPos);
	}

	template <typename T, uint B> inline
	vector<T,B>&
	vector<T,B>::operator = ( const vector& inref )
	{
		copy( inref );
		return *this;
	}

}



#endif // _su_vectori_h_


