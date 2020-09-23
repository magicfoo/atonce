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
#include <sufw/su_uids.h>
using namespace su;
using namespace su::rexp;





namespace
{


	typedef su::BString<8*1024>			LineString;
	typedef su::BString<128>			TokString;
	typedef su::vector<TokString,16>	TokStringA;


	struct ctok
	{
		pcstr		begin;
		pcstr		end;
	};


	struct ctoken
	{
		inline static void token( ctok& t, pcstr first, pcstr last )
		{
			SU_ASSERT( first != last );
			t.begin	= first;
			t.end = last;
		}
	};


	typedef re_token< re_id, TokenAction<ctoken> >			rx_id;

	typedef re_token< re_char<'['> >						rx_op_brack;
	typedef re_token< re_char<']'> >						rx_cl_brack;
	typedef re_token< re_char<':'> >						rx_colon;
	typedef re_token< re_char<'='> >						rx_equ;


	// [ id ]
	// [ id : parent-id parent-id ... ]

	typedef re_opt< re_follow<rx_colon,re_plus<rx_id> > >	rx_opt_id_list;

	typedef re_follow< rx_op_brack,
			re_follow< rx_id,
			re_follow< rx_opt_id_list,
					   rx_cl_brack > > >					rx_section;


	// id = ...

	typedef re_follow<	rx_id,
						rx_equ >							rx_id_assign;



	struct cfile
	{
		// content lo/hi
		byte*	ctnt_lo;
		byte*	ctnt_hi;

		// pool lo/hi
		byte*	pool_lo;
		byte*	pool_hi;

		// error lineno
		int		err_lineno;
		pcstr	err_msg;


		template <typename T>
		T raise_error ( int lineno, pcstr inmessage, T retvalue )
		{
			err_lineno = lineno;
			err_msg    = inmessage;
			#if SU_BUILD_DBGCODE
			SU_BREAK();
			#endif
			return retvalue;
		}

		int clen ( byte* p )
		{
			if( !p || p>=ctnt_hi )
				return 0;
			return (ctnt_hi - p);
		}

		void* pool_alloc ( uint inbs )
		{
			byte* p0 = util::Round4(pool_lo);
			byte* p1 = p0 + inbs;
			if( p1 > pool_hi )
				return NULL;
			pool_lo = p1;
			return p0;
		}

		byte* eat_line ( byte* p, int& lineno )
		{
			while( clen(p) )
			{
				if( p[0]==10 || p[0]==13 )
					break;
				p++;
			}
			return p;
		}

		byte* eat_space ( byte* p, int& lineno )
		{
			while( clen(p) )
			{
				if( p[0]>32 )
					break;
				if( p[0]=='\n' )
					lineno++;
				p++;
			}
			return p;
		}

		byte* eat_space_and_comment ( byte* p, int& lineno )
		{
			for( ;; )
			{
				byte* pp = p;

				p = eat_space(p,lineno);

				if( clen(p)==0 )
					break;

				if( p[0]=='#' ) {
					p = eat_line(p,lineno);
					continue;
				}

				if( clen(p)>1 && p[0]=='/' && p[1]=='/' ) {
					p = eat_line(p,lineno);
					continue;
				}

				if( clen(p)>1 && p[0]=='-' && p[1]=='-' ) {
					p = eat_line(p,lineno);
					continue;
				}

				if( p==pp )
					break;
			}
			return p;
		}

		byte* next_line ( byte* p, LineString& outl, int& lineno )
		{
			outl.clear();
			if( clen(p) )
			{
				byte* p0   = eat_space_and_comment( p, lineno );
				byte* p1   = eat_line( p0, lineno );
				int   plen = p1-p0;
				if( plen >= LineString::bound )
					return raise_error( lineno, "line is too long !", (byte*)NULL );
				outl.copy( (char*)p0, 0, plen );
				return p1;
			}
			return p;
		}

		byte* first_section ( TokString& outname, TokStringA& outparents, int& lineno )
		{
			lineno = 1;
			return next_section( ctnt_lo, outname, outparents, lineno );
		}

		byte* next_section ( byte* p, TokString& outname, TokStringA& outparents, int& lineno )
		{
			outname.clear();
			outparents.clear();

			while( clen(p) )
			{
				LineString l;
				p = next_line( p, l, lineno );
				SU_REQUIRE_OR_NULL( p );

				vector<ctok,16> s;
				if( rx_section::match(&l.begin(),&l.end(),s).success )
				{
					for( uint i=0; i<s.size() ; i++ )
					{
						TokString ts;
						ts.copy( s[i].begin, 0, s[i].end-s[i].begin );
						if( i==0 )	outname = ts;
						else		outparents.push_back( ts );
					}
					break;
				}
			}

			return p;
		}

		LineString get_line_options ( LineString& l, bool& isoptional, bool& isglobal )
		{
			// true if the line is ended with the reserved token 'isexisting'
			LineString ll( l );
			ll.strip();
			isglobal   = ll.startswith( "g_" );
			isoptional = ll.endswith( "ifexisting" );
			if( isoptional )	ll.erasend(10);
			return ll;
		}

		bool load_section_lines ( byte* p, int& lineno )
		{
			for( ;; )
			{
				if( !clen(p) )
					return true;

				LineString l;
				p = next_line( p, l, lineno );
				
				if( l.empty() )
					continue;

				// multi lines ?
				LineString ll = l;
				while( clen(p) && ll.endswith('\\') )
				{
					p = next_line( p, ll, lineno );
					l.erasend( 1 );
					l += ll;
				}

				// section header ?
				if( rx_section::match(l.begin(),l.end()).success )
					return true;

				// stripping
				l.strip();

				// id = ... ?
				vector<ctok,4> s;
				MatchResult<pcstr> mr;
				mr = rx_id_assign::match( (pcstr)l.begin(), (pcstr)l.end(), s );
				if( !mr.success )
					return raise_error( lineno, "invalid token !", false );

				// split id = value
			//	TokString id  = l.left( l.offsetof(s[0].end) ).strip();
				LineString id  = l.left( l.offsetof(s[0].end) ).strip();
				LineString va = l.right( l.offsetof(mr.pos) ).strip();

				// options
				bool isoptional;
				bool isglobal;
				va = get_line_options( va, isoptional, isglobal );

				locals::Accessor* acc;
				acc = locals::FindAccessor( id.cstr() );
				if( acc )
				{
					if( !acc->loadcontent( va.begin(), va.end() ) )
						return raise_error( lineno, "assignation has failed !", false );
				}
				else if( !isoptional )
				{
					return raise_error( lineno, "not assigned !", false );
				}
			}
		}

		bool load_section ( TokString& inname )
		{
			SU_REQUIRE_OR_FALSE( inname.len() );

			int lineno;

			// find section
			TokString  sname;
			TokStringA sparents;
			byte* p = first_section( sname, sparents, lineno );
			for( ;; )
			{
				if( !clen(p) )
					return false;

				if( !sname.equals(inname) ) {
					p = next_section( p, sname, sparents, lineno );
					continue;
				}

				// load parent sections
				for( uint i=0 ; i<sparents.size() ; i++ ) {
					if( !load_section(sparents[i]) )
						return false;
				}

				// load section lines
				return load_section_lines( p, lineno );
			}
		}

		bool setup ( byte* incbegin, byte* incend )
		{
			SU_REQUIRE_OR_FALSE( incbegin );
			SU_REQUIRE_OR_FALSE( incend );
			SU_REQUIRE_OR_FALSE( incend >= incbegin );

			uint cbs = incend - incbegin;

			IAllocator* ator = factory::CreateObject<IAllocator>( uid::MAllocatorStd, NULL );
			SU_REQUIRE_OR_FALSE( ator );

			byte* profilemem = (byte*) ator->Alloc( cbs );
			SU_REQUIRE_OR_FALSE( profilemem );

			globals::profilelo = profilemem;
			globals::profilehi = profilemem + cbs;

			ctnt_lo = incbegin;
			ctnt_hi = incend;

			err_lineno = -1;
			err_msg    = NULL;
			return true;
		}
	};

}



bool su_load_profile_from_memory( pcstr inpname, byte* incontentbegin, byte* incontentend )
{
	cfile cf;

	if( cf.setup(incontentbegin,incontentend) )
	{
		TokString ts = TokString(inpname);

		if( cf.load_section(ts) )
		{
			if( globals::profilehi >= globals::profilelo )
			{
				return true;
			}
		}
	}

	return false;
}


