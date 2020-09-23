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
using namespace su;


// If you want variable wiping, #define SHA1_WIPE_VARIABLES, if not,
// #define SHA1_NO_WIPE_VARIABLES. If you don't define anything, it
// defaults to wiping.
#define SHA1_WIPE_VARIABLES


#ifdef SU_HW_LSB
#define SHA1_LITTLE_ENDIAN
#endif


// Rotate x bits to the left
#define ROL32(_val32,_nBits) (((_val32)<<(_nBits))|((_val32)>>(32-(_nBits))))


#ifdef SHA1_LITTLE_ENDIAN
#define SHABLK0(i) (sha1_workspace.l[i] = \
	(ROL32(sha1_workspace.l[i],24) & 0xFF00FF00) | (ROL32(sha1_workspace.l[i],8) & 0x00FF00FF))
#else
#define SHABLK0(i) (sha1_workspace.l[i])
#endif


#define SHABLK(i) (sha1_workspace.l[i&15] = ROL32(sha1_workspace.l[(i+13)&15] ^ sha1_workspace.l[(i+8)&15] \
	^ sha1_workspace.l[(i+2)&15] ^ sha1_workspace.l[i&15],1))

// SHA-1 rounds
#define _R0(v,w,x,y,z,i) {z+=((w&(x^y))^y)+SHABLK0(i)+0x5A827999+ROL32(v,5);w=ROL32(w,30);}
#define _R1(v,w,x,y,z,i) {z+=((w&(x^y))^y)+SHABLK(i)+0x5A827999+ROL32(v,5);w=ROL32(w,30);}
#define _R2(v,w,x,y,z,i) {z+=(w^x^y)+SHABLK(i)+0x6ED9EBA1+ROL32(v,5);w=ROL32(w,30);}
#define _R3(v,w,x,y,z,i) {z+=(((w|x)&y)|(w&x))+SHABLK(i)+0x8F1BBCDC+ROL32(v,5);w=ROL32(w,30);}
#define _R4(v,w,x,y,z,i) {z+=(w^x^y)+SHABLK(i)+0xCA62C1D6+ROL32(v,5);w=ROL32(w,30);}


union SHA1_WORKSPACE_BLOCK
{
	uint8  c[64];
	uint32 l[16];
};



namespace
{

	bool					sha1_session_began = false;
	uint32					sha1_state[5];
	uint32					sha1_count[2];
	uint8					sha1_buffer[64];
	uint8					sha1_digest[20];
	SHA1_WORKSPACE_BLOCK	sha1_workspace;


	void sha1_reset ( )
	{
		// SHA1 initialization constants
		sha1_state[0] = 0x67452301;
		sha1_state[1] = 0xEFCDAB89;
		sha1_state[2] = 0x98BADCFE;
		sha1_state[3] = 0x10325476;
		sha1_state[4] = 0xC3D2E1F0;

		sha1_count[0] = 0;
		sha1_count[1] = 0;
	}


	void sha1_transform ( uint8* pBuffer )
	{
		uint32 a = sha1_state[0], b = sha1_state[1], c = sha1_state[2], d = sha1_state[3], e = sha1_state[4];

		memcpy( sha1_workspace.c, pBuffer, 64 );

		// 4 rounds of 20 operations each. Loop unrolled.
		_R0(a,b,c,d,e, 0); _R0(e,a,b,c,d, 1); _R0(d,e,a,b,c, 2); _R0(c,d,e,a,b, 3);
		_R0(b,c,d,e,a, 4); _R0(a,b,c,d,e, 5); _R0(e,a,b,c,d, 6); _R0(d,e,a,b,c, 7);
		_R0(c,d,e,a,b, 8); _R0(b,c,d,e,a, 9); _R0(a,b,c,d,e,10); _R0(e,a,b,c,d,11);
		_R0(d,e,a,b,c,12); _R0(c,d,e,a,b,13); _R0(b,c,d,e,a,14); _R0(a,b,c,d,e,15);
		_R1(e,a,b,c,d,16); _R1(d,e,a,b,c,17); _R1(c,d,e,a,b,18); _R1(b,c,d,e,a,19);
		_R2(a,b,c,d,e,20); _R2(e,a,b,c,d,21); _R2(d,e,a,b,c,22); _R2(c,d,e,a,b,23);
		_R2(b,c,d,e,a,24); _R2(a,b,c,d,e,25); _R2(e,a,b,c,d,26); _R2(d,e,a,b,c,27);
		_R2(c,d,e,a,b,28); _R2(b,c,d,e,a,29); _R2(a,b,c,d,e,30); _R2(e,a,b,c,d,31);
		_R2(d,e,a,b,c,32); _R2(c,d,e,a,b,33); _R2(b,c,d,e,a,34); _R2(a,b,c,d,e,35);
		_R2(e,a,b,c,d,36); _R2(d,e,a,b,c,37); _R2(c,d,e,a,b,38); _R2(b,c,d,e,a,39);
		_R3(a,b,c,d,e,40); _R3(e,a,b,c,d,41); _R3(d,e,a,b,c,42); _R3(c,d,e,a,b,43);
		_R3(b,c,d,e,a,44); _R3(a,b,c,d,e,45); _R3(e,a,b,c,d,46); _R3(d,e,a,b,c,47);
		_R3(c,d,e,a,b,48); _R3(b,c,d,e,a,49); _R3(a,b,c,d,e,50); _R3(e,a,b,c,d,51);
		_R3(d,e,a,b,c,52); _R3(c,d,e,a,b,53); _R3(b,c,d,e,a,54); _R3(a,b,c,d,e,55);
		_R3(e,a,b,c,d,56); _R3(d,e,a,b,c,57); _R3(c,d,e,a,b,58); _R3(b,c,d,e,a,59);
		_R4(a,b,c,d,e,60); _R4(e,a,b,c,d,61); _R4(d,e,a,b,c,62); _R4(c,d,e,a,b,63);
		_R4(b,c,d,e,a,64); _R4(a,b,c,d,e,65); _R4(e,a,b,c,d,66); _R4(d,e,a,b,c,67);
		_R4(c,d,e,a,b,68); _R4(b,c,d,e,a,69); _R4(a,b,c,d,e,70); _R4(e,a,b,c,d,71);
		_R4(d,e,a,b,c,72); _R4(c,d,e,a,b,73); _R4(b,c,d,e,a,74); _R4(a,b,c,d,e,75);
		_R4(e,a,b,c,d,76); _R4(d,e,a,b,c,77); _R4(c,d,e,a,b,78); _R4(b,c,d,e,a,79);

		// Add the working vars back into state
		sha1_state[0] += a;
		sha1_state[1] += b;
		sha1_state[2] += c;
		sha1_state[3] += d;
		sha1_state[4] += e;

		// Wipe variables
		#ifdef SHA1_WIPE_VARIABLES
		a = b = c = d = e = 0;
		#endif
	}



	void sha1_update ( uint8* pbData, uint uLen )
	{
		uint32 j = ((sha1_count[0] >> 3) & 0x3F);

		if((sha1_count[0] += (uLen << 3)) < (uLen << 3))
			++sha1_count[1]; // Overflow

		sha1_count[1] += (uLen >> 29);

		uint32 i;
		if((j + uLen) > 63)
		{
			i = 64 - j;
			memcpy( sha1_buffer+j, pbData, i );
			sha1_transform( sha1_buffer );

			for( ; (i + 63) < uLen; i += 64)
				sha1_transform( pbData+i );

			j = 0;
		}
		else
		{
			i = 0;
		}

		if((uLen - i) != 0)
			memcpy( sha1_buffer+j, pbData+i, uLen-i );
	}


	void sha1_final ( )
	{
		uint32 i;

		uint8 finalcount[8];
		for(i = 0; i < 8; ++i)
			finalcount[i] = (uint8)((sha1_count[((i >= 4) ? 0 : 1)]
				>> ((3 - (i & 3)) * 8) ) & 255); // Endian independent

		sha1_update( (uint8*)"\200", 1 );

		while ((sha1_count[0] & 504) != 448)
			sha1_update( (uint8*)"\0", 1 );

		sha1_update( finalcount, 8 ); // Cause a SHA1Transform()

		for(i = 0; i < 20; ++i)
			sha1_digest[i] = (uint8)((sha1_state[i >> 2] >> ((3 - (i & 3)) * 8)) & 0xFF);

		// Wipe variables for security reasons
	#ifdef SHA1_WIPE_VARIABLES
		memset( sha1_buffer, 0, 64 );
		memset( sha1_state, 0, 20 );
		memset( sha1_count, 0, 8 );
		memset( finalcount, 0, 8 );
		sha1_transform( sha1_buffer );
	#endif
	}

}





void util::hmac_sha1_begin ( )
{
	sha1_reset();
	sha1_session_began = true;
}


void util::hmac_sha1_append ( pointer inSegment, uint inBSize )
{
	SU_ASSERT( sha1_session_began );
	SU_REQUIRE_OR_RETURN( sha1_session_began );
	SU_REQUIRE_OR_RETURN( inSegment );
	SU_REQUIRE_OR_RETURN( inBSize  );
	sha1_update( (uint8*)inSegment, inBSize );
}


byte* util::hmac_sha1_end ( )
{
	SU_REQUIRE_OR_NULL( sha1_session_began );
	sha1_final();
	sha1_session_began = false;
	return (byte*)sha1_digest;
}


byte* util::hmac_sha1 ( pointer inSegment, uint inBSize )
{
	hmac_sha1_begin();
	hmac_sha1_append( inSegment, inBSize );
	return hmac_sha1_end();
}




