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


//
// Typically use this header as following :
//
// file myapp.cpp :
// ----------------
//
//	#include <sufw/core/base/su_startup.h>
//
//	void my_app()
//	{
//		su::app::Init();
//
//		for( ... )
//		{
// 			...
// 			su::app::Update();
// 			...
// 		}
//
//		su::app::Shut();
//	}
//
//
//	SU_Main( my_app )
//
//





#ifndef _su_startup_h_
#define _su_startup_h_


#if defined(SU_FRAMEWORK) || !defined(SU_APPLICATION)
	#error "Only Sugar Application can use Startup entry point !"
#endif


#define SU_PLATFORM_STARTUP
#include "su_settings.h"
#undef  SU_PLATFORM_STARTUP



#endif // _su_startup_h_



