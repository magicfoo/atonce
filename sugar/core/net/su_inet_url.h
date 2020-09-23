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


//
// URL of the form :
//
//		[proto://][username[:password]@](ip|hostname)[:port][/path/][?query][#fragment]
//



struct URL_proto {
	static void token( URL& u, pcstr first, pcstr last ) {
		u.proto.copy( first, 0, last-first );
	} };

struct URL_uname {
	inline static void token( URL& u, pcstr first, pcstr last ) {
		u.uname.copy( first, 0, last-first );
	} };

struct URL_passwd {
	inline static void token( URL& u, pcstr first, pcstr last ) {
		u.passwd.copy( first, 0, last-first );
	} };

struct URL_hname {
	inline static void token( URL& u, pcstr first, pcstr last ) {
		u.hname.copy( first, 0, last-first );
	} };

struct URL_port {
	inline static void token( URL& u, pcstr first, pcstr last ) {
		u.port = util::Strtou10(first);
	} };

struct URL_path {
	inline static void token( URL& u, pcstr first, pcstr last ) {
		u.path.copy( first, 0, last-first );
	} };

struct URL_query {
	inline static void token( URL& u, pcstr first, pcstr last ) {
		u.query.copy( first, 0, last-first );
	} };

struct URL_frag {
	inline static void token( URL& u, pcstr first, pcstr last ) {
		u.frag.copy( first, 0, last-first );
	} };



typedef		re_or<		re_follow< re_char<'2'>, re_follow< re_char<'5'>, re_range<'0','5'> > >,			// [250,255]
			re_or<		re_follow< re_char<'2'>, re_follow< re_range<'0','4'>, re_range<'0','9'> > >,		// [200,249]
			re_or<		re_follow< re_char<'1'>, re_follow< re_range<'0','9'>, re_range<'0','9'> > >,		// [100,199)
			re_or<		re_follow< re_range<'1','9'>, re_range<'0','9'> >,									// [10,00]
						re_range<'0','9'> > > > >										re_url_ip_u8;		// [0,9]


typedef		re_follow<	re_uinteger,
			re_star<	re_follow< re_char<'.'>, re_uinteger > > >						re_url_ip;


typedef		re_follow<	re_url_ip_u8,
			re_follow<	re_char<'.'>,
			re_follow<	re_url_ip_u8,
			re_follow<	re_char<'.'>,
			re_follow<	re_url_ip_u8,
			re_follow<	re_char<'.'>,
						re_url_ip_u8 > > > > > >										re_url_ipv4;


typedef		re_follow<	re_action<re_word,TokenAction<URL_proto > >,
			re_follow<	re_char<':'>,
			re_follow<	re_char<'/'>,
						re_char<'/'> > > >												re_url_proto;


typedef		re_follow<	re_action<re_id,TokenAction<URL_uname > >,
			re_follow<	re_opt<	re_follow< re_char<':'>, re_action<re_id,TokenAction<URL_passwd > > > >,
						re_char<'@'> > >												re_url_login;


typedef		re_plus< re_or< _w, re_char<'-'> > >										re_url_domain;


typedef		re_follow<	re_url_domain,
			re_star<	re_follow< re_char<'.'>, re_url_domain > > >					re_url_hname;


typedef		re_action< re_url_hname, TokenAction<URL_hname> >							re_url_addr;


typedef		re_follow<	re_char<':'>,
						re_action< re_uinteger,TokenAction<URL_port> > >				re_url_port;


typedef		re_follow<	re_char<'/'>,
						re_star< re_nchar<'?'>, TokenAction<URL_path> > >				re_url_path;


typedef		re_follow<	re_char<'?'>,
						re_star< re_nchar<'#'>, TokenAction<URL_query> > >				re_url_query;


typedef		re_follow<	re_char<'#'>,
						re_action< re_id, TokenAction<URL_frag> > >						re_url_frag;


typedef		re_follow<	re_opt<re_url_proto>,
			re_follow<	re_opt<re_url_login>,
			re_follow<	re_url_addr,
			re_follow<	re_opt<re_url_port>,
			re_follow<	re_opt<re_url_path>,
			re_follow<	re_opt<re_url_query>,
						re_opt<re_url_frag> > > > > > >									re_url;



bool inet_decompose_url( URL& out_u, pcstr u )
{
	SU_REQUIRE_OR_FALSE( u );

	pcstr eu = util::Strend(u);
	if( eu == u )
		return false;

	vector<URL> s( globals::syspool );
	MatchResult<pcstr> m = re_url::match( u, eu, s );

	// fully matched ?
	if( !m.success || m.pos!=eu )
		return false;

	// merge elements
	out_u = URL();
	for( int i=0 ; i<s.size() ; i++ )
	{
		if( s[i].proto.size() )		out_u.proto  = s[i].proto;
		if( s[i].uname.size() )		out_u.uname  = s[i].uname;
		if( s[i].passwd.size() )	out_u.passwd = s[i].passwd;
		if( s[i].hname.size() )		out_u.hname  = s[i].hname;
		if( s[i].port )				out_u.port   = s[i].port;
		if( s[i].path.size() )		out_u.path   = s[i].path;
		if( s[i].query.size() )		out_u.query  = s[i].query;
		if( s[i].frag.size() )		out_u.frag   = s[i].frag;
	}

	pcstr h  = out_u.hname.cstr();
	pcstr eh = util::Strend(h);

	if( h && eh>h )
	{
		// hostname is an ip ?
		m = re_url_ip::match(h,eh);
		if( m.success && m.pos==eh )
		{
			// hostname is an valid ipv4 ?
			m = re_url_ipv4::match(h,eh);
			if( !m.success || m.pos!=eh )
				return false;

			out_u.ip = out_u.hname;
		}
	}

	return true;
}


