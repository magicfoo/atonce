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
#include "su_ilualang.h"
#include "..\..\core\base\su_iswitch.h"
#include <stdarg.h>




//==== defines =====================================================================================

#ifndef SIDL_IMPL_CLASSNAME
#define SIDL_IMPL_CLASSNAME MLuaLang
#endif

#ifndef SIDL_IMPL_MUID
#define SIDL_IMPL_MUID 0x4b19492fu
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
#define SIDL_STUB_CLASSNAME MLuaLang_Stub
#endif

#ifndef SIDL_IMPL_INTERFACE
#define SIDL_IMPL_INTERFACE(I) ((I*)MLuaLang_get_interface(MLuaLang_stub_to_base(this),I::ClassId))
#endif


//==== declarations ================================================================================

namespace {

su::IInterface* MLuaLang_create_object( su::Pool*, su::Pool*, su::AnyList* );
void* MLuaLang_base_to_stub ( void* );
void* MLuaLang_stub_to_base ( void* );
su::addin::Builder* MLuaLang_builder ( );
uint MLuaLang_count_interface ( );
bool MLuaLang_enum_interface ( uint, su::classid*, pcstr* );
su::IInterface* MLuaLang_first_interface ( void* );
su::IInterface* MLuaLang_get_interface ( void*, su::classid );
uint MLuaLang_count_switch ( );
bool MLuaLang_enum_switch ( uint, su::ISwitch::Desc& );
int MLuaLang_find_switch ( pcstr );
bool MLuaLang_call_switch_stack ( void*, uint, void*, va_list );
bool MLuaLang_call_switch_mem ( void*, uint, su::Any*, int, su::Any* );

}


//==== stub area ===================================================================================

#undef  SIDL_PROPER
#undef  SIDL_STUB
#undef  SIDL_BUILDER
#undef  SIDL_CODE
#define SIDL_STUB
#include "su_mlualang_stub.h"




//==== skeleton ====================================================================================

namespace {


struct MLuaLang
{

//---- bridges -------------------------------------------------------------------------------------

struct ILuaLang_t : public su::ILuaLang {
	// from IInterface
	::su::classbase         GetBase           (  ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return &(*m); }
	::su::addin::Builder *  GetBuilder        (  ) { return MLuaLang_builder(); }
	void                    Retain            (  ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); (*m).Retain(); }
	::uint                  CountRef          (  ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (*m).CountRef(); }
	void                    Release           (  ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); (*m).Release(); }
	::uint                  CountInterface    (  ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (*m).CountInterface(); }
	bool                    EnumInterface     ( ::uint p0,::su::classid * p1,::pcstr * p2 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (*m).EnumInterface(p0,p1,p2); }
	::su::IInterface *      GetInterface      ( ::su::classid p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (*m).GetInterface(p0); }
	// from ILuaLang
	int                     Lua_error         ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_error(p0); }
	int                     Lua_traceback     ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_traceback(p0); }
	int                     Lua_breakpoint    ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_breakpoint(p0); }
	int                     Lua_where         ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_where(p0); }
	int                     Lua_getInfo       ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_getInfo(p0); }
	int                     Lua_getRegistry   ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_getRegistry(p0); }
	int                     Lua_getLocal      ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_getLocal(p0); }
	int                     Lua_findLocal     ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_findLocal(p0); }
	int                     Lua_setLocal      ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_setLocal(p0); }
	int                     Lua_getChunkLevel ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_getChunkLevel(p0); }
	int                     Lua_isBytecode    ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_isBytecode(p0); }
	int                     Lua_parseFile     ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_parseFile(p0); }
	int                     Lua_parseText     ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_parseText(p0); }
	int                     Lua_dumpCode      ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_dumpCode(p0); }
	int                     Lua_loadCode      ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_loadCode(p0); }
	int                     Lua_sizeOf        ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_sizeOf(p0); }
	int                     Lua_isNone        ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_isNone(p0); }
	int                     Lua_isNumber      ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_isNumber(p0); }
	int                     Lua_isInteger     ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_isInteger(p0); }
	int                     Lua_toInteger     ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_toInteger(p0); }
	int                     Lua_isNil         ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_isNil(p0); }
	int                     Lua_isString      ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_isString(p0); }
	int                     Lua_isBinary      ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_isBinary(p0); }
	int                     Lua_isFunction    ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_isFunction(p0); }
	int                     Lua_isTable       ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_isTable(p0); }
	int                     Lua_toTable       ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_toTable(p0); }
	int                     Lua_clearTable    ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_clearTable(p0); }
	int                     Lua_dupTable      ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_dupTable(p0); }
	int                     Lua_copyTable     ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_copyTable(p0); }
	int                     Lua_resizeTable   ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_resizeTable(p0); }
	int                     Lua_countTable    ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_countTable(p0); }
	int                     Lua_isArray       ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_isArray(p0); }
	int                     Lua_countArray    ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_countArray(p0); }
	int                     Lua_isEmpty       ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_isEmpty(p0); }
	int                     Lua_getKeys       ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_getKeys(p0); }
	int                     Lua_getValues     ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_getValues(p0); }
	int                     Lua_mapValues     ( lua_State* p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ILuaLang_i,this); return (m->stub).Lua_mapValues(p0); }
} ILuaLang_i;


struct ISwitch_t : public su::ISwitch {
	// from IInterface
	::su::classbase         GetBase        (  ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ISwitch_i,this); return &(*m); }
	::su::addin::Builder *  GetBuilder     (  ) { return MLuaLang_builder(); }
	void                    Retain         (  ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ISwitch_i,this); (*m).Retain(); }
	::uint                  CountRef       (  ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ISwitch_i,this); return (*m).CountRef(); }
	void                    Release        (  ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ISwitch_i,this); (*m).Release(); }
	::uint                  CountInterface (  ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ISwitch_i,this); return (*m).CountInterface(); }
	bool                    EnumInterface  ( ::uint p0,::su::classid * p1,::pcstr * p2 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ISwitch_i,this); return (*m).EnumInterface(p0,p1,p2); }
	::su::IInterface *      GetInterface   ( ::su::classid p0 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ISwitch_i,this); return (*m).GetInterface(p0); }
	// from ISwitch
	::uint                  CountSwitch    (  ) { return MLuaLang_count_switch(); }
	bool                    EnumSwitch     ( ::uint p0,::su::ISwitch::Desc & p1 ) { return MLuaLang_enum_switch(p0,p1); }
	int                     FindSwitch     ( ::pcstr p0 ) { return MLuaLang_find_switch(p0); }
	bool                    CallSwitch     ( ::uint p0,void * p1,... ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ISwitch_i,this); va_list vl; va_start(vl,p1); return MLuaLang_call_switch_stack(m,p0,p1,vl); va_end(vl); }
	bool                    CallSwitchMem  ( ::uint p0,::su::Any * p1,int p2,::su::Any * p3 ) { MLuaLang* m = SU_DELEGATE_BASEPTR(MLuaLang,ISwitch_i,this); return MLuaLang_call_switch_mem(m,p0,p1,p2,p3); }
} ISwitch_i;


//---- stub nested object --------------------------------------------------------------------------

MLuaLang_Stub stub;


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
	return MLuaLang_count_interface();
}


bool EnumInterface ( uint i, su::classid* cid, pcstr* cname )
{
	return MLuaLang_enum_interface(i,cid,cname);
}


su::IInterface* GetInterface ( classid cid )
{
	SU_IMPLEMENTS_MEMBER( ILuaLang_i, cid );
	SU_IMPLEMENTS_MEMBER( ISwitch_i, cid );
	return NULL;
}


static MLuaLang* Create( Pool* np, Pool* dp, AnyList* o )
{
	static MLuaLang m;
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


static void Destroy( MLuaLang* m )
{
	m->stub.Shut();
	m->refcount=0;
}

};

}




//==== builder =====================================================================================


SU_BEGIN_BUILDER( MLuaLang )

su::classid GetMUID ( ) { return SIDL_IMPL_MUID; }
pcstr GetMClassname ( ) { return "MLuaLang"; }
uint32 GetRevision ( ) { return SIDL_IMPL_REVISION; }
pcstr GetPackage ( ) { return SIDL_IMPL_PACKAGE; }
uint CountInterface ( ) { return MLuaLang_count_interface(); }
bool EnumInterface ( uint i, su::classid* cid, pcstr* cname ) { return MLuaLang_enum_interface(i,cid,cname); }
su::IInterface* FirstInterface ( void* base ) { return MLuaLang_first_interface(base); }
su::IInterface* GetInterface ( void* base, su::classid cid ) { return MLuaLang_get_interface(base,cid); }
su::IInterface* CreateObject( su::Pool* nestp, su::Pool* datap, su::AnyList* opts ) { return MLuaLang_create_object(nestp,datap,opts); }
void* BaseToStub ( void* base ) { return MLuaLang_base_to_stub(base); }
void* StubToBase ( void* base ) { return MLuaLang_stub_to_base(base); }
uint CountSwitch ( ) { return MLuaLang_count_switch(); }
bool EnumSwitch ( uint i, su::ISwitch::Desc& desc ) { return MLuaLang_enum_switch(i,desc); }

SU_END_BUILDER( MLuaLang )




//==== definitions =================================================================================


namespace {


su::IInterface* MLuaLang_create_object( su::Pool* nestp, su::Pool* datap, su::AnyList* opts )
{
	MLuaLang* m = MLuaLang::Create( nestp, datap, opts );
	SU_REQUIRE_OR_NULL( m );
	return &m->ILuaLang_i;
}


void* MLuaLang_base_to_stub ( void* base )
{
	SU_REQUIRE_OR_NULL( base );
	return &(((MLuaLang*)base)->stub);
}


void* MLuaLang_stub_to_base ( void* stubp )
{
	SU_REQUIRE_OR_NULL( stubp );
	return SU_DELEGATE_BASEPTR(MLuaLang,stub,stubp);
}


su::addin::Builder* MLuaLang_builder ( )
{
	return &_builder_MLuaLang;
}


uint MLuaLang_count_interface ( )
{
	return 2;
}


bool MLuaLang_enum_interface ( uint i, su::classid* cid, pcstr* cname )
{
	static const su::classid cids[] = { su::ILuaLang::ClassId, su::ISwitch::ClassId };
	static const pcstr cnames[] = { "ILuaLang", "ISwitch" };
	SU_REQUIRE_OR_FALSE( i<2 );
	if( cid )   *cid   = cids[i];
	if( cname ) *cname = cnames[i];
	return true;
}


su::IInterface* MLuaLang_first_interface ( void* base )
{
	SU_REQUIRE_OR_NULL( base );
	return &(((MLuaLang*)base)->ILuaLang_i);
}


su::IInterface* MLuaLang_get_interface ( void* base, su::classid cid )
{
	SU_REQUIRE_OR_NULL( base );
	return ((MLuaLang*)base)->GetInterface(cid);
}


uint MLuaLang_count_switch ( )
{
	return 37;
}


bool MLuaLang_enum_switch ( uint i, su::ISwitch::Desc& desc )
{
	static su::ISwitch::Desc swdesc[] = {
		{ "error", "Lua_error", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "traceback", "Lua_traceback", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "breakpoint", "Lua_breakpoint", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "where", "Lua_where", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "getInfo", "Lua_getInfo", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "getRegistry", "Lua_getRegistry", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "getLocal", "Lua_getLocal", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "findLocal", "Lua_findLocal", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "setLocal", "Lua_setLocal", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "getChunkLevel", "Lua_getChunkLevel", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "isBytecode", "Lua_isBytecode", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "parseFile", "Lua_parseFile", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "parseText", "Lua_parseText", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "dumpCode", "Lua_dumpCode", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "loadCode", "Lua_loadCode", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "sizeOf", "Lua_sizeOf", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "isNone", "Lua_isNone", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "isNumber", "Lua_isNumber", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "isInteger", "Lua_isInteger", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "toInteger", "Lua_toInteger", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "isNil", "Lua_isNil", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "isString", "Lua_isString", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "isBinary", "Lua_isBinary", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "isFunction", "Lua_isFunction", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "isTable", "Lua_isTable", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "toTable", "Lua_toTable", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "clearTable", "Lua_clearTable", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "dupTable", "Lua_dupTable", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "copyTable", "Lua_copyTable", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "resizeTable", "Lua_resizeTable", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "countTable", "Lua_countTable", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "isArray", "Lua_isArray", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "countArray", "Lua_countArray", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "isEmpty", "Lua_isEmpty", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "getKeys", "Lua_getKeys", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "getValues", "Lua_getValues", "int,lua_State*", "wp", "ILuaLang", NULL },
		{ "mapValues", "Lua_mapValues", "int,lua_State*", "wp", "ILuaLang", NULL },
		};
	SU_REQUIRE_OR_FALSE( i<37 );
	desc = swdesc[i];
	return true;
}


int MLuaLang_find_switch ( pcstr inname=NULL )
{
	SU_REQUIRE_OR_NEG( inname && inname[0] );
	for( uint i=0 ; i<37 ; i++ ) {
		su::ISwitch::Desc d; MLuaLang_enum_switch(i,d);
		if( util::Strequals(inname,d.name,false) )
			return i;
		}
	return -1;
}


bool MLuaLang_call_switch_stack ( void* base, uint i, void* r, va_list args )
{
	SU_REQUIRE_OR_FALSE( base );
	SU_REQUIRE_OR_FALSE( i<37 );
	if( i==0 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_error(p0);
		return true;
	}
	if( i==1 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_traceback(p0);
		return true;
	}
	if( i==2 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_breakpoint(p0);
		return true;
	}
	if( i==3 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_where(p0);
		return true;
	}
	if( i==4 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_getInfo(p0);
		return true;
	}
	if( i==5 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_getRegistry(p0);
		return true;
	}
	if( i==6 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_getLocal(p0);
		return true;
	}
	if( i==7 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_findLocal(p0);
		return true;
	}
	if( i==8 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_setLocal(p0);
		return true;
	}
	if( i==9 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_getChunkLevel(p0);
		return true;
	}
	if( i==10 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_isBytecode(p0);
		return true;
	}
	if( i==11 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_parseFile(p0);
		return true;
	}
	if( i==12 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_parseText(p0);
		return true;
	}
	if( i==13 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_dumpCode(p0);
		return true;
	}
	if( i==14 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_loadCode(p0);
		return true;
	}
	if( i==15 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_sizeOf(p0);
		return true;
	}
	if( i==16 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_isNone(p0);
		return true;
	}
	if( i==17 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_isNumber(p0);
		return true;
	}
	if( i==18 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_isInteger(p0);
		return true;
	}
	if( i==19 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_toInteger(p0);
		return true;
	}
	if( i==20 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_isNil(p0);
		return true;
	}
	if( i==21 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_isString(p0);
		return true;
	}
	if( i==22 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_isBinary(p0);
		return true;
	}
	if( i==23 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_isFunction(p0);
		return true;
	}
	if( i==24 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_isTable(p0);
		return true;
	}
	if( i==25 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_toTable(p0);
		return true;
	}
	if( i==26 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_clearTable(p0);
		return true;
	}
	if( i==27 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_dupTable(p0);
		return true;
	}
	if( i==28 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_copyTable(p0);
		return true;
	}
	if( i==29 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_resizeTable(p0);
		return true;
	}
	if( i==30 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_countTable(p0);
		return true;
	}
	if( i==31 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_isArray(p0);
		return true;
	}
	if( i==32 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_countArray(p0);
		return true;
	}
	if( i==33 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_isEmpty(p0);
		return true;
	}
	if( i==34 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_getKeys(p0);
		return true;
	}
	if( i==35 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_getValues(p0);
		return true;
	}
	if( i==36 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaLang*)base)->ILuaLang_i.Lua_mapValues(p0);
		return true;
	}
	return false;
}


bool MLuaLang_call_switch_mem ( void* base, uint i, Any* r, int argc, Any* argv )
{
	SU_REQUIRE_OR_FALSE( base );
	SU_REQUIRE_OR_FALSE( i<37 );
	if( i==0 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_error(p0) );
		return true;
	}
	if( i==1 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_traceback(p0) );
		return true;
	}
	if( i==2 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_breakpoint(p0) );
		return true;
	}
	if( i==3 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_where(p0) );
		return true;
	}
	if( i==4 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_getInfo(p0) );
		return true;
	}
	if( i==5 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_getRegistry(p0) );
		return true;
	}
	if( i==6 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_getLocal(p0) );
		return true;
	}
	if( i==7 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_findLocal(p0) );
		return true;
	}
	if( i==8 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_setLocal(p0) );
		return true;
	}
	if( i==9 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_getChunkLevel(p0) );
		return true;
	}
	if( i==10 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_isBytecode(p0) );
		return true;
	}
	if( i==11 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_parseFile(p0) );
		return true;
	}
	if( i==12 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_parseText(p0) );
		return true;
	}
	if( i==13 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_dumpCode(p0) );
		return true;
	}
	if( i==14 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_loadCode(p0) );
		return true;
	}
	if( i==15 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_sizeOf(p0) );
		return true;
	}
	if( i==16 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_isNone(p0) );
		return true;
	}
	if( i==17 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_isNumber(p0) );
		return true;
	}
	if( i==18 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_isInteger(p0) );
		return true;
	}
	if( i==19 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_toInteger(p0) );
		return true;
	}
	if( i==20 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_isNil(p0) );
		return true;
	}
	if( i==21 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_isString(p0) );
		return true;
	}
	if( i==22 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_isBinary(p0) );
		return true;
	}
	if( i==23 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_isFunction(p0) );
		return true;
	}
	if( i==24 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_isTable(p0) );
		return true;
	}
	if( i==25 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_toTable(p0) );
		return true;
	}
	if( i==26 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_clearTable(p0) );
		return true;
	}
	if( i==27 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_dupTable(p0) );
		return true;
	}
	if( i==28 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_copyTable(p0) );
		return true;
	}
	if( i==29 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_resizeTable(p0) );
		return true;
	}
	if( i==30 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_countTable(p0) );
		return true;
	}
	if( i==31 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_isArray(p0) );
		return true;
	}
	if( i==32 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_countArray(p0) );
		return true;
	}
	if( i==33 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_isEmpty(p0) );
		return true;
	}
	if( i==34 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_getKeys(p0) );
		return true;
	}
	if( i==35 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_getValues(p0) );
		return true;
	}
	if( i==36 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaLang*)base)->ILuaLang_i.Lua_mapValues(p0) );
		return true;
	}
	return false;
}


}


// !! This file has been automatically generated by the sugar sidl tool.
// !! Do not edit this file manually under risk of losing your changes.

