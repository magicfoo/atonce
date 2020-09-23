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



#define	POOL_NO_SHRINK




namespace
{

	struct SIDL_STUB_CLASSNAME;

	
	void* vm_realloc ( void* ud, void* ptr, size_t osize, size_t nsize );

	int	vm_lstate ( lua_State* );
	int vm_gcinfo ( lua_State* );
	int vm_collectgarbage ( lua_State* );
	int vm_new ( lua_State* );
	


	void getfunc ( lua_State *L, int opt)
	{
		if (lua_isfunction(L, 1))
			lua_pushvalue(L, 1);
		else
		{
			lua_Debug ar;
			int level = opt ? luaT_optint(L, 1, 1) : luaT_checkint(L, 1);
			luaT_argcheck(L, level >= 0, 1, "level must be non-negative");
			if (lua_getstack(L, level, &ar) == 0)
				luaT_argerror(L, 1, "invalid level");
			lua_getinfo(L, "f", &ar);
			if (lua_isnil(L, -1))
				luaT_error(L, "no function environment for tail call at level %d", level);
		}
	}



	int	vm_atpanic ( lua_State *L )
	{
		pcstr reason = luaT_getstring(L,1);
		LUAT_DBGWHERE( L );
		return 1;
	}


	int	vm_aterror ( lua_State *L )
	{
		luaT_traceback(L);
		pcstr traceback = luaT_getstring(L,-1);
		pcstr reason = luaT_getstring(L,1);

		SU_STDERR( "\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n" );
		SU_STDERR( "MLuaState error catched:\n" );
		SU_STDERR( traceback );
		SU_STDERR( "\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n" );
	//	LUAT_DBGWHERE( L );
	//	SU_BREAK();

		lua_settop(L,1);
		return 1;
	}


	int vm_next ( lua_State* L )
	{
		luaT_checktype(L, 1, LUA_TTABLE);
		lua_settop(L, 2);  /* create a 2nd argument if there isn't one */
		return lua_next(L,1) ? 2 : 0;
	}


	int vm_pairs_next ( lua_State *L )
	{
		return vm_next(L);
	}


	int vm_pairs ( lua_State *L )
	{
		luaT_checktype(L, 1, LUA_TTABLE);
		lua_pushvalue(L, lua_upvalueindex(1));  /* return generator, */
		lua_pushvalue(L, 1);  /* state, */
		lua_pushnil(L);  /* and initial value */
		return 3;
	}


	int vm_ipairs_next ( lua_State *L )
	{
		luaT_checktype(L, 1, LUA_TTABLE);
		int i = luaT_checkint(L, 2);
		i++;  /* next value */
		lua_pushinteger(L, i);
		lua_rawgeti(L, 1, i);
		return (lua_isnil(L, -1)) ? 0 : 2;
	}


	int vm_ipairs ( lua_State *L )
	{
		luaT_checktype(L, 1, LUA_TTABLE);
		lua_pushvalue(L, lua_upvalueindex(1));  /* return generator, */
		lua_pushvalue(L, 1);  /* state, */
		lua_pushinteger(L, 0);  /* and initial value */
		return 3;
	}


	int vm_iter_next ( lua_State* L )
	{
		SU_REQUIRE_OR_0( lua_istable(L,1) );
		lua_settop(L, 2);  /* create a 2nd argument if there isn't one */
		return lua_next(L,1) ? 2 : 0;
	}


	int vm_iter ( lua_State *L )
	{
		int t = lua_type(L,1);
		if( t!=LUA_TNIL && t!=LUA_TTABLE )
			luaT_typerror( L, 1, lua_typename(L,LUA_TTABLE) );
		lua_pushvalue(L, lua_upvalueindex(1));  /* return generator, */
		lua_pushvalue(L, 1);  /* state, */
		lua_pushnil(L);  /* and initial value */
		return 3;
	}


	int vm_iiter_next ( lua_State *L )
	{
		SU_REQUIRE_OR_0( lua_istable(L,1) );
		int i = luaT_checkint(L, 2);
		i++;  /* next value */
		lua_pushinteger(L, i);
		lua_rawgeti(L, 1, i);
		return (lua_isnil(L, -1)) ? 0 : 2;
	}


	int vm_iiter ( lua_State *L )
	{
		int t = lua_type(L,1);
		if( t!=LUA_TNIL && t!=LUA_TTABLE )
			luaT_typerror( L, 1, lua_typename(L,LUA_TTABLE) );
		lua_pushvalue(L, lua_upvalueindex(1));  /* return generator, */
		lua_pushvalue(L, 1);  /* state, */
		lua_pushinteger(L, 0);  /* and initial value */
		return 3;
	}


	int vm_type ( lua_State* L )
	{
		luaT_checkany(L, 1);
		lua_pushstring(L, luaT_typename(L, 1));
		return 1;
	}


	int vm_unpack (lua_State *L)
	{
		int i, e, n;
		luaT_checktype(L, 1, LUA_TTABLE);
		i = luaT_optint(L, 2, 1);
		e = luaT_opt(L, luaT_checkint, 3, luaT_getn(L, 1));
		n = e - i + 1;  /* number of elements */
		if (n <= 0) return 0;  /* empty range */
		luaT_checkstack(L, n, "table too big to unpack");
		for (; i<=e; i++)  /* push arg[i...e] */
			lua_rawgeti(L, 1, i);
		return n;
	}


	int vm_select (lua_State *L)
	{
		int n = lua_gettop(L);
		if (lua_type(L, 1) == LUA_TSTRING && *lua_tostring(L, 1) == '#') {
			lua_pushinteger(L, n-1);
			return 1;
		}
		else {
			int i = luaT_checkint(L, 1);
			if (i < 0) i = n + i;
			else if (i > n) i = n;
			luaT_argcheck(L, 1 <= i, 1, "index out of range");
			return n - i;
		}
	}


	int vm_getfenv ( lua_State*	L )
	{
		getfunc(L, 1);
		if (lua_iscfunction(L, -1))  // is a C function?
			lua_pushvalue(L, LUA_GLOBALSINDEX );  // return the thread's global env.
		else
			lua_getfenv(L, -1);
		return 1;
	}


	int vm_setfenv ( lua_State* L )
	{
		luaT_checktype(L, 2, LUA_TTABLE);
		getfunc(L, 0);
		lua_pushvalue(L, 2);
		if (lua_isnumber(L, 1) && lua_tonumber(L, 1) == 0)
		{
			// change environment of current thread
			lua_pushthread(L);
			lua_insert(L, -2);
			lua_setfenv(L, -2);
			return 0;
		}
		else if (lua_iscfunction(L, -2) || lua_setfenv(L, -2) == 0)
		{
			luaT_error(L, LUA_QL("setfenv") " cannot change environment of given object");
		}
		return 1;
	}


	int vm_getmetatable ( lua_State* L )
	{
		luaT_checkany(L, 1);
		if( !lua_getmetatable(L, 1) )
		{
			lua_pushnil(L);
			return 1;  // no metatable
		}
		luaT_getmetafield(L, 1, "__metatable");
		return 1;  // returns either __metatable field (if present) or metatable */
	}


	int vm_setmetatable ( lua_State* L )
	{
		int t = lua_type(L, 2);
		luaT_checktype(L, 1, LUA_TTABLE);
		luaT_argcheck( L, t==LUA_TNIL || t==LUA_TTABLE, 2, "nil or table expected" );
		if (luaT_getmetafield(L, 1, "__metatable"))
			luaT_error(L, "cannot change a protected metatable");
		lua_settop(L, 2);
		lua_setmetatable(L, 1);
		return 1;
	}


	int vm_rawget ( lua_State* L )
	{
		luaT_checktable( L, 1 );
		luaT_checkany( L, 2 );
		lua_settop( L, 2 );
		lua_rawget( L, 1 );
		return 1;
	}


	int vm_rawset ( lua_State* L )
	{
		luaT_checktable( L, 1 );
		luaT_checkany( L, 2 );
		luaT_checkany( L, 3 );
		lua_settop( L, 3 );
		lua_rawset( L, 1 );
		return 0;
	}


	int vm_yield ( lua_State* L )
	{
		return lua_yield( L, 0 );
	}


	int vm_resume ( lua_State* L )
	{
		return lua_resume( L, 0 );
	}


	void setglobal ( lua_State *L, pcstr name, lua_CFunction f )
	{
		lua_pushcfunction(L, f);
		lua_setglobal(L, name );
	}

	
	void setglobalaux ( lua_State *L, pcstr name, lua_CFunction f, lua_CFunction u )
	{
		lua_pushcfunction(L, u);
		lua_pushcclosure(L, f, 1);
		lua_setglobal(L, name );
	}


	void setglobalstub ( lua_State *L, pcstr name, lua_CFunction f, SIDL_STUB_CLASSNAME* s )
	{
		lua_pushlightuserdata( L, s );
		lua_pushcclosure(L, f, 1);
		lua_setglobal(L, name );
	}






	struct SIDL_STUB_CLASSNAME
	{

		Pool*						pool;

		lua_State*					lstate;
		Pool*						lpool;
		bool						loptimizecode;
		vector<lua_State*>			lcoroutines;


		bool Init ( Pool* p, AnyList* o )
		{
			pool = p;
			lstate = NULL;
			return true;
		}


		void Shut ( )
		{
			Unrealize();
		}


		bool Realize ( bool inOptimizeCode, Pool* inStatePool )
		{
			SU_REQUIRE_OR_FALSE( !IsRealized() );

			loptimizecode = inOptimizeCode;
			lpool = inStatePool ? inStatePool : pool->CreateSub( Pool::RM_Immediate );
			lcoroutines.relocate( lpool );

			lstate = lua_newstate( vm_realloc, this );
			if( !lstate )
			{
				Pool::Destroy( lpool );
				return false;
			}

			// check long integer support
			SU_COMPILE_TIME_ASSERT( sizeof(LUA_NUMBER) == sizeof(double) );
			lua_pushinteger( lstate, 0xFEDC0102 );
			SU_ASSERT( lua_tointeger(lstate,-1) == 0xFEDC0102 );
			lua_pop(lstate,1);

			lua_atpanic( lstate, vm_atpanic );

			// set global _G
			lua_pushvalue( lstate, LUA_GLOBALSINDEX );
			lua_setglobal( lstate, "_G" );

			setglobal( lstate, "next", vm_next );
			setglobal( lstate, "unpack", vm_unpack );
			setglobal( lstate, "select", vm_select );
			setglobal( lstate, "type", vm_type );
			setglobal( lstate, "getfenv", vm_getfenv );
			setglobal( lstate, "setfenv", vm_setfenv );
			setglobal( lstate, "getmetatable", vm_getmetatable );
			setglobal( lstate, "setmetatable", vm_setmetatable );
			setglobal( lstate, "rawget", vm_rawget );
			setglobal( lstate, "rawset", vm_rawset );
			setglobal( lstate, "yield", vm_yield );
			setglobal( lstate, "resume", vm_resume );
			
			setglobalstub( lstate, "lstate", vm_lstate, this );
			setglobalstub( lstate, "gcinfo", vm_gcinfo, this );
			setglobalstub( lstate, "collectgarbage", vm_collectgarbage, this );
			setglobalstub( lstate, "new", vm_new, this );

			setglobalaux( lstate, "pairs", vm_pairs, vm_pairs_next );
			setglobalaux( lstate, "ipairs", vm_ipairs, vm_ipairs_next );
			setglobalaux( lstate, "iter", vm_iter, vm_iter_next );
			setglobalaux( lstate, "iiter", vm_iiter, vm_iiter_next );

			// coroutines in registry
			lua_newtable( lstate );
			lua_setfield( lstate, LUA_REGISTRYINDEX, "coroutines" );

			// base commands
			ISwitch* sw = SIDL_IMPL_INTERFACE( ISwitch );
			SU_ASSERT( sw );

			int sp = lua_gettop( lstate );
			uint nb = luaT_swimport( lstate, LUA_GLOBALSINDEX, sw );
			lua_settop( lstate, sp );
			return true;
		}


		bool IsRealized ( )
		{
			return (lstate != NULL);
		}


		void Unrealize ( )
		{
			SU_REQUIRE_OR_RETURN( IsRealized() );

			lcoroutines.relocate( NULL );

			lua_close( lstate );
			lstate = NULL;

			Pool::Destroy( lpool );
		}


		void GarbageCollect ( )
		{
			SU_REQUIRE_OR_RETURN( IsRealized() );
			garbage_collect( lstate, LUA_GCCOLLECT, 0 );
		}


		lua_State* GetState ( )
		{
			SU_REQUIRE_OR_NULL( IsRealized() );
			return lstate;
		}


		Pool* GetStatePool ( )
		{
			SU_REQUIRE_OR_NULL( IsRealized() );
			return lpool;
		}


		void push_chunk_builtins ( lua_State* L, ILuaState::Chunk& inChunk )
		{
			// { file=path, name=name, main=true }
			lua_newtable(L);
			lua_pushstring(L,inChunk.path);
			lua_setfield(L,-2,"file");
			lua_pushstring(L,inChunk.name);
			lua_setfield(L,-2,"name");
			lua_pushboolean(L,true);
			lua_setfield(L,-2,"main");
		}


		bool resume_chunk( lua_State* L, ILuaState::Chunk& inChunk, ILuaState::Error* outError )
		{
			SU_ASSERT( L )
			int sp = lua_gettop( L );
			int lerr;

			luaT_Chunk lck;
			lck.addr	  = inChunk.baddr;
			lck.bsize	  = inChunk.bsize;
			lck.bcapacity = inChunk.bsize;
			luaT_pushcode( L, lck, inChunk.name, inChunk.path );

			lerr = luaT_loadcode(L,-1);
			if( lerr==0 )
			{
				push_chunk_builtins(L,inChunk);
				lerr = lua_resume(L,1);
				if( lerr==0 || lerr==LUA_YIELD )
				{
					if( outError )	{ outError->status = lerr==0?ILuaState::S_FINISHED:ILuaState::S_YIELDED; }
					if( outError )	{ outError->reason.clear(); }
					lua_settop( L, sp );
					return true;
				}
			}

			if( outError )
			{
				if     ( lerr == LUA_ERRRUN)		outError->status = ILuaState::S_ERRRUN;
				else if( lerr == LUA_ERRSYNTAX )	outError->status = ILuaState::S_ERRSYNTAX;
				else if( lerr == LUA_ERRMEM )		outError->status = ILuaState::S_ERRMEM;
				else if( lerr == LUA_ERRERR )		outError->status = ILuaState::S_ERRERR;
				else								outError->status = ILuaState::S_ERRCHUNK;
			}

			if( lua_isstring(L,-1) )
			{
				pcstr errmsg = lua_tostring( L, -1 );
				if( outError )
					outError->reason = errmsg;
				#if defined(SU_DEBUG)
				SU_STDERR( errmsg );
				#endif
			}

			lua_settop( L, sp );
			return false;
		}


		bool pcall_chunk( lua_State* L, ILuaState::Chunk& inChunk, ILuaState::Error* outError )
		{
			SU_ASSERT( L )
			int sp = lua_gettop( L );
			int lerr;

			luaT_Chunk lck;
			lck.addr	  = inChunk.baddr;
			lck.bsize	  = inChunk.bsize;
			lck.bcapacity = inChunk.bsize;
			luaT_pushcode( L, lck, inChunk.name, inChunk.path );

			lerr = luaT_loadcode(L,-1);
			if( lerr==0 )
			{
				lua_pushcfunction( L, vm_aterror );
				lua_pushvalue( L, -2 );
				push_chunk_builtins(L,inChunk);
				lerr = lua_pcall(L,1,0,-3);
				if( lerr==0 )
				{
					if( outError )	outError->status = ILuaState::S_FINISHED;
					if( outError )	outError->reason.clear();
					lua_settop( L, sp );
					return true;
				}
			}

			if( outError )
			{
				if     ( lerr == LUA_ERRRUN)		outError->status = ILuaState::S_ERRRUN;
				else if( lerr == LUA_ERRSYNTAX )	outError->status = ILuaState::S_ERRSYNTAX;
				else if( lerr == LUA_ERRMEM )		outError->status = ILuaState::S_ERRMEM;
				else if( lerr == LUA_ERRERR )		outError->status = ILuaState::S_ERRERR;
				else								outError->status = ILuaState::S_ERRCHUNK;
			}

			if( lua_isstring(L,-1) )
			{
				pcstr errmsg = lua_tostring( L, -1 );
				if( outError )
					outError->reason = errmsg;
				#if defined(SU_DEBUG)
			//	SU_STDERR( errmsg );
				#endif
			}

			lua_settop( L, sp );
			return false;
		}


		bool RunTask( ILuaState::Chunk& inChunk, ILuaState::Error* outError )
		{
			SU_REQUIRE_OR_FALSE( IsRealized() );
			return pcall_chunk( lstate, inChunk, outError );
		}


		void register_co ( int co, int thidx, pcstr name, pcstr path )
		{
			thidx = luaT_absidx( lstate, thidx );
			SU_ASSERT( lua_isthread(lstate,thidx) );
		
			lua_getfield( lstate, LUA_REGISTRYINDEX, "coroutines" );
			SU_ASSERT( lua_istable(lstate,-1) );
			//
			lua_newtable( lstate );
			// "thread"
			lua_pushvalue( lstate, thidx );
			lua_setfield( lstate, -2, "thread" );
			// "name"
			lua_pushstring( lstate, name );
			lua_setfield( lstate, -2, "name" );
			// "path"
			lua_pushstring( lstate, path );
			lua_setfield( lstate, -2, "path" );
			//
			lua_rawseti( lstate, -2, co );
			lua_pop( lstate, 1 );
		}


		void unregister_co ( int co )
		{
			lua_getfield( lstate, LUA_REGISTRYINDEX, "coroutines" );
			SU_ASSERT( lua_istable(lstate,-1) );
			lua_pushnil( lstate );
			lua_rawseti( lstate, -2, co );
			lua_pop( lstate, 1 );
		}


		int StartCoroutine ( ILuaState::Chunk& inChunk, ILuaState::Error* outError )
		{
			SU_REQUIRE_OR_FALSE( IsRealized() );

			// find a co index
			int co = -1;
			for( int i=0 ; i<lcoroutines.size() ; i++ )
			{
				if( lcoroutines[i]==NULL ) {
					co = i;
					break;
				}
			}
			if( co<0 )
			{
				co = lcoroutines.size();
				lcoroutines.push_back( NULL );
			}

			// co thread
			lua_State* lco = lua_newthread( lstate );
			SU_REQUIRE_OR_FALSE( lco );
			SU_ASSERT( lua_isthread(lstate,-1) );

			// register co
			register_co( co, -1, inChunk.name, inChunk.path );
			lua_pop( lstate, 1 );
			lcoroutines[co] = lco;

			if( resume_chunk(lco,inChunk,outError) )
			{
				return co;
			}
			else
			{
				lcoroutines[co] = NULL;
				unregister_co( co );
				return -1;
			}
		}


		ILuaState::Status GetCoroutineStatus ( int inCoid )
		{
			if( IsRealized() && inCoid>=0 && inCoid<lcoroutines.size() )
			{
				lua_State* lco = lcoroutines[inCoid];
				if( lco )
				{
					int stt = lua_status( lco );
					if( stt==0 )				return ILuaState::S_FINISHED;
					if( stt==LUA_YIELD )		return ILuaState::S_YIELDED;
					if( stt==LUA_ERRRUN )		return ILuaState::S_ERRRUN;
					if( stt==LUA_ERRSYNTAX )	return ILuaState::S_ERRSYNTAX;
					if( stt==LUA_ERRMEM )		return ILuaState::S_ERRMEM;
					if( stt==LUA_ERRERR )		return ILuaState::S_ERRERR;
				}
			}
			return ILuaState::S_UNDEFINED;
		}


		bool ResumeCoroutine ( int inCoid, ILuaState::Error* outError )
		{
			bool res = false;

			if( IsRealized() && inCoid>=0 && inCoid<lcoroutines.size() )
			{
				lua_State* lco = lcoroutines[inCoid];
				if( lco )
				{
					int stt = lua_status( lco );
					if( stt == LUA_YIELD )
					{
						stt = lua_resume( lco, 0 );
						res = true;
					}
				}
			}

			if( outError )	outError->status = GetCoroutineStatus( inCoid );
			if( outError )	outError->reason.clear();
			return res;
		}
		
		
		void RecycleCoroutine ( int inCoid )
		{
			if( IsRealized() && inCoid>=0 && inCoid<lcoroutines.size() )
			{
				unregister_co( inCoid );
				lcoroutines[inCoid] = NULL;
			}
		}


		int garbage_collect ( lua_State* L, int what, int data )
		{
			int res;

			res = lua_gc( L, what, data );
			if( lpool )
				lpool->RecycleDeep();

			return res;
		}


		void* realloc ( void* ptr, size_t osize, size_t nsize )
		{
			void* nptr = NULL;

			if( nsize==osize )
			{
				nptr = ptr;
			}
			else if( osize==0 && nsize )
			{
				nptr = lpool->Alloc( nsize );
				SU_ASSERT( nptr );
			}
			else if( nsize==0 && osize )
			{
				lpool->Free( ptr );
			}
		#if defined(POOL_NO_SHRINK)
			else if( nsize<osize )
			{
				nptr = ptr;
			}
		#endif
			else
			{
				nptr = lpool->Alloc( nsize );
				SU_ASSERT( nptr );

				if( nptr )
				{
					util::Memcopy( nptr, ptr, Min(osize,nsize) );
					lpool->Free( ptr );
				}
			}

			return nptr;
		}

	};


	void* vm_realloc ( void* ud, void* ptr, size_t osize, size_t nsize )
	{
		// invalid (Lua 5.1 requirement)
		SU_REQUIRE_OR_NULL( osize || nsize );

		SIDL_STUB_CLASSNAME* lobj = (SIDL_STUB_CLASSNAME*) ud;
		return lobj->realloc( ptr, osize, nsize );
	}


	int vm_lstate ( lua_State* L )
	{
		SIDL_STUB_CLASSNAME* stub = (SIDL_STUB_CLASSNAME*) lua_touserdata( L, lua_upvalueindex(1) );
		lua_pushlightuserdata( L, stub );
		return 1;
	}


	int vm_gcinfo ( lua_State* L )
	{
		lua_pushinteger(L, lua_getgccount(L));
		return 1;
	}


	int vm_collectgarbage ( lua_State* L )
	{
		SIDL_STUB_CLASSNAME* stub = (SIDL_STUB_CLASSNAME*) lua_touserdata( L, lua_upvalueindex(1) );

		static const char *const opts[] = {
			"stop",
			"restart",
			"collect",
			"count",
			"step",
			"setpause",
			"setstepmul",
			NULL
		};

		static const int optsnum[] = {
			LUA_GCSTOP,
			LUA_GCRESTART,
			LUA_GCCOLLECT,
			LUA_GCCOUNT,
			LUA_GCSTEP,
			LUA_GCSETPAUSE,
			LUA_GCSETSTEPMUL
		};

		int o = luaT_checkoption(L, 1, "collect", opts);
		int ex = luaT_optint(L, 2, 0);
		int res = stub->garbage_collect( L, optsnum[o], ex );
		switch (optsnum[o])
		{
		case LUA_GCCOUNT:
			{
				int b = lua_gc(L, LUA_GCCOUNTB, 0);
				lua_pushnumber(L, res + ((lua_Number)b/1024));
				return 1;
			}
		case LUA_GCSTEP :
			{
				lua_pushboolean(L, res);
				return 1;
			}
		default:
			{
				lua_pushnumber(L, res);
				return 1;
			}
		}
	}


	int vm_new ( lua_State* L )
	{
		SIDL_STUB_CLASSNAME* stub = (SIDL_STUB_CLASSNAME*) lua_touserdata( L, lua_upvalueindex(1) );

		pcstr  mname    = luaT_checkstring(L,1);
		pcstr  swfilter = luaT_optstring(L,2,NULL);
		uint32 muid     = util::hmac_classid( mname );
		SU_REQUIRE_OR_0( muid );

		int t0, t1;
		t0 = lua_gettop(L);

		AnyArray<2> options;
		options.set(0,"luastate",L);
		options.set(1,"swfilter",swfilter);
		IInterface* o = factory::CreateObject( muid, stub->lpool, stub->lpool, &options );
		SU_REQUIRE_OR_0( o );

		t1 = lua_gettop(L);
		SU_ASSERT( t1 >= t0 );

		if( t1 > t0 )
		{
			SafeRelease( o );
			return t1-t0;
		}
		else
		{
			// default binding to lua
			luaT_swpushobject( L, o, swfilter );
			SafeRelease( o );
			return 1;
		}
	}



}




