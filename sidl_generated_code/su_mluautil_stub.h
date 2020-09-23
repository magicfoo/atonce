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


#include <sufw/su_mapping.h>
#include <sufw/asset/su_asset.h>
#include <sufw/lang/lua/su_lua.h>
#include <sufw-build/zip/unzip.h>
using namespace su;


#include "su_mluautil_typeid.h"




namespace
{

	struct SIDL_STUB_CLASSNAME
	{

		Pool*		pool;
		ISwitch*	sw;


		bool Init ( Pool* p, AnyList* o )
		{
			pool = p;
			sw = SIDL_IMPL_INTERFACE( ISwitch );

			if( o && o->isType("luastate",Any::Addr) )
			{
				lua_State* l = (lua_State*)o->asAddr("luastate");
				pcstr swfilter = o->asString("swfilter");
				luaT_swpushobject( l, sw, swfilter );

				// register SID values
				lua_pushstring(l,"SID");
				lua_newtable(l);
				pcstr s;
				for( uint i=0 ; i < sid::Count() ; i++ ) {
					if( s=sid::Str(i) ) {
						lua_pushfstring(l,s);
						lua_pushinteger(l,i);
						lua_rawset(l,-3);
					}
				}
				luaT_swsetobject(l,-3);
				SU_ASSERT( lua_isuserdata(l,-1) );
			}

			return true;
		}


		void Shut ( )
		{
			//
		}


		int Lua_selectMap ( lua_State* L )
		{
			luaT_checktable( L, 1 );

			AnyArray<1> options;
			options.set( 0, "luastate", L );
			IDatatree* ldtt = factory::CreateObject<IDatatree>( uid::MLuaDatatree, pool, pool, &options );
			SU_REQUIRE_OR_0( ldtt );

			StringId mapid = map::select( ldtt );
			lua_pushinteger( L, mapid );
			lua_pushstring( L, sid::Str(mapid) );

			ldtt->Release();
			return 2;
		}


		int Lua_binarizeMap ( lua_State* L )
		{
			luaT_checktable( L, 1 );

			Pool* binpool = globals::authpool;
			SU_REQUIRE_OR_0( binpool );

			AnyArray<1> options;
			options.set( 0, "luastate", L );
			IDatatree* ldtt = factory::CreateObject<IDatatree>( uid::MLuaDatatree, pool, pool, &options );
			SU_REQUIRE_OR_0( ldtt );

			byte* baddr;
			uint  bs;
			map::context ctxt;
			bs = map::binarize( ldtt, binpool, baddr, ctxt );

			ldtt->Release();

			if( bs )
			{
				void* ud = lua_newuserdata( L, bs );
				if( ud )
				{
					util::Memcopy( ud, baddr, bs );
					binpool->Free( baddr );
					// success !
					return 1;
				}
				binpool->Free( baddr );
			}

			// failed !

			// result 1: no data !
			lua_pushnil( L );

			// result 2: the context
			lua_newtable( L );
			lua_pushinteger( L, ctxt.errcode );
			lua_setfield( L, -2, "errcode" );
			lua_pushstring( L, ctxt.errmsg );
			lua_setfield( L, -2, "errmsg" );
			lua_pushstring( L, ctxt.mapname );
			lua_setfield( L, -2, "mapname" );
			lua_pushstring( L, ctxt.tagname );
			lua_setfield( L, -2, "tagname" );
			lua_pushstring( L, ctxt.recname );
			lua_setfield( L, -2, "recname" );
			lua_pushstring( L, ctxt.varname );
			lua_setfield( L, -2, "varname" );
			return 2;
		}


		int Lua_hash32 ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );

			byte* h = NULL;

			if( lua_isstring(L,1) )
				h = util::hmac_crc32( (void*)luaT_getstring(L,1), lua_objlen(L,1) );

			else if( lua_isuserdata(L,1) )
				h = util::hmac_crc32( lua_touserdata(L,1), lua_objlen(L,1) );

			if( !h )
				return 0;

			lua_pushstring( L, util::hmac_string(h,4) );
			return 1;
		}


		int Lua_hash128 ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );

			byte* h = NULL;

			if( lua_isstring(L,1) )
				h = util::hmac_md5( (void*)luaT_getstring(L,1), lua_objlen(L,1) );

			else if( lua_isuserdata(L,1) )
				h = util::hmac_md5( lua_touserdata(L,1), lua_objlen(L,1) );

			if( !h )
				return 0;

			lua_pushstring( L, util::hmac_string(h,16) );
			return 1;
		}


		int Lua_setConsoleFile ( lua_State* L )
		{
			Pool* p = globals::syspool;
			pstr logfile = (pstr)luaT_checkstring(L,1);
			globals::consolefilename = logfile;
			classid consclassid = globals::cout ? globals::cout->GetBuilder()->GetMUID() : 0;
			if( consclassid != uid::MFileConsole )
			{
				SafeRelease( globals::cout );
				SafeRelease( globals::cerr );
				SafeRelease( globals::cdbg );

				AnyArray<1> opt;
				opt.set( 0, "label", "stdout" );
				globals::cout = factory::CreateObject<IConsole>( uid::MFileConsole, p, p, &opt );

				opt.set( 0, "label", "stderr" );
				globals::cerr = factory::CreateObject<IConsole>( uid::MFileConsole, p, p, &opt );

				#if (SU_BUILD_CONSOLE)
				opt.set( 0, "label", "stddbg" );
				globals::cdbg = factory::CreateObject<IConsole>( uid::MFileConsole, p, p, &opt );
				#endif
			}
			return 0;
		}


		int Lua_getTime ( lua_State* L )
		{
			Time t;
			clock::GetTime(&t);
			double td = double(t.ipart) + double(t.fpart);
			lua_pushnumber(L,td);
			return 1;
		}


		int Lua_getDate ( lua_State* L )
		{
			Date d;
			clock::GetDate(&d);
			lua_newtable(L);
			lua_pushinteger(L,d.dsec);		lua_setfield(L,-2,"dsec");
			lua_pushinteger(L,d.dmin);		lua_setfield(L,-2,"dmin");
			lua_pushinteger(L,d.dhour);		lua_setfield(L,-2,"dhour");
			lua_pushinteger(L,d.mday);		lua_setfield(L,-2,"mday");
			lua_pushinteger(L,d.wday);		lua_setfield(L,-2,"wday");
			lua_pushinteger(L,d.mon);		lua_setfield(L,-2,"mon");
			lua_pushinteger(L,d.year);		lua_setfield(L,-2,"year");
			return 1;
		}


		int Lua_getNow ( lua_State* L )
		{
			Date d;
			Time t;
			clock::GetNow(&d,&t);
			lua_newtable(L);
			lua_pushnumber(L,t.ipart);		lua_setfield(L,-2,"ti");
			lua_pushinteger(L,t.fpart);		lua_setfield(L,-2,"tf");
			lua_pushinteger(L,d.dsec);		lua_setfield(L,-2,"dsec");
			lua_pushinteger(L,d.dmin);		lua_setfield(L,-2,"dmin");
			lua_pushinteger(L,d.dhour);		lua_setfield(L,-2,"dhour");
			lua_pushinteger(L,d.mday);		lua_setfield(L,-2,"mday");
			lua_pushinteger(L,d.wday);		lua_setfield(L,-2,"wday");
			lua_pushinteger(L,d.mon);		lua_setfield(L,-2,"mon");
			lua_pushinteger(L,d.year);		lua_setfield(L,-2,"year");
			return 1;
		}

			
		int Lua_sid ( lua_State* L )
		{
			SU_REQUIRE_OR_0( lua_gettop(L)>0 );
			if( lua_isnumber(L,1) )
			{
				lua_pushstring( L, sid::Str(lua_tointeger(L,1)) );
				return 1;
			}
			else if( lua_isstring(L,1) )
			{
				lua_pushinteger( L, sid::Find(luaT_getstring(L,1)) );
				return 1;
			}
			else
			{
				return 0;
			}
		}



		//
		// PAK support

		
		void push_pack_header ( lua_State* L, const IPakDesigner::PakHeader& hd )
		{
			lua_newtable( L );
			lua_pushinteger( L, hd.magicid );
			lua_setfield( L, -2, "magicid" );
			lua_pushinteger( L, hd.format );
			lua_setfield( L, -2, "format" );
			lua_pushstring( L, hd.label );
			lua_setfield( L, -2, "label" );
			lua_pushstring( L, hd.comment );
			lua_setfield( L, -2, "comment" );
			lua_pushinteger( L, hd.revision );
			lua_setfield( L, -2, "revision" );
			lua_pushinteger( L, hd.fullbs );
			lua_setfield( L, -2, "fullbs" );
			lua_pushinteger( L, hd.tocso );
			lua_setfield( L, -2, "tocso" );
			lua_pushinteger( L, hd.toceo );
			lua_setfield( L, -2, "toceo" );
			lua_pushinteger( L, hd.lorid );
			lua_setfield( L, -2, "lorid" );
			lua_pushinteger( L, hd.hirid );
			lua_setfield( L, -2, "hirid" );
		}


		void push_pak_desc ( lua_State* L, const IPakDesigner::PakDesc& desc )
		{
			lua_newtable( L );
			lua_pushinteger( L, desc.rid );
			lua_setfield( L, -2, "rid" );
			lua_pushinteger( L, desc.tid );
			lua_setfield( L, -2, "tid" );
			lua_pushinteger( L, desc.dataso );
			lua_setfield( L, -2, "dataso" );
			lua_pushinteger( L, desc.dataeo );
			lua_setfield( L, -2, "dataeo" );
			lua_pushinteger( L, desc.contentbs );
			lua_setfield( L, -2, "contentbs" );

			StringId mapid = map::revert( desc.tid );
			if( mapid ) {
				lua_pushstring( L, sid::Str(mapid) );
				lua_setfield( L, -2, "mapid" );
			}
		}


		int Lua_listMaps ( lua_State* L )
		{
			pcstr infile = luaT_checkstring( L, 1 );

			SU_REQUIRE_OR_ZERO( globals::fs );
			SU_REQUIRE_OR_ZERO( globals::fs->IsFile(infile) );

			IPakDesigner* pk = factory::CreateObject<IPakDesigner>( uid::MPakDesigner, pool );
			SU_REQUIRE_OR_ZERO( pk );

			int res = 0;

			if( pk->Setup(infile) )
			{
				IPakDesigner::PakHeader hd;
				hd = pk->GetHeader();
				if( hd.fullbs )
				{
					lua_newtable( L );
					uint n = pk->CountDesc();
					for( uint i=0 ; i<n ; i++ )
					{
						IPakDesigner::PakDesc desc;
						desc = pk->EnumDesc(i);

						push_pak_desc(L,desc);
						lua_rawseti(L,-2,i+1);
					}
					res = 1;
				}
			}

			pk->Release();
			return res;
		}


		int Lua_listTags ( lua_State* L )
		{
			pcstr infile = luaT_checkstring( L, 1 );
			int rid = luaT_checkinteger( L, 2 );

			SU_REQUIRE_OR_ZERO( globals::authpool );
			SU_REQUIRE_OR_ZERO( globals::fs );
			SU_REQUIRE_OR_ZERO( globals::fs->IsFile(infile) );

			IPakDesigner* pk = factory::CreateObject<IPakDesigner>( uid::MPakDesigner, pool );
			SU_REQUIRE_OR_ZERO( pk );

			int res = 0;

			if( pk->Setup(infile) )
			{
				IPakDesigner::PakHeader hd;
				hd = pk->GetHeader();
				if( hd.fullbs )
				{
					IPakDesigner::PakDesc desc;
					desc = pk->GetDesc(rid);
					if( desc.rid==rid )
					{
						StringId mid;
						mid = map::revert( desc.tid );
						if( mid )
						{
							uint mcsign, mfsign, malign;
							if( map::info(mid,mcsign,mfsign,malign) )
							{
								byte* content = (byte*) globals::authpool->Alloc( desc.contentbs );
								if( content )
								{
									if( pk->ExtractContent(rid,content) )
									{
										map::base_map m;
										m.load( content, malign );

										if( m.tag_first() )
										{
											lua_newtable(L);
											for( uint k=1 ; m.tag_next() ; k++ )
											{
												uint tbo   = m.tag_addr() - m.data_begin();
												uint tbs   = m.tag_bsize();
												uint tcode = m.tag_code();

												StringId tid   = map::revert(mid,tcode);
												pcstr	 tname = sid::Str(tid);

												lua_newtable(L);
												lua_pushinteger(L,tbs);
												lua_setfield(L,-2,"bsize");
												lua_pushinteger(L,tbo);
												lua_setfield(L,-2,"boffset");
												lua_pushinteger(L,tcode);
												lua_setfield(L,-2,"code");
												lua_pushstring(L,tname);
												lua_setfield(L,-2,"tagid");
												lua_rawseti(L,-2,k);
											}
											res = 1;
										}
									}
									globals::authpool->Free( content );
								}
							}
						}
					}
				}
			}

			pk->Release();
			return res;
		}


		int Lua_openPak ( lua_State* L )
		{
			pcstr infile = luaT_checkstring( L, 1 );
			bool inforcecreate = luaT_optboolean( L, 2, false );
			SU_REQUIRE_OR_ZERO( infile );

			IPakDesigner* pk = factory::CreateObject<IPakDesigner>( uid::MPakDesigner, pool );
			SU_REQUIRE_OR_ZERO( pk );

			if( !pk->Setup(infile,inforcecreate) )
			{
				pk->Release();
				return 0;
			}

			luaT_swpushdata( L, SWDATA_PAK_TYPEID, &pk, sizeof(pk), sw, "LuaPak_*" );
			return 1;
		}

	
		int LuaPak_getHeader ( lua_State* L )
		{
			luaT_checkudata( L, 1 );

			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			IPakDesigner::PakHeader hd;
			hd = (*pk)->GetHeader();

			push_pack_header( L, hd );
			return 1;
		}


		int LuaPak_getDesc ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			int rid = luaT_checkinteger( L, 2 );

			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			IPakDesigner::PakDesc desc;
			desc = (*pk)->GetDesc( rid );

			push_pak_desc( L, desc );
			return 1;
		}


		int LuaPak_countDesc ( lua_State* L )
		{
			luaT_checkudata( L, 1 );

			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			lua_pushinteger( L, (*pk)->CountDesc() );
			return 1;
		}


		int LuaPak_enumDesc ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			int ri = luaT_checkinteger( L, 2 );

			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			IPakDesigner::PakDesc desc;
			desc = (*pk)->EnumDesc( ri );

			push_pak_desc( L, desc );
			return 1;
		}


		int LuaPak_extractBytes ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			int sof = luaT_checkinteger( L, 2 );
			int eof = luaT_checkinteger( L, 3 );
			SU_REQUIRE_OR_FALSE( eof > sof );

			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			uint bs = eof - sof;
			void* ud = lua_newuserdata( L, bs );
			if( ud )
			{
				if( (*pk)->ExtractBytes(sof,eof,ud) )
					return 1;
			}

			return 0;
		}


		int LuaPak_extractRaw ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			int rid = luaT_checkinteger( L, 2 );

			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			IPakDesigner::PakDesc desc;
			desc = (*pk)->GetDesc( rid );

			if( desc.rid == rid )
			{
				uint bs = desc.dataeo - desc.dataso;
				void* ud = lua_newuserdata( L, bs );
				if( ud )
				{
					if( (*pk)->ExtractRaw(rid,ud) )
						return 1;
				}
			}

			return 0;
		}


		int LuaPak_extractContent ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			int rid = luaT_checkinteger( L, 2 );

			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			IPakDesigner::PakDesc desc;
			desc = (*pk)->GetDesc( rid );

			if( desc.rid == rid )
			{
				void* ud = lua_newuserdata( L, desc.contentbs );
				if( ud )
				{
					if( (*pk)->ExtractContent(rid,ud) )
						return 1;
				}
			}

			return 0;
		}


		int LuaPak_startUpdating ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			(*pk)->StartUpdating();
			return 0;
		}


		int LuaPak_updateLabel ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			pcstr lb = luaT_checkstring( L, 2 );

			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			lua_pushboolean( L, (*pk)->UpdateLabel(lb) );
			return 1;
		}


		int LuaPak_updateComment ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			pcstr lb = luaT_checkstring( L, 2 );

			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			lua_pushboolean( L, (*pk)->UpdateComment(lb) );
			return 1;
		}


		int LuaPak_updateRevision ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			uint rev = luaT_checkinteger( L, 2 );

			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			lua_pushboolean( L, (*pk)->UpdateRevision(rev) );
			return 1;
		}


		int LuaPak_updateContent ( lua_State* L )
		{
			luaT_checkudata(L,1);
			uint rid = luaT_checkinteger(L,2);
			uint tid = luaT_checkinteger(L,3);
			luaT_checkudata(L,4);
			bool pkb = luaT_optboolean(L,5,true);

			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			bool b = (*pk)->UpdateContent( rid, tid, luaT_vdata(L,4), luaT_vlen(L,4), pkb );

			lua_pushboolean( L, b );
			return 1;
		}


		int LuaPak_endUpdating ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			(*pk)->EndUpdating();
			return 0;
		}


		int LuaPak_optimize ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			lua_pushboolean( L, (*pk)->Optimize() );
			return 1;
		}


		int LuaPak_gc ( lua_State* L )
		{
			luaT_checkudata( L, 1 );
			IPakDesigner** pk = luaT_swtodata<IPakDesigner*>( L, 1, SWDATA_PAK_TYPEID );
			SU_ASSERT( pk );

			(*pk)->Release();
			return 0;
		}




		//
		// ZIP


		int Lua_openZip ( lua_State* L )
		{
			pcstr vpath = luaT_checkstring(L,1);

			SU_REQUIRE_OR_0( globals::fs );
			PString hpath = globals::fs->GetHostPath( vpath );

			unzFile uf = unzOpen( hpath );
			SU_REQUIRE_OR_0( uf );
		
			luaT_swpushdata( L, SWDATA_ZIP_TYPEID, &uf, sizeof(uf), sw, "LuaZip_*" );
			return 1;
		}


		int LuaZip_len ( lua_State* L )
		{
			luaT_checkudata(L,1);
			unzFile* ppk = luaT_swtodata<unzFile>( L, 1, SWDATA_ZIP_TYPEID );
			SU_ASSERT( ppk && *ppk );

			unz_global_info ginfo;
			int res = unzGetGlobalInfo( *ppk, &ginfo );
			SU_REQUIRE_OR_0( res == UNZ_OK );

			lua_pushinteger( L, ginfo.number_entry );
			return 1;
		}

		
		int LuaZip_comment ( lua_State* L )
		{
			luaT_checkudata(L,1);
			unzFile* ppk = luaT_swtodata<unzFile>( L, 1, SWDATA_ZIP_TYPEID );
			SU_ASSERT( ppk && *ppk );

			char buff[1024];
			int res = unzGetGlobalComment( *ppk, buff, sizeof(buff) );
			SU_REQUIRE_OR_0( res > 0 );

			lua_pushlstring( L, buff, res );
			return 1;
		}


		int LuaZip_tell ( lua_State* L )
		{
			luaT_checkudata(L,1);
			unzFile* ppk = luaT_swtodata<unzFile>( L, 1, SWDATA_ZIP_TYPEID );
			SU_ASSERT( ppk && *ppk );

			lua_pushinteger( L, unztell(*ppk) );
			return 1;
		}


		int LuaZip_eof ( lua_State* L )
		{
			luaT_checkudata(L,1);
			unzFile* ppk = luaT_swtodata<unzFile>( L, 1, SWDATA_ZIP_TYPEID );
			SU_ASSERT( ppk && *ppk );

			lua_pushboolean( L, unzeof(*ppk)==1 );
			return 1;
		}


		int LuaZip_firstFile ( lua_State* L )
		{
			luaT_checkudata(L,1);
			unzFile* ppk = luaT_swtodata<unzFile>( L, 1, SWDATA_ZIP_TYPEID );
			SU_ASSERT( ppk && *ppk );

			int res = unzGoToFirstFile( *ppk );
			SU_REQUIRE_OR_0( res == UNZ_OK );

			lua_pushboolean(L,true);
			return 1;
		}


		int LuaZip_nextFile ( lua_State* L )
		{
			luaT_checkudata(L,1);
			unzFile* ppk = luaT_swtodata<unzFile>( L, 1, SWDATA_ZIP_TYPEID );
			SU_ASSERT( ppk && *ppk );

			int res = unzGoToNextFile( *ppk );
			SU_REQUIRE_OR_0( res==UNZ_OK );

			lua_pushboolean(L,true);
			return 1;
		}


		int LuaZip_seekFile ( lua_State* L )
		{
			luaT_checkudata(L,1);
			pcstr filename = luaT_checkstring(L,2);

			unzFile* ppk = luaT_swtodata<unzFile>( L, 1, SWDATA_ZIP_TYPEID );
			SU_ASSERT( ppk && *ppk );

			int res = unzLocateFile( *ppk, filename, 0 );
			SU_REQUIRE_OR_0( res==UNZ_OK || res==UNZ_END_OF_LIST_OF_FILE );

			lua_pushboolean(L,true);
			return 1;
		}


		int LuaZip_aboutCurrent ( lua_State* L )
		{
			luaT_checkudata(L,1);
			unzFile* ppk = luaT_swtodata<unzFile>( L, 1, SWDATA_ZIP_TYPEID );
			SU_ASSERT( ppk && *ppk );

			unz_file_info finfo;
			char          fname[1024];
			char          fcomment[1024];

			int res = unzGetCurrentFileInfo( *ppk, &finfo, fname, sizeof(fname), NULL, 0, fcomment, sizeof(fcomment) );
			SU_REQUIRE_OR_0( res==UNZ_OK );

			lua_newtable(L);
			lua_pushstring(L,fname);
			lua_setfield(L,-2,"filename");
			lua_pushstring(L,fcomment);
			lua_setfield(L,-2,"comment");
			lua_pushinteger(L,finfo.compressed_size);
			lua_setfield(L,-2,"compressed_size");
			lua_pushinteger(L,finfo.uncompressed_size);
			lua_setfield(L,-2,"uncompressed_size");
			lua_pushinteger(L,finfo.compression_method);
			lua_setfield(L,-2,"method");
			lua_pushinteger(L,finfo.tmu_date.tm_sec);
			lua_setfield(L,-2,"date_sec");
			lua_pushinteger(L,finfo.tmu_date.tm_min);
			lua_setfield(L,-2,"date_min");
			lua_pushinteger(L,finfo.tmu_date.tm_hour);
			lua_setfield(L,-2,"date_hour");
			lua_pushinteger(L,finfo.tmu_date.tm_mday);
			lua_setfield(L,-2,"date_day");
			lua_pushinteger(L,finfo.tmu_date.tm_mon);
			lua_setfield(L,-2,"date_mon");
			lua_pushinteger(L,finfo.tmu_date.tm_year);
			lua_setfield(L,-2,"date_year");
			return 1;
		}


		int LuaZip_openCurrent ( lua_State* L )
		{
			luaT_checkudata(L,1);
			unzFile* ppk = luaT_swtodata<unzFile>( L, 1, SWDATA_ZIP_TYPEID );
			SU_ASSERT( ppk && *ppk );

			int res = unzOpenCurrentFile( *ppk );
			SU_REQUIRE_OR_0( res==UNZ_OK );

			lua_pushboolean(L,true);
			return 1;
		}


		int LuaZip_closeCurrent ( lua_State* L )
		{
			luaT_checkudata(L,1);
			unzFile* ppk = luaT_swtodata<unzFile>( L, 1, SWDATA_ZIP_TYPEID );
			SU_ASSERT( ppk && *ppk );

			int res = unzCloseCurrentFile( *ppk );
			SU_REQUIRE_OR_0( res==UNZ_OK );

			lua_pushboolean(L,true);
			return 1;
		}


		int LuaZip_readCurrent ( lua_State* L )
		{
			luaT_checkudata(L,1);
			int len = luaT_optinteger(L,2,0);

			unzFile* ppk = luaT_swtodata<unzFile>( L, 1, SWDATA_ZIP_TYPEID );
			SU_ASSERT( ppk && *ppk );

			int res;

			if( len==0 )
			{
				unz_file_info finfo;
				res = unzGetCurrentFileInfo( *ppk, &finfo, NULL, 0, NULL, 0, NULL, 0 );
				SU_REQUIRE_OR_0( res==UNZ_OK );
				len = finfo.uncompressed_size;
			}

			void* ud = lua_newuserdata( L, len );
			SU_REQUIRE_OR_0( ud );

			res = unzReadCurrentFile( *ppk, ud, len );
			SU_REQUIRE_OR_0( res==len );

			return 1;
		}


		int LuaZip_gc ( lua_State* L )
		{
			luaT_checkudata(L,1);
			unzFile* ppk = luaT_swtodata<unzFile>( L, 1, SWDATA_ZIP_TYPEID );
			SU_ASSERT( ppk && *ppk );

			unzClose( *ppk );
			return 0;
		}



		//
		// VECTOR


		int Lua_newVector ( lua_State* L )
		{
			if( lua_isuserdata(L,1) )
			{
				int   l = luaT_vlen(L,1);
				void* s = luaT_vdata(L,1);
				void* d = luaT_newvector(L,l);
				util::Memcopy(d,s,l);
				return 1;
			}
			else
			{
				int s = luaT_checkinteger(L,1);
				int c = luaT_optinteger(L,2,1);
				luaT_newvector(L,s,c);
				return 1;
			}
		}


		int Lua_vcpy ( lua_State* L )
		{
			luaT_checkudata(L,1);
			luaT_checkudata(L,2);
			luaT_vcpy( L, 1, 2 );
			return 0;
		}


		int Lua_veq ( lua_State* L )
		{
			luaT_checkudata(L,1);
			luaT_checkudata(L,2);
			lua_pushboolean( L, luaT_veq(L,1,2) );
			return 1;
		}

		
		int Lua_vlen ( lua_State* L )
		{
			luaT_checkudata(L,1);
			lua_pushinteger( L, luaT_vlen(L,1) );
			return 1;
		}


		int Lua_vstr ( lua_State* L )
		{
			luaT_checkudata(L,1);
			luaT_pushvstr(L,1);
			return 1;
		}


		int Lua_vlb ( lua_State* L )
		{
			luaT_checkudata(L,1);
			int n = lua_gettop(L)-1;
			return luaT_vlb(L,1,n);
		}


		int Lua_vlh ( lua_State* L )
		{
			luaT_checkudata(L,1);
			int n = lua_gettop(L)-1;
			return luaT_vlh(L,1,n);
		}


		int Lua_vlw ( lua_State* L )
		{
			luaT_checkudata(L,1);
			int n = lua_gettop(L)-1;
			return luaT_vlw(L,1,n);
		}


		int Lua_vld ( lua_State* L )
		{
			luaT_checkudata(L,1);
			int n = lua_gettop(L)-1;
			return luaT_vld(L,1,n);
		}


		int Lua_vlq ( lua_State* L )
		{
			luaT_checkudata(L,1);
			int n = lua_gettop(L)-1;
			return luaT_vlq(L,1,n);
		}


		int Lua_vlf ( lua_State* L )
		{
			luaT_checkudata(L,1);
			int n = lua_gettop(L)-1;
			return luaT_vlf(L,1,n);
		}


		int Lua_vsb ( lua_State* L )
		{
			luaT_checkudata(L,1);
			return luaT_vsb(L,1);
		}


		int Lua_vsh ( lua_State* L )
		{
			luaT_checkudata(L,1);
			return luaT_vsh(L,1);
		}


		int Lua_vsw ( lua_State* L )
		{
			luaT_checkudata(L,1);
			return luaT_vsw(L,1);
		}


		int Lua_vsd ( lua_State* L )
		{
			luaT_checkudata(L,1);
			return luaT_vsd(L,1);
		}


		int Lua_vsq ( lua_State* L )
		{
			luaT_checkudata(L,1);
			return luaT_vsq(L,1);
		}


		int Lua_vsf ( lua_State* L )
		{
			luaT_checkudata(L,1);
			return luaT_vsf(L,1);
		}



		int Lua_newBlob ( lua_State* L )
		{
			uint bsize = luaT_checkinteger(L,1);
			SU_REQUIRE_OR_ZERO( bsize );
			lua_newuserdata( L, bsize );
			return 1;
		}

		
		int Lua_newArray ( lua_State* L )
		{
			uint size = luaT_checkinteger(L,1);
			SU_REQUIRE_OR_ZERO( size );
			lua_createtable( L, size, 0 );
			return 1;
		}

		

		//
		// Cursor support

		int Lua_newCursor ( lua_State* L )
		{
			luaT_checkudata(L,1);
			void* blob = lua_touserdata(L,1);
			SU_REQUIRE_OR_ZERO( blob );

			void* ud = luaT_swpushdata( L, SWDATA_CURS_TYPEID, NULL, sizeof(MCursor), sw, "LuaCurs_*" );
			SU_REQUIRE_OR_ZERO( ud );
			luaT_swdatactor<MCursor>(L,-1,SWDATA_CURS_TYPEID);

			MCursor* cur = (MCursor*) ud;
			cur->setup( blob, lua_objlen(L,1) );
			return 1;
		}


	#define	SAFE_CURSOR( L, c ) \
				MCursor* c; \
				{ \
					luaT_checkudata( L, 1 ); \
					c = luaT_swtodata<MCursor>( L, 1, SWDATA_CURS_TYPEID );	\
					SU_REQUIRE_OR_ZERO( c ); \
				}

		int LuaCurs_size( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			lua_pushinteger( L, cs->size() );
			return 1;
		}

		int LuaCurs_pos( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			lua_pushinteger( L, cs->pos() );
			return 1;
		}

		int LuaCurs_seek( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int inbits = luaT_checkinteger( L, 2 );
			lua_pushinteger( L, cs->seek( inbits ) );
			return 1;
		}

		int LuaCurs_backward( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			lua_pushinteger( L, cs->backward() );
			return 1;
		}

		int LuaCurs_hasbackward( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int inbits = luaT_checkinteger( L, 2 );
			lua_pushboolean( L, cs->hasbackward( inbits ) );
			return 1;
		}

		int LuaCurs_gobackward( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int inbits = luaT_checkinteger( L, 2 );
			cs->gobackward( inbits );
			return 0;
		}

		int LuaCurs_forward( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			lua_pushinteger( L, cs->forward() );
			return 1;
		}

		int LuaCurs_hasforward( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int inbits = luaT_checkinteger( L, 2 );
			lua_pushboolean( L, cs->hasforward( inbits ) );
			return 1;
		}

		int LuaCurs_goforward( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int inbits = luaT_checkinteger( L, 2 );
			cs->goforward( inbits );
			return 0;
		}

		int LuaCurs_align( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int inbits = luaT_checkinteger( L, 2 );
			cs->align( inbits );
			return 0;
		}

		int LuaCurs_uread( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int inbits = luaT_checkinteger( L, 2 );
			lua_pushinteger( L, cs->uread( inbits ) );
			return 1;
		}

		int LuaCurs_sread( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int inbits = luaT_checkinteger( L, 2 );
			lua_pushinteger( L, cs->sread( inbits ) );
			return 1;
		}

		int LuaCurs_sizeB( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			lua_pushinteger( L, cs->sizeB() );
			return 1;
		}

		int LuaCurs_seekB( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int inPos = luaT_checkinteger( L, 2 );
			byte* start = cs->addrB();
			byte* bseek = start + inPos;
			cs->seekB( bseek );
			return 0;
		}

		int LuaCurs_usedB( lua_State* L )
		{
			SAFE_CURSOR( L, cs );

			if( lua_isnoneornil( L, 2 ) )
			{
				lua_pushinteger( L, cs->usedB() );
			}
			else
			{
				int inBCount = luaT_checkinteger( L, 2 );
				lua_pushboolean( L, cs->usedB( inBCount ) );
			}
			return 1;
		}

		int LuaCurs_leftB( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			if( lua_isnoneornil( L, 2 ) )
			{
				lua_pushinteger( L, cs->leftB() );
			}
			else
			{
				int inBCount = luaT_checkinteger( L, 2 );
				lua_pushboolean( L, cs->leftB( inBCount ) );
			}
			return 1;
		}

		int LuaCurs_findB( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int val = luaT_checkinteger( L, 2 );
			uint64 of;
			int pos = cs->findB( of, val ) ? int(of) : -1;
			lua_pushinteger( L, pos );
			return 0;
		}

		int LuaCurs_writeu8( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int n = lua_gettop(L);
			for( int i=2 ; i<=n ; i++ ) {
				uint val = (uint)luaT_checkinteger( L, i );
				cs->u8 << val;
			}
			return 0;
		}

		int LuaCurs_writes8( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int n = lua_gettop(L);
			for( int i=2 ; i<=n ; i++ ) {
				int val = luaT_checkinteger( L, i );
				cs->s8 << val;
			}
			return 0;
		}

		int LuaCurs_writeu16( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int n = lua_gettop(L);
			for( int i=2 ; i<=n ; i++ ) {
				uint val = (uint)luaT_checkinteger( L, i );
				cs->u16 << val;
			}
			return 0;
		}

		int LuaCurs_writes16( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int n = lua_gettop(L);
			for( int i=2 ; i<=n ; i++ ) {
				int val = luaT_checkinteger( L, i );
				cs->s16 << val;
			}
			return 0;
		}

		int LuaCurs_writeu32( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int n = lua_gettop(L);
			for( int i=2 ; i<=n ; i++ ) {
				uint val = (uint)luaT_checkinteger( L, i );
				cs->u32 << val;
			}
			return 0;
		}

		int LuaCurs_writes32( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int n = lua_gettop(L);
			for( int i=2 ; i<=n ; i++ ) {
				int val = luaT_checkinteger( L, i );
				cs->s32 << val;
			}
			return 0;
		}

		int LuaCurs_writef32( lua_State* L )
		{
			SAFE_CURSOR( L, cs );
			int n = lua_gettop(L);
			for( int i=2 ; i<=n ; i++ ) {
				float val = (float)luaT_checknumber( L, i );
				cs->f32 << val;
			}
			return 0;
		}

		int LuaCurs_gc ( lua_State* L )
		{
			luaT_checkudata(L,1);
			luaT_swdatadtor<MCursor>(L,1,SWDATA_CURS_TYPEID);
			return 0;
		}
	};


}


