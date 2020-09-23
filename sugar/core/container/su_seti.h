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




#ifndef _su_seti_h_
#define _su_seti_h_



namespace su
{
	#define	SU_SET_DECL		template <typename Key, typename Compare> inline
	#define	SU_SET_DEF		Set<Key,Compare>
	#define	SU_IT_DECL		SU_SET_DECL
	#define	SU_IT_DEF		SU_SET_DEF::iterator


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

	SU_SET_DECL
	SU_SET_DEF::Set	(	Pool* p	)
	{
		SU_ASSERT( p );
		sp = p;
	}

	SU_SET_DECL
	SU_SET_DEF::Set	(	Pool* p, const Set &	inref )
	{
		SU_ASSERT( p );
		sp = p;
		insert( inref.begin(), inref.end() );
	}

	SU_SET_DECL
	SU_SET_DEF::Set	(	Pool* p, iterator itfirst, iterator itlast	)
	{
		SU_ASSERT( p );
		sp = p;
		insert( itfirst, itlast );
	}

	SU_SET_DECL
	SU_SET_DEF::~Set	(	)
	{
		free();
	}

	SU_SET_DECL
	typename SU_IT_DEF
	SU_SET_DEF::begin	(		) const
	{
		return iterator( tr.first() );
	}

	SU_SET_DECL
	typename SU_IT_DEF
	SU_SET_DEF::last	(		) const
	{
		return iterator( tr.last() );
	}

	SU_SET_DECL
	typename SU_IT_DEF
	SU_SET_DEF::end	(		) const
	{
		return iterator( NULL );
	}

	SU_SET_DECL
	typename SU_SET_DEF::size_type
	SU_SET_DEF::size	(		) const
	{
		return tr.size();
	}

	SU_SET_DECL
	bool
	SU_SET_DEF::empty	(		) const
	{
		return tr.size() == 0;
	}

	SU_SET_DECL
	typename SU_SET_DEF::reference
	SU_SET_DEF::front	(		) const
	{
		SU_ASSERT( tr.first() );
		return tr.first()->key;
	}

	SU_SET_DECL
	typename SU_SET_DEF::reference
	SU_SET_DEF::back	(		) const
	{
		SU_ASSERT( tr.last() );
		return tr.last()->key;
	}

	SU_SET_DECL
	typename SU_IT_DEF
	SU_SET_DEF::find	(	const value_type &	key		)
	{
		node* n = tr.find( key );
		return iterator( n );
	}

	SU_SET_DECL
	typename SU_IT_DEF
	SU_SET_DEF::insert	(	const value_type &	key		)
	{
		node* n;
		n = sp->New<node>();

		n->key = key;
		if( tr.insertUnique(n) )
		{
			return iterator(n);
		}
		else
		{
			sp->Delete(n);
			return iterator( NULL );
		}
	}

	SU_SET_DECL
	void
	SU_SET_DEF::insert	(	iterator itfirst, iterator itlast )
	{
		iterator it = itfirst;
		while( it.ptr && it!=itlast )
			insert( *it++ );
	}

	SU_SET_DECL
	typename SU_IT_DEF
	SU_SET_DEF::erase	(	iterator	it )
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
			sp->Delete(n);
			return iterator( p );
		}
	}

	SU_SET_DECL
	typename SU_IT_DEF
	SU_SET_DEF::erase	(	iterator itfirst, iterator itlast	)
	{
		iterator it = itfirst;
		while( it.ptr && it!=itlast )
			it = erase( it );
		return it;
	}

	SU_SET_DECL
	void
	SU_SET_DEF::clear	(			)
	{
		erase( begin(), end() );
	}

	SU_SET_DECL
	void
	SU_SET_DEF::copy	(	const Set &		inref	)
	{
		clear();
		insert( inref.begin(), inref.end() );
	}

	SU_SET_DECL
	void
	SU_SET_DEF::copy	(	iterator itfirst, iterator itlast	)
	{
		clear();
		insert( itfirst, itlast );
	}

	SU_SET_DECL
	void
	SU_SET_DEF::free	(				)
	{
		clear();
	}

	#undef	SU_SET_DECL
	#undef	SU_SET_DEF
	#undef	SU_IT_DECL
	#undef	SU_IT_DEF

}




#endif // _su_seti_h_




