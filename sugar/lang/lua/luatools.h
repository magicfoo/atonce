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



#ifndef _luatools_h_
#define _luatools_h_





struct luaT_Reg
{
	const char*		name;
	lua_CFunction	func;
};


struct luaT_DbgWhere
{
	char			source[512];
	char			fname[512];
	int				lineno;

	enum CallerType
	{
		CALLT_C = 0,	// 'C' or 't'
		CALLT_M,		// 'm'
		CALLT_L,		// 'l'
	};

	CallerType		what;
	int				linedefined;
	int				lastlinedefined;
};


struct luaT_Chunk
{
	void*			addr;
	int				bcapacity;
	int				bsize;
};


struct luaT_Enum
{
	const char*		name;
	int				value;
};



typedef void*		luaT_Object;
typedef void*		luaT_Data;



void				luaT_tostringbuff	( lua_State *L, int ti, char* outbuff );
pcstr				luaT_getstring		( lua_State *L, int ti );

void				luaT_register		( lua_State *L, int ti, const luaT_Reg *l );
int					luaT_getmetafield	( lua_State *L, int obj, const char *e );
bool				luaT_hasfield		( lua_State *L, int obj, const char *k );
int					luaT_callmeta		( lua_State *L, int obj, const char *e );
int					luaT_typerror		( lua_State *L, int narg, const char *tname );
int					luaT_argerror		( lua_State *L, int numarg, const char *extramsg );
const char*			luaT_checklstring	( lua_State *L, int numArg, size_t *l );
const char*			luaT_optlstring		( lua_State *L, int numArg, const char *def, size_t *l );
lua_Number			luaT_checknumber	( lua_State *L, int numArg );
lua_Number			luaT_optnumber		( lua_State *L, int nArg, lua_Number def );
lua_Integer			luaT_checkinteger	( lua_State *L, int numArg );
bool				luaT_checkboolean	( lua_State *L, int narg);
lua_Integer			luaT_optinteger		( lua_State *L, int nArg, lua_Integer def );
bool				luaT_optboolean		( lua_State *L, int nArg, bool def );
void				luaT_checkstack		( lua_State *L, int sz, const char *msg);
void				luaT_checktype		( lua_State *L, int narg, int t);
void				luaT_checkany		( lua_State *L, int narg);
int					luaT_error			( lua_State *L, const char *fmt, ...);
int					luaT_checkoption	( lua_State *L, int narg, const char *def, const char *const lst[] );
int					luaT_loadbuffer		( lua_State *L, const char *buff, size_t sz, const char *name );
int					luaT_loadstring		( lua_State *L, const char *s );

bool				luaT_isinteger		( lua_State *L, int ti );
bool				luaT_isscalar		( lua_State *L, int ti );
bool				luaT_ispurearray	( lua_State *L, int ti );
bool				luaT_istableempty	( lua_State *L, int ti );
int					luaT_tablelen		( lua_State *L, int ti );

void				luaT_cleartable		( lua_State *L, int ti );
void				luaT_duptable		( lua_State *L, int ti );
void				luaT_copytable		( lua_State *L, int dsti, int srci );
void				luaT_cleartable		( lua_State *L, int ti );
void				luaT_resizetable	( lua_State *L, int ti, int n );

void				luaT_where			( lua_State *L, int lvl=0 );
int					luaT_fmain			( lua_State *L, int from_lvl=0 );
int					luaT_fmain_env		( lua_State *L, int from_lvl=0 );
int					luaT_buildpathtable	( lua_State *L, int ti, const char* inpath );
int					luaT_dbgwhere		( lua_State* L, int lvl, luaT_DbgWhere*, int count );
int					luaT_traceback		( lua_State* L );
lua_State*			luaT_getthread		( lua_State* L, int& arg );



// code operation
// =0: successed
// !0: failed
//		LUA_ERRRUN
//		LUA_ERRSYNTAX
//		LUA_ERRMEM
//		LUA_ERRERR
//		-1 (invalid parameters)
//		-2 (output chunk is too small)
//		-3 (invalid code object)

bool				luaT_isbytecode		( luaT_Chunk& inc );
bool				luaT_isbytecode		( lua_State* L, int idx );

// pushcode
// =0 ( -- code )
// !0 ( -- )
int					luaT_pushcode		( lua_State* L, luaT_Chunk& inc, const char* inname, const char* inpath );

// dumpcode. can dump lua function or code.
// =0 ( -- )
// !0 ( -- )
int					luaT_dumpcode		( lua_State* L, luaT_Chunk& outc, int idx, bool instrip );

// loadcode
// =0 ( -- func )
// !0 ( -- reason-str )
int					luaT_loadcode		( lua_State* L, int idx );




#define luaT_argcheck(L,cond,numarg,extramsg)		((void)((cond) || luaT_argerror(L, (numarg), (extramsg))))
#define	luaT_checkstring(L,n)						(luaT_checklstring(L, (n), NULL))
#define luaT_optstring(L,n,d)						(luaT_optlstring(L, (n), (d), NULL))
#define luaT_checkint(L,n)							((int)luaT_checkinteger(L, (n)))
#define luaT_checktable(L,n)						luaT_checktype(L,(n),LUA_TTABLE)
#define luaT_checkudata(L,n)						luaT_checktype(L,(n),LUA_TUSERDATA)
#define luaT_checkludata(L,n)						luaT_checktype(L,(n),LUA_TLIGHTUSERDATA)
#define luaT_optint(L,n,d)							((int)luaT_optinteger(L, (n), (d)))
#define luaT_checklong(L,n)							((long)luaT_checkinteger(L, (n)))
#define luaT_optlong(L,n,d)							((long)luaT_optinteger(L, (n), (d)))
#define luaT_typename(L,i)							lua_typename(L, lua_type(L,(i)))
#define luaT_istype(L,i,t)							(lua_type(L,i)==(t))
#define luaT_opt(L,f,n,d)							(lua_isnoneornil(L,(n))?(d):f(L,(n)))
#define luaT_getn(L,i)								((int)lua_objlen(L, i))

// convert a stack index to an absolute safe index
#define luaT_absidx(L,i)							(((i)>0||(i)<=LUA_REGISTRYINDEX)?(i):lua_gettop(L)+(i)+1)



#ifdef _DEBUG
	#define LUAT_DBGWHERE(L) \
		luaT_DbgWhere luaT_dbgw[16]; \
		util::Zero(luaT_dbgw); \
		luaT_dbgwhere(L,1,luaT_dbgw,16);
#else
	#define LUAT_DBGWHERE(L)
#endif




//
// 3 ways to inject C objects in the Lua vm
//

// way 1 :
//
// import ISwitch's method calls to a Lua table
// !! be aware that the object is not counter referenced by the Lua vm
// returns amount of imported functions

uint						luaT_swimport			( lua_State *L, int idx, su::ISwitch* insw, pcstr innamefilter=NULL );



// way 2 :
//
// push an object interface on the stack, as a full standalone Lua object, with dtor management.
// a reference is taken on the object and released when garbaged.
// if the object has an ISwitch interface, switches will be exported to Lua.

bool						luaT_swpushobject		( lua_State *L, su::IInterface* inobj, pcstr innamefilter=NULL, luaT_Enum* inenums=NULL );
bool						luaT_swisobject			( lua_State *L, int idx );
su::IInterface*				luaT_swtoobject			( lua_State *L, int idx );
template <typename T> T*	luaT_swtoobject			( lua_State *L, int idx );
su::IInterface*				luaT_swcheckobject		( lua_State *L, int idx );
template <typename T> T*	luaT_swcheckobject		( lua_State *L, int idx );
void						luaT_swsetobject		( lua_State *L, int idx );



// way 3 :
//
// push an (sub-)object data on the stack, by content, and delegates the management
// to a third party ISwitched object

void*						luaT_swpushdata			( lua_State *L, uint32 intypeid, void* indata, int inbsize, su::ISwitch* insw, pcstr innamefilter=NULL, pcstr indtorname="gc" );
uint32						luaT_swdatatypeid		( lua_State *L, int idx );
void*						luaT_swtodata			( lua_State *L, int idx, uint32 intypeid );
template <typename T> T*	luaT_swtodata			( lua_State *L, int idx, uint32 intypeid );
template <typename T> T*	luaT_swdatactor			( lua_State *L, int idx, uint32 intypeid );
template <typename T> T*	luaT_swdatadtor			( lua_State *L, int idx, uint32 intypeid );





//
// C object holding a reference to a lua object

void						luaT_registerObject		( lua_State* L, void* key );
void						luaT_unregisterObject	( lua_State* L, void* key );
void						luaT_callObject			( lua_State* L, void* key, pcstr method, int nargs=0, int nresults=0 );




// inject numerical vector

void*						luaT_newvector			( lua_State *L, int size, int count=1 );
template <typename T> T*	luaT_pushvector			( lua_State *L, const T& t, int count=1 );
int							luaT_vlen				( lua_State *L, int idx );
void*						luaT_vdata				( lua_State *L, int idx );
void						luaT_vcpy				( lua_State *L, int didx, int sidx );
bool						luaT_veq				( lua_State *L, int idx1, int idx2 );
pcstr						luaT_pushvstr			( lua_State *L, int idx );
//
int							luaT_vlb				( lua_State *L, int idx, int n );
int							luaT_vlh				( lua_State *L, int idx, int n );
int							luaT_vlw				( lua_State *L, int idx, int n );
int							luaT_vld				( lua_State *L, int idx, int n );
int							luaT_vlq				( lua_State *L, int idx, int n );
int							luaT_vlf				( lua_State *L, int idx, int n );
//
int							luaT_vsb				( lua_State *L, int idx );
int							luaT_vsh				( lua_State *L, int idx );
int							luaT_vsw				( lua_State *L, int idx );
int							luaT_vsd				( lua_State *L, int idx );
int							luaT_vsq				( lua_State *L, int idx );
int							luaT_vsf				( lua_State *L, int idx );
//
int							luaT_vaddb				( lua_State *L, int didx, int idx1, int idx2 );
int							luaT_vaddh				( lua_State *L, int didx, int idx1, int idx2 );
int							luaT_vaddw				( lua_State *L, int didx, int idx1, int idx2 );
int							luaT_vaddd				( lua_State *L, int didx, int idx1, int idx2 );
int							luaT_vaddf				( lua_State *L, int didx, int idx1, int idx2 );
//
int							luaT_vsubb				( lua_State *L, int didx, int idx1, int idx2 );
int							luaT_vsubh				( lua_State *L, int didx, int idx1, int idx2 );
int							luaT_vsubw				( lua_State *L, int didx, int idx1, int idx2 );
int							luaT_vsubd				( lua_State *L, int didx, int idx1, int idx2 );
int							luaT_vsubf				( lua_State *L, int didx, int idx1, int idx2 );
//
int							luaT_vextb				( lua_State *L, int sidx, int idx1, int idx2 );
int							luaT_vexth				( lua_State *L, int sidx, int idx1, int idx2 );
int							luaT_vextw				( lua_State *L, int sidx, int idx1, int idx2 );
int							luaT_vextd				( lua_State *L, int sidx, int idx1, int idx2 );
//
int							luaT_vpackb				( lua_State *L, int didx, int idx1, int idx2 );
int							luaT_vpackh				( lua_State *L, int didx, int idx1, int idx2 );
int							luaT_vpackw				( lua_State *L, int didx, int idx1, int idx2 );
int							luaT_vpackd				( lua_State *L, int didx, int idx1, int idx2 );
//
int							luaT_vslb				( lua_State *L, int idx, int n );
int							luaT_vslh				( lua_State *L, int idx, int n );
int							luaT_vslw				( lua_State *L, int idx, int n );
int							luaT_vsld				( lua_State *L, int idx, int n );
//
int							luaT_vsrb				( lua_State *L, int idx, int n );
int							luaT_vsrh				( lua_State *L, int idx, int n );
int							luaT_vsrw				( lua_State *L, int idx, int n );
int							luaT_vsrd				( lua_State *L, int idx, int n );





#include "luatools_i.h"
#endif	// _luatools_h_



