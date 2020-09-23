/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2009 AtOnce Technologies
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




#ifndef _luatools_i_h_
#define _luatools_i_h_


#include <sufw/core/base/su_util.h>


template <typename T> inline
T* luaT_swtoobject ( lua_State *L, int idx )
{
	su::IInterface* itf = luaT_swtoobject(L,idx);
	if( itf )	return itf->GetInterface<T>();
	else		return NULL;
}


template <typename T> inline
T* luaT_swcheckobject ( lua_State *L, int idx )
{
	su::IInterface* itf = luaT_swcheckobject(L,idx);
	if( itf )	return itf->GetInterface<T>();
	else		return NULL;
}


template <typename T> inline
T* luaT_swtodata ( lua_State *L, int idx, uint32 intypeid )
{
	return (T*) luaT_swtodata( L, idx, intypeid );
}



template <typename T> inline
T* luaT_swdatactor ( lua_State *L, int idx, uint32 intypeid )
{
	T* o = (T*) luaT_swtodata( L, idx, intypeid );
	SU_REQUIRE_OR_NULL( o );
	SU_CC_CALL_CTOR( o );
	return o;
}



template <typename T> inline
T* luaT_swdatadtor ( lua_State *L, int idx, uint32 intypeid )
{
	T* o = (T*) luaT_swtodata( L, idx, intypeid );
	SU_REQUIRE_OR_NULL( o );

	// disables the swdata metatable to avoid a call to __gc()
	lua_pushnil( L );
	lua_setmetatable( L, idx );

	SU_CC_CALL_DTOR( o );
	return o;
}


template <typename T> inline
T* luaT_pushvector ( lua_State *L, const T& t, int count )
{
	T* v = (T*) luaT_newvector( L, sizeof(T), count );
	SU_ASSERT( v );
	su::util::Memncopy( v, &t, sizeof(t), count );
	return v;
}





#endif	// _luatools_i_h_



