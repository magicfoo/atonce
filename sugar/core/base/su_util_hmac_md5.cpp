/*LIC-HDR********************************************************************
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


#include "../su_base.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "md5/md5.h"
using namespace su;



namespace
{

	bool			md5_session_began = false;
	md5_state_s		md5_session;
	md5_byte_t		md5_digest;

}


void util::hmac_md5_begin ( )
{
	md5_init( &md5_session );
	md5_session_began = true;
}


void util::hmac_md5_append ( pointer inBuffer, uint inBSize )
{
	SU_ASSERT( md5_session_began );
	SU_REQUIRE_OR_RETURN( md5_session_began );
	SU_REQUIRE_OR_RETURN( inBuffer );
	SU_REQUIRE_OR_RETURN( inBSize  );
	md5_append( &md5_session, (const md5_byte_t*)inBuffer, inBSize );
}


byte* util::hmac_md5_end ( )
{
	SU_REQUIRE_OR_NULL( md5_session_began );
	md5_finish( &md5_session, &md5_digest );
	md5_session_began = false;
	return (byte*)md5_digest;
}


byte* util::hmac_md5 ( pointer inSegment, uint inBSize )
{
	hmac_md5_begin();
	hmac_md5_append( inSegment, inBSize );
	return hmac_md5_end();
}


uint128 util::hmac_md5_w ( pointer inSegment, uint inBSize )
{
	uint128 w;
	Zero( w );

	byte* p = hmac_md5( inSegment, inBSize );
	SU_REQUIRE_OR_RETURN_V( p, w );

	Memcopy( &w, p, 16 );
	return MSBToNative(w);
}


