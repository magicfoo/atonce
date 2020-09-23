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


#ifndef _su_config_h_
#define _su_config_h_



// I'm using Sugar framework !
#define	USING_SUGAR




// -- Build version ------------------------------------------------------------------------------------------
//
//
//										| fast-code | dbg4code | devkit | authoring | profiling | verbose | console |
//										|-----------|----------|--------|-----------|-----------|---------|---------|
// SU_DEBUG								|     -     |     O    |   O    |     O     |     O     |    O    |    O    |
// SU_DEBUGOPT							|     O     |     O    |   O    |     O     |     O     |    O    |    O    |
// SU_RELEASE							|     O     |     -    |   O    |     O     |     O     |    O    |    O    |
// SU_MASTER							|     O     |          |   -    |     -     |     O     |    O    |    O    |
// SU_FINAL								|     O     |          |        |           |     -     |    -    |    -    |
//
// SU_BUILD_FASTCODE	: optimised code
// SU_BUILD_DBGCODE		: enables debug specific code as outputs/assert
// SU_BUILD_DEVKIT		: supporting devkit extra functionalities
// SU_BUILD_AUTHORING	: enables authoring
// SU_BUILD_PROFILING	: enables profiling
// SU_BUILD_VERBOSE		: enables verbose applications
// SU_BUILD_CONSOLE		: enables console outputs

#if !defined(SU_DEBUG) && !defined(SU_DEBUGOPT) && !defined(SU_RELEASE) && !defined(SU_MASTER) && !defined(SU_FINAL)
	#if defined(_DEBUG) || defined(DEBUG)
		#define SU_DEBUG
	#elif defined(_DEBUGOPT) || defined(_DEBUG_OPT) || defined(DEBUGOPT) || defined(DEBUG_OPT)
		#define SU_DEBUGOPT
	#elif defined(_RELEASE) || defined(RELEASE) || defined(_NDEBUG) || defined(NDEBUG)
		#define SU_RELEASE
	#elif defined(_MASTER) || defined(MASTER)
		#define SU_MASTER
	#elif defined(_FINAL) || defined(FINAL)
		#define SU_FINAL
	#else
		#error "Unknown build version !"
	#endif
#endif

#if defined(SU_FINAL)
	#define	SU_BUILD_FASTCODE		1
	#define SU_BUILD_DBGCODE		0
	#define SU_BUILD_DEVKIT			0
	#define SU_BUILD_AUTHORING		0
	#define SU_BUILD_PROFILING		0
	#define SU_BUILD_VERBOSE		0
	#define SU_BUILD_CONSOLE		0
#elif defined(SU_MASTER)
	#define SU_BUILD_FASTCODE		1
	#define SU_BUILD_DBGCODE		0
	#define SU_BUILD_DEVKIT			0
	#define SU_BUILD_AUTHORING		0
	#define SU_BUILD_PROFILING		1
	#define SU_BUILD_VERBOSE		1
	#define SU_BUILD_CONSOLE		1
#elif defined(SU_RELEASE)
	#define SU_BUILD_FASTCODE		1
	#define SU_BUILD_DBGCODE		0
	#define SU_BUILD_DEVKIT			1
	#define SU_BUILD_AUTHORING		1
	#define SU_BUILD_PROFILING		1
	#define SU_BUILD_VERBOSE		1
	#define SU_BUILD_CONSOLE		1
#elif defined(SU_DEBUGOPT)
	#define SU_BUILD_FASTCODE		1
	#define SU_BUILD_DBGCODE		1
	#define SU_BUILD_DEVKIT			1
	#define SU_BUILD_AUTHORING		1
	#define SU_BUILD_PROFILING		1
	#define SU_BUILD_VERBOSE		1
	#define SU_BUILD_CONSOLE		1
#elif defined(SU_DEBUG)
	#define SU_BUILD_FASTCODE		0
	#define SU_BUILD_DBGCODE		1
	#define SU_BUILD_DEVKIT			1
	#define SU_BUILD_AUTHORING		1
	#define SU_BUILD_PROFILING		1
	#define SU_BUILD_VERBOSE		1
	#define SU_BUILD_CONSOLE		1
#endif

#if !defined(SU_BUILD_LEVEL)
	#if defined(SU_FINAL)
		#define	SU_BUILD_LEVEL		final
		#define	SU_BUILD_LEVEL_S	"final"
	#elif defined(SU_MASTER)
		#define	SU_BUILD_LEVEL		master
		#define	SU_BUILD_LEVEL_S	"master"
	#elif defined(SU_RELEASE)
		#define	SU_BUILD_LEVEL		release
		#define	SU_BUILD_LEVEL_S	"release"
	#elif defined(SU_DEBUGOPT)
		#define	SU_BUILD_LEVEL		debugopt
		#define	SU_BUILD_LEVEL_S	"debugopt"
	#elif defined(SU_DEBUG)
		#define	SU_BUILD_LEVEL		debug
		#define	SU_BUILD_LEVEL_S	"debug"
	#endif
#endif




// -- Code part ------------------------------------------------------------------------------------------
//
// SU_FRAMEWORK
// SU_APPLICATION
// SU_GENERIC
//

#if defined(SU_GENERIC)
	#undef	SU_FRAMEWORK
	#undef  SU_APPLICATION
#elif !defined(SU_FRAMEWORK) && !defined(SU_APPLICATION)
	#define SU_APPLICATION
#elif defined(SU_FRAMEWORK)
	#undef  SU_APPLICATION
#elif defined(SU_APPLICATION)
	#undef	SU_FRAMEWORK
#endif




// -- Platform part ------------------------------------------------------------------------------------------
//


#if defined(SU_GENERIC)
	#include <sufw/su_generic.h>
#else
	#define SU_PLATFORM_CONFIG
	#include "su_settings.h"
	#undef SU_PLATFORM_CONFIG
#endif

#if !defined(SU_HW)
	#error "Unknown platform hw !"
#endif

#if !defined(SU_HW_MSB) && !defined(SU_HW_LSB)
	#error "Unknown platform hw endian !"
#endif

#if !defined(SU_CC)
	#error "Unknown c/cpp compiler !"
#endif



#endif // _su_config_h_



