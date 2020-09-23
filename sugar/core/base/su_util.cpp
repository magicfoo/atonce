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


#include "../su_base.h"

SU_CC_OPEN_C
#include "su_langc.h"
SU_CC_CLOSE_C

#include <stdio.h>
#include <stdlib.h>
using namespace su;



#include "su_util_uucode.h"


void*
su::util::Memset(	void* inPtr, int inValue, uint32 inSize )
{
	if( inPtr && inSize )
	{
		uchar* p0 = (uchar*) inPtr;
		uchar* p1 = p0 + inSize;
		while( p0 != p1 )
			*p0++ = inValue;
	}
	return inPtr;
}


void*
su::util::Memcopy(	void* inDst, const void* inSrc, uint32 inSize )
{
	if( inDst && inSrc && inSize )
	{
		uchar* d0 = (uchar*) inDst;
		uchar* s0 = (uchar*) inSrc;
		uchar* s1 = s0 + inSize;
		while( s0 != s1 )
			*d0++ = *s0++;
	}
	return inDst;
}


void*
su::util::Memncopy(	void* inDst, const void* inSrc, uint32 inSize, uint inCpt	)
{
	if( inDst && inSrc && inSize && inCpt )
	{
		uchar* dst = (uchar*) inDst;
		uchar* end = dst + inCpt*inSize;
		while( dst != end )
		{
			Memcopy( dst, inSrc, inSize );
			dst += inSize;
		}
	}
	return inDst;
}


void*
su::util::Memmove(	void * inDst, const void * inSrc, uint32 inSize )
{
	/*
	* If the buffers don't overlap, it doesn't matter what direction
	* we copy in. If they do, it does, so just assume they always do.
	* We don't concern ourselves with the possibility that the region
	* to copy might roll over across the top of memory, because it's
	* not going to happen.
	*
	*/

	if( inDst && inSrc && inSize )
	{
		byte* d = (byte*) inDst;
		byte* s = (byte*) inSrc;

		if( d < s )
		{
			/*
			* If the destination is below the source, we have to copy
			* front to back.
			*
			*      dest:   dddddddd
			*      src:    ^   ssssssss
			*              |___|  ^   |
			*                     |___|
			*/
			byte* d_end = d + inSize;
			while( d < d_end )
				*d++ = *s++;
		}
		else
		{
			/*
			* If the destination is above the source, we have to copy
			* back to front to avoid overwriting the data we want to
			* copy.
			*
			*      dest:       dddddddd
			*      src:    ssssssss   ^
			*              |   ^  |___|
			*              |___|
			*/
			byte* d_end = d;
			d += inSize-1;
			s += inSize-1;
			while( d >= d_end )
				*d-- = *s--;
		}
	}

	return inDst;
}


int
su::util::Memcompare(	const void* av, const void* bv,  uint inSize		)
{
	if( av && bv && inSize )
	{
		byte* a = (byte*) av;
		byte* b = (byte*) bv;
		byte* a_end = a + inSize;
		while( a < a_end )
		{
			if( *a != *b )
				return (int)(*a-*b);
			a++; b++;
		}
	}
	return 0;
}



namespace
{

	// History buffer
	uint64 _x[5] = { 4002576854UL,3733863077UL,3750272681UL,3762340723UL,1728061494UL };

	uint32 _BRandom()
	{
		uint64 sum;
		sum = uint64(2111111111ul)*_x[3] + uint64(1492)*_x[2] + uint64(1776)*_x[1] + uint64(5115)*_x[0] + uint64(_x[4]);
		_x[3] = _x[2];
		_x[2] = _x[1];
		_x[1] = _x[0];
		_x[4] = uint32(sum >> 32);          // Carry
		_x[0] = uint32(sum & 0xFFFFFFFFU);	// Low 32 bits of sum
		return _x[0];
	}

	float _FRandom()
	{
		return float(_BRandom()) * (1.f/(65536.f*65536.f));
	}

	int _IRandom(int min, int max)
	{
		if (max <= min)
		{
			if (max == min) return min;
			else			return 0x80000000;
		}
		int r = int((max - min + 1) * _FRandom()) + min; 
		if (r > max) r = max;
		return r;
	}
	
	void _RandomInit (uint32 *	inState) /* [5] */
	{
		for (int i = 0; i < 5; i++)
		{
			_x[i] = inState[i];
		}
	}
/*
	void _RandomInit (uint32 seed) {
		int i;
		uint32 s = seed;
		for (i = 0; i < 5; i++) {
			s = s * 29943829 - 1;
			_x[i] = s;
		}
		for (i=0; i<19; i++) _BRandom();
	}
*/
}


bool
su::util::RandRestore( pcstr inRandState )	/* 256 char */
{
	if ( !inRandState || inRandState[0] == '\0' )
		return false;

	for (int i = 0; i < 5; i++) {
		_x[i] = 0;
	}

	pcstr	ptr		= inRandState;
	int		i		= 0;

	while ( i < 5 ) {
		if (*ptr == '\0') {
			_x[i] = i;
		}
		else {
			pstr end;
			_x[i] = Strtoulong(ptr,&end,10);
			ptr = end;
		}
		i++;
	}
	return true;
}


bool
su::util::RandSave( pstr outRandStatea )	/* 256 char */
{
	if (!outRandStatea) return false;
	Strformat(outRandStatea,"%u %u %u %u %u",_x[0],_x[1],_x[2],_x[3],_x[4],_x[5]);
	return true;
}


uint32
su::util::Rand	(	uint32 inM0, uint inM1	)
{
	return _IRandom( inM0, inM1 );
}


float
su::util::Randf	(					)
{
	return _FRandom();
}


float
su::util::Randsf	(				)
{
	return (_FRandom() * 2.0f) -1.0f;
}





void* su::util::Zero( void * inDst, uint32 inSize )
{
	return Memset( inDst, 0, inSize );
}


void* su::util::Uninitialized ( void* inDst, uint32 inSize )
{
	return Memset( inDst, 0xCC, inSize );
}


bool su::util::CheckUninitialized ( void* inDst, uint32 inSize )
{
	if( inDst && inSize )
	{
		unsigned char *a, *b;
		a = (unsigned char*) inDst;
		b = a + inSize;
		while( a<b )
		{
			if( *a != 0xCC )
				return false;
			a++;
		}
	}
	return true;
}






uint su::util::uuencodelen ( void* inbuf, uint insize )
{
	SU_REQUIRE_OR_0( inbuf );
	return _su_uuencodelen( inbuf, insize );
}


uint su::util::uuencode ( char* outbuf, void* inbuf, uint insize )
{
	SU_REQUIRE_OR_0( outbuf );
	SU_REQUIRE_OR_0( inbuf );
	return _su_uuencode( outbuf, inbuf, insize );
}


bool su::util::uustream ( pcstr ins0, pcstr ins1 )
{
	SU_REQUIRE_OR_0( ins0 );
	SU_REQUIRE_OR_0( ins1 );
	return _su_uustream( ins0, ins1 );
}


uint su::util::uudecodelen ( pcstr ins0, pcstr ins1 )
{
	SU_REQUIRE_OR_0( ins0 );
	SU_REQUIRE_OR_0( ins1 );
	return _su_uudecodelen( ins0, ins1 );
}


uint su::util::uudecode ( void* outbuf, pcstr ins0, pcstr ins1 )
{
	SU_REQUIRE_OR_0( outbuf );
	SU_REQUIRE_OR_0( ins0 );
	SU_REQUIRE_OR_0( ins1 );
	return _su_uudecode( outbuf, ins0, ins1 );
}


uint su::util::uuencodeseqlen ( UUseqstyle style, int* array, int size )
{
	return _su_uuencodeseqlen( (UUSeqStyle)style, array, size );
}


uint su::util::uuencodeseqlen ( UUseqstyle style, float* array, int size )
{
	return _su_uuencodeseqlen( (UUSeqStyle)style, array, size );
}


uint su::util::uuencodeseq ( char* outbuf, UUseqstyle style, int* array, int size )
{
	return _su_uuencodeseq( outbuf, (UUSeqStyle)style, array, size );
}


uint su::util::uuencodeseq ( char* outbuf, UUseqstyle style, float* array, int size )
{
	return _su_uuencodeseq( outbuf, (UUSeqStyle)style, array, size );
}


uint su::util::uudecodeseqlen ( pcstr ins0, pcstr ins1 )
{
	return _su_uudecodeseqlen( ins0, ins1 );
}


uint su::util::uudecodeseq ( int* array, pcstr ins0, pcstr ins1 )
{
	return _su_uudecodeseq( array, ins0, ins1 );
}


uint su::util::uudecodeseq ( float* array, pcstr ins0, pcstr ins1 )
{
	return _su_uudecodeseq( array, ins0, ins1 );
}



