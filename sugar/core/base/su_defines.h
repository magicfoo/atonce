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



#ifndef _su_defines_h_
#define _su_defines_h_



#define	SU_CC_ALIGNED32( inType, inVar )			SU_CC_ALIGNED_X( inType, inVar, 4 )
#define	SU_CC_ALIGNED64( inType, inVar )			SU_CC_ALIGNED_X( inType, inVar, 8 )
#define	SU_CC_ALIGNED128( inType, inVar )			SU_CC_ALIGNED_X( inType, inVar, 16 )	// 1QW
#define	SU_CC_ALIGNED512( inType, inVar )			SU_CC_ALIGNED_X( inType, inVar, 64 )	// 4QW
#define	SU_CC_ALIGNED1024( inType, inVar )			SU_CC_ALIGNED_X( inType, inVar, 128 )	// 8QW
#define	SU_CC_ALIGNED_DCACHE( inType, inVar )		SU_CC_ALIGNED_X( inType, inVar, SU_HW_DCACHE_BALIGN )
#define	SU_CC_ALIGNED_IO( inType, inVar )			SU_CC_ALIGNED_X( inType, inVar, SU_HW_IO_BALIGN )



#define	SU_COMPILE_TIME_CHECK_TYPES								\
		SU_COMPILE_TIME_ASSERT( sizeof(uint8)   == 1  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(uint16)  == 2  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(uint32)  == 4  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(uint64)  == 8  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(uint128) == 16 );		\
		SU_COMPILE_TIME_ASSERT( sizeof(int8)    == 1  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(int16)   == 2  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(int32)   == 4  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(int64)   == 8  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(int128)  == 16 );		\
		SU_COMPILE_TIME_ASSERT( sizeof(pvoid)   == 4  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(pcstr)   == 4  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(pstr)    == 4  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(uint8*)  == 4  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(uint16*) == 4  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(uint32*) == 4  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(uint64*) == 4  );		\
		SU_COMPILE_TIME_ASSERT( sizeof(uint128*)== 4  );



#define	SU_DECL_INTERFACE( inCLASSBASE, inCLASSID ) \
	typedef inCLASSBASE		ClassBase; \
	static const classid	ClassId = inCLASSID;


#define SU_NO_CMP_OPERATORS( inCLASS ) \
	private: \
		bool operator == ( const inCLASS& ) { SU_ERROR("Invalid operator !"); return false; } \
		bool operator != ( const inCLASS& ) { SU_ERROR("Invalid operator !"); return false; } \
	public:

#define SU_NO_ASSIGN_OPERATORS( inCLASS ) \
	private: \
		inCLASS& operator = ( const inCLASS& ) { SU_ERROR("Invalid operator !"); return *this; } \
	public:

#define SU_NO_OPERATORS( inCLASS ) \
	SU_NO_CMP_OPERATORS( inCLASS ) \
	SU_NO_ASSIGN_OPERATORS( inCLASS )




// member sizeof/offsetof
// Theses versions *try* to work around warnings generated by offsetof() on non-POD types members,
// not allowed by C++ standard but safely supported by some compilers.
// An arbitrary base offset is used to circumvent g++'s invalid offsetof check.
// GCC option -Wno-invalid-offsetof could also be used.
#if !defined(SU_MOFFSET_BASE)
#define	SU_MOFFSET_BASE	(0)
#endif
#define	SU_MSIZE(	inCLASS, inMEMBER )	sizeof((((const inCLASS*)SU_MOFFSET_BASE)->inMEMBER))
#define	SU_MOFFSET(	inCLASS, inMEMBER )	uint(((char*)(&(((const inCLASS*)SU_MOFFSET_BASE)->inMEMBER)))-((char*)SU_MOFFSET_BASE))
#define	SU_MQOFFSET(inCLASS, inMEMBER )	(SU_MOFFSET(inCLASS,inMEMBER)>>4)



//
// compile time assert
// SU_COMPILE_TIME_ASSERT_DECL can be used in any C-declaration area.

#define	SU_COMPILE_TIME_ASSERT(inC)					{ typedef int	__compile_time_assert_fail[1-2*!(inC)]; }
#define	SU_COMPILE_TIME_ASSERT_ISPOD( inC )			{ typedef union __compile_time_assert_ispod { inC t; };	}	// non-POD type not allowed in union (C++ standard)
#define	SU_COMPILE_TIME_ASSERT_ISRVALUE(inV)		{ typedef int	__compile_time_assert_isrvalue[inV];	}
#define	SU_COMPILE_TIME_ASSERT_ISSAMETYPE(inU,inV)	{ typedef int	__compile_time_assert_issametype[1-2*!(su::ttrait::is_sametype<inU,inV>::value)]; }
#define	SU_COMPILE_TIME_ASSERT_ISPOINTER(inC)		{ typedef int	__compile_time_assert_ispointer[1-2*!(su::ttrait::is_pointer<inC>::value)]; }
#define	SU_COMPILE_TIME_ASSERT_DECL(inName,inC)		struct inName SU_COMPILE_TIME_ASSERT(inC) ;



//
// console output


#if (SU_BUILD_CONSOLE)

	#define SU_STDOUT( inMSG ) \
		{ \
			if( su::globals::cout ) { \
				su::globals::cout->Output( inMSG, __FILE__, __FUNCTION__, __LINE__ ); \
		} }

	#define SU_STDERR( inMSG ) \
		{ \
			if( su::globals::cerr ) { \
				su::globals::cerr->Output( inMSG, __FILE__, __FUNCTION__, __LINE__ ); \
		} }

	#define	SU_ERROR( inMSG ) \
		{	static bool skip = FALSE; \
			if( !skip ) { \
				if( su::globals::cerr ) { \
					su::IConsole::Action act = su::globals::cerr->AskUserAction( inMSG, __FILE__, __FUNCTION__, __LINE__ ); \
					if(		 act == su::IConsole::A_DONTCARE )	skip = TRUE; \
					else if( act == su::IConsole::A_EXIT )		su::core::Exit(-1); \
					else if( act == su::IConsole::A_BREAK )		SU_BREAK(); \
		} } }

#else

	#define SU_STDOUT( inMSG )		{}
	#define SU_STDERR( inMSG )		{}
	#define	SU_ERROR( inMSG )		{}

#endif




//
// verbose

#if (SU_BUILD_VERBOSE)
	#define SU_VERBOSE_STR( inSTR )		(inSTR)
#else
	#define SU_VERBOSE_STR( inSTR )		NULL
#endif




//
// debug control code

#if (SU_BUILD_DBGCODE)
	#define SU_DBGOUT(inMSG)				SU_STDOUT( inMSG )
	#define	SU_BREAK_IF(inC)				{ if(inC)    { SU_BREAK(); } }
	#define SU_ASSERT(inC)					{ if(!(inC)) { SU_ERROR( "Assert has failed on "#inC"\n" ); } }
	#define SU_ASSERT_IF(inC0,inC1)			{ if(inC0)	 { SU_ASSERT(inC1); } }
	#define SU_ASSERTC(inC,inMSG)			{ if(!(inC)) { SU_ERROR(inMSG); } }
	#define SU_ASSERT_ALIGNED(inC,inA)		SU_ASSERTC( (inC)&&((uint32(inC)&((inA)-1))==0), "Data must be aligned on a "#inA" bytes boundary and not NULL !\n" )
	#define SU_ASSERT_A32(inC)				SU_ASSERT_ALIGNED(inC,4)
	#define SU_ASSERT_A64(inC)				SU_ASSERT_ALIGNED(inC,8)
	#define SU_ASSERT_A128(inC)				SU_ASSERT_ALIGNED(inC,16)
	#define SU_ASSERT_NA128(inC)			SU_ASSERTC( ((uint32(inC)&0xF)==0), "Data must be aligned on a 16 bytes boundary !\n" )
	#define SU_ASSERT_A256(inC)				SU_ASSERT_ALIGNED(inC,32)
	#define SU_ASSERT_NA256(inC)			SU_ASSERTC( ((uint32(inC)&0x1F)==0), "Data must be aligned on a 32 bytes boundary !\n" )
	#define SU_ASSERT_A512(inC)				SU_ASSERT_ALIGNED(inC,64)
	#define SU_ASSERT_RESULT(inC)			SU_ASSERT( inC )
	#define	SU_ASSERT_RETURN_MTH(inRes,inC)	{ if(!(inC)) { SU_ASSERT(inC); return inRes; } }
	#define	SU_ASSERT_RETURN_CAL(inC)		{ if(!(inC)) { SU_ASSERT(inC); return; } }
	#define SU_ASSERT_CSTRING(inS)			SU_ASSERT( inS[sizeof(inS)-1]==0 )
	#define SU_TRAPPED(inMSG,inCODE)		{ const char* _msg = inMSG; *((int*)0) = 0xdeadf00d; }
#else
	#define SU_DBGOUT( inMSG )				{}
	#define	SU_BREAK_IF(inC)				{}
	#define	SU_ASSERT(inC)					{}
	#define	SU_ASSERT_IF(inC0,inC1)			{}
	#define	SU_ASSERTC(inC,inMSG)			{}
	#define SU_ASSERT_ALIGNED(inC,inA)		{}
	#define SU_ASSERT_A32(inC)				{}
	#define SU_ASSERT_A64(inC)				{}
	#define SU_ASSERT_A128(inC)				{}
	#define SU_ASSERT_NA128(inC)			{}
	#define SU_ASSERT_A256(inC)				{}
	#define SU_ASSERT_NA256(inC)			{}
	#define SU_ASSERT_A512(inC)				{}
	#define SU_ASSERT_RESULT(inC)			{ (inC); }
	#define	SU_ASSERT_RETURN_MTH(inRes,inC)	{ if(!(inC)) { return inRes; } }
	#define	SU_ASSERT_RETURN_CAL(inC)		{ if(!(inC)) { return; } }
	#define SU_ASSERT_CSTRING(inC)			{}
	#define SU_TRAPPED(inMSG,inCODE)		{ const char* _msg = inMSG; *((int*)0) = 0xdeadf00d; }
#endif




#define SU_MCC32( in3, in2, in1, in0 ) \
	(((uint32)in0)|(((uint32)in1)<<8)|(((uint32)in2)<<16)|(((uint32)in3)<<24))

#define SU_MCHAR1( x )					SU_MCC32( 0,    0,    0,    x[0] )
#define SU_MCHAR2( x )					SU_MCC32( 0,    0,    x[0], x[1] )
#define SU_MCHAR3( x )					SU_MCC32( 0,    x[0], x[1], x[2] )
#define SU_MCHAR4( x )					SU_MCC32( x[0], x[1], x[2], x[3] )



#define SU_STRINGIFY( I )	#I


#define SU_SIZEOF( Array )	(sizeof(Array)/sizeof(Array[0]))



#define SU_RETURN_V_IF( inC, inValue )				if( (inC) )   { return (inValue); }
#define SU_RETURN_IF( inC )							if( (inC) )   { return; }

#define SU_REQUIRE_OR_RETURN_V( inC, inValue )		if( !(inC) )  { return (inValue); }
#define SU_REQUIRE_OR_RETURN( inC )					if( !(inC) )  { return; }

#define SU_RETURN_FALSE_IF( inC )					SU_RETURN_V_IF( (inC), FALSE )
#define SU_RETURN_TRUE_IF( inC )					SU_RETURN_V_IF( (inC), TRUE )
#define SU_RETURN_NULL_IF( inC )					SU_RETURN_V_IF( (inC), NULL )

#define SU_REQUIRE_OR_FALSE( inC )					SU_REQUIRE_OR_RETURN_V( (inC), FALSE )
#define SU_REQUIRE_OR_TRUE( inC )					SU_REQUIRE_OR_RETURN_V( (inC), TRUE )
#define SU_REQUIRE_OR_NULL( inC )					SU_REQUIRE_OR_RETURN_V( (inC), NULL )
#define SU_REQUIRE_OR_ZERO( inC )					SU_REQUIRE_OR_RETURN_V( (inC), 0 )
#define SU_REQUIRE_OR_0( inC )						SU_REQUIRE_OR_RETURN_V( (inC), 0 )
#define SU_REQUIRE_OR_NEG( inC )					SU_REQUIRE_OR_RETURN_V( (inC), -1 )




#define SU_IMPLEMENTS( inC, inClassId ) \
	{	su::IInterface* i = su::BaseInterface( (inC*)this, inClassId ); \
		if( i )	return i; \
	}

#define SU_IMPLEMENTS_MEMBER( inMember, inClassId ) \
	{	su::IInterface* i = su::BaseInterface( &inMember, inClassId ); \
		if( i )	return i; \
	}


#define	SU_DELEGATE_BASEPTR( inC, inI, inThis )															((inC*)(((char*)inThis)-((char*)SU_MOFFSET(inC,inI))))
#define	SU_DELEGATE_CHECKOFFSET( inC, inI, inThis )														SU_ASSERT( &(SU_DELEGATE_BASEPTR(inC,inI,inThis)->inI) == (inThis) )

#define	SU_DELEGATE_MTH0( inC, inI, inMTH, inTR )														inTR inMTH ( ) { SU_DELEGATE_CHECKOFFSET(inC,inI,this); return SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(); }
#define	SU_DELEGATE_MTH1( inC, inI, inMTH, inTR, inT0 )													inTR inMTH ( inT0 P0 ) { SU_DELEGATE_CHECKOFFSET(inC,inI,this); return SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0); }
#define	SU_DELEGATE_MTH2( inC, inI, inMTH, inTR, inT0, inT1 )											inTR inMTH ( inT0 P0, inT1 P1 ) { SU_DELEGATE_CHECKOFFSET(inC,inI,this); return SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1); }
#define	SU_DELEGATE_MTH3( inC, inI, inMTH, inTR, inT0, inT1, inT2 )										inTR inMTH ( inT0 P0, inT1 P1, inT2 P2 ) { SU_DELEGATE_CHECKOFFSET(inC,inI,this); return SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1,P2); }
#define	SU_DELEGATE_MTH4( inC, inI, inMTH, inTR, inT0, inT1, inT2, inT3 )								inTR inMTH ( inT0 P0, inT1 P1, inT2 P2, inT3 P3 ) { SU_DELEGATE_CHECKOFFSET(inC,inI,this); return SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1,P2,P3); }
#define	SU_DELEGATE_MTH5( inC, inI, inMTH, inTR, inT0, inT1, inT2, inT3, inT4 )							inTR inMTH ( inT0 P0, inT1 P1, inT2 P2, inT3 P3, inT4 P4 )	{ SU_DELEGATE_CHECKOFFSET(inC,inI,this); return SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1,P2,P3,P4); }
#define	SU_DELEGATE_MTH6( inC, inI, inMTH, inTR, inT0, inT1, inT2, inT3, inT4, inT5 )					inTR inMTH ( inT0 P0, inT1 P1, inT2 P2, inT3 P3, inT4 P4, inT5 P5 )	{ SU_DELEGATE_CHECKOFFSET(inC,inI,this); return SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1,P2,P3,P4,P5); }
#define	SU_DELEGATE_MTH7( inC, inI, inMTH, inTR, inT0, inT1, inT2, inT3, inT4, inT5, inT6 )				inTR inMTH ( inT0 P0, inT1 P1, inT2 P2, inT3 P3, inT4 P4, inT5 P5, inT6 P6 )	{ SU_DELEGATE_CHECKOFFSET(inC,inI,this); return SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1,P2,P3,P4,P5,P6); }
#define	SU_DELEGATE_MTH8( inC, inI, inMTH, inTR, inT0, inT1, inT2, inT3, inT4, inT5, inT6, inT7 )		inTR inMTH ( inT0 P0, inT1 P1, inT2 P2, inT3 P3, inT4 P4, inT5 P5, inT6 P6, inT7 P7 ) { SU_DELEGATE_CHECKOFFSET(inC,inI,this); return SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1,P2,P3,P4,P5,P6,P7); }

#define	SU_DELEGATE_CAL0( inC, inI, inMTH )																void inMTH ( ) { SU_DELEGATE_CHECKOFFSET(inC,inI,this); SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(); }
#define	SU_DELEGATE_CAL1( inC, inI, inMTH, inT0 )														void inMTH ( inT0 P0 ) { SU_DELEGATE_CHECKOFFSET(inC,inI,this); SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0); }
#define	SU_DELEGATE_CAL2( inC, inI, inMTH, inT0, inT1 )													void inMTH ( inT0 P0, inT1 P1 ) { SU_DELEGATE_CHECKOFFSET(inC,inI,this); SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1); }
#define	SU_DELEGATE_CAL3( inC, inI, inMTH, inT0, inT1, inT2 )											void inMTH ( inT0 P0, inT1 P1, inT2 P2 ) { SU_DELEGATE_CHECKOFFSET(inC,inI,this); SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1,P2); }
#define	SU_DELEGATE_CAL4( inC, inI, inMTH, inT0, inT1, inT2, inT3 )										void inMTH ( inT0 P0, inT1 P1, inT2 P2, inT3 P3 ) { SU_DELEGATE_CHECKOFFSET(inC,inI,this); SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1,P2,P3); }
#define	SU_DELEGATE_CAL5( inC, inI, inMTH, inT0, inT1, inT2, inT3, inT4 )								void inMTH ( inT0 P0, inT1 P1, inT2 P2, inT3 P3, inT4 P4 )	{ SU_DELEGATE_CHECKOFFSET(inC,inI,this); SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1,P2,P3,P4); }
#define	SU_DELEGATE_CAL6( inC, inI, inMTH, inT0, inT1, inT2, inT3, inT4, inT5 )							void inMTH ( inT0 P0, inT1 P1, inT2 P2, inT3 P3, inT4 P4, inT5 P5 )	{ SU_DELEGATE_CHECKOFFSET(inC,inI,this); SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1,P2,P3,P4,P5); }
#define	SU_DELEGATE_CAL7( inC, inI, inMTH, inT0, inT1, inT2, inT3, inT4, inT5, inT7 )					void inMTH ( inT0 P0, inT1 P1, inT2 P2, inT3 P3, inT4 P4, inT5 P5, inT6 P6 )	{ SU_DELEGATE_CHECKOFFSET(inC,inI,this); SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1,P2,P3,P4,P5,P6); }
#define	SU_DELEGATE_CAL8( inC, inI, inMTH, inT0, inT1, inT2, inT3, inT4, inT5, inT6, inT7 )				void inMTH ( inT0 P0, inT1 P1, inT2 P2, inT3 P3, inT4 P4, inT5 P5, inT6 P6, inT7 P7 ) { SU_DELEGATE_CHECKOFFSET(inC,inI,this); SU_DELEGATE_BASEPTR(inC,inI,this)->inMTH(P0,P1,P2,P3,P4,P5,P6,P7); }




#define su_type( inType ) \
	namespace su { struct inType; }


#define su_ns_type( inNS, inType ) \
	namespace su { namespace inNS { struct inType; } }



#define SU_BEGIN_STARTER( inName ) \
	namespace { struct G : public addin::Starter {

#define SU_END_STARTER( inName ) \
	} _starter_##inName; }



#define SU_BEGIN_BUILDER( inName ) \
	namespace { struct G : public addin::Builder {

#define SU_END_BUILDER( inName ) \
	} _builder_##inName; } \
	void su_forceref_builder_##inName() {}




/*
#if defined(SU_FRAMEWORK) && !defined(SU_NEWDELETE)
	#undef	new
	#undef	delete
	inline void*	operator new		( SU_SIZET bsize )	{	SU_TRAPPED( "Illegal new operator in framework part !", 0 ); return NULL;	}
	inline void*	operator new []		( SU_SIZET bsize )	{	SU_TRAPPED( "Illegal new operator in framework part !", 0 ); return NULL;	}
	inline void		operator delete		( void* ptr )		{	SU_TRAPPED( "Illegal delete operator in framework part !", 0 );				}
	inline void		operator delete []	( void* ptr )		{	SU_TRAPPED( "Illegal delete operator in framework part !", 0 );				}
#endif
*/




namespace su
{

	enum TransferState
	{
		TSFR_IDLE = (0),
		TSFR_PENDING,
		TSFR_INPROGRESS,
		TSFR_COMPLETED,
		TSFR_ABORTED = (-1),
		TSFR_FAILED	= (-2)
	};

	enum TransferError
	{
		TSFR_SUCCESS = (0),
		TSFR_ERR_PARAMS,
		TSFR_ERR_DATA,
		TSFR_ERR_SUPPORT,
		TSFR_ERR_FATAL,
		TSFR_ERR_RETRY,
		TSFR_ERR_UNDEFINED
	};

	enum TransferSubError
	{
		TSFR_EERR_SUCCESS = (0),
		TSFR_EERR_INVALID_PARAMS,
		TSFR_EERR_NO_DATA,
		TSFR_EERR_BAD_DATA,
		TSFR_EERR_WRONG_DISK,
		TSFR_EERR_DAMAGED_DISK,
		TSFR_EERR_NO_DISK,
		TSFR_EERR_NO_DRIVE,
		TSFR_EERR_UNDEFINED
	};

	struct TransferParams
	{
		unsigned char*	baddr;
		unsigned long	bsize;
		unsigned long	boffset;
	};

}



#endif // _su_defines_h_



