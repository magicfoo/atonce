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



#ifndef _su_multiset_h_
#define _su_multiset_h_



namespace su
{

	//
	// multiset
	//
	// based on red-black tree
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
	//


	template <	typename Key,							// item type
				typename Compare	>					// item comparison object function

	struct MultiSet
	{

		struct iterator
		{
			typedef MultiSet<Key,Compare>			container_type;
			typedef tree<Key,Compare>				tree_type;
			typedef typename tree_type::node		node;
			typedef	uint							size_type;
			typedef Key								key_type;
			typedef	Key								value_type;
			typedef Key*							pointer;
			typedef Key&							reference;

						iterator		(					);
						iterator		(	const iterator&	);
			explicit	iterator		(	node*			);
			iterator	operator +		(	int				) const;
			iterator	operator -		(	int				) const;
			iterator&	operator +=		(	int				);
			iterator&	operator -=		(	int				);
			iterator&	operator ++		(					);
			iterator&	operator --		(					);
			iterator 	operator ++		(	int				);
			iterator 	operator --		(	int				);
			iterator&	operator =		(	iterator 		);
			bool		operator ==		(	iterator 		) const;
			bool		operator !=		(	iterator 		) const;
			pointer		operator ->		(					) const;
			reference	operator *		(					) const;
			pointer		operator &		(					) const;
		protected:
			friend struct MultiSet<Key,Compare>;
			node*		ptr;
		};



		typedef	uint						size_type;
		typedef Compare						compare_type;
		typedef	Key							value_type;
		typedef Key							key_type;
		typedef Key*						pointer;
		typedef Key&						reference;
		typedef tree<Key,Compare>			tree_type;
		typedef typename tree_type::node	node;
//typename node;




						MultiSet		(	Pool*									);
						MultiSet		(	Pool*, const MultiSet &					);
						MultiSet		(	Pool*, iterator, iterator				);			// [first,last)
		virtual		   ~MultiSet		(											);
		iterator		begin			(											) const;
		iterator		last			(											) const;
		iterator		end				(											) const;
		size_type		size			(											) const;
		bool			empty			(											) const;
		reference		front			(											) const;
		reference		back			(											) const;
		iterator		find			(	const value_type &						);
		iterator		insert			(	const value_type &, size_type count=1	);
		void			insert			(	iterator, iterator						);			// [first,last)
		iterator		erase			(	iterator								);
		iterator		erase			(	iterator, iterator						);
		void			clear			(											);
		void			copy			(	const MultiSet &						);
		void			copy			(	iterator, iterator						);
		void			free			(											);			// reserve(0)
	protected:
		tree_type		tr;
		Pool*			mp;

		SU_NO_OPERATORS( MultiSet )
	};

}



#include "su_multiseti.h"
#endif // _su_multiset_h_


