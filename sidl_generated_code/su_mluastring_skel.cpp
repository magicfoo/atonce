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


//==== includes ====================================================================================

#include <sufw/precompiled/supch_core.h>
#include "..\..\core\base\su_iinterface.h"
#include "su_iluastring.h"
#include "..\..\core\base\su_iswitch.h"
#include <stdarg.h>




//==== defines =====================================================================================

#ifndef SIDL_IMPL_CLASSNAME
#define SIDL_IMPL_CLASSNAME MLuaString
#endif

#ifndef SIDL_IMPL_MUID
#define SIDL_IMPL_MUID 0x90564396u
#endif

#ifndef SIDL_IMPL_QUALIFIER
#define SIDL_IMPL_QUALIFIER static
#endif

#ifndef SIDL_IMPL_IS_STATIC
#define SIDL_IMPL_IS_STATIC 
#endif

#ifndef SIDL_IMPL_SWITCHED
#define SIDL_IMPL_SWITCHED 
#endif

#ifndef SIDL_IMPL_REVISION
#define SIDL_IMPL_REVISION 0
#endif

#ifndef SIDL_IMPL_PACKAGE
#define SIDL_IMPL_PACKAGE "su.fw.lang"
#endif

#ifndef SIDL_IMPL_ATTR_V
#define SIDL_IMPL_ATTR_V NULL
#endif

#ifndef SIDL_IMPL_ATTR_C
#define SIDL_IMPL_ATTR_C 0
#endif

#ifndef SIDL_STUB_CLASSNAME
#define SIDL_STUB_CLASSNAME MLuaString_Stub
#endif

#ifndef SIDL_IMPL_INTERFACE
#define SIDL_IMPL_INTERFACE(I) ((I*)MLuaString_get_interface(MLuaString_stub_to_base(this),I::ClassId))
#endif


//==== declarations ================================================================================

namespace {

su::IInterface* MLuaString_create_object( su::Pool*, su::Pool*, su::AnyList* );
void* MLuaString_base_to_stub ( void* );
void* MLuaString_stub_to_base ( void* );
su::addin::Builder* MLuaString_builder ( );
uint MLuaString_count_interface ( );
bool MLuaString_enum_interface ( uint, su::classid*, pcstr* );
su::IInterface* MLuaString_first_interface ( void* );
su::IInterface* MLuaString_get_interface ( void*, su::classid );
uint MLuaString_count_switch ( );
bool MLuaString_enum_switch ( uint, su::ISwitch::Desc& );
int MLuaString_find_switch ( pcstr );
bool MLuaString_call_switch_stack ( void*, uint, void*, va_list );
bool MLuaString_call_switch_mem ( void*, uint, su::Any*, int, su::Any* );

}


//==== stub area ===================================================================================

#undef  SIDL_PROPER
#undef  SIDL_STUB
#undef  SIDL_BUILDER
#undef  SIDL_CODE
#define SIDL_STUB
#include "su_mluastring_stub.h"




//==== skeleton ====================================================================================

namespace {


struct MLuaString
{

//---- bridges -------------------------------------------------------------------------------------

struct ILuaString_t : public su::ILuaString {
	// from IInterface
	::su::classbase         GetBase           (  ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return &(*m); }
	::su::addin::Builder *  GetBuilder        (  ) { return MLuaString_builder(); }
	void                    Retain            (  ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); (*m).Retain(); }
	::uint                  CountRef          (  ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (*m).CountRef(); }
	void                    Release           (  ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); (*m).Release(); }
	::uint                  CountInterface    (  ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (*m).CountInterface(); }
	bool                    EnumInterface     ( ::uint p0,::su::classid * p1,::pcstr * p2 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (*m).EnumInterface(p0,p1,p2); }
	::su::IInterface *      GetInterface      ( ::su::classid p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (*m).GetInterface(p0); }
	// from ILuaString
	int                     Lua_len           ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_len(p0); }
	int                     Lua_cvt           ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_cvt(p0); }
	int                     Lua_format        ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_format(p0); }
	int                     Lua_match         ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_match(p0); }
	int                     Lua_decodeHex     ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_decodeHex(p0); }
	int                     Lua_decodeFloat   ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_decodeFloat(p0); }
	int                     Lua_isEmpty       ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_isEmpty(p0); }
	int                     Lua_isSpace       ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_isSpace(p0); }
	int                     Lua_isAlpha       ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_isAlpha(p0); }
	int                     Lua_isDigit2      ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_isDigit2(p0); }
	int                     Lua_isDigit10     ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_isDigit10(p0); }
	int                     Lua_isDigit16     ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_isDigit16(p0); }
	int                     Lua_isAlnum       ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_isAlnum(p0); }
	int                     Lua_isUpper       ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_isUpper(p0); }
	int                     Lua_isLower       ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_isLower(p0); }
	int                     Lua_isIdent       ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_isIdent(p0); }
	int                     Lua_compare       ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_compare(p0); }
	int                     Lua_equals        ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_equals(p0); }
	int                     Lua_find          ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_find(p0); }
	int                     Lua_findAny       ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_findAny(p0); }
	int                     Lua_startsWith    ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_startsWith(p0); }
	int                     Lua_startsWithAny ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_startsWithAny(p0); }
	int                     Lua_endsWith      ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_endsWith(p0); }
	int                     Lua_endsWithAny   ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_endsWithAny(p0); }
	int                     Lua_copy          ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_copy(p0); }
	int                     Lua_indexOf       ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_indexOf(p0); }
	int                     Lua_indexOfAny    ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_indexOfAny(p0); }
	int                     Lua_upper         ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_upper(p0); }
	int                     Lua_lower         ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_lower(p0); }
	int                     Lua_strip         ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_strip(p0); }
	int                     Lua_split         ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_split(p0); }
	int                     Lua_splitlines    ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_splitlines(p0); }
	int                     Lua_splitargs     ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_splitargs(p0); }
	int                     Lua_join          ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_join(p0); }
	int                     Lua_mid           ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_mid(p0); }
	int                     Lua_xmid          ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_xmid(p0); }
	int                     Lua_left          ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_left(p0); }
	int                     Lua_right         ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_right(p0); }
	int                     Lua_front         ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_front(p0); }
	int                     Lua_back          ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_back(p0); }
	int                     Lua_replace       ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_replace(p0); }
	int                     Lua_reverse       ( lua_State* p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ILuaString_i,this); return (m->stub).Lua_reverse(p0); }
} ILuaString_i;


struct ISwitch_t : public su::ISwitch {
	// from IInterface
	::su::classbase         GetBase        (  ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ISwitch_i,this); return &(*m); }
	::su::addin::Builder *  GetBuilder     (  ) { return MLuaString_builder(); }
	void                    Retain         (  ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ISwitch_i,this); (*m).Retain(); }
	::uint                  CountRef       (  ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ISwitch_i,this); return (*m).CountRef(); }
	void                    Release        (  ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ISwitch_i,this); (*m).Release(); }
	::uint                  CountInterface (  ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ISwitch_i,this); return (*m).CountInterface(); }
	bool                    EnumInterface  ( ::uint p0,::su::classid * p1,::pcstr * p2 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ISwitch_i,this); return (*m).EnumInterface(p0,p1,p2); }
	::su::IInterface *      GetInterface   ( ::su::classid p0 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ISwitch_i,this); return (*m).GetInterface(p0); }
	// from ISwitch
	::uint                  CountSwitch    (  ) { return MLuaString_count_switch(); }
	bool                    EnumSwitch     ( ::uint p0,::su::ISwitch::Desc & p1 ) { return MLuaString_enum_switch(p0,p1); }
	int                     FindSwitch     ( ::pcstr p0 ) { return MLuaString_find_switch(p0); }
	bool                    CallSwitch     ( ::uint p0,void * p1,... ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ISwitch_i,this); va_list vl; va_start(vl,p1); return MLuaString_call_switch_stack(m,p0,p1,vl); va_end(vl); }
	bool                    CallSwitchMem  ( ::uint p0,::su::Any * p1,int p2,::su::Any * p3 ) { MLuaString* m = SU_DELEGATE_BASEPTR(MLuaString,ISwitch_i,this); return MLuaString_call_switch_mem(m,p0,p1,p2,p3); }
} ISwitch_i;


//---- stub nested object --------------------------------------------------------------------------

MLuaString_Stub stub;


//---- skeleton management code area ---------------------------------------------------------------

int refcount;


void Retain ( )
{
	refcount++;
}


uint CountRef ( )
{
	return refcount;
}


void Release ( )
{
	if( refcount==1 )
		Destroy( this );
	else
		refcount--;
}


uint CountInterface ( )
{
	return MLuaString_count_interface();
}


bool EnumInterface ( uint i, su::classid* cid, pcstr* cname )
{
	return MLuaString_enum_interface(i,cid,cname);
}


su::IInterface* GetInterface ( classid cid )
{
	SU_IMPLEMENTS_MEMBER( ILuaString_i, cid );
	SU_IMPLEMENTS_MEMBER( ISwitch_i, cid );
	return NULL;
}


static MLuaString* Create( Pool* np, Pool* dp, AnyList* o )
{
	static MLuaString m;
	static bool yet = false;
	if( !yet )
	{
		m.refcount = 0;
		yet = true;
	}
	if( m.refcount==0 )
	{
		if( !m.stub.Init(dp,o) )
			return NULL;
	}
	m.refcount += 1;
	return &m;
}


static void Destroy( MLuaString* m )
{
	m->stub.Shut();
	m->refcount=0;
}

};

}




//==== builder =====================================================================================


SU_BEGIN_BUILDER( MLuaString )

su::classid GetMUID ( ) { return SIDL_IMPL_MUID; }
pcstr GetMClassname ( ) { return "MLuaString"; }
uint32 GetRevision ( ) { return SIDL_IMPL_REVISION; }
pcstr GetPackage ( ) { return SIDL_IMPL_PACKAGE; }
uint CountInterface ( ) { return MLuaString_count_interface(); }
bool EnumInterface ( uint i, su::classid* cid, pcstr* cname ) { return MLuaString_enum_interface(i,cid,cname); }
su::IInterface* FirstInterface ( void* base ) { return MLuaString_first_interface(base); }
su::IInterface* GetInterface ( void* base, su::classid cid ) { return MLuaString_get_interface(base,cid); }
su::IInterface* CreateObject( su::Pool* nestp, su::Pool* datap, su::AnyList* opts ) { return MLuaString_create_object(nestp,datap,opts); }
void* BaseToStub ( void* base ) { return MLuaString_base_to_stub(base); }
void* StubToBase ( void* base ) { return MLuaString_stub_to_base(base); }
uint CountSwitch ( ) { return MLuaString_count_switch(); }
bool EnumSwitch ( uint i, su::ISwitch::Desc& desc ) { return MLuaString_enum_switch(i,desc); }

SU_END_BUILDER( MLuaString )




//==== definitions =================================================================================


namespace {


su::IInterface* MLuaString_create_object( su::Pool* nestp, su::Pool* datap, su::AnyList* opts )
{
	MLuaString* m = MLuaString::Create( nestp, datap, opts );
	SU_REQUIRE_OR_NULL( m );
	return &m->ILuaString_i;
}


void* MLuaString_base_to_stub ( void* base )
{
	SU_REQUIRE_OR_NULL( base );
	return &(((MLuaString*)base)->stub);
}


void* MLuaString_stub_to_base ( void* stubp )
{
	SU_REQUIRE_OR_NULL( stubp );
	return SU_DELEGATE_BASEPTR(MLuaString,stub,stubp);
}


su::addin::Builder* MLuaString_builder ( )
{
	return &_builder_MLuaString;
}


uint MLuaString_count_interface ( )
{
	return 2;
}


bool MLuaString_enum_interface ( uint i, su::classid* cid, pcstr* cname )
{
	static const su::classid cids[] = { su::ILuaString::ClassId, su::ISwitch::ClassId };
	static const pcstr cnames[] = { "ILuaString", "ISwitch" };
	SU_REQUIRE_OR_FALSE( i<2 );
	if( cid )   *cid   = cids[i];
	if( cname ) *cname = cnames[i];
	return true;
}


su::IInterface* MLuaString_first_interface ( void* base )
{
	SU_REQUIRE_OR_NULL( base );
	return &(((MLuaString*)base)->ILuaString_i);
}


su::IInterface* MLuaString_get_interface ( void* base, su::classid cid )
{
	SU_REQUIRE_OR_NULL( base );
	return ((MLuaString*)base)->GetInterface(cid);
}


uint MLuaString_count_switch ( )
{
	return 42;
}


bool MLuaString_enum_switch ( uint i, su::ISwitch::Desc& desc )
{
	static su::ISwitch::Desc swdesc[] = {
		{ "len", "Lua_len", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "cvt", "Lua_cvt", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "format", "Lua_format", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "match", "Lua_match", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "decodeHex", "Lua_decodeHex", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "decodeFloat", "Lua_decodeFloat", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "isEmpty", "Lua_isEmpty", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "isSpace", "Lua_isSpace", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "isAlpha", "Lua_isAlpha", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "isDigit2", "Lua_isDigit2", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "isDigit10", "Lua_isDigit10", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "isDigit16", "Lua_isDigit16", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "isAlnum", "Lua_isAlnum", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "isUpper", "Lua_isUpper", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "isLower", "Lua_isLower", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "isIdent", "Lua_isIdent", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "compare", "Lua_compare", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "equals", "Lua_equals", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "find", "Lua_find", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "findAny", "Lua_findAny", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "startsWith", "Lua_startsWith", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "startsWithAny", "Lua_startsWithAny", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "endsWith", "Lua_endsWith", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "endsWithAny", "Lua_endsWithAny", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "copy", "Lua_copy", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "indexOf", "Lua_indexOf", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "indexOfAny", "Lua_indexOfAny", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "upper", "Lua_upper", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "lower", "Lua_lower", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "strip", "Lua_strip", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "split", "Lua_split", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "splitlines", "Lua_splitlines", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "splitargs", "Lua_splitargs", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "join", "Lua_join", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "mid", "Lua_mid", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "xmid", "Lua_xmid", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "left", "Lua_left", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "right", "Lua_right", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "front", "Lua_front", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "back", "Lua_back", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "replace", "Lua_replace", "int,lua_State*", "wp", "ILuaString", NULL },
		{ "reverse", "Lua_reverse", "int,lua_State*", "wp", "ILuaString", NULL },
		};
	SU_REQUIRE_OR_FALSE( i<42 );
	desc = swdesc[i];
	return true;
}


int MLuaString_find_switch ( pcstr inname=NULL )
{
	SU_REQUIRE_OR_NEG( inname && inname[0] );
	for( uint i=0 ; i<42 ; i++ ) {
		su::ISwitch::Desc d; MLuaString_enum_switch(i,d);
		if( util::Strequals(inname,d.name,false) )
			return i;
		}
	return -1;
}


bool MLuaString_call_switch_stack ( void* base, uint i, void* r, va_list args )
{
	SU_REQUIRE_OR_FALSE( base );
	SU_REQUIRE_OR_FALSE( i<42 );
	if( i==0 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_len(p0);
		return true;
	}
	if( i==1 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_cvt(p0);
		return true;
	}
	if( i==2 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_format(p0);
		return true;
	}
	if( i==3 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_match(p0);
		return true;
	}
	if( i==4 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_decodeHex(p0);
		return true;
	}
	if( i==5 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_decodeFloat(p0);
		return true;
	}
	if( i==6 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_isEmpty(p0);
		return true;
	}
	if( i==7 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_isSpace(p0);
		return true;
	}
	if( i==8 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_isAlpha(p0);
		return true;
	}
	if( i==9 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_isDigit2(p0);
		return true;
	}
	if( i==10 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_isDigit10(p0);
		return true;
	}
	if( i==11 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_isDigit16(p0);
		return true;
	}
	if( i==12 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_isAlnum(p0);
		return true;
	}
	if( i==13 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_isUpper(p0);
		return true;
	}
	if( i==14 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_isLower(p0);
		return true;
	}
	if( i==15 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_isIdent(p0);
		return true;
	}
	if( i==16 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_compare(p0);
		return true;
	}
	if( i==17 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_equals(p0);
		return true;
	}
	if( i==18 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_find(p0);
		return true;
	}
	if( i==19 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_findAny(p0);
		return true;
	}
	if( i==20 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_startsWith(p0);
		return true;
	}
	if( i==21 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_startsWithAny(p0);
		return true;
	}
	if( i==22 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_endsWith(p0);
		return true;
	}
	if( i==23 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_endsWithAny(p0);
		return true;
	}
	if( i==24 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_copy(p0);
		return true;
	}
	if( i==25 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_indexOf(p0);
		return true;
	}
	if( i==26 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_indexOfAny(p0);
		return true;
	}
	if( i==27 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_upper(p0);
		return true;
	}
	if( i==28 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_lower(p0);
		return true;
	}
	if( i==29 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_strip(p0);
		return true;
	}
	if( i==30 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_split(p0);
		return true;
	}
	if( i==31 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_splitlines(p0);
		return true;
	}
	if( i==32 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_splitargs(p0);
		return true;
	}
	if( i==33 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_join(p0);
		return true;
	}
	if( i==34 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_mid(p0);
		return true;
	}
	if( i==35 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_xmid(p0);
		return true;
	}
	if( i==36 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_left(p0);
		return true;
	}
	if( i==37 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_right(p0);
		return true;
	}
	if( i==38 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_front(p0);
		return true;
	}
	if( i==39 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_back(p0);
		return true;
	}
	if( i==40 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_replace(p0);
		return true;
	}
	if( i==41 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaString*)base)->ILuaString_i.Lua_reverse(p0);
		return true;
	}
	return false;
}


bool MLuaString_call_switch_mem ( void* base, uint i, Any* r, int argc, Any* argv )
{
	SU_REQUIRE_OR_FALSE( base );
	SU_REQUIRE_OR_FALSE( i<42 );
	if( i==0 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_len(p0) );
		return true;
	}
	if( i==1 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_cvt(p0) );
		return true;
	}
	if( i==2 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_format(p0) );
		return true;
	}
	if( i==3 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_match(p0) );
		return true;
	}
	if( i==4 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_decodeHex(p0) );
		return true;
	}
	if( i==5 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_decodeFloat(p0) );
		return true;
	}
	if( i==6 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_isEmpty(p0) );
		return true;
	}
	if( i==7 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_isSpace(p0) );
		return true;
	}
	if( i==8 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_isAlpha(p0) );
		return true;
	}
	if( i==9 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_isDigit2(p0) );
		return true;
	}
	if( i==10 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_isDigit10(p0) );
		return true;
	}
	if( i==11 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_isDigit16(p0) );
		return true;
	}
	if( i==12 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_isAlnum(p0) );
		return true;
	}
	if( i==13 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_isUpper(p0) );
		return true;
	}
	if( i==14 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_isLower(p0) );
		return true;
	}
	if( i==15 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_isIdent(p0) );
		return true;
	}
	if( i==16 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_compare(p0) );
		return true;
	}
	if( i==17 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_equals(p0) );
		return true;
	}
	if( i==18 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_find(p0) );
		return true;
	}
	if( i==19 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_findAny(p0) );
		return true;
	}
	if( i==20 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_startsWith(p0) );
		return true;
	}
	if( i==21 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_startsWithAny(p0) );
		return true;
	}
	if( i==22 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_endsWith(p0) );
		return true;
	}
	if( i==23 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_endsWithAny(p0) );
		return true;
	}
	if( i==24 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_copy(p0) );
		return true;
	}
	if( i==25 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_indexOf(p0) );
		return true;
	}
	if( i==26 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_indexOfAny(p0) );
		return true;
	}
	if( i==27 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_upper(p0) );
		return true;
	}
	if( i==28 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_lower(p0) );
		return true;
	}
	if( i==29 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_strip(p0) );
		return true;
	}
	if( i==30 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_split(p0) );
		return true;
	}
	if( i==31 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_splitlines(p0) );
		return true;
	}
	if( i==32 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_splitargs(p0) );
		return true;
	}
	if( i==33 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_join(p0) );
		return true;
	}
	if( i==34 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_mid(p0) );
		return true;
	}
	if( i==35 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_xmid(p0) );
		return true;
	}
	if( i==36 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_left(p0) );
		return true;
	}
	if( i==37 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_right(p0) );
		return true;
	}
	if( i==38 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_front(p0) );
		return true;
	}
	if( i==39 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_back(p0) );
		return true;
	}
	if( i==40 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_replace(p0) );
		return true;
	}
	if( i==41 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaString*)base)->ILuaString_i.Lua_reverse(p0) );
		return true;
	}
	return false;
}


}


// !! This file has been automatically generated by the sugar sidl tool.
// !! Do not edit this file manually under risk of losing your changes.

