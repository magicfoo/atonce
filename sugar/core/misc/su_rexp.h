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



#ifndef _su_rexp_h_
#define _su_rexp_h_



namespace su {	namespace rexp
{


	//
	// Some actions
	//
	// NullAction	: Do nothing
	//
	// TokenAction	: Create a new token with the matching string part.
	//				  This action calls the UserToken::token( stack::value_type& outRes, iterator first, iterator last ) function.
	//
	// ReduceAction	: Reduce a list of tokens/expressions of type stack::value_type to a single result.
	//				  This action calls the UserReduce::reduce( stack::value_type& outRes, int argc, stack::value_type* argv ) function.

	#define re_action_apply \
		template<typename iterator, typename stack> \
		inline static void apply( stack& s, uint position, iterator first, iterator last );

	struct NullAction
	{
		re_action_apply
	};

	template <typename UserToken>
	struct TokenAction
	{
		re_action_apply
	};

	template <typename UserReduce>
	struct ReduceAction
	{
		re_action_apply
	};



	//
	// Match result

	template <typename iterator>
	struct MatchResult
	{
		bool				success;
		iterator			pos;
							MatchResult			(					);
							MatchResult			( iterator, bool	);
		static MatchResult	Failed				( iterator			);
		static MatchResult	Successed			( iterator			);
	};



	// re_action	< re,			Action >	: wraps an existing re with an action.
	// re_any		<				Action >	: matchs any char
	// re_char		< C,			Action >	: matchs char C
	// re_nchar		< C,			Action >	: matchs any char except C
	// re_range		< C0, C1,		Action >	: matchs char included in [C0,C1]
	// re_nrange	< C0, C1,		Action >	: matchs char not included in [C0,C1]
	// re_or		< re0, re1,		Action >	: matchs re0 | re1 (ex : "a|b" )
	// re_token		< re,			Action >	: matchs (no-printable*)re (ex : "  a")
	// re_star		< re,			Action >	: matchs re* (ex : "a*")
	// re_plus		< re,			Action >	: matchs re+ (ex : "a+")
	// re_opt		< re,			Action >	: matchs re? (ex : "a?")
	// re_repeat	< re,  N,	M,	Action >	: matchs re{N,M} (ex : "(ab){4,8}")
	// re_action	< re,			Action >	: wraps an existing re with an action.
	// re_cdata		< re,			Action >	: matchs characters until the regexp re matchs (ex : "(a*)" matchs "1234" in "1234aa")
	// re_follow	< re0, re1,		Action >	: matchs re0re1 (ex : "ab")
	// re_followng	< re0, re1,		Action >	: no greedy, matchs re0re1 (ex : "(a*)(ab)" matchs "aaab")

	#define re_match_wstack \
		template<typename iterator, typename stack> \
		inline static MatchResult<iterator> match( iterator first, iterator end, stack& s );

	#define re_match_nostack \
		template<typename iterator> \
		inline static MatchResult<iterator> match( iterator first, iterator end );


	template<int C, typename Action=NullAction>
	struct re_char	
	{
		re_match_wstack
		re_match_nostack
	};

	template<int C, typename Action=NullAction>
	struct re_nchar	
	{
		re_match_wstack
		re_match_nostack
	};

	template<int C0,int C1, typename Action=NullAction> 
	struct re_range
	{
		re_match_wstack
		re_match_nostack
	};

	template<int C0,int C1, typename Action=NullAction> 
	struct re_nrange
	{
		re_match_wstack
		re_match_nostack
	};

	template<typename Action=NullAction>
	struct re_any	// "." : any character
	{
		re_match_wstack
		re_match_nostack
	};

	template<typename re0, typename re1, typename Action=NullAction>
	struct re_or
	{
		re_match_wstack
		re_match_nostack
	};

	template<typename re0, typename re1, typename Action=NullAction>
	struct re_follow
	{
		re_match_wstack
		re_match_nostack
	};

	template<typename re0, typename re1, typename Action=NullAction>
	struct re_followng
	{
		re_match_wstack
		re_match_nostack
	};

	template<typename re, typename Action=NullAction>
	struct re_token
	{
		re_match_wstack
		re_match_nostack
	};

	template<typename re, typename Action=NullAction>
	struct re_star
	{
		re_match_wstack
		re_match_nostack
	};

	template<typename re, typename Action=NullAction>
	struct re_plus
	{
		re_match_wstack
		re_match_nostack
	};

	template<typename re, typename Action=NullAction>
	struct re_opt
	{
		re_match_wstack
		re_match_nostack
	};

	template<typename re, unsigned int N, unsigned int M=N, typename Action=NullAction>
	struct re_repeat
	{
		re_match_wstack
		re_match_nostack
	};

	template<typename re, typename Action> 
	struct re_action
	{
		re_match_wstack
		re_match_nostack
	};

	template<typename re, typename Action=NullAction> 
	struct re_cdata
	{
		re_match_wstack
		re_match_nostack
	};



	//
	// Some popular regexps

	typedef re_char		< ' ' >								_s;			// space
	typedef re_char		< '\t' >							_t;			// tab
	typedef re_char		< '\n' >							_n;			// new line
	typedef re_char		< '\r' >							_r;			// carriage return
	typedef re_char		< '_' >								_u;			// underscore
	typedef re_char		< '\'' >							_q;			// quote
	typedef re_char		< '\"' >							_dq;		// double quote
	typedef re_char		< '.' >								_p;			// point
	typedef re_char		< ',' >								_c;			// comma

	typedef re_or		< _s,_t >							_st;		// space or tab
	typedef re_star		< _st >								_stN;		// (space or tab)*
	typedef re_range	< 1, 32 >							_np;		// no printable
	typedef re_star		< _np >								_npN;		// (no printable)*
	typedef re_opt		< _r >								_ropt;
	typedef re_follow	< _ropt, _n >						_ropt_n;
	typedef re_or		< _ropt_n, _r >						_ln;		// logical new line
	typedef re_range	< 'a', 'z' >						_lo;		// lower case
	typedef re_range	< 'A', 'Z' >						_up;		// upper case
	typedef re_or		< _lo, _up >						_le;		// letter
	typedef re_range	< '0', '9' >						_di;		// digit
	typedef re_or		< _le , _di >						_aln;		// alphanum
	typedef re_or		< _aln , _u	>						_w;			// a word letter

	typedef re_range	< 'a', 'f' >						_lohex;		// lower case letter for hexa
	typedef re_range	< 'A', 'F' >						_uphex;		// upper case letter for hexa
	typedef re_or		< _lohex, _uphex >					_lhex;		// letter for hexa
	typedef re_or		< _di	, _lhex >					_xdi;		// hexadecimal digit

	typedef re_plus		< _le >								re_word;

	typedef re_follow	< re_or <_le,_u>,
						  re_star<_w> >						re_id;

	typedef	re_opt		< re_char< '-' > >					re_minus;
	typedef	re_follow   < re_minus, _stN >					re_minusst;

	typedef re_plus		< _di >								re_uinteger;

	typedef re_follow	< re_minusst,
						  re_plus<_di> >					re_integer;

	typedef re_follow	< re_minusst,
			re_follow	< re_plus<_di>,
			re_opt		<
			re_follow	< re_char<'.'>,
						  re_plus<_di> > > > >				re_real;

	typedef	re_plus		< _xdi >							re_hexcore;

	typedef	re_follow	< re_char<'0'>,
			re_follow	< re_char<'x'>,
						  re_hexcore > >					re_hexadecimal;

	typedef	re_follow	< re_char<'0'>,
			re_follow	< re_char<'p'>,
						  re_hexcore > >					re_realhex;



	//
	// Type

	template< typename iterator, uint Bound = 0 >
	struct re_Type
	{
		// Token

		struct Token
		{
			iterator	begin;
			iterator	end;
		};

		// Stack

		typedef vector<Token,Bound> Stack;

		// Action

		struct UserToken
		{
			inline static void token( Token& t, iterator first, iterator last )
			{
				SU_ASSERT( first != last );
				t.begin	= first;
				t.end   = last;
			}
		};

		typedef TokenAction<UserToken> Action;

		// Some basic tokens

		typedef re_token< re_uinteger, Action >		uinteger;
		typedef re_token< re_integer,  Action >		integer;
		typedef re_token< re_word,     Action >		word;
		typedef re_token< re_id,       Action >		id;
		typedef re_token< re_real,     Action >		real;
	};


}};


#undef	re_match_wstack
#undef	re_match_nostack
#undef	re_action_apply


#include "su_rexpi.h"

#endif //_su_rexp_h_


