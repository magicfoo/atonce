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


#include "../su_base.h"
using namespace su;




IInterface* factory::CreateObject ( classid inMUID, Pool* inNestPool, Pool* inDataPool, AnyList* inOptions )
{
	addin::Builder* b = addin::Builder::Find( inMUID );
	SU_REQUIRE_OR_NULL( b );

	if( !inDataPool )
		inDataPool = inNestPool;

	return b->CreateObject( inNestPool, inDataPool, inOptions );
}





