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




#ifndef _su_multiseti_h_
#define _su_multiseti_h_



namespace su
{
	#define	SU_MULTISET_DECL	template <typename Key, typename Compare> inline
	#define	SU_MULTISET_DEF		MultiSet<Key,Compare>
	#define	SU_IT_DECL			SU_MULTISET_DECL
	#define	SU_IT_DEF			SU_MULTISET_DEF::iterator

	SU_IT_DECL
	SU_IT_DEF::iterator(	) : ptr( NULL )
	{
		//
	}

	SU_IT_DECL
	SU_IT_DEF::iterator	(	node*	n	) : ptr( n )
	{
		//
	}

	SU_IT_DECL
	SU_IT_DEF::iterator(	const iterator&	it	) : ptr( it.ptr )
	{
		//
	}

	SU_IT_DECL
	typename SU_IT_DEF
	SU_IT_DEF::operator + (	int	count ) const
	{
		node* n = ptr;
		while( n && count-- )
			n = (node*) n->_next;
		return iterator( n );
	}

	SU_IT_DECL
	typename SU_IT_DEF
	SU_IT_DEF::operator - (	int	count ) const
	{
		SU_ASSERTC( ptr, "end iterator can't decrement !" );
		node* n = ptr;
		while( n && count-- )
			n = (node*) n->_prev;
		return iterator( n );
	}

	SU_IT_DECL
	typename SU_IT_DEF &
	SU_IT_DEF::operator += ( int count )
	{
		while( ptr && count-- )
			ptr = (node*) ptr->_next;
		return *this;
	}

	SU_IT_DECL
	typename SU_IT_DEF &
	SU_IT_DEF::operator -= ( int count )
	{
		SU_ASSERTC( ptr, "end iterator can't decrement !" );
		while( ptr && count-- )
			ptr = (node*) ptr->_prev;
		return *this;
	}

	SU_IT_DECL
	typename SU_IT_DEF &
	SU_IT_DEF::operator ++ (				)
	{
		if( ptr )
			ptr = (node*) ptr->_next;
		return *this;
	}

	SU_IT_DECL
	typename SU_IT_DEF &
	SU_IT_DEF::operator --	(			)
	{
		SU_ASSERTC( ptr, "end iterator can't decrement !" );
		if( ptr )
			ptr = (node*) ptr->_prev;
		return *this;
	}

	SU_IT_DECL
	typename SU_IT_DEF
	SU_IT_DEF::operator ++ (	int		)
	{
		iterator it( *this );
		if( ptr )
			ptr = (node*) ptr->_next;
		return it;
	}

	SU_IT_DECL
	typename SU_IT_DEF
	SU_IT_DEF::operator --	(	int		)
	{
		SU_ASSERTC( ptr, "end iterator can't decrement !" );
		iterator it( *this );
		if( ptr )
			ptr = (node*) ptr->_prev;
		return it;
	}

	SU_IT_DECL
	typename SU_IT_DEF &
	SU_IT_DEF::operator =	(	iterator inRef )
	{
		ptr = inRef.ptr;
		return *this;
	}

	SU_IT_DECL
	bool
	SU_IT_DEF::operator ==	(	iterator inRef ) const
	{
		return ( ptr == inRef.ptr );
	}

	SU_IT_DECL
	bool
	SU_IT_DEF::operator !=	(	iterator inRef ) const
	{
		return ( ptr != inRef.ptr );
	}

	SU_IT_DECL
	typename SU_IT_DEF::pointer
	SU_IT_DEF::operator ->	(		) const
	{
		return &ptr->key;
	}

	SU_IT_DECL
	typename SU_IT_DEF::reference
	SU_IT_DEF::operator *	(		) const
	{
		return ptr->key;
	}

	SU_IT_DECL
	typename SU_IT_DEF::pointer
	SU_IT_DEF::operator &	(		) const
	{
		return &ptr->key;
	}

	SU_MULTISET_DECL
	SU_MULTISET_DEF::MultiSet	(	Pool*	p	)
	{
		SU_ASSERT( p );
		mp = p;
	}

	SU_MULTISET_DECL
	SU_MULTISET_DEF::MultiSet	(	Pool*	p, const MultiSet &	inref )
	{
		SU_ASSERT( p );
		mp = p;
		insert( inref.begin(), inref.end() );
	}

	SU_MULTISET_DECL
	SU_MULTISET_DEF::MultiSet	(	Pool*	p, iterator itfirst, iterator itlast	)
	{
		SU_ASSERT( p );
		mp = p;
		insert( itfirst, itlast );
	}

	SU_MULTISET_DECL
	SU_MULTISET_DEF::~MultiSet	(	)
	{
		free();
	}

	SU_MULTISET_DECL
	typename SU_IT_DEF
	SU_MULTISET_DEF::begin	(		) const
	{
		return iterator( tr.first() );
	}

	SU_MULTISET_DECL
	typename SU_IT_DEF
	SU_MULTISET_DEF::last	(		) const
	{
		return iterator( tr.last() );
	}

	SU_MULTISET_DECL
	typename SU_IT_DEF
	SU_MULTISET_DEF::end	(		) const
	{
		return iterator( NULL );
	}

	SU_MULTISET_DECL
	typename SU_MULTISET_DEF::size_type
	SU_MULTISET_DEF::size	(		) const
	{
		return tr.size();
	}

	SU_MULTISET_DECL
	bool
	SU_MULTISET_DEF::empty	(		) const
	{
		return tr.size() == 0;
	}

	SU_MULTISET_DECL
	typename SU_MULTISET_DEF::reference
	SU_MULTISET_DEF::front	(		) const
	{
		SU_ASSERT( tr.first() );
		return tr.first()->key;
	}

	SU_MULTISET_DECL
	typename SU_MULTISET_DEF::reference
	SU_MULTISET_DEF::back	(		) const
	{
		SU_ASSERT( tr.last() );
		return tr.last()->key;
	}

	SU_MULTISET_DECL
	typename SU_IT_DEF
	SU_MULTISET_DEF::find	(	const value_type &	key		)
	{
		node* n = tr.find( key );
		return iterator( n );
	}

	SU_MULTISET_DECL
	typename SU_IT_DEF
	SU_MULTISET_DEF::insert	(	const value_type &	key, size_type count )
	{
		node* f = NULL;
		while( count-- )
		{
			node* n;
			n = mp->New<node>();
			n->key = key;
			node* k = tr.insert( n );
			SU_ASSERT( k );
			if( !f )
				f = k;
		}
		return iterator( f );
	}

	SU_MULTISET_DECL
	void
	SU_MULTISET_DEF::insert	(	iterator itfirst, iterator itlast )
	{
		iterator it = itfirst;
		while( it.ptr && it!=itlast )
			insert( *it++ );
	}

	SU_MULTISET_DECL
	typename SU_IT_DEF
	SU_MULTISET_DEF::erase	(	iterator	it )
	{
		node* n = it.ptr;
		if( !n )
		{
			return iterator( NULL );
		}
		else
		{
			node* p = (node*) n->_next;
			tr.erase( n );
			mp->Delete( n );
			return iterator( p );
		}
		//return NULL;
	}

	SU_MULTISET_DECL
	typename SU_IT_DEF
	SU_MULTISET_DEF::erase	(	iterator itfirst, iterator itlast	)
	{
		iterator it = itfirst;
		while( it.ptr && it!=itlast )
			it = erase( it );
		return it;
	}

	SU_MULTISET_DECL
	void
	SU_MULTISET_DEF::clear	(			)
	{
		erase( begin(), end() );
	}

	SU_MULTISET_DECL
	void
	SU_MULTISET_DEF::copy	(	const MultiSet &		inref	)
	{
		clear();
		insert( inref.begin(), inref.end() );
	}

	SU_MULTISET_DECL
	void
	SU_MULTISET_DEF::copy	(	iterator itfirst, iterator itlast	)
	{
		clear();
		insert( itfirst, itlast );
	}

	SU_MULTISET_DECL
	void
	SU_MULTISET_DEF::free	(				)
	{
		clear();
	}

	#undef	SU_MULTISET_DECL
	#undef	SU_MULTISET_DEF
	#undef	SU_IT_DECL
	#undef	SU_IT_DEF

}




#endif // _su_multiseti_h_




