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



#ifndef _su_vector_h_
#define _su_vector_h_



namespace su
{

	//
	// vector object
	// bound parameter means :
	// =0: unlimited capacity
	// >0: bounded capacity up to N items

	template < typename T, uint Bound = 0 >

	struct vector
	{

		static const uint				bound = Bound;
		typedef uint					bound_type;
		typedef	uint					size_type;
		typedef T						value_type;
		typedef T*						pointer;
		typedef const T*				cpointer;
		typedef T&						reference;


		struct iterator
		{
			typedef vector<T,Bound>		container_type;

						iterator		(						);
						iterator		(	const iterator&		);
			explicit	iterator		(	cpointer			);
			explicit	iterator		(	pointer				);
			iterator	operator +		(	int					) const;
			iterator	operator -		(	int					) const;
			iterator&	operator +=		(	int					);
			iterator&	operator -=		(	int					);
			iterator&	operator ++		(						);
			iterator 	operator ++		(	int					);
			iterator&	operator --		(						);
			iterator 	operator --		(	int					);
			size_type 	operator -		(	iterator 			) const;
			iterator&	operator =		(	iterator 			);
			bool		operator ==		(	iterator 			) const;
			bool		operator !=		(	iterator 			) const;
			bool		operator <		(	iterator 			) const;
			bool		operator >		(	iterator 			) const;
			bool		operator <=		(	iterator			) const;
			bool		operator >=		(	iterator			) const;
			pointer		operator ->		(						) const;
			reference	operator *		(						);
			pointer		operator &		(						);

		protected:
			pointer	ptr;
		};


						vector			(	Pool* = NULL									);
						vector			(	const vector&									);
						vector			(	Pool*, const vector&							);
		virtual		   ~vector			(													);
		void			relocate		(	Pool*											);
		Pool*			pool			(													) const;
		bool			hascdtor		(													) const;
		reference		at				(	size_type										) const;
		pointer			data			(													) const;	// (size() ? &[0] : NULL)
		pointer			addr			(	size_type										) const;	// (size() ? &[i] : NULL)
		size_type		bsize			(													) const;
		iterator		begin			(													) const;
		iterator		end				(													) const;
		size_type		size			(													) const;
		size_type		capacity		(													) const;
		bool			empty			(													) const;
		reference		front			(													) const;
		reference		back			(													) const;

		void			reserve			(	size_type										);
		iterator		insert			(	iterator, const value_type& = T(), size_type=1	);
		iterator		insert			(	iterator, iterator, iterator					);			// [first,last)
		iterator		erase			(	iterator										);
		iterator		erase			(	iterator, iterator								);			// [first,last)
		void			resize			(	size_type, const value_type& = T()				);
		void			squeeze			(													);			// reserve( size() )
		void			clear			(													);
		void			copy			(	const vector &									);
		void			copy			(	iterator, iterator								);
		reference		alloc_back		(	size_type										);
		void			push_back		(	const value_type &								);
		void			pop_back		(													);

		reference		operator []		(	size_type										) const;
		vector&			operator =		(	const vector&									);

		SU_NO_CMP_OPERATORS( vector )

	protected:
		T*				vfirst;
		T*				vlast;
		T*				vend;
		Pool*			vp;
		byte			vbytes[ sizeof(T)*Bound + 1*(!Bound) ];
	};


	typedef vector<bool>			BoolA;
	typedef vector<byte>			ByteA;
	typedef vector<char>			CharA;
	typedef vector<short>			ShortA;
	typedef vector<int>				IntA;
	typedef vector<uchar>			UCharA;
	typedef vector<uint>			UIntA;
	typedef vector<uint32>			U32A;
	typedef vector<uint64>			U64A;
	typedef vector<IInterface*>		InterfaceA;
	typedef vector<classid>			ClassidA;

}


#include "su_vectori.h"
#endif // _su_vector_h_




