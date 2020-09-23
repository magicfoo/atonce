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



#include <sufw/core/su_core.h>
using namespace su;


void
su::_tree::_init	(	 )
{
	_initNode( &_nil );
	_nil._color = NC_BLACK;
	_clear();
}


void
su::_tree::_shut	(	)
{
	//
}


void
su::_tree::_clear()
{
	_first = NULL;
	_last  = NULL;
	_root  = &_nil;
	_size  = 0;
}


su::_tree::_node*
su::_tree::_insert(	_node*	node	)
{
	if( !node )	return NULL;
	_initNode( node );
	node = _doInsert( node, TRUE );
	if( node )	_size++;
	return node;
}


su::_tree::_node*
su::_tree::_insertUnique(	_node*	node	)
{
	if( !node )	return NULL;
	_initNode( node );
	node = _doInsert( node, FALSE );
	if( node )	_size++;
	return node;
}


void
su::_tree::_erase(	_node*	node	)
{
	if( !node )	return;
	_doErase ( node );
	SU_ASSERT( _size );
	_size--;
}


su::_tree::_node*
su::_tree::_find( _node* ref ) const
{
	_node* y = _root;
	int c;
	while( y != &_nil ) {
		c = _compareNode( ref, y );
		if( c == 0 ) return y;
		if( c < 0 )	y = y->_left;
		else        y = y->_right;
	}
	return NULL;
}


su::_tree::_node*
su::_tree::_nextUnique( _node* node ) const
{
	if( !node )	return NULL;
	if( node->_color == NC_DUP )	node = _find( node );
	if( node )						node = _treeNext( node );
	return node;
}

	
su::_tree::_node*
su::_tree::_prevUnique( _node* node ) const
{
	if( !node )	return NULL;
	if( node->_color == NC_DUP )	node = _find( node );
	if( node )						node = _treePrev( node );
	return node;
}


void
su::_tree::_rotateRight( _node* x )
{
	SU_ASSERT( x );
	_node* y = x->_left;
	x->_left = y->_right;
	if( y->_right != &_nil )	y->_right->_parent = x;
	if( y != &_nil )			y->_parent = x->_parent;
	if( x->_parent ) {
		if( x == x->_parent->_right )	x->_parent->_right = y;
		else                	        x->_parent->_left = y;
	} else {
		_root = y;
	}
	y->_right = x;
	if( x != &_nil) x->_parent = y;
}

	
void
su::_tree::_rotateLeft( _node* x )
{
	SU_ASSERT( x );
	_node* y = x->_right;
	x->_right = y->_left;
	if( y->_left != &_nil )	y->_left->_parent = x;
	if( y != &_nil )		y->_parent = x->_parent;
	if( x->_parent )	{
		if( x == x->_parent->_left )  x->_parent->_left = y;
		else                       	  x->_parent->_right = y;
	} else {
		_root = y;
	}
	y->_left = x;
	if( x != &_nil ) x->_parent = y;
}


void
su::_tree::_fixInsert( _node* x )
{
	SU_ASSERT( x );
	while( x != _root && x->_parent->_color == NC_RED )
	{
		if( x->_parent == x->_parent->_parent->_left )
		{
			_node* y = x->_parent->_parent->_right;
			if( y->_color == NC_RED ) {
				x->_parent->_color = NC_BLACK;
				y->_color = NC_BLACK;
				x->_parent->_parent->_color = NC_RED;
				x = x->_parent->_parent;
			} else {
				if( x == x->_parent->_right ) {
					x = x->_parent;
					_rotateLeft( x );
				}
				x->_parent->_color = NC_BLACK;
				x->_parent->_parent->_color = NC_RED;
				_rotateRight( x->_parent->_parent );
			}
		}
		else
		{
			_node* y = x->_parent->_parent->_left;
			if( y->_color == NC_RED )
			{
				x->_parent->_color = NC_BLACK;
				y->_color = NC_BLACK;
				x->_parent->_parent->_color = NC_RED;
				x = x->_parent->_parent;
			} else {
				if( x == x->_parent->_left ) {
					x = x->_parent;
					_rotateRight( x );
				}
				x->_parent->_color = NC_BLACK;
				x->_parent->_parent->_color = NC_RED;
				_rotateLeft( x->_parent->_parent );
			}
		}
	}
	_root->_color = NC_BLACK;
}


su::_tree::_node*
su::_tree::_doInsert( _node* x, bool allowMulti )
{
	SU_ASSERT( x );
	_node *y=_root, *p=NULL;
	int c = 0;

	while( y != &_nil ) {
		c = _compareNode( x, y );
		if( c == 0 ) {
			if( !allowMulti )	return NULL;
			x->_color = NC_DUP;
			y = _treeNext( y );
			if( y ) _dllInsertBefore( y, x );
			else	_dllAddAfter( _last, x );
			return x;
		}
		p = y;
		y = (c < 0) ? y->_left : y->_right;
	}

	if( p ) {
		x->_parent = p;
		if( c < 0 ) {
			_dllInsertBefore( p, x );
			p->_left = x;
		} else {
			y = _treeNext(p);
			if( y ) _dllInsertBefore( y, x );
			else    _dllAddAfter( _last, x );
			p->_right = x;
		}
	} else {
		_root = x;
		x->_color = NC_BLACK;
		_first = x;
		_last  = x;
	}

	_fixInsert( x );
	return x;
}


void
su::_tree::_fixErase( _node* x )
{
	SU_ASSERT( x );
	while( x != _root && x->_color == NC_BLACK) {
		if( x == x->_parent->_left )
		{
			_node* w = x->_parent->_right;
			if( w->_color == NC_RED ) {
				w->_color = NC_BLACK;
				x->_parent->_color = NC_RED;
				_rotateLeft( x->_parent );
				w = x->_parent->_right;
			}
			if( w->_left->_color == NC_BLACK && w->_right->_color == NC_BLACK ) {
				w->_color = NC_RED;
				x = x->_parent;
			}
			else {
				if( w->_right->_color == NC_BLACK ) {
					w->_left->_color = NC_BLACK;
					w->_color = NC_RED;
					_rotateRight( w );
					w = x->_parent->_right;
				}
				w->_color = x->_parent->_color;
				x->_parent->_color = NC_BLACK;
				w->_right->_color = NC_BLACK;
				_rotateLeft( x->_parent );
				x = _root;
			}
		}
		else
		{
			_node* w = x->_parent->_left;
			if( w->_color == NC_RED ) {
				w->_color = NC_BLACK;
				x->_parent->_color = NC_RED;
				_rotateRight( x->_parent );
				w = x->_parent->_left;
			}
			if( w->_right->_color == NC_BLACK && w->_left->_color == NC_BLACK )	{
				w->_color = NC_RED;
				x = x->_parent;
			}
			else {
				if( w->_left->_color == NC_BLACK ) {
					w->_right->_color = NC_BLACK;
					w->_color = NC_RED;
					_rotateLeft( w );
					w = x->_parent->_left;
				}
				w->_color = x->_parent->_color;
				x->_parent->_color = NC_BLACK;
				w->_left->_color = NC_BLACK;
				_rotateRight( x->_parent );
				x = _root;
			}
		}
	}
	x->_color = NC_BLACK;
}


void
su::_tree::_doErase( _node* x )
{
	SU_ASSERT( x );
	if( x->_color == NC_DUP ) {
		_dllErase( x );
		return;
	}

	if( x->_next && x->_next->_color == NC_DUP ) {
		_replaceTreeNode( x, x->_next );
		_dllErase( x );
		return;
	}

	_dllErase( x );

	_node *y, *z;
	bool balance;

	if( x->_left == &_nil || x->_right == &_nil )	y = x;
	else										y = _findMin( x->_right );

	if( y->_left != &_nil )	z = y->_left;
	else					z = y->_right;

	z->_parent = y->_parent;
	if( y->_parent ) {
		if( y == y->_parent->_left )  y->_parent->_left  = z;
		else                        y->_parent->_right = z;
	} else {
		_root = z;
	}

	balance = ( y->_color == NC_BLACK );
	if( y != x ) _replaceTreeNode( x, y );
	if( balance ) _fixErase( z );
}


void
su::_tree::_dllErase( _node* x )
{
	SU_ASSERT( x );
	if( x->_prev )   x->_prev->_next = x->_next;
	else            _first = x->_next;
	if( x->_next )   x->_next->_prev = x->_prev;
	else            _last = x->_prev;
}

	
void
su::_tree::_dllInsertBefore( _node* x, _node* y )
{
	SU_ASSERT( x );
	SU_ASSERT( y );
	y->_prev = x->_prev;
	if( y->_prev )   y->_prev->_next = y;
	else            _first = y;
	y->_next = x;
	x->_prev = y;
}


void
su::_tree::_dllAddAfter( _node* x, _node* y )
{
	SU_ASSERT( x );
	SU_ASSERT( y );
	y->_next = x->_next;
	if( y->_next )   y->_next->_prev = y;
	else            _last = y;
	y->_prev = x;
	x->_next = y;
}


void
su::_tree::_initNode( _node* x )
{
	SU_ASSERT( x );
	x->_color  = NC_RED;
	x->_parent = NULL;
	x->_left   = &_nil;
	x->_right  = &_nil;
	x->_next   = NULL;
	x->_prev   = NULL;
}


su::_tree::_node*
su::_tree::_treeNext( _node* x ) const
{
	SU_ASSERT( x );
	if( x->_right != &_nil )		return _findMin( x->_right );
	while( x->_parent ) {
		if( x == x->_parent->_left )	break;
		x = x->_parent;
	}
	return x->_parent;
}


su::_tree::_node*
su::_tree::_treePrev( _node* x )const
{
	SU_ASSERT( x );
	if( x->_left != &_nil )		return _findMax( x->_left );
	while( x->_parent ) {
		if( x == x->_parent->_right )	break;
		x = x->_parent;
	}
	return x->_parent;
}


su::_tree::_node*
su::_tree::_findMin( _node* x ) const
{
	SU_ASSERT( x );
	while( x->_left != &_nil )	x = x->_left;
	return x;
}


su::_tree::_node*
su::_tree::_findMax( _node* x ) const
{
	SU_ASSERT( x );
	while( x->_right != &_nil )	x = x->_right;
	return x;
}


void
su::_tree::_replaceTreeNode( _node* x, _node* y )
{
	SU_ASSERT( x );
	SU_ASSERT( y );
	y->_color  = x->_color;
	y->_left   = x->_left;
	y->_right  = x->_right;
	y->_parent = x->_parent;
	if( y->_parent ) {
		if (y->_parent->_left == x )  y->_parent->_left  = y;
		else                        y->_parent->_right = y;
	} else {
		_root = y;
	}
	y->_left->_parent  = y;
	y->_right->_parent = y;
}


