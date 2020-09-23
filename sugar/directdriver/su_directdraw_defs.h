/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2008 AtOnce Technologies
**
** This file is part of the Sugar Framework.
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




#ifndef _su_ddraw_defs_h_
#define _su_ddraw_defs_h_



namespace su { namespace dd {

	struct CmdDesc
	{
		byte						code;
		su::IDirectShader::Action	act;
	};

} }



#define SU_DD_CMD_DEFINE(_code) \
			((_code)<<24)



#define SU_DD_UI(_v,_p,_l) \
			((uint(_v)>>(_p))&((1<<(_l))-1))

#define SU_DD_BOOL(_v,_p) \
			(SU_DD_UI(_v,_p,1)?true:false)

#define SU_DD_ADDR24(_lo,_hi) \
			( (uint(_lo)&0xfffffful) | (uint(_hi)&0xff000000ul) )



#define SU_DD_ACT_NAME( _name ) \
			dd_cmd_##_name

#define SU_DD_ACT_DECL( _name ) \
			uint* SU_DD_ACT_NAME(_name)( uint* ip, uint opcode, void* udata, void* ctxt )

#define SU_DD_ACT_DEF( _name, _implrev, _checkrev ) \
			SU_COMPILE_TIME_ASSERT_DECL( dd_rev_##_name, _implrev==_checkrev ) \
			SU_DD_ACT_DECL( _name )

#define SU_DD_CMD_DESC \
			su::dd::CmdDesc



#define SU_DD_RS_ZTEST( _ena, _cmp )				( (1<<24) | (_cmp<<1) | (_ena&0x1) )
#define SU_DD_RS_ALPHATEST( _ena, _cmp, _val )		( (2<<24) | ((_val&0xff)<<16) | (_cmp<<1) | (_ena&0x1) )
#define SU_DD_RS_ZWRITE( _ena )						( (3<<24) | (_ena&0x1) )
#define SU_DD_RS_CULLFACE( _cullmode )				( (4<<24) | (_cullmode&0x3) )
#define SU_DD_RS_BLEND( _blendmode )				( (5<<24) | (_blendmode&0xff) )

#define SU_DD_RS_ZTEST_STATE( _ena )				SU_DD_RS_ZTEST( _ena, dd::CF_UNDEF )
#define SU_DD_RS_ALPHATEST_STATE( _ena )			SU_DD_RS_ALPHATEST( _ena, dd::CF_UNDEF, 0 )



#endif // _su_ddraw_defs_h_


