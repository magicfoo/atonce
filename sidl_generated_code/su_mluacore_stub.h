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



#include "su_lua.h"
using namespace su;




namespace
{


	struct SIDL_STUB_CLASSNAME
	{

		Pool*	pool;


		bool Init ( Pool* p, AnyList* o )
		{
			pool = p;
			return true;
		}


		void Shut ( )
		{
			//
		}


		int Lua_stdout ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L) );
			pcstr s = lua_tostring( L, 1 );
			SU_STDOUT( s );
			return 0;
		}


		int Lua_stderr ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L) );
			pcstr s = lua_tostring( L, 1 );
			SU_STDERR( s );
			return 0;
		}


		int Lua_loadProfile ( lua_State *L )
		{
			pcstr fname = luaT_checkstring(L,1);
			pcstr sname = luaT_checkstring(L,2);
			bool res = core::LoadProfile(fname,sname);
			lua_pushboolean(L,res);
			return 1;
		}


		int Lua_update ( lua_State *L )
		{
			core::Update();
			return 0;
		}


		int Lua_processEvent ( lua_State *L )
		{
			core::ProcessEvent();
			return 0;
		}


		int Lua_waitEvent ( lua_State *L )
		{
			core::WaitEvent();
			return 0;
		}


		int Lua_setWaitInterval ( lua_State *L )
		{
			float mstime = luaT_checknumber(L,1);
			core::SetWaitInterval(mstime);
			return 0;
		}


		int Lua_sleep ( lua_State *L )
		{
			float mstime = luaT_checknumber(L,1);
			core::Sleep(mstime);
			return 0;
		}


		int Lua_buildLevel ( lua_State *L )
		{
			pcstr bl = core::GetBuildLevel();
			lua_pushstring( L, bl );
			return 1;
		}


		int Lua_buildPlatform ( lua_State *L )
		{
			pcstr bp = core::GetBuildPlatform();
			lua_pushstring( L, bp );
			return 1;
		}


		int Lua_buildRevision ( lua_State *L )
		{
			pcstr br = core::GetBuildRevisionNumber();
			lua_pushstring( L, br );
			return 1;
		}


		int Lua_createObject ( lua_State* L )
		{
			pcstr  mname = luaT_checkstring(L,1);
			pcstr  swfilter = luaT_optstring(L,2,NULL);
			uint32 muid = util::hmac_classid( mname );
			SU_REQUIRE_OR_0( muid );

			AnyArray<1> options;
			options.set( 0, "luastate", L );

			IInterface* o = factory::CreateObject( muid, pool, pool, &options );
			SU_REQUIRE_OR_0( o );

			luaT_swpushobject( L, o, swfilter );
			SafeRelease( o );
			return 1;
		}

	};


}


