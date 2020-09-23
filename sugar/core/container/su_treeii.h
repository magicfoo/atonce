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




#ifndef _su_treeii_h_
#define _su_treeii_h_



namespace su
{
	#define	SU_TREE_DECL	template < typename Key, typename Compare > inline
	#define	SU_TREE_DEF		tree<Key,Compare>
	#define	SU_TREE_NODE	tree<Key,Compare>::node

	SU_TREE_DECL
	SU_TREE_DEF::tree	(		)
	{
		_init();
	}

	SU_TREE_DECL
	SU_TREE_DEF::~tree	(		)
	{
		_shut();
	}

	SU_TREE_DECL
	void
	SU_TREE_DEF::clear()
	{
		_clear();
	}

	SU_TREE_DECL
	typename SU_TREE_NODE*
	SU_TREE_DEF::insert(	node*	n	)
	{
		return (node*)_insert( n );
	}

	SU_TREE_DECL
	typename SU_TREE_NODE*
	SU_TREE_DEF::insertUnique(	node*	n	)
	{
		return (node*)_insertUnique( n );
	}

	SU_TREE_DECL
	void
	SU_TREE_DEF::erase(	node*	n	)
	{
		_erase( n );
	}

	SU_TREE_DECL
	typename SU_TREE_NODE*
	SU_TREE_DEF::find( const value_type & key ) const
	{
		node n;	n.key = key;
		return (node*)_find( &n );
	}

	SU_TREE_DECL
	typename SU_TREE_NODE*
	SU_TREE_DEF::first() const
	{
		return (node*)_first;
	}
	
	SU_TREE_DECL
	typename SU_TREE_NODE*
	SU_TREE_DEF::last() const
	{
		return (node*)_last;
	}
	
	SU_TREE_DECL
	uint
	SU_TREE_DEF::size() const
	{
		return _size;
	}

	SU_TREE_DECL
	typename SU_TREE_NODE*
	SU_TREE_DEF::nextUnique( node* n ) const
	{
		return (node*)_nextUnique( n );
	}

	SU_TREE_DECL
	typename SU_TREE_NODE*
	SU_TREE_DEF::prevUnique( node* n ) const
	{
		return (node*)_prevUnique( n );
	}

	SU_TREE_DECL
	int
	SU_TREE_DEF::_compareNode( const _node* _n0, const _node* _n1 ) const
	{
		Compare compareKey;
		const Key& k0 = ((const node*)_n0)->key;
		const Key& k1 = ((const node*)_n1)->key;
		if( compareKey(k0,k1) )	return -1;
		if( compareKey(k1,k0) )	return +1;
		return 0;
	}

	#undef	SU_TREE_DEF
	#undef	SU_TREE_DECL
	#undef	SU_TREE_NODE
}




#endif // _su_treeii_h_




