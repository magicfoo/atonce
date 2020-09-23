/*LIC-HDR*****************************************************************
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



#ifndef _su_types_h_
#define _su_types_h_




#if !defined(SU_NO_BASETYPES)
typedef char								CBool;

#ifndef FALSE
#define FALSE								false
#endif

#ifndef TRUE
#define TRUE								true
#endif

#define	CFALSE								0
#define	CTRUE								1

#if defined(__cplusplus)
#define NULL								0
#else
#define NULL								((void*)0)
#endif

#if defined(__cplusplus)
#define NOT_NULL							1
#else
#define NOT_NULL							((void*)1)
#endif

typedef unsigned char						byte;
typedef unsigned char						uchar;
typedef unsigned short						ushort;
typedef unsigned int						uint;
typedef unsigned long						ulong;
typedef	void *								pvoid;
typedef	const char *						pcstr;
typedef char *								pstr;
typedef	const byte *						pcbyte;
typedef byte *								pbyte;

typedef signed   char						int8;
typedef unsigned char						uint8;
typedef signed   short						int16;
typedef unsigned short						uint16;
typedef signed   int						int32;
typedef unsigned int						uint32;
typedef unsigned int						mcc32;		// 'hex', see SU_MCC32()
#endif



#if !defined(SU_NO_EXTTYPES)
SU_CC_DECL_INT64( int64 )
SU_CC_DECL_UINT64( uint64 )
SU_CC_DECL_INT128( int128 )
SU_CC_DECL_UINT128( uint128 )
#endif


su_type( IInterface )
su_type( IConsole	)


namespace su
{
	typedef uint32		classid;
	typedef void*		classptr;
	typedef void*		classbase;
	typedef void*		pointer;
	const classid		invalid_uid = 0;
	const classptr		invalid_ptr = NULL;
}




#endif	// _su_types_h_



