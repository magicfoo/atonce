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



#ifndef _su_util_h_
#define _su_util_h_


namespace su { struct Pool; }




namespace su { namespace util
{

	// Memory operations

	su_ie void*						Memset				(	void* inPtr, int inValue, uint32 inSize							);
	su_ie void*						Memcopy				(	void* inDst, const void* inSrc, uint32 inSize					);
	su_ie void*						Memncopy			(	void* inDst, const void* inSrc, uint32 inSize, uint inCpt		);
	su_ie void*						Memmove				(	void* inDst, const void* inSrc, uint32 inSize					);
	su_ie int						Memcompare			(	const void* inA, const void* inB, uint inSize					);


	// Object memory operations

	template < typename T >	T*		Objectctor			(	T* inDst, uint inCount											);
	template < typename T >	T*		Objectdtor			(	T* inDst, uint inCount											);
	template < typename T >	T*		Objectcopy			(	T* inDst, T* inSrc, uint inCount								);
	template < typename T >	T*		Objectncopy			(	T* inDst, T* inSrc, uint inCount								);
	template < typename T >	T*		Objectmove			(	T* inDst, T* inSrc, uint inCount								);
	template < typename T >	T*		Objectnew			(	Pool*, uint inCount												);
	template < typename T >	void	Objectdelete		(	Pool*, T* inO, uint inCount										);


	// char evaluations and operations

	su_ie bool						Isspace				(	int	  c															);
	su_ie bool						Isalpha				(	int	  c															);
	su_ie bool						Isdigit				(	int	  c															);
	su_ie bool						Isdigit2			(	int	  c															);
	su_ie bool						Isdigit16			(	int	  c															);
	su_ie bool						Isalnum				(	int	  c															);
	su_ie bool						Isupper				(	int	  c															);
	su_ie bool						Islower				(	int	  c															);
	su_ie int						Toupper				(	int   c															);
	su_ie int						Tolower				(	int	  c															);


	// string operations

	su_ie bool						Strisempty			(	pcstr inString													);
	su_ie bool						Strisspace			(	pcstr inString													);
	su_ie bool						Strisalpha			(	pcstr inString													);
	su_ie bool						Strisdigit			(	pcstr inString													);
	su_ie bool						Strisdigit2			(	pcstr inString													);
	su_ie bool						Strisdigit16		(	pcstr inString													);
	su_ie bool						Strisalnum			(	pcstr inString													);
	su_ie bool						Strisupper			(	pcstr inString													);
	su_ie bool						Strislower			(	pcstr inString													);
	su_ie bool						Strisident			(	pcstr inString													);
	su_ie pstr						Strcopy				(	pstr  inDest,  pcstr inSrc										);
	su_ie pstr						Strcopy				(	pstr  inDest,  pcstr inSrc, uint len							);
	su_ie pstr						Strconcat			(	pstr  inDest,  pcstr inSrc										);
	su_ie uint						Strlen				(	pcstr inString													);
	su_ie pcstr						Strend				(	pcstr inString													);		// ending '0'
	su_ie pcstr						Strlast				(	pcstr inString													);		// last character
	su_ie int						Strcompare			(	pcstr string1, int inC, bool cs=TRUE							);
	su_ie int						Strcompare			(	pcstr string1, pcstr string2, bool cs=TRUE						);
	su_ie int						Strcompare			(	pcstr string1, pcstr string2, uint len, bool cs=TRUE			);
	su_ie bool						Strequals			(	pcstr string1, int inC, bool cs=TRUE							);
	su_ie bool						Strequals			(	pcstr string1, pcstr string2, bool cs=TRUE						);
	su_ie bool						Strequals			(	pcstr string1, pcstr string2, uint len, bool cs=TRUE			);
	su_ie bool						Strstartswith		(	pcstr inString, int inC, bool cs=TRUE							);
	su_ie bool						Strstartswith		(	pcstr inString, pcstr inRef, bool cs=TRUE						);
	su_ie bool						Strstartswithany	(	pcstr inString, pcstr inCharSet, bool cs=TRUE					);
	su_ie bool						Strendswith			(	pcstr inString, int inC, bool cs=TRUE							);
	su_ie bool						Strendswith			(	pcstr inString, pcstr inRef, bool cs=TRUE						);
	su_ie bool						Strendswithany		(	pcstr inString, pcstr inCharSet, bool cs=TRUE					);
	su_ie pcstr						Strfind				(	pcstr inString, int inC, bool cs=TRUE, bool rev=FALSE			);
	su_ie pstr						Strfind				(	pstr  inString, int inC, bool cs=TRUE, bool rev=FALSE			);
	su_ie pcstr						Strfind				(	pcstr inString, pcstr inSearch, bool cs=TRUE, bool rev=FALSE	);
	su_ie pstr						Strfind				(	pstr  inString, pcstr inSearch, bool cs=TRUE, bool rev=FALSE	);
	su_ie pcstr						Strfindany			(	pcstr inString, pcstr inCharSet, bool cs=TRUE, bool rev=FALSE	);
	su_ie pstr						Strfindany			(	pstr inString,  pcstr inCharSet, bool cs=TRUE, bool rev=FALSE	);
	su_ie int						Strindexof			(	pcstr inString, int inC, bool cs=TRUE, bool rev=FALSE			);		// < 0 => not found !
	su_ie int						Strindexof			(	pcstr inString, pcstr inSearch, bool cs=TRUE, bool rev=FALSE	);		// < 0 => not found !
	su_ie int						Strindexofany		(	pcstr inString, pcstr inCharSet, bool cs=TRUE, bool rev=FALSE	);		// < 0 => not found !
	su_ie pstr						Strtoupper			(	pstr  outString, pcstr inString									);
	su_ie pstr						Strtolower			(	pstr  outString, pcstr inString									);
	su_ie pstr						Strtoupper			(	pstr  ioString													);		// convert in place
	su_ie pstr						Strtolower			(	pstr  ioString 													);		// convert in place
	su_ie float						Strtofloat			(	pcstr inString,  pstr* endPtr									);		// fails if *endPtr==NULL or *endPtr==inString
	su_ie long						Strtolong			(	pcstr inString,  pstr* endPtr, int base							);		// fails if *endPtr==NULL or *endPtr==inString
	su_ie ulong						Strtoulong			(	pcstr inString,  pstr* endPtr, int base							);		// fails if *endPtr==NULL or *endPtr==inString
	su_ie long						Strtoi10			(	pcstr inString													);		// base=10
	su_ie ulong						Strtou10			(	pcstr inString													);		// base=10
	su_ie int						Strformat			(	pstr  outString, char c, int rep=1								);
	su_ie int						Strformat			(	pstr  outString, long a, int base								);
	su_ie int						Strformat			(	pstr  outString, ulong a, int base								);
	su_ie int						Strformat			(	pstr  outString, float a, char fmt='g', int prec=6				);
	su_ie int						Strformat			(	pstr  outString, pcstr inFormat, ...							);		// sprintf like
	su_ie bool						Stroption			(	pcstr inString, pcstr inOption, pcstr& outStart, pcstr& outEnd	);		// [start,end[
	su_ie bool						Stroption			(	pcstr inString, pcstr inOption, pstr inBuffer, uint inCap		);
	su_ie template<typename T> bool	Stroption			(	pcstr inString, pcstr inOption, T& inBuffer						);
	su_ie pcstr						Strextract			(	pcstr inString, int inIth, char inSep=','						);
	su_ie pcstr						Strjoin				(	pstr inBuffer, pcstr inPos, pcstr inText, char inSep=','		);		// returns the current position in the buffer

	// filename style pattern matching

	enum FnmatchFlags
	{
		FNM_NOESCAPE	= 0x01,		// Disable backslash escaping.
		FNM_PATHNAME	= 0x02,		// Slash must be matched by slash.
		FNM_PERIOD		= 0x04,		// Period must be matched by period.
		FNM_LEADING_DIR	= 0x08,		// Ignore /<tail> after Imatch.
		FNM_CASEFOLD	= 0x10,		// Case insensitive search.
		FNM_IGNORECASE	= FNM_CASEFOLD,
		FNM_FILE_NAME	= FNM_PATHNAME
	};

	su_ie bool						Fnmatch				(	pcstr inPattern, pcstr inString, uint inFlags			);		// FnmatchFlags



	//
	// A convenience object to prevent buffer overflowing

	template < typename T=char, int Bound=256 >
	struct SafeBuffer
	{
		static const uint	bound = Bound;
		typedef T			value_type;
		typedef T*			pointer;
		typedef T&			reference;

							SafeBuffer ( );
						~	SafeBuffer ( );
		bool				check();
		int					size();
		pointer				addr();
		pointer				first();
		pointer				last();
		pointer				end();
		void				zero();
							operator pointer ( );
		reference			operator [] ( int );

	protected:
		uint32	mark0[2];
		T		bytes[Bound];
		uint32	mark1[2];
	};



	//
	// A Bitmask object to ...


	template < int Bound=32 >
	struct bitmask
	{
		static const uint	len   = (Bound+31)>>5;
		static const uint	bound = Bound;

							bitmask		( );
							bitmask		( const bitmask& inref );

		uint				countset	( ) const;
		uint				countunset	( ) const;
		void				set			( uint i );
		void				setrange	( uint i, uint j );
		void				setall		( );
		void				clear		( uint i );
		void				clearrange	( uint i, uint j );
		void				clearall	( );
		bool				test		( uint i ) const;		
		bool				testrange	( uint i, uint j ) const;
		bool				testall		( ) const;
		int					first		( ) const;
		int					next		( int i ) const;
		
		bitmask<Bound>		bfand		( const bitmask& inref ) const;
		bitmask<Bound>		bfor		( const bitmask& inref ) const;
		bitmask<Bound>		bfxor		( const bitmask& inref ) const;
		
		bitmask<Bound>		rising		( const bitmask& inref ) const;			// 0->1
		bitmask<Bound>		falling		( const bitmask& inref ) const;			// 1->0
		bitmask<Bound>		switching	( const bitmask& inref ) const;			// b->!b
		bitmask<Bound>		operator -	( const bitmask& inref ) const;

		bool				equals		( const bitmask& inref ) const;
		bool				operator ==	( const bitmask& inref ) const;
		void				operator =	( const bitmask& inref );

		bool				operator []	( uint i ) const;

		uint32 bits[len];
	};



	// RNG

	su_ie bool						RandRestore			(	pcstr		inRandState									);		// 5 uint32 : "uint32 uint32 uint32 uint32 uint32" 
	su_ie bool						RandSave			(	pstr		outRandStatea								);		// an allocated buffer with 256 chars.
	su_ie uint32					Rand				(	uint32		inM0=0, uint inM1=0xFFFFFF					);		// in [M0,M1]
	su_ie float						Randf				(															);		// in [0,+1]
	su_ie float						Randsf				(															);		// in [-1,+1]



	// Coding

	enum UUseqstyle
	{
		UUSEQ_DECIMAL = 0,
		UUSEQ_DUMPED,
		UUSEQ_MIXED,
	};

	su_ie uint						uuencodelen			(	void* inbuf, uint insize								);
	su_ie uint						uuencode			(	char* outbuf, void* inbuf, uint insize					);
	su_ie bool						uustream			(	pcstr ins0, pcstr ins1									);
	su_ie uint						uudecodelen			(	pcstr ins0, pcstr ins1									);
	su_ie uint						uudecode			(	void* outbuf, pcstr ins0, pcstr ins1					);

	su_ie uint						uuencodeseqlen		(	UUseqstyle style, int* array, int size					);
	su_ie uint						uuencodeseqlen		(	UUseqstyle style, float* array, int size				);
	su_ie uint						uuencodeseq			(	char* outbuf, UUseqstyle style, int* array, int size	);
	su_ie uint						uuencodeseq			(	char* outbuf, UUseqstyle style, float* array, int size	);
	su_ie uint						uudecodeseqlen		(	pcstr ins0, pcstr ins1									);
	su_ie uint						uudecodeseq			(	int* array, pcstr ins0, pcstr ins1						);
	su_ie uint						uudecodeseq			(	float* array, pcstr ins0, pcstr ins1					);



	// HMAC-ing

	// CRC-32 checksum ISO-3307 (4 bytes)
	su_ie byte*						hmac_crc32			(	pointer		inSegment,	uint	inBSize					);		// byte [4]
	su_ie uint32					hmac_crc32_w		(	pointer		inSegment,	uint	inBSize					);
	su_ie byte*						hmac_crc32			(	pcstr		inText,		bool	inNormalized = true		);
	su_ie uint32					hmac_crc32_w		(	pcstr		inText,		bool	inNormalized = true		);
	su_ie uint32					hmac_crc32_format	(	pcstr		inFormat,   ...								);

	// MD5 (16 bytes)
	su_ie byte*						hmac_md5			(	pointer		inSegment,	uint	inBSize					);		// byte [16]
	su_ie uint128					hmac_md5_w			(	pointer		inSegment,	uint	inBSize					);
	su_ie void						hmac_md5_begin		(															);
	su_ie void						hmac_md5_append		(	pointer		inSegment,	uint	inBSize					);
	su_ie byte*						hmac_md5_end		(															);

	// SHA-1 (20 bytes)
	su_ie byte*						hmac_sha1			(	pointer		inSegment,	uint	inBSize					);		// byte [20]
	su_ie void						hmac_sha1_begin		(															);
	su_ie void						hmac_sha1_append	(	pointer		inSegment,	uint	inBSize					);
	su_ie byte*						hmac_sha1_end		(															);

	// tools
	su_ie classid					hmac_classid		(	pcstr		inText										);
	su_ie pcstr						hmac_string			(	byte*		inHmac,		uint inHlen						);




	// Endianess

	template <typename T> T			InvertByteOrder		(	T  inC					);
	template <typename T> void		Swap				(   T & ioC1, T & ioC2		);

	template <typename T> T			LSBToNative			(	T				);		// according to SU_HW_LSB / SU_HW_MSB
	template <typename T> T			MSBToNative			(	T				);		// according to SU_HW_LSB / SU_HW_MSB
	template <typename T> T			NativeToLSB			(	T				);		// according to SU_HW_LSB / SU_HW_MSB
	template <typename T> T			NativeToMSB			(	T				);		// according to SU_HW_LSB / SU_HW_MSB
	template <typename T> T			ConvertLSB			(	T&				);		// inplace LSB -> native
	template <typename T> T			ConvertMSB			(	T&				);		// inplace MSB -> native
	template <typename T> T			SelectLSB			(	T, T			);		// (LSB? A: B)
	template <typename T> T			SelectMSB			(	T, T			);		// (MSB? A: B)


	su_ie void*						Zero				(	void* inDst, uint32 inSize										);
	template <typename inC> void*	Zero				(	inC & inV														) { return Zero(&inV,sizeof(inC)); }

	su_ie void*						Uninitialized		(	void* inDst, uint32 inSize										);
	template <typename inC> void*	Uninitialized		(	inC & inV														) { return Uninitialized(&inV,sizeof(inC)); }

	su_ie bool						CheckUninitialized	(	void* inDst, uint32 inSize										);
	template <typename inC> bool	CheckUninitialized	(	inC & inV														) { return CheckUninitialized(&inV,sizeof(inC)); }



	// Upper round. inA must be a power of two !
	// RoundX( 5, 16 ) = 16
	// RoundX( 5, 0 )  = 5
	// Round2( v ) = RoundX( v, 2 )

	template <typename T> T			RoundX				( T inV, uint inA );
	template <typename T> T			Round2				( T inV );
	template <typename T> T			Round4				( T inV );
	template <typename T> T			Round8				( T inV );
	template <typename T> T			Round16				( T inV );
	template <typename T> T			Round32				( T inV );
	template <typename T> T			Round64				( T inV );
	template <typename T> T			Round128			( T inV );
	template <typename T> T			Round256			( T inV );
	template <typename T> T			Round512			( T inV );
	template <typename T> T			Round1024			( T inV );


	// inV == 2^N ?
	inline bool						IsPow2				( uint32 inV );
	inline bool						IsPow2				( uint64 inV );

	// GetFloorPow2( 16 ) = 4
	// GetFloorPow2( 18 ) = 4
	inline uint						GetFloorPow2		( uint32 inV );

	// GetCeilPow2( 16 ) = 4
	// GetCeilPow2( 18 ) = 5
	inline uint						GetCeilPow2			( uint32 inV );

	// GetLowestPowOf2( 100010100b ) = 000000100b
	// GetLowestPowOf2( 100010000b ) = 000010000b
	// returned 0 if v == 0 !
	inline uint32					GetLowestPowOf2		( uint32 v );
	inline uint64					GetLowestPowOf2		( uint64 v );

	// GetGreatestPowOf2( 000010100b ) = 000010000b
	// GetGreatestPowOf2( 010010000b ) = 010000000b
	// returned 0 if v == 0 !
	inline uint32					GetGreatestPowOf2	( uint32 v );
	inline uint64					GetGreatestPowOf2	( uint64 v );

	// GetLowestBitNo( 100010100b ) = 2
	// GetLowestBitNo( 100010000b ) = 4
	// Undefined if v == 0 !
	inline uint						GetLowestBitNo		( uint32  v );
	inline uint						GetLowestBitNo		( uint64  v );

	// GetGreatestBitNo( 000010100b ) = 4
	// GetGreatestBitNo( 010010000b ) = 7
	// Undefined if v == 0 !
	inline uint						GetGreatestBitNo	( uint32  v );
	inline uint						GetGreatestBitNo	( uint64  v );

	// Bit count
	// GetBitCount(0) = 0
	// GetBitCount(3) = 2
	// ...
	inline uint						GetBitCount			( uint32 v );
	inline uint						GetBitCount			( uint64 v );

	// GetBitRangeMask
	// GetBitRangeMask( 1,1 ) = b00000000000000000000000000000010
	// GetBitRangeMask( 1,30) = b01111111111111111111111111111110
	// GetBitRangeMask( 30,1) = b11000000000000000000000000000011
	inline uint32					GetBitRangeMask		( uint inFromNo, uint inToNo );

	// GetBitRangeCount
	// GetBitRangeCount( 0x00000002, 0 ) = 0
	// GetBitRangeCount( 0x00000002, 1 ) = 1
	// GetBitRangeCount( 0xFFFF0000, 0 ) = 0
	// GetBitRangeCount( 0xFFFF0000, 16) = 16
	// GetBitRangeCount( 0xFFFF0000, 31) = 1
	inline uint						GetBitRangeCount	( uint32 inV, uint inFromNo	);

	// float -> uint
	inline uint						Ftoui				( float v );
	inline int32					FTOI_19_4			( float v );
	inline int32					FTOI_23_0			( float v );

	// 32bits aligned uint64 target copy
	inline void						cpy64to32			( uint32* dst32/*[2]*/, uint64* src64/*[1]*/ );

	// copy 32bits-hi-part
	inline void						cpy64HIto32			( uint32* dst32/*[1]*/, uint64* src64/*[1]*/ );

	// copy 32bits-low-part
	inline void						cpy64LOto32			( uint32* dst32/*[1]*/, uint64*	src64/*[1]*/ );

	inline uint32					asuint32			( float  v );
	inline uint32					asuint32			( uint32 v );
	inline uint32					asuint32			( int32  v );
	inline uint32					asuint32			( bool   v );
	inline uint32					asuint32			( void*  v );


	// r/w bits, limited to a word of 16bits
	// uses the MSB storage model
	// (p) is the nearest byte pointer on the bit stream
	// (s) is the shift counter (i.e. the bit offset)
	// (n) is the amount of bits to read from the stream, limited to 16b for performance reasons !
	// (i) is the value to write to the stream
	su_ie uchar						ub_read				( byte* p, int s, int n );
	su_ie char						sb_read				( byte* p, int s, int n );
	su_ie void						ub_write			( byte* p, int s, int n, uchar i );
	su_ie void						sb_write			( byte* p, int s, int n, char  i );
	su_ie int						ub_align			( byte* p, int s, int a );
	su_ie void						ub_copy				( byte* p, int s, byte* fromp, int froms, int n );
	su_ie void						ub_zero				( byte* p, int s, int n );


	// r/w variable-length integers, limited to a long word of 32bits
	// uses the MSB storage model
	su_ie uint						ui_read				( byte* p, int s, int n );
	su_ie int						si_read				( byte* p, int s, int n );
	su_ie void						ui_write			( byte* p, int s, int n, uint i );
	su_ie void						si_write			( byte* p, int s, int n, int  i );


	template<typename LessThanOrder>
	void							iqsort				(	short*		ioIndexes,
															uint		inCount,
															void*		inArg		);
	template<typename LessThanOrder>
	short*							iqsort				(	uint		inCount,
															void*		inArg		);

} }



#include "su_utili.h"
#endif // _su_util_h_



