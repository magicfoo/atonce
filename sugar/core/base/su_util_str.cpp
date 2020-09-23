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

SU_CC_OPEN_C
#include "su_langc.h"
SU_CC_CLOSE_C

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

using namespace su;




namespace
{

	const uint32 t_space[]		= {	0x3e00, 1, 0, 0, 0, 0, 0, 0 };
	const uint32 t_alpha[]		= {	0, 0, 0x7FFFFFE, 0x7FFFFFE, 0, 0, 0, 0 };
	const uint32 t_upper[]		= {	0, 0, 0x7FFFFFE, 0, 0, 0, 0, 0 };
	const uint32 t_lower[]		= {	0, 0, 0, 0x7FFFFFE, 0, 0, 0, 0 };
	const uint32 t_digit[]		= {	0, 0x3FF0000, 0, 0, 0, 0, 0, 0 };
	const uint32 t_digit2[]		= {	0, 0x30000, 0, 0, 0, 0, 0, 0 };
	const uint32 t_digit16[]	= {	0, 0x3FF0000, 0x7E, 0x7E, 0, 0, 0, 0 };
	const uint32 t_alphanum[]	= {	0, 0x3FF0000, 0x7FFFFFE, 0x7FFFFFE, 0, 0, 0, 0 };
	const uint32 t_ident[]		= {	0, 0x3FF0000, 0x87FFFFFE, 0x7FFFFFE, 0, 0, 0, 0 };

	inline uint is_a( uchar c, const uint32* t )
	{
		SU_ASSERT( c>=0 && c<=255 );
		return t[c>>5] & (1<<(c&31));
	}

	inline bool are_all( pcstr s, const uint32* t, bool defempty=false )
	{
		if( !s )	return false;
		if( !*s )	return defempty;
		for( ; *s ; s++ ) {
			if( !is_a(*s,t) )
				return false;
		}
		return true;
	}

	inline bool are_none( pcstr s, const uint32* t, bool defempty=true )
	{
		if( !s )	return true;
		if( !*s )	return defempty;
		for( ; *s ; s++ ) {
			if( is_a(*s,t) )
				return false;
		}
		return true;
	}

}






int su::util::Toupper(	int c )
{
	if( is_a(c,t_lower) )
		return c-'a'+'A';
	else
		return c;
}


int su::util::Tolower(	int c )
{
	if( is_a(c,t_upper) )
		return c-'A'+'a';
	else
		return c;
}


bool su::util::Isupper( int c )
{
	return is_a(c,t_upper) != 0;
}


bool su::util::Islower( int c )
{
	return is_a(c,t_lower) != 0;
}


bool su::util::Isdigit ( int c )
{
	return is_a(c,t_digit) != 0;
}


bool su::util::Isdigit2 ( int c )
{
	return is_a(c,t_digit2) != 0;
}


bool su::util::Isdigit16 ( int c )
{
	return is_a(c,t_digit16) != 0;
}


bool su::util::Isspace ( int c )
{
	return is_a(c,t_space) != 0;
}


bool su::util::Isalpha ( int c )
{
	return is_a(c,t_alpha) != 0;
}


bool su::util::Isalnum ( int c )
{
	return is_a(c,t_alphanum) != 0;
}


bool su::util::Strisempty ( pcstr s )
{
	return (s==NULL) || (s[0]==0);
}



bool su::util::Strisspace ( pcstr s )
{
	return are_all(s,t_space);
}


bool su::util::Strisalpha ( pcstr s )
{
	return are_all(s,t_alpha);
}


bool su::util::Strisdigit (	pcstr s		)
{
	return are_all(s,t_digit);
}


bool su::util::Strisdigit2 (	pcstr s		)
{
	return are_all(s,t_digit2);
}


bool su::util::Strisdigit16 (	pcstr s		)
{
	return are_all(s,t_digit16);
}


bool su::util::Strisalnum (	pcstr s		)
{
	return are_all(s,t_alphanum);
}


bool su::util::Strisident ( pcstr s )
{
	return are_all(s,t_ident) && !is_a(s[0],t_digit);
}


bool su::util::Strisupper	(	pcstr s		)
{
	return are_none(s,t_lower);
}


bool su::util::Strislower	(	pcstr s		)
{
	return are_none(s,t_upper);
}


pstr su::util::Strcopy(	pstr  inDest,  pcstr inSrc	)
{
	return ::strcpy( inDest, inSrc );
}


pstr su::util::Strcopy(	pstr  inDest,  pcstr inSrc, uint count	)
{
	return ::strncpy( inDest, inSrc, count );
}


pstr su::util::Strconcat(	pstr  inDest,  pcstr inSrc	)
{
	return ::strcat( inDest, inSrc );
}


uint su::util::Strlen	(	pcstr inString	)
{
	if( !inString )
		return 0;
	return (uint)::strlen( inString );
}


pcstr su::util::Strend ( pcstr s )
{
	if( !s )	return NULL;
	int l = Strlen(s);
	return s+l;
}


pcstr su::util::Strlast ( pcstr s )
{
	if( !s )	return NULL;
	if( !*s )	return NULL;
	int l = Strlen(s);
	return s+l-1;
}


int su::util::Strcompare	(	pcstr string, int inC, bool cs	)
{
	char tmp[2] = { inC, '\0' };
	return Strcompare( string, tmp, cs );
}


int su::util::Strcompare	(	pcstr string1, pcstr string2, bool cs	)
{
	if( string1 && *string1==0 )	string1 = NULL;
	if( string2 && *string2==0 )	string2 = NULL;
	if( !string1 && !string2 )		return 0;
	if( !string1 )					return -1;
	if( !string2 )					return +1;

	if( cs )
	{
		return ::strcmp( string1, string2 );
	}
	else
	{
#if defined(SU_HW_WINDOWS)
		return ::_stricmp( string1, string2 );
#elif defined(SU_HW_DARWIN) || defined (SU_HW_IOS) || defined(SU_HW_ANDROID) || defined(SU_HW_PSP2)
		return ::strcasecmp( string1, string2 );
#else
		return ::stricmp( string1, string2 );
#endif
	}
}


int su::util::Strcompare	(	pcstr string1, pcstr string2, uint count, bool cs	)
{
	if( string1 && *string1==0 )	string1 = NULL;
	if( string2 && *string2==0 )	string2 = NULL;
	if( count == 0 )				return 0;
	if( !string1 && !string2 )		return 0;
	if( !string1 )					return -1;
	if( !string2 )					return +1;

	if( cs )
	{
		return ::strncmp( string1, string2, count );
	}
	else
	{
#if defined(SU_HW_WINDOWS)
		return ::_strnicmp( string1, string2, count );
#elif defined(SU_HW_DARWIN) || defined (SU_HW_IOS) || defined(SU_HW_ANDROID) || defined(SU_HW_PSP2)
		return ::strncasecmp( string1, string2, count );
#else
		return ::strnicmp( string1, string2, count );
#endif
	}
}


bool su::util::Strequals	(	pcstr string, int inC, bool cs	)
{
	return Strcompare( string, inC, cs ) == 0;
}


bool su::util::Strequals	(	pcstr string1, pcstr string2, bool cs	)
{
	return Strcompare( string1, string2, cs ) == 0;
}


bool su::util::Strequals	(	pcstr string1, pcstr string2, uint len, bool cs	)
{
	return Strcompare( string1, string2, len, cs ) == 0;
}


bool su::util::Strstartswith	(	pcstr inString, int inC, bool cs	)
{
	if( inString && *inString ) {
		int c = *inString;
		if( c == inC )
			return TRUE;
		if( !cs && Tolower(c)==Tolower(inC) )
			return TRUE;
	}
	return FALSE;
}


bool su::util::Strstartswith	(	pcstr inString, pcstr inRef, bool cs	)
{
	return Strcompare( inString, inRef, Strlen(inRef), cs ) == 0;
}


bool su::util::Strstartswithany	(	pcstr inString, pcstr inCharSet, bool cs	)
{
	if( !inString || *inString==0 )
		return FALSE;

	if( !inCharSet || *inCharSet==0 )
		return FALSE;

	pcstr s = inCharSet;
	while( *s ) {
		if( Strstartswith(inString,*s,cs) )
			return TRUE;
		s++;
	}

	return FALSE;
}


bool su::util::Strendswith	(	pcstr inString, int inC, bool cs	)
{
	int l = Strlen( inString );
	if( l==0 )
		return FALSE;
	int c = inString[l-1];
	if( c == inC )
		return TRUE;
	if( !cs && Tolower(c)==Tolower(inC) )
		return TRUE;
	return FALSE;
}


bool su::util::Strendswith		(	pcstr inString, pcstr inRef, bool cs	)
{
	int l0 = Strlen( inString );
	int l1 = Strlen( inRef );
	if( l0 < l1 )			return FALSE;
	if( l0==0 && l1==0 )	return TRUE;
	if( l0==0 )				return FALSE;
	return Strcompare( inString+l0-l1, inRef, Strlen(inRef), cs ) == 0;
}


bool su::util::Strendswithany	(	pcstr inString, pcstr inCharSet, bool cs	)
{
	if( !inString || *inString==0 )
		return FALSE;

	if( !inCharSet || *inCharSet==0 )
		return FALSE;

	pcstr s = inCharSet;
	while( *s ) {
		if( Strendswith(inString,*s,cs) )
			return TRUE;
		s++;
	}

	return FALSE;
}


pcstr su::util::Strfind		(	pcstr inString, int inC, bool cs, bool rev	)
{
	if( inString )
	{
		int c0, c1;
		if( cs ) {
			c0 = c1 = inC;
		} else {
			c0 = Tolower(inC);
			c1 = Toupper(inC);
		}
		if( rev )
		{
			pcstr s = inString + Strlen( inString );
			while( s >= inString ) {
				if( *s==c0 || *s==c1 )
					return s;
				s--;
			}
		}
		else
		{
			pcstr s = inString;
			while( *s ) {
				if( *s==c0 || *s==c1 )
					return s;
				s++;
			}
			// The null terminating character is included in the search !
			if( inC==0 )
				return s;
		}
	}
	return NULL;
}


pcstr su::util::Strfind		(	pcstr inString, pcstr inSearch, bool cs, bool rev		)
{
	if( rev )
	{
		pcstr f0 = Strfind( inString, inSearch, cs, FALSE );
		while( f0 ) {
			pcstr f1 = Strfind( f0+1, inSearch, cs, FALSE );
			if( f1 )	f0 = f1;
			else		return f0;
		}
		return f0;
	}
	else
	{
		if( inString && inSearch )
		{
			if( *inSearch == 0 )	return inString;
			if( *inString == 0 )	return NULL;
			int l = Strlen( inSearch );
			pcstr in = Strfind( inString, *inSearch, cs, rev );
			while( in ) {
				if( Strcompare(in,inSearch,l,cs)==0 )
					return in;
				in = Strfind( in+1, *inSearch, cs, rev );
			}
		}
		return NULL;
	}
}


pcstr su::util::Strfindany	(	pcstr inString, pcstr inCharSet, bool cs, bool rev	)
{
	if( !inCharSet )
		return FALSE;

	char tmp[256];
	::memset( tmp, 0, 256 );
	pcstr cset = inCharSet;
	if( cs )
	{
		while( *cset )
		{
			uchar c = *cset++;
			tmp[c] = 1;
		}
	}
	else
	{
		while( *cset )
		{
			uchar c = *cset++;
			tmp[tolower(c)] = 1;
			tmp[toupper(c)] = 1;
		}
	}

	if( rev )
	{
		pcstr s = inString + Strlen(inString);
		while( s >= inString )
		{
			if( tmp[*s] )
				return s;
			s--;
		}
	}
	else
	{
		pcstr s = inString;
		while( *s )
		{
			if( tmp[*s] )
				return s;
			s++;
		}
	}

	return NULL;
}


pstr su::util::Strfind		(	pstr  inString, int inC, bool cs, bool rev )
{
	return (pstr) Strfind( (pcstr)inString, inC, cs, rev );
}

pstr su::util::Strfind		(	pstr  inString, pcstr inSearch, bool cs, bool rev	)
{
	return (pstr) Strfind( (pcstr)inString, inSearch, cs, rev );
}

pstr su::util::Strfindany	(	pstr inString,  pcstr inCharSet, bool cs, bool rev	)
{
	return (pstr) Strfindany( (pcstr)inString, inCharSet, cs, rev );
}


int su::util::Strindexof	(	pcstr inString, int inC, bool cs, bool rev	)
{
	pcstr f = Strfind( inString, inC, cs, rev );
	int pos = f ? int(f-inString) : -1;
	return pos;
}


int su::util::Strindexof	(	pcstr inString, pcstr inSearch, bool cs, bool rev )
{
	pcstr f = Strfind( inString, inSearch, cs, rev );
	int pos = f ? int(f-inString) : -1;
	return pos;
}


int su::util::Strindexofany	(	pcstr inString, pcstr inCharSet, bool cs, bool rev )
{
	pcstr f = Strfindany( inString, inCharSet, cs, rev );
	int pos = f ? int(f-inString) : -1;
	return pos;
}


pstr su::util::Strtolower	(	pstr  inString	)
{
	if( inString )
	{
		pstr s = inString;
		while( *s )
			*s++ = Tolower( *s );
		return inString;
	}
	return NULL;
}


pstr su::util::Strtoupper	(	pstr  	inString	)
{
	if( inString )
	{
		pstr s = inString;
		while( *s )
			*s++ = Toupper( *s );
		return inString;
	}
	return NULL;
}


pstr su::util::Strtoupper	(	pstr  outString, pcstr inString		)
{
	Strcopy( outString, inString );
	return Strtoupper( outString );
}


pstr su::util::Strtolower	(	pstr  outString, pcstr inString		)
{
	Strcopy( outString, inString );
	return Strtolower( outString );
}


int su::util::Strformat ( pstr outString, char c, int rep )
{
	SU_REQUIRE_OR_0( outString );
	pstr s  = outString;
	pstr ss = outString + rep;
	while( s < ss )
		*s++ = c;
	*s = 0;
	return rep;
}


int su::util::Strformat	(	pstr  outString, long a, int base	)
{
	const char* fmt;
	if( base==10 )			fmt = "%d";
	else if( base==16 )		fmt = "%x";
	else if( base==8 )		fmt = "%o";
	else return NULL;
	if( a < 0 ) {
		int res = sprintf( outString+1, fmt, -a );
		if( res )
			outString[0] = '-';
		return res+1;
	} else {
		return sprintf( outString, fmt, a );
	}
}


int su::util::Strformat	(	pstr  outString, ulong a, int base	)
{
	const char* fmt;
	if( base==10 )			fmt = "%u";
	else if( base==16 )		fmt = "%x";
	else if( base==8 )		fmt = "%o";
	else return NULL;
	return sprintf( outString, fmt, a );
}


int su::util::Strformat	(	pstr  outString, float a, char fmt, int prec	)
{
	char fmt2[32];
	sprintf( fmt2, "%%.%d%c", prec, fmt );
	return sprintf( outString, fmt2, a );
}


int su::util::Strformat	( pstr outString, pcstr format, ... )
{
	va_list	arg;
	va_start( arg, format );
	int res = vsprintf( outString, format, arg );
	va_end( arg );
	return res;
}


float su::util::Strtofloat	( pcstr	string, pstr* endp )
{
	SU_REQUIRE_OR_0( string );
	return su_langc_strtofloat( string, endp );
}





long su::util::Strtolong ( pcstr nptr, pstr* endptr, int base )
{
	SU_REQUIRE_OR_0( nptr );
	return ::strtol( nptr, endptr, base );
}


ulong su::util::Strtoulong	( pcstr nptr, pstr* endptr, int base )
{
	SU_REQUIRE_OR_0( nptr );
	return ::strtoul( nptr, endptr, base );
}


long su::util::Strtoi10 (	pcstr nptr )
{
	SU_REQUIRE_OR_0( nptr );
	return Strtolong( nptr, NULL, 10 );
}

ulong su::util::Strtou10 (	pcstr nptr )
{
	SU_REQUIRE_OR_0( nptr );
	return Strtoulong( nptr, NULL, 10 );
}



bool su::util::Stroption (	pcstr inString, pcstr inOption, pcstr& outStart, pcstr& outEnd )
{
	SU_REQUIRE_OR_FALSE( inString );
	SU_REQUIRE_OR_FALSE( inOption );

	pcstr s0, s1, s2;
	s0 = inString;
	s1 = Strfind( s0, inOption );
	SU_REQUIRE_OR_FALSE( s1 );
	s2 = s1 + Strlen(inOption);

	while( s1 > s0 )
	{
		s1--;
		if( Isspace(*s1) )
			continue;
		else if( *s1==';' )
			break;
		else
			return Stroption(s2,inOption,outStart,outEnd);
	}

	while( *s2 && Isspace(*s2) )
		s2++;

	if( *s2=='=' )
		s2++;

	while( *s2 && Isspace(*s2) )
		s2++;

	outStart = s2;

	while( *s2 && *s2!=';' )
		s2++;

	outEnd = s2;
	return true;
}



bool su::util::Stroption (	pcstr inString, pcstr inOption, pstr inBuffer, uint inCap )
{
	pcstr s, e;
	if( Stroption(inString,inOption,s,e) )
	{
		uint len = e-s;
		if( len < inCap )
		{
			Strcopy( inBuffer, s, len );
			inBuffer[len] = 0;
			return true;
		}
	}

	return false;
}

pcstr su::util::Strextract( pcstr inString, int inIth, char inSep )
{
	SU_REQUIRE_OR_NULL( inString );
	SU_REQUIRE_OR_NULL( *inString );
	int cpt = 0;
	while( cpt <= inIth && *inString)
	{
		while( *inString==inSep )				inString++;
		while( *inString && *inString!=inSep )	inString++;
		cpt++;
	}
	return inString;
}
	
pcstr su::util::Strjoin( pstr inBuffer, pcstr inPos, pcstr inText, char inSep )
{	
	if( inBuffer != inPos ) // not beginning : add separator
	{
		int offset = inPos - inBuffer;
		inBuffer += offset;
		char sepA[2];
		sepA[0] = inSep;
		sepA[1] = 0;
		::strcat( inBuffer, sepA );
	}
	else
		inBuffer[0] = 0;

	::strcat( inBuffer, inText );

	return inBuffer + ::strlen( inText );
}