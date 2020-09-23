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


// http://tools.ietf.org/html/rfc5849
// http://abcdrfc.free.fr/rfc-vf/rfc1945.html
// http://tools.ietf.org/html/rfc3986#section-3

// http://www.commentcamarche.net/contents/securite/cookies.php3
// http://www.commentcamarche.net/contents/internet/http.php3#q=http&cur=1&url=%2F

// header field definitions : http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html



/*

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
*/

#include <sufw/su_system.h>
#include <sufw/core/su_core.h>
#include <sufw/asset/su_idatatree.h>
using namespace su;
using namespace su::inet;
using namespace su::rexp;



#define	INET_MAX_CONX			(8)
#define	INET_MAX_LOOP			(8)
#define INET_SND_RCV_BUFFER		(4*1024)
#define INET_COOKIE_JAR_MAX		(20)



namespace
{

	Pool* inet_pool ( )
	{
		SU_ASSERT( globals::syspool );
		return globals::syspool;
	}


	#include "su_inet_url.h"


	#include "su_inet_cookie.h"
	CookieJar	cookjar;


	#include "su_inet_conx.h"
	bool		conx_used	[INET_MAX_CONX];
	Conx		conx		[INET_MAX_CONX];


	void co_initall ( )
	{
		for( uint i=0 ; i<INET_MAX_CONX ; i++ )
			conx_used[i] = false;
	}


	void co_shutall ( )
	{
		for( uint i=0 ; i<INET_MAX_CONX ; i++ )
			if( conx_used[i] )
				conx[i].shut();
	}

	
	Conx* co_get( int inCo )
	{
		SU_REQUIRE_OR_NULL( inCo >= 0 );
		SU_REQUIRE_OR_NULL( inCo < INET_MAX_CONX );
		SU_REQUIRE_OR_NULL( conx_used[inCo] );
		return conx+inCo;
	}

}






int inet::Create ( )
{
	for( int i=0 ; i<INET_MAX_CONX ; i++ )
	{
		if( conx_used[i] )
			continue;

		if( !conx[i].init() )
			return -1;

		conx_used[i] = true;
		return i;
	}

	return -1;
}


void inet::Release ( int coid )
{
	SU_REQUIRE_OR_RETURN( coid >= 0 );
	SU_REQUIRE_OR_RETURN( coid < INET_MAX_CONX );
	SU_REQUIRE_OR_RETURN( conx_used[coid] );
	conx_used[coid] = false;
	conx[coid].shut();
}


bool inet::SetOpt (	int coid, Option inOpt, const Any& inValue )
{
	Conx* co = co_get( coid );
	SU_REQUIRE_OR_FALSE( co );
	return co->setopt( inOpt, inValue );
}


Any inet::GetOpt ( int coid, Option inOpt )
{
	Conx* co = co_get( coid );
	SU_REQUIRE_OR_FALSE( co );

	return co->getopt( inOpt );
}


void inet::DelOpt (	int coid, Option inOpt )
{
	Conx* co = co_get( coid );
	SU_REQUIRE_OR_RETURN( co );
	co->delopt( inOpt );
}


inet::State inet::GetState ( int coid )
{
	Conx* co = co_get( coid );
	SU_REQUIRE_OR_RETURN_V( co, inet::S_INVALID );
	return co->getstate();
}


void inet::Cleanup ( int coid )
{
	Conx* co = co_get( coid );
	SU_REQUIRE_OR_RETURN( co );
	return co->cleanup();
}


bool inet::Connect ( int coid )
{
	Conx* co = co_get( coid );
	SU_REQUIRE_OR_FALSE( co );
	return co->connect();
}


void inet::Disconnect (	int coid )
{
	Conx* co = co_get( coid );
	SU_REQUIRE_OR_RETURN( co );
	co->disconnect();
}


int inet::Receive ( int coid, void* inBuffer, uint inBSize )
{
	Conx* co = co_get( coid );
	SU_REQUIRE_OR_NEG( co );
	return co->receive( inBuffer, inBSize );
}


int inet::Send ( int coid, void* inBuffer, uint inBSize )
{
	Conx* co = co_get( coid );
	SU_REQUIRE_OR_NEG( co );
	return co->send( inBuffer, inBSize );
}


int inet::Perform ( int coid, uint inmaxmstime )
{
	Conx* co = co_get( coid );
	SU_REQUIRE_OR_NEG( co );
	return co->perform( inmaxmstime );
}


bool inet::DecomposeURL ( URL& out_u, pcstr u )
{
	return inet_decompose_url( out_u, u );
}


void inet::CleanupCookies	( )
{
	cookjar.cleanup();
}


bool inet::PushCookies ( su::IDatatree* indtt )
{
	return cookjar.push( indtt );
}



bool inet::LoadCookies ( su::IDatatree* indtt )
{
	return cookjar.load( indtt );
}


pcstr inet::GetCookie (	pcstr indomain, pcstr inname )
{
	return cookjar.get(indomain,inname);
}




SU_BEGIN_STARTER( CoreINet )

void OnStage ( Stage stage )
{
	if( stage == STAGE_START )
	{
		co_initall();
		cookjar.cleanup();
	}
	else if( stage == STAGE_TERMINATE )
	{
		co_shutall();
		cookjar.cleanup();
	}
}

SU_END_STARTER( CoreINet )





