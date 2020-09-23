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



#ifndef _su_string_h_
#define _su_string_h_



namespace su
{

	struct String
	{
		typedef vector<char,0>				vector_type;
		typedef vector_type::value_type		char_type;
		typedef vector_type::value_type		value_type;
		typedef	vector_type::size_type		size_type;
		typedef const char*					cpointer;
		typedef char*						pointer;
		typedef char&						reference;


		struct iterator
		{
			typedef String	container_type;

							iterator		(						);
							iterator		(	const iterator&		);
			explicit		iterator		(	pointer				);
			explicit		iterator		(	cpointer			);
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
			char_type		operator *		(						) const;
			pointer			operator &		(						) const;
							operator const char * (					) const;

		protected:
			pointer			ptr;
		};


		template < typename S >

		struct creator
		{
			typedef String	container_type;

							creator			(	Pool*, const S& invalue									);

			creator&		relocate		(	Pool* p = NULL											) const;
			Pool*			pool			(															) const;
			const S&		value			(															) const;
			cpointer		cstr			(	cpointer inDef = NULL									) const;

			creator			dup				(															) const;
			creator			left			(	size_type pos											) const;
			creator			right			(	size_type pos											) const;
			creator			mid				(	size_type pos, size_type len							) const;
			creator			front			(	size_type len											) const;
			creator			back			(	size_type len											) const;
			creator			lower			(															) const;
			creator			upper			(															) const;
			creator			subst			(	size_type pos, size_type len, const String&				) const;
			creator			subst			(	size_type pos, size_type len, pcstr						) const;
			creator			subst			(	const String& src, const String& dst, bool cs=true		) const;
			creator			subst			(	pcstr src, pcstr dst, bool cs=true						) const;
			creator			substall		(	const String& src, const String& dst, bool cs=true		) const;
			creator			substall		(	pcstr src, pcstr dst, bool cs=true						) const;
			creator			arg				(	char_type												) const;
			creator			arg				(	pcstr													) const;
			creator			arg				(	const String&											) const;
			creator			arg				(	long, int base=10										) const;
			creator			arg				(	ulong, int base=10										) const;
			creator			arg				(	float, char fmt='g', int prec=2							) const;

			SU_NO_OPERATORS( creator )
							operator const char * (														) const;

		private:
			const S			s;
		};


							String			(	Pool* = NULL															);
		explicit			String			(	Pool*, cpointer															);
		explicit			String			(	Pool*, const String&													);
		virtual			   ~String			(																			);

		void				relocate		(	Pool*																	);
		Pool*				pool			(																			) const;

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
		bool				isspace			(																			) const;
		bool				isalpha			(																			) const;
		bool				isdigit			(																			) const;
		bool				isalnum			(																			) const;
		int					compare			(	char_type, bool cs=true													) const;
		int					compare			(	pcstr, bool cs=true														) const;
		int					compare			(	pcstr, uint len, bool cs=true											) const;
		int					compare			(	const String&, bool cs=true												) const;
		int					compare			(	const String&, uint len, bool cs=true									) const;
		bool				equals			(	char_type, bool cs=true													) const;
		bool				equals			(	pcstr, bool cs=true														) const;
		bool				equals			(	pcstr, uint len, bool cs=true											) const;
		bool				equals			(	const String&, bool cs=true												) const;
		bool				equals			(	const String&, uint len, bool cs=true									) const;
		bool				startswith		(	char_type, bool cs=true													) const;
		bool				startswith		(	pcstr, bool cs=true														) const;
		bool				startswith		(	const String&, bool cs=true												) const;
		bool				startswithany	(	pcstr charset, bool cs=true												) const;
		bool				startswithany	(	const String& charset, bool cs=true										) const;
		bool				endswith		(	char_type, bool cs=true													) const;
		bool				endswith		(	pcstr, bool cs=true														) const;
		bool				endswith		(	const String&, bool cs=true												) const;
		bool				endswithany		(	pcstr charset, bool cs=true												) const;
		bool				endswithany		(	const String& charset, bool cs=true										) const;
		int					offsetof		(	pcstr																	) const;	// < 0 => not available !
		int					offsetof		(	iterator																) const;	// < 0 => not available !
		int					indexof			(	char_type, size_type pos=0, bool cs=true, bool rev=false				) const;	// < 0 => not found !
		int					indexof			(	pcstr, size_type pos=0, bool cs=true, bool rev=false					) const;	// < 0 => not found !
		int					indexof			(	const String&, size_type pos=0, bool cs=true, bool rev=false			) const;	// < 0 => not found !
		int					indexofany		(	const String& charset, size_type pos=0,bool cs=true, bool rev=false		) const;	// < 0 => not found !
		int					indexofany		(	pcstr charset, size_type pos=0,bool cs=true, bool rev=false				) const;	// < 0 => not found !
		size_type			tonumber		(	long&, int base, size_type pos											) const;
		size_type			tonumber		(	ulong&, int base, size_type pos											) const;
		size_type			tonumber		(	float&, size_type pos													) const;
		void				split			(	size_type pos, String& outl, String& outr								) const;
		void				splitsep		(	const String& seplist, String& outl, String& outr						) const;
		void				splitsep		(	pcstr seplist, String& outl, String& outr								) const;
		uint32				hash32			(	bool innormalized = true												) const;
		uint128				hash128			(																			) const;

		// modifiers that do not require a pool

		String&				clear			(																			);
		String&				tolower			(																			);
		String&				toupper			(																			);

		// modifiers requiring a pool

		void				reserve			(	size_type																);
		void				resize			(	size_type, char_type													);
		void				squeeze			(																			);
		String&				strip			(																			);
		String&				erase			(	size_type pos, size_type len											);
		String&				insert			(	size_type pos, const String&											);
		String&				insert			(	size_type pos, cpointer													);
		String&				copy			(	cpointer																);
		String&				copy			(	const String&															);
		String&				copy			(	cpointer, size_type pos, size_type len									);
		String&				copy			(	const String&, size_type pos, size_type len								);
		String&				append			(	cpointer																);
		String&				append			(	const String&															);
		String&				append			(	char_type																);
		String&				prepend			(	cpointer																);
		String&				prepend			(	const String&															);
		String&				erasend			(	size_type len															);
		String&				replace			(	size_type pos, size_type len, const String&								);
		String&				replace			(	size_type pos, size_type len, pcstr										);
		String&				replace			(	const String& src, const String& dst, bool cs=true						);
		String&				replace			(	pcstr src, pcstr dst, bool cs=true										);
		String&				replaceall		(	const String& src, const String& dst, bool cs=true						);
		String&				replaceall		(	pcstr src, pcstr dst, bool cs=true										);

		// operators

		SU_NO_CMP_OPERATORS( String )
							operator const char *	(																	) const;
		reference			operator []				(	size_type														);
		String&				operator =				(	pcstr															);
		String&				operator =				(	const String&													);
		String&				operator =				(	const creator<String>&											);
		String&				operator +=				(	const String&													);
		String&				operator +=				(	pcstr															);
		String&				operator +=				(	char_type														);

		// get a pooled creator

		creator<String>		operator ()				(	Pool* p															) const;

		// constants

		static const String	Null;

	protected:
		vector_type			data;
		void				assert_cstr		( );
		void				assert_pool		( ) const;
	};

}



#include "su_stringi.h"
#endif // _su_string_h_



