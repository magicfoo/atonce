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


#include <sufw/asset/su_asset.h>
#include <sufw/lang/lua/su_lua.h>
#include <sufw/lang/lua/su_iluadatatree.h>
using namespace su;




namespace
{

	struct SIDL_STUB_CLASSNAME
	{

		Pool*	pool;


		bool Init ( Pool* p, AnyList* o )
		{
			pool = p;
			return true;
		}


		void Shut ( )
		{
			//
		}


		int Lua_isExists ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr pa = luaT_checkstring( L, 1 );
			lua_pushboolean( L, globals::fs->IsExists(pa) );
			return 1;
		}


		int Lua_isEmpty ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr pa = luaT_checkstring( L, 1 );
			lua_pushboolean( L, globals::fs->IsEmpty(pa) );
			return 1;
		}


		int Lua_isDirectory ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr pa = luaT_checkstring( L, 1 );
			lua_pushboolean( L, globals::fs->IsDirectory(pa) );
			return 1;
		}


		int Lua_isFile ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr pa = luaT_checkstring( L, 1 );
			lua_pushboolean( L, globals::fs->IsFile(pa) );
			return 1;
		}


		int Lua_isReadOnly ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr pa = luaT_checkstring( L, 1 );
			lua_pushboolean( L, globals::fs->IsReadOnly(pa) );
			return 1;
		}


		int Lua_isHidden ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr pa = luaT_checkstring( L, 1 );
			lua_pushboolean( L, globals::fs->IsHidden(pa) );
			return 1;
		}


		int Lua_isSystem ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr pa = luaT_checkstring( L, 1 );
			lua_pushboolean( L, globals::fs->IsSystem(pa) );
			return 1;
		}


		int Lua_isFullname ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr pa = luaT_checkstring( L, 1 );
			lua_pushboolean( L, globals::fs->IsFullname(pa) );
			return 1;
		}


		int Lua_listDirectory ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr dir = luaT_checkstring( L, 1 );
			pcstr filter = luaT_optstring( L, 2, NULL );

			SU_REQUIRE_OR_ZERO( globals::fs->IsDirectory(dir) );

			IDirectory* idir = globals::fs->OpenDir(dir,pool);
			SU_REQUIRE_OR_ZERO( idir );
			lua_newtable( L );
			PString pa;
			bool gotit = idir->GetFirst(pa);
			uint n = 0;
			while( gotit )
			{
				SU_ASSERT( pa.size() );
				if( !filter || util::Fnmatch(filter,pa,util::FNM_IGNORECASE) )
				{
					lua_pushstring( L, pa.cstr() );
					lua_rawseti( L, -2, ++n );
				}
				gotit = idir->GetNext(pa);
			}
			idir->Release();
			return 1;
		}


		int Lua_findFile ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr   fn = luaT_checkstring( L, 1 );
			pcstr   og = luaT_optstring( L, 2, NULL );
			PString pa = globals::fs->FindFile( fn, og );
			SU_REQUIRE_OR_ZERO( pa.size() );
			lua_pushstring( L, pa.cstr() );
			return 1;
		}


		int Lua_splitExtension ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr fn = luaT_checkstring( L, 1 );
			lua_pushstring( L, globals::fs->SplitExtension(fn) );
			return 1;
		}


		int Lua_hasExtension ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr fn  = luaT_checkstring( L, 1 );
			pcstr ext = luaT_checkstring( L, 2 );			
			lua_pushboolean( L, globals::fs->HasExtension( fn, ext ));
			return 1;
		}


		int Lua_getExtension ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr fn = luaT_checkstring( L, 1 );
			lua_pushstring( L, globals::fs->GetExtension( fn ) );
			return 1;
		}


		int Lua_getSize ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr fn = luaT_checkstring( L, 1 );
			uint64 bs;
			if( globals::fs->GetSize(fn,bs) )
			{
				uint32 lo, hi;
				lo = bs & 0xFFFFFFFF;
				hi = bs>>32;
				lua_pushinteger( L, lo );
				lua_pushinteger( L, hi );
				return 2;
			}
			else
			{
				return 0;
			}
		}


		int Lua_glob ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr fn = luaT_checkstring( L, 1 );
			vector<PString> gfiles( pool );
			globals::fs->GlobFiles(fn,gfiles,pool);
			if( gfiles.empty() )
			{
				return 0;
			}
			else
			{
				lua_newtable( L );
				for( uint i=0 ; i<gfiles.size() ; i++ )
				{
					lua_pushstring( L, gfiles[i].cstr() );
					lua_rawseti( L, -2, i+1 );
				}
				return 1;
			}
		}


		int Lua_getDirname ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr   inpa = luaT_checkstring( L, 1 );
			PString pa   = globals::fs->GetDirname(inpa);
			SU_REQUIRE_OR_ZERO( pa.size() );
			lua_pushstring( L, pa.cstr() );
			return 1;
		}


		int Lua_getBasename ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr   inpa = luaT_checkstring( L, 1 );
			PString pa   = globals::fs->GetBasename(inpa);
			SU_REQUIRE_OR_ZERO( pa.size() );
			lua_pushstring( L, pa.cstr() );
			return 1;
		}


		int Lua_getFullname ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr   inpa = luaT_checkstring( L, 1 );
			PString pa   = globals::fs->GetFullname(inpa);
			SU_REQUIRE_OR_ZERO( pa.size() );
			lua_pushstring( L, pa.cstr() );
			return 1;
		}


		int Lua_getNormalized ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr   inpa = luaT_checkstring( L, 1 );
			PString pa   = globals::fs->GetNormalized(inpa);
			SU_REQUIRE_OR_ZERO( pa.size() );
			lua_pushstring( L, pa.cstr() );
			return 1;
		}


		int Lua_getVirtualPath ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr   inpa = luaT_checkstring( L, 1 );
			PString pa   = globals::fs->GetVirtualPath(inpa);
			SU_REQUIRE_OR_ZERO( pa.size() );
			lua_pushstring( L, pa.cstr() );
			return 1;
		}


		int Lua_getHostPath ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			pcstr   inpa = luaT_checkstring( L, 1 );
			PString pa   = globals::fs->GetHostPath(inpa);
			SU_REQUIRE_OR_ZERO( pa.size() );
			lua_pushstring( L, pa.cstr() );
			return 1;
		}


		int Lua_hashContent ( lua_State* L )
		{
			SU_REQUIRE_OR_ZERO( globals::fs );
			int n = lua_gettop( L );

			SU_REQUIRE_OR_ZERO( n );
			util::hmac_md5_begin();

			ObstackObject<> obs;

			for( uint i=1 ; i<=n ; i++ )
			{
				pcstr f = luaT_checkstring( L, i );
				if( f )
				{
					IFileView* v = globals::fs->OpenView(f,obs);
					if( v )
					{
						uint64 bs = v->GetSize();
						void*  ba = v->Map();
						if( ba && bs )	util::hmac_md5_append( ba, bs );
						SafeRelease( v );
					}
				}
			}

			uint8* md5 = util::hmac_md5_end();
			if( !md5 )
				return 0;

			lua_pushstring( L, util::hmac_string(md5,16) );
			return 1;
		}


		int Lua_removeFile ( lua_State* L )
		{
			pcstr pa = luaT_checkstring(L,1);
			SU_REQUIRE_OR_ZERO( globals::fs );
			bool res = globals::fs->RemoveFile( pa );
			lua_pushboolean(L,res);
			return 1;
		}


		int Lua_removeDir ( lua_State* L )
		{
			pcstr pa = luaT_checkstring(L,1);
			bool forceNotEmpty = luaT_optboolean(L,2,false);
			SU_REQUIRE_OR_ZERO( globals::fs );
			bool res = globals::fs->RemoveDir( pa, forceNotEmpty );
			lua_pushboolean(L,res);
			return 1;
		}


		int Lua_parseStream ( lua_State* L )
		{
			// parseStream( string )
			luaT_checkstring( L, 1 );

			AnyArray<1> options;
			options.set( 0, "luastate", L );
			IDatatree* ldtt = factory::CreateObject<IDatatree>( uid::MLuaDatatree, pool, pool, &options );
			SU_ASSERT( ldtt );

			IDatatreeIO* iodtt = factory::CreateObject<IDatatreeIO>( uid::MDatatreeIO, pool );
			SU_ASSERT( iodtt );

			pcstr stream = lua_tostring( L, 1 );
			int n = iodtt->Parse( ldtt, (byte*)stream, util::Strlen(stream) );

			SafeRelease( iodtt );
			SafeRelease( ldtt );

			return Max(n,0);
		}


		int Lua_loadStream ( lua_State* L )
		{
			// loadStream( filename )
			luaT_checkstring( L, 1 );
			SU_REQUIRE_OR_0( globals::fs );

			AnyArray<1> options;
			options.set( 0, "luastate", L );
			IDatatree* ldtt = factory::CreateObject<IDatatree>( uid::MLuaDatatree, pool, pool, &options );
			SU_ASSERT( ldtt );

			IDatatreeIO* iodtt = factory::CreateObject<IDatatreeIO>( uid::MDatatreeIO, pool );
			SU_ASSERT( iodtt );

			PString yfile = lua_tostring( L, 1 );
			int n = iodtt->Load( ldtt, yfile );

			SafeRelease( iodtt );
			SafeRelease( ldtt );

			return Max(n,0);
		}


		int Lua_presentStream ( lua_State* L )
		{
			// presentStream( t )
			SU_REQUIRE_OR_0( globals::authpool );

			bool doref = true;
			if( lua_gettop(L)>1 && lua_isboolean(L,1) )
			{
				doref = lua_toboolean(L,1);
				lua_remove(L,1);
			}

			AnyArray<1> options;
			options.set( 0, "luastate", L );
			IDatatree* ldtt = factory::CreateObject<IDatatree>( uid::MLuaDatatree, pool, pool, &options );
			SU_ASSERT( ldtt );

			IDatatreeIO* iodtt = factory::CreateObject<IDatatreeIO>( uid::MDatatreeIO, pool );
			SU_ASSERT( iodtt );

			int n = 0;

			// maximize with a buffer of 64Mo !
			uint capacity = 64*1024*1024;
			byte* buffer = (byte*) globals::authpool->Alloc( capacity );
			if( buffer )
			{
				uint bsize;
				if( iodtt->Present(ldtt,buffer,capacity,bsize,lua_gettop(L),doref) )
				{
					SU_ASSERT( bsize < capacity )
					buffer[bsize] = 0;
					lua_pushstring( L, (char*)buffer );
					n = 1;
				}

				globals::authpool->Free( buffer );
			}

			SafeRelease( iodtt );
			SafeRelease( ldtt );
			return n;
		}


		int Lua_dumpStream ( lua_State* L )
		{
			// dumpStream( filename )
			SU_REQUIRE_OR_0( globals::fs );
			luaT_checkstring( L, 1 );
			PString yfile = lua_tostring( L, 1 );
			lua_remove( L, 1 );

			bool doref = true;
			if( lua_gettop(L)>1 && lua_isboolean(L,1) ) {
				doref = lua_toboolean(L,1);
				lua_remove(L,1);
			}

			AnyArray<1> options;
			options.set( 0, "luastate", L );
			IDatatree* ldtt = factory::CreateObject<IDatatree>( uid::MLuaDatatree, pool, pool, &options );
			SU_ASSERT( ldtt );

			IDatatreeIO* iodtt = factory::CreateObject<IDatatreeIO>( uid::MDatatreeIO, pool );
			SU_ASSERT( iodtt );

			int n = iodtt->Dump( ldtt, yfile, lua_gettop(L), doref );

			SafeRelease( iodtt );
			SafeRelease( ldtt );
			return Max(n,0);
		}


		int Lua_loadBinary ( lua_State* L )
		{
			PString path = luaT_checkstring( L, 1 );

			SU_REQUIRE_OR_0( globals::fs );
			IFileView* fv = globals::fs->OpenView( path, pool );
			SU_REQUIRE_OR_0( fv );

			int ret = 0;

			void* bytes = fv->Map();
			if( bytes )
			{
				int len = fv->GetSize();
				void* ud = lua_newuserdata(L,len);
				if( ud )
				{
					util::Memcopy(ud,bytes,len);
					ret = 1;
				}
			}

			SafeRelease( fv );
			return ret;
		}


		int Lua_dumpBinary ( lua_State* L )
		{
			PString path = luaT_checkstring( L, 1 );
			luaT_checkudata(L,2);

			SU_REQUIRE_OR_0( globals::fs );
			bool res = globals::fs->DumpToFile( path, lua_touserdata(L,2), lua_objlen(L,2) );

			lua_pushboolean(L,res);
			return 1;
		}


		int Lua_mount ( lua_State* L )
		{
			// mount( dirname, devclassname, devoptions )
			pcstr dirname = luaT_checkstring(L,1);
			pcstr devcname = luaT_checkstring(L,2);
			pcstr devoptions = luaT_checkstring(L,3);
			SU_REQUIRE_OR_0( globals::fs );
			bool r = globals::fs->Mount(dirname,devcname,devoptions);
			lua_pushboolean(L,r);
			return 1;
		}


		int Lua_unmount ( lua_State* L )
		{
			// unmount( dirname )
			pcstr dirname = luaT_checkstring(L,1);
			SU_REQUIRE_OR_0( globals::fs );
			bool r = globals::fs->Unmount(dirname);
			lua_pushboolean(L,r);
			return 1;
		}


		int Lua_unmountAll ( lua_State* L )
		{
			SU_REQUIRE_OR_0( globals::fs );
			globals::fs->UnmountAll();
			return 0;
		}


		int Lua_listMount ( lua_State* L )
		{
			SU_REQUIRE_OR_0( globals::fs );
			lua_newtable(L);
			int n = globals::fs->CountMount();
			for( int i=0 ; i<n ; i++ )
			{
				pcstr mpath = globals::fs->EnumMountPath(i);
				pcstr mopts = globals::fs->EnumMountOptions(i);
				lua_pushstring(L,mopts);
				lua_setfield(L,-2,mpath);
			}
			return 1;
		}

	};


}


