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


#include "su_lua.h"
#include <sufw/core/su_core.h>
using namespace su;


#define SWDATA_INET_TYPEID	0x67AD6990






namespace
{

	#define def_swenum( n )	{#n, inet::n}

	luaT_Enum swenums[] =
	{
		def_swenum( S_INVALID ),
		def_swenum( S_DISCONNECTED ),
		def_swenum( S_CONNECTING ),
		def_swenum( S_CONNECTED ),
		def_swenum( S_REQUESTING ),
		def_swenum( S_RECEIVING ),
		//
		def_swenum( HR_START_REQUESTING ),
		def_swenum( HR_START_RECEIVING ),
		def_swenum( HR_WRITE_FAILED ),
		def_swenum( HR_FAILED ),
		def_swenum( HR_SUCCESSED ),
		//
		def_swenum( O_VERBOSE ),
		def_swenum( O_HEADER ),
		def_swenum( O_WRITE_LENGTH ),
		def_swenum( O_WRITE_FUNC ),
		def_swenum( O_WRITE_UDATA ),
		def_swenum( O_WRITE_ADDR ),
		def_swenum( O_WRITE_CAPACITY ),
		def_swenum( O_READ_LENGTH ),
		def_swenum( O_READ_TYPE ),
		def_swenum( O_READ_FUNC ),
		def_swenum( O_READ_UDATA ),
		def_swenum( O_READ_ADDR ),
		def_swenum( O_HEADER_FUNC ),
		def_swenum( O_HEADER_UDATA ),
		def_swenum( O_HOOK_FUNC ),
		def_swenum( O_HOOK_UDATA ),
		def_swenum( O_LASTERROR ),
		def_swenum( O_URL ),
		def_swenum( O_PORT ),
		def_swenum( O_PROTOCOL ),
		def_swenum( O_USERNAME ),
		def_swenum( O_USERPWD ),
		def_swenum( O_HTTP_VERSION ),
		def_swenum( O_HTTP_HOST ),
		def_swenum( O_HTTP_COOKIE ),
		def_swenum( O_HTTP_PROPERTIES ),
		def_swenum( O_CRLF ),
		def_swenum( O_RANGE ),
		def_swenum( O_TIMEOUT_MS ),
		def_swenum( O_LOW_SPEED_LIMIT ),
		def_swenum( O_LOW_SPEED_TIME ),
		def_swenum( O_MAX_SEND_SPEED_LARGE ),
		def_swenum( O_MAX_RECV_SPEED_LARGE ),
		//
		{NULL,0}
	};



	struct SIDL_STUB_CLASSNAME
	{

		Pool*		pool;
		ISwitch*	sw;


		bool Init ( Pool* p, AnyList* o )
		{
			pool = p;
			sw = SIDL_IMPL_INTERFACE( ISwitch );

			if( o && o->isType("luastate",Any::Addr) )
			{
				lua_State* lstate   = (lua_State*)o->asAddr("luastate");
				pcstr      swfilter = o->asString("swfilter");
				luaT_swpushobject( lstate, sw, swfilter, swenums );
			}

			return true;
		}


		void Shut ( )
		{
			//
		}


		int Lua_decomposeUrl ( lua_State* L)
		{
			pcstr u = luaT_checkstring(L,1);
			
			inet::URL d;
			if( !inet::DecomposeURL(d,u) )
				return 0;

			lua_newtable(L);
			if( d.proto.size() )	{ lua_pushstring(L,d.proto); lua_setfield(L,-2,"proto"); }
			if( d.uname.size() )	{ lua_pushstring(L,d.uname); lua_setfield(L,-2,"uname"); }
			if( d.passwd.size() )	{ lua_pushstring(L,d.passwd); lua_setfield(L,-2,"passwd"); }
			if( d.ip.size() )		{ lua_pushstring(L,d.ip); lua_setfield(L,-2,"ip"); }
			if( d.hname.size() )	{ lua_pushstring(L,d.hname); lua_setfield(L,-2,"hname"); }
			if( d.port )			{ lua_pushinteger(L,d.port); lua_setfield(L,-2,"port"); }
			if( d.path.size() )		{ lua_pushstring(L,d.path); lua_setfield(L,-2,"path"); }
			if( d.query.size() )	{ lua_pushstring(L,d.query); lua_setfield(L,-2,"query"); }
			if( d.frag.size() )		{ lua_pushstring(L,d.frag); lua_setfield(L,-2,"frag"); }
			return 1;
		}


		int Lua_newConnection ( lua_State* L)
		{
			int co = inet::Create();
			SU_REQUIRE_OR_0( co >= 0 );
			luaT_swpushdata( L, SWDATA_INET_TYPEID, &co, sizeof(co), sw, "LuaCo_*" );
			return 1;
		}


		int LuaCo_setopt ( lua_State* L)
		{
			luaT_checkudata(L,1);
			int* co = luaT_swtodata<int>(L,1,SWDATA_INET_TYPEID );

			bool res = false;

			inet::Option opt = (inet::Option) luaT_checkinteger(L,2);

			if( luaT_isinteger(L,3) )
				res = inet::SetOpt( *co, (inet::Option)opt, Any(lua_tointeger(L,3)) );

			else if( lua_isstring(L,3) )
				res = inet::SetOpt( *co, (inet::Option)opt, Any((pcstr)lua_tostring(L,3)) );

			else
				luaT_argerror( L, 3, "Must be an option value, numerical or string" );

			if( !res )	return 0;
			lua_pushboolean(L,true);
			return 1;
		}


		int LuaCo_getopt ( lua_State* L)
		{
			luaT_checkudata(L,1);
			int* co = luaT_swtodata<int>(L,1,SWDATA_INET_TYPEID );

			inet::Option opt = (inet::Option) luaT_checkinteger(L,2);
			Any val = inet::GetOpt( *co, opt );

			if( val.isString() )
				lua_pushstring( L, val.asString() );

			else if( val.isInt() )
				lua_pushinteger( L, val.asInt() );

			else
				lua_pushnil( L );

			return 1;
		}


		int LuaCo_delopt ( lua_State* L)
		{
			luaT_checkudata(L,1);
			int* co = luaT_swtodata<int>(L,1,SWDATA_INET_TYPEID );

			inet::Option opt = (inet::Option) luaT_checkinteger(L,2);

			inet::DelOpt( *co, opt );
			return 0;
		}


		int LuaCo_getstate ( lua_State* L)
		{
			luaT_checkudata(L,1);
			int* co = luaT_swtodata<int>(L,1,SWDATA_INET_TYPEID );

			lua_pushinteger( L, (int)inet::GetState(*co) );
			return 1;
		}


		int LuaCo_connect ( lua_State* L)
		{
			luaT_checkudata(L,1);
			int* co = luaT_swtodata<int>(L,1,SWDATA_INET_TYPEID );

			lua_pushboolean( L, inet::Connect(*co) );
			return 1;
		}


		int LuaCo_disconnect ( lua_State* L)
		{
			luaT_checkudata(L,1);
			int* co = luaT_swtodata<int>(L,1,SWDATA_INET_TYPEID );

			inet::Disconnect(*co);
			return 0;
		}


		int LuaCo_receive ( lua_State* L)
		{
			luaT_checkudata(L,1);
			int* co = luaT_swtodata<int>(L,1,SWDATA_INET_TYPEID );

			byte buffer[16*1024];

			int rbs = inet::Receive(*co,buffer,sizeof(buffer));
			if( rbs > 0 )
			{
				void* ud = lua_newuserdata( L, rbs );
				util::Memcopy( ud, buffer, rbs );
				return 1;
			}

			return 0;
		}


		int LuaCo_send ( lua_State* L)
		{
			luaT_checkudata(L,1);
			int* co = luaT_swtodata<int>(L,1,SWDATA_INET_TYPEID );

			byte* baddr = NULL;
			uint  bsize = 0;

			if( lua_isstring(L,2) )
			{
				baddr = (byte*) lua_tostring(L,2);
				bsize = lua_objlen(L,2);
			}
			else if( lua_isuserdata(L,2) )
			{
				baddr = (byte*) lua_touserdata(L,2);
				bsize = lua_objlen(L,2);
			}

			if( baddr && bsize )
			{
				int obs = inet::Send(*co,baddr,bsize);
				if( obs )
				{
					lua_pushinteger(L,obs);
					return 1;
				}
			}

			return 0;
		}


		int LuaCo_perform ( lua_State* L)
		{
			luaT_checkudata(L,1);
			int* co = luaT_swtodata<int>(L,1,SWDATA_INET_TYPEID );

			lua_pushinteger( L, inet::Perform(*co) );
			return 1;
		}


		int LuaCo_gc ( lua_State* L)
		{
			luaT_checkudata(L,1);
			int* co = luaT_swtodata<int>(L,1,SWDATA_INET_TYPEID );
			inet::Release(*co);
			return 1;
		}

	};


}






