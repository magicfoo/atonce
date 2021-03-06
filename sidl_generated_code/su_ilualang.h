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


// !! This file has been automatically generated by the sugar sidl tool.
// !! Do not edit this file manually under risk of losing your changes.


#ifndef _su_ilualang_h_
#define _su_ilualang_h_




struct lua_State;


namespace su
{


	struct ILuaLang : public IInterface
	{
		SU_DECL_INTERFACE( IInterface, 0xcf5347d5u )


		virtual int    Lua_error            ( lua_State* L ) = 0;

		virtual int    Lua_traceback        ( lua_State* L ) = 0;

		virtual int    Lua_breakpoint       ( lua_State* L ) = 0;

		virtual int    Lua_where            ( lua_State* L ) = 0;

		virtual int    Lua_getInfo          ( lua_State* L ) = 0;

		virtual int    Lua_getRegistry      ( lua_State* L ) = 0;

		virtual int    Lua_getLocal         ( lua_State* L ) = 0;

		virtual int    Lua_findLocal        ( lua_State* L ) = 0;

		virtual int    Lua_setLocal         ( lua_State* L ) = 0;

		virtual int    Lua_getChunkLevel    ( lua_State* L ) = 0;

		virtual int    Lua_isBytecode       ( lua_State* L ) = 0;

		virtual int    Lua_parseFile        ( lua_State* L ) = 0;

		virtual int    Lua_parseText        ( lua_State* L ) = 0;

		virtual int    Lua_dumpCode         ( lua_State* L ) = 0;

		virtual int    Lua_loadCode         ( lua_State* L ) = 0;

		virtual int    Lua_sizeOf           ( lua_State* L ) = 0;

		virtual int    Lua_isNone           ( lua_State* L ) = 0;

		virtual int    Lua_isNumber         ( lua_State* L ) = 0;

		virtual int    Lua_isInteger        ( lua_State* L ) = 0;

		virtual int    Lua_toInteger        ( lua_State* L ) = 0;

		virtual int    Lua_isNil            ( lua_State* L ) = 0;

		virtual int    Lua_isString         ( lua_State* L ) = 0;

		virtual int    Lua_isBinary         ( lua_State* L ) = 0;

		virtual int    Lua_isFunction       ( lua_State* L ) = 0;

		virtual int    Lua_isTable          ( lua_State* L ) = 0;

		virtual int    Lua_toTable          ( lua_State* L ) = 0;

		virtual int    Lua_clearTable       ( lua_State* L ) = 0;

		virtual int    Lua_dupTable         ( lua_State* L ) = 0;

		virtual int    Lua_copyTable        ( lua_State* L ) = 0;

		virtual int    Lua_resizeTable      ( lua_State* L ) = 0;

		virtual int    Lua_countTable       ( lua_State* L ) = 0;

		virtual int    Lua_isArray          ( lua_State* L ) = 0;

		virtual int    Lua_countArray       ( lua_State* L ) = 0;

		virtual int    Lua_isEmpty          ( lua_State* L ) = 0;

		virtual int    Lua_getKeys          ( lua_State* L ) = 0;

		virtual int    Lua_getValues        ( lua_State* L ) = 0;

		virtual int    Lua_mapValues        ( lua_State* L ) = 0;
	};


}




#endif // _su_ilualang_h_


// !! This file has been automatically generated by the sugar sidl tool.
// !! Do not edit this file manually under risk of losing your changes.

