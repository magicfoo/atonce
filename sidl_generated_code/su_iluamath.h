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


#ifndef _su_iluamath_h_
#define _su_iluamath_h_




struct lua_State;


namespace su
{


	struct ILuaMath : public IInterface
	{
		SU_DECL_INTERFACE( IInterface, 0x577dc7fau )


		virtual int    Lua_rand              ( lua_State* L ) = 0;

		virtual int    Lua_srand             ( lua_State* L ) = 0;

		virtual int    Lua_cos               ( lua_State* L ) = 0;

		virtual int    Lua_sin               ( lua_State* L ) = 0;

		virtual int    Lua_tan               ( lua_State* L ) = 0;

		virtual int    Lua_sqrt              ( lua_State* L ) = 0;

		virtual int    Lua_newVec2           ( lua_State* L ) = 0;

		virtual int    LuaVec2_x             ( lua_State* L ) = 0;

		virtual int    LuaVec2_y             ( lua_State* L ) = 0;

		virtual int    LuaVec2_yx            ( lua_State* L ) = 0;

		virtual int    LuaVec2_asTable       ( lua_State* L ) = 0;

		virtual int    Lua_newVec3           ( lua_State* L ) = 0;

		virtual int    LuaVec3_x             ( lua_State* L ) = 0;

		virtual int    LuaVec3_y             ( lua_State* L ) = 0;

		virtual int    LuaVec3_z             ( lua_State* L ) = 0;

		virtual int    LuaVec3_xzy           ( lua_State* L ) = 0;

		virtual int    LuaVec3_yxz           ( lua_State* L ) = 0;

		virtual int    LuaVec3_yzx           ( lua_State* L ) = 0;

		virtual int    LuaVec3_zxy           ( lua_State* L ) = 0;

		virtual int    LuaVec3_zyx           ( lua_State* L ) = 0;

		virtual int    LuaVec3_asTable       ( lua_State* L ) = 0;

		virtual int    Lua_newVec4           ( lua_State* L ) = 0;

		virtual int    LuaVec4_x             ( lua_State* L ) = 0;

		virtual int    LuaVec4_y             ( lua_State* L ) = 0;

		virtual int    LuaVec4_z             ( lua_State* L ) = 0;

		virtual int    LuaVec4_w             ( lua_State* L ) = 0;

		virtual int    LuaVec4_asTable       ( lua_State* L ) = 0;

		virtual int    Lua_newQuat           ( lua_State* L ) = 0;

		virtual int    LuaQuat_x             ( lua_State* L ) = 0;

		virtual int    LuaQuat_y             ( lua_State* L ) = 0;

		virtual int    LuaQuat_z             ( lua_State* L ) = 0;

		virtual int    LuaQuat_w             ( lua_State* L ) = 0;

		virtual int    LuaQuat_xzyw          ( lua_State* L ) = 0;

		virtual int    LuaQuat_asTable       ( lua_State* L ) = 0;

		virtual int    Lua_newMatrix         ( lua_State* L ) = 0;

		virtual int    LuaMat_get            ( lua_State* L ) = 0;

		virtual int    LuaMat_set            ( lua_State* L ) = 0;

		virtual int    LuaMat_asTable        ( lua_State* L ) = 0;

		virtual int    Lua_copy              ( lua_State* L ) = 0;

		virtual int    Lua_zero              ( lua_State* L ) = 0;

		virtual int    Lua_identity          ( lua_State* L ) = 0;

		virtual int    Lua_isIdentity        ( lua_State* L ) = 0;

		virtual int    Lua_norm              ( lua_State* L ) = 0;

		virtual int    Lua_norm2             ( lua_State* L ) = 0;

		virtual int    Lua_normalize         ( lua_State* L ) = 0;

		virtual int    Lua_orthoNormalize    ( lua_State* L ) = 0;

		virtual int    Lua_cross             ( lua_State* L ) = 0;

		virtual int    Lua_dot               ( lua_State* L ) = 0;

		virtual int    Lua_near              ( lua_State* L ) = 0;

		virtual int    Lua_add               ( lua_State* L ) = 0;

		virtual int    Lua_sub               ( lua_State* L ) = 0;

		virtual int    Lua_mul               ( lua_State* L ) = 0;

		virtual int    Lua_min               ( lua_State* L ) = 0;

		virtual int    Lua_max               ( lua_State* L ) = 0;

		virtual int    Lua_scale             ( lua_State* L ) = 0;

		virtual int    Lua_apply             ( lua_State* L ) = 0;

		virtual int    Lua_applyVector       ( lua_State* L ) = 0;

		virtual int    Lua_lerp              ( lua_State* L ) = 0;

		virtual int    Lua_slerp             ( lua_State* L ) = 0;

		virtual int    Lua_inverse           ( lua_State* L ) = 0;

		virtual int    Lua_fastInverse       ( lua_State* L ) = 0;

		virtual int    Lua_translate         ( lua_State* L ) = 0;

		virtual int    Lua_preTranslate      ( lua_State* L ) = 0;

		virtual int    Lua_rotate            ( lua_State* L ) = 0;

		virtual int    Lua_preRotate         ( lua_State* L ) = 0;

		virtual int    Lua_rotateX           ( lua_State* L ) = 0;

		virtual int    Lua_rotateY           ( lua_State* L ) = 0;

		virtual int    Lua_rotateZ           ( lua_State* L ) = 0;

		virtual int    Lua_preRotateX        ( lua_State* L ) = 0;

		virtual int    Lua_preRotateY        ( lua_State* L ) = 0;

		virtual int    Lua_preRotateZ        ( lua_State* L ) = 0;

		virtual int    Lua_preScale          ( lua_State* L ) = 0;

		virtual int    Lua_translation       ( lua_State* L ) = 0;

		virtual int    Lua_rotationX         ( lua_State* L ) = 0;

		virtual int    Lua_rotationY         ( lua_State* L ) = 0;

		virtual int    Lua_rotationZ         ( lua_State* L ) = 0;

		virtual int    Lua_rotation          ( lua_State* L ) = 0;

		virtual int    Lua_scaling           ( lua_State* L ) = 0;

		virtual int    Lua_buildTR           ( lua_State* L ) = 0;
	};


}




#endif // _su_iluamath_h_


// !! This file has been automatically generated by the sugar sidl tool.
// !! Do not edit this file manually under risk of losing your changes.

