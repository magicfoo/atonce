/*LIC-HDR********************************************************************
**
** Copyright (C) 2005-2010 AtOnce Technologies
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



#include <sufw/directdriver/su_directdriver.h>
#include <sufw/lang/lua/su_lua.h>
using namespace su;


#define SWDATA_DD_TYPEID	0x87FE76EA



namespace
{

	struct SIDL_STUB_CLASSNAME
	{

		Pool*		pool;
		ISwitch*	sw;


		bool Init ( Pool* p, AnyList* o )
		{
			pool = p;
			sw = SIDL_IMPL_INTERFACE( ISwitch );
			return true;
		}


		void Shut ( )
		{
			//
		}


		int Lua_newDirectDraw ( lua_State* L )
		{
			if( lua_isstring(L,1) )
			{
				pcstr  mname = luaT_checkstring(L,1);
				uint32 muid = util::hmac_classid( mname );
				SU_REQUIRE_OR_0( muid );

				AnyArray<1> options;
				options.set( 0, "luastate", L );
				IDirectDraw* dd = factory::CreateObject<IDirectDraw>( muid, pool, pool, &options );
				SU_REQUIRE_OR_0( dd );

				luaT_swpushdata( L, SWDATA_DD_TYPEID, &dd, sizeof(void*), sw, "LuaDD_*" );
				return 1;
			}
			else
			{
				IDirectDraw* dd = luaT_swcheckobject<IDirectDraw>(L,1);
				SU_REQUIRE_OR_0( dd );

				luaT_swpushdata( L, SWDATA_DD_TYPEID, &dd, sizeof(void*), sw, "LuaDD_*" );
				dd->Retain();
				return 1;
			}
		}


		int LuaDD_realize ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			IDirectDraw** dd = luaT_swtodata<IDirectDraw*>( L, 1, SWDATA_DD_TYPEID );
			SU_ASSERT( dd );
			bool res = (*dd)->Realize();
			lua_pushboolean(L,res);
			return 1;
		}


		int LuaDD_createView ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			IDirectDraw** dd = luaT_swtodata<IDirectDraw*>( L, 1, SWDATA_DD_TYPEID );
			SU_ASSERT( dd );

			uint vid;
			uint cmdCreateView[] = 
			{
				SU_DD_WCREATE_1	( 200, 200 ),
				SU_DD_WCREATE_2	( NULL ),
				SU_DD_WCREATE_3	( &vid ),
				SU_DD_END		(		)
			};

			(*dd)->StartAndWaitCmdList( cmdCreateView );
			SU_REQUIRE_OR_FALSE( vid>=0 );

			lua_pushinteger(L,vid);
			return 1;
		}


		int LuaDD_unrealize ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			IDirectDraw** dd = luaT_swtodata<IDirectDraw*>( L, 1, SWDATA_DD_TYPEID );
			SU_ASSERT( dd );
			(*dd)->Unrealize();
			return 0;
		}


		int LuaDD_countDisplay ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			IDirectDraw** dd = luaT_swtodata<IDirectDraw*>( L, 1, SWDATA_DD_TYPEID );
			SU_ASSERT( dd );
			lua_pushinteger( L, (*dd)->CountDisplay() );
			return 1;
		}


		int LuaDD_countScreen ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			IDirectDraw** dd = luaT_swtodata<IDirectDraw*>( L, 1, SWDATA_DD_TYPEID );
			SU_ASSERT( dd );
			lua_pushinteger( L, (*dd)->CountScreen() );
			return 1;
		}


		int LuaDD_getShaderModel ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			IDirectDraw** dd = luaT_swtodata<IDirectDraw*>( L, 1, SWDATA_DD_TYPEID );
			SU_ASSERT( dd );
			lua_pushinteger( L, (*dd)->GetShaderModel() );
			return 0;
		}


		int LuaDD_gc ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			IDirectDraw** dd = luaT_swtodata<IDirectDraw*>( L, 1, SWDATA_DD_TYPEID );
			SU_ASSERT( dd );
			(*dd)->Release();
			return 0;
		}

	};


}


