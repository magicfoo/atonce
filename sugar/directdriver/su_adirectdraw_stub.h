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


#define RSC_CHUNK_BS	(32*4)
#define RSC_MEM_BS		(256*1024)





namespace
{


	struct SIDL_STUB_SUPERNAME
	{

		static uint* dd_cmd_unknown ( uint* ip, uint opcode, void* udata, void* ctxt )
		{
			SU_ERROR( "unknown cmd reached !" );
			byte code = ip[0] >> 24;
			return NULL;
		}


		struct Shdcmd
		{
			IDirectShader::Action		act;
			void*						udata;
		};


		struct Shdnode
		{
			uint						pid;		// parent id (0 for the root)
			IDirectShader*				obj;
			Shdcmd						cmd[128];
		};


		typedef vector<IDirectDraw::Display,2>		DisplayA;
		typedef vector<IDirectDraw::Screen,2>		ScreenA;
		typedef vector<Shdnode,64>					Shdtree;


		Pool*					pool;
		Pool*					rpool;
		DisplayA				displays;
		ScreenA					screens;
		IAllocatorChunk*		rscator;
		void**					rsctab;
		Shdtree					shdtree;
		uint					shdstart;

		Shdnode*				runnode;
		uint*					runip;
		uint*					lerrorip;




	#if defined(SU_HW_WINDOWS)

		static BOOL CALLBACK enum_monitor_proc( HMONITOR hmon, HDC hdc, LPRECT prect, LPARAM lparam )
		{
			SU_REQUIRE_OR_FALSE( lparam );

			IDirectDraw::Screen s;
			s.dpy = 0;
			s.x = prect->left;
			s.w = prect->right - prect->left;
			s.y = prect->top;
			s.h = prect->bottom - prect->top;

			((SIDL_STUB_SUPERNAME*)lparam)->screens.push_back( s );
			return true;
		}

		void init_display_and_screens ( )
		{
			displays.resize( 1 );
			displays[0].x = ::GetSystemMetrics( SM_XVIRTUALSCREEN );
			displays[0].y = ::GetSystemMetrics( SM_YVIRTUALSCREEN );
			displays[0].w = ::GetSystemMetrics( SM_CXVIRTUALSCREEN );
			displays[0].h = ::GetSystemMetrics( SM_CYVIRTUALSCREEN );

			screens.clear();
			int mon = ::GetSystemMetrics( SM_CMONITORS );
			SU_ASSERT( mon <= 8 );
			::EnumDisplayMonitors( NULL, NULL, &enum_monitor_proc, (LPARAM)this );
			SU_ASSERT( screens.size() == mon );
		}

	#elif defined(SU_HW_IOS)

		void init_display_and_screens ( )
		{
			UIScreen* ms = [UIScreen mainScreen];
			CGRect r = [ms bounds];
			
			SU_ASSERT( r.size.width );
			SU_ASSERT( r.size.height );
			
			
		#if defined( __IPHONE_4_0 )
			if( globals::ios_haveretina && globals::ios_enablehd )
			{
				r.size.width  *= 2.0f;
				r.size.height *= 2.0f;
			}
		#endif
			
			
			displays.resize( 1 );
			screens.resize( 1 );

			screens[0].dpy = 0;
			screens[0].x = displays[0].x = r.origin.x;
			screens[0].y = displays[0].y = r.origin.y;
			screens[0].w = displays[0].w = r.size.width;
			screens[0].h = displays[0].h = r.size.height;
		}
	
	#elif defined(SU_HW_DARWIN)

		void init_display_and_screens ( )
		{
			NSRect r = [[NSScreen mainScreen] frame];
			SU_REQUIRE_OR_RETURN( r.size.width )
			SU_REQUIRE_OR_RETURN( r.size.height )
			
			displays.resize( 1 );
			screens.resize( 1 );
			
			screens[0].dpy = 0;
			screens[0].x = displays[0].x = r.origin.x;
			screens[0].y = displays[0].y = r.origin.y;
			screens[0].w = displays[0].w = r.size.width;
			screens[0].h = displays[0].h = r.size.height;
		}

	#elif defined( SU_HW_ANDROID )

		void init_display_and_screens ( )
		{
			displays.resize( 1 );
			screens.resize( 1 );

			uint w, h;
			{
				SU_ASSERT( globals::and_jnienv );
				SU_ASSERT( globals::and_holdercb );

				JNIEnv* env			= (JNIEnv*)globals::and_jnienv;
				jobject hcb		= (jobject)globals::and_holdercb;
				jclass clsView		= env->GetObjectClass( hcb );
				jmethodID v_width	= env->GetMethodID( clsView, "GetWidth", "()I" );
				jmethodID v_height	= env->GetMethodID( clsView, "GetHeight", "()I" );
				w = (uint)env->CallIntMethod( hcb, v_width );
				if( env->ExceptionCheck() )	w = 0;
				h = (uint)env->CallIntMethod( hcb, v_height );
				if( env->ExceptionCheck() )	h = 0;
			}

			screens[0].dpy = 0;
			screens[0].x = displays[0].x = 0;
			screens[0].y = displays[0].y = 0;
			screens[0].w = displays[0].w = w;
			screens[0].h = displays[0].h = h;
		}

	#elif defined( SU_HW_PSP2 )

		void init_display_and_screens ( )
		{
			displays.resize( 1 );
			screens.resize( 1 );

			screens[0].dpy = 0;
			screens[0].x = displays[0].x = 0;
			screens[0].y = displays[0].y = 0;
			screens[0].w = displays[0].w = 960;
			screens[0].h = displays[0].h = 544;
		}

	#endif


		bool Init ( Pool* p, AnyList* o )
		{
			SU_REQUIRE_OR_FALSE( p );

			pool = p;
			rpool = NULL;

			init_display_and_screens();
			return true;
		}


		void Shut ( )
		{
			//
		}


		Pool* PreRealize ( )
		{
			rpool = pool->CreateSub();
			SU_ASSERT( rpool );

			void* m = rpool->Alloc( RSC_MEM_BS );
			if( m )
			{
				rscator = factory::CreateObject<IAllocatorChunk>( uid::MAllocatorChunk, rpool );
				if( rscator && rscator->Realize(m,RSC_MEM_BS,RSC_CHUNK_BS) )
				{
					uint n = rscator->CountChunk();
					rsctab = (void**) rpool->Alloc( 4*n );
					if( rsctab )
					{
						util::Memset( rsctab, 0, 4*n );

						shdtree.clear();

						// reserve the rsc idx 0 !
						void* r0 = rscator->Alloc(4);
						SU_ASSERT( rscator->GetChunkId(r0) == dd::INVALID_IDX );

						runip = NULL;
						lerrorip = NULL;
						return rpool;
					}
				}
			}

			Pool::Destroy( rpool );
			return NULL;
		}


		bool PostRealizeShaders ( uint* shade_tree )
		{
			SU_REQUIRE_OR_FALSE( shade_tree );

			IDirectDraw* ddraw;
			ddraw = SIDL_IMPL_INTERFACE( IDirectDraw );

			AnyArray<1> options;
			options.set( 0, "ddraw", ddraw );

			uint tree_size  = shade_tree[0];
			uint tree_start = shade_tree[1];
			SU_ASSERT( tree_start < tree_size );

			shdtree.clear();
			shdtree.resize( tree_size );
			
			for( uint i=0 ; i<tree_size ; i++ )
			{
				uint32 muid = shade_tree[i*2+2];
				uint   pid  = shade_tree[i*2+3];

				IDirectShader* obj;
				obj = factory::CreateObject<IDirectShader>( muid, rpool, rpool, &options );
				SU_ASSERT( obj );


				// setting up inherited cmd table
				if( i==0 )
				{
					SU_ASSERT( pid==0 );
					for( uint j=0 ; j<128 ; j++ )
					{
						shdtree[i].cmd[j].act   = dd_cmd_unknown;
						shdtree[i].cmd[j].udata = NULL;
					}
				}
				else
				{
					SU_ASSERT( pid<i  );
					util::Memcopy( shdtree[i].cmd, shdtree[pid].cmd, sizeof(shdtree[i].cmd) );
				}

				// setting up shader specific commands
				uint na = obj->CountAction();
				SU_ASSERT( na < 128 );

				for( uint k=0 ; k<na ; k++ )
				{
					byte					code;
					IDirectShader::Action	act;
					void*					udata;

					obj->EnumAction( k, code, act, udata );
					SU_ASSERT( code < 128 );

					shdtree[i].cmd[code].act   = act;
					shdtree[i].cmd[code].udata = udata;
				}

				shdtree[i].pid = pid;
				shdtree[i].obj = obj;
			}
			
			shdstart = tree_start;
			return true;
		}


		void PreUnrealize (  )
		{
			for( uint i=0 ; i<shdtree.size() ; i++ )
				SafeRelease( shdtree[i].obj );

			Pool::Destroy( rpool );
		}


		virtual bool IsRealized (  ) = 0;


		uint CountDisplay (  )
		{
			return displays.size();
		}


		bool EnumDisplay ( uint inIndex, IDirectDraw::Display& outDisplay )
		{
			SU_REQUIRE_OR_FALSE( inIndex < displays.size() );
			outDisplay = displays[ inIndex ];
			return true;
		}


		uint CountScreen (  )
		{
			return screens.size();
		}


		bool EnumScreen ( uint inIndex, IDirectDraw::Screen& outScreen )
		{
			SU_REQUIRE_OR_FALSE( inIndex < screens.size() );

			outScreen = screens[ inIndex ];
			return true;
		}


		int AllocResource ( void*& outAddr, uint inBSize )
		{
			SU_ASSERT( inBSize <= RSC_CHUNK_BS );
			SU_REQUIRE_OR_RETURN_V( IsRealized(), dd::INVALID_IDX );
			SU_REQUIRE_OR_RETURN_V( inBSize <= RSC_CHUNK_BS, dd::INVALID_IDX );
			SU_ASSERT( rscator );

			void* rp = rscator->Alloc( inBSize );
			if( !rp )	return dd::INVALID_IDX;

			int ri = rscator->GetChunkId( rp );
			SU_ASSERT( ri != -1 );

			SU_ASSERT( rsctab[ri] == NULL );
			rsctab[ri] = rp;

			outAddr = rp;
			return ri;
		}


		void FreeResource ( int inId )
		{
			SU_REQUIRE_OR_RETURN( IsRealized() );
			SU_ASSERT( rscator );

			void* addr = ResourceBase( inId );
			if( !addr )		return;

			rscator->Free( addr );
			rsctab[inId] = NULL;
		}


		int ResourceCountMax ( )
		{
			SU_REQUIRE_OR_RETURN_V( IsRealized(), 0 );
			SU_ASSERT( rscator );
			return rscator->CountChunk();
		}


		void** ResourceTab ( )
		{
			SU_REQUIRE_OR_NULL( IsRealized() );
			return rsctab;
		}


		void* ResourceBase ( int inId )
		{
			SU_REQUIRE_OR_NULL( IsRealized() );
			SU_ASSERT( rscator );
			if( inId<0 )						return NULL;
			if( inId>=ResourceCountMax() )		return NULL;
			return rsctab[ inId ];
		}


		template < typename T > inline
		T* ResourceObject ( int inId )
		{
			void* addr = ResourceBase( inId );
			return (T*) addr;
		}


		uint CountShader ( )
		{
			SU_REQUIRE_OR_RETURN_V( IsRealized(), 0 );
			return shdtree.size();
		}


		IDirectShader* EnumShader ( uint inIndex )
		{
			SU_REQUIRE_OR_NULL( IsRealized() );
			SU_REQUIRE_OR_NULL( inIndex<shdtree.size() );
			return shdtree[ inIndex ].obj;
		}


		bool IsFullscreen ( )
		{
			return false;
		}


		IDirectDraw::ShaderModel GetShaderModel ( )
		{
			return IDirectDraw::SM_None;
		}


		bool StartCommandList ( uint* inBAddr, void* ctxt )
		{
			SU_REQUIRE_OR_FALSE( IsRealized() );
			SU_REQUIRE_OR_FALSE( shdtree.size() );
			SU_REQUIRE_OR_FALSE( inBAddr!=NULL );
			SU_REQUIRE_OR_FALSE( runip==NULL );

			uint n = CountShader();
			for( uint i=0 ; i<n ; i++ )
				EnumShader(i)->OnStart();

			runip    = inBAddr;
			runnode  = &shdtree[shdstart];
			lerrorip = NULL;

			register uint *ip, br, opc, code;
			Shdcmd*	cmd;

			for( ;; )
			{
				ip    = runip;
				opc   = *ip;
				br    = opc>>31;
				code  = (opc<<1)>>25;
				cmd	  = runnode->cmd + code;

				// break request ?
				SU_BREAK_IF( br );

				runip = ip;
				runip = (*cmd->act)( runip, opc, cmd->udata, ctxt );

				// error ?
				SU_BREAK_IF( !runip );
				if( !runip ) { lerrorip = ip; break; }

				// end ?
				if( code==0 )	break;
			}

			runip   = NULL;
			runnode = NULL;
			return true;
		}


		bool SelectShader ( int ni )
		{
			SU_ASSERT( IsRealized() );
			SU_ASSERT( runip );
			SU_ASSERT( ni >= 0 );
			SU_ASSERT( ni < int(shdtree.size()) );
			runnode = &shdtree[ni];
			return true;
		}


		bool IsTerminated (  )
		{
			SU_REQUIRE_OR_TRUE( IsRealized() );
			return (runip == NULL);
		}


		void WaitForTermination (  )
		{
			SU_REQUIRE_OR_RETURN( IsRealized() );
			while( !IsTerminated() )
			{
				//
			}
		}


		uint* GetLastErrorAddr ( )
		{
			return lerrorip;
		}

	};


}


