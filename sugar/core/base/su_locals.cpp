/*LIC-HDR*****************************************************************
**
** Copyright (C) 2001-2009 AtOnce Technologies
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
using namespace su::rexp;





namespace
{

	locals::Accessor* facc = NULL;

}


locals::Accessor::Accessor ( )
{
	key = NULL;

	// link
	next = facc;
	facc = this;
}


locals::Accessor* locals::FirstAccessor ( )
{
	return facc;
}


locals::Accessor* locals::NextAccessor ( locals::Accessor* inAcc )
{
	if( !inAcc )	return facc;
	else			return inAcc->next;
}


locals::Accessor* locals::FindAccessor ( pcstr inkey )
{
	Accessor* acc = FirstAccessor();
	while( acc )
	{
		if( util::Strequals(inkey,acc->key) )
			return acc;
		acc = NextAccessor( acc );
	}
	return NULL;
}


pcstr locals::LoadContent ( pcstr inkey, pcstr begin, pcstr end )
{
	Accessor* acc = FindAccessor( inkey );
	if( acc )	return acc->loadcontent(begin,end);
	else		return NULL;
}


pcstr locals::SaveContent ( pcstr inkey, pcstr begin, pcstr end )
{
	Accessor* acc = FindAccessor( inkey );
	if( acc )	return acc->savecontent(begin,end);
	else		return NULL;
}







namespace
{

	struct tok
	{
		pcstr		begin;
		pcstr		end;
	};

	typedef vector<tok,8>	tokA;


	struct get_tok
	{
		inline static void token( tok& tok, pcstr first, pcstr last )
		{
			SU_ASSERT( first != last );
			tok.begin = first;
			tok.end = last;
		}
	};


	typedef re_or<		re_char<'0'>,
						re_char<'1'> >						rx_01;

	typedef re_follow<	re_char<'y'>,
			re_follow<	re_char<'e'>,
						re_char<'s'> > >					rx_yes;

	typedef re_follow<	re_char<'n'>,
						re_char<'o'> >						rx_no;

	typedef re_follow<	re_char<'o'>,
			re_follow<	re_char<'u'>,
						re_char<'i'> > >					rx_oui;

	typedef re_follow<	re_char<'n'>,
			re_follow<	re_char<'o'>,
						re_char<'n'> > >					rx_non;

	typedef re_follow<	re_char<'t'>,
			re_follow<	re_char<'r'>,
			re_follow<	re_char<'u'>,
						re_char<'e'> > > >					rx_true;

	typedef re_follow<	re_char<'f'>,
			re_follow<	re_char<'a'>,
			re_follow<	re_char<'l'>,
			re_follow<	re_char<'s'>,
						re_char<'e'> > > > >				rx_false;

	typedef re_follow<	 re_char<'"'>,
			re_followng< re_star<re_range<32,255> >,
						 re_char<'"'> > >					rx_string1;

	typedef re_follow<	 re_char<'\''>,
			re_followng< re_star<re_range<32,255> >,
						 re_char<'\''> > >					rx_string2;

	typedef	re_follow	< re_char<'0'>,
			re_follow	< re_char<'x'>,
						  re_repeat<_xdi,1,8> > >			rx_0x_number;

	typedef	re_follow	< re_char<'0'>,
			re_follow	< re_char<'p'>,
						  re_repeat<_xdi,1,8> > >			rx_0p_number;

	typedef	re_follow	< re_char<'0'>,
			re_follow	< re_char<'b'>,
						  re_repeat<rx_01,1,32> > >			rx_0b_number;

	typedef	re_follow	< re_char<'u'>,
			re_follow	< re_char<'i'>,
			re_follow	< re_char<'d'>,
			re_follow	< re_char<'('>,
			re_follow	< re_id,
						  re_char<')'> > > > > >			rx_uid_number;

	typedef re_token<	re_or<	rx_true,
						re_or<	rx_yes,
						re_or<	rx_oui,
								re_char<'1'> > > > >		rx_bool_true;

	typedef re_token<	re_or<	rx_false,
						re_or<	rx_no,
						re_or<	rx_non,
								re_char<'0'> > > > >		rx_bool_false;

	typedef re_token<	re_id,
						TokenAction<get_tok> >				rx_id;

	typedef re_token<	re_integer,
						TokenAction<get_tok> >				rx_integer;

	typedef re_token<	re_or<	re_uinteger,
						re_or<	rx_0x_number,
						re_or<	rx_0b_number,
								rx_uid_number > > >,
						TokenAction<get_tok> >				rx_uinteger;

	typedef re_token<	re_or<	re_real,
								rx_0p_number >,
						TokenAction<get_tok> >				rx_real;

	typedef re_token<	re_or<	rx_string1,
								rx_string2 >,
						TokenAction<get_tok> >				rx_string;



	int read_int ( pcstr begin, pcstr end )
	{
		return util::Strtolong( begin, NULL, 10 );
	}


	uint read_uint ( pcstr begin, pcstr end )
	{
		uint l = end - begin;

		if( l>2 && util::Strstartswith(begin,"0x") )
			return util::Strtoulong( begin+2, NULL, 16 );

		if( l>2 && util::Strstartswith(begin,"0b") )
			return util::Strtoulong( begin+2, NULL, 2 );

		if( l>4 && util::Strstartswith(begin,"uid(") )
		{
			LString ls;
			ls.copy( begin+4, 0, l-5 );
			return util::hmac_crc32_w( ls.cstr() );
		}

		return util::Strtoulong( begin, NULL, 10 );
	}


	float read_real ( pcstr begin, pcstr end )
	{
		uint l = end - begin;

		if( l>2 && util::Strstartswith(begin,"0p") )
		{
			union
			{
				uint32 ui;
				float  fp;
			};
			ui = util::Strtoulong( begin+2, NULL, 16 );
			return fp;
		}

		return util::Strtofloat( begin, NULL );
	}


}





// bool

locals::BoolAccessor::BoolAccessor ( pcstr ink, bool* inv )
{
	key = ink;
	val = inv;
}

pcstr locals::BoolAccessor::loadcontent ( pcstr begin, pcstr end )
{
	MatchResult<pcstr> mr;

	// true ?
	mr = rx_bool_true::match(begin,end);
	if( mr.success ) {
		if( val )	*val = true;
		return mr.pos;
	}

	// false ?
	mr = rx_bool_false::match(begin,end);
	if( mr.success ) {
		if( val )	*val = false;
		return mr.pos;
	}

	return NULL;
}

pcstr locals::BoolAccessor::savecontent ( pcstr begin, pcstr end )
{
	return NULL;
}




// int

locals::IntAccessor::IntAccessor ( pcstr ink, int* inv )
{
	key = ink;
	val = inv;
}

pcstr locals::IntAccessor::loadcontent ( pcstr begin, pcstr end )
{
	MatchResult<pcstr> mr;
	tokA s;

	mr = rx_integer::match(begin,end,s);
	SU_REQUIRE_OR_NULL( mr.success );
	SU_REQUIRE_OR_NULL( s.size() == 1 );

	if( val )
		*val = read_int( s[0].begin, s[0].end );

	return mr.pos;
}

pcstr locals::IntAccessor::savecontent ( pcstr begin, pcstr end )
{
	return NULL;
}



// uint

locals::UIntAccessor::UIntAccessor ( pcstr ink, uint* inv )
{
	key = ink;
	val = inv;
}

pcstr locals::UIntAccessor::loadcontent ( pcstr begin, pcstr end )
{
	MatchResult<pcstr> mr;
	tokA s;

	mr = rx_uinteger::match(begin,end,s);
	SU_REQUIRE_OR_NULL( mr.success );
	SU_REQUIRE_OR_NULL( s.size() == 1 );

	if( val )
		*val = read_uint( s[0].begin, s[0].end );

	return mr.pos;
}

pcstr locals::UIntAccessor::savecontent ( pcstr begin, pcstr end )
{
	return NULL;
}



// float

locals::FloatAccessor::FloatAccessor ( pcstr ink, float* inv )
{
	key = ink;
	val = inv;
}

pcstr locals::FloatAccessor::loadcontent ( pcstr begin, pcstr end )
{
	MatchResult<pcstr> mr;
	tokA s;

	mr = rx_real::match(begin,end,s);
	SU_REQUIRE_OR_NULL( mr.success );
	SU_REQUIRE_OR_NULL( s.size() == 1 );

	if( val )
		*val = read_real( s[0].begin, s[0].end );

	return mr.pos;
}

pcstr locals::FloatAccessor::savecontent ( pcstr begin, pcstr end )
{
	return NULL;
}






// string

locals::StringAccessor::StringAccessor ( pcstr ink, char** inv )
{
	key = ink;
	val = inv;
}

pcstr locals::StringAccessor::loadcontent ( pcstr begin, pcstr end )
{
	MatchResult<pcstr> mr;
	tokA s;

	mr = rx_string::match(begin,end,s);
	SU_REQUIRE_OR_NULL( mr.success );

	if( val )
	{
		int maxlen = globals::profilehi - globals::profilelo - 1;
		int slen = s[0].end - s[0].begin - 2;	// removes '"' * 2
		if( slen > maxlen )
			return NULL;	// profile heap is too small !

		char* sa = (char*)globals::profilelo;
		globals::profilelo += (slen+1);

		*val = sa;
		util::Zero( sa, slen+1 );
		util::Strcopy( sa, s[0].begin+1, slen );
	}

	return mr.pos;
}

pcstr locals::StringAccessor::savecontent ( pcstr begin, pcstr end )
{
	return NULL;
}






// uint list

locals::UListAccessor::UListAccessor ( pcstr ink, uint** inv )
{
	key = ink;
	val = inv;
}

pcstr locals::UListAccessor::loadcontent ( pcstr begin, pcstr end )
{
	MatchResult<pcstr> mr(begin,true);
	tokA s;

	uint* l0 = (uint*) util::Round4(globals::profilelo);
	uint* l1 = l0;

	for( ;; )
	{
		if( ((byte*)l1) >= globals::profilehi )
			return NULL;

		s.clear();
		mr = rx_uinteger::match(mr.pos,end,s);
		if( !mr.success )
			break;

		SU_REQUIRE_OR_NULL( s.size() > 0 );
		uint ui = read_uint( s[0].begin, s[0].end );
		*l1++ = ui;
	}

	if( l1==l0 )
		return NULL;

	if( val )
	{
		*val = l0;
		globals::profilelo = (byte*)l1;
	}

	return mr.pos;
}

pcstr locals::UListAccessor::savecontent ( pcstr begin, pcstr end )
{
	return NULL;
}






// int list

locals::IListAccessor::IListAccessor ( pcstr ink, int** inv )
{
	key = ink;
	val = inv;
}

pcstr locals::IListAccessor::loadcontent ( pcstr begin, pcstr end )
{
	MatchResult<pcstr> mr(begin,true);
	tokA s;

	int* l0 = (int*) util::Round4(globals::profilelo);
	int* l1 = l0;

	for( ;; )
	{
		if( ((byte*)l1) >= globals::profilehi )
			return NULL;

		s.clear();
		mr = rx_integer::match(mr.pos,end,s);
		if( !mr.success )
			break;

		SU_REQUIRE_OR_NULL( s.size() > 0 );
		int i = read_int( s[0].begin, s[0].end );
		*l1++ = i;
	}

	if( l1==l0 )
		return NULL;

	if( val )
	{
		*val = l0;
		globals::profilelo = (byte*)l1;
	}

	return mr.pos;
}

pcstr locals::IListAccessor::savecontent ( pcstr begin, pcstr end )
{
	return NULL;
}





// real list

locals::FListAccessor::FListAccessor ( pcstr ink, float** inv )
{
	key = ink;
	val = inv;
}

pcstr locals::FListAccessor::loadcontent ( pcstr begin, pcstr end )
{
	MatchResult<pcstr> mr(begin,true);
	tokA s;

	float* l0 = (float*) util::Round4(globals::profilelo);
	float* l1 = l0;

	for( ;; )
	{
		if( ((byte*)l1) >= globals::profilehi )
			return NULL;

		s.clear();
		mr = rx_real::match(mr.pos,end,s);
		if( !mr.success )
			break;

		SU_REQUIRE_OR_NULL( s.size() > 0 );
		float f = read_real( s[0].begin, s[0].end );
		*l1++ = f;
	}

	if( l1==l0 )
		return NULL;

	if( val )
	{
		*val = l0;
		globals::profilelo = (byte*)l1;
	}

	return mr.pos;
}

pcstr locals::FListAccessor::savecontent ( pcstr begin, pcstr end )
{
	return NULL;
}






