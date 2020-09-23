

#ifndef _SU_UTIL_UUCODE_H_
#define _SU_UTIL_UUCODE_H_


#ifndef _su_types_h_
typedef	const char *	pcstr;
typedef char *			pstr;
typedef unsigned int	uint;
typedef unsigned long	ulong;
#endif




namespace
{


	// blob uu encoding
	// according to RFC?


	const char uuenc_charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	const int uudec_charset[] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1, -1,		// '=' is -2
		-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
		-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1	};




	uint _su_uuencodelen ( void* inbuf, uint insize )
	{
		if(!inbuf) return 0;
		return ((insize+2)/3)*4+4;
	}


	uint _su_uuencode ( char* outbuf, void* inbuf, uint insize )
	{
		if(!outbuf) return 0;
		if(!inbuf) return 0;
		if( sizeof(uuenc_charset) != 64+1 )return 0;
		unsigned char* inb   = (unsigned char*)inbuf;
		char* outb = outbuf;
		while( insize )
		{
			uint b0, b1, b2;
			uint u0, u1, u2, u3;
			b0 = inb[0];
			b1 = insize>1 ? inb[1] : 0;
			b2 = insize>2 ? inb[2] : 0;
			u0 =			uuenc_charset[ (b0>>2)&0x3f ];
			u1 =			uuenc_charset[ ((b0<<4)&0x3f)|((b1>>4)&0x3f) ];
			u2 = insize>1 ? uuenc_charset[ ((b1<<2)&0x3f)|((b2>>6)&0x3f) ] : '=';
			u3 = insize>2 ? uuenc_charset[ (b2&0x3f) ] : '=';
			outb[0] = u0;
			outb[1] = u1;
			outb[2] = u2;
			outb[3] = u3;
			outb   += 4;
			inb    += 3;
			insize -= (insize<3U) ? insize : 3U;
		}
		outb[0] = '=';
		outb[1] = '=';
		outb[2] = '=';
		outb[3] = '=';
		outb += 4;
		return (outb-outbuf);
	}


	bool _su_uustream ( pcstr ins0, pcstr ins1 )
	{
		if(!ins0) return false;
		if(!ins1) return false;
		if(ins1<=ins0) return false;
		int n = ins1-ins0;
		if(n%4) return false;
		if( *(ins1-1)!='=' ) return false;
		if( *(ins1-2)!='=' ) return false;
		if( *(ins1-3)!='=' ) return false;
		if( *(ins1-4)!='=' ) return false;
		while( ins0<ins1 )
			if( uudec_charset[*ins0++] == -1 )
				return false;
		return true;
	}


	uint _su_uudecodelen ( pcstr ins0, pcstr ins1 )
	{
		if(!ins0) return 0;
		if(!ins1) return 0;
		if(ins1<=ins0) return 0;
		int n = ins1-ins0;
		if(n%4) return 0;
		if(n<8) return 0;
		int len = ((n/4)-1)*3;
		if( *(ins1-1)!='=' ) return 0;
		if( *(ins1-2)!='=' ) return 0;
		if( *(ins1-3)!='=' ) return 0;
		if( *(ins1-4)!='=' ) return 0;
		if( *(ins1-5)=='=' ) len -= 1;
		if( *(ins1-6)=='=' ) len -= 1;
		return len;
	}


	uint _su_uudecode ( void* outbuf, pcstr ins0, pcstr ins1 )
	{
		if(!outbuf) return 0;
		if(!ins0) return 0;
		if(!ins1) return 0;
		if(ins1<=ins0) return 0;
		int n = ins1-ins0;
		if(n%4) return 0;
		if(n<8) return 0;
		unsigned char* outb = (unsigned char*)outbuf;
		while( ins0<(ins1-4) )
		{
			uint b0, b1, b2;
			uint u0, u1, u2, u3;
			if( ins0[0] >= 128 ) return 0;
			if( ins0[1] >= 128 ) return 0;
			if( ins0[2] >= 128 ) return 0;
			if( ins0[3] >= 128 ) return 0;
			u0 = uudec_charset[ ins0[0] ];
			u1 = uudec_charset[ ins0[1] ];
			u2 = uudec_charset[ ins0[2] ];
			u3 = uudec_charset[ ins0[3] ];
			if( u0 == ~0U ) return 0;
			if( u1 == ~0U ) return 0;
			if( u2 == ~0U ) return 0;
			if( u3 == ~0U ) return 0;
			b0 = (u0<<2) | (u1>>4);
			b1 = (u1<<4) | (u2>>2);
			b2 = (u2<<6) | (u3>>0);
			*outb++ = b0;	if( ins0[2]=='=' )	break;
			*outb++ = b1;	if( ins0[3]=='=' )	break;
			*outb++ = b2;
			ins0 += 4;
		}
		return outb - ((unsigned char*)outbuf);
	}




	// [integer|float] sequence encoding
	// basic ideas are :
	// - better precision on float storage can be achieved with an hex dump of the IEEE754 float storage (starting with '-') ;
	// - shorter signed integer storage form is obtained with a hex dump (starting with '-') ;
	// - most significant nibbles of a signed-integer hex representation can be signed extended (ffffffff <-> f, 000000f3 <-> 0f3) ;
	// - less significant nibbles of a float hex representation can be 0 extended (3f800000 <-> 3f8, 34d54 <-> 34d54000) ;


	enum UUSeqStyle
	{
		UUSEQ_DECIMAL = 0,
		UUSEQ_DUMPED,
		UUSEQ_MIXED,
	};


	const char hexenc_charset[] = "0123456789abcdef";

	const int hexdec_charset[] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
		-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1	};

	const int uuliteral_charset[] = {
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  1,  1,  0,		// + - .
		 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,		// 0-9
		 0,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,		// A-F
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,		// _
		 0,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,		// a-f
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0	};



	bool _su_favor_dumpedi ( int v )
	{
		if( v>65535 )	return true;
		if( v<-65535 )	return true;
		return false;
	}


	bool _su_favor_dumpedf ( float v )
	{
		union { float f; uint u; };
		f = v;
		int s,e;
		s = (u>>31);
		e = ((u>>23)&0xff)-127;
		if( e>13 || e<13 )	return true;
		uint p = u<<(9+e+10);
		return (p!=0);
	}


	char* _su_to_literali ( UUSeqStyle style, char* p, int v )
	{
		if( style==UUSEQ_DUMPED || (style==UUSEQ_MIXED && _su_favor_dumpedi(v)) )
		{
			union { signed int i; uint u; };
			i = v;
			int l=32;
			while( (l>4) && ((u>>27)==0 || (u>>27)==0x1f) ) {
				l -= 4;
				u <<= 4;
			}
			*p++ = '_';
			while( l ) {
				*p++ = hexenc_charset[u>>28];
				u <<= 4;
				l -= 4;
			}
		}
		else
		{
			char tmp[32];
			int n = ::sprintf(tmp,"%ld",long(v));
			for( int i=0 ; i<n ; i++ )	*p++ = tmp[i];
		}
		return p;
	}

	
	int _su_from_literali ( pcstr l0, pcstr l1 )
	{
		if( l0[0]=='_' )
		{
			l0++;
			int l = 0;
			signed int i=0;
			while( (l0<l1) && (l<32) && uuliteral_charset[*l0]==1 )
			{
				i |= ((uint)hexdec_charset[*l0]) << (28-l);
				l0++;
				l+=4;
			}
			i >>= 32-l;
			return i;
		}
		else
		{
			char* endp;
			return strtol( l0, &endp, 10 );
		}
	}


	char* _su_to_literalf ( UUSeqStyle style, char* p, float v )
	{
		if( style==UUSEQ_DUMPED || (style==UUSEQ_MIXED && _su_favor_dumpedf(v)) )
		{
			union { float f; uint u; };
			f = v;
			*p++ = '_';
			for( int i=0 ; (u)||(i==0) ; i++, u<<=4 )
				*p++ = hexenc_charset[u>>28];
		}
		else
		{
			char tmp[32];
			int n = ::sprintf(tmp,"%f",v);
			bool asperiod = false;
			for( int i=0 ; i<n ; i++ )
			{
				if( tmp[i]=='.' ) asperiod = true;
				*p++ = tmp[i];
			}
			while( asperiod && *(p-1)=='0' )	p--;
			if( asperiod && *(p-1)=='.' )		p--;
		}
		return p;
	}


	float _su_from_literalf ( pcstr l0, pcstr l1 )
	{
		if( l0[0]=='_' )
		{
			union { uint u; float f; };
			l0++;
			u = 0;
			int l = 0;
			while( (l0<l1) && (l<32) && uuliteral_charset[*l0]==1 )
			{
				u |= ((uint)hexdec_charset[*l0]) << (28-l);
				l0++;
				l+=4;
			}
			return f;
		}
		else
		{
			char* endp;
			return ::strtod( l0, &endp );
		}
	}


	int _su_count_literali ( UUSeqStyle style, int v )
	{
		char tmp[16];
		return _su_to_literali(style,tmp,v) - tmp;
	}


	int _su_count_literalf ( UUSeqStyle style, float v )
	{
		char tmp[16];
		return _su_to_literalf(style,tmp,v) - tmp;
	}


	struct _su_seqtok
	{
		pcstr t0, t1, e;

		_su_seqtok( pcstr s0, pcstr s1 )
		{
			t0 = t1 = s0;
			e = s1;
		}

		bool next ( )
		{
			while( (t1<e) && uuliteral_charset[*t1]==0 )	t1++;
			t0 = t1;
			while( (t1<e) && uuliteral_charset[*t1]==1 )	t1++;
			return (t1>t0);
		}
	};



	uint _su_uuencodeseqlen ( UUSeqStyle style, int* array, int size )
	{
		if(!array )	return 0;
		if(!size)	return 0;

		int l = 0;
		for( int i=0 ; i<size ; i++ )
		{
			if( i>0 )	l += 1;
			l += _su_count_literali(style,array[i]);
		}
		return l;
	}


	uint _su_uuencodeseqlen ( UUSeqStyle style, float* array, int size )
	{
		if(!array )	return 0;
		if(!size)	return 0;

		int l = 0;
		for( int i=0 ; i<size ; i++ )
		{
			if( i>0 )	l += 1;
			l += _su_count_literalf(style,array[i]);
		}
		return l;
	}


	uint _su_uuencodeseq ( char* outbuf, UUSeqStyle style, int* array, int size )
	{
		if(!outbuf) return 0;
		if(!array )	return 0;
		if(!size)	return 0;

		char* p = outbuf;
		for( int i=0 ; i<size ; i++ )
		{
			if( i>0 )	*p++ = ' ';
			p = _su_to_literali(style,p,array[i]);
		}
		return (p-outbuf);
	}


	uint _su_uuencodeseq ( char* outbuf, UUSeqStyle style, float* array, int size )
	{
		if(!outbuf) return 0;
		if(!array )	return 0;
		if(!size)	return 0;

		char* p = outbuf;
		for( int i=0 ; i<size ; i++ )
		{
			if( i>0 )	*p++ = ' ';
			p = _su_to_literalf(style,p,array[i]);
		}
		return (p-outbuf);
	}


	uint _su_uudecodeseqlen ( pcstr ins0, pcstr ins1 )
	{
		if(!ins0)		return 0;
		if(!ins1)		return 0;
		if(ins1<ins0)	return 0;

		_su_seqtok stok( ins0, ins1 );

		int n = 0;
		while( stok.next() ) n++;
		return n;
	}


	uint _su_uudecodeseq ( int* array, pcstr ins0, pcstr ins1 )
	{
		if(!ins0)		return 0;
		if(!ins1)		return 0;
		if(ins1<ins0)	return 0;

		_su_seqtok stok( ins0, ins1 );

		int n = 0;
		while( stok.next() )
		{
			array[n] = _su_from_literali(stok.t0,stok.t1);
			n++;
		}
		return n;
	}


	uint _su_uudecodeseq ( float* array, pcstr ins0, pcstr ins1 )
	{
		if(!ins0)		return 0;
		if(!ins1)		return 0;
		if(ins1<ins0)	return 0;

		_su_seqtok stok( ins0, ins1 );

		int n = 0;
		while( stok.next() )
		{
			array[n] = _su_from_literalf(stok.t0,stok.t1);
			n++;
		}
		return n;
	}


}


#endif // _SU_UTIL_UUCODE_H_



