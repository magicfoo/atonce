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

SU_CC_OPEN_C
#include <lobject.h>
#include <lstring.h>
#include <lundump.h>
SU_CC_CLOSE_C



#define toproto(L,i) \
			(clvalue(L->top+(i))->l.p)


using namespace su;



namespace
{

	void tag_error (lua_State *L, int narg, int tag)
	{
		luaT_typerror(L, narg, lua_typename(L, tag));
	}

	struct LoadS
	{
		const char *s;
		size_t size;
	};

	const char *getS ( lua_State* /*L*/, void *ud, size_t *size )
	{
		LoadS* ls = (LoadS*) ud;
		if (ls->size == 0) return NULL;
		*size = ls->size;
		ls->size = 0;
		return ls->s;
	}

}



void luaT_tostringbuff ( lua_State *L, int ti, char* outbuff )
{
	if( !outbuff )
		return;

	luaT_checkany( L, ti );

	// is there a metafield?
	if( luaT_callmeta(L,ti,"__tostring") )
	{
		luaT_tostringbuff( L, 1, outbuff );
		lua_pop( L, 1 );
		return;
	}

	switch( lua_type(L,ti) )
	{
		case LUA_TNUMBER:
			util::Strformat( outbuff, (float)lua_tonumber(L,ti) );
			break;
		case LUA_TSTRING:
			util::Strcopy( outbuff, lua_tostring(L,ti) );
			break;
		case LUA_TBOOLEAN:
			util::Strcopy( outbuff, (lua_toboolean(L,ti) ? "true" : "false") );
			break;
		case LUA_TNIL:
			util::Strcopy( outbuff, "<<nil>>" );
			break;
		default:
			util::Strformat( outbuff, "%s: %p", luaT_typename(L,ti), lua_topointer(L,ti) );
			break;
	}
}



pcstr luaT_getstring ( lua_State *L, int ti )
{
	if( luaT_istype(L,ti,LUA_TSTRING) )
		return lua_tostring(L,ti);
	else
		return NULL;
}



void luaT_where (lua_State *L, int level)
{
	lua_Debug ar;
	if (lua_getstack(L, level, &ar))
	{
		lua_getinfo(L, "Sl", &ar);
		if( ar.currentline > 0 )
		{
			lua_pushfstring( L, "%s(%d) : ", ar.short_src, ar.currentline );
			return;
		}
	}
	lua_pushliteral(L, "");
}


int luaT_error (lua_State *L, const char *fmt, ... )
{
  va_list argp;
  va_start(argp, fmt);
  luaT_where(L, 1);
  lua_pushvfstring(L, fmt, argp);
  va_end(argp);
  lua_concat(L, 2);
  return lua_error(L);
}



int luaT_argerror ( lua_State *L, int narg, const char *extramsg )
{
  lua_Debug ar;
  if (!lua_getstack(L, 0, &ar))  /* no stack frame? */
    return luaT_error(L, "bad argument #%d (%s)", narg, extramsg);
  lua_getinfo(L, "n", &ar);
  if (util::Strcompare(ar.namewhat, "method") == 0) {
    narg--;  /* do not count `self' */
    if (narg == 0)  /* error is in the self argument itself? */
      return luaT_error(L, "calling " LUA_QS " on bad self (%s)",
                           ar.name, extramsg);
  }
  if (ar.name == NULL)
    ar.name = "?";
  return luaT_error(L, "bad argument #%d to " LUA_QS " (%s)",
                        narg, ar.name, extramsg);
}



int luaT_typerror (lua_State *L, int narg, const char *tname)
{
  const char *msg = lua_pushfstring(L, "%s expected, got %s", tname, luaT_typename(L, narg) );
  return luaT_argerror(L, narg, msg);
}



int luaT_checkoption (	lua_State *L, int narg, const char *def, const char *const lst[])
{
  const char *name = (def) ? luaT_optstring(L, narg, def) :
                             luaT_checkstring(L, narg);
  int i;
  for (i=0; lst[i]; i++)
	  if (util::Strcompare(lst[i], name) == 0)
      return i;
  return luaT_argerror(L, narg, lua_pushfstring(L, "invalid option " LUA_QS, name));
}


void luaT_checkstack (lua_State *L, int space, const char *mes)
{
  if (!lua_checkstack(L, space))
    luaT_error(L, "stack overflow (%s)", mes);
}


void luaT_checktype (lua_State *L, int narg, int t)
{
  if (lua_type(L, narg) != t)
    tag_error(L, narg, t);
}


void luaT_checkany (lua_State *L, int narg)
{
  if (lua_type(L, narg) == LUA_TNONE)
    luaT_argerror(L, narg, "value expected");
}


const char *luaT_checklstring (lua_State *L, int narg, size_t *len)
{
	if( !lua_isstring(L,narg) )
		tag_error(L, narg, LUA_TSTRING);
	return lua_tolstring(L, narg, len);
}


const char *luaT_optlstring (lua_State *L, int narg, const char *def, size_t *len)
{
	if( lua_isstring(L,narg) )
		return lua_tolstring(L, narg, len);
	else
		return def;
}


lua_Number luaT_checknumber (lua_State *L, int narg)
{
  lua_Number d = lua_tonumber(L, narg);
  if (d == 0 && !lua_isnumber(L, narg))  /* avoid extra test when d is not 0 */
    tag_error(L, narg, LUA_TNUMBER);
  return d;
}


lua_Number luaT_optnumber (lua_State *L, int narg, lua_Number def)
{
  return luaT_opt(L, luaT_checknumber, narg, def);
}


lua_Integer luaT_checkinteger (lua_State *L, int narg)
{
	if( !lua_isnumber(L,narg) )
		tag_error(L, narg, LUA_TNUMBER);
	lua_Integer d = lua_tointeger(L, narg);
	lua_Number dd = (lua_Number)d;
	lua_Number n = lua_tonumber(L, narg);
	if( n!=dd )
		tag_error(L, narg, LUA_TNUMBER);
  return d;
}


bool luaT_checkboolean (lua_State *L, int narg)
{
	if( !lua_type(L,narg)==LUA_TBOOLEAN)
		tag_error(L, narg, LUA_TBOOLEAN);
	return lua_toboolean(L,narg) != 0;
}


lua_Integer luaT_optinteger ( lua_State *L, int narg, lua_Integer def )
{
	return luaT_opt( L, luaT_checkinteger, narg, def );
}


bool luaT_optboolean ( lua_State *L, int narg, bool def )
{
	return luaT_opt( L, lua_toboolean, narg, int(def) ) != 0;
}


int luaT_getmetafield ( lua_State *L, int obj, const char *event )
{
  if (!lua_getmetatable(L, obj))  /* no metatable? */
    return 0;
  lua_pushstring(L, event);
  lua_rawget(L, -2);
  if (lua_isnil(L, -1)) {
    lua_pop(L, 2);  /* remove metatable and metafield */
    return 0;
  }
  else {
    lua_remove(L, -2);  /* remove only metatable */
    return 1;
  }
}


bool luaT_hasfield ( lua_State *L, int obj, const char *k )
{
	lua_getfield(L,obj,k);
	bool res = !lua_isnil(L,-1);
	lua_pop(L,1);
	return res;
}


int luaT_callmeta (lua_State *L, int obj, const char *event)
{
  obj = luaT_absidx(L, obj);
  if (!luaT_getmetafield(L, obj, event))  /* no metafield? */
    return 0;
  lua_pushvalue(L, obj);
  lua_call(L, 1, 1);
  return 1;
}


void luaT_register ( lua_State *L, int ti, const luaT_Reg *l )
{
	SU_ASSERT( lua_istable(L,ti) );
	while( l->func )
	{
		lua_pushcclosure( L, l->func, 0 );
		lua_setfield( L, ti, l->name);
		l++;
	}
}




int luaT_loadbuffer (lua_State *L, const char *buff, size_t size, const char *name )
{
  LoadS ls;
  ls.s = buff;
  ls.size = size;
  return lua_load(L, getS, &ls, name);
}


int luaT_loadstring ( lua_State *L, const char *s )
{
	return luaT_loadbuffer(L, s, util::Strlen(s), s);
}


void luaT_resizetable ( lua_State *L, int ti, int n )
{
	SU_ASSERT( lua_istable(L,ti) );

	int t = luaT_absidx(L,ti);
	int i = 1;

	lua_pushnil(L);
	while( lua_next(L,t) )
	{
		// key -- key val
		// clear the key ?
		if( n <= 0 )
		{
			// key val -- key key nil
			lua_pop(L,1);
			lua_pushvalue(L,-1);
			lua_pushnil(L);
			// key key nil -- key
			lua_settable(L,t);
		}
		else
		{
			// key val -- key
			lua_pop(L,1);
		}
		i++;
		n--;
	}

	// append 0 0 0 ...
	while( n > 0 )
	{
		lua_pushinteger( L, i );
		lua_pushinteger( L, 0 );
		lua_settable(L,t);
		i++;
		n--;
	}
}


void luaT_duptable ( lua_State *L, int ti )
{
	SU_ASSERT( lua_istable(L,ti) );
	int t = luaT_absidx(L,ti);

	lua_newtable(L);

	lua_pushnil(L);
	while( lua_next(L,t) )
	{
		// t key val -- t key key val
		lua_pushvalue(L,-2);
		lua_insert(L,-2);
		// t key key val -- t key
		lua_settable(L,-4);
	}
}


void luaT_copytable ( lua_State *L, int dsti, int srci )
{
	SU_ASSERT( lua_istable(L,dsti) );
	SU_ASSERT( lua_istable(L,srci) );
	int di = luaT_absidx(L,dsti);
	int si = luaT_absidx(L,srci);

	luaT_cleartable( L, di );

	lua_pushnil(L);
	while( lua_next(L,si) )
	{
		// key val -- key key val
		lua_pushvalue(L,-2);
		lua_insert(L,-2);
		// key key val -- key
		lua_settable(L,di);
	}
}


void luaT_cleartable ( lua_State *L, int ti )
{
	SU_ASSERT( lua_istable(L,ti) );
	int t = luaT_absidx(L,ti);
	lua_pushnil(L);
	while( lua_next(L,t) )
	{
		// key val -- key key nil
		lua_pop(L,1);
		lua_pushvalue(L,-1);
		lua_pushnil(L);
		// key key nil -- key
		lua_settable(L,t);
	}
}


bool luaT_isinteger ( lua_State *L, int ti )
{
  if( !lua_isnumber(L,ti) )
	  return false;
  lua_Integer d = lua_tointeger(L,ti);
  lua_Number dd = (lua_Number)d;
  lua_Number n = lua_tonumber(L,ti);
  return (n==dd);
}


bool luaT_isscalar ( lua_State *L, int ti )
{
	int t = lua_type( L, ti );
	if( t == LUA_TBOOLEAN )		return true;
	if( t == LUA_TNUMBER )		return true;
	if( t == LUA_TSTRING )		return true;
	return false;
}


bool luaT_istableempty ( lua_State *L, int ti )
{
	SU_ASSERT( lua_type(L,ti) == LUA_TTABLE );
	int t = luaT_absidx( L, ti );
	int l = lua_objlen( L, t );
	if( l == 0 ) {
		lua_pushnil( L );
		if( lua_next(L,t) == 0 )
			return true;
		lua_pop( L, 2 );
	}
	return false;
}


int luaT_tablelen ( lua_State *L, int ti )
{
	SU_ASSERT( lua_type(L,ti) == LUA_TTABLE );

	int t = luaT_absidx(L,ti);
	SU_ASSERT( t>0 );

	int n = 0;

	lua_pushnil(L);
	while( lua_next(L,t) )
	{
		// key val -- key
		lua_pop(L,1);
		n++;
	}

	return n;
}


bool luaT_ispurearray ( lua_State *L, int ti )
{
	SU_ASSERT( lua_type(L,ti) == LUA_TTABLE );
	int t = luaT_absidx( L, ti );
	int l = lua_objlen( L, t );
	if( l==0 )
	{
		// empty ?
		lua_pushnil( L );
		if( lua_next(L,t) == 0 )
			return true;
		lua_pop( L, 2 );
		return false;
	}
	else
	{
		// check integer key
		lua_pushinteger( L, 1 );
		lua_gettable( L, t );
		if( !lua_isnil(L,-1) )
		{
			lua_pushinteger( L, l );
			lua_gettable( L, t );
			if( !lua_isnil(L,-1) )
			{
				lua_pop( L, 2 );
				return true;
			}
			lua_pop( L, 2 );
		}
		lua_pop( L, 1 );
	}
	return false;
}




int luaT_fmain ( lua_State *L, int from_lvl )
{
	lua_Debug ar;
	for( ;; from_lvl++ )
	{
		int r;

		r = lua_getstack( L, from_lvl, &ar );
		if( !r ) return -1;

		r = lua_getinfo( L, "S", &ar );
		if( !r ) return -1;

		if( util::Strcompare(ar.what,"main")==0 )
		{
			lua_pushstring( L, ar.source );
			r = lua_getinfo( L, "f", &ar );
			if( !r ) return -1;
			SU_ASSERT( lua_isfunction(L,-1) );
			return from_lvl;
		}
	}
}




int luaT_fmain_env ( lua_State *L, int from_lvl )
{
	int lvl = luaT_fmain( L, from_lvl );
	SU_ASSERT( lvl >= 0 );
	if( lvl < 0 )
	{
		return 0;
	}
	else
	{
		lua_remove( L, -2 );
		SU_ASSERT( lua_isfunction(L,-1) );
		lua_getfenv( L, -1 );
		lua_remove( L, -2 );
		SU_ASSERT( lua_istable(L,-1) );
		return 0;
	}
}




int luaT_dbgwhere ( lua_State* L, int lvl, luaT_DbgWhere* d, int count )
{
	if( d && count )
	{
		util::Memset( d, 0, sizeof(luaT_DbgWhere)*count );
	}

	lua_Debug ar;
	int i;
	for( i=0 ; d && i<count ; lvl++,i++ )
	{
		int r;
		r = lua_getstack( L, lvl, &ar );
		if( !r ) return i;

		r = lua_getinfo( L, "Sln", &ar );
		if( !r ) return i;

		int len;
		if( ar.source )
		{
			len = util::Strlen( ar.source );
			util::Strcopy( d[i].source, ar.source, Min(len,510) );
		}
		if( ar.name )
		{
			len = util::Strlen( ar.name );
			util::Strcopy( d[i].fname, ar.name, Min(len,510) );
		}

		d[i].lineno = ar.currentline;

		d[i].linedefined = ar.linedefined;
		d[i].lastlinedefined = ar.lastlinedefined;
		if( *ar.what == 'm' )
			d[i].what = luaT_DbgWhere::CALLT_M;
		else if( *ar.what == 'C' || *ar.what == 't' )
			d[i].what = luaT_DbgWhere::CALLT_C;
		else
			d[i].what = luaT_DbgWhere::CALLT_L;
	}

	return i;
}


int luaT_buildpathtable ( lua_State* L, int ti, pcstr inpath )
{
	int sp = lua_gettop( L );

	pcstr pcur = inpath;
	for( ;; )
	{
		pcstr pnext = util::Strfind( pcur, '.' );

		int len = pnext ? pnext-pcur : util::Strlen(pcur);
		if( len<2 || len>120 )
		{
			lua_settop( L, sp );
			return -1;
		}

		char k[128];
		util::Strcopy( k, pcur, len );
		k[len] = 0;

		lua_getfield( L, ti, k );
		if( lua_isnil(L,-1) )
		{
			lua_newtable( L );
			lua_pushvalue( L, -1 );
			lua_setfield( L, ti, k );
		}
		else if( !lua_istable(L,-1) )
		{
			lua_settop( L, sp );
			return -1;
		}

		if( ti > sp )
			lua_remove( L, ti );

		ti = lua_gettop( L );

		if( !pnext )
			break;

		pcur = pnext+1;
	}

	return ti;
}



lua_State* luaT_getthread ( lua_State *L, int& arg )
{
	if( lua_isthread(L,1) )
	{
		arg = 1;
		return lua_tothread(L,1);
	}
	else
	{
		arg = 0;
		return L;
	}
}



#define LEVELS1	12	/* size of the first part of the stack */
#define LEVELS2	10	/* size of the second part of the stack */

int luaT_traceback ( lua_State *L )
{
	int level;
	int firstpart = 1;  /* still before eventual `...' */
	int arg;
	lua_State *L1 = luaT_getthread(L,arg);
	lua_Debug ar;
	if (lua_isnumber(L, arg+2)) {
		level = (int)lua_tointeger(L, arg+2);
		lua_pop(L, 1);
	}
	else
		level = (L == L1) ? 1 : 0;  /* level 0 may be this own function */
	if (lua_gettop(L) == arg)
		lua_pushliteral(L, "");
	else if (!lua_isstring(L, arg+1)) return 1;  /* message is not a string */
	else lua_pushliteral(L, "\n");
	lua_pushliteral(L, "stack traceback:");
	while (lua_getstack(L1, level++, &ar)) {
		if (level > LEVELS1 && firstpart) {
			/* no more than `LEVELS2' more levels? */
			if (!lua_getstack(L1, level+LEVELS2, &ar))
				level--;  /* keep going */
			else {
				lua_pushliteral(L, "\n\t...");  /* too many levels */
				while (lua_getstack(L1, level+LEVELS2, &ar))  /* find last levels */
					level++;
			}
			firstpart = 0;
			continue;
		}
		lua_pushliteral(L, "\n\t");
		lua_getinfo(L1, "Snl", &ar);
		lua_pushfstring(L, "%s(%d):", ar.short_src, ar.currentline );
		if (*ar.namewhat != '\0')  /* is there a name? */
			lua_pushfstring(L, " in function " LUA_QS, ar.name);
		else {
			if (*ar.what == 'm')  /* main? */
				lua_pushfstring(L, " in main chunk");
			else if (*ar.what == 'C' || *ar.what == 't')
				lua_pushliteral(L, " ?");  /* C function or tail call */
			else
				lua_pushfstring(L, " in function <%s(%d)>",
				ar.short_src, ar.linedefined);
		}
		lua_concat(L, lua_gettop(L) - arg);
	}
	lua_concat(L, lua_gettop(L) - arg);
	return 1;
}








namespace
{

	int lua_writer ( lua_State* L, const void* p, size_t size, void* u )
	{
		UNUSED( L );

		luaT_Chunk* ck = (luaT_Chunk*) u;

		if( size )
		{
			if( ck->bsize >= 0 )
			{
				SU_ASSERT( ck->bsize <= ck->bcapacity );
				if( (ck->bsize+int(size)) <= (ck->bcapacity) )
				{
					util::Memcopy( ((char*)ck->addr)+ck->bsize, p, size );
					ck->bsize += size;
				}
				else
				{
					// need more memory !
					ck->bsize = -1;
				}
			}
		}

		return 0;
	}

}



bool luaT_isbytecode ( luaT_Chunk& inc )
{
	SU_REQUIRE_OR_FALSE( inc.addr );
	SU_REQUIRE_OR_FALSE( inc.bcapacity );
	SU_REQUIRE_OR_FALSE( inc.bsize >= LUAC_HEADERSIZE );

	char h[LUAC_HEADERSIZE];
	luaU_header(h);

	return (util::Memcompare(h,inc.addr,LUAC_HEADERSIZE)==0);
}



bool luaT_isbytecode ( lua_State* L, int idx )
{
	SU_REQUIRE_OR_FALSE( L );

	if( lua_isfunction(L,idx) )
	{
		return true;
	}
	else if( lua_isuserdata(L,idx) )
	{
		void* ud = lua_touserdata( L, idx );

		lua_getfield( L, idx, "bsize" );
		if( !lua_isnumber(L,-1) ) {
			lua_pop(L,1);
			return false;
		}

		int bs = lua_tointeger(L,-1);
		lua_pop(L,1);
		SU_ASSERT( bs > 0 );

		luaT_Chunk ck;
		ck.addr = ud;
		ck.bsize = bs;
		ck.bcapacity = bs;

		return luaT_isbytecode( ck );
	}
	else
	{
		return false;
	}
}



int luaT_pushcode ( lua_State* L, luaT_Chunk& inc, const char* inname, const char* inpath )
{
	SU_REQUIRE_OR_RETURN_V( inc.addr, -1 );
	SU_REQUIRE_OR_RETURN_V( inc.bcapacity, -1 );
	SU_REQUIRE_OR_RETURN_V( inc.bsize, -1 );

	// builtins support
	const char builtins_bytes[] = "local __builtins__ = ...;";
	int n = sizeof(builtins_bytes) - 1;

	// chunk data containing raw data
	// ( -- ud )
	byte* ud = (byte*) lua_newuserdata( L, n+inc.bsize );
	util::Memcopy( ud+0, builtins_bytes, n );
	util::Memcopy( ud+n, inc.addr, inc.bsize );

	// create meta holding extra informations
	// ( ud -- mt ud )
	lua_newtable(L);
	int mt = lua_gettop( L );

	// __index table in metatable
	// ( mt ud -- xt mt ud )
	lua_newtable( L );
	int xt = lua_gettop( L );
	lua_pushvalue( L, xt );
	lua_setfield( L, mt, "__index" );

	// store bsize
	// ( xt mt ud -- xt mt ud )
	lua_pushinteger( L, inc.bsize );
	lua_setfield( L, xt, "bsize" );

	// store name
	// ( xt mt ud -- xt mt ud )
	lua_pushstring( L, inname );
	lua_setfield( L, xt, "name" );

	// store path
	// ( xt mt ud -- xt mt ud )
	lua_pushstring( L, inpath );
	lua_setfield( L, xt, "path" );

	// set meta to object
	// ( xt mt ud -- ud )
	lua_pop( L, 1 );
	lua_setmetatable( L, -2 );
	return 0;
}



int luaT_dumpcode ( lua_State* L, luaT_Chunk& outc, int idx, bool instrip )
{
	SU_REQUIRE_OR_RETURN_V( outc.addr, -1 );
	SU_REQUIRE_OR_RETURN_V( outc.bcapacity, -2 );

	if( lua_isfunction(L,idx) )
	{
		const Proto* f = toproto( L, idx );
		SU_REQUIRE_OR_RETURN_V( f, -3 );

		outc.bsize = 0;
		int status = luaU_dump( L, f, lua_writer, &outc, instrip?1:0 );

		if( status!=0 )
			return -3;

		if( outc.bsize<0 )
			return -2;

		return 0;
	}
	else if( lua_isuserdata(L,idx) )
	{
		void* ud = lua_touserdata( L, idx );

		int bs = lua_objlen( L, idx );
		SU_ASSERT( bs > 0 );

		if( outc.bcapacity < bs )
			return -2;

		util::Memcopy( outc.addr, ud, bs );
		outc.bsize = bs;
		return 0;
	}
	else
	{
		return -1;
	}
}




int luaT_loadcode ( lua_State* L, int idx )
{
	if( lua_isfunction(L,idx) ) {
		lua_pushvalue( L, idx );
		return 0;
	}

	void* ud = lua_touserdata( L, idx );
	if( !ud ) {
		lua_pushstring( L, "invalid referenced code !" );
		return -3;
	}

	// get bs
	int bs = lua_objlen( L, idx );
	SU_ASSERT( bs > 0 );

	// get name
	lua_getfield( L, idx, "path" );
	int ld = luaT_loadbuffer( L, (pcstr)ud, bs, luaT_getstring(L,-1) );
	lua_remove( L, -2 );
	return ld;
}





namespace
{

	inline bool sw_set_any ( Any& out, lua_State* L, int idx )
	{
		int t = lua_gettop(L);
		SU_ASSERT( idx <= t );
		SU_REQUIRE_OR_FALSE( idx <= t );

		int p = lua_type( L, idx );
		
		if( p == LUA_TBOOLEAN )
		{
			out.set( lua_toboolean(L,idx)!=0 );
			return true;
		}
		else if( p == LUA_TLIGHTUSERDATA )
		{
			out.set( (void*)lua_touserdata(L,idx) );
			return true;
		}
		else if( p == LUA_TUSERDATA )
		{
			out.set( (void*)lua_touserdata(L,idx) );
			return true;
		}
		else if( p == LUA_TNUMBER )
		{
			lua_Number n = lua_tonumber( L, idx );
			if( n==long(n) )	out.set( int32(n) );
			else				out.set( float(n) );
			return true;
		}
		else if( p == LUA_TSTRING )
		{
			out.set( (pcstr)lua_tostring(L,idx) );
			return true;
		}
		return false;
	}


	inline bool sw_push_any ( Any& in, lua_State* L )
	{
		if( in.isBool() )
		{
			lua_pushboolean( L, in.asBool() );
			return true;
		}
		else if( in.isFloat() )
		{
			lua_pushnumber( L, in.asFloat() );
			return true;
		}
		else if( in.isNumeric() )
		{
			lua_pushnumber( L, in.asInt() );
			return true;
		}
		else if( in.isString() )
		{
			lua_pushstring( L, in.asString() );
			return true;
		}
		else if( in.isPointer() )
		{
			lua_pushlightuserdata( L, in.asPointer() );
			return true;
		}
		return false;
	}


	inline int sw_collectargs ( Any* outargs, int inmaxargs, lua_State* L, int idx )
	{
		SU_REQUIRE_OR_0( L );
		SU_REQUIRE_OR_0( outargs );
		SU_REQUIRE_OR_0( inmaxargs );
		int t = lua_gettop(L);
		int i = 0;
		for( ; i<inmaxargs && idx+i<=t ; i++ )
			if( !sw_set_any(outargs[i],L,idx+i) )
				return 0;
		return i;
	}


	inline int sw_call ( ISwitch* sw, int si, lua_State* L, bool luahook )
	{
		SU_ASSERT( sw );
		SU_ASSERT( L );

		int ret;

		if( luahook )
		{
			sw->CallSwitch( si, &ret, L );
		}
		else
		{
			Any argv[8];
			int argc = sw_collectargs( argv, SU_SIZEOF(argv), L, 1 );

			Any res;
			sw->CallSwitchMem( si, &res, argc, argv );

			ret = sw_push_any(res,L) ? 1 : 0;
		}

		return ret;
	}


}









namespace
{


	struct swfilter
	{
		su::ISwitch*	sw;
		pcstr			filter;
		int				count;
		int				ith;
		bool			luahook;
		ISwitch::Desc	desc;

		swfilter( su::IInterface* inobj, pcstr innamefilter )
		{
			SU_ASSERT( inobj );
			sw = inobj->GetInterface<ISwitch>();
			filter = sw ? innamefilter : NULL;
			count  = sw ? sw->CountSwitch() : 0;
			ith    = -1;
		}

		bool next (	)
		{
			for( ; sw ; )
			{
				ith++;

				if( !sw->EnumSwitch(ith,desc) ) {
					ith = -1;
					break;
				}

				if( !desc.types )
					continue;

				if( filter && !util::Fnmatch(filter,desc.func,util::FNM_IGNORECASE) )
					continue;

				luahook = util::Strequals( desc.sign, "int,lua_State*" );

				return true;
			}

			return false;
		}
	};



	int bind_import_call ( lua_State* L )
	{
		ISwitch* sw = (ISwitch*) lua_touserdata( L, lua_upvalueindex(1) );
		int si = lua_tointeger( L, lua_upvalueindex(2) );
		int	lk = lua_toboolean( L, lua_upvalueindex(3) );

		return sw_call( sw, si, L, (lk!=0) );
	}


}




uint luaT_swimport ( lua_State *L, int idx, su::ISwitch* insw, pcstr innamefilter )
{
	SU_ASSERT( L );
	SU_ASSERT( insw );

	int aidx = luaT_absidx( L, idx );
	uint nb = 0;

	swfilter swf( insw, innamefilter );
	while( swf.next() )
	{
		lua_pushlightuserdata( L, insw );
		lua_pushinteger( L, swf.ith );
		lua_pushboolean( L, swf.luahook );
		lua_pushcclosure( L, bind_import_call, 3 );
		lua_setfield( L, aidx, swf.desc.name );
		nb++;
	}

	return nb;
}






namespace
{


	int bind_swobject_dtor ( lua_State* L )
	{
		SU_ASSERT( lua_objlen(L,1)==sizeof(ISwitch*) );
		ISwitch** psw = (ISwitch**) lua_touserdata(L,1);
		SU_ASSERT( psw );
		SU_ASSERT( *psw );

		(*psw)->Release();
		return 0;
	}

	
	int bind_swobject_countref ( lua_State* L )
	{
		SU_ASSERT( lua_objlen(L,1)==sizeof(ISwitch*) );
		ISwitch** psw = (ISwitch**) lua_touserdata(L,1);
		SU_ASSERT( psw );
		SU_ASSERT( *psw );
		lua_pushinteger( L, (*psw)->CountRef() );
		return 1;
	}


	int bind_swobject_retain ( lua_State* L )
	{
		SU_ASSERT( lua_objlen(L,1)==sizeof(ISwitch*) );
		ISwitch** psw = (ISwitch**) lua_touserdata(L,1);
		SU_ASSERT( psw );
		SU_ASSERT( *psw );
		(*psw)->Retain();
		return 0;
	}


	int bind_swobject_release ( lua_State* L )
	{
		SU_ASSERT( lua_objlen(L,1)==sizeof(ISwitch*) );
		ISwitch** psw = (ISwitch**) lua_touserdata(L,1);
		SU_ASSERT( psw );
		SU_ASSERT( *psw );
		(*psw)->Release();
		return 0;
	}


	int bind_swobject_getinterface ( lua_State* L )
	{
		SU_ASSERT( lua_objlen(L,1)==sizeof(ISwitch*) );
		ISwitch** psw = (ISwitch**) lua_touserdata(L,1);
		SU_ASSERT( psw );
		SU_ASSERT( *psw );

		pcstr mname = luaT_getstring(L,2);
		uint32 muid = util::hmac_classid( mname );
		SU_REQUIRE_OR_0( muid );

		IInterface* o = (*psw)->GetInterface( muid );
		SU_REQUIRE_OR_0( o );

		luaT_swpushobject( L, o );
		return 1;
	}


	int bind_swobject_call ( lua_State* L )
	{
		SU_ASSERT( lua_objlen(L,1)==sizeof(ISwitch*) );
		ISwitch** psw = (ISwitch**) lua_touserdata(L,1);

		if( psw==NULL || *psw==NULL )
		{
			luaT_error( L, "Bad object call: Try the obj:mth() call style instead !" );
			return 0;
		}

		ISwitch* sw = *psw;
		lua_remove(L,1);

		int si = lua_tointeger( L, lua_upvalueindex(1) );
		int	lk = lua_toboolean( L, lua_upvalueindex(2) );
		SU_ASSERT( si >= 0 );

		return sw_call( sw, si, L, (lk!=0) );
	}


	bool push_swobject_class ( lua_State *L, su::IInterface* inobj, pcstr innamefilter, luaT_Enum* inenums )
	{
		SU_ASSERT( L );
		SU_ASSERT( inobj );
		SU_ASSERT( inobj->GetBuilder() );

		pcstr mclass = inobj->GetBuilder()->GetMClassname();
		SU_ASSERT( mclass );

		char swclass[256];
		util::Strformat( swclass, "swobject:%s:%s",
			mclass,
			util::Strlen(innamefilter) ? innamefilter : "*" );

		// already registered ?
		// ( -- mt )
		lua_getfield( L, LUA_REGISTRYINDEX, swclass );
		if( lua_istable(L,-1) )
			return false;	// not newly defined !

		// define the class
		// ( nil -- )
		lua_pop(L,1);

		// create meta
		// ( -- mt )
		lua_newtable(L);
		int mt = lua_gettop(L);

		// register meta in registry
		lua_pushvalue(L, mt);
		lua_setfield( L, LUA_REGISTRYINDEX, swclass );

		// store dtor event
		// ( mt -- mt )
		lua_pushcfunction( L, bind_swobject_dtor );
		lua_setfield( L, mt, "__gc" );

		// build the __index table xt
		// ( mt -- mt xt )
		lua_newtable( L );
		int xt = lua_gettop( L );
		lua_pushvalue( L, xt );
		lua_setfield( L, mt, "__index" );

		// store methods in __index table
		swfilter swf( inobj, innamefilter );
		while( swf.next() )
		{
			lua_pushinteger( L, swf.ith );
			lua_pushboolean( L, swf.luahook );
			lua_pushcclosure( L, bind_swobject_call, 2 );
			lua_setfield( L, xt, swf.desc.name );
		}

		// store enums in __index table
		// ( mt -- mt xt )
		while( inenums && inenums->name )
		{
			lua_pushinteger( L, inenums->value );
			lua_setfield( L, xt, inenums->name );
			inenums++;
		}

		// store countref event
		// ( mt -- mt xt )
		lua_pushcfunction( L, bind_swobject_countref );
		lua_setfield( L, xt, "__countref" );

		// store retain event
		// ( mt -- mt xt )
		lua_pushcfunction( L, bind_swobject_retain );
		lua_setfield( L, xt, "__retain" );

		// store release event
		// ( mt -- mt xt )
		lua_pushcfunction( L, bind_swobject_release );
		lua_setfield( L, xt, "__release" );

		// store getinterface event
		// ( mt -- mt xt )
		lua_pushcfunction( L, bind_swobject_getinterface );
		lua_setfield( L, xt, "__getinterface" );

		// store swobject
		// ( mt -- mt xt )
		lua_pushboolean( L, true );
		lua_setfield( L, xt, "__swobject" );

		// store swclass
		// ( mt -- mt xt )
		lua_pushstring( L, swclass );
		lua_setfield( L, xt, "__swclass" );

		// store swimplementation
		// ( mt -- mt xt )
		lua_pushstring( L, mclass );
		lua_setfield( L, xt, "__swimplementation" );

		lua_settop( L, mt );

		return true;	// newly defined
	}

}



bool luaT_swpushobject ( lua_State *L, IInterface* inobj, pcstr innamefilter, luaT_Enum* inenums )
{
	SU_ASSERT( L );

	if( inobj )
	{
		inobj->Retain();

		// ( -- ud )
		// push the ISwitch interface preferably ...
		ISwitch* sw = inobj->GetInterface<ISwitch>();
		void** ud = (void**)lua_newuserdata(L,sizeof(void*));
		*ud = sw ? (void*)sw : (void*)inobj;

		// ( ud -- ud mt )
		bool newly = push_swobject_class( L, inobj, innamefilter, inenums );

		// ( ud mt -- ud )
		lua_setmetatable( L, -2 );
		return newly;
	}
	else
	{
		lua_pushnil(L);
		return false;
	}
}



su::IInterface* luaT_swtoobject ( lua_State *L, int idx )
{
	SU_ASSERT( L );

	void** ud = (void**)lua_touserdata( L, idx );
	SU_REQUIRE_OR_NULL( ud );
	SU_REQUIRE_OR_NULL( lua_objlen(L,idx) == sizeof(void*) );

	// is swobject ?
	// ( -- b )
	lua_getfield( L, idx, "__swobject" );

	IInterface* res = NULL;

	if( lua_isboolean(L,-1) && lua_toboolean(L,-1) )
		res = (IInterface*)(*ud);

	lua_pop(L,1);
	return res;
}



bool luaT_swisobject ( lua_State *L, int idx )
{
	return luaT_swtoobject(L,idx) != NULL;
}


su::IInterface* luaT_swcheckobject ( lua_State *L, int idx )
{
	int adx = luaT_absidx(L,idx);
	su::IInterface* o = luaT_swtoobject(L,adx);
	if( !o )
	{
		const char *msg = lua_pushfstring( L, "A SWObject is expected, got %s", luaT_typename(L,adx) );
		luaT_argerror(L, adx, msg);
	}
	return o;
}


void luaT_swsetobject ( lua_State *L, int idx )
{
	SU_ASSERT( lua_gettop(L) >= 3 );
	SU_ASSERT( luaT_swtoobject(L,idx) );
	// k v -- k v mt xt
	lua_getmetatable(L,idx);
	lua_getfield(L,-1,"__index");
	// k v mt xt -- k v mt xt key val
	lua_pushvalue(L,-4);
	lua_pushvalue(L,-4);
	// k v mt xt key val -- k v mt xt
	lua_rawset(L,-3);
	// k v mt xt --
	lua_pop(L,4);
}






namespace
{

	int bind_swdata_call ( lua_State* L )
	{
		ISwitch* sw = (ISwitch*) lua_touserdata( L, lua_upvalueindex(1) );
		int si = lua_tointeger( L, lua_upvalueindex(2) );
		int	lk = lua_toboolean( L, lua_upvalueindex(3) );

		void* uaddr = lua_touserdata( L, 1 );
		int   ubs   = lua_objlen( L, 1 );
		if( !uaddr || !ubs )
		{
			luaT_error( L, "Bad object call: Try the obj:mth() call form instead !" );
			return 0;
		}

		return sw_call( sw, si, L, (lk!=0) );
	}


	uint32 push_swdata_class ( lua_State *L, su::ISwitch* insw, uint32 intypeid, pcstr innamefilter, pcstr indtorname )
	{
		SU_ASSERT( L );
		SU_ASSERT( insw );
		SU_ASSERT( insw->GetBuilder() );

		pcstr mclass = insw->GetBuilder()->GetMClassname();
		SU_ASSERT( mclass );

		char swclass[256];
		util::Strformat( swclass, "swdata:%s:%s:%s",
			mclass,
			util::Strisempty(innamefilter) ? "*" : innamefilter,
			util::Strisempty(indtorname) ? "-" : indtorname	);

		uint32 tid = intypeid ? intypeid : util::hmac_crc32_w(swclass);

		// already registered ?
		// ( -- mt )
		lua_getfield( L, LUA_REGISTRYINDEX, swclass );

		if( !lua_istable(L,-1) )
		{
			// ( x -- )
			lua_pop(L,1);

			// create meta
			// ( -- mt )
			lua_newtable(L);
			int mt = lua_gettop(L);

			// register meta in registry
			lua_pushvalue(L, mt);
			lua_setfield( L, LUA_REGISTRYINDEX, swclass );

			// store methods as __index table
			// ( mt -- xt mt )
			lua_newtable( L );
			int xt = lua_gettop( L );
			lua_pushvalue( L, xt );
			lua_setfield( L, mt, "__index" );

			swfilter swf( insw, innamefilter );
			while( swf.next() )
			{
				lua_pushlightuserdata( L, insw );
				lua_pushinteger( L, swf.ith );
				lua_pushboolean( L, swf.luahook );
				lua_pushcclosure( L, bind_swdata_call, 3 );

				// as dtor ?
				if( indtorname && util::Strequals(indtorname,swf.desc.name) )
					lua_setfield( L, mt, "__gc" );
				else
					lua_setfield( L, xt, swf.desc.name );
			}

			// store swdata
			// ( mt -- xt mt )
			lua_pushboolean( L, true );
			lua_setfield( L, xt, "__swdata" );

			// store swclass
			// ( mt -- xt mt )
			lua_pushstring( L, swclass );
			lua_setfield( L, xt, "__swclass" );

			// store swimplementation
			// ( mt -- xt mt )
			lua_pushstring( L, mclass );
			lua_setfield( L, xt, "__swimplementation" );

			// store typeid
			// ( mt -- xt mt )
			char tmp[32];
			util::Strformat( tmp, "%08x", tid );
			lua_pushstring( L, tmp );
			lua_setfield( L, xt, "__swtypeid" );

			// store typeid as userdata for fast access
			// ( mt -- xt mt )
			lua_pushlightuserdata( L, (void*)tid );
			lua_setfield( L, xt, "__swtypeid_ud" );

			lua_settop( L, mt );
		}

		return tid;
	}

}



void* luaT_swpushdata ( lua_State *L, uint32 intypeid, void* indata, int inbsize, su::ISwitch* insw, pcstr innamefilter, pcstr indtorname )
{
	SU_ASSERT( L );
	SU_ASSERT( insw );
	SU_ASSERT( inbsize );

	// ( -- ud )
	void* ud = lua_newuserdata( L, inbsize );
	SU_ASSERT( ud );
	if( indata )
		util::Memcopy( ud, indata, inbsize );

	// ( ud -- mt ud )
	push_swdata_class( L, insw, intypeid, innamefilter, indtorname );

	// ( mt ud -- ud )
	lua_setmetatable( L, -2 );
	return ud;
}



uint32 luaT_swdatatypeid ( lua_State *L, int idx )
{
	SU_ASSERT( L );
	int ridx = luaT_absidx(L,idx);
	SU_REQUIRE_OR_ZERO( lua_isuserdata(L,ridx) );

	// ( -- typeid )
	lua_getfield( L, ridx, "__swtypeid_ud" );

	if( lua_islightuserdata(L,-1) )
	{
		uint32 tid = (uint32) lua_touserdata(L,-1);
		lua_pop(L,1);
		return tid;
	}
	else
	{
		lua_pop(L,1);
		return 0;
	}
}


void* luaT_swtodata ( lua_State *L, int idx, uint32 intypeid )
{
	SU_ASSERT( L );
	SU_REQUIRE_OR_NULL( intypeid );

	#if SU_BUILD_DBGCODE
	uint32 tid = luaT_swdatatypeid(L,idx);
	SU_REQUIRE_OR_NULL( tid == intypeid );
	#endif

	return lua_touserdata(L,idx);
}










void luaT_registerObject( lua_State* L, void* key )
{
	SU_ASSERT( L );
	SU_ASSERT( key );
	luaT_checktable( L, -1 );
	lua_pushlightuserdata( L, key );
	lua_pushvalue( L, -2 );
	lua_settable( L, LUA_REGISTRYINDEX );
}


void luaT_unregisterObject( lua_State* L, void* key )
{
	SU_ASSERT( L );
	SU_ASSERT( key );
	lua_pushlightuserdata( L, key );
	lua_pushnil( L );
	lua_settable( L, LUA_REGISTRYINDEX );
}


void luaT_callObject ( lua_State* L, void* key, pcstr method, int nargs, int nresults )
{
	// call t.method( t [,args] )
	SU_ASSERT( L );
	SU_ASSERT( key );
	SU_ASSERT( method );
	// args -- args t
	lua_pushlightuserdata( L, key );
	lua_gettable( L, LUA_REGISTRYINDEX );
	SU_ASSERT( lua_istable(L,-1) );
	// args t -- args t f
	lua_getfield( L, -1, method );
	SU_ASSERT( lua_isfunction(L,-1) );
	// args t f -- f t args
	lua_insert( L, -2-nargs );
	lua_insert( L, -1-nargs );
	SU_ASSERT( lua_isfunction(L,-2-nargs) );
	SU_ASSERT( lua_istable(L,-1-nargs) );
	// f args -- results
	lua_call( L, nargs+1, nresults );
}












void* luaT_newvector ( lua_State *L, int size, int count )
{
	SU_ASSERT( L );
	SU_ASSERT( size > 0 );
	SU_ASSERT( count > 0 );
	void* ud = lua_newuserdata( L, size*count );
	SU_ASSERT( ud );
	return ud;
}


int luaT_vlen ( lua_State *L, int idx )
{
	SU_ASSERT( L );
	SU_ASSERT( lua_type(L,idx) == LUA_TUSERDATA );
	return lua_objlen(L,idx);
}


void* luaT_vdata ( lua_State *L, int idx )
{
	SU_ASSERT( L );
	SU_ASSERT( lua_type(L,idx) == LUA_TUSERDATA );
	return lua_touserdata(L,idx);
}


void luaT_vcpy ( lua_State *L, int didx, int sidx )
{
	void* dv = luaT_vdata( L, didx );
	int   dl = luaT_vlen( L, didx );
	void* sv = luaT_vdata( L, sidx );
	int   sl = luaT_vlen( L, sidx );
	util::Memcopy( dv, sv, Min(sl,dl) );
}


bool luaT_veq ( lua_State *L, int idx1, int idx2 )
{
	void* v1 = luaT_vdata( L, idx1 );
	int   l1 = luaT_vlen( L, idx1 );
	void* v2 = luaT_vdata( L, idx2 );
	int   l2 = luaT_vlen( L, idx2 );
	if( v1==v2 )	return true;
	if( l1!=l2 )	return false;
	return util::Memcompare(v1,v2,l1)==0;
}


static char htoh[] = "0123456789abcdef";

pcstr luaT_pushvstr ( lua_State *L, int idx )
{
	void* v = luaT_vdata( L, idx );
	int   l = luaT_vlen( L, idx );

	char* buff = (char*) lua_newuserdata( L, l*2+1 );
	SU_ASSERT( buff );
	char* pb = buff;
	byte* pu = (byte*) v;
	byte* pe = pu+l;
	while( pu<pe )
	{
		pb[0] = htoh[ (*pu)>>4  ];
		pb[1]	 = htoh[ (*pu)&0xF ];
		pb += 2;
		pu += 1;
	}
	*pb = 0;

	lua_pushstring( L, buff );
	lua_remove( L, -2 );
	return lua_tostring(L,-1);
}


int luaT_vlb ( lua_State *L, int idx, int n )
{
	uint8* v = (uint8*) luaT_vdata( L, idx );
	int l = luaT_vlen( L, idx );
	SU_ASSERT( n*1 <= l );
	int t = lua_gettop(L);
	SU_ASSERT( t > n );
	for( int i=0 ; i<n ; i++ )
		v[i] = lua_tointeger(L,-n+i);
	return n;
}


int luaT_vlh ( lua_State *L, int idx, int n )
{
	uint16* v = (uint16*) luaT_vdata( L, idx );
	int l = luaT_vlen( L, idx );
	SU_ASSERT( n*2 <= l );
	int t = lua_gettop(L);
	SU_ASSERT( t > n );
	for( int i=0 ; i<n ; i++ )
		v[i] = lua_tointeger(L,-n+i);
	return n;
}


int luaT_vlw ( lua_State *L, int idx, int n )
{
	uint32* v = (uint32*) luaT_vdata( L, idx );
	int l = luaT_vlen( L, idx );
	SU_ASSERT( n*4 <= l );
	int t = lua_gettop(L);
	SU_ASSERT( t > n );
	for( int i=0 ; i<n ; i++ )
		v[i] = lua_tointeger(L,-n+i);
	return n;
}


int luaT_vld ( lua_State *L, int idx, int n )
{
	uint64* v = (uint64*) luaT_vdata( L, idx );
	int l = luaT_vlen( L, idx );
	SU_ASSERT( n*8 <= l );
	int t = lua_gettop(L);
	SU_ASSERT( t > n );
	for( int i=0 ; i<n ; i++ )
		v[i] = lua_tointeger(L,-n+i);
	return n;
}


int luaT_vlq ( lua_State *L, int idx, int n )
{
	uint128* v = (uint128*) luaT_vdata( L, idx );
	int l = luaT_vlen( L, idx );
	SU_ASSERT( n*16 <= l );
	int t = lua_gettop(L);
	SU_ASSERT( t > n );
	for( int i=0 ; i<n ; i++ )
		v[i] = lua_tointeger(L,-n+i);
	return n;
}


int luaT_vlf ( lua_State *L, int idx, int n )
{
	float* v = (float*) luaT_vdata( L, idx );
	int l = luaT_vlen( L, idx );
	SU_ASSERT( n*4 <= l );
	int t = lua_gettop(L);
	SU_ASSERT( t > n );
	for( int i=0 ; i<n ; i++ )
		v[i] = lua_tonumber(L,-n+i);
	return n;
}


#define VSTORE_MAX 32

int luaT_vsb ( lua_State *L, int idx )
{
	uint8* v = (uint8*) luaT_vdata( L, idx );
	int l = luaT_vlen( L, idx );
	int n = Min( l>>0, VSTORE_MAX );
	for( int i=0 ; i<n ; i++ )
		lua_pushinteger(L,v[i]);
	return n;
}

int luaT_vsh ( lua_State *L, int idx )
{
	uint16* v = (uint16*) luaT_vdata( L, idx );
	int l = luaT_vlen( L, idx );
	SU_ASSERT( (l&1)==0 );
	int n = Min( l>>1, VSTORE_MAX );
	for( int i=0 ; i<n ; i++ )
		lua_pushinteger(L,v[i]);
	return n;
}


int luaT_vsw ( lua_State *L, int idx )
{
	uint32* v = (uint32*) luaT_vdata( L, idx );
	int l = luaT_vlen( L, idx );
	SU_ASSERT( (l&3)==0 );
	int n = Min( l>>2, VSTORE_MAX );
	for( int i=0 ; i<n ; i++ )
		lua_pushinteger(L,v[i]);
	return n;
}


int luaT_vsd ( lua_State *L, int idx )
{
	uint64* v = (uint64*) luaT_vdata( L, idx );
	int l = luaT_vlen( L, idx );
	SU_ASSERT( (l&7)==0 );
	int n = Min( l>>3, VSTORE_MAX );
	for( int i=0 ; i<n ; i++ )
		lua_pushinteger(L,v[i]);
	return n;
}


int luaT_vsq ( lua_State *L, int idx )
{
	uint128* v = (uint128*) luaT_vdata( L, idx );
	int l = luaT_vlen( L, idx );
	SU_ASSERT( (l&15)==0 );
	int n = Min( l>>4, VSTORE_MAX );
	for( int i=0 ; i<n ; i++ )
		lua_pushinteger(L,(uint)v[i]);
	return n;
}


int luaT_vsf ( lua_State *L, int idx )
{
	float* v = (float*) luaT_vdata( L, idx );
	int l = luaT_vlen( L, idx );
	SU_ASSERT( (l&3)==0 );
	int n = Min( l>>2, VSTORE_MAX );
	for( int i=0 ; i<n ; i++ )
		lua_pushnumber(L,v[i]);
	return n;
}



