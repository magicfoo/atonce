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



#ifndef _su_utili_h_
#define _su_utili_h_



namespace su { namespace util
{


	template<typename T> inline
	bool Stroption ( pcstr inString, pcstr inOption, T& inBuffer )
	{
		return Stroption( inString, inOption, inBuffer, sizeof(T) );
	}


	template < typename T > inline
	T* Objectctor ( T* inDst, uint inCount )
	{
		SU_REQUIRE_OR_NULL( inDst );
		SU_REQUIRE_OR_NULL( inCount );
		const bool hascdtor = support_yes_or_undefined< has_user_cdtor<T> >::value;
		if( hascdtor )
		{
			T* t     = inDst;
			T* t_end = t + inCount;
			while( t < t_end )
			{
				SU_CC_CALL_CTOR( t++ );
			}
		}
		return inDst;
	}

	template < typename T > inline
	T* Objectdtor ( T* inDst, uint inCount )
	{
		SU_REQUIRE_OR_NULL( inDst );
		SU_REQUIRE_OR_NULL( inCount );
		const bool hascdtor = support_yes_or_undefined< has_user_cdtor<T> >::value;
		if( hascdtor )
		{
			T* t     = inDst;
			T* t_end = t + inCount;
			while( t < t_end )
			{
				SU_CC_CALL_DTOR( t++ );
			}
		}
		return inDst;
	}

	template < typename T > inline
	T* Objectcopy ( T* inDst, T* inSrc, uint inCount )
	{
		SU_REQUIRE_OR_NULL( inDst );
		SU_REQUIRE_OR_NULL( inSrc );
		SU_REQUIRE_OR_NULL( inCount );
		const bool hascdtor = support_yes_or_undefined< has_user_cdtor<T> >::value;
		if( hascdtor )
		{
			T* s     = inSrc;
			T* d     = inDst;
			T* d_end = d + inCount;
			while( d < d_end )
				*d++ = *s++;
		}
		else
		{
			Memcopy( inDst, inSrc, inCount*sizeof(T) );
		}
		return inDst;
	}

	template < typename T > inline
	T* Objectncopy ( T* inDst, T* inSrc, uint inCount )
	{
		SU_REQUIRE_OR_NULL( inDst );
		SU_REQUIRE_OR_NULL( inSrc );
		SU_REQUIRE_OR_NULL( inCount );
		const bool hascdtor = support_yes_or_undefined< has_user_cdtor<T> >::value;
		if( hascdtor )
		{
			T* s     = inSrc;
			T* d     = inDst;
			T* d_end = d + inCount;
			while( d < d_end )
				*d++ = *s;
		}
		else
		{
			Memncopy( inDst, inSrc, sizeof(T), inCount );
		}
		return inDst;
	}

	template < typename T > inline
	T* Objectmove ( T* inDst, T* inSrc, uint inCount )
	{
		SU_REQUIRE_OR_NULL( inDst );
		SU_REQUIRE_OR_NULL( inSrc );
		SU_REQUIRE_OR_NULL( inCount );
		const bool hascdtor = support_yes_or_undefined< has_user_cdtor<T> >::value;
		if( hascdtor )
		{
			if( inDst < inSrc )
			{
				// If the destination is below the source,
				// we have to copy front to back.
				//
				//     dest:   dddddddd
				//     src:    ^   ssssssss
				//             |___|  ^   |
				//                    |___|
				T* s  = inSrc;
				T* d  = inDst;
				T* dd = d + inCount;
				while( d < dd )
					*d++ = *s++;
			}
			else
			{
				// If the destination is above the source, we have to copy
				// back to front to avoid overwriting the data we want to
				// copy.
				//
				//      dest:       dddddddd
				//      src:    ssssssss   ^
				//              |   ^  |___|
				//              |___|
				T* s  = inSrc + inCount;
				T* d  = inDst + inCount;
				T* dd = inDst;
				while( d > dd )
					*--d = *--s;
			}
		}
		else
		{
			Memmove( inDst, inSrc, inCount*sizeof(T) );
		}
		return inDst;
	}

	template < typename T > inline
	T* Objectnew ( Pool* inPool, uint inCount )
	{
		SU_REQUIRE_OR_NULL( inPool );
		SU_REQUIRE_OR_NULL( inCount );

		T* o = (T*) inPool->Alloc( sizeof(T)*inCount );
		if( o )
			Objectctor( o, inCount );
		return o;
	}

	template < typename T > inline
	void Objectdelete ( Pool* inPool, T* inO, uint inCount )
	{
		SU_REQUIRE_OR_RETURN( inPool );
		SU_REQUIRE_OR_RETURN( inO );
		SU_REQUIRE_OR_RETURN( inCount );

		Objectdtor( inO, inCount );
		inPool->Free( inO );
	}



	template <typename tC> inline
	void Swap( tC & ioC1, tC & ioC2 )
	{
		tC tmp = ioC1;
		ioC1 = ioC2;
		ioC2 = tmp;
	}

	template <typename tC> inline
	tC InvertByteOrder( tC inC )
	{
		union	{
			uchar	c[ sizeof(tC) ];
			tC		t;
		};
		t = inC;
		uchar* lo = c;
		uchar* hi = c + sizeof(tC) - 1;
		while( lo < hi )
			Swap( *lo++, *hi-- );
		return t;
	}

	template <typename tC> inline
	tC LSBToNative( tC inC )
	{
	#ifdef SU_HW_LSB
		return inC;
	#else
		return InvertByteOrder( inC );
	#endif
	}

	template <typename tC> inline
	tC MSBToNative( tC inC )
	{
	#ifdef SU_HW_MSB
		return inC;
	#else
		return InvertByteOrder( inC );
	#endif
	}

	template <typename tC> inline
	tC NativeToLSB( tC inC )
	{
	#ifdef SU_HW_LSB
		return inC;
	#else
		return InvertByteOrder( inC );
	#endif
	}

	template <typename tC> inline
	tC NativeToMSB( tC inC )
	{
	#ifdef SU_HW_MSB
		return inC;
	#else
		return InvertByteOrder( inC );
	#endif
	}

	template < typename tC > inline
	tC	ConvertLSB	(	tC&		inC		)
	{
		return ( inC = LSBToNative(inC) );
	}

	template < typename tC > inline
	tC	ConvertMSB	(	tC&		inC		)
	{
		return ( inC = MSBToNative(inC) );
	}

	template <typename T> inline
	T SelectLSB ( T ina, T inb )
	{
	#ifdef SU_HW_LSB
		return ina;
	#else
		return inb;
	#endif
	}

	template <typename T> inline
	T SelectMSB ( T ina, T inb )
	{
	#ifdef SU_HW_MSB
		return ina;
	#else
		return inb;
	#endif
	}

	template < typename T > inline
	T RoundX( T inV, uint inA )
	{
		return (inA ? ((T)((uint32(inV)+(inA-1))&(~(inA-1)))) : inV );
	}

	template <typename T> inline T	Round2( T inV )				{ return RoundX( inV, 2U  );  }
	template <typename T> inline T	Round4( T inV )				{ return RoundX( inV, 4U  );  }
	template <typename T> inline T	Round8( T inV )				{ return RoundX( inV, 8U  );  }
	template <typename T> inline T	Round16( T inV )			{ return RoundX( inV, 16U );  }
	template <typename T> inline T	Round32( T inV )			{ return RoundX( inV, 32U );  }
	template <typename T> inline T	Round64( T inV )			{ return RoundX( inV, 64U );  }
	template <typename T> inline T	Round128( T inV )			{ return RoundX( inV, 128U ); }
	template <typename T> inline T	Round256( T inV )			{ return RoundX( inV, 256U ); }
	template <typename T> inline T	Round512( T inV )			{ return RoundX( inV, 512U ); }
	template <typename T> inline T	Round1024( T inV )			{ return RoundX( inV, 1024U); }

	inline bool IsPow2( uint32 inV )
	{
		return ((inV&(inV-1)) == 0);
	}

	inline bool IsPow2( uint64 inV )
	{
		return ((inV&(inV-1)) == 0);
	}

	inline uint32 GetLowestPowOf2( uint32 v )
	{
		return v & (v^(v-1));
	}

	inline uint64 GetLowestPowOf2( uint64 v )
	{
		return v & (v^(v-1));
	}

	inline uint32 GetGreatestPowOf2( uint32 v )
	{
		union { float asFp; int32 asI; };
		if( v==0 )		return 1;
		if( v>>31 )	{
			return 0x80000000;
		} else {
			asFp = float( int32(v) );
			return uint32(1<<((asI>>23)-127));
		}
	}

	inline uint64 GetGreatestPowOf2( uint64 v )
	{
		uint32 lo = uint32( v & 0xFFFFFFFF );
		uint32 hi = uint32( v >> 32 );
		if( hi )
			return uint64(GetGreatestPowOf2(hi)) << 32;
		else
			return uint64(GetGreatestPowOf2(lo));
	}

	inline uint GetLowestBitNo( uint32  v )
	{
		union { float asFp; int32 asI; };
		if( v == 0x80000000 ) {
			return 31;
		} else {
			asFp = float( (int32)GetLowestPowOf2(v) );
			return uint32((asI>>23)-127);
		}
	}

	inline uint GetLowestBitNo( uint64  v )
	{
		uint32 lo = uint32( v & 0xFFFFFFFF );
		uint32 hi = uint32( v >> 32 );
		if( lo )
			return GetLowestBitNo( lo );
		else
			return GetLowestBitNo( hi ) + 32;
	}

	inline uint GetGreatestBitNo( uint32  v )
	{
		union { float asFp; int32 asI; };
		if( v>>31 ) {
			return 31;
		} else {
			asFp = float( int32(v) );
			return uint32((asI>>23)-127);
		}
	}

	inline uint GetFloorPow2( uint32 inV )
	{
		return GetGreatestBitNo( inV );
	}

	inline uint GetCeilPow2( uint32 inV )
	{
		uint bhi = GetFloorPow2( inV );
		if( (1U<<bhi) < inV )
			bhi++;
		return bhi;
	}

	inline uint GetGreatestBitNo( uint64  v )
	{
		uint32 lo = uint32( v & 0xFFFFFFFF );
		uint32 hi = uint32( v >> 32 );
		if( hi )
			return GetGreatestBitNo( hi ) + 32;
		else
			return GetGreatestBitNo( lo );
	}


	inline uint GetBitCount( uint32 a )
	{
		// From graphics gems II
		a = (a&0x55555555) + ((a>>1) &(0x55555555));	// a: 16  2-bit tallies
		a = (a&0x33333333) + ((a>>2) &(0x33333333));	// a:  8  4-bit tallies
		a = (a&0x07070707) + ((a>>4) &(0x07070707));	// a:  4  8-bit tallies
		a = (a&0x000F000F) + ((a>>8) &(0x000F000F));	// a:  2 16-bit tallies
		a = (a&0x0000001F) + ((a>>16)&(0x0000001F));	// a:  1 32-bit tally
		return a;
	}


	inline uint GetBitCount( uint64 a )
	{
		return GetBitCount(uint32(a)) + GetBitCount(uint32(a>>32));
	}


	inline uint32 GetBitRangeMask	(	uint	inFromNo,	uint	inToNo	)
	{
		uint32 m;
		if( inFromNo == inToNo )
		{
			m = (1U << inFromNo);
		}
		else if( inToNo > inFromNo )
		{
			m = ~0U;
			m = ( m >> (inFromNo)  ) << (inFromNo);
			m = ( m << (31-inToNo) ) >> (31-inToNo);
		}
		else	// if( inFromNo > inToNo )
		{ 
			m = ~0U;
			m = ( m >> (inToNo+1)    ) << (inToNo+1);
			m = ( m << (32-inFromNo) ) >> (32-inFromNo);
			m = ~m;
		}

		return m;
	}


	inline uint	GetBitRangeCount	(	uint32	inV,	uint	inFromNo	)
	{
		if( inFromNo > 31 )
			return 0;
		uint32 im = ~( inV >> inFromNo );
		return ( im ? GetLowestBitNo(im) : 32 );
	}


	inline void cpy64to32(	uint32*	dst32/*[2]*/,
							uint64*	src64/*[1]*/	)
	{
		dst32[0] = ((uint32*)src64)[0];
		dst32[1] = ((uint32*)src64)[1];
	}

	inline void cpy64HIto32(	uint32*	dst32/*[1]*/,
								uint64*	src64/*[1]*/	)
	{
		dst32[0] = ((uint32*)src64)[1];
	}

	inline void cpy64LOto32(	uint32*	dst32/*[1]*/,
								uint64*	src64/*[1]*/	)
	{
		dst32[0] = ((uint32*)src64)[0];
	}

	inline uint Ftoui( float v )
	{
		if( v < 0.0f )			return 0;
		if( v < 4194304.0f )	return uint(int(v));
		return uint((int(v*0.5f))<<1);
	}

	inline int32 FTOI_19_4( float v )
	{
		union { float asFp; int32 asI; };
		asFp = v + 524288.0f /*float($80000)*/;
		return asI & 0x7FFFFF;
	}

	inline int32 FTOI_23_0( float v )
	{
		union { float asFp; int32 asI; };
		asFp = v + 8388608.0f /*float($800000)*/;
		return asI & 0x7FFFFF;
	}

	inline uint32 asuint32( float v )
	{
		union { float f; uint32 u; };
		f = v;
		return u;
	}

	inline uint32 asuint32( uint32 v )
	{
		return v;
	}

	inline uint32 asuint32( int32 v )
	{
		return uint32(v);
	}

	inline uint32 asuint32( bool v )
	{
		return uint32(v);
	}

	inline uint32 asuint32( void* v )
	{
		return uint32(v);
	}



	template<typename LessThanOrder> inline
	void _iqsort_rec ( short* outIndices, uint inCount, void* inArg, int a, int b )
	{
		if( a >= b)
			return ;

		int i = a+1;
		int j = b;

		int pivIdx = ( ( a + b ) / 2 );

		int tmp				= outIndices[a];
		outIndices[a]		= outIndices[pivIdx];
		outIndices[pivIdx]	= tmp;


		LessThanOrder lto;
		while( i <= j )
		{
			while( ( i <= b ) && lto( outIndices[i], outIndices[a], inArg ) )
			{
				i++;
			}

			while( ( j > a) && !lto( outIndices[j], outIndices[a], inArg ) )
			{
				j--;
			}
			if( i < j)
			{
				tmp				= outIndices[i];
				outIndices[i]	= outIndices[j];
				outIndices[j]	= tmp;
			}
		}

		tmp				= outIndices[a];
		outIndices[a]	= outIndices[j];
		outIndices[j]	= tmp;


		_iqsort_rec<LessThanOrder>( outIndices, inCount, inArg, a,   j-1 );
		_iqsort_rec<LessThanOrder>( outIndices, inCount, inArg, j+1, b	 );
	}


	template<typename LessThanOrder> inline
	void iqsort ( short* ioIndexes, uint inCount, void* inArg )
	{
		SU_REQUIRE_OR_RETURN( ioIndexes );
		SU_REQUIRE_OR_RETURN( inArg );

		for( uint i=0; i<inCount; i++ )
		{
			ioIndexes[i] = i;
		}

		_iqsort_rec<LessThanOrder>( ioIndexes, inCount, inArg, 0, inCount-1 );
	}


	template<typename LessThanOrder> inline
	short* iqsort ( uint inCount, void* inArg )
	{
		SU_REQUIRE_OR_NULL( inCount<=1024 );
		SU_REQUIRE_OR_NULL( inArg );

		static short idxA[1024];

		for( uint i=0; i<inCount; i++ )
		{
			idxA[i] = i;
		}

		_iqsort_rec<LessThanOrder>( idxA, inCount, inArg, 0, inCount-1 );
		return idxA;
	}


	// SafeBuffer

	template < typename T, int B > inline
	SafeBuffer<T,B>::SafeBuffer ( )
	{
		SU_ASSERT( (byte*)mark0 < (byte*)bytes );
		SU_ASSERT( (byte*)mark1 >= (byte*)(bytes+B) );
		mark0[0] = mark1[0] = 'safe';
		mark0[1] = mark1[1] = uint32(this);
	}

	template < typename T, int B > inline
	SafeBuffer<T,B>::~SafeBuffer ( )
	{
		SU_ASSERT( check() );
	}

	template < typename T, int B > inline
	bool SafeBuffer<T,B>::check ( )
	{
		if( mark0[0]!='safe' )			return false;
		if( mark1[0]!='safe' )			return false;
		if( mark0[1]!=uint32(this) )	return false;
		if( mark1[1]!=uint32(this) )	return false;
		return true;
	}

	template < typename T, int B > inline
	int SafeBuffer<T,B>::size ( )
	{
		return B;
	}

	template < typename T, int B > inline
	typename SafeBuffer<T,B>::pointer SafeBuffer<T,B>::addr ( )
	{
		return bytes;
	}

	template < typename T, int B > inline
	typename SafeBuffer<T,B>::pointer SafeBuffer<T,B>::first ( )
	{
		return bytes;
	}

	template < typename T, int B > inline
	typename SafeBuffer<T,B>::pointer SafeBuffer<T,B>::last ( )
	{
		return bytes+B;
	}

	template < typename T, int B > inline
	typename SafeBuffer<T,B>::pointer SafeBuffer<T,B>::end ( )
	{
		return bytes+B-1;
	}

	template < typename T, int B > inline
	void SafeBuffer<T,B>::zero ( )
	{
		Zero( bytes );
	}

	template < typename T, int B > inline
	SafeBuffer<T,B>::operator T* ( )
	{
		return (T*) bytes;
	}

	template < typename T, int B > inline
	typename SafeBuffer<T,B>::reference
	SafeBuffer<T,B>::operator [] ( int i )
	{
		SU_ASSERT( i>=0 );
		SU_ASSERT( i<B );
		return (T)bytes[i];
	}	

	//Bitmask

	template < int Bound > inline
	bitmask<Bound>::bitmask ( )
	{
		SU_COMPILE_TIME_ASSERT( len > 0 );
		clearall( );
	}

	template < int Bound > inline
	bitmask<Bound>::bitmask ( const bitmask& inref )
	{
		for( int i=0; i<len ; i++ )
			bits[i] = inref.bits[i];
	}

	template < int Bound > inline
	uint bitmask<Bound>::countset ( ) const
	{
		register int count=0;
		for( int i=0; i<bound ; i++ )
			if(test(i))count+=1;
		return count;
	}

	template < int Bound > inline
	uint bitmask<Bound>::countunset ( ) const
	{
		return bound-countset();
	}	

	template < int Bound > inline
	void bitmask<Bound>::set ( uint i )
	{
		SU_ASSERT( i<bound );
		bits[i>>5] |= (1<<(i&31));		
	}

	template < int Bound > inline
	void bitmask<Bound>::setrange ( uint i, uint j )
	{
		SU_ASSERT( j<bound );
		SU_ASSERT( i<bound );
		SU_ASSERT( j>=i );
		for( int k=i; k<=j ; k++ )	set(k);
	}

	template < int Bound > inline
	void bitmask<Bound>::setall		( )
	{
		setrange( 0, bound-1 );
	}

	template < int Bound > inline
	void bitmask<Bound>::clear		( uint i )
	{
		SU_ASSERT( i<bound );			
		bits[i>>5] &= ~(1<<(i&31));
	}

	template < int Bound > inline
	void bitmask<Bound>::clearrange	( uint i, uint j )
	{
		SU_ASSERT( i<bound );
		SU_ASSERT( j<bound );
		SU_ASSERT( j>=i );
		for( int k=i; k<=j ; k++ )	clear(k);
	}

	template < int Bound > inline
	void bitmask<Bound>::clearall ( )
	{
		clearrange( 0, bound-1 );
	}

	template < int Bound > inline
	bool bitmask<Bound>::test ( uint i ) const
	{
		SU_ASSERT( i<bound );				
		return (bits[i>>5]&(1<<(i&31)))!=0;
	}

	template < int Bound > inline
	bool bitmask<Bound>::testrange	( uint i, uint j ) const
	{
		SU_ASSERT( i<bound );
		SU_ASSERT( j<bound );
		SU_ASSERT( j>=i );
		for( int k=i; k<=j ; k++ ) 
		{
			if( test(k) )
				return true;
		}
		
		return false;
	}

	template < int Bound > inline
	bool bitmask<Bound>::testall ( ) const
	{
		return testrange( 0, bound-1 );
	}

	template < int Bound > inline
	int bitmask<Bound>::first ( ) const
	{
		for( int k=0; k<bound ; k++ )
			if( test(k) ) return k;
		return -1;
	}

	template < int Bound > inline
	int bitmask<Bound>::next ( int i ) const
	{
		SU_ASSERT( i<bound );

		for( int k=i; k<bound ; k++ )
			if( test(k) ) return k;
		return -1;
	}

	template < int Bound > inline
	bitmask<Bound> bitmask<Bound>::bfand ( const bitmask& inref ) const
	{
		SU_ASSERT( inref.len == len );
		bitmask<Bound> result;
		for(int i=0; i<bound ; i++)
		{
			if( test(i) && inref.test(i) )
				result.set(i);
		}
		return result;
	}

	template < int Bound > inline
	bitmask<Bound> bitmask<Bound>::bfor ( const bitmask& inref ) const
	{
		SU_ASSERT( inref.len == len );
		bitmask<Bound> result;
		for(int i=0; i<bound ; i++)
		{
			if( test(i) || inref.test(i) )
				result.set(i);
		}
		return result;
	}

	template < int Bound > inline
	bitmask<Bound> bitmask<Bound>::bfxor ( const bitmask& inref ) const
	{	
		SU_ASSERT( inref.len == len );
		bitmask<Bound> result;
		for(int i=0; i<bound ; i++)
		{
			if( test(i) && !inref.test(i) )	result.set(i);
			if( !test(i) && inref.test(i) )	result.set(i);
		}
		return result;
	}

	template < int Bound > inline
	bitmask<Bound> bitmask<Bound>::rising ( const bitmask& inref ) const
	{
		SU_ASSERT( inref.len == len );
		bitmask<Bound> result;
		for(int i=0; i<bound ; i++)
			if( test(i) && !inref.test(i) )	result.set(i);
		return result;
	}

	template < int Bound > inline
	bitmask<Bound> bitmask<Bound>::falling ( const bitmask& inref ) const
	{
		SU_ASSERT( inref.len == len );
		bitmask<Bound> result;
		for(int i=0; i<bound ; i++)
			if( !test(i) && inref.test(i) )	result.set(i);
		return result;
	}	

	template < int Bound > inline
	bitmask<Bound> bitmask<Bound>::switching ( const bitmask& inref ) const
	{
		SU_ASSERT( inref.len == len );
		return bfxor( inref );
	}

	template < int Bound > inline
	bitmask<Bound> bitmask<Bound>::operator - ( const bitmask& inref ) const
	{
		SU_ASSERT( inref.len == len );
		return switching( inref );
	}

	template < int Bound > inline
	bool bitmask<Bound>::equals ( const bitmask& inref ) const
	{
		SU_ASSERT( inref.len == len );
		for(int i=0; i<bound ; i++)
			if( test(i) != inref.test(i) ) return false;
		return true;
	}

	template < int Bound > inline
	bool bitmask<Bound>::operator == ( const bitmask& inref ) const
	{
		SU_ASSERT( inref.len == len );
		return equals( inref );	
	}

	template < int Bound > inline
	void bitmask<Bound>::operator = ( const bitmask& inref )
	{
		SU_ASSERT( inref.len == len );
		for( int i=0; i<len ; i++ )
			bits[i] = inref.bits[i];
	}

	template < int Bound > inline
	bool bitmask<Bound>::operator [] ( uint i ) const
	{		
		SU_ASSERT( i<Bound );
		return test(i);
	}

} }



#endif // _su_utili_h_


