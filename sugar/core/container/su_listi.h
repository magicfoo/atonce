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



#ifndef _su_listi_h_
#define _su_listi_h_




namespace su
{

	template < typename T > inline
	liter<T>::liter( T*& inhead )
		: h(inhead)
	{
		rewind();
	}

	template < typename T > inline
	T*
	liter<T>::prev ( ) const
	{
		return p;
	}

	template < typename T > inline
	T*
	liter<T>::cur ( ) const
	{
		return c;
	}

	template < typename T > inline
	T*
	liter<T>::next ( ) const
	{
		return n;
	}

	template < typename T > inline
	void
	liter<T>::step ( )
	{
		if( c )
		{
			p = c;
			c = n;
			n = n ? n->next : NULL;
		}
	}

	template < typename T > inline
	void
	liter<T>::rewind ( )
	{
		p = NULL;
		c = h;
		n = c ? c->next : NULL;
	}

	template < typename T > inline
	uint
	liter<T>::count ( ) const
	{
		uint n = 0;
		T* t = c;
		while( t )
		{
			n++;
			t = t->next;
		}
		return n;
	}

	template < typename T > inline
	uint
	liter<T>::size ( ) const
	{
		uint n = 0;
		T* t = h;
		while( t )
		{
			n++;
			t = t->next;
		}
		return n;
	}

	template < typename T > inline
	void
	liter<T>::del ( )
	{
		if( c )
		{
			if( p )		p->next = n;
			else		h = n;
			c = n;
			n = n ? n->next : NULL;
		}
	}

	template < typename T > inline
	void
	liter<T>::insert ( T* t )
	{
		if( p )		p->next = t;
		else		h = t;
		t->next = c;
		n = c;
		c = t;
	}

	template < typename T > inline
	T*
	liter<T>::operator * ( ) const
	{
		return c;
	}

	template < typename T > inline
	T*
	liter<T>::operator -> ( ) const
	{
		return c;
	}

	template < typename T > inline
	void
	liter<T>::operator ++ ( )
	{
		step();
	}

	template < typename T > inline
	void
	liter<T>::operator ++ ( int )
	{
		step();
	}






	template < typename T > inline
	void list_init ( T*& first )
	{
		first = NULL;
	}


	template < typename T > inline
	bool list_has ( T* first, T* e )
	{
		SU_ASSERT( e );
		T* i = first;
		for( ; i ; i=i->next )
			if( i==e )	return true;
		return false;
	}


	template < typename T > inline
	void list_remove ( T*& first, T* e )
	{
		SU_ASSERT( e );
		SU_ASSERT( list_has(first,e) );
		if( first==e )	first = e->next;
		if( e->prev )	e->prev->next = e->next;
		if( e->next )	e->next->prev = e->prev;
		e->next = e->prev = NULL;
	}


	template < typename T > inline
	void list_insert_front ( T*& first, T* e )
	{
		SU_ASSERT( e );
		SU_ASSERT( !list_has(first,e) );
		e->prev = NULL;
		e->next = first;
		if( first )	first->prev = e;
		first = e;
	}


	template < typename T > inline
	void list_insert_inplace ( T*& first, T* e, T* here )
	{
		SU_ASSERT( e );
		SU_ASSERT( here );
		SU_ASSERT( !list_has(first,e) );
		SU_ASSERT( list_has(first,here) );
		if( first==here )	first = e;
		if( here->prev )	here->prev->next = e;
		e->prev = here->prev;
		e->next = here;
		here->prev = e;
	}


	template < typename T > inline
	void list_insert_next ( T* e, T* here )
	{
		SU_ASSERT( e );
		SU_ASSERT( here );
		e->prev = here;
		e->next = here->next;
		here->next = e;
		if( e->next )	e->next->prev = e;
	}


	template < typename T > inline
	void list_recycle_all ( T*& first, Pool* inp )
	{
		SU_ASSERT( inp );
		T *tt, *t = first;
		while( t ) {
			tt = t;
			t = t->next;
			tt->recycle( inp );
		}
		first = NULL;
	}


	template < typename T > inline
	void list_release_all ( T*& first, Pool* inp )
	{
		SU_ASSERT( inp );
		T *tt, *t = first;
		while( t ) {
			tt = t;
			t = t->next;
			tt->release( inp );
		}
		first = NULL;
	}


}



#endif // _su_listi_h_


