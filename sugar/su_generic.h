/*LIC-HDR*****************************************************************
**
** Copyright (C) 2001-2010 AtOnce Technologies
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


#if defined(SU_GENERIC) || defined(SU_HW_GENERIC) 


#if defined(SU_HW) && !defined(SU_HW_GENERIC)
	#error "Platform conflict detected !"
#endif


#if !defined(SU_GENERIC)
	#define SU_GENERIC
#endif

#if !defined(SU_HW_GENERIC)
	#define SU_HW_GENERIC
#endif


#if !defined(SU_HW)
	#define SU_HW						"generic"
	#define SU_HW_LSB
	#define SU_HW_DCACHE_BALIGN			(16)
	#define	SU_HW_IO_BALIGN				(16)
	#define	SU_HW_FILE_BALIGN			(4)
	#define	SU_HW_DMAC_BALIGN			(16)
	#define	SU_HW_MEM_BALIGN			(16)
#endif // SU_HW


// -- cc

#if defined(SU_CC) && !defined(SU_CC_GENERIC)
	#error "Compiler conflict detected !"
#endif


#if !defined(SU_CC)
	#define SU_CC_GENERIC
	#define SU_CC						"generic"

	#define SU_CC_DECL_INT64( tname ) \
		typedef signed long long tname;

	#define SU_CC_DECL_UINT64( tname ) \
		typedef unsigned long long tname;

	#define SU_CC_DECL_INT128( tname ) \
		typedef struct { signed long long hi,lo; } tname; \
		inline bool operator == ( const tname& u0, const tname& u1 ) \
		{	return ( u0.hi==u1.hi && u0.lo == u1.lo );	}

	#define SU_CC_DECL_UINT128( tname ) \
		typedef struct { unsigned long long hi,lo; } tname; \
		inline bool operator == ( const tname& u0, const tname& u1 ) \
		{	return ( u0.hi==u1.hi && u0.lo == u1.lo );	}

	#define	SU_ALIGNED_X( inType, inVar, inBA )

	#if !defined(SU_BREAK)
		#define	SU_BREAK()	/* __generic_break() */
	#endif

	#if !defined(SU_SIZET)
		#define SU_SIZET	unsigned int
	#endif

	#if !defined(SU_CC_PLACEMENT_NEW)
		#define SU_CC_PLACEMENT_NEW( inType, inAddr )
	#endif

	#if !defined(SU_CC_CALL_CTOR)
		template<typename T> T* SU_CC_CALL_CTOR( T* inAddr )				 { return 0; }
		template<typename T> T* SU_CC_CALL_CTOR( T* inAddr, const T& inRef ) { return 0; }
	#endif

	#if !defined(SU_CC_CALL_DTOR)
		template<typename T> void SU_CC_CALL_DTOR( T* inAddr ) { if(inAddr){ inAddr->~T(); } }
	#endif

	#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
	#define	SU_CC_OPEN_C		extern "C" {
	#define	SU_CC_CLOSE_C		}
	#else
	#define	SU_CC_OPEN_C
	#define	SU_CC_CLOSE_C
	#endif
#endif // SU_CC_GENERIC


#endif // SU_GENERIC



