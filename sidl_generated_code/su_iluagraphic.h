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


#ifndef _su_iluagraphic_h_
#define _su_iluagraphic_h_




struct lua_State;


namespace su
{


	struct ILuaGraphic : public IInterface
	{
		SU_DECL_INTERFACE( IInterface, 0x416513acu )


		virtual int    Lua_newDirectDraw       ( lua_State* L ) = 0;

		virtual int    LuaDD_realize           ( lua_State* L ) = 0;

		virtual int    LuaDD_createView        ( lua_State* L ) = 0;

		virtual int    LuaDD_unrealize         ( lua_State* L ) = 0;

		virtual int    LuaDD_countDisplay      ( lua_State* L ) = 0;

		virtual int    LuaDD_countScreen       ( lua_State* L ) = 0;

		virtual int    LuaDD_getShaderModel    ( lua_State* L ) = 0;

		virtual int    LuaDD_gc                ( lua_State* L ) = 0;
	};


}




#endif // _su_iluagraphic_h_


// !! This file has been automatically generated by the sugar sidl tool.
// !! Do not edit this file manually under risk of losing your changes.
