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


#include "su_langc.h"




#ifdef __cplusplus
extern "C" 
{
#endif



int su_langc_isspace ( int c )
{
	return (c==32) || (c>=9 && c<=13) ? 1 : 0;
}



int su_langc_isdigit ( int c )
{
	return (c>='0' && c<='9') ? 1 : 0;
}




static int maxExponent = 38;	/* Largest possible base 10 exponent.  Any
								* exponent larger than this will already
								* produce underflow or overflow, so there's
								* no need to worry about additional digits.
								*/
static float powersOf10[] = {	/* Table giving binary powers of 10.  Entry */
	10.f,					/* is 10^2^i.  Used to convert decimal */
	100.f,					/* exponents into floating-point numbers. */
	1.0e4f,
	1.0e8f,
	1.0e16f,
	1.0e32f
};



float su_langc_strtofloat ( const char* string, char** endp	)
{
	int sign, expSign = 0;
	float fraction, dblExp, *d;
	register const char* p;
	register int c;
	int exp = 0;			/* Exponent read from "EX" field. */
	int fracExp = 0;		/* Exponent that derives from the fractional
							* part.  Under normal circumstatnces, it is
							* the negative of the number of digits in F.
							* However, if I is very long, the last digits
							* of I get dropped (otherwise a long I with a
							* large negative exponent could cause an
							* unnecessary overflow on I alone).  In this
							* case, fracExp is incremented one for each
							* dropped digit. */
	int mantSize;			/* Number of digits in mantissa. */
	int decPt;				/* Number of mantissa digits BEFORE decimal
							* point. */
	const char* pExp;		/* Temporarily holds location of exponent
							* in string. */

	/*
	* Strip off leading blanks and check for a sign.
	*/

	p = string;
	while (su_langc_isspace(*p))
		p += 1;
	if (*p == '-') {
		sign = 1;
		p += 1;
	} else {
		if (*p == '+')
			p += 1;
		sign = 0;
	}

	/*
	* Count the number of digits in the mantissa (including the decimal
	* point), and also locate the decimal point.
	*/

	decPt = -1;
	for (mantSize = 0; ; mantSize += 1)
	{
		c = *p;
		if (!su_langc_isdigit(c)) {
			if ((c != '.') || (decPt >= 0))
				break;
			decPt = mantSize;
		}
		p += 1;
	}

	/*
	* Now suck up the digits in the mantissa.  Use two integers to
	* collect 9 digits each (this is faster than using floating-point).
	* If the mantissa has more than 18 digits, ignore the extras, since
	* they can't affect the value anyway.
	*/

	pExp  = p;
	p -= mantSize;
	if (decPt < 0) {
		decPt = mantSize;
	} else {
		mantSize -= 1;			/* One of the digits was the point. */
	}
	if (mantSize > 18) {
		fracExp = decPt - 18;
		mantSize = 18;
	} else {
		fracExp = decPt - mantSize;
	}
	if (mantSize == 0) {
		fraction = 0.0;
		p = string;
		goto Strtof_done;
	} else {
		int frac1, frac2;
		frac1 = 0;
		for ( ; mantSize > 9; mantSize -= 1)
		{
			c = *p;
			p += 1;
			if (c == '.') {
				c = *p;
				p += 1;
			}
			frac1 = 10*frac1 + (c - '0');
		}
		frac2 = 0;
		for (; mantSize > 0; mantSize -= 1)
		{
			c = *p;
			p += 1;
			if (c == '.') {
				c = *p;
				p += 1;
			}
			frac2 = 10*frac2 + (c - '0');
		}
		fraction = (float)( (1.0e9 * frac1) + frac2 );
	}

	/*
	* Skim off the exponent.
	*/

	p = pExp;
	if ((*p == 'E') || (*p == 'e')) {
		p += 1;
		if (*p == '-') {
			expSign = 1;
			p += 1;
		} else {
			if (*p == '+') {
				p += 1;
			}
			expSign = 0;
		}
		if (!su_langc_isdigit(*p)) {
			p = pExp;
			goto Strtof_done;
		}
		while (su_langc_isdigit(*p)) {
			exp = exp * 10 + (*p - '0');
			p += 1;
		}
	}
	if (expSign) {
		exp = fracExp - exp;
	} else {
		exp = fracExp + exp;
	}

	/*
	* Generate a floating-point number that represents the exponent.
	* Do this by processing the exponent one bit at a time to combine
	* many powers of 2 of 10. Then combine the exponent with the
	* fraction.
	*/

	if (exp < 0) {
		expSign = 1;
		exp = -exp;
	} else {
		expSign = 0;
	}
	if (exp > maxExponent) {
		exp = maxExponent;
		//		errno = ERANGE;
	}
	dblExp = 1.0;
	for (d = powersOf10; exp != 0; exp >>= 1, d += 1) {
		if (exp & 01) {
			dblExp *= *d;
		}
	}
	if (expSign) {
		fraction /= dblExp;
	} else {
		fraction *= dblExp;
	}

Strtof_done:
	if (endp) {
		*endp = (char *) p;
	}

	if( sign )	return -fraction;
	else		return fraction;
}


void su_langc_strcopy ( char* d, char* s )
{
	if( !d )		return;
	if( !s )		return;
	while( *s )		*d++ = *s++;
	*d = 0;
}


char* su_langc_strdup ( char* s )
{
	char *d;
	int  l;
	if( !s )		return 0L;
	l = 0;
	while( s[l] )	l++;
	d = (char*) su_langc_malloc( l+1 );
	if( !d )		return 0L;
	su_langc_strcopy( d, s );
	return d;
}


void su_langc_memset ( void* a, int v , int n )
{
	char* pa;
	if( !a || !n )	return;
	pa = (char*) a;
	while( n-- ) *pa++ = v;
}


void su_langc_memcopy ( void* a, void* b, int n )
{
	char *pa, *pb;
	if( !a || !b || !n )	return;
	pa = (char*) a;
	pb = (char*) b;
	while( n-- )	*pa++ = *pb++;
}



#ifdef __cplusplus
}
#endif



