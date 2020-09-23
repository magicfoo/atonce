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

#include <sufw/directdriver/su_directdriver.h>
#include <sufw/asset/su_irscmanager.h>
#include <sufw/engine/su_engine.h>


using namespace su;
using namespace su::sg;



namespace
{
	void	Box3Unit	(	Box3*	ioBox	)
	{
		ioBox->center	= Vec3::ZERO;
		ioBox->rotation	= Quat::UNIT;
		ioBox->length	= Vec3::ONE;
	}


	struct SIDL_STUB_CLASSNAME
	{
		Pool*			m_pool;

		IDirectApp*		m_dapp;
		IRscManager*	m_rscMgr;
		SceneGraph*		m_sg;

		Viewport*		m_viewport;
		Frame*			m_camFrameA[2];
		Camera*			m_camCore;
		Box3			m_sceneBBox;

		Handle*			m_cPick;

		IDirectViewer::ViewMode	m_mode;

		Vec2			m_viewSize;

		Time			m_t0, m_lastTime;

		bool Init ( Pool* p, AnyList* o )
		{
			m_pool	 = p;
			SU_ASSERT( m_pool );

			m_dapp	 = NULL;
			m_rscMgr = NULL;
			m_sg	 = NULL;

			m_viewport		= NULL;
			m_camFrameA[0]	= NULL;
			m_camFrameA[1]	= NULL;
			m_camCore		= NULL;
			Box3Unit( &m_sceneBBox );

			m_cPick			= NULL;

			return true;
		}


		void Shut ( )
		{
			Unrealize();
		}


		bool Realize ( IDirectApp* inDApp, IRscManager* inRscMgr, IDirectViewer::ViewMode inMode )
		{
			Unrealize();

			SU_REQUIRE_OR_FALSE( inDApp );
			SU_REQUIRE_OR_FALSE( inRscMgr );

			m_dapp = inDApp;
			m_dapp->Retain();

			m_rscMgr = inRscMgr;
			m_rscMgr->Retain();

			m_sg = SceneGraph::Create( m_pool, m_dapp->GetDirectDraw() );
			SU_ASSERT( m_sg );

			m_mode = inMode;

			
			m_sg->SetView( inDApp->GetScreenView(), inDApp->GetScreenViewSize() );

			// Create cam
			if( m_mode == IDirectViewer::VM_CameraAuto )
			{
				m_sg->SetRenderMode( RM_Render );

				Handle* hdf = Handle::Create( m_sg->GetRoot() );
				Handle* hd  = Handle::Create( hdf );

				m_camFrameA[0] = Frame::Create( hdf );
				m_camCore	   = Camera::Create( hd );

				hd->Release();
				hdf->Release();
			}
			else if( m_mode == IDirectViewer::VM_Authoring )
			{
				m_sg->SetRenderMode( RM_Edition );

				Handle* hdff = Handle::Create( m_sg->GetRoot() );
				Handle* hdf  = Handle::Create( hdff );
				

				m_camFrameA[0]	= Frame::Create( hdff );	// rot
				m_camFrameA[1]	= Frame::Create( hdf );		// trans
				
				
		/*		
			#if defined( SU_HW_IOS ) || defined( SU_HW_ANDROID )
				Handle* hdc  = Handle::Create( hdf );
				Handle* hd	= Handle::Create( hdc );
				
				Frame* nf = Frame::Create( hdc );
				nf->SetRotation( Quat( HalfPi, Vec3::UNIT_Z ) );
			#else
		*/		Handle* hd	= Handle::Create( hdf );
		//	#endif
				
				
				m_camCore = Camera::Create( hd );
				hd->Release();
				
				hdf->Release();
				hdff->Release();
			}
			
			m_camCore->SetPerspective( Pi*0.4f, Vec2( 0.01f, 2.0f + 1.0f*3.0f ) );

			
			// Create viewport
			{
				Handle* hd = Handle::Create( m_sg->GetRoot() );
				m_viewport = Viewport::Create( hd );
				hd->Release();

				m_viewSize = m_dapp->GetScreenViewSize();

				m_viewport->SetCamera( m_camCore );
				m_viewport->SetRect( Vec4( 0.0f, 0.0f, m_viewSize.x, m_viewSize.y ) );
				
				if( m_mode == IDirectViewer::VM_Authoring )
				{
					m_viewport->SetArcBallTarget( m_camFrameA[1] );
					m_viewport->SetHCubeTarget	( m_camFrameA[1] );
				}
			}

			clock::GetTime( &m_t0 );
			clock::GetTime( &m_lastTime );

			return true;
		}


		void Unrealize ( )
		{
			m_cPick = NULL;

			SafeRelease( m_viewport );
			SafeRelease( m_camCore );
			SafeRelease( m_camFrameA[0] );
			SafeRelease( m_camFrameA[1] );

			SafeRelease( m_sg );
			SafeRelease( m_rscMgr );
			SafeRelease( m_dapp );
		}


		bool	AddCollection	(		uint	inRid	)
		{
			IRscManager::Resource r;
			bool bFetch = m_rscMgr->FetchResource( inRid, r );
			if( !bFetch )
			{
				uint pl_ids[1] = { inRid };

				int stash = m_rscMgr->CreateStash( m_pool );
				int pl = m_rscMgr->EnqueuePrefetchList( stash, pl_ids, 1 );

				SU_REQUIRE_OR_FALSE( m_rscMgr->IsPendingPrefetchList( pl ) );

				while( m_rscMgr->IsBusyPrefetchList( pl ) )
				{
					m_rscMgr->Squeeze();
					m_rscMgr->Update();
				}
				bFetch = m_rscMgr->FetchResource( inRid, r );
			}

			SU_REQUIRE_OR_FALSE( bFetch );
			SU_REQUIRE_OR_FALSE( r.tid == map::rsc_collection::CSIGN );

			collection::Resource* colRsc = collection::create( m_rscMgr, inRid, m_pool );
			SU_REQUIRE_OR_FALSE( colRsc );

			bool bSucc = false;
			Handle* colHd = Handle::Create( m_sg->GetRoot() );
			Collection* colCore = Collection::Create( colHd );
			if( colCore )
			{
				bSucc = colCore->Realize( colRsc );
				colCore->Release();
			}
			colHd->Release();

			colRsc->release();

			if( !m_sg->BuildBBox( &m_sceneBBox, m_sg->GetRoot() ) )
			{
				Box3Unit( &m_sceneBBox );
			}

			if( m_mode == IDirectViewer::VM_Authoring )
			{
				float z = Vec3Norm( &(m_sceneBBox.length) ) * 0.25f;
				m_camFrameA[0]->SetPosition( Vec3( 0.0f, 0.0f, -z ) );

				m_camCore->SetPerspective( Pi*0.4f, Vec2( 0.01f, 2.0f + z*3.0f ) );
			}

			return true;
		}



		void	ShowGrid	(	bool	inOnOff	)
		{
			if( m_viewport )
				m_viewport->ShowGrid( inOnOff );
		}

		void	ShowWarp	(	bool	inOnOff	)
		{
			if( m_viewport )
				m_viewport->ShowWarp( inOnOff );
		}

		void	ShowCubeHelper	(	bool	inOnOff	)
		{
			if( m_viewport )
				m_viewport->ShowHCube( inOnOff );
		}

		void	ShowArcBall	(	bool	inOnOff	)
		{
			if( m_viewport )
				m_viewport->ShowArcBall( inOnOff );
		}

		void	_UpdateCamera	(		)
		{
			Time ctime;
			clock::GetTime( &ctime );

			float dt  = ctime - m_lastTime;
			float dt0 = ctime - m_t0;

			if( m_mode == IDirectViewer::VM_CameraAuto )
			{
				float z = Vec3Norm( &(m_sceneBBox.length) ) * 0.25f;

				Matrix camWorld;
				MatrixTranslation( &camWorld, 0.0f, 0.0f, -z );

				Quat q( dt0, Vec3::UNIT_Y );
				Quat qX( 0.3f, Vec3::UNIT_X );
				
				MatrixRotate	( &camWorld, &camWorld, &qX );
				MatrixRotate	( &camWorld, &camWorld, &q );
				MatrixTranslate	( &camWorld, &camWorld, &(m_sceneBBox.center) );
				
				m_camFrameA[0]->SetTR( camWorld );
				m_camCore->SetPerspective( Pi*0.4f, Vec2( 0.01f, 2.0f + z*3.0f ) );
			}
			else if( m_mode == IDirectViewer::VM_Authoring )
			{
				int evt = m_dapp->GetEvent();

				static bool onLeftClick = false;
				static Vec2 clickPos;

			#if defined( SU_HW_WINDOWS )
				static int lastX = m_dapp->event_mouse_x();
				static int lastY = m_dapp->event_mouse_y();
				int cX = m_dapp->event_mouse_x();
				int cY = m_dapp->event_mouse_y();
			#else
				static int lastX = m_dapp->event_touch_x( 0 );
				static int lastY = m_dapp->event_touch_y( 0 );
				int cX = m_dapp->event_touch_x( 0 );
				int cY = m_dapp->event_touch_y( 0 );
			#endif

				Vec2 cPos( cX, cY );
				bool mouseMove = ( ( lastX != cX ) || ( lastY != cY ) );

				if( evt == IDirectApp::E_MOUSE_DOWN || evt == IDirectApp::E_TOUCH_DOWN )
				{
				#if defined( SU_HW_WINDOWS )
					util::bitmask<8> bm = m_dapp->event_mouse_push();
				#else
					util::bitmask<10> bm = m_dapp->event_touch_push();
				#endif

					if( bm.test( 0 ) )
					{
						onLeftClick = true;
						clickPos = cPos;

						m_viewport->BeginArcBall( clickPos );
						Vec3 dir = m_viewport->PickHCubeTrigger( clickPos );
						m_viewport->MoveHCubeTo( dir );
					}
				}
				else if( evt == IDirectApp::E_MOUSE_UP || evt == IDirectApp::E_TOUCH_UP )
				{
				#if defined( SU_HW_WINDOWS )
					util::bitmask<8> bm = m_dapp->event_mouse_release();
				#else
					util::bitmask<10> bm = m_dapp->event_touch_release();
				#endif

					if( bm.test( 0 ) )
					{
                        Vec2 cDst = cPos - clickPos;
						if( Vec2Norm2( &cDst ) < 25.0f )
						{
							if( m_cPick )
							{
								((Geometry*)(m_cPick->GetCore()))->Select( false );
								m_cPick = NULL;
							}

							Handle* cPick = m_sg->Pick( m_viewport, cPos );
							if( cPick )
							{
								Core* cc = cPick->GetCore();
								if( cc && cc->IsType( Geometry::Type ) )
								{
									((Geometry*)cc)->Select( true );
									m_cPick = cPick;
								}
							}
						}
						m_viewport->EndArcBall( cPos );
						onLeftClick = false;
					}
				}

				if( onLeftClick && mouseMove )
				{
					m_viewport->MoveArcBall( cPos );
				}
				if( !onLeftClick && mouseMove )
				{
					int abIdx = m_viewport->PickArcBallTrigger( cPos );
					m_viewport->HighlightArcBallTrigger( abIdx );
				}
				if( mouseMove || m_viewport->IsHCubeMoving() )
				{
					Vec3 dir = m_viewport->PickHCubeTrigger( cPos );
					m_viewport->HighlightHCubeTrigger( dir );
				}

				lastX = cX;
				lastY = cY;
			}
			
			clock::GetTime( &m_lastTime );
		}

		
		void Update ( )
		{
			SU_REQUIRE_OR_RETURN( m_sg );
			SU_ASSERT( m_viewport );
			SU_ASSERT( m_camCore );

			_UpdateCamera();
			
			m_sg->Draw();
			m_dapp->Update();
		}

	};


}


