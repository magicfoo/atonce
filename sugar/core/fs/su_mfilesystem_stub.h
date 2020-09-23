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


using namespace su;
#include "../../codec/su_codec.h"



namespace
{


	struct MountInfo
	{
		PString			vbase;			// virtual path prefix
		PString			hbase;			// host path prefix
		PString			drioptions;		// driver options
		IDriverFS*		driobj;			// driver object
	};

	typedef vector<MountInfo,16>	MountInfoA;


	struct SIDL_STUB_CLASSNAME
	{
		MountInfoA		miA;
		PString			cwd;
		Pool*			pool;
		IFileSystem*	myfs;


		bool normalized_path ( pcstr inpath, PString& outnpath )
		{
			SU_REQUIRE_OR_FALSE( inpath );
			PString path( inpath );

			vector<PString,64> comps(pool);

			// split in components
			int pos = 0;
			for( ;; )
			{
				int p = path.indexofany( "/\\", pos );

				PString comp;
				if( p < 0 )	comp = path.right( pos );
				else		comp = path.mid( pos, p-pos );
				pos = p+1;

				if( comp.empty() || comp.equals(".") )
				{
					// ...
				}
				else if( comp.equals("..") )
				{
					if( comps.empty() )
						return false;
					comps.pop_back();
				}
				else
				{
					comps.push_back( comp );
				}

				if( p < 0 )
					break;
			}

			if( inpath[0]=='/' )		outnpath = "/";
			else if( inpath[0]=='\\' )	outnpath = "/";
			else						outnpath = "";

			for( uint i=0 ; i<comps.size() ; i++ )
			{
				if( i>0 )	outnpath += "/";
				outnpath += comps[i];
			}

			return true;
		}


		int find_mount_from_virtual ( pcstr invpath )
		{
			PString	inpath( invpath );

			int	match_idx = -1;
			int	match_len = 0;
			for( uint i=0 ; i<miA.size() ; i++ )
			{
				bool matched = inpath.startswith( miA[i].vbase, false );
				if( matched )
				{
					int len = miA[i].vbase.size();
					if( match_idx<0 || len>=match_len )
					{
						match_idx = i;
						match_len = len;
					}
				}
			}

			return match_idx;
		}


		int find_mount_from_host ( pcstr inhpath )
		{
			PString	inpath( inhpath );

			int	match_idx = -1;
			int	match_len = 0;
			for( uint i=0 ; i<miA.size() ; i++ )
			{
				bool matched = inpath.startswith( miA[i].hbase, false );
				if( matched )
				{
					int len = miA[i].hbase.size();
					if( match_idx<0 || len>=match_len )
					{
						match_idx = i;
						match_len = len;
					}
				}
			}

			return match_idx;
		}


		int virtual_to_host ( pcstr invpath, PString& outhpath )
		{
			PString npath;
			if( !normalized_path(invpath,npath) )
				return -1;

			SU_REQUIRE_OR_NEG( !npath.empty() );
			SU_REQUIRE_OR_NEG( npath.startswith("/") );

			int i = find_mount_from_virtual( npath.cstr() );
			SU_REQUIRE_OR_NEG( i>=0 );

			outhpath = npath.right( miA[i].vbase.size() );
			while( outhpath.startswithany("/\\") )
				outhpath.erase(0,1);

			return i;
		}


		int host_to_virtual ( pcstr inhpath, PString& outvpath )
		{
			PString npath;
			if( !normalized_path(inhpath,npath) )
				return -1;

			SU_REQUIRE_OR_NEG( !npath.empty() );

			int i = find_mount_from_host( npath.cstr() );
			SU_REQUIRE_OR_NEG( i>=0 );

			outvpath = npath.right( miA[i].hbase.size() );
			while( outvpath.startswithany("/\\") )
				outvpath.erase(0,1);

			return i;
		}





		bool Init ( Pool* p, AnyList* o )
		{
			cwd = "/";
			pool = p;

			miA.relocate( p );

			myfs = SIDL_IMPL_INTERFACE( IFileSystem );

			if( myfs && globals::fs==NULL )
				globals::fs = myfs;

			return TRUE;
		}


		void Shut ( )
		{
			if( globals::fs == myfs )
				globals::fs = NULL;

			pool = NULL;
		}


		bool IsExists ( pcstr inPathname )
		{
			PString hpath;
			int i = virtual_to_host( inPathname, hpath );
			if( i<0 )
				return FALSE;
			uint attr = miA[i].driobj->GetAttributes( hpath );
			return ( attr != IDriverFS::A_NOT_FOUND );
		}


		bool IsEmpty ( pcstr inPathname )
		{
			PString hpath;
			int i = virtual_to_host( inPathname, hpath );
			if( i<0 )
				return FALSE;
			uint64 bs = 0;
			if( !miA[i].driobj->GetSize(hpath,bs) )
				return FALSE;
			return (bs == 0);
		}


		bool IsDirectory ( pcstr inPathname )
		{
			PString hpath;
			int i = virtual_to_host( inPathname, hpath );
			if( i<0 )
				return FALSE;
			uint attr = miA[i].driobj->GetAttributes( hpath );
			return ( attr & IDriverFS::A_DIR ) != 0;
		}


		bool IsFile ( pcstr inPathname )
		{
			PString hpath;
			int i = virtual_to_host( inPathname, hpath );
			if( i<0 )
				return FALSE;
			uint attr = miA[i].driobj->GetAttributes( hpath );
			return ( attr & IDriverFS::A_FILE ) != 0;
		}


		bool IsReadOnly ( pcstr inPathname )
		{
			PString hpath;
			int i = virtual_to_host( inPathname, hpath );
			if( i<0 )
				return FALSE;
			uint attr = miA[i].driobj->GetAttributes( hpath );
			return ( attr & IDriverFS::A_RONLY ) != 0;
		}


		bool IsHidden ( pcstr inPathname )
		{
			PString hpath;
			int i = virtual_to_host( inPathname, hpath );
			if( i<0 )
				return FALSE;
			uint attr = miA[i].driobj->GetAttributes( hpath );
			return ( attr & IDriverFS::A_HIDDEN ) != 0;
		}


		bool IsSystem ( pcstr inPathname )
		{
			PString hpath;
			int i = virtual_to_host( inPathname, hpath );
			if( i<0 )
				return FALSE;
			uint attr = miA[i].driobj->GetAttributes( hpath );
			return ( attr & IDriverFS::A_SYSTEM ) != 0;
		}


		bool IsFullname ( pcstr inPathname )
		{
			return util::Strstartswithany( inPathname, "/\\" );
		}


		bool IsInDirectory ( pcstr inPathname,
		                     pcstr inDirname )
		{
			PString p = GetDirname( GetNormalized(inPathname) );
			PString d = GetNormalized( inDirname );
			for( ;; )
			{
				if( p.empty() )
					return FALSE;
				if( p.equals(d) )
					return TRUE;
				p = GetDirname( p );
			}
		}


		PString SplitExtension ( pcstr inPathname )
		{
			PString p( inPathname );
			while( p.endswithany("/\\") )
				p.erasend(1);
			int ext = p.indexof('.',0,FALSE,TRUE);
			if( ext < 0 )
				return p;
			return p.left( ext );
		}


		bool HasExtension ( pcstr inPathname,
		                    pcstr inExtension )
		{
			PString extfn = GetExtension( inPathname );
			char b[50];
			util::Strtolower( b, inExtension );			 
			return util::Strequals(extfn,b) ;
		}


		PString GetExtension ( pcstr inPathname )
		{
			PString p( inPathname );
			while( p.endswithany("/\\") )
				p.erasend(1);
			int ext = p.indexof('.',0,FALSE,TRUE);
			if( ext < 0 )
				return PString();
			PString retStr = util::Strtolower((pstr)(p.right( ext+1 )).cstr());
			return retStr;
		}


		PString GetBasename ( pcstr inPathname )
		{
			PString p( inPathname );
			while( p.endswithany("/\\") )
				p.erasend(1);
			int sep = p.indexofany("/\\",0,FALSE,TRUE);
			if( sep < 0 )
				return p;
			return p.right( sep+1 );
		}


		PString GetDirname ( pcstr inPathname )
		{
			PString p( inPathname );
			while( p.endswithany("/\\") )
				p.erasend(1);
			int sep = p.indexofany("/\\",0,FALSE,TRUE);
			if( sep < 0 )
				return PString();
			if( sep==0 )
				return PString("/");
			return p.left( sep );
		}


		PString GetFullname ( pcstr inPathname )
		{
			if( IsFullname(inPathname) )
				return PString(inPathname);
			return PString("%%/%%").arg(GetCurrentDirectory()).arg(inPathname);
		}


		PString GetNormalized ( pcstr inPathname )
		{
			PString npath;
			if( !normalized_path(inPathname,npath) )
				return PString(inPathname);
			return PString(npath);
		}


		bool GetSize (	pcstr	inFilename,
						uint64& outBSize )
		{
			PString hpath;
			int i = virtual_to_host( inFilename, hpath );
			if( i<0 )
				return FALSE;
			return miA[i].driobj->GetSize( hpath, outBSize );
		}


		bool GetWriteTime ( pcstr inFilename,
		                    Date& outDate )
		{
			PString hpath;
			int i = virtual_to_host( inFilename, hpath );
			if( i<0 )
				return FALSE;
			return miA[i].driobj->GetWriteTime( hpath, outDate );
		}


		bool GetCreationTime ( pcstr inFilename,
		                       Date& outDate )
		{
			PString hpath;
			int i = virtual_to_host( inFilename, hpath );
			if( i<0 )
				return FALSE;
			return miA[i].driobj->GetCreationTime( hpath, outDate );
		}


		uint128 HashContent ( pcstr inFilename )
		{
			ObstackObject<> obs;

			uint128 h;
			util::Zero(h);

			IFileView* v = OpenView(inFilename,obs);
			if( v )
			{
				uint64 bs = v->GetSize();
				void*  ba = v->Map();
				if( ba )
					h = util::hmac_md5_w( ba, bs );
				SafeRelease( v );
			}

			return h;
		}


		PString GetCurrentDirectory (  )
		{
			return PString( cwd.cstr() );
		}


		bool SetCurrentDirectory ( pcstr inDirname )
		{
			if( !IsDirectory(inDirname) )
				return FALSE;
			cwd.copy( GetNormalized(inDirname).cstr() );
			return TRUE;
		}


		PString GetCommonPath ( pcstr inPathname0,
		                       pcstr inPathname1 )
		{
			PString p0 = GetNormalized( inPathname0 );
			PString p1 = GetNormalized( inPathname1 );
			for( ;; )
			{
				if( p0.equals(p1) )
					return p0;
				if( p0.empty() || p1.empty() )
					return PString();
				if( p0.size() > p1.size() )
					p0 = GetDirname(p0);
				else
					p1 = GetDirname(p1);
			}
		}


		PString GetCommonDirectory ( pcstr inPathname0,
		                             pcstr inPathname1 )
		{
			PString p0 = GetDirname( GetNormalized( inPathname0 ) );
			PString p1 = GetDirname( GetNormalized( inPathname1 ) );
			for( ;; )
			{
				if( p0.equals(p1) )
					return p0;
				if( p0.empty() || p1.empty() )
					return PString();
				if( p0.size() > p1.size() )
					p0 = GetDirname(p0);
				else
					p1 = GetDirname(p1);
			}
		}


		PString GetRelativeFrom ( pcstr inPathname,
		                          pcstr inOriginPath )
		{
			// find a common path
			PString p = GetNormalized( inPathname );
			PString o = GetNormalized( inOriginPath );
			PString bp, bo;
			for( ;; )
			{
				if( p.equals(o) )
					return bo+bp;

				if( p.empty() || o.empty() )
					return PString(inPathname);

				if( p.size() > o.size() )
				{
					if( bp.size() )
						bp.prepend( "/" );
					bp.prepend( GetBasename(p) );
					p = GetDirname(p);
				}
				else
				{
					bo.append( "../" );
					o = GetDirname(o);
				}
			}
		}


		PString FindFile( pcstr inPathname, pcstr inOriginPath )
		{
			if( inPathname )
			{
				if( IsFile(inPathname) )
					return PString(inPathname);

				if( IsFullname(inPathname) )
					return PString(inPathname);

				PString fn;

				if( inOriginPath )
				{
					PString dn;
					if( IsDirectory(inOriginPath) )
						dn = inOriginPath;
					else
						dn = GetDirname( inOriginPath );
					fn = PString("%%/%%").arg(dn).arg(inPathname);
				}
				else
				{
					fn = GetFullname( inPathname );
				}

				if( IsFile(fn) )
					return PString(fn);
			}

			return PString();
		}


		bool Hide ( pcstr inPathname )
		{
			PString hpath;
			int i = virtual_to_host( inPathname, hpath );
			if( i<0 )
				return FALSE;
			uint attr = miA[i].driobj->OrAttributes( hpath, IDriverFS::A_HIDDEN );
			return ( attr & IDriverFS::A_HIDDEN ) != 0;
		}


		bool Unhide ( pcstr inPathname )
		{
			PString hpath;
			int i = virtual_to_host( inPathname, hpath );
			if( i<0 )
				return FALSE;
			uint attr = miA[i].driobj->AndAttributes( hpath, ~IDriverFS::A_HIDDEN );
			return ( attr & IDriverFS::A_HIDDEN ) == 0;
		}


		bool SetReadOnly ( pcstr inPathname )
		{
			PString hpath;
			int i = virtual_to_host( inPathname, hpath );
			if( i<0 )
				return FALSE;
			uint attr = miA[i].driobj->OrAttributes( hpath, IDriverFS::A_RONLY );
			return ( attr & IDriverFS::A_RONLY ) != 0;
		}


		bool SetReadWrite ( pcstr inPathname )
		{
			PString hpath;
			int i = virtual_to_host( inPathname, hpath );
			if( i<0 )
				return FALSE;
			uint attr = miA[i].driobj->AndAttributes( hpath, ~IDriverFS::A_RONLY );
			return ( attr & IDriverFS::A_RONLY ) == 0;
		}


		bool CopyFile ( pcstr inDstFilename, pcstr inSrcFilename, bool inFailIfExists, Pool* inP )
		{
			PString dst_hpath;
			int i = virtual_to_host( inDstFilename, dst_hpath );
			if( i<0 )
				return FALSE;

			PString src_hpath;
			int j = virtual_to_host( inSrcFilename, src_hpath );
			if( j<0 )
				return FALSE;

			// same device ?
			if( i==j )
				return miA[i].driobj->CopyFile( dst_hpath, src_hpath, inFailIfExists );

			// inter device !
			bool crt = miA[i].driobj->CreateFile( dst_hpath, inFailIfExists );
			if( !crt )
				return FALSE;


			Pool* p = (inP) ? inP : pool;

			IFileWriter* dstf = miA[i].driobj->OpenWriter( dst_hpath, p );
			IFileReader* srcf = miA[j].driobj->OpenReader( src_hpath, p );
			if( !dstf || !srcf )
			{
				SafeRelease( dstf );
				SafeRelease( srcf );
				return FALSE;
			}

			uint64	bs		= srcf->GetSize();
			uint64	buffbs	= Min( bs, uint64(32*1024) );
			void*	buffptr	= p->Alloc( buffbs );
			bool	success	= (buffptr != NULL);

			srcf->SetPos( 0 );
			dstf->SetPos( 0 );

			while( bs && success )
			{
				uint toload = Min( bs, buffbs );

				if( !srcf->Read(buffptr,toload) ) {
					success = FALSE;
					break;
				}

				if( !dstf->Write(buffptr,toload) ) {
					success = FALSE;
					break;
				}

				bs -= toload;
			}

			p->Free( buffptr );
			SafeRelease( dstf );
			SafeRelease( srcf );
			return success;
		}


		bool MoveFile ( pcstr inDstFilename,
		                pcstr inSrcFilename )
		{
			PString dst_hpath;
			int i = virtual_to_host( inDstFilename, dst_hpath );
			if( i<0 )	return FALSE;

			PString src_hpath;
			int j = virtual_to_host( inSrcFilename, src_hpath );
			if( j<0 )	return FALSE;

			// same device ?
			if( i==j )
				return miA[i].driobj->MoveFile( dst_hpath, src_hpath );

			// inter device !
			bool cpy = CopyFile( inDstFilename, inSrcFilename, NULL, FALSE );
			if( !cpy )
				return FALSE;
			return miA[i].driobj->RemoveFile( dst_hpath );
		}


		bool CreateFile ( pcstr inFilename,
		                  bool inFailIfExists,
		                  uint64 inBSize )
		{
			PString hpath;
			int i = virtual_to_host( inFilename, hpath );
			if( i<0 )
				return FALSE;
			return miA[i].driobj->CreateFile( hpath, inFailIfExists, inBSize );
		}


		bool CreateDir ( pcstr inDirname )
		{
			PString hpath;
			int i = virtual_to_host( inDirname, hpath );
			if( i<0 )
				return FALSE;
			return miA[i].driobj->CreateDir( hpath );
		}


		bool RemoveFile ( pcstr inFilename )
		{
			PString hpath;
			int i = virtual_to_host( inFilename, hpath );
			if( i<0 )
				return FALSE;
			return miA[i].driobj->RemoveFile( hpath );
		}


		bool RemoveDir ( pcstr inDirname, bool inForceNotEmpty )
		{
			PString hpath;
			int i = virtual_to_host( inDirname, hpath );
			if( i<0 )
				return FALSE;
			return miA[i].driobj->RemoveDir( hpath, inForceNotEmpty );
		}


		IFileView* OpenView	(	pcstr	inFilename, Pool*	inPool		)
		{
			PString hpath;
			int i = virtual_to_host( inFilename, hpath );
			if( i<0 )
				return NULL;
			return miA[i].driobj->OpenView( hpath, inPool );
		}


		IFileBurst* OpenBurst (	pcstr	inFilename, Pool*	inPool )
		{
			PString hpath;
			int i = virtual_to_host( inFilename, hpath );
			if( i<0 )
				return NULL;
			return miA[i].driobj->OpenBurst( hpath, inPool );
		}


		IFileReader* OpenReader	(	pcstr	inFilename, Pool*	inPool		)
		{
			PString hpath;
			int i = virtual_to_host( inFilename, hpath );
			if( i<0 )
				return NULL;
			return miA[i].driobj->OpenReader( hpath, inPool );
		}


		IFileWriter* OpenWriter	(	pcstr	inFilename,
									Pool*	inPool,
									bool	inCreateIfNotExists,
									bool	inAppend	)
		{
			PString hpath;
			int i = virtual_to_host( inFilename, hpath );
			if( i<0 )
				return NULL;
			return miA[i].driobj->OpenWriter( hpath, inPool, inCreateIfNotExists, inAppend  );
		}


		IDirectory* OpenDir	( pcstr inDirname, Pool* inPool )
		{
			PString hpath;
			int i = virtual_to_host( inDirname, hpath );
			if( i<0 )
				return NULL;
			return miA[i].driobj->OpenDir( hpath, inPool );
		}


		uint GlobFiles ( pcstr inFiltername, vector<PString>& outFiles, Pool* inPool )
		{
			SU_ASSERT( outFiles.pool() );
			SU_REQUIRE_OR_0( outFiles.pool() );
			outFiles.clear();

			PString tname = GetNormalized( inFiltername );
			PString dname = GetDirname( tname );

			IDirectory* idir = OpenDir( dname, inPool );
			SU_REQUIRE_OR_0( idir );

			PString fname;
			bool cnt = idir->GetFirst(fname);
			while( cnt )
			{
				fname = dname + PString("/") + fname;
				if( util::Fnmatch(tname,fname,util::FNM_PATHNAME) )
					outFiles.push_back( fname );
				cnt = idir->GetNext(fname);
			}

			SafeRelease( idir );
			return outFiles.size();
		}


		bool Mount ( pcstr inDirname, pcstr inDriClassname, pcstr inDriOptions )
		{	
			SU_REQUIRE_OR_FALSE( inDirname )	
			SU_REQUIRE_OR_FALSE( inDriClassname )

			PString dname = GetNormalized( inDirname );
			SU_REQUIRE_OR_FALSE( dname.startswith("/") )

			classid mid = util::hmac_classid( inDriClassname );
			SU_REQUIRE_OR_FALSE( mid )

			AnyArray<1> options;
			options.set( 0, "options", inDriOptions );

			IDriverFS* idri = factory::CreateObject<IDriverFS>( mid, pool, pool, &options );
			if( !idri )
				return false;

			PString hbase = idri->GetHostPath( "" );
			while( hbase.endswithany("/\\") )
				hbase.erasend(1);

			PString vbase = dname;
			while( vbase.endswithany("/\\") )
				vbase.erasend(1);

			MountInfo mi;
			mi.vbase      = vbase;
			mi.hbase      = hbase;
			mi.drioptions = inDriOptions;
			mi.driobj	  = idri;
			miA.push_back( mi );

			return TRUE;
		}


		bool Unmount ( pcstr inDirname )
		{
			if( !inDirname )
				return FALSE;

			PString dname = GetNormalized( inDirname );
			if( !dname.startswith("/") )
				return FALSE;

			int i = find_mount_from_virtual( dname );
			if( i < 0 )
				return FALSE;

			miA[i].driobj->Release();
			miA.erase( miA.begin()+i );

			return TRUE;
		}


		void UnmountAll (  )
		{
			for( uint i=0 ; i<miA.size() ; i++ )
				miA[i].driobj->Release();
			miA.clear();
		}

	
		int CountMount ( )
		{
			return miA.size();
		}


		pcstr EnumMountPath ( int inidx )
		{
			SU_REQUIRE_OR_NULL( inidx >= 0 )
			SU_REQUIRE_OR_NULL( inidx <  miA.size() )
			return miA[inidx].vbase;
		}


		pcstr EnumMountOptions ( int inidx )
		{
			SU_REQUIRE_OR_NULL( inidx >= 0 )
			SU_REQUIRE_OR_NULL( inidx <  miA.size() )
			return miA[inidx].drioptions;
		}


		bool DumpToFile	(	pcstr		inFilename,
							pointer		inBufferAddr,
							uint		inBufferBSize,
							bool		inAppend		)
		{
			SU_REQUIRE_OR_FALSE( inFilename );
			SU_REQUIRE_OR_FALSE( inBufferAddr );
			SU_REQUIRE_OR_FALSE( inBufferBSize );

			ObstackObject<> obs;

			IFileWriter* w = OpenWriter( inFilename, obs, true, inAppend );
			SU_REQUIRE_OR_FALSE( w );

			if( inAppend )		w->SetPos( w->GetSize() );
			else				w->SetPos( 0 );

			int ws = w->Write( inBufferAddr, inBufferBSize );

			w->Release();
			return (ws == inBufferBSize);
		}


		bool DumpFromFile	(  uint&    outBufferBSize,
                               pcstr    inFilename,
                               void*    inBufferAddr,
                               uint     inBufferBCapacity	)
		{
			outBufferBSize = 0;

			SU_REQUIRE_OR_FALSE( inFilename );
			SU_REQUIRE_OR_FALSE( inBufferAddr );
			SU_REQUIRE_OR_FALSE( inBufferBCapacity );

			ObstackObject<> obs;

			IFileReader* r = OpenReader( inFilename, obs );
			SU_REQUIRE_OR_FALSE( r );

			uint fs = r->GetSize();
			if( fs > inBufferBCapacity )
			{
				r->Release();
				return false;
			}

			r->SetPos( 0 );
			int  rs = r->Read( inBufferAddr, fs );
			r->Release();

			SU_REQUIRE_OR_FALSE( rs == fs );

			outBufferBSize = rs;
			return true;
		}


		bool DumpFromFile	(  uint&    outBufferBSize,
                               void*&   outBufferAddr,
                               pcstr    inFilename,
                               Pool*    inPool			)
		{
			outBufferBSize = 0;
			outBufferAddr = NULL;

			SU_REQUIRE_OR_FALSE( inFilename );
			SU_REQUIRE_OR_FALSE( inPool );

			uint64 bs;
			if( !GetSize(inFilename,bs) )
				return false;

			void* ba = inPool->Alloc( bs );
			if( !ba )
				return false;

			if( !DumpFromFile(outBufferBSize,inFilename,ba,bs) )
			{
				inPool->Free( ba );
				return false;
			}

			SU_ASSERT( outBufferBSize==bs );

			outBufferAddr  = ba;
			outBufferBSize = bs;
			return true;
		}


		bool DumpSliceFromFile	(  void*	inBufferAddr,
                                   pcstr    inFilename,
                                   uint     inSliceBOffset,
                                   int      inSliceBSize	)
		{
			SU_REQUIRE_OR_FALSE( inBufferAddr );
			SU_REQUIRE_OR_FALSE( inFilename );
			SU_REQUIRE_OR_FALSE( inSliceBSize );

			ObstackObject<> obs;

			IFileReader* r = OpenReader( inFilename, obs );
			SU_REQUIRE_OR_FALSE( r );

			r->SetPos( inSliceBOffset );
			int rs = r->Read( inBufferAddr, inSliceBSize );
			r->Release();

			return (rs == inSliceBSize);
		}


		PString GetHostPath ( pcstr	inVirtualPath	)
		{
			PString hpath;
			int i = virtual_to_host( inVirtualPath, hpath );
			SU_REQUIRE_OR_NULL( i>=0 );

			return miA[i].driobj->GetHostPath( hpath );
		}


		PString GetVirtualPath ( pcstr	inHostPath	)
		{
			PString vpath;
			int i = host_to_virtual( inHostPath, vpath );
			SU_REQUIRE_OR_NULL( i>=0 );

			PString vfull = PString("%%/%%").arg(miA[i].vbase).arg(vpath);
			vfull = GetNormalized( vfull );

			return vfull;
		}


		bool StartInflateFile( IFileWriter* outFile, IFileReader* inFile, Pool* inpool )
		{
			return init_lzma_file( inpool, outFile, inFile );
		}


		int ContinueInflateFile( uint inmaxmstime )
		{
			Time t0, t1;
			su::clock::GetTime( &t0 );

			int r;

			for( ;; )
			{
				r = decode_lzma_file();
				if( r<0 || r>=100 )
					break;

				su::clock::GetTime( &t1 );
				float dt = t1-t0;
				SU_ASSERT( dt >= 0.f );
				uint mst = int(dt*1000.f);
				if( mst >= inmaxmstime )
					break;
			}

			return r;
		}


		void AbortInflateFile( )
		{
			return abort_lzma_file();
		}

	};


}


