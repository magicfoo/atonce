/*LIC-HDR********************************************************************
**
** Copyright (C) 2005-2011 AtOnce Technologies
**
** This file is part of the Sugar core-system framework.
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




#ifndef _su_collectioni_h_
#define _su_collectioni_h_




template <typename T> T* su::collection::Tag::load ( T& outtag ) const
{
	if( !addr )					return NULL;
	if( T::CODE != code )		return NULL;
	SU_ASSERT( map::base_tag::getcode(addr) == code );
	if( outtag.load(addr)==0 )	return NULL;
	return &outtag;
}




#endif // _su_collectioni_h_



