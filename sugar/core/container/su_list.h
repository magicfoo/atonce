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



#ifndef _su_list_h_
#define _su_list_h_



namespace su
{

	//
	// simple list iterator

	template < typename T >

	struct liter
	{
		T*&		h;
		T*		p;
		T*		c;
		T*		n;

				liter		( T*&	);

		T*		prev		(		) const;
		T*		cur			(		) const;
		T*		next		(		) const;

		void	step		(		);
		void	rewind		(		);
		uint	count		(		) const;		// # from the current
		uint	size		(		) const;		// # from the head

		void	del			(		);
		void	insert		( T*	);

		T*		operator *	(		) const;
		T*		operator ->	(		) const;
		void	operator ++ (		);
		void	operator ++ ( int	);
	};


	template < typename T > inline void		list_init				( T*& first );
	template < typename T > inline bool		list_has				( T* first, T* e );
	template < typename T > inline void		list_remove				( T*& first, T* e );
	template < typename T > inline void		list_insert_front		( T*& first, T* e );
	template < typename T > inline void		list_insert_inplace		( T*& first, T* e, T* here );
	template < typename T > inline void		list_recycle_all		( T*& first, Pool* inp );
	template < typename T > inline void		list_release_all		( T*& first, Pool* inp );

}



#include "su_listi.h"
#endif // _su_list_h_


