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



#ifndef _su_bstring_h_
#define _su_bstring_h_



namespace su
{

	// bounded string, using static memory and fixed capacity

	template < uint Bound >

	struct BString
	{
		static const uint							bound = Bound;
		typedef vector<char,Bound>					vector_type;
		typedef typename vector_type::bound_type	bound_type;
		typedef typename vector_type::value_type	char_type;
		typedef typename vector_type::value_type	value_type;
		typedef	typename vector_type::size_type		size_type;
		typedef typename vector_type::iterator		vector_iterator;
		typedef const char*							cpointer;
		typedef char_type*							pointer;
		typedef char_type&							reference;


		struct iterator
		{
			typedef BString<Bound>	container_type;

							iterator		(						);
							iterator		(	const iterator&		);
			explicit		iterator		(	pointer				);
			explicit		iterator		(	pointer, int , int	);
			iterator		operator +		(	int					) const;
			iterator		operator -		(	int					) const;
			iterator&		operator +=		(	int					);
			iterator&		operator -=		(	int					);
			iterator&		operator ++		(						);
			iterator&		operator --		(						);
			iterator		operator ++		(	int					);
			iterator 		operator --		(	int					);
			size_type 		operator -		(	iterator			) const;
			iterator&		operator =		(	iterator			);
			bool			operator ==		(	iterator			) const;
			bool			operator !=		(	iterator			) const;
			bool			operator <		(	iterator			) const;
			bool			operator >		(	iterator			) const;
			bool			operator <=		(	iterator			) const;
			bool			operator >=		(	iterator			) const;
			reference		operator *		(						) const;
			pointer			operator &		(						) const;
							operator const char * (					) const;

		protected:
			pointer			ptr;
		};


							BString			(																			);
							BString			(	const BString&															);
							BString			(	cpointer																);
		virtual			   ~BString			(																			);

		bool				isnull			(																			) const;
		bool				empty			(																			) const;
		size_type			size			(																			) const;
		size_type			len				(																			) const;
		reference			at				(	size_type																) const;
		cpointer			cstr			(	cpointer inDef = NULL													) const;	// returns NULL if empty !
		pointer				str				(	pointer inDef = NULL													) const;	// returns NULL if empty !
		iterator			begin			(																			) const;
		iterator			end				(																			) const;
		size_type			capacity		(																			) const;
		bool				matchstring		(	pcstr instring, bool cs=true											) const;
		bool				matchpattern	(	pcstr inpattern, bool cs=true											) const;
		bool				isspace			(																			) const;
		bool				isalpha			(																			) const;
		bool				isdigit			(																			) const;
		bool				isalnum			(																			) const;
		int					compare			(	char_type, bool cs=true													) const;
		int					compare			(	pcstr, bool cs=true														) const;
		int					compare			(	pcstr, uint len, bool cs=true											) const;
		int					compare			(	const BString&, bool cs=true											) const;
		int					compare			(	const BString&, uint len, bool cs=true									) const;
		bool				equals			(	char_type, bool cs=true													) const;
		bool				equals			(	pcstr, bool cs=true														) const;
		bool				equals			(	pcstr, uint len, bool cs=true											) const;
		bool				equals			(	const BString&, bool cs=true											) const;
		bool				equals			(	const BString&, uint len, bool cs=true									) const;
		bool				startswith		(	char_type, bool cs=true													) const;
		bool				startswith		(	pcstr, bool cs=true														) const;
		bool				startswith		(	const BString&, bool cs=true											) const;
		bool				startswithany	(	pcstr charset, bool cs=true												) const;
		bool				startswithany	(	const BString& charset, bool cs=true									) const;
		bool				endswith		(	char_type, bool cs=true													) const;
		bool				endswith		(	pcstr, bool cs=true														) const;
		bool				endswith		(	const BString&, bool cs=true											) const;
		bool				endswithany		(	pcstr charset, bool cs=true												) const;
		bool				endswithany		(	const BString& charset, bool cs=true									) const;
		int					offsetof		(	pcstr																	) const;	// < 0 => not available !
		int					offsetof		(	iterator																) const;	// < 0 => not available !
		int					indexof			(	char_type, size_type pos=0, bool cs=true, bool rev=false				) const;	// < 0 => not found !
		int					indexof			(	pcstr, size_type pos=0, bool cs=true, bool rev=false					) const;	// < 0 => not found !
		int					indexof			(	const BString&, size_type pos=0, bool cs=true, bool rev=false			) const;	// < 0 => not found !
		int					indexofany		(	const BString& charset, size_type pos=0,bool cs=true, bool rev=false	) const;	// < 0 => not found !
		int					indexofany		(	pcstr charset, size_type pos=0,bool cs=true, bool rev=false				) const;	// < 0 => not found !
		size_type			tonumber		(	long&, int base, size_type pos											) const;
		size_type			tonumber		(	ulong&, int base, size_type pos											) const;
		size_type			tonumber		(	float&, size_type pos													) const;
		void				split			(	size_type pos, BString& outl, BString& outr								) const;
		void				splitsep		(	const BString& seplist, BString& outl, BString& outr					) const;
		void				splitsep		(	pcstr seplist, BString& outl, BString& outr								) const;
		uint32				hash32			(	bool innormalized = true												) const;
		uint128				hash128			(																			) const;

		// modifiers

		void				reserve			(	size_type																);
		void				resize			(	size_type, char_type													);
		void				squeeze			(																			);
		BString&			strip			(																			);
		BString&			clear			(																			);
		BString&			erase			(	size_type pos, size_type len											);
		BString&			insert			(	size_type pos, const BString&											);
		BString&			insert			(	size_type pos, cpointer													);
		BString&			copy			(	cpointer																);
		BString&			copy			(	const BString&															);
		BString&			copy			(	cpointer, size_type pos, size_type len									);
		BString&			copy			(	const BString&, size_type pos, size_type len							);
		BString&			tolower			(																			);
		BString&			toupper			(																			);
		BString&			append			(	cpointer																);
		BString&			append			(	const BString&															);
		BString&			append			(	char_type																);
		BString&			prepend			(	cpointer																);
		BString&			prepend			(	const BString&															);
		BString&			erasend			(	size_type len															);
		BString&			replace			(	size_type pos, size_type len, const BString&							);
		BString&			replace			(	size_type pos, size_type len, pcstr										);
		BString&			replace			(	const BString& src, const BString& dst, bool cs=true					);
		BString&			replace			(	pcstr src, pcstr dst, bool cs=true										);
		BString&			replaceall		(	const BString& src, const BString& dst, bool cs=true					);
		BString&			replaceall		(	pcstr src, pcstr dst, bool cs=true										);

		// creators

		BString				dup				(																			) const;
		BString				left			(	size_type pos															) const;
		BString				right			(	size_type pos															) const;
		BString				mid				(	size_type pos, size_type len											) const;
		BString				front			(	size_type len															) const;
		BString				back			(	size_type len															) const;
		BString				lower			(																			) const;
		BString				upper			(																			) const;
		BString				subst			(	size_type pos, size_type len, const BString&							) const;
		BString				subst			(	size_type pos, size_type len, pcstr										) const;
		BString				subst			(	const BString& src, const BString& dst, bool cs=true					) const;
		BString				subst			(	pcstr src, pcstr dst, bool cs=true										) const;
		BString				substall		(	const BString& src, const BString& dst, bool cs=true					) const;
		BString				substall		(	pcstr src, pcstr dst, bool cs=true										) const;
		BString				arg				(	char_type, int rep=1													) const;
		BString				arg				(	pcstr																	) const;
		BString				arg				(	const BString&															) const;
		BString				arg				(	long, int base=10														) const;
		BString				arg				(	ulong, int base=10														) const;
		BString				arg				(	float, char fmt='g', int prec=2											) const;
		BString				argl			(	long, int base=10														) const;
		BString				argul			(	ulong, int base=10														) const;
		BString				argf			(	float, char fmt='g', int prec=2											) const;

		// operators

		SU_NO_CMP_OPERATORS( BString )
							operator const char *	(																	) const;
		reference			operator []				(	size_type														);
		BString&			operator =				(	const BString&													);
		BString&			operator =				(	pcstr															);
		BString&			operator +=				(	const BString&													);
		BString&			operator +=				(	pcstr															);
		BString&			operator +=				(	char_type														);
		BString				operator +				(	const BString&													) const;
		BString				operator +				(	char_type														) const;

	protected:
		vector_type			data;
		void				assert_cstr		( );
	};

	typedef BString<32>		SString;		// short string
	typedef BString<128>	LString;		// long string
	typedef BString<256>	PString;		// path string
	typedef BString<1024>	TString;		// text string of 1Ko

}



#include "su_bstringi.h"
#endif // _su_bstring_h_



