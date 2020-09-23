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




#ifndef _su_treei_h_
#define _su_treei_h_



namespace su
{

	struct_ie _tree
	{
		enum
		{
			NC_RED,
			NC_BLACK,
			NC_DUP
		};

		struct _node
		{
			char			_color;
			_node*			_parent;
			_node*			_left;
			_node*			_right;
			_node*			_next;
			_node*			_prev;
		};


		typedef _node		_node_type;


		void			_init				(									);
		void			_shut				(									);
		_node*			_insert				(	_node*							);
		_node*			_insertUnique		(	_node*							);
		void			_erase				(	_node*							);
		_node*			_find				(	_node*							)const;
		void			_clear				(									);
		_node*			_nextUnique			(	_node*							)const;
		_node*			_prevUnique			(	_node*							)const;
	protected:
		_node			_nil;
		_node*			_root;
		uint			_size;
		_node*			_first;
		_node*			_last;
		void			_rotateRight		(	_node*							);
		void			_rotateLeft			(	_node*							);
		void			_fixInsert			(	_node*							);
		void			_fixErase			(	_node*							);
		_node*			_doInsert			(	_node*	,	bool				);
		void			_doErase			(	_node*							);
		void			_dllErase			(	_node*							);
		void			_dllInsertBefore	(	_node*	,	_node*				);
		void			_dllAddAfter		(	_node*	,	_node*				);
		_node*			_treeNext			(	_node*							)const;
		_node*			_treePrev			(	_node*							)const;
		_node*			_findMin			(	_node*							)const;
		_node*			_findMax			(	_node*							)const;
		void			_initNode			(	_node*							);
		void			_replaceTreeNode	(	_node*	,	_node*				);
		virtual int		_compareNode		(	const _node*, const _node*		) const = 0;	// <0 (lt), >0 (gt), ==0 (eq)

		SU_NO_OPERATORS( _tree )
	};

}



#endif // _su_treei_h_




