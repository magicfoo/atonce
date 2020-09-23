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


#undef	su_imp
#undef	su_exp
#undef	SU_EXPORTING
#undef	SU_IMPORTING


#if defined(SU_HW_WINDOWS) && defined(SU_DLL)
#define	su_exp				__declspec( dllexport )
#define	su_imp				__declspec( dllimport )
#define SU_IMPORTING
#else
#define	su_imp
#define	su_exp
#endif


#undef	su_ie
#define	su_ie				su_imp

#undef	struct_ie
#define struct_ie			struct	su_ie

#undef	union_ie
#define union_ie			union	su_ie

#undef	static_ie
#define static_ie			static	su_ie



