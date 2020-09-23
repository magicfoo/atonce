/*LIC-HDR*****************************************************************
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



#include <sufw/core/su_core.h>
using namespace su;




SU_BEGIN_BUILDER( NotSupported )


pcstr GetMClassname ( )
{
	return "NotSupported";
}


su::classid GetMUID ( )
{
	return 0x53AF4F1EU;
}


SU_END_BUILDER( NotSupported )


