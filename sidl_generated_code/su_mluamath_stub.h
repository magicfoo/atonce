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



#define SWDATA_VEC2_TYPEID		0xb89292a0
#define SWDATA_VEC3_TYPEID		0x7438923e
#define SWDATA_VEC4_TYPEID		0x7efd9b27
#define SWDATA_QUAT_TYPEID		0x8e758771
#define SWDATA_MATRIX_TYPEID	0x5ec0332e



namespace
{
	struct SIDL_STUB_CLASSNAME
	{
		ISwitch* sw;
		bool Init ( Pool* p, AnyList* o )
		{
			sw = SIDL_IMPL_INTERFACE( ISwitch );
			return true;
		}


		void Shut ( )
		{
			sw = NULL;
		}


		int Lua_rand ( lua_State* L )
		{
			lua_pushnumber( L, util::Randf() );
			return 1;
		}


		int Lua_srand ( lua_State* L )
		{
			lua_pushnumber( L, util::Randsf() );
			return 1;
		}


		int Lua_cos ( lua_State* L )
		{
			float a = (float)luaT_checknumber( L, 1 );
			lua_pushnumber( L, Cosf( a ) );
			return 1;
		}


		int Lua_sin ( lua_State* L )
		{
			float a = (float)luaT_checknumber( L, 1 );
			lua_pushnumber( L, Sinf( a ) );
			return 1;
		}


		int Lua_tan ( lua_State* L )
		{
			float a = (float)luaT_checknumber( L, 1 );
			lua_pushnumber( L, Tanf( a ) );
			return 1;
		}

		int Lua_sqrt ( lua_State* L )
		{
			float v= (float)luaT_checknumber( L, 1 );
			lua_pushnumber( L, Sqrtf( v ) );
			return 1;
		}


		template<typename T> inline
		T*	push_new_obj ( lua_State* L, uint inTypeID, pcstr inFilter )
		{
			void* p = luaT_swpushdata( L, inTypeID, NULL, sizeof(T), sw, inFilter );
			SU_REQUIRE_OR_NULL( p );
			luaT_swdatactor<T>( L, -1, inTypeID );
			return (T*)p;
		}
		inline Vec2* push_new_vec2 ( lua_State* L )	{
			return push_new_obj<Vec2>( L, SWDATA_VEC2_TYPEID, "LuaVec2_*" );
		}
		inline Vec3* push_new_vec3 ( lua_State* L )	{
			return push_new_obj<Vec3>( L, SWDATA_VEC3_TYPEID, "LuaVec3_*" );
		}
		inline Vec4* push_new_vec4 ( lua_State* L )	{
			return push_new_obj<Vec4>( L, SWDATA_VEC4_TYPEID, "LuaVec4_*" );
		}
		inline Quat* push_new_quat ( lua_State* L ) {
			return push_new_obj<Quat>( L, SWDATA_QUAT_TYPEID, "LuaQuat_*" );
		}
		inline Matrix* push_new_matrix ( lua_State* L )	{
			return push_new_obj<Matrix>( L, SWDATA_MATRIX_TYPEID, "LuaMat_*" );
		}


		inline Vec2* get_vec2 ( lua_State* L, int inIdx )
		{
			luaT_checkudata( L, 1 );
			return luaT_swtodata<Vec2>( L, inIdx, SWDATA_VEC2_TYPEID );
		}
		inline Vec3* get_vec3 ( lua_State* L, int inIdx )
		{
			luaT_checkudata( L, 1 );
			return luaT_swtodata<Vec3>( L, inIdx, SWDATA_VEC3_TYPEID );
		}
		inline Vec4* get_vec4 ( lua_State* L, int inIdx )
		{
			luaT_checkudata( L, 1 );
			return luaT_swtodata<Vec4>( L, inIdx, SWDATA_VEC4_TYPEID );
		}
		inline Quat* get_quat ( lua_State* L, int inIdx )
		{
			luaT_checkudata( L, 1 );
			return luaT_swtodata<Quat>( L, inIdx, SWDATA_QUAT_TYPEID );
		}
		inline Matrix* get_matrix ( lua_State* L, int inIdx )
		{
			luaT_checkudata( L, 1 );
			return luaT_swtodata<Matrix>( L, inIdx, SWDATA_MATRIX_TYPEID );
		}


		inline bool is_vec2 ( lua_State* L, int inIdx )	{
			return ( luaT_swdatatypeid( L, inIdx ) == SWDATA_VEC2_TYPEID );
		}
		inline bool is_vec3 ( lua_State* L, int inIdx )	{
			return ( luaT_swdatatypeid( L, inIdx ) == SWDATA_VEC3_TYPEID );
		}
		inline bool is_vec4 ( lua_State* L, int inIdx )	{
			return ( luaT_swdatatypeid( L, inIdx ) == SWDATA_VEC4_TYPEID );
		}
		inline bool is_quat ( lua_State* L, int inIdx )	{
			return ( luaT_swdatatypeid( L, inIdx ) == SWDATA_QUAT_TYPEID );
		}
		inline bool is_matrix ( lua_State* L, int inIdx )	{
			return ( luaT_swdatatypeid( L, inIdx ) == SWDATA_MATRIX_TYPEID );
		}


		void	table_safe_value	(	float*		outValue,
										lua_State*	L,
										int			inTableIdx,
										int			inAsKey		)
		{
			SU_ASSERT( lua_istable( L, 1 ) );
			SU_ASSERT( outValue );

			lua_rawgeti( L, inTableIdx, inAsKey );
			if( lua_isnumber( L, -1 ) )
				*outValue = lua_tonumber( L, -1 );
			lua_pop( L, 1 );
		}


		//
		// --- Vec2
		int Lua_newVec2 ( lua_State* L )
		{
			int argc = lua_gettop( L );
			Vec2* v = push_new_vec2( L );
			SU_REQUIRE_OR_ZERO( v );
			
			if( argc > 0 )
			{
				if( lua_istable( L, 1 ) )
				{
					table_safe_value( &(v->x), L, 1, 1 );
					table_safe_value( &(v->y), L, 1, 2 );
				}
				else
				{
					v->x = (float)luaT_checknumber( L, 1 );
					v->y = (float)luaT_optnumber( L, 2, 0.0 );
				}
			}
			return 1;
		}

		int LuaVec2_x ( lua_State* L )
		{
			Vec2* v = get_vec2( L, 1 );
			SU_REQUIRE_OR_ZERO( v );
			if( lua_gettop( L ) > 1 )
			{
				v->x = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, v->x );
			return 1;
		}
		int LuaVec2_y ( lua_State* L )
		{
			Vec2* v = get_vec2( L, 1 );
			SU_REQUIRE_OR_ZERO( v );
			if( lua_gettop( L ) > 1 )
			{
				v->y = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, v->y );
			return 1;
		}
		int LuaVec2_yx ( lua_State* L )
		{
			Vec2* i = get_vec2( L, 1 );
			SU_REQUIRE_OR_ZERO( i );
			Vec2* o = push_new_vec2( L );
			SU_REQUIRE_OR_ZERO( o );
			o->x = i->y;
			o->y = i->x;
			return 1;
		}
		int LuaVec2_asTable ( lua_State* L )
		{
			Vec2* i = get_vec2( L, 1 );
			SU_REQUIRE_OR_ZERO( i );

			lua_newtable( L );
			lua_pushnumber( L, 1 );	lua_pushnumber( L, i->x );	lua_settable( L, -3 );
			lua_pushnumber( L, 2 );	lua_pushnumber( L, i->y );	lua_settable( L, -3 );
			return 1;
		}


		//
		// --- Vec3
		int Lua_newVec3 ( lua_State* L )
		{
			int argc = lua_gettop( L );
			Vec3* v = push_new_vec3( L );
			SU_REQUIRE_OR_ZERO( v );

			if( argc > 0 )
			{
				if( lua_istable( L, 1 ) )
				{
					table_safe_value( &(v->x), L, 1, 1 );
					table_safe_value( &(v->y), L, 1, 2 );
					table_safe_value( &(v->z), L, 1, 3 );
				}
				else
				{
					v->x = (float)luaT_checknumber( L, 1 );
					v->y = (float)luaT_optnumber( L, 2, 0.0 );
					v->z = (float)luaT_optnumber( L, 3, 0.0 );
				}
			}
			return 1;
		}
		int LuaVec3_x ( lua_State* L )
		{
			Vec3* v = get_vec3( L, 1 );
			SU_REQUIRE_OR_ZERO( v );

			if( lua_gettop( L ) > 1 )
			{
				v->x = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, v->x );
			return 1;
		}
		int LuaVec3_y ( lua_State* L )
		{
			Vec3* v = get_vec3( L, 1 );
			SU_REQUIRE_OR_ZERO( v );
			if( lua_gettop( L ) > 1 )
			{
				v->y = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, v->y );
			return 1;
		}
		int LuaVec3_z ( lua_State* L )
		{
			Vec3* v = get_vec3( L, 1 );
			SU_REQUIRE_OR_ZERO( v );
			if( lua_gettop( L ) > 1 )
			{
				v->z = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, v->z );
			return 1;
		}
		
		int LuaVec3_xzy ( lua_State* L )
		{
			Vec3* v = get_vec3( L, 1 );
			Vec3* o = push_new_vec3( L );
			SU_REQUIRE_OR_ZERO( v && o );
			o->x = v->x;
			o->y = v->z;
			o->z = v->y;
			return 1;
		}
		int LuaVec3_yxz ( lua_State* L )
		{
			Vec3* v = get_vec3( L, 1 );
			Vec3* o = push_new_vec3( L );
			SU_REQUIRE_OR_ZERO( v && o );
			o->x = v->y;
			o->y = v->x;
			o->z = v->z;
			return 1;
		}
		int LuaVec3_yzx ( lua_State* L )
		{
			Vec3* v = get_vec3( L, 1 );
			Vec3* o = push_new_vec3( L );
			SU_REQUIRE_OR_ZERO( v && o );
			o->x = v->y;
			o->y = v->z;
			o->z = v->x;
			return 1;
		}
		int LuaVec3_zxy ( lua_State* L )
		{
			Vec3* v = get_vec3( L, 1 );
			Vec3* o = push_new_vec3( L );
			SU_REQUIRE_OR_ZERO( v && o );
			o->x = v->z;
			o->y = v->x;
			o->z = v->y;
			return 1;
		}
		int LuaVec3_zyx ( lua_State* L )
		{
			Vec3* v = get_vec3( L, 1 );
			Vec3* o = push_new_vec3( L );
			SU_REQUIRE_OR_ZERO( v && o );
			o->x = v->z;
			o->y = v->y;
			o->z = v->x;
			return 1;
		}
		int LuaVec3_asTable ( lua_State* L )
		{
			Vec3* i = get_vec3( L, 1 );
			SU_REQUIRE_OR_ZERO( i );

			lua_newtable( L );
			lua_pushnumber( L, 1 );	lua_pushnumber( L, i->x );	lua_settable( L, -3 );
			lua_pushnumber( L, 2 );	lua_pushnumber( L, i->y );	lua_settable( L, -3 );
			lua_pushnumber( L, 3 );	lua_pushnumber( L, i->z );	lua_settable( L, -3 );
			return 1;
		}


		//
		// --- Vec4
		int Lua_newVec4 ( lua_State* L )
		{
			int argc = lua_gettop( L );
			Vec4* v = push_new_vec4( L );
			SU_REQUIRE_OR_ZERO( v );
			
			if( argc > 0 )
			{
				if( lua_istable( L, 1 ) )
				{
					table_safe_value( &(v->x), L, 1, 1 );
					table_safe_value( &(v->y), L, 1, 2 );
					table_safe_value( &(v->z), L, 1, 3 );
					table_safe_value( &(v->w), L, 1, 4 );
				}
				else
				{
					v->x = (float)luaT_checknumber( L, 1 );
					v->y = (float)luaT_optnumber( L, 2, 0.0 );
					v->z = (float)luaT_optnumber( L, 3, 0.0 );
					v->w = (float)luaT_optnumber( L, 4, 0.0 );
				}
			}
			return 1;
		}
		int LuaVec4_x ( lua_State* L )
		{
			Vec4* v = get_vec4( L, 1 );
			SU_REQUIRE_OR_ZERO( v );
			if( lua_gettop( L ) > 1 )
			{
				v->x = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, v->x );
			return 1;
		}
		int LuaVec4_y ( lua_State* L )
		{
			Vec4* v = get_vec4( L, 1 );
			SU_REQUIRE_OR_ZERO( v );
			if( lua_gettop( L ) > 1 )
			{
				v->y = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, v->y );
			return 1;
		}
		int LuaVec4_z ( lua_State* L )
		{
			Vec4* v = get_vec4( L, 1 );
			SU_REQUIRE_OR_ZERO( v );
			if( lua_gettop( L ) > 1 )
			{
				v->z = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, v->z );
			return 1;
		}
		int LuaVec4_w ( lua_State* L )
		{
			Vec4* v = get_vec4( L, 1 );
			SU_REQUIRE_OR_ZERO( v );
			if( lua_gettop( L ) > 1 )
			{
				v->w = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, v->w );
			return 1;
		}
		int LuaVec4_asTable ( lua_State* L )
		{
			Vec4* i = get_vec4( L, 1 );
			SU_REQUIRE_OR_ZERO( i );

			lua_newtable( L );
			lua_pushnumber( L, 1 );	lua_pushnumber( L, i->x );	lua_settable( L, -3 );
			lua_pushnumber( L, 2 );	lua_pushnumber( L, i->y );	lua_settable( L, -3 );
			lua_pushnumber( L, 3 );	lua_pushnumber( L, i->z );	lua_settable( L, -3 );
			lua_pushnumber( L, 4 );	lua_pushnumber( L, i->w );	lua_settable( L, -3 );
			return 1;
		}


		//
		// --- Quat
		int Lua_newQuat ( lua_State* L )
		{
			int argc = lua_gettop( L );
			Quat* q = push_new_quat( L );
			SU_REQUIRE_OR_ZERO( q );

			if( argc > 0 )
			{
				if( lua_istable( L, 1 ) )
				{
					table_safe_value( &(q->x), L, 1, 1 );
					table_safe_value( &(q->y), L, 1, 2 );
					table_safe_value( &(q->z), L, 1, 3 );
					table_safe_value( &(q->w), L, 1, 4 );
				}
				else
				{
					q->x = (float)luaT_checknumber( L, 1 );
					q->y = (float)luaT_optnumber( L, 2, 0.0 );
					q->z = (float)luaT_optnumber( L, 3, 0.0 );
					q->w = (float)luaT_optnumber( L, 4, 0.0 );
				}
			}
			return 1;
		}
		int LuaQuat_x ( lua_State* L )
		{
			Quat* q = get_quat( L, 1 );
			SU_REQUIRE_OR_ZERO( q );
			if( lua_gettop( L ) > 1 )
			{
				q->x = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, q->x );
			return 1;
		}
		int LuaQuat_y ( lua_State* L )
		{
			Quat* q = get_quat( L, 1 );
			SU_REQUIRE_OR_ZERO( q );
			if( lua_gettop( L ) > 1 )
			{
				q->y = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, q->y );
			return 1;
		}
		int LuaQuat_z ( lua_State* L )
		{
			Quat* q = get_quat( L, 1 );
			SU_REQUIRE_OR_ZERO( q );
			if( lua_gettop( L ) > 1 )
			{
				q->z = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, q->z );
			return 1;
		}
		int LuaQuat_w ( lua_State* L )
		{
			Quat* q = get_quat( L, 1 );
			SU_REQUIRE_OR_ZERO( q );
			if( lua_gettop( L ) > 1 )
			{
				q->w = (float)luaT_checknumber( L, 2 );
				return 0;
			}
			lua_pushnumber( L, q->w );
			return 1;
		}
		int LuaQuat_xzyw ( lua_State* L )
		{
			Quat* q = get_quat( L, 1 );
			Quat* o = push_new_quat( L );
			SU_REQUIRE_OR_ZERO( q && o );
			o->x = q->x;
			o->y = q->z;
			o->z = q->y;
			o->w = q->w;
			return 1;
		}
		
		int LuaQuat_asTable ( lua_State* L )
		{
			Quat* i = get_quat( L, 1 );
			SU_REQUIRE_OR_ZERO( i );

			lua_newtable( L );
			lua_pushnumber( L, 1 );	lua_pushnumber( L, i->x );	lua_settable( L, -3 );
			lua_pushnumber( L, 2 );	lua_pushnumber( L, i->y );	lua_settable( L, -3 );
			lua_pushnumber( L, 3 );	lua_pushnumber( L, i->z );	lua_settable( L, -3 );
			lua_pushnumber( L, 4 );	lua_pushnumber( L, i->w );	lua_settable( L, -3 );
			return 1;
		}

		//
		// --- Matrix
		int Lua_newMatrix ( lua_State* L )
		{
			int argc = lua_gettop( L );
			Matrix* m = push_new_matrix( L );
			SU_REQUIRE_OR_ZERO( m );

			if( argc > 0 )
			{
				if( lua_istable( L, 1 ) )
				{
					float* pData = (float*)m;
					for( uint i=0; i<16; i++ )
						table_safe_value( pData+i, L, 1, i+1 );
				}
			}
			return 1;
		}
		int LuaMat_get ( lua_State* L )
		{
			Matrix* m = get_matrix( L, 1 );
			SU_REQUIRE_OR_ZERO( m );
			int r = luaT_checkinteger( L, 1 );
			int c = luaT_checkinteger( L, 2 );
			lua_pushnumber( L, (*m)( r, c ) );
			return 1;
		}
		int LuaMat_set ( lua_State* L )
		{
			Matrix* m = get_matrix( L, 1 );
			SU_REQUIRE_OR_ZERO( m );
			int r = luaT_checkinteger( L, 1 );
			int c = luaT_checkinteger( L, 2 );
			(*m)( r, c ) = luaT_checknumber( L, 3 );
			return 0;
		}
		int LuaMat_asTable ( lua_State* L )
		{
			Matrix* i = get_matrix( L, 1 );
			SU_REQUIRE_OR_ZERO( i );
			
			float* pData = (float*)i;
			lua_newtable( L );
			for( uint k=0; k<16; k++ )
			{
				lua_pushnumber( L, k+1 );
				lua_pushnumber( L, pData[k] );
				lua_settable( L, -3 );
			}

			return 1;
		}




		//
		// -- Operations

		int Lua_copy ( lua_State* L )
		{
		// (outGiven) ? copy( out, in ) : out = copy( in )
			bool outGiven = ( lua_gettop( L ) >= 2 );
			if( is_vec2( L, 1 ) )
			{
				Vec2* o = (outGiven) ? get_vec2( L, 1 ) : push_new_vec2( L );
				Vec2* i = (outGiven) ? get_vec2( L, 2 ) : get_vec2( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				Vec2Copy( o, i );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* o = (outGiven) ? get_vec3( L, 1 ) : push_new_vec3( L );
				Vec3* i = (outGiven) ? get_vec3( L, 2 ) : get_vec3( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				Vec3Copy( o, i );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* o = (outGiven) ? get_vec4( L, 1 ) : push_new_vec4( L );
				Vec4* i = (outGiven) ? get_vec4( L, 2 ) : get_vec4( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				Vec4Copy( o, i );
				return (outGiven) ? 0 : 1;
			}
			else if( is_quat( L, 1 ) )
			{
				Quat* o = (outGiven) ? get_quat( L, 1 ) : push_new_quat( L );
				Quat* i = (outGiven) ? get_quat( L, 2 ) : get_quat( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				QuatCopy( o, i );
				return (outGiven) ? 0 : 1;
			}
			else if( is_matrix( L, 1 ) )
			{
				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = (outGiven) ? get_matrix( L, 2 ) : get_matrix( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				MatrixCopy( o, i );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}

		int Lua_zero ( lua_State* L )
		{
		// zero( out )
			if( is_vec2( L, 1 ) )
			{
				Vec2* v = get_vec2( L, 1 );
				SU_REQUIRE_OR_ZERO( v );
				Vec2Zero( v );
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* v = get_vec3( L, 1 );
				SU_REQUIRE_OR_ZERO( v );
				Vec3Zero( v );
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* v = get_vec4( L, 1 );
				SU_REQUIRE_OR_ZERO( v );
				Vec4Zero( v );
			}
			else if( is_quat( L, 1 ) )
			{
				Quat* q = get_quat( L, 1 );
				SU_REQUIRE_OR_ZERO( q );
				QuatZero( q );
			}
			else if( is_matrix( L, 1 ) )
			{
				Matrix* m = get_matrix( L, 1 );
				SU_REQUIRE_OR_ZERO( m );
				MatrixZero( m );
			}
			return 0;
		}

		int Lua_identity ( lua_State* L )
		{
		// identity( out )
			if( is_quat( L, 1 ) )
			{
				Quat* q = get_quat( L, 1 );
				SU_REQUIRE_OR_ZERO( q );
				QuatIdentity( q );
			}
			else if( is_matrix( L, 1 ) )
			{
				Matrix* m = get_matrix( L, 1 );
				SU_REQUIRE_OR_ZERO( m );
				MatrixIdentity( m );
			}
			return 0;
		}

		int Lua_isIdentity ( lua_State* L )
		{
		// ii = isIdentity( in )
			if( is_quat( L, 1 ) )
			{
				Quat* q = get_quat( L, 1 );
				SU_REQUIRE_OR_ZERO( q );
				lua_pushboolean( L, QuatIsIdentity( q ) );
				return 1;
			}
			else if( is_matrix( L, 1 ) )
			{
				Matrix* m = get_matrix( L, 1 );
				SU_REQUIRE_OR_ZERO( m );
				lua_pushboolean( L, MatrixIsIdentity( m ) );
				return 1;
			}
			return 0;
		}

		int Lua_norm ( lua_State* L )
		{
		// n = norm( in )
			if( is_vec2( L, 1 ) )
			{
				Vec2* v = get_vec2( L, 1 );
				SU_REQUIRE_OR_ZERO( v );
				lua_pushnumber( L, Vec2Norm( v ) );
				return 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* v = get_vec3( L, 1 );
				SU_REQUIRE_OR_ZERO( v );
				lua_pushnumber( L, Vec3Norm( v ) );
				return 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* v = get_vec4( L, 1 );
				SU_REQUIRE_OR_ZERO( v );
				lua_pushnumber( L, Vec4Norm( v ) );
				return 1;
			}
			else if( is_quat( L, 1 ) )
			{
				Quat* q = get_quat( L, 1 );
				SU_REQUIRE_OR_ZERO( q );
				lua_pushnumber( L, QuatNorm( q ) );
				return 1;
			}
			return 0;
		}

		int Lua_norm2 ( lua_State* L )
		{
		// n2 = norm2( in )
			if( is_vec2( L, 1 ) )
			{
				Vec2* v = get_vec2( L, 1 );
				SU_REQUIRE_OR_ZERO( v );
				lua_pushnumber( L, Vec2Norm2( v ) );
				return 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* v = get_vec3( L, 1 );
				SU_REQUIRE_OR_ZERO( v );
				lua_pushnumber( L, Vec3Norm2( v ) );
				return 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* v = get_vec4( L, 1 );
				SU_REQUIRE_OR_ZERO( v );
				lua_pushnumber( L, Vec4Norm2( v ) );
				return 1;
			}
			else if( is_quat( L, 1 ) )
			{
				Quat* q = get_quat( L, 1 );
				SU_REQUIRE_OR_ZERO( q );
				lua_pushnumber( L, QuatNorm2( q ) );
				return 1;
			}
			return 0;
		}

		int Lua_normalize ( lua_State* L )
		{
		// (outGiven) ? normalize( out, in ) : out = normalize( in )
			bool outGiven = ( lua_gettop( L ) >= 2 );
			if( is_vec2( L, 1 ) )
			{
				Vec2* o = (outGiven) ? get_vec2( L, 1 ) : push_new_vec2( L );
				Vec2* i = (outGiven) ? get_vec2( L, 2 ) : get_vec2( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				Vec2Normalize( o, i );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* o = (outGiven) ? get_vec3( L, 1 ) : push_new_vec3( L );
				Vec3* i = (outGiven) ? get_vec3( L, 2 ) : get_vec3( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				Vec3Normalize( o, i );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* o = (outGiven) ? get_vec4( L, 1 ) : push_new_vec4( L );
				Vec4* i = (outGiven) ? get_vec4( L, 2 ) : get_vec4( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				Vec4Normalize( o, i );
				return (outGiven) ? 0 : 1;
			}
			else if( is_quat( L, 1 ) )
			{
				Quat* o = (outGiven) ? get_quat( L, 1 ) : push_new_quat( L );
				Quat* i = (outGiven) ? get_quat( L, 2 ) : get_quat( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				QuatNormalize( o, i );
				return (outGiven) ? 0 : 1;
			}
			else if( is_matrix( L, 1 ) )
			{
				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = (outGiven) ? get_matrix( L, 2 ) : get_matrix( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				MatrixNormalize( o, i );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_orthoNormalize ( lua_State* L )
		{
			if( is_matrix( L, 1 ) )
			{
			// (outGiven) ? orthoNormalize( out, in ) : out = orthoNormalize( in )
				bool outGiven = ( lua_gettop( L ) >= 2 );
				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = (outGiven) ? get_matrix( L, 2 ) : get_matrix( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				MatrixOrthoNormalize( o, i );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}

		int Lua_cross ( lua_State* L )
		{
			if( is_vec2( L, 1 ) )
			{
			// dot( inA, inB )
				Vec2* a = get_vec2( L, 1 );
				Vec2* b = get_vec2( L, 2 );
				SU_REQUIRE_OR_ZERO( a && b );
				lua_pushnumber( L, Vec2Cross( a, b ) );
				return 1;
			}
			else if( is_vec3( L, 1 ) )
			{
			// (outGiven) ? dot( out, inA, inB ) : out = dot( inA, inB )
				bool outGiven = ( lua_gettop( L ) >= 3 );
				int si = (outGiven) ? 2:1;
				Vec3* o = (outGiven) ? get_vec3( L, 1 ) : push_new_vec3( L );
				Vec3* a = get_vec3( L, si+0 );
				Vec3* b = get_vec3( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec3Cross( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec4( L, 1 ) )
			{
			// (outGiven) ? dot( out, inA, inB, inC ) : out = dot( inA, inB, inC )
				bool outGiven = ( lua_gettop( L ) >= 4 );
				int si = (outGiven) ? 2:1;

				Vec4* o = (outGiven) ? get_vec4( L, 1 ) : push_new_vec4( L );
				Vec4* a = get_vec4( L, si+0 );
				Vec4* b = get_vec4( L, si+1 );
				Vec4* c = get_vec4( L, si+2 );
				SU_REQUIRE_OR_ZERO( a && b && c && o );
				Vec4Cross( o, a, b, c );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_dot ( lua_State* L )
		{
		// dot( inA, inB )
			if( is_vec2( L, 1 ) )
			{
				Vec2* a = get_vec2( L, 1 );
				Vec2* b = get_vec2( L, 2 );
				SU_REQUIRE_OR_ZERO( a && b );
				lua_pushnumber( L, Vec2Dot( a, b ) );
				return 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* a = get_vec3( L, 1 );
				Vec3* b = get_vec3( L, 2 );
				SU_REQUIRE_OR_ZERO( a && b );
				lua_pushnumber( L, Vec3Dot( a, b ) );
				return 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* a = get_vec4( L, 1 );
				Vec4* b = get_vec4( L, 2 );
				SU_REQUIRE_OR_ZERO( a && b );
				lua_pushnumber( L, Vec4Dot( a, b ) );
				return 1;
			}
			else if( is_quat( L, 1 ) )
			{
				Quat* a = get_quat( L, 1 );
				Quat* b = get_quat( L, 2 );
				SU_REQUIRE_OR_ZERO( a && b );
				lua_pushnumber( L, QuatDot( a, b ) );
				return 1;
			}
			return 0;
		}
		int Lua_near ( lua_State* L )
		{
		// near( inA, inB ) : near( inA, inB, eps )
			if( is_vec2( L, 1 ) )
			{
				Vec2* a = get_vec2( L, 1 );
				Vec2* b = get_vec2( L, 2 );
				SU_REQUIRE_OR_ZERO( a && b );
				if( lua_isnoneornil( L, 3 ) )
					lua_pushboolean( L, Vec2Near( a, b ) );
				else
					lua_pushboolean( L, Vec2Near( a, b, luaT_checknumber( L, 3 ) ) );
				return 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* a = get_vec3( L, 1 );
				Vec3* b = get_vec3( L, 2 );
				SU_REQUIRE_OR_ZERO( a && b );
				if( lua_isnoneornil( L, 3 ) )
					lua_pushboolean( L, Vec3Near( a, b ) );
				else
					lua_pushboolean( L, Vec3Near( a, b, luaT_checknumber( L, 3 ) ) );
				return 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* a = get_vec4( L, 1 );
				Vec4* b = get_vec4( L, 2 );
				SU_REQUIRE_OR_ZERO( a && b );
				if( lua_isnoneornil( L, 3 ) )
					lua_pushboolean( L, Vec4Near( a, b ) );
				else
					lua_pushboolean( L, Vec4Near( a, b, luaT_checknumber( L, 3 ) ) );
				return 1;
			}
			else if( is_quat( L, 1 ) )
			{
				Quat* a = get_quat( L, 1 );
				Quat* b = get_quat( L, 2 );
				SU_REQUIRE_OR_ZERO( a && b );
				if( lua_isnoneornil( L, 3 ) )
					lua_pushboolean( L, QuatNear( a, b ) );
				else
					lua_pushboolean( L, QuatNear( a, b, luaT_checknumber( L, 3 ) ) );
				return 1;
			}
			return 0;
		}

		int Lua_add ( lua_State* L )
		{
		// (outGiven) ?  add( out, inA, inB ) : out = add( inA, inB )
			bool outGiven = ( lua_gettop( L ) >= 3 );
			int si = (outGiven) ? 2:1;
			if( is_vec2( L, 1 ) )
			{
				Vec2* o = (outGiven) ? get_vec2( L, 1 ) : push_new_vec2( L );
				Vec2* a = get_vec2( L, si+0 );
				Vec2* b = get_vec2( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec2Add( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* o = (outGiven) ? get_vec3( L, 1 ) : push_new_vec3( L );
				Vec3* a = get_vec3( L, si+0 );
				Vec3* b = get_vec3( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec3Add( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* o = (outGiven) ? get_vec4( L, 1 ) : push_new_vec4( L );
				Vec4* a = get_vec4( L, si+0 );
				Vec4* b = get_vec4( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec4Add( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_quat( L, 1 ) )
			{
				Quat* o = (outGiven) ? get_quat( L, 1 ) : push_new_quat( L );
				Quat* a = get_quat( L, si+0 );
				Quat* b = get_quat( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				QuatAdd( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_matrix( L, 1 ) )
			{
				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* a = get_matrix( L, si+0 );
				Matrix* b = get_matrix( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				MatrixAdd( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_sub ( lua_State* L )
		{
		// (outGiven) ?  sub( out, inA, inB ) : out = sub( inA, inB )
			bool outGiven = ( lua_gettop( L ) >= 3 );
			int si = (outGiven) ? 2:1;
			if( is_vec2( L, 1 ) )
			{
				Vec2* o = (outGiven) ? get_vec2( L, 1 ) : push_new_vec2( L );
				Vec2* a = get_vec2( L, si+0 );
				Vec2* b = get_vec2( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec2Sub( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* o = (outGiven) ? get_vec3( L, 1 ) : push_new_vec3( L );
				Vec3* a = get_vec3( L, si+0 );
				Vec3* b = get_vec3( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec3Sub( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* o = (outGiven) ? get_vec4( L, 1 ) : push_new_vec4( L );
				Vec4* a = get_vec4( L, si+0 );
				Vec4* b = get_vec4( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec4Sub( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_quat( L, 1 ) )
			{
				Quat* o = (outGiven) ? get_quat( L, 1 ) : push_new_quat( L );
				Quat* a = get_quat( L, si+0 );
				Quat* b = get_quat( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				QuatSub( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_matrix( L, 1 ) )
			{
				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* a = get_matrix( L, si+0 );
				Matrix* b = get_matrix( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				MatrixSub( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_mul ( lua_State* L )
		{
		// (outGiven) ?  mul( out, inA, inB ) : out = mul( inA, inB )
			bool outGiven = ( lua_gettop( L ) >= 3 );
			int si = (outGiven) ? 2:1;
			if( is_vec2( L, 1 ) )
			{
				Vec2* o = (outGiven) ? get_vec2( L, 1 ) : push_new_vec2( L );
				Vec2* a = get_vec2( L, si+0 );
				Vec2* b = get_vec2( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec2Mul( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* o = (outGiven) ? get_vec3( L, 1 ) : push_new_vec3( L );
				Vec3* a = get_vec3( L, si+0 );
				Vec3* b = get_vec3( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec3Mul( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* o = (outGiven) ? get_vec4( L, 1 ) : push_new_vec4( L );
				Vec4* a = get_vec4( L, si+0 );
				Vec4* b = get_vec4( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec4Mul( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_quat( L, 1 ) )
			{
				Quat* o = (outGiven) ? get_quat( L, 1 ) : push_new_quat( L );
				Quat* a = get_quat( L, si+0 );
				Quat* b = get_quat( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				QuatMul( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_matrix( L, 1 ) )
			{
				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* a = get_matrix( L, si+0 );
				Matrix* b = get_matrix( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				MatrixMul( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}

		int Lua_min ( lua_State* L )
		{
		// (outGiven) ?  min( out, inA, inB ) : out = min( inA, inB )
			bool outGiven = ( lua_gettop( L ) >= 3 );
			int si = (outGiven) ? 2:1;
			if( is_vec2( L, 1 ) )
			{
				Vec2* o = (outGiven) ? get_vec2( L, 1 ) : push_new_vec2( L );
				Vec2* a = get_vec2( L, si+0 );
				Vec2* b = get_vec2( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec2Min( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* o = (outGiven) ? get_vec3( L, 1 ) : push_new_vec3( L );
				Vec3* a = get_vec3( L, si+0 );
				Vec3* b = get_vec3( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec3Min( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* o = (outGiven) ? get_vec4( L, 1 ) : push_new_vec4( L );
				Vec4* a = get_vec4( L, si+0 );
				Vec4* b = get_vec4( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec4Min( o, a, b );
				return (outGiven) ? 0 : 1;
			}

			float a = luaT_checknumber( L, 1 );
			float b = luaT_checknumber( L, 2 );
			lua_pushnumber( L, Min( a, b ) );
			return 1;
		}

		int Lua_max ( lua_State* L )
		{
		// (outGiven) ?  max( out, inA, inB ) : out = max( inA, inB )
			bool outGiven = ( lua_gettop( L ) >= 3 );
			int si = (outGiven) ? 2:1;
			if( is_vec2( L, 1 ) )
			{
				Vec2* o = (outGiven) ? get_vec2( L, 1 ) : push_new_vec2( L );
				Vec2* a = get_vec2( L, si+0 );
				Vec2* b = get_vec2( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec2Max( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* o = (outGiven) ? get_vec3( L, 1 ) : push_new_vec3( L );
				Vec3* a = get_vec3( L, si+0 );
				Vec3* b = get_vec3( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec3Max( o, a, b );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* o = (outGiven) ? get_vec4( L, 1 ) : push_new_vec4( L );
				Vec4* a = get_vec4( L, si+0 );
				Vec4* b = get_vec4( L, si+1 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				Vec4Max( o, a, b );
				return (outGiven) ? 0 : 1;
			}

			float a = luaT_checknumber( L, 1 );
			float b = luaT_checknumber( L, 2 );
			lua_pushnumber( L, Max( a, b ) );
			return 1;
		}

		int Lua_scale ( lua_State* L )
		{
			if( is_vec2( L, 1 ) )
			{
			// (outGiven) ?  scale( out, in, inFlt ) : out = scale( in, inFlt )
				bool outGiven = ( lua_gettop( L ) >= 3 );
				Vec2* o = (outGiven) ? get_vec2( L, 1 ) : push_new_vec2( L );
				Vec2* a = (outGiven) ? get_vec2( L, 2 ) : get_vec2( L, 1 );
				SU_REQUIRE_OR_ZERO( a && o );
				float f = (outGiven) ? luaT_checknumber( L, 3 ) : luaT_checknumber( L, 2 );
				Vec2Scale( o, a, f );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec3( L, 1 ) )
			{
			// (outGiven) ?  scale( out, in, inFlt ) : out = scale( in, inFlt )
				bool outGiven = ( lua_gettop( L ) >= 3 );
				Vec3* o = (outGiven) ? get_vec3( L, 1 ) : push_new_vec3( L );
				Vec3* a = (outGiven) ? get_vec3( L, 2 ) : get_vec3( L, 1 );
				SU_REQUIRE_OR_ZERO( a && o );
				float f = (outGiven) ? luaT_checknumber( L, 3 ) : luaT_checknumber( L, 2 );
				Vec3Scale( o, a, f );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec4( L, 1 ) )
			{
			// (outGiven) ?  scale( out, in, inFlt ) : out = scale( in, inFlt )
				bool outGiven = ( lua_gettop( L ) >= 3 );
				Vec4* o = (outGiven) ? get_vec4( L, 1 ) : push_new_vec4( L );
				Vec4* a = (outGiven) ? get_vec4( L, 2 ) : get_vec4( L, 1 );
				SU_REQUIRE_OR_ZERO( a && o );
				float f = (outGiven) ? luaT_checknumber( L, 3 ) : luaT_checknumber( L, 2 );
				Vec4Scale( o, a, f );
				return (outGiven) ? 0 : 1;
			}
			else if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  scale( out, inM, x, y, z ) : out = scale( inM, x, y, z )
			// (outGiven) ?  scale( out, inM, inVec3 ) : out = scale( inM, inVec3 )
			// (outGiven) ?  scale( out, inM, inMatT ) : out = scale( inM, inMatT )
				
				int argc = lua_gettop( L );
				bool outGiven = is_matrix( L, argc ) ? ( argc == 3 ) : ( is_matrix( L, 1 ) && is_matrix( L, 2 ) );
				int si = (outGiven) ? 2:1;

				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = get_matrix( L, si+0 );
				SU_REQUIRE_OR_ZERO( o && i );

				if( is_vec3( L, si+1 ) )
				{
					Vec3* v = get_vec3( L, si+1 );
					SU_REQUIRE_OR_ZERO( v );
					MatrixScale( o, i, v );
				}
				else if( is_matrix( L, si+1 ) )
				{
					Matrix* mt = get_matrix( L, si+1 );
					SU_REQUIRE_OR_ZERO( mt );
					MatrixScale( o, i, mt );
				}
				else
				{
					float x = luaT_checknumber( L, si+1 );
					float y = luaT_checknumber( L, si+2 );
					float z = luaT_checknumber( L, si+3 );
					MatrixScale( o, i, x, y, z );
				}
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}

		int Lua_apply ( lua_State* L )
		{
			if( is_vec2( L, 1 ) )
			{
				// (outGiven) ?  apply( out, inVec2, inMat ) : out = apply( inVec2, inMat )
				
				bool outGiven = ( lua_gettop( L ) >= 3 );
				int si = (outGiven) ? 2:1;
				Vec2* o	  = (outGiven) ? get_vec2( L, 1 ) : push_new_vec2( L );
				Vec2* i	  = get_vec2	( L, si+0 );
				Matrix* m = get_matrix	( L, si+1 );
				SU_REQUIRE_OR_ZERO( m && i && o );
				Vec2Apply( o, i, m );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				// (outGiven) ?  apply( out, inVec3, inMat )		: out = apply( inVec3, inMat )
				// (outGiven) ?  apply( out, inVec3, inMat, inFlt )	: out = apply( inVec3, inMat, inFlt )
				// (outGiven) ?  apply( out, inVec3, inQuat )		: out = apply( inVec3, inQuat )

				bool outGiven = is_vec3( L, 2 );
				int si = (outGiven) ? 2:1;
				
				Vec3* o = (outGiven) ? get_vec3( L, 1 ) : push_new_vec3( L );
				Vec3* i = get_vec3( L, si+0 );
				SU_REQUIRE_OR_ZERO( o );

				if( is_matrix( L, si+1 ) )
				{
					Matrix* m = get_matrix( L, si+1 );
					SU_REQUIRE_OR_ZERO( m );

					if( lua_isnoneornil( L, si+2 ) )
					{
						Vec3Apply( o, i, m );
					}
					else
					{
						float f = luaT_checknumber( L, si+2 );
						Vec3Apply( o, i, m, f );
					}
				}
				else
				{
					Quat* q = get_quat( L, si+1 );
					SU_REQUIRE_OR_ZERO( q );
					Vec3Apply( o, i, q );
				}
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				// (outGiven) ?  apply( out, inVec4, inMat )		: out = apply( inVec4, inMat )
				// (outGiven) ?  apply( out, inVec4, inMat, inFlt )	: out = apply( inVec4, inMat, inFlt )
				// (outGiven) ?  apply( out, inVec4, inQuat )		: out = apply( inVec4, inQuat )

				bool outGiven = is_vec4( L, 2 );
				int si = (outGiven) ? 2:1;
				
				Vec4* o = (outGiven) ? get_vec4( L, 1 ) : push_new_vec4( L );
				Vec4* i = get_vec4( L, si+0 );
				SU_REQUIRE_OR_ZERO( o && i );

				if( is_matrix( L, si+1 ) )
				{
					Matrix* m = get_matrix( L, si+1 );
					SU_REQUIRE_OR_ZERO( m );
					if( lua_isnoneornil( L, si+2 ) )
					{
						Vec4Apply( o, i, m );
					}
					else
					{
						float f = luaT_checknumber( L, si+2 );
						Vec4Apply( o, i, m, f );
					}
				}
				else
				{
					Quat* q = get_quat( L, si+1 );
					SU_REQUIRE_OR_ZERO( q );
					Vec4Apply( o, i, q );
				}
				return (outGiven) ? 0 : 1;
			}

			return 0;
		}
		int Lua_applyVector ( lua_State* L )
		{
			int argc = lua_gettop( L );
			if( is_vec2( L, 1 ) )
			{
			// (outGiven) ?  applyVector( out, inVec2, inMat ) : out = applyVector( inVec2, inMat )
				
				bool outGiven = ( argc >= 3 );
				int si = (outGiven) ? 2:1;
				Vec2* o	  = (outGiven) ? get_vec2( L, 1 ) : push_new_vec2( L );
				Vec2* i	  = get_vec2  ( L, si+0 );
				Matrix* m = get_matrix( L, si+1 );
				SU_REQUIRE_OR_ZERO( m && i && o );
				Vec2ApplyVector( o, i, m );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec3( L, 1 ) )
			{
			// (outGiven) ?  applyVector( out, inVec3, inMat )			: out = applyVector( inVec3, inMat )
			// (outGiven) ?  applyVector( out, inVec3, inMat, inFlt )	: out = applyVector( inVec3, inMat, inFlt )

				bool outGiven = is_vec3( L, 2 );
				int si = (outGiven) ? 2:1;
				
				Vec3* o	  = (outGiven) ? get_vec3( L, 1 ) : push_new_vec3( L );
				Vec3* i	  = get_vec3  ( L, si+0 );
				Matrix* m = get_matrix( L, si+1 );
				SU_REQUIRE_OR_ZERO( o && i && m );

				if( lua_isnoneornil( L, si+2 ) )
				{
					Vec3ApplyVector( o, i, m );
				}
				else
				{
					float f = luaT_checknumber( L, si+2 );
					Vec3ApplyVector( o, i, m, f );
				}
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec4( L, 1 ) )
			{
			// (outGiven) ?  applyVector( out, inVec4, inMat )			: out = applyVector( inVec4, inMat )
			// (outGiven) ?  applyVector( out, inVec4, inMat, inFlt )	: out = applyVector( inVec4, inMat, inFlt )

				bool outGiven = is_vec4( L, 2 );
				int si = (outGiven) ? 2:1;
				
				Vec4* o	  = (outGiven) ? get_vec4( L, 1 ) : push_new_vec4( L );
				Vec4* i	  = get_vec4  ( L, si+0 );
				Matrix* m = get_matrix( L, si+1 );
				SU_REQUIRE_OR_ZERO( o && i && m );

				if( lua_isnoneornil( L, si+2 ) )
				{
					Vec4ApplyVector( o, i, m );
				}
				else
				{
					float f = luaT_checknumber( L, si+2 );
					Vec4ApplyVector( o, i, m, f );
				}
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}

		int Lua_lerp ( lua_State* L )
		{
		// (outGiven) ?  lerp( out, in0, in1, inFlt ) : out = lerp( in0, in1, inFlt )

			bool outGiven = ( lua_gettop( L ) >= 4 );
			int si = (outGiven) ? 2:1;

			if( is_vec2( L, 1 ) )
			{
				Vec2* o = (outGiven) ? get_vec2( L, 1 ) : push_new_vec2( L );
				Vec2* a = get_vec2( L, si+0 );
				Vec2* b = get_vec2( L, si+1 );
				float f = luaT_checknumber( L, si+2 );
				SU_REQUIRE_OR_ZERO( o && a && b );
				Vec2Lerp( o, a, b, f );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec3( L, 1 ) )
			{
				Vec3* o = (outGiven) ? get_vec3( L, 1 ) : push_new_vec3( L );
				Vec3* a = get_vec3( L, si+0 );
				Vec3* b = get_vec3( L, si+1 );
				float f = luaT_checknumber( L, si+2 );
				SU_REQUIRE_OR_ZERO( o && a && b );
				Vec3Lerp( o, a, b, f );
				return (outGiven) ? 0 : 1;
			}
			else if( is_vec4( L, 1 ) )
			{
				Vec4* o = (outGiven) ? get_vec4( L, 1 ) : push_new_vec4( L );
				Vec4* a = get_vec4( L, si+0 );
				Vec4* b = get_vec4( L, si+1 );
				float f = luaT_checknumber( L, si+2 );
				SU_REQUIRE_OR_ZERO( o && a && b );
				Vec4Lerp( o, a, b, f );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}

		int Lua_slerp ( lua_State* L )
		{
		// (outGiven) ?  slerp( out, in0, in1, inFlt ) : out = slerp( in0, in1, inFlt )
			
			if( is_quat( L, 1 ) )
			{
				bool outGiven = ( lua_gettop( L ) >= 4 );
				int si = (outGiven) ? 2:1;

				Quat* o = (outGiven) ? get_quat( L, 1 ) : push_new_quat( L );
				Quat* a = get_quat( L, si+0 );
				Quat* b = get_quat( L, si+1 );
				float f = luaT_checknumber( L, si+2 );
				SU_REQUIRE_OR_ZERO( a && b && o );
				QuatSlerp( o, a, b, f );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}

		int Lua_inverse ( lua_State* L )
		{
			bool outGiven = ( lua_gettop( L ) >= 2 );
			if( is_quat( L, 1 ) )
			{
			// (outGiven) ?  inverse( out, in ) : out = inverse( in )
				
				Quat* o = (outGiven) ? get_quat( L, 1 ) : push_new_quat( L );
				Quat* i = (outGiven) ? get_quat( L, 2 ) : get_quat( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				QuatInverse( o, i );
				return (outGiven) ? 0 : 1;
			}
			else if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  det = inverse( out, in ) : out, det = inverse( in )

				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = (outGiven) ? get_matrix( L, 2 ) : get_matrix( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				float det;
				MatrixInverse( o, &det, i );
				lua_pushnumber( L, det );
				return (outGiven) ? 1 : 2;
			}
			return 0;
		}

		int Lua_fastInverse ( lua_State* L )
		{
		// (outGiven) ?  fastInverse( out, in ) : out = fastInverse( in )
			
			bool outGiven = ( lua_gettop( L ) >= 2 );
			if( is_quat( L, 1 ) )
			{
				Quat* o = (outGiven) ? get_quat( L, 1 ) : push_new_quat( L );
				Quat* i = (outGiven) ? get_quat( L, 2 ) : get_quat( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				QuatFastInverse( o, i );
				return (outGiven) ? 0 : 1;
			}
			else if( is_matrix( L, 1 ) )
			{
				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = (outGiven) ? get_matrix( L, 2 ) : get_matrix( L, 1 );
				SU_REQUIRE_OR_ZERO( i && o );
				MatrixFastInverse( o, i );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		
		int Lua_translate ( lua_State* L )
		{
			if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  translate( out, inM, x, y, z ) : out = translate( inM, x, y, z )
			// (outGiven) ?  translate( out, inM, inVec3 ) : out = translate( inM, inVec3 )
			// (outGiven) ?  translate( out, inM, inMatT ) : out = translate( inM, inMatT )
				
				int argc = lua_gettop( L );
				bool outGiven = is_matrix( L, argc ) ? ( argc == 3 ) : ( is_matrix( L, 1 ) && is_matrix( L, 2 ) );
				int si = (outGiven) ? 2:1;

				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = get_matrix( L, si+0 );
				SU_REQUIRE_OR_ZERO( o && i );

				if( is_vec3( L, si+1 ) )
				{
					Vec3* v = get_vec3( L, si+1 );
					MatrixTranslate( o, i, v );
				}
				else if( is_matrix( L, si+1 ) )
				{
					Matrix* mt = get_matrix( L, si+1 );
					MatrixTranslate( o, i, mt );
				}
				else
				{
					float x = luaT_checknumber( L, si+1 );
					float y = luaT_checknumber( L, si+2 );
					float z = luaT_checknumber( L, si+3 );
					MatrixTranslate( o, i, x, y, z );
				}
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_preTranslate ( lua_State* L )
		{
			if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  preTranslate( out, inM, x, y, z ) : out = preTranslate( inM, x, y, z )
			// (outGiven) ?  preTranslate( out, inM, inVec3 ) : out = preTranslate( inM, inVec3 )
			// (outGiven) ?  preTranslate( out, inM, inMatT ) : out = preTranslate( inM, inMatT )
				
				int argc = lua_gettop( L );
				bool outGiven = is_matrix( L, argc ) ? ( argc == 3 ) : ( is_matrix( L, 1 ) && is_matrix( L, 2 ) );
				int si = (outGiven) ? 2:1;

				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = get_matrix( L, si+0 );
				SU_REQUIRE_OR_ZERO( o && i );

				if( is_vec3( L, si+1 ) )
				{
					Vec3* v = get_vec3( L, si+1 );
					SU_REQUIRE_OR_ZERO( v );
					MatrixPreTranslate( o, i, v );
				}
				else if( is_matrix( L, si+1 ) )
				{
					Matrix* mt = get_matrix( L, si+1 );
					SU_REQUIRE_OR_ZERO( mt );
					MatrixPreTranslate( o, i, mt );
				}
				else
				{
					float x = luaT_checknumber( L, si+1 );
					float y = luaT_checknumber( L, si+2 );
					float z = luaT_checknumber( L, si+3 );
					MatrixPreTranslate( o, i, x, y, z );
				}
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_rotate ( lua_State* L )
		{
			if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  rotate( out, inM, inQuat )			 : out = rotate( inM, inQuat )
			// (outGiven) ?  rotate( out, inM, inVec3, inAngle ) : out = rotate( inM, inVec3, inAngle )
			// (outGiven) ?  rotate( out, inM, inMatR )			 : out = rotate( inM, inMatR )

				int argc = lua_gettop( L );
				bool outGiven = is_matrix( L, argc ) ? ( argc == 3 ) : ( is_matrix( L, 1 ) && is_matrix( L, 2 ) );
				int si = (outGiven) ? 2:1;

				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = get_matrix( L, si+0 );
				SU_REQUIRE_OR_ZERO( o && i );

				if( is_quat( L, si+1 ) )
				{
					Quat* q = get_quat( L, si+1 );
					SU_REQUIRE_OR_ZERO( q );
					MatrixRotate( o, i, q );
				}
				else if( is_vec3( L, si+1 ) )
				{
					Vec3* v = get_vec3( L, si+1 );
					SU_REQUIRE_OR_ZERO( v );
					float f = luaT_checknumber( L, si+2 );
					MatrixRotate( o, i, v, f );
				}
				else if( is_matrix( L, si+1 ) )
				{
					Matrix* mr = get_matrix( L, si+1 );
					SU_REQUIRE_OR_ZERO( mr );
					MatrixRotate( o, i, mr );
				}
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_preRotate ( lua_State* L )
		{
			if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  rotate( out, inM, inQuat )			 : out = rotate( inM, inQuat )
			// (outGiven) ?  rotate( out, inM, inVec3, inAngle ) : out = rotate( inM, inVec3, inAngle )
			// (outGiven) ?  rotate( out, inM, inMatR )			 : out = rotate( inM, inMatR )
				
				int argc = lua_gettop( L );
				bool outGiven = is_matrix( L, argc ) ? ( argc == 3 ) : ( is_matrix( L, 1 ) && is_matrix( L, 2 ) );
				int si = (outGiven) ? 2:1;

				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = get_matrix( L, si+0 );
				SU_REQUIRE_OR_ZERO( o && i );

				if( is_quat( L, si+1 ) )
				{
					Quat* q = get_quat( L, si+1 );
					SU_REQUIRE_OR_ZERO( q );
					MatrixPreRotate( o, i, q );
				}
				else if( is_vec3( L, si+1 ) )
				{
					Vec3* v = get_vec3( L, si+1 );
					SU_REQUIRE_OR_ZERO( v );
					float f = luaT_checknumber( L, si+2 );
					MatrixPreRotate( o, i, v, f );
				}
				else if( is_matrix( L, si+1 ) )
				{
					Matrix* mr = get_matrix( L, si+1 );
					SU_REQUIRE_OR_ZERO( mr );
					MatrixPreRotate( o, i, mr );
				}
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}

		int Lua_rotateX ( lua_State* L )
		{
			if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  rotateX( out, in, angle ) : out = rotateX( in, angle )
				bool outGiven = ( lua_gettop( L ) >= 3 );
				int si	  = (outGiven) ? 2 : 1;
				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = get_matrix( L, si+0 );
				SU_REQUIRE_OR_ZERO( o && i );
				float a	  = luaT_checknumber( L, si+1 );
				MatrixRotateX( o, i, a );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_rotateY ( lua_State* L )
		{
			if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  rotateY( out, in, angle ) : out = rotateY( in, angle )
				bool outGiven = ( lua_gettop( L ) >= 3 );
				int si	  = (outGiven) ? 2 : 1;
				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = get_matrix( L, si+0 );
				SU_REQUIRE_OR_ZERO( o && i );
				float a	  = luaT_checknumber( L, si+1 );
				MatrixRotateY( o, i, a );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_rotateZ ( lua_State* L )
		{
			if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  rotateZ( out, in, angle ) : out = rotateZ( in, angle )
				bool outGiven = ( lua_gettop( L ) >= 3 );
				int si	  = (outGiven) ? 2 : 1;

				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = get_matrix( L, si+0 );
				float a	  = luaT_checknumber( L, si+1 );

				SU_ASSERT( o );
				SU_ASSERT( i );

				MatrixRotateZ( o, i, a );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_preRotateX ( lua_State* L )
		{
			if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  rotateZ( out, in, angle ) : out = rotateZ( in, angle )
				bool outGiven = ( lua_gettop( L ) >= 3 );
				int si	  = (outGiven) ? 2 : 1;

				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = get_matrix( L, si+0 );
				float a	  = luaT_checknumber( L, si+1 );

				SU_ASSERT( o );
				SU_ASSERT( i );

				MatrixPreRotateX( o, i, a );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_preRotateY ( lua_State* L )
		{
			if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  rotateZ( out, in, angle ) : out = rotateZ( in, angle )
				bool outGiven = ( lua_gettop( L ) >= 3 );
				int si	  = (outGiven) ? 2 : 1;

				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = get_matrix( L, si+0 );
				float a	  = luaT_checknumber( L, si+1 );

				SU_ASSERT( o );
				SU_ASSERT( i );

				MatrixPreRotateY( o, i, a );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_preRotateZ ( lua_State* L )
		{
			if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  rotateZ( out, in, angle ) : out = rotateZ( in, angle )
				bool outGiven = ( lua_gettop( L ) >= 3 );
				int si	  = (outGiven) ? 2 : 1;
				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				Matrix* i = get_matrix( L, si+0 );
				float a	  = luaT_checknumber( L, si+1 );

				SU_ASSERT( o );
				SU_ASSERT( i );

				MatrixPreRotateZ( o, i, a );
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_preScale ( lua_State* L )
		{
			if( is_matrix( L, 1 ) )
			{
			// (outGiven) ?  preScale( out, inM, x, y, z ) : out = preScale( inM, x, y, z )
			// (outGiven) ?  preScale( out, inM, inVec3 ) : out = preScale( inM, inVec3 )
			// (outGiven) ?  preScale( out, inM, inMatT ) : out = preScale( inM, inMatT )
				
				int argc = lua_gettop( L );
				bool outGiven = is_matrix( L, argc ) ? ( argc == 3 ) : ( is_matrix( L, 1 ) && is_matrix( L, 2 ) );
				int si = (outGiven) ? 2:1;

				Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
				SU_ASSERT( o );

				Matrix* i = get_matrix( L, si+0 );
				SU_ASSERT( i );

				if( is_vec3( L, si+1 ) )
				{
					Vec3* v = get_vec3( L, si+1 );
					MatrixPreScale( o, i, v );
				}
				else if( is_matrix( L, si+1 ) )
				{
					Matrix* ms = get_matrix( L, si+1 );
					MatrixPreScale( o, i, ms );
				}
				else
				{
					float x = luaT_checknumber( L, si+1 );
					float y = luaT_checknumber( L, si+2 );
					float z = luaT_checknumber( L, si+3 );
					MatrixPreScale( o, i, x, y, z );
				}
				return (outGiven) ? 0 : 1;
			}
			return 0;
		}
		int Lua_translation ( lua_State* L )
		{
		// (outGiven) ?  translation( out, inVec3 ) : out = translation( inVec3 )
		// (outGiven) ?  translation( out, inMatT ) : out = translation( inMatT )
		// (outGiven) ?  translation( out, x, y, z ) : out = translation( x, y, z )

			int argc = lua_gettop( L );
			bool outGiven = is_matrix( L, argc ) ? ( argc == 2 ) : is_matrix( L, 1 );
			int si = (outGiven) ? 2:1;
			
			Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
			SU_ASSERT( o );

			if( is_vec3( L, si+0 ) )
			{
				Vec3* v = get_vec3( L, si+0 );
				MatrixTranslation( o, v );
			}
			else if( is_matrix( L, si+0 ) )
			{
				Matrix* mt = get_matrix( L, si+0 );
				MatrixTranslation( o, mt );
			}
			else
			{
				float x = luaT_checknumber( L, si+0 );
				float y = luaT_checknumber( L, si+1 );
				float z = luaT_checknumber( L, si+2 );
				MatrixTranslation( o, x, y, z );
			}
			return (outGiven) ? 0 : 1;
		}

		int Lua_rotationX ( lua_State* L )
		{
		// (outGiven) ?  rotationX( out, x ) : out = rotationX( x )
			bool outGiven = is_matrix( L, 1 );
			int si = (outGiven) ? 2:1;

			Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
			SU_ASSERT( o );
			float a = luaT_checknumber( L, si+0 );
			MatrixRotationX( o, a );

			return (outGiven) ? 0 : 1;
		}
		int Lua_rotationY ( lua_State* L )
		{
		// (outGiven) ?  rotationX( out, x ) : out = rotationX( x )
			bool outGiven = is_matrix( L, 1 );
			int si = (outGiven) ? 2:1;

			Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
			SU_ASSERT( o );
			float a = luaT_checknumber( L, si+0 );
			MatrixRotationY( o, a );

			return (outGiven) ? 0 : 1;
		}
		int Lua_rotationZ ( lua_State* L )
		{
		// (outGiven) ?  rotationX( out, x ) : out = rotationX( x )
			bool outGiven = is_matrix( L, 1 );
			int si = (outGiven) ? 2:1;
			
			Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
			SU_ASSERT( o );
			float a = luaT_checknumber( L, si+0 );
			MatrixRotationZ( o, a );

			return (outGiven) ? 0 : 1;
		}

		
		int Lua_rotation ( lua_State* L )
		{
		// (outGiven) ?  rotation( out, inVec3, inAngle ) : out = rotation( inVec3, inAngle )
		// (outGiven) ?  rotation( out, inMatR ) : out = rotation( inMatR )
		// (outGiven) ?  rotation( out, inQuat ) : out = rotation( inQuat )

			int argc = lua_gettop( L );
			bool outGiven = is_matrix( L, argc ) ? ( argc == 2 ) : is_matrix( L, 1 );
			int si = (outGiven) ? 2:1;

			Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
			SU_ASSERT( o );

			if( is_vec3( L, si+0 ) )
			{
				Vec3* v = get_vec3( L, si+0 );
				float a = luaT_checknumber( L, si+1 );
				MatrixRotation( o, v, a );
			}
			else if( is_matrix( L, si+0 ) )
			{
				Matrix* r = get_matrix( L, si+0 );
				MatrixRotation( o, r );
			}
			else if( is_quat( L, si+0 ) )
			{
				Quat* q = get_quat( L, si+0 );
				MatrixRotation( o, q );
			}
			return (outGiven) ? 0 : 1;
		}

		int Lua_scaling ( lua_State* L )
		{
		// (outGiven) ?  scaling( out, inVec3 ) : out = scaling( inVec3 )
		// (outGiven) ?  scaling( out, inMatS ) : out = scaling( inMatS )
		// (outGiven) ?  scaling( out, x, y, z ) : out = scaling( x, y, z )

			int argc = lua_gettop( L );
			bool outGiven = is_matrix( L, argc ) ? ( argc == 2 ) : is_matrix( L, 1 );
			int si = (outGiven) ? 2:1;

			Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
			SU_ASSERT( o );
			if( is_vec3( L, si+0 ) )
			{
				Vec3* v = get_vec3( L, si+0 );
				MatrixScaling( o, v );
			}
			else if( is_matrix( L, si+0 ) )
			{
				Matrix* ms = get_matrix( L, si+0 );
				MatrixScaling( o, ms );
			}
			else
			{
				float x = luaT_checknumber( L, si+0 );
				float y = luaT_checknumber( L, si+1 );
				float z = luaT_checknumber( L, si+2 );
				MatrixScaling( o, x, y, z );
			}
			return (outGiven) ? 0 : 1;
		}

		int Lua_buildTR ( lua_State* L )		
		{
		// (outGiven) ?  buildTR( out, inT, inR, inS ) : out = buildTR( inT, inR, inS )		// inT, inR, inS are optionals
			bool outGiven = is_matrix( L, 1 );
			int si = (outGiven) ? 2:1;

			Matrix* o = (outGiven) ? get_matrix( L, 1 ) : push_new_matrix( L );
			SU_ASSERT( o );
			
			Vec3* t = get_vec3( L, si+0 );
			Quat* r = get_quat( L, si+1 );
			Vec3* s = get_vec3( L, si+2 );

			MatrixBuildTR( o, t, r, s );
			return (outGiven) ? 0 : 1;
		}

	};


}


