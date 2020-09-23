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



#ifndef _su_tree_h_
#define _su_tree_h_

#include "su_treei.h"



namespace su
{

	//
	// Red/Black tree
	// Double linked nodes for fast traversal.
	//
	// Only node linking is managed !!
	// The user has the responsability of nodes allocation/destruction !!
	//
	// Compare object are similar to STL's Strict Weak Order object.
	//
	//		struct MyCmp
	//		{
	//			bool operator () ( const Key& k0, const Key& k1 ) const
	//			{
	//				// ...
	//			}
	//		}


	template <	typename Key,				// item type
				typename Compare	>		// item comparison object function

	struct tree : public _tree
	{
		struct node : public _node
		{
			typedef Key		value_type;
			value_type		key;
		};


		typedef node		node_type;
		typedef	Compare		compare_type;
		typedef	uint		size_type;
		typedef Key			value_type;
		typedef Key			key_type;
		typedef Key*		pointer;
		typedef Key&		reference;


						tree			(									);
		virtual		   ~tree			(									);
		node*			insert			(	node*							);
		node*			insertUnique	(	node*							);
		void			erase			(	node*							);
		node*			find			(	const value_type&				) const;
		void			clear			(									);
		node*			first			(									) const;
		node*			last			(									) const;
		uint			size			(									) const;
		node*			nextUnique		(	node*							) const;
		node*			prevUnique		(	node*							) const;
	protected:
		virtual int		_compareNode	(	const _node*, const _node*		) const;

		SU_NO_OPERATORS( tree )
	};

}



#include "su_treeii.h"
#endif // _su_tree_h_


