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

		bool Init ( Pool* p, AnyList* o )
		{
			return true;
		}


		void Shut ( )
		{
			//
		}


		int Lua_getRegistry ( lua_State *L )
		{
			lua_pushvalue( L, LUA_REGISTRYINDEX );
			return 1;
		}


		int Lua_breakpoint ( lua_State* L )
		{
			pcstr reason = lua_tostring( L, 1 );
			LUAT_DBGWHERE(L);
			SU_BREAK();
			return 0;
		}


		int Lua_error ( lua_State* L )
		{
			int level = luaT_optint(L, 2, 1);
			lua_settop(L, 1);
			if( lua_isstring(L, 1) && level > 0)	// add extra information ?
			{
				luaT_where(L, level);
				lua_pushvalue(L, 1);
				lua_concat(L, 2);
			}
			return lua_error(L);
		}


		static void settabss (lua_State *L, const char *i, const char *v)
		{
			lua_pushstring(L, v);
			lua_setfield(L, -2, i);
		}


		static void settabsi (lua_State *L, const char *i, int v)
		{
			lua_pushinteger(L, v);
			lua_setfield(L, -2, i);
		}



		int Lua_getInfo ( lua_State *L )
		{
			lua_Debug ar;
			int arg;
			lua_State *L1 = luaT_getthread(L,arg);
			const char *options = luaT_optstring(L, arg+2, "flnSu");
			if (lua_isnumber(L, arg+1)) {
				if (!lua_getstack(L1, (int)lua_tointeger(L, arg+1), &ar)) {
					lua_pushnil(L);  /* level out of range */
					return 1;
				}
			}
			else if (lua_isfunction(L, arg+1)) {
				lua_pushfstring(L, ">%s", options);
				options = lua_tostring(L, -1);
				lua_pushvalue(L, arg+1);
				lua_xmove(L, L1, 1);
			}
			else
				return luaT_argerror(L, arg+1, "function or level expected");
			if (!lua_getinfo(L1, options, &ar))
				return luaT_argerror(L, arg+2, "invalid option");
			lua_createtable(L, 0, 2);
			if (util::Strfind(options, 'S')) {
				settabss(L, "source", ar.source);
				settabss(L, "short_src", ar.short_src);
				settabsi(L, "linedefined", ar.linedefined);
				settabsi(L, "lastlinedefined", ar.lastlinedefined);
				settabss(L, "what", ar.what);
			}
			if (util::Strfind(options, 'l'))
				settabsi(L, "currentline", ar.currentline);
			if (util::Strfind(options, 'u'))
				settabsi(L, "nups", ar.nups);
			if (util::Strfind(options, 'n')) {
				settabss(L, "name", ar.name);
				settabss(L, "namewhat", ar.namewhat);
			}
			if (util::Strfind(options, 'L'))
				treatstackoption(L, L1, "activelines");
			if (util::Strfind(options, 'f'))
				treatstackoption(L, L1, "func");
			return 1;  /* return table */
		}

		static void treatstackoption (lua_State *L, lua_State *L1, const char *fname)
		{
			if (L == L1) {
				lua_pushvalue(L, -2);
				lua_remove(L, -3);
			}
			else
				lua_xmove(L1, L, 1);
			lua_setfield(L, -2, fname);
		}


		int Lua_getLocal (lua_State *L)
		{
			int arg;
			lua_State *L1 = luaT_getthread(L,arg);

			lua_Debug ar;
			if( !lua_getstack(L1,luaT_checkint(L,arg+1),&ar) )  // out of range?
				return luaT_argerror(L, arg+1, "level out of range");

			pcstr name = lua_getlocal( L1, &ar, luaT_checkint(L,arg+2) );
			SU_REQUIRE_OR_0( name );

			lua_xmove(L1, L, 1);
			lua_pushstring(L, name);
			lua_pushvalue(L, -2);
			return 2;
		}


		int Lua_setLocal (lua_State *L)
		{
			int arg;
			lua_State *L1 = luaT_getthread(L,arg);
			lua_Debug ar;
			if (!lua_getstack(L1, luaT_checkint(L, arg+1), &ar))  /* out of range? */
				return luaT_argerror(L, arg+1, "level out of range");
			luaT_checkany(L, arg+3);
			lua_settop(L, arg+3);
			lua_xmove(L, L1, 1);
			lua_pushstring(L, lua_setlocal(L1, &ar, luaT_checkint(L, arg+2)));
			return 1;
		}


		int Lua_findLocal( lua_State *L )
		{
			int arg;
			lua_State *L1 = luaT_getthread(L,arg);

			lua_Debug ar;
			if( !lua_getstack(L1,luaT_checkint(L,arg+1),&ar) )  // out of range?
				return luaT_argerror(L, arg+1, "level out of range");

			pcstr name = luaT_checkstring(L,arg+2);
			for( int i=1 ;; i++ )
			{
				pcstr lname = lua_getlocal( L1, &ar, i );
				if( !lname )
					break;
				if( util::Strequals(lname,name) ) {
					lua_xmove( L1, L, 1 );
					return 1;
				}
			}

			return 0;
		}


		int Lua_getChunkLevel ( lua_State* L )
		{
			int level = luaT_fmain( L );
			lua_pushinteger( L, level );
			return 1;
		}


		int Lua_where ( lua_State* L )
		{
			luaT_where( L );
			return 1;
		}


		int Lua_traceback (lua_State *L)
		{
			return luaT_traceback( L );
		}


		int Lua_isBytecode ( lua_State* L )
		{
			return luaT_isbytecode( L, 1 );
		}


		int Lua_parseFile ( lua_State* L )
		{
			pcstr inpath = luaT_checklstring( L, 1, NULL );
			pcstr inname = luaT_optlstring( L, 2, NULL, NULL );

			SU_REQUIRE_OR_ZERO( inpath );
			SU_REQUIRE_OR_ZERO( globals::fs );
			SU_REQUIRE_OR_ZERO( globals::fs->IsFile(inpath) );

			uint64 sz;
			SU_REQUIRE_OR_ZERO( globals::fs->GetSize(inpath,sz) );
			SU_REQUIRE_OR_ZERO( sz > 0 );

			ObstackObject<> obs;

			IFileView* fv = globals::fs->OpenView( inpath, obs );
			SU_REQUIRE_OR_ZERO( fv );

			int ret = 0;

			void* bytes = fv->Map();
			if( bytes )
			{
				luaT_Chunk ck;
				ck.addr      = bytes;
				ck.bsize     = fv->GetSize();
				ck.bcapacity = fv->GetSize();

				if( luaT_pushcode(L,ck,inname?inname:inpath,inpath)==0 )
					ret = 1;
			}

			SafeRelease( fv );
			return ret;
		}


		int Lua_parseText ( lua_State* L )
		{
			pcstr intext = luaT_checklstring( L, 1, NULL );
			pcstr inname = luaT_optlstring( L, 2, NULL, NULL );

			SU_REQUIRE_OR_ZERO( intext );

			luaT_Chunk ck;
			ck.addr      = (void*)intext;
			ck.bsize     = util::Strlen(intext);
			ck.bcapacity = util::Strlen(intext);

			int ret = 0;

			if( luaT_pushcode(L,ck,inname,inname)==0 )
				ret = 1;

			return ret;
		}


		int Lua_dumpCode ( lua_State* L )
		{
			pcstr inpath = luaT_optlstring( L, 2, NULL, NULL );
			pcstr inname = luaT_optlstring( L, 1, NULL, NULL );

			ObstackObject<> obs;

			luaT_Chunk ck;
			ck.bsize     = 0;
			ck.bcapacity = 64*1024;
			ck.addr      = obs.Alloc( ck.bcapacity );

			if( ck.addr )
			{
				if( luaT_dumpcode(L,ck,1,true)==0 )
				{
					if( luaT_pushcode(L,ck,inname,inpath)==0 )
					{
						return 1;
					}
				}
			}

			return 0;
		}


		int Lua_loadCode ( lua_State* L )
		{
			luaT_loadcode( L, 1 );
			return 1;
		}


		int Lua_isNone ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			int t = lua_type(L,1);
			bool res = false;
			if( t==LUA_TNIL )
			{
				res = true;
			}
			else if( t==LUA_TLIGHTUSERDATA )
			{
				void* p = lua_touserdata(L,1);
				if( !p )	res = true;
			}
			else if( t==LUA_TNUMBER )
			{
				if( lua_tonumber(L,1) == 0. )
					res = true;
			}
			else if( t==LUA_TSTRING )
			{
				if( lua_objlen(L,1) == 0 )
					res = true;
			}
			else if( t==LUA_TTABLE )
			{
				lua_pushnil(L);
				if( lua_next(L,1)==0 )
					res = 1;
			}
			SU_REQUIRE_OR_0( res );
			lua_pushboolean(L,true);
			return 1;
		}


		int Lua_isNumber ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			int t = lua_type(L,1);
			if( t==LUA_TNUMBER )
			{
				lua_pushboolean(L,true);
				return 1;
			}
			return 0;
		}


		int Lua_isInteger ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			int t = lua_type(L,1);
			if( t==LUA_TNUMBER )
			{
				lua_Number n = lua_tonumber(L,1);
				if( lua_Number(int(n))==n )
				{
					lua_pushboolean(L,true);
					return 1;
				}
			}
			return 0;
		}


		int Lua_toInteger ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			int t = lua_type(L,1);
			if( t==LUA_TNUMBER )
			{
				lua_Number n = lua_tonumber(L,1);
				lua_pushinteger( L, lua_Integer(n) );
				return 1;
			}
			return 0;
		}


		int Lua_isNil ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			int t = lua_type(L,1);
			if( t==LUA_TNIL )
			{
				lua_pushboolean(L,true);
				return 1;
			}
			return 0;
		}


		int Lua_isString ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			int t = lua_type(L,1);
			if( t==LUA_TSTRING )
			{
				lua_pushboolean(L,true);
				return 1;
			}
			return 0;
		}


		int Lua_isBinary ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			int t = lua_type(L,1);
			if( t==LUA_TLIGHTUSERDATA || t==LUA_TUSERDATA )
			{
				lua_pushboolean(L,true);
				return 1;
			}
			return 0;
		}


		int Lua_isFunction ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			int t = lua_type(L,1);
			if( t==LUA_TFUNCTION )
			{
				lua_pushboolean(L,true);
				return 1;
			}
			return 0;
		}


		int Lua_isBoolean ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			int t = lua_type(L,1);
			if( t==LUA_TBOOLEAN )
			{
				lua_pushboolean(L,true);
				return 1;
			}
			return 0;
		}


		int Lua_isTable ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			SU_REQUIRE_OR_0( lua_type(L,1)==LUA_TTABLE );
			lua_pushboolean(L,true);
			return 1;
		}


		int Lua_toTable ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			if( lua_type(L,1)!=LUA_TTABLE )
			{
				lua_newtable(L);
				lua_pushvalue(L,1);
				lua_rawseti(L,-2,1);
			}
			return 1;
		}


		int Lua_sizeOf ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			int t = lua_type(L,1);
			int n = 0;
			if( t==LUA_TTABLE )
				n = luaT_tablelen(L,1);
			else
				n = lua_objlen(L,1);
			lua_pushinteger(L,n);
			return 1;
		}


		int Lua_copyTable ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			SU_REQUIRE_OR_0( lua_type(L,1)==LUA_TTABLE );
			SU_REQUIRE_OR_0( lua_type(L,2)==LUA_TTABLE );
			luaT_copytable( L, 1, 2 );		// dst, src
			return 0;
		}


		int Lua_dupTable ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			SU_REQUIRE_OR_0( lua_type(L,1)==LUA_TTABLE );
			luaT_duptable( L, 1 );
			return 1;
		}


		int Lua_clearTable ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			SU_REQUIRE_OR_0( lua_type(L,1)==LUA_TTABLE );
			luaT_cleartable( L, 1 );
			return 0;
		}


		int Lua_resizeTable ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			SU_REQUIRE_OR_0( lua_type(L,1)==LUA_TTABLE );
			int n = luaT_checkinteger(L,2);
			luaT_resizetable(L,1,n);
			return 0;
		}


		int Lua_countTable ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)==1 );
			SU_REQUIRE_OR_0( lua_type(L,1)==LUA_TTABLE );
			lua_pushinteger( L, luaT_tablelen(L,1) );
			return 1;
		}


		int Lua_isArray ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			SU_REQUIRE_OR_0( lua_type(L,1)==LUA_TTABLE );
			lua_pushboolean( L, luaT_ispurearray(L,1) );
			return 1;
		}


		int Lua_countArray ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)==1 );
			SU_REQUIRE_OR_0( lua_type(L,1)==LUA_TTABLE );
			lua_pushinteger( L, lua_objlen(L,1) );
			return 1;
		}


		int Lua_isEmpty ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			SU_REQUIRE_OR_0( lua_type(L,1)==LUA_TTABLE );
			lua_pushboolean( L, luaT_istableempty(L,1) );
			return 1;
		}


		int Lua_getKeys ( lua_State* L )
		{
			// { [k1]=v1, [k2]=v2, ... } => { [1]=k1, [2]=k2, ... }
			SU_REQUIRE_OR_0( lua_gettop(L)==1 );
			SU_REQUIRE_OR_0( lua_type(L,1)==LUA_TTABLE );
			int k=1;
			lua_newtable( L );
			lua_pushnil(L);
			while( lua_next(L,1) )
			{
				// t key val -- t key key
				lua_pop(L,1);
				lua_pushvalue(L,-1);
				// t key key -- t key
				lua_rawseti(L,-3,k++);
			}
			return 1;
		}


		int Lua_getValues ( lua_State* L )
		{
			// { [k1]=v1, [k2]=v2, ... } => { [1]=v1, [2]=v2, ... }
			SU_REQUIRE_OR_0( lua_gettop(L)==1 );
			SU_REQUIRE_OR_0( lua_type(L,1)==LUA_TTABLE );
			int k=1;
			lua_newtable( L );
			lua_pushnil(L);
			while( lua_next(L,1) )
			{
				// t key val -- t key
				lua_rawseti(L,-3,k++);
			}
			return 1;
		}


		int Lua_mapValues ( lua_State* L )
		{
			// { [k1]=v1, [k2]=v2, ... } => { [v1]=1, [v2]=1, ... }
			SU_REQUIRE_OR_0( lua_gettop(L)==1 );
			SU_REQUIRE_OR_0( lua_type(L,1)==LUA_TTABLE );
			int k=1;
			lua_newtable( L );
			lua_pushnil(L);
			while( lua_next(L,1) )
			{
				// t key val -- t key val 1
				lua_pushinteger( L, 1 );
				// t key val 1 -- t key
				lua_rawset(L,-4);
			}
			return 1;
		}

	};


}


