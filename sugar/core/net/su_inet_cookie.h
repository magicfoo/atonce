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



/*

http://www.html-kit.com/tools/cookietester/

*/




struct Cookie
{
	SString			name;
	LString			value;
	SString			domain;
	PString			path;
	LString			expires;
	bool			secure;
	bool			httponly;

					Cookie			(					);
	void			cleanup			(					);
	bool			set				( pcstr inCookie	);
	bool			equals			( Cookie& inref		);
	void			push			( IDatatree* indtt	);
	void			load			( IDatatree* indtt	);
};




struct CookieJar
{
	static const uint BOUND = INET_COOKIE_JAR_MAX;
	Cookie			cookies	[BOUND];
	
	void			cleanup			(					);
	bool			add				( Cookie& incook	);
	PString			collect			( URL& u			);
	pcstr			get				( pcstr indomain, pcstr inname );
	bool			push			( IDatatree* indtt	);
	bool			load			( IDatatree* indtt	);
};









Cookie::Cookie ( )
{
	cleanup();
}


void Cookie::cleanup ( )
{
	name.clear();
	value.clear();
	domain.clear();
	path.clear();
	expires.clear();
	secure = false;
	httponly = false;
}


void ck_attr ( LString& ioline, LString& outname, LString& outvalue )
{
	// name=value ; name=value ...
	outname.clear();
	outvalue.clear();

	// split attribute
	LString attr;
	if( ioline.indexof(';') > 0 ) {
		ioline.splitsep(";",attr,ioline);
	} else {
		attr = ioline;
		ioline.clear();
	}

	// split name/value
	if( attr.indexof('=') > 0 ) {
		attr.splitsep("=",outname,outvalue);
	} else {
		outname = attr;
	}

	ioline.strip();
	outname.strip();
	outvalue.strip();
}


bool Cookie::set ( pcstr inCookie )
{
	// [Set-Cookie:] LSID=DQAAAK…Eaem_vYg; Domain=docs.foo.com; Path=/accounts; Expires=Wed, 13-Jan-2021 22:23:01 GMT; Secure; HttpOnly
	SU_REQUIRE_OR_FALSE( inCookie );

	cleanup();

	LString hline( inCookie );
	LString _name, _value;

	while( hline.size() )
	{
		ck_attr( hline, _name, _value );

		if		( _name.equals("Domain",false) )	domain = _value;
		else if	( _name.equals("Path",false) )		path = _value;
		else if	( _name.equals("Expires",false) )	expires = _value;
		else if	( _name.equals("Secure",false) )	secure = true;
		else if	( _name.equals("HttpOnly",false) )	httponly = true;
		else if	( _value.size() )					{ name=_name; value=_value; }
	}

	return !name.empty();
}


bool Cookie::equals	( Cookie& inref )
{
	if( !name.equals(inref.name) )			return false;
	if( !value.equals(inref.value) )		return false;
	if( !domain.equals(inref.domain) )		return false;
	if( !path.equals(inref.path) )			return false;
	if( !expires.equals(inref.expires) )	return false;
	if( secure != inref.secure )			return false;
	if( httponly != httponly )				return false;
	return true;
}


void Cookie::push ( su::IDatatree* indtt )
{
	SU_REQUIRE_OR_RETURN( indtt );
	indtt->pushmapping();
	indtt->pushstring( name );			indtt->store(-2,"name");
	indtt->pushstring( value );			indtt->store(-2,"value");
	indtt->pushstring( domain );		indtt->store(-2,"domain");
	indtt->pushstring( path );			indtt->store(-2,"path");
	indtt->pushstring( expires );		indtt->store(-2,"expires");
	indtt->pushboolean( secure );		indtt->store(-2,"secure");
	indtt->pushboolean( httponly );		indtt->store(-2,"httponly");
}


void Cookie::load ( su::IDatatree* indtt )
{
	indtt->push(-1,"name");			name = indtt->asstring(-1);			indtt->pop(1);
	indtt->push(-1,"value");		value = indtt->asstring(-1);		indtt->pop(1);
	indtt->push(-1,"domain");		domain = indtt->asstring(-1);		indtt->pop(1);
	indtt->push(-1,"path");			path = indtt->asstring(-1);			indtt->pop(1);
	indtt->push(-1,"expires");		expires = indtt->asstring(-1);		indtt->pop(1);
	indtt->push(-1,"secure");		secure = indtt->asboolean(-1);		indtt->pop(1);
	indtt->push(-1,"httponly");		httponly = indtt->asboolean(-1);	indtt->pop(1);
}




void CookieJar::cleanup ( )
{
	for( uint i=0 ; i<BOUND ; i++ )
		cookies[i].cleanup();
}


bool CookieJar::add ( Cookie& incook )
{
	SString indomain = incook.domain;
	SString inname   = incook.name;

	if( inname.empty() )
		return false;

	// find the cookie in the jar
	for( uint i=0 ; i<BOUND ; i++ )
	{
		SString n = cookies[i].name;
		SString d = cookies[i].domain;

		if( n.empty() )
			break;

		if( !n.equals(inname,false) )
			continue;

		if( !d.equals(indomain,false) )
			continue;

		// found and remove !
		for( ; i<BOUND-1 ; i++ )
			cookies[i] = cookies[i+1];
		break;
	}

	// scroll cookies down
	for( uint i=BOUND-1 ; i>0 ; i-- )
		cookies[i] = cookies[i-1];

	// set at the top (index 0)
	cookies[0] = incook;
	return true;
}

	
PString CookieJar::collect ( URL& u )
{
	PString attrs;

	LString udomain = u.hname;

	for( uint i=0 ; i<BOUND ; i++ )
	{
		SString n = cookies[i].name;
		LString v = cookies[i].value;
		SString d = cookies[i].domain;

		if( n.empty() )
			break;

		if( !d.equals(udomain,false) )
			continue;

		if( attrs.size() )
			attrs += "; ";
		attrs += n;
		attrs += "=";
		attrs += v;
	}

	return attrs;
}


pcstr CookieJar::get ( pcstr indomain, pcstr inname )
{
	for( uint i=0 ; i<BOUND ; i++ )
	{
		SString d = cookies[i].domain;
		SString n = cookies[i].name;
		LString v = cookies[i].value;
		if( d.equals(indomain,false) && n.equals(inname,false) )
			return v.cstr();
	}
	return NULL;
}


bool CookieJar::push ( su::IDatatree* indtt )
{
	SU_REQUIRE_OR_FALSE( indtt );

	indtt->pushsequence();

	for( uint i=0 ; i<BOUND ; i++ )
	{
		if( cookies[i].name.size() )
		{
			cookies[i].push( indtt );
			indtt->append(-2);
		}
	}

	return true;
}


bool CookieJar::load ( su::IDatatree* indtt )
{
	SU_REQUIRE_OR_FALSE( indtt );
	SU_REQUIRE_OR_FALSE( indtt->issequence(-1) );

	cleanup();

	uint n = indtt->size(-1);
	if( n > BOUND  )
		return false;

	for( uint i=0 ; i<n; i++ )
	{
		indtt->push(-1,i);
		cookies[i].load( indtt );
		indtt->pop(1);
	}

	return true;
}




