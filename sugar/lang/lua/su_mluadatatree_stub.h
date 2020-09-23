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



#include "su_lua.h"
using namespace su;



#define LUA_ASSERT_IDX( inIDX, inL ) \
			SU_ASSERTC( (inIDX)>0, "Invalid stack index !" );	\
			SU_ASSERTC( (inIDX)<=lua_gettop(inL), "Invalid stack index !" );

#define XSTACK_MAX_TOP		64




namespace
{

	struct SIDL_STUB_CLASSNAME
	{
		lua_State*	L;
		void*		reg_root_key;
		void*		reg_pmap_key;


		bool Init ( Pool* p, AnyList* o )
		{
			L = NULL;

			char tmp[64];
			union { uint64 u64; uint32 u32[2]; };
			u64 = uint64(this);

			util::Strformat( tmp, "dtt_root_key_@%08x%08x", u32[1], u32[0] );
			reg_root_key = (void*) util::hmac_crc32_w(tmp);

			util::Strformat( tmp, "dtt_pmap_key_@%08x%08x", u32[1], u32[0] );
			reg_pmap_key = (void*) util::hmac_crc32_w(tmp);

			if( o && o->isType("luastate",Any::Addr) )
			{
				lua_State* lstate = (lua_State*) o->asPointer( "luastate" );
				Setup( lstate );
			}

			return true;
		}


		void Shut ( )
		{
			if( L )
			{
				lua_pushlightuserdata( L, reg_root_key );
				lua_pushnil( L );
				lua_settable( L, LUA_REGISTRYINDEX );

				lua_pushlightuserdata( L, reg_pmap_key );
				lua_pushnil( L );
				lua_settable( L, LUA_REGISTRYINDEX );
			}
		}


		bool Setup ( lua_State* inL )
		{
			SU_REQUIRE_OR_FALSE( inL );
			SU_REQUIRE_OR_FALSE( L == NULL );
			L = inL;

			lua_checkstack( L, XSTACK_MAX_TOP );

			// create the property-map as a key-weaked table
			// only collectable objects are removed from a weak table
			// integers, booleans or strings are not !
			lua_pushlightuserdata( L, reg_pmap_key );
			lua_newtable( L );
			lua_newtable( L );
			lua_pushstring( L, "k" );
			lua_setfield( L, -2, "__mode" );
			lua_setmetatable( L, -2 );
			lua_settable( L, LUA_REGISTRYINDEX );

			return true;
		}


		lua_State* GetState ( )
		{
			return L;
		}


		int c_idx ( int in_lua_idx )
		{
			SU_ASSERT( in_lua_idx != 0 );
			if( in_lua_idx < 0 )	return in_lua_idx;
			else 					return in_lua_idx-1;
		}


		int lua_idx ( int in_c_idx )
		{
			if( in_c_idx < 0 )	return in_c_idx;
			else 				return in_c_idx+1;
		}


		int lua_abs_idx ( int in_c_idx )
		{
			if( in_c_idx < 0 )	return lua_gettop(L)+in_c_idx+1;
			else 				return in_c_idx+1;
		}


		bool isempty ( )
		{
			SU_REQUIRE_OR_TRUE( L );
			return lua_gettop(L) == 0;
		}


		int getcount ( )
		{
			SU_REQUIRE_OR_0( L );
			return lua_gettop(L);
		}


		void setcount ( int inn )
		{
			SU_REQUIRE_OR_RETURN( L );
			SU_ASSERT( inn>=0 );
			return lua_settop( L, inn );
		}


		int baseindex ( int inidx )
		{
			SU_REQUIRE_OR_0( L );
			if( inidx < 0 )
			{
				int t = lua_gettop(L)-1;
				SU_ASSERT( inidx <= t );
				return t+1+inidx;
			}
			else
			{
				return inidx;
			}
		}


		int topindex ( int inidx )
		{
			SU_REQUIRE_OR_0( L );
			if( inidx < 0 )
			{
				return inidx;
			}
			else
			{
				int t = lua_gettop(L)-1;
				SU_ASSERT( inidx <= t );
				return -(t-inidx)-1;
			}
		}


		int gettop (  )
		{
			SU_REQUIRE_OR_0( L );
			return lua_gettop(L)-1;
		}


		void settop ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( L );
			int top = lua_abs_idx(inidx);
			SU_ASSERT( top >= 0 );
			return lua_settop( L, top );
		}


		void pop ( int incount )
		{
			SU_REQUIRE_OR_RETURN( L );
			SU_ASSERT( incount>=0 );
			SU_ASSERT( incount<=lua_gettop(L) );
			lua_pop( L, incount );
		}


		void push ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			lua_pushvalue( L, lidx );
		}


		void insert ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			lua_insert( L, lidx );
		}


		void remove ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			lua_remove( L, lidx );
		}


		void replace ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			lua_replace( L, lidx );
		}


		bool equal ( int inidx1, int inidx2 )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx1 = lua_abs_idx(inidx1);
			int lidx2 = lua_abs_idx(inidx2);
			LUA_ASSERT_IDX( lidx1, L );
			LUA_ASSERT_IDX( lidx2, L );
			if( lua_isuserdata(L,lidx1) && lua_isuserdata(L,lidx2) )
			{
				size_t l1 = lua_objlen(L,lidx1);
				size_t l2 = lua_objlen(L,lidx2);
				if( l1 != l2 )		return false;
				void* u1 = lua_touserdata(L,lidx1);
				void* u2 = lua_touserdata(L,lidx2);
				if( u1 == u2 )		return true;
				return util::Memcompare(u1,u2,l1) == 0;
			}
			else
			{
				return lua_equal(L,lidx1,lidx2) == 1;
			}
		}


		bool isnil ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			return lua_isnil( L, lidx );
		}


		bool isboolean ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			return lua_isboolean( L, lidx );
		}


		bool isblob ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			return lua_type( L, lidx ) == LUA_TUSERDATA;
		}


		bool isstring ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			return lua_type( L, lidx ) == LUA_TSTRING;
		}


		bool isinteger ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			SU_REQUIRE_OR_FALSE( lua_type(L,lidx) == LUA_TNUMBER );
			float n = lua_tonumber(L,lidx);
			int   i = lua_tointeger(L,lidx);
			return float(i) == n;
		}


		bool isnumber ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			SU_REQUIRE_OR_FALSE( lua_type(L,lidx) == LUA_TNUMBER );
			float n = lua_tonumber(L,lidx);
			int   i = lua_tointeger(L,lidx);
			return float(i) != n;
		}


		bool isscalar ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			int t = lua_type( L, lidx );
			return (t==LUA_TBOOLEAN || t==LUA_TNUMBER || t==LUA_TSTRING || t==LUA_TUSERDATA );
		}


		bool iscollection ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			int t = lua_type( L, lidx );
			return (t==LUA_TTABLE);
		}


		bool issequence ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			int t = lua_type(L,lidx);
			SU_REQUIRE_OR_FALSE( t==LUA_TTABLE );
			if( luaT_istableempty(L,lidx) )
				return true;
			return luaT_ispurearray( L, lidx );
		}


		bool ismapping ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			int t = lua_type(L,lidx);
			SU_REQUIRE_OR_FALSE( t==LUA_TTABLE );
			if( luaT_istableempty(L,lidx) )
				return true;
			return !luaT_ispurearray( L, lidx );
		}


		IDatatree::NodeType nodetype ( int inidx )
		{
			SU_REQUIRE_OR_RETURN_V( L, IDatatree::NT_Nil );
			if( isscalar(inidx) )	return IDatatree::NT_Scalar;
			if( issequence(inidx) )	return IDatatree::NT_Sequence;
			if( ismapping(inidx) )	return IDatatree::NT_Mapping;
			return IDatatree::NT_Nil;
		}


		IDatatree::ScalarType scalartype ( int inidx )
		{
			SU_REQUIRE_OR_RETURN_V( L, IDatatree::ST_Undef );
			if( isboolean(inidx) )		return IDatatree::ST_Boolean;
			if( isstring(inidx) )		return IDatatree::ST_String;
			if( isinteger(inidx) )		return IDatatree::ST_Integer;
			if( isnumber(inidx) )		return IDatatree::ST_Number;
			if( isblob(inidx) )			return IDatatree::ST_Blob;
			return IDatatree::ST_Undef;
		}


		byte* asblob ( int inidx, int* outbsize )
		{
			SU_REQUIRE_OR_NULL( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			int t = lua_type(L,lidx);
			SU_REQUIRE_OR_NULL( t==LUA_TUSERDATA );
			if( outbsize )	*outbsize = lua_objlen( L, lidx );
			return (byte*) lua_touserdata( L, lidx );
		}


		bool asboolean ( int inidx )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			int t = lua_type(L,lidx);
			SU_REQUIRE_OR_FALSE( t==LUA_TBOOLEAN );
			int i = lua_toboolean( L, lidx );
			if( i )		return true;
			else		return false;
		}


		pcstr asstring ( int inidx )
		{
			SU_REQUIRE_OR_NULL( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			int t = lua_type(L,lidx);
			SU_REQUIRE_OR_NULL( t==LUA_TSTRING );
			return lua_tostring( L, lidx );
		}


		int asinteger ( int inidx )
		{
			SU_REQUIRE_OR_0( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			int t = lua_type(L,lidx);
			SU_REQUIRE_OR_0( t==LUA_TNUMBER );
			return lua_tointeger( L, lidx );
		}


		float asnumber ( int inidx )
		{
			SU_REQUIRE_OR_0( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			int t = lua_type(L,lidx);
			SU_REQUIRE_OR_0( t==LUA_TNUMBER );
			return lua_tonumber( L, lidx );
		}


		bool toboolean ( int inidx )
		{
			IDatatree::ScalarType st;
			st = scalartype( inidx );
			if( st == IDatatree::ST_Boolean )
			{
				return asboolean(inidx);
			}
			else if( st == IDatatree::ST_Integer )
			{
				return asinteger(inidx) != 0;
			}
			else if( st == IDatatree::ST_Number )
			{
				return asnumber(inidx) != 0.f;
			}
			else if( st == IDatatree::ST_String )
			{
				pcstr s = asstring(inidx);
				if( s && *s )
				{
					if( util::Strequals(s,"true",false) )		return true;
					if( util::Strequals(s,"yes",false) )		return true;
					if( util::Strequals(s,"oui",false) )		return true;
					if( util::Strequals(s,"vrai",false) )		return true;
					if( util::Strequals(s,"1",false) )			return true;
				}
			}
			return false;
		}


		pcstr tostring ( int inidx )
		{
			static char shared[16];
			IDatatree::ScalarType st;
			st = scalartype( inidx );
			if( st == IDatatree::ST_Boolean )
			{
				bool b = asboolean(inidx);
				if( b )		return "true";
				else		return "false";
			}
			else if( st == IDatatree::ST_Integer )
			{
				int i = asinteger(inidx);
				int len = util::Strformat( shared, (long)i, 10 );
				SU_ASSERT( len<sizeof(shared) );
				return shared;
			}
			else if( st == IDatatree::ST_Number )
			{
				float n = asnumber(inidx);
				int len = util::Strformat( shared, n );
				SU_ASSERT( len<sizeof(shared) );
				return shared;
			}
			else if( st == IDatatree::ST_String )
			{
				return asstring(inidx);
			}
			return NULL;
		}


		int tointeger ( int inidx )
		{
			IDatatree::ScalarType st;
			st = scalartype( inidx );
			if( st == IDatatree::ST_Boolean )
			{
				bool b = asboolean(inidx);
				if( b )		return 1;
				else		return 0;
			}
			else if( st == IDatatree::ST_Integer )
			{
				int i = asinteger(inidx);
				return i;
			}
			else if( st == IDatatree::ST_Number )
			{
				float n = asnumber(inidx);
				return int(n);
			}
			else if( st == IDatatree::ST_String )
			{
				pcstr s = asstring(inidx);
				if( s && *s )
					return util::Strtoi10(s);
			}
			return 0;
		}


		float tonumber ( int inidx )
		{
			IDatatree::ScalarType st;
			st = scalartype( inidx );
			if( st == IDatatree::ST_Boolean )
			{
				bool b = asboolean(inidx);
				if( b )		return 1.f;
				else		return 0.f;
			}
			else if( st == IDatatree::ST_Integer )
			{
				int i = asinteger(inidx);
				return float(i);
			}
			else if( st == IDatatree::ST_Number )
			{
				float n = asnumber(inidx);
				return n;
			}
			else if( st == IDatatree::ST_String )
			{
				pcstr s = asstring(inidx);
				if( s && *s )
					return util::Strtofloat(s,NULL);
			}
			return 0.f;
		}


		void clear ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			luaT_cleartable( L, lidx );
		}


		int size ( int inidx )
		{
			SU_REQUIRE_OR_0( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			SU_REQUIRE_OR_0( lua_istable(L,lidx) );
			return luaT_tablelen(L,lidx);
		}


		void resize ( int inidx, int inlen )
		{
			SU_ASSERT( inlen >= 0 );
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			SU_REQUIRE_OR_RETURN( lua_istable(L,lidx) );
			int n = lua_objlen(L,lidx);
			if( inlen > n )
			{
				// grow
				for( int i=n+1 ; i<=inlen ; i++ )
				{
					lua_pushinteger( L, i );
					lua_pushinteger( L, 0 );
					lua_settable( L, lidx );
				}
			}
			else
			{
				// shrink
				for( int i=inlen+1 ; i<=n ; i++ )
				{
					lua_pushinteger( L, i );
					lua_pushnil( L );
					lua_settable( L, lidx );
				}
			}
			SU_ASSERT( lua_objlen(L,lidx) == inlen );
		}


		void push ( int inidx, int inith )
		{
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			SU_REQUIRE_OR_RETURN( lua_istable(L,lidx) );
			SU_ASSERT( inith >= 0 );
			int n = lua_objlen(L,lidx);
			SU_ASSERT( inith < n );
			lua_pushinteger( L, inith+1 );
			lua_gettable( L, lidx );
		}


		void store ( int inidx, int inith )
		{
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			SU_REQUIRE_OR_RETURN( lua_istable(L,lidx) );
			SU_ASSERT( inith >= 0 );
			SU_ASSERT( inith < lua_objlen(L,lidx) );
			// ( v -- v k v )
			lua_pushinteger( L, inith+1 );
			lua_pushvalue( L, -2 );
			// ( v k v -- v )
			lua_settable( L, lidx );
			// ( v -- )
			lua_pop( L, 1 );
		}


		void append ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			SU_REQUIRE_OR_RETURN( lua_istable(L,lidx) );
			int sz = lua_objlen( L, lidx );
			lua_pushinteger( L, sz+1 );
			lua_pushvalue( L, -2 );
			lua_settable( L, lidx );
			lua_pop( L, 1 );
		}


		bool haskey ( int inidx, pcstr inkey )
		{
			SU_REQUIRE_OR_FALSE( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			SU_REQUIRE_OR_FALSE( lua_istable(L,lidx) );
			lua_pushstring( L, inkey );
			lua_gettable( L, lidx );
			bool is_nil = lua_isnil(L,-1);
			lua_pop(L,1);
			return !is_nil;
		}


		void push ( int inidx, pcstr inkey )
		{
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			SU_REQUIRE_OR_RETURN( lua_istable(L,lidx) );
			lua_pushstring( L, inkey );
			lua_gettable( L, lidx );
		}


		struct KeyPath
		{
			pcstr cur;
			char  buff[64];
			int   depth;

			KeyPath ( pcstr inpath )
			{
				cur = inpath;
				depth = 1;
			}

			pcstr extract ( )
			{
				SU_REQUIRE_OR_NULL( cur && *cur );
				char *b    = buff;
				char *bend = buff + sizeof(buff);
				while( *cur && *cur!='.' && b<bend )
					*b++ = *cur++;
				SU_REQUIRE_OR_NULL( b<bend );
				*b = 0;
				if( *cur=='.' )	cur++;
				depth++;
				return buff;
			}
		};


		void pushchain ( int inidx, pcstr inkeypath )
		{
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );

			lua_pushvalue( L, lidx );

			bool failed = true;
			KeyPath kp( inkeypath );
			for( ;; )
			{
				pcstr co = kp.extract();
				if( !co )	break;

				if( lua_istable(L,-1) )
				{
					if( util::Strisdigit(co) ) {
						int ith = util::Strtou10(co);
						lua_pushinteger(L,ith+1);	// start-index at 1 for lua !
					} else {
						lua_pushstring(L,co);
					}
					lua_gettable(L,-2);
					lua_remove(L,-2);
					failed = false;
					continue;
				}

				// not found !
				failed = true;
				break;
			}

			if( failed )
			{
				lua_pop(L,1);
				lua_pushnil(L);
			}
		}


		void store ( int inidx, pcstr inkey )
		{
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			SU_REQUIRE_OR_RETURN( lua_istable(L,lidx) );
			lua_pushstring( L, inkey );
			lua_pushvalue( L, -2 );
			lua_settable( L, lidx );
			lua_pop( L, 1 );
		}


		void unkey ( int inidx, pcstr inkey )
		{
			SU_REQUIRE_OR_RETURN( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			SU_REQUIRE_OR_RETURN( lua_istable(L,lidx) );
			lua_pushstring( L, inkey );
			lua_pushnil( L );
			lua_settable( L, lidx );
		}


		pcstr enumkey ( int inidx, pcstr inkey )
		{
			SU_REQUIRE_OR_NULL( L );
			int lidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( lidx, L );
			SU_REQUIRE_OR_NULL( lua_istable(L,lidx) );
			if( inkey && *inkey )	lua_pushstring( L, inkey );
			else					lua_pushnil( L );
			pcstr next_k = NULL;
			if( lua_next(L,lidx) != 0 )
			{
				// 'key' (at index -2) and 'value' (at index -1)
				if( lua_isstring(L,-2) )
					next_k = lua_tostring( L, -2 );
				// next_k refers to the internal string value
				// so is still available after the pop
				lua_pop( L, 2 );
			}
			return next_k;
		}


		void pushroot ( )
		{
			SU_REQUIRE_OR_RETURN( L );
			lua_pushlightuserdata( L, reg_root_key );
			lua_gettable( L, LUA_REGISTRYINDEX );
		}


		void storeroot ( )
		{
			SU_REQUIRE_OR_RETURN( L );
			int t = lua_type( L, -1 );
			SU_ASSERT( t==LUA_TNIL || t==LUA_TBOOLEAN || t==LUA_TNUMBER || t==LUA_TSTRING || t==LUA_TTABLE || t==LUA_TUSERDATA );
			lua_pushlightuserdata( L, reg_root_key );
			lua_pushvalue( L, -2 );
			lua_settable( L, LUA_REGISTRYINDEX );
			lua_pop( L, 1 );
		}


		void pushnil (  )
		{
			SU_REQUIRE_OR_RETURN( L );
			lua_pushnil( L );
		}


		void pushscalar ( pcstr invalue )
		{
			SU_REQUIRE_OR_RETURN( L );
			pcstr v0 = invalue;
			pcstr v1 = v0 + util::Strlen(v0);

			if( util::Strisdigit(v0) ) {
				pushinteger( util::Strtoi10(invalue) );
				return;
			}

			pstr endptr;
			float f = util::Strtofloat(v0,&endptr);
			if( endptr==v1 ) {
				pushnumber( f );
				return;
			}

			if( util::Strequals(invalue,"false",false) )		pushboolean(false); 
			else if( util::Strequals(invalue,"no",false) )		pushboolean(false); 
			else if( util::Strequals(invalue,"non",false) )		pushboolean(false); 
			else if( util::Strequals(invalue,"faux",false) )	pushboolean(false); 
			else if( util::Strequals(invalue,"true",false) )	pushboolean(true); 
			else if( util::Strequals(invalue,"yes",false) )		pushboolean(true); 
			else if( util::Strequals(invalue,"oui",false) )		pushboolean(true); 
			else if( util::Strequals(invalue,"vrai",false) )	pushboolean(true);
			else												pushstring(invalue);
		}


		void pushboolean ( bool invalue )
		{
			SU_REQUIRE_OR_RETURN( L );
			lua_pushboolean( L, invalue?1:0 );
		}


		void pushstring ( pcstr invalue )
		{
			SU_REQUIRE_OR_RETURN( L );
			if( !invalue || *invalue==0 )
				lua_pushnil( L );
			else
				lua_pushstring( L, invalue );
		}


		void pushstring ( pcstr instart, pcstr inend )
		{
			SU_REQUIRE_OR_RETURN( L );
			if( !instart || !inend || inend<=instart )
				lua_pushnil( L );
			else
				lua_pushlstring( L, instart, inend-instart );
		}


		void pushinteger ( int invalue )
		{
			SU_REQUIRE_OR_RETURN( L );
			lua_pushinteger( L, invalue );
		}


		void pushnumber ( float invalue )
		{
			SU_REQUIRE_OR_RETURN( L );
			lua_pushnumber( L, invalue );
		}


		void pushblob ( byte* inaddr, int inlen )
		{
			SU_REQUIRE_OR_RETURN( L );
			void* ud = lua_newuserdata( L, inlen );
			if( ud )
				util::Memcopy( ud, inaddr, inlen );
		}


		void pushsequence ( int incapacity )
		{
			SU_REQUIRE_OR_RETURN( L );
			lua_createtable( L, incapacity, 0 );
		}


		void pushmapping (  )
		{
			SU_REQUIRE_OR_RETURN( L );
			lua_newtable( L );
		}



		//
		// Properties


		void PushPropTable ( )
		{
			SU_REQUIRE_OR_RETURN( L );

			lua_pushlightuserdata( L, reg_pmap_key );
			lua_gettable( L, LUA_REGISTRYINDEX );
			SU_ASSERT( lua_istable(L,-1) );
		}


		void storeproperty ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( L );
			int aidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( aidx, L );

			// ( props -- props pmap )
			PushPropTable();

			// ( props pmap -- props pmap k v )
			lua_pushvalue( L, aidx );
			lua_pushvalue( L, -3 );

			// ( props pmap k v -- props pmap )
			lua_settable( L, -3 );

			// ( props pmap -- )
			lua_pop( L, 2 );
		}


		void pushproperty ( int inidx )
		{
			SU_REQUIRE_OR_RETURN( L );
			int aidx = lua_abs_idx(inidx);
			LUA_ASSERT_IDX( aidx, L );

			// ( -- pmap )
			PushPropTable();

			// ( pmap -- pmap props )
			lua_pushvalue( L, aidx );
			lua_gettable( L, -2 );

			// ( pmap props -- props )
			lua_remove( L, -2 );
		}
	};


}


