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
#include "su_iluagraphic.h"
#include "..\..\core\base\su_iswitch.h"
#include <stdarg.h>




//==== defines =====================================================================================

#ifndef SIDL_IMPL_CLASSNAME
#define SIDL_IMPL_CLASSNAME MLuaGraphic
#endif

#ifndef SIDL_IMPL_MUID
#define SIDL_IMPL_MUID 0xf1bc602eu
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
#define SIDL_STUB_CLASSNAME MLuaGraphic_Stub
#endif

#ifndef SIDL_IMPL_INTERFACE
#define SIDL_IMPL_INTERFACE(I) ((I*)MLuaGraphic_get_interface(MLuaGraphic_stub_to_base(this),I::ClassId))
#endif


//==== declarations ================================================================================

namespace {

su::IInterface* MLuaGraphic_create_object( su::Pool*, su::Pool*, su::AnyList* );
void* MLuaGraphic_base_to_stub ( void* );
void* MLuaGraphic_stub_to_base ( void* );
su::addin::Builder* MLuaGraphic_builder ( );
uint MLuaGraphic_count_interface ( );
bool MLuaGraphic_enum_interface ( uint, su::classid*, pcstr* );
su::IInterface* MLuaGraphic_first_interface ( void* );
su::IInterface* MLuaGraphic_get_interface ( void*, su::classid );
uint MLuaGraphic_count_switch ( );
bool MLuaGraphic_enum_switch ( uint, su::ISwitch::Desc& );
int MLuaGraphic_find_switch ( pcstr );
bool MLuaGraphic_call_switch_stack ( void*, uint, void*, va_list );
bool MLuaGraphic_call_switch_mem ( void*, uint, su::Any*, int, su::Any* );

}


//==== stub area ===================================================================================

#undef  SIDL_PROPER
#undef  SIDL_STUB
#undef  SIDL_BUILDER
#undef  SIDL_CODE
#define SIDL_STUB
#include "su_mluagraphic_stub.h"




//==== skeleton ====================================================================================

namespace {


struct MLuaGraphic
{

//---- bridges -------------------------------------------------------------------------------------

struct ILuaGraphic_t : public su::ILuaGraphic {
	// from IInterface
	::su::classbase         GetBase              (  ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return &(*m); }
	::su::addin::Builder *  GetBuilder           (  ) { return MLuaGraphic_builder(); }
	void                    Retain               (  ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); (*m).Retain(); }
	::uint                  CountRef             (  ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return (*m).CountRef(); }
	void                    Release              (  ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); (*m).Release(); }
	::uint                  CountInterface       (  ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return (*m).CountInterface(); }
	bool                    EnumInterface        ( ::uint p0,::su::classid * p1,::pcstr * p2 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return (*m).EnumInterface(p0,p1,p2); }
	::su::IInterface *      GetInterface         ( ::su::classid p0 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return (*m).GetInterface(p0); }
	// from ILuaGraphic
	int                     Lua_newDirectDraw    ( lua_State* p0 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return (m->stub).Lua_newDirectDraw(p0); }
	int                     LuaDD_realize        ( lua_State* p0 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return (m->stub).LuaDD_realize(p0); }
	int                     LuaDD_createView     ( lua_State* p0 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return (m->stub).LuaDD_createView(p0); }
	int                     LuaDD_unrealize      ( lua_State* p0 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return (m->stub).LuaDD_unrealize(p0); }
	int                     LuaDD_countDisplay   ( lua_State* p0 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return (m->stub).LuaDD_countDisplay(p0); }
	int                     LuaDD_countScreen    ( lua_State* p0 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return (m->stub).LuaDD_countScreen(p0); }
	int                     LuaDD_getShaderModel ( lua_State* p0 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return (m->stub).LuaDD_getShaderModel(p0); }
	int                     LuaDD_gc             ( lua_State* p0 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ILuaGraphic_i,this); return (m->stub).LuaDD_gc(p0); }
} ILuaGraphic_i;


struct ISwitch_t : public su::ISwitch {
	// from IInterface
	::su::classbase         GetBase        (  ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ISwitch_i,this); return &(*m); }
	::su::addin::Builder *  GetBuilder     (  ) { return MLuaGraphic_builder(); }
	void                    Retain         (  ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ISwitch_i,this); (*m).Retain(); }
	::uint                  CountRef       (  ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ISwitch_i,this); return (*m).CountRef(); }
	void                    Release        (  ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ISwitch_i,this); (*m).Release(); }
	::uint                  CountInterface (  ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ISwitch_i,this); return (*m).CountInterface(); }
	bool                    EnumInterface  ( ::uint p0,::su::classid * p1,::pcstr * p2 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ISwitch_i,this); return (*m).EnumInterface(p0,p1,p2); }
	::su::IInterface *      GetInterface   ( ::su::classid p0 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ISwitch_i,this); return (*m).GetInterface(p0); }
	// from ISwitch
	::uint                  CountSwitch    (  ) { return MLuaGraphic_count_switch(); }
	bool                    EnumSwitch     ( ::uint p0,::su::ISwitch::Desc & p1 ) { return MLuaGraphic_enum_switch(p0,p1); }
	int                     FindSwitch     ( ::pcstr p0 ) { return MLuaGraphic_find_switch(p0); }
	bool                    CallSwitch     ( ::uint p0,void * p1,... ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ISwitch_i,this); va_list vl; va_start(vl,p1); return MLuaGraphic_call_switch_stack(m,p0,p1,vl); va_end(vl); }
	bool                    CallSwitchMem  ( ::uint p0,::su::Any * p1,int p2,::su::Any * p3 ) { MLuaGraphic* m = SU_DELEGATE_BASEPTR(MLuaGraphic,ISwitch_i,this); return MLuaGraphic_call_switch_mem(m,p0,p1,p2,p3); }
} ISwitch_i;


//---- stub nested object --------------------------------------------------------------------------

MLuaGraphic_Stub stub;


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
	return MLuaGraphic_count_interface();
}


bool EnumInterface ( uint i, su::classid* cid, pcstr* cname )
{
	return MLuaGraphic_enum_interface(i,cid,cname);
}


su::IInterface* GetInterface ( classid cid )
{
	SU_IMPLEMENTS_MEMBER( ILuaGraphic_i, cid );
	SU_IMPLEMENTS_MEMBER( ISwitch_i, cid );
	return NULL;
}


static MLuaGraphic* Create( Pool* np, Pool* dp, AnyList* o )
{
	static MLuaGraphic m;
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


static void Destroy( MLuaGraphic* m )
{
	m->stub.Shut();
	m->refcount=0;
}

};

}




//==== builder =====================================================================================


SU_BEGIN_BUILDER( MLuaGraphic )

su::classid GetMUID ( ) { return SIDL_IMPL_MUID; }
pcstr GetMClassname ( ) { return "MLuaGraphic"; }
uint32 GetRevision ( ) { return SIDL_IMPL_REVISION; }
pcstr GetPackage ( ) { return SIDL_IMPL_PACKAGE; }
uint CountInterface ( ) { return MLuaGraphic_count_interface(); }
bool EnumInterface ( uint i, su::classid* cid, pcstr* cname ) { return MLuaGraphic_enum_interface(i,cid,cname); }
su::IInterface* FirstInterface ( void* base ) { return MLuaGraphic_first_interface(base); }
su::IInterface* GetInterface ( void* base, su::classid cid ) { return MLuaGraphic_get_interface(base,cid); }
su::IInterface* CreateObject( su::Pool* nestp, su::Pool* datap, su::AnyList* opts ) { return MLuaGraphic_create_object(nestp,datap,opts); }
void* BaseToStub ( void* base ) { return MLuaGraphic_base_to_stub(base); }
void* StubToBase ( void* base ) { return MLuaGraphic_stub_to_base(base); }
uint CountSwitch ( ) { return MLuaGraphic_count_switch(); }
bool EnumSwitch ( uint i, su::ISwitch::Desc& desc ) { return MLuaGraphic_enum_switch(i,desc); }

SU_END_BUILDER( MLuaGraphic )




//==== definitions =================================================================================


namespace {


su::IInterface* MLuaGraphic_create_object( su::Pool* nestp, su::Pool* datap, su::AnyList* opts )
{
	MLuaGraphic* m = MLuaGraphic::Create( nestp, datap, opts );
	SU_REQUIRE_OR_NULL( m );
	return &m->ILuaGraphic_i;
}


void* MLuaGraphic_base_to_stub ( void* base )
{
	SU_REQUIRE_OR_NULL( base );
	return &(((MLuaGraphic*)base)->stub);
}


void* MLuaGraphic_stub_to_base ( void* stubp )
{
	SU_REQUIRE_OR_NULL( stubp );
	return SU_DELEGATE_BASEPTR(MLuaGraphic,stub,stubp);
}


su::addin::Builder* MLuaGraphic_builder ( )
{
	return &_builder_MLuaGraphic;
}


uint MLuaGraphic_count_interface ( )
{
	return 2;
}


bool MLuaGraphic_enum_interface ( uint i, su::classid* cid, pcstr* cname )
{
	static const su::classid cids[] = { su::ILuaGraphic::ClassId, su::ISwitch::ClassId };
	static const pcstr cnames[] = { "ILuaGraphic", "ISwitch" };
	SU_REQUIRE_OR_FALSE( i<2 );
	if( cid )   *cid   = cids[i];
	if( cname ) *cname = cnames[i];
	return true;
}


su::IInterface* MLuaGraphic_first_interface ( void* base )
{
	SU_REQUIRE_OR_NULL( base );
	return &(((MLuaGraphic*)base)->ILuaGraphic_i);
}


su::IInterface* MLuaGraphic_get_interface ( void* base, su::classid cid )
{
	SU_REQUIRE_OR_NULL( base );
	return ((MLuaGraphic*)base)->GetInterface(cid);
}


uint MLuaGraphic_count_switch ( )
{
	return 8;
}


bool MLuaGraphic_enum_switch ( uint i, su::ISwitch::Desc& desc )
{
	static su::ISwitch::Desc swdesc[] = {
		{ "newDirectDraw", "Lua_newDirectDraw", "int,lua_State*", "wp", "ILuaGraphic", NULL },
		{ "realize", "LuaDD_realize", "int,lua_State*", "wp", "ILuaGraphic", NULL },
		{ "createView", "LuaDD_createView", "int,lua_State*", "wp", "ILuaGraphic", NULL },
		{ "unrealize", "LuaDD_unrealize", "int,lua_State*", "wp", "ILuaGraphic", NULL },
		{ "countDisplay", "LuaDD_countDisplay", "int,lua_State*", "wp", "ILuaGraphic", NULL },
		{ "countScreen", "LuaDD_countScreen", "int,lua_State*", "wp", "ILuaGraphic", NULL },
		{ "getShaderModel", "LuaDD_getShaderModel", "int,lua_State*", "wp", "ILuaGraphic", NULL },
		{ "gc", "LuaDD_gc", "int,lua_State*", "wp", "ILuaGraphic", NULL },
		};
	SU_REQUIRE_OR_FALSE( i<8 );
	desc = swdesc[i];
	return true;
}


int MLuaGraphic_find_switch ( pcstr inname=NULL )
{
	SU_REQUIRE_OR_NEG( inname && inname[0] );
	for( uint i=0 ; i<8 ; i++ ) {
		su::ISwitch::Desc d; MLuaGraphic_enum_switch(i,d);
		if( util::Strequals(inname,d.name,false) )
			return i;
		}
	return -1;
}


bool MLuaGraphic_call_switch_stack ( void* base, uint i, void* r, va_list args )
{
	SU_REQUIRE_OR_FALSE( base );
	SU_REQUIRE_OR_FALSE( i<8 );
	if( i==0 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaGraphic*)base)->ILuaGraphic_i.Lua_newDirectDraw(p0);
		return true;
	}
	if( i==1 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_realize(p0);
		return true;
	}
	if( i==2 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_createView(p0);
		return true;
	}
	if( i==3 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_unrealize(p0);
		return true;
	}
	if( i==4 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_countDisplay(p0);
		return true;
	}
	if( i==5 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_countScreen(p0);
		return true;
	}
	if( i==6 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_getShaderModel(p0);
		return true;
	}
	if( i==7 ) {
		SU_ASSERT( r );
		lua_State* p0 = va_arg(args,lua_State*);
		*((int*)r) = ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_gc(p0);
		return true;
	}
	return false;
}


bool MLuaGraphic_call_switch_mem ( void* base, uint i, Any* r, int argc, Any* argv )
{
	SU_REQUIRE_OR_FALSE( base );
	SU_REQUIRE_OR_FALSE( i<8 );
	if( i==0 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaGraphic*)base)->ILuaGraphic_i.Lua_newDirectDraw(p0) );
		return true;
	}
	if( i==1 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_realize(p0) );
		return true;
	}
	if( i==2 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_createView(p0) );
		return true;
	}
	if( i==3 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_unrealize(p0) );
		return true;
	}
	if( i==4 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_countDisplay(p0) );
		return true;
	}
	if( i==5 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_countScreen(p0) );
		return true;
	}
	if( i==6 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_getShaderModel(p0) );
		return true;
	}
	if( i==7 ) {
		SU_ASSERT( argc==1 && argv )
		SU_ASSERT( r )
		lua_State* p0 = (lua_State*)argv[0].asPointer();
		r->set( ((MLuaGraphic*)base)->ILuaGraphic_i.LuaDD_gc(p0) );
		return true;
	}
	return false;
}


}


// !! This file has been automatically generated by the sugar sidl tool.
// !! Do not edit this file manually under risk of losing your changes.

