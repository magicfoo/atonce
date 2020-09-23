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
using namespace util;



namespace
{

	const char char_is_space[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	const char char_is_hex[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	1, 2, 3, 4, 5, 6, 7, 8, 9,10, 0, 0, 0, 0, 0, 0,
		0,11,12,13,14,15,16, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0,11,12,13,14,15,16, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};


	pcstr my_strtoul ( pcstr p, ulong& ul, bool& nl )
	{
		SU_REQUIRE_OR_NULL( p );
		SU_REQUIRE_OR_NULL( *p );

		ul = 0;
		nl = false;

		while( char_is_space[*p] ) {
			if( *p=='\n' )	nl = true;
			p++;
		}

		pcstr hp = p;
		while( char_is_hex[*p] ) {
			ul = (ul<<4) | (char_is_hex[*p]-1);
			p++;
		}

		return (p==hp) ? NULL : p;
	}




	struct SIDL_STUB_CLASSNAME
	{

		bool Init ( Pool* p, AnyList* o )
		{
			return true;
		}


		void Shut ( )
		{
			//
		}

		int Lua_len ( lua_State* L )
		{
			int l = util::Strlen( lua_tostring(L,1) );
			lua_pushinteger( L, l );
			return 1;
		}


		int Lua_cvt (lua_State *L)
		{
			util::SafeBuffer<>	 buff;
			buff.zero();

			luaT_checkany(L, 1);
			luaT_tostringbuff( L, 1, buff );
			lua_pushstring( L, buff );
			return 1;
		}


		int Lua_match ( lua_State *L )
		{
			pcstr instring  = luaT_checkstring( L, 1 );
			pcstr inpattern = luaT_checkstring( L, 2 );
			bool  cs        = luaT_optboolean( L, 3, true );
			uint  fnflags   = cs ? 0 : util::FNM_IGNORECASE;
			bool  matched = util::Fnmatch( inpattern, instring, fnflags );
			lua_pushboolean( L, matched );
			return 1;
		}


		int Lua_format ( lua_State *L )
		{
			size_t sfl;
			const char *strfrmt = luaT_checklstring(L, 1, &sfl);
			const char *strfrmt_end = strfrmt+sfl;

			util::SafeBuffer<>	 buff;
			buff.zero();

			su::TString ts;

			while( strfrmt && strfrmt<strfrmt_end )
			{
				if( *strfrmt != '%' )
				{
					ts += *strfrmt++;
				}
				else if( *++strfrmt == '%' )
				{
					ts += *strfrmt++;
				}
				else
				{
					pstr ends;
					int argi = util::Strtoulong( strfrmt, &ends, 10 );
					strfrmt = ends;

					luaT_checkany( L, argi+1 );
					luaT_tostringbuff( L, argi+1, buff );
					ts += buff;
				}
			}

			lua_pushstring( L, ts.cstr() );

			return 1;
		}


		int Lua_decodeHex ( lua_State *L )
		{
			pcstr hexstr = luaT_checklstring( L, 1, NULL );

			// ( -- t )
			lua_newtable( L );

			bool  nl;
			ulong ul;
			pcstr s = hexstr;
			uint  r = 0;
			uint  w = 0;
			uint  k = 0;
			while( s=my_strtoul(s,ul,nl) )
			{
				lua_pushinteger( L, ++k );
				lua_pushnumber( L, ul );
				lua_settable( L, -3 );
				if( nl ) {
					w = Max(r,w);
					r = 0;
				} else {
					r++;
				}
			}

			if( w>1 )
			{
				// ( t -- mt t )
				lua_newtable( L );

				lua_pushinteger( L, w );
				lua_setfield( L, -2, "width" );

				// ( mt t -- t )
				lua_setmetatable( L, -2 );
			}

			return 1;
		}


		int Lua_decodeFloat ( lua_State *L )
		{
			pcstr hexstr = luaT_checklstring( L, 1, NULL );

			// ( -- t )
			lua_newtable( L );

			bool  nl;
			ulong ul;
			pcstr s = hexstr;
			uint  r = 0;
			uint  w = 0;
			uint  k = 0;
			while( s=my_strtoul(s,ul,nl) )
			{
				union { uint32 ni; float nf; };
				ni = ul;
				lua_pushinteger( L, ++k );
				lua_pushnumber( L, nf );
				lua_settable( L, -3 );
				if( nl ) {
					w = Max(r,w);
					r = 0;
				} else {
					r++;
				}
			}

			if( w>1 )
			{
				// ( t -- mt t )
				lua_newtable( L );

				lua_pushinteger( L, w );
				lua_setfield( L, -2, "width" );

				// ( mt t -- t )
				lua_setmetatable( L, -2 );
			}

			return 1;
		}

		int Lua_isEmpty ( lua_State *L ) 
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			bool bRes = Strisempty(instr);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_isSpace ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			bool bRes = Strisspace(instr);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_isAlpha ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			bool bRes = Strisalpha(instr);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_isDigit2 ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			bool bRes = Strisdigit2(instr);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_isDigit10 ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			bool bRes = Strisdigit(instr);
			lua_pushboolean(L,bRes);
			return 1;
		}
		int Lua_isDigit16 ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			bool bRes = Strisdigit16(instr);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_isAlnum ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			bool bRes = Strisalnum(instr);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_isUpper ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			bool bRes = Strisupper(instr);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_isLower ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			bool bRes = Strislower(instr);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_isIdent ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			bool bRes = Strisident(instr);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_compare ( lua_State *L )
		{
			pcstr instr  = luaT_checklstring( L, 1, NULL );
			pcstr instr2 = luaT_checklstring( L, 2, NULL );
			bool cs = luaT_optboolean(L,3,true);
			int n = luaT_optinteger(L,4,-1);
			int iRes;
			if(n==-1)
				iRes = Strcompare(instr,instr2,cs);
			else
				iRes = Strcompare(instr,instr2,n,cs);
			lua_pushinteger(L,iRes);
			return 1;
		}

		int Lua_equals ( lua_State *L )
		{
			Lua_compare(L);
			int n = luaT_checkinteger(L,-1);
			bool bRes = (n==0);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_find ( lua_State *L )
		{
			pcstr instr  = luaT_checklstring( L, 1, NULL );
			pcstr instr2 = luaT_checklstring( L, 2, NULL );
			bool cs = luaT_optboolean(L,3,true);
			bool rev = luaT_optboolean(L,4,false);
			pcstr sRes = Strfind(instr,instr2,cs,rev);
			lua_pushstring(L,sRes);
			return 1;
		}

		int Lua_findAny ( lua_State *L )
		{
			pcstr instr  = luaT_checklstring( L, 1, NULL );
			pcstr instr2 = luaT_checklstring( L, 2, NULL );
			bool cs = luaT_optboolean(L,3,true);
			bool rev = luaT_optboolean(L,4,false);
			pcstr sRes = Strfindany(instr,instr2,cs,rev);
			lua_pushstring(L,sRes);
			return 1;
		}

		int Lua_startsWith ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			pcstr instr2 = luaT_checklstring( L, 2, NULL );
			bool cs = luaT_optboolean(L,3,true);
			bool bRes = Strstartswith(instr,instr2,cs);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_startsWithAny ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			pcstr instr2 = luaT_checklstring( L, 2, NULL );
			bool cs = luaT_optboolean(L,3,true);
			bool bRes = Strstartswithany(instr,instr2,cs);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_endsWith ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			pcstr instr2 = luaT_checklstring( L, 2, NULL );
			bool cs = luaT_optboolean(L,3,true);
			bool bRes = Strendswith(instr,instr2,cs);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_endsWithAny ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			pcstr instr2 = luaT_checklstring( L, 2, NULL );
			bool cs = luaT_optboolean(L,3,true);
			bool bRes = Strendswithany(instr,instr2,cs);
			lua_pushboolean(L,bRes);
			return 1;
		}

		int Lua_copy ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			int n = luaT_optinteger(L,2,-1);
			util::SafeBuffer<>	 buff;
			buff.zero();
			pstr sRes;
			if(n==-1)
				sRes = Strcopy(buff,instr);
			else
				sRes = Strcopy(buff,instr,n);
			lua_pushstring(L,sRes);
			return 1;
		}

		int Lua_indexOf ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			pcstr instr2 = luaT_checklstring( L, 2, NULL );
			bool cs = luaT_optboolean(L,3,true);
			bool rev = luaT_optboolean(L,4,false);
			int iRes = Strindexof(instr,instr2,cs,rev);
			lua_pushinteger(L,iRes);
			return 1;
		}

		int Lua_indexOfAny ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			pcstr instr2 = luaT_checklstring( L, 2, NULL );
			bool cs = luaT_optboolean(L,3,true);
			bool rev = luaT_optboolean(L,4,false);
			int iRes = Strindexofany(instr,instr2,cs,rev);
			lua_pushinteger(L,iRes);
			return 1;
		}

		int Lua_upper ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			util::SafeBuffer<> buff;
			buff.zero();
			pstr sRes = Strtoupper(buff,instr);
			lua_pushstring(L,sRes);
			return 1;
		}

		int Lua_lower ( lua_State *L )
		{
			pcstr instr = luaT_checklstring( L, 1, NULL );
			util::SafeBuffer<> buff;
			buff.zero();
			pstr sRes = Strtolower(buff,instr);
			lua_pushstring(L,sRes);
			return 1;
		}

		int Lua_split ( lua_State* L )
		{
			pcstr str = luaT_checklstring( L, 1, NULL );
			pcstr sep = luaT_optstring( L, 2, NULL);
			SU_REQUIRE_OR_0( str );
			if( !sep ) sep = " \t\n\r";

			lua_newtable( L );
			int t = lua_gettop( L );

			int cpt = 0;
			pcstr start = str;
			pcstr end   = str + util::Strlen(str);

			while( start<end )
			{
				pcstr any = util::Strfindany(start,sep,false);
				if( !any )
				{
					lua_pushlstring(L,start,end-start);
					lua_rawseti(L,t,++cpt);
					break;
				}
				else
				{
					if( any>start )
					{
						lua_pushlstring(L,start,any-start);
						lua_rawseti(L,t,++cpt);
					}
					while( util::Strstartswithany(any,sep,false) )
						any++;
					start = any;
				}
			}
			return 1;
		}

		int Lua_splitlines ( lua_State* L )
		{
			pcstr str = luaT_checklstring( L, 1, NULL );
			SU_REQUIRE_OR_0( str );

			lua_newtable( L );
			int t = lua_gettop( L );

			int cpt = 0;
			pcstr start = str;
			pcstr end   = str + util::Strlen(str);
			pcstr sep   = "\n\r";

			while( start<end )
			{
				pcstr nl = util::Strfindany(start,sep,false);
				if( !nl )
				{
					lua_pushlstring(L,start,end-start);
					lua_rawseti(L,t,++cpt);
					break;
				}
				else
				{
					lua_pushlstring(L,start,nl-start);
					lua_rawseti(L,t,++cpt);

					int nlc=0;
					while( nl<end )
					{
						if     ( *nl=='\n' )	{ nlc++; nl+=nl[1]=='\r'?2:1; }
						else if( *nl=='\r' )	{ nlc++; nl+=nl[1]=='\n'?2:1; }
						else					break;
						if( nlc>1 ) {
							lua_pushstring(L,"");
							lua_rawseti(L,t,++cpt);
						}
					}
					start = nl;
				}
			}
			return 1;
		}

		int Lua_join ( lua_State* L )
		{
			luaT_checktable( L, 1 );
			pcstr sep = luaT_optstring( L, 2, NULL);
			if( util::Strlen(sep)!=1 ) sep=" ";
			util::SafeBuffer<char,2048>	 buff;
			buff.zero();
			int cpt = 0;
			lua_pushnil(L);
			while( lua_next(L,1) )
			{
				// t key val
				pcstr str = lua_tostring(L,-1);
				if( str ) {
					if( cpt )
						util::Strconcat(buff,sep);
					util::Strconcat(buff,str);
					cpt++;
				}
				lua_pop(L,1);
			}
			lua_pushstring(L,buff);
			return 1;
		}

		static bool mid_clamp(int& start, int& end, int size, bool clamp)
		{
			if(start>size)
			{
				if(clamp)
					start = size;
				else
					return false;
			}
			else if(start<-size || start==0)
			{
				if(clamp)
					start = 1;
				else
					return false;
			}
			else if(start<0)
				start += size + 1;
			assert(start>0);
			assert(start<=size);
			start--;
			
			if(end>size)
			{
				if(clamp)
					end = size;
				else
					return false;
			}
			else if(end<-size || end==0)
			{
				if(clamp)
					end = 1;
				else
					return false;
			}
			else if(end<0)
				end += size + 1;
			assert(end>0);
			assert(end<=size);

			if(start>=end)
			{
				if(clamp)
					end = start+1;
				else
					return false;
			}
			return true;
		}

		int Lua_mid ( lua_State* L )
		{
			size_t size;
			pcstr str = luaT_checklstring( L, 1, &size );
			int s = luaT_checkinteger( L, 2);
			int e = luaT_checkinteger( L, 3);
			bool c = luaT_optboolean(L,4,true);
			if(!mid_clamp(s,e,size,c))
			{
				lua_pushstring(L,"");
				return 1;
			}
			SU_ASSERT(s>=0);
			SU_ASSERT(e>s);
			SU_ASSERT(size>=e);
			util::SafeBuffer<>	 buff;
			buff.zero();
			pcstr b = str+s;
			Strcopy(buff,b,e-s);
			lua_pushstring(L,buff);
			return 1;
		}

		int Lua_xmid ( lua_State* L )
		{
			size_t size;
			pcstr str = luaT_checklstring( L, 1, &size );
			int s = luaT_checkinteger( L, 2);
			int e = luaT_checkinteger( L, 3);
			bool c = luaT_optboolean(L,4,true);
			if(!mid_clamp(s,e,size,c))
			{
				lua_pushstring(L,str);
				return 1;
			}
			assert(s>=0);
			assert(e>s);
			assert(size>=e);
			util::SafeBuffer<>	 buff;
			buff.zero();
			Strcopy(buff,str,s);
			pcstr b = str+e;
			Strcopy(buff,b,size-e);
			lua_pushstring(L,buff);
			return 1;
		}

		int Lua_left ( lua_State* L )
		{
			pcstr s = luaT_checkstring( L, 1 );
			int i = luaT_checkinteger( L, 2 );
			PString r = PString(s).left(i);
			lua_pushstring( L, r );
			return 1;
		}

		int Lua_right ( lua_State* L )
		{
			pcstr s = luaT_checkstring( L, 1 );
			int i = luaT_checkinteger( L, 2 );
			PString r = PString(s).right(i);
			lua_pushstring( L, r );
			return 1;
		}

		int Lua_front ( lua_State* L )
		{
			pcstr s = luaT_checkstring( L, 1 );
			int i = luaT_checkinteger( L, 2 );
			PString r = PString(s).front(i);
			lua_pushstring( L, r );
			return 1;
		}

		int Lua_back ( lua_State* L )
		{
			pcstr s = luaT_checkstring( L, 1 );
			int i = luaT_checkinteger( L, 2 );
			PString r = PString(s).back(i);
			lua_pushstring( L, r );
			return 1;
		}

		int Lua_replace ( lua_State* L )
		{
			size_t size;
			pcstr strIn = luaT_checklstring( L, 1, &size);
			size_t sizeS;
			pcstr strS = luaT_checklstring( L, 2, &sizeS);
			size_t sizeD;
			pcstr strD = luaT_checklstring( L, 3, &sizeD);
			bool cs = luaT_optboolean(L,4,true);

			util::SafeBuffer<>	 buff;
			buff.zero();
			pstr cur = buff;
			while(size>0)
			{
				int index = util::Strindexof(strIn,strS,cs);
				if(index == -1)
				{
					Strcopy(cur,strIn,size);
					cur+=size;
					strIn+=size;
					size = 0;
				}
				else
				{
					Strcopy(cur,strIn,index);
					cur+=index;
					strIn+=index;
					size -= index;
					Strcopy(cur,strD,sizeD);
					cur  += sizeD;
					strIn+= sizeS;
					size -= sizeS;
				}
			}
			lua_pushstring(L,buff);
			return 1;
		}

		int Lua_reverse ( lua_State* L )
		{
			size_t size;
			pcstr str = luaT_checklstring( L, 1, &size);
			if(size == 0)
			{
				lua_pushstring(L,"");
				return 1;
			}
			pcstr curread = str+size-1;
			util::SafeBuffer<>	 buff;
			buff.zero();
			pstr curwrite = buff;
			for(;;)
			{
				*curwrite = *curread;
				curwrite++;
				if(curread == str)
					break;
				curread--;
			}
			lua_pushstring(L,buff);
			return 1;
		}

		int Lua_strip ( lua_State* L )
		{
			pcstr cs = luaT_checkstring(L,1);
			TString s(cs);
			s.strip();
			lua_pushstring( L, s );
			return 1;
		}

		int Lua_splitargs ( lua_State* L )
		{
			pcstr cs = luaT_checkstring(L,1);
			pcstr es = cs + util::Strlen(cs);

			lua_newtable(L);
			int t = lua_gettop(L);

			int state = 0;
			BString<64>   optname;
			BString<1024> optvalue;

			for( ;; )
			{
				switch( state )
				{
					case 0 :
						// look for /option
						if( cs==es ) {
							return 1;
						}
						if( *cs=='/' ) {
							optname.clear();
							optvalue.clear();
							state=1;
						}
						cs++;
						break;
					case 1 :
						// parsing option name
						if( util::Isalnum(*cs) || *cs=='-' || *cs=='_' ) {
							optname.append(*cs++);
						}
						else if( *cs==':' ) {
							cs++;
							state=3;
						}
						else {
							state=2;
						}
						break;
					case 2 :
						// store option flag (an option without value)
						if( optname.size() && !luaT_hasfield(L,t,optname) ) {
							lua_pushstring( L, optname.cstr() );
							lua_pushboolean( L, true );
							lua_settable( L, t );
						}
						state = 0;
						break;
					case 3 :
						// parsing option value
						if( *cs=='\"' ) {
							cs++;
							state=5;
						}
						else if( *cs=='\'' ) {
							cs++;
							state=6;
						}
						else if( cs<es && !util::Isspace(*cs) && *cs!='/' ) {
							optvalue.append(*cs++);
						}
						else {
							state = 4;
						}
						break;
					case 4 :
						// store full option with value
						if( optname.size() && optvalue.size() ) {
							lua_pushstring(L,optname);
							lua_gettable(L,-2);
							if( lua_isnil(L,-1) ) {
								long l;
								optvalue.strip();
								if( optvalue.tonumber(l,10,0)==optvalue.size() )
									lua_pushinteger(L,l);
								else
									lua_pushstring(L,optvalue);
								lua_setfield(L,t,optname);
							} else if( lua_istable(L,-1) ) {
								int n = lua_objlen(L,-1);
								lua_pushstring(L,optvalue);
								lua_rawseti(L,-2,n+1);
							} else {
								pcstr s = lua_tostring(L,-1);
								lua_newtable(L);
								lua_pushvalue(L,-2);
								lua_rawseti(L,-2,1);
								lua_pushstring(L,optvalue);
								lua_rawseti(L,-2,2);
								lua_setfield(L,t,optname);
							}
							lua_settop(L,t);
						}
						state = 0;
						break;
					case 5 :
						// parsing double-quoted "..."
						if( cs==es ) {
							state=3;
						}
						else if( *cs=='\"' ) {
							cs++;
							state=3;
						}
						else {
							optvalue.append(*cs++);
						}
						break;
					case 6 :
						// parsing single-quoted '...'
						if( cs==es ) {
							state=3;
						}
						else if( *cs=='\'' ) {
							cs++;
							state=3;
						}
						else {
							optvalue.append(*cs++);
						}
						break;
				}
			}
		}
	};

}




