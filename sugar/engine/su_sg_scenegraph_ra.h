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


#if defined( SU_HW_IOS )
	#import <QCAR/QCAR.h>
	#import <QCAR/CameraDevice.h>
	#import <QCAR/Tracker.h>
	#import <QCAR/VideoBackgroundConfig.h>
	#import <QCAR/Renderer.h>
	#import <QCAR/Tool.h>
	#import <QCAR/Trackable.h>
	#import <UIKit/UIKit.h>
#endif


namespace
{
	bool    ra_init				(	Pool*			inPool,
									IDirectDraw*	inDD	);
	void    ra_shut				(							);
	void	ra_fill_background	(	vector<uint>*	ioCmdA	);
	void	ra_get_viewproj		(	Matrix*			outView,
									Matrix*			outProj	);

#if defined( SU_HW_IOS )
	
	Matrix  g_camProjection;
	Matrix  g_camView;
    bool    g_trackLoaded = false;

	struct DrawCamBack_Struct: public dd::UserFunc
	{
		uint* Action ( uint* ip, uint opcode, void* ctxt )
		{
			QCAR::State state = QCAR::Renderer::getInstance().begin();
        
			for( int i = 0; i < state.getNumActiveTrackables(); ++i )
			{
			    const QCAR::Trackable* trackable = state.getActiveTrackable(i);
            
			    const QCAR::Matrix34F& pose34 = trackable->getPose();
			    QCAR::Matrix44F modelView = QCAR::Tool::convertPose2GLMatrix( pose34 );
            
			    MatrixLoad( &g_camView, modelView.data );
            
			    Vec3 tmpY = *( g_camView.up() );
			    *(g_camView.up()) = *(g_camView.front());
			    *(g_camView.front()) = tmpY;
			}
			QCAR::Renderer::getInstance().end();
			return ip+1;
	    }
	};
	
	DrawCamBack_Struct g_drawCamBack;



	struct req_createcam_p
	{
		req_createcam_p(){ bSuccess = false; }
		bool bSuccess;
	};

	void req_createcam_f( void* inP )
	{
		req_createcam_p* crcam = (req_createcam_p*)inP;
		// Get the device screen dimensions
		CGRect sRect = [[UIScreen mainScreen] bounds];
    
        if( globals::ios_haveretina && globals::ios_enablehd )
        {
            sRect.size.width *= 2.0f;
            sRect.size.height *= 2.0f;
        }
        
		QCAR::onSurfaceCreated();
		QCAR::onSurfaceChanged( sRect.size.height, sRect.size.width );
    
        
		// Background thread must have its own autorelease pool
		NSAutoreleasePool* nspool = [[NSAutoreleasePool alloc] init];
		QCAR::setInitParameters( QCAR::GL_20 );
    
		int nPercentComplete = 0;
    
		do
		{
			nPercentComplete = QCAR::init();
		}
        while( 0 <= nPercentComplete && 100 > nPercentComplete );
    
		NSLog( @"QCAR::init percent: %d", nPercentComplete );
    
		if( nPercentComplete < 0 )
		{
			return;
		}
    
		[nspool release];
    
    
        // Load the tracker data
        if( !g_trackLoaded )
        {
            NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
            nPercentComplete = 0;
            
            do
            {
                nPercentComplete = QCAR::Tracker::getInstance().load();
            }
            while( 0 <= nPercentComplete && 100 > nPercentComplete );
            
            [pool release];
            if( 0 > nPercentComplete )
            {
                return;
            }
            g_trackLoaded = true;
        }
        
    
    
		QCAR::setHint( QCAR::HINT_IMAGE_TARGET_MULTI_FRAME_ENABLED, 1 );
		QCAR::setHint( QCAR::HINT_IMAGE_TARGET_MILLISECONDS_PER_MULTI_FRAME, 25 );
    
		if( QCAR::CameraDevice::getInstance().init() )
        {
			// Configure video background
            
            QCAR::CameraDevice::MODE camMode = QCAR::CameraDevice::MODE_DEFAULT;
            
            if( globals::camgrabquality == 0 )
            {
                camMode = QCAR::CameraDevice::MODE_OPTIMIZE_SPEED;
            }
            else if( globals::camgrabquality == 2 )
            {
                camMode = QCAR::CameraDevice::MODE_OPTIMIZE_QUALITY;
            }
            
            // Select the video mode
			QCAR::CameraDevice& cameraDevice = QCAR::CameraDevice::getInstance();
            
            if( QCAR::CameraDevice::getInstance().selectVideoMode( camMode ) )
			{
                QCAR::VideoMode videoMode = cameraDevice.getVideoMode( camMode );
                
                // Configure the video background
                QCAR::VideoBackgroundConfig config;
                config.mEnabled = true;
                config.mSynchronous = true;
                config.mPosition.data[0] = 0.0f;
                config.mPosition.data[1] = 0.0f;
                
                // Compare aspect ratios of video and screen.  If they are different
                // we use the full screen size while maintaining the video's aspect
                // ratio, which naturally entails some cropping of the video.
                // Note - screenRect is portrait but videoMode is always landscape,
                // which is why "width" and "height" appear to be reversed.
                float arVideo = (float)videoMode.mWidth / (float)videoMode.mHeight;
                float arScreen = sRect.size.height / sRect.size.width;
                
                if( arVideo > arScreen )
                {
                    // Video mode is wider than the screen.  We'll crop the left and right edges of the video
                    config.mSize.data[0] = (int)sRect.size.width * arVideo;
                    config.mSize.data[1] = (int)sRect.size.width;
                }
                else
                {
                    // Video mode is taller than the screen.  We'll crop the top and bottom edges of the video.
                    // Also used when aspect ratios match (no cropping).
                    config.mSize.data[0] = (int)sRect.size.height;
                    config.mSize.data[1] = (int)sRect.size.height / arVideo;
                }
                
                // Set the config
                QCAR::Renderer::getInstance().setVideoBackgroundConfig(config);
                
                // end config
                
                
				if( QCAR::CameraDevice::getInstance().start() )
				{
					// Start the tracker
					QCAR::Tracker::getInstance().start();
                
					// Cache the projection matrix
					const QCAR::CameraCalibration& cameraCalibration = QCAR::Tracker::getInstance().getCameraCalibration();
                
					QCAR::Matrix44F proj = QCAR::Tool::getProjectionGL( cameraCalibration, 0.02f, 500.0f );
                
					MatrixLoad( &g_camProjection, proj.data );
					crcam->bSuccess = true;
				}
			}
		}
	}
    
    void req_releasecam_f( void* inP )
    {
		QCAR::Tracker::getInstance().stop();
		QCAR::CameraDevice::getInstance().stop();
		QCAR::CameraDevice::getInstance().deinit();
		QCAR::deinit();
	}
	
	bool    ra_init				(	Pool*			inPool,
									IDirectDraw*	inDD	)
	{
		req_createcam_p param;
		globals::ios_appdeleg_call( req_createcam_f, (void*)&param );
		return param.bSuccess;
	}
	void    ra_shut()
	{
		globals::ios_appdeleg_call( req_releasecam_f, NULL );
	}

	void	ra_fill_background	(	vector<uint>*	ioCmdA	)
	{
		ioCmdA->push_back( SU_DD_FUNC_1( 0, 1 ) );
		ioCmdA->push_back( SU_DD_FUNC_n( &g_drawCamBack ) );
            
		ioCmdA->push_back( SU_DD_TRAP( 0, 0 ) );
		ioCmdA->push_back( SU_DD_RENDERSTATE_1( 2, 0 ) );
		ioCmdA->push_back( SU_DD_RS_CULLFACE( dd::CM_FRONT ) );
		ioCmdA->push_back( SU_DD_RS_ZTEST_STATE( 1 ) );
	}

	void	ra_get_viewproj		(	Matrix*			outView,
									Matrix*			outProj	)
	{
		if( outView )	*outView = g_camView;
		if( outProj )	*outProj = g_camProjection;
	}
#elif defined( SU_HW_WINDOWS )

	Pool*					pool = NULL;
	IImageAnalyzer*			imgAnalyzer = NULL;
	IDirectDraw*			dd = NULL;
	dd::CameraDescriptor	camDesc;
	byte*					camRgbData = NULL;
	uint					backPbiRA = dd::INVALID_IDX;
	uint					backQuad = dd::INVALID_IDX;
	bool					raIsRealized = false;

	bool    ra_init				(	Pool*			inPool,
									IDirectDraw*	inDD	)
	{
		SU_ASSERT( !imgAnalyzer );
		SU_ASSERT( !pool );
		SU_ASSERT( !camRgbData );
		SU_ASSERT( backPbiRA == dd::INVALID_IDX );

		pool= inPool;
		dd	= inDD;

		imgAnalyzer = factory::CreateObject<IImageAnalyzer>( uid::MImageAnalyzer, pool );
		SU_ASSERT( imgAnalyzer );
		if( imgAnalyzer )
		{
			uint camInitCmd[] =
			{
				SU_DD_CAMERA_1( dd::CAM_PREPARE ),
				SU_DD_CAMERA_2( &camDesc ),
				SU_DD_END(  ),
			};
			dd->StartAndWaitCmdList( camInitCmd );

		//	SU_STDOUT( LString( "Camera : %%x%%\n" ).argl( camDesc.width ).argl( camDesc.height ) );
			if( camDesc.data && camDesc.width && camDesc.height )
			{
				camRgbData = (byte*)pool->Alloc( camDesc.width * camDesc.height * 3 );
				SU_ASSERT( camRgbData );

				if( camRgbData )
				{
					uint pbiW = Pow( 2, util::GetCeilPow2( camDesc.width ) );
					uint pbiH = Pow( 2, util::GetCeilPow2( camDesc.height ) );
	
					uint camPbiCmd[] = 
					{
						SU_DD_PCREATE_1( pbiW>>4, pbiH>>4, 1, 1, 0, 0, 0 ),
						SU_DD_PCREATE_2( &backPbiRA ),
						SU_DD_END()
					};
					dd->StartAndWaitCmdList( camPbiCmd );


					float p2w = Pow( 2, util::GetCeilPow2( camDesc.width ) );
					float p2h = Pow( 2, util::GetCeilPow2( camDesc.height ) );
					backQuad = create_vbquad( inDD, camDesc.width/p2w, camDesc.height/p2h, false );

					dd->Retain();

					raIsRealized = true;
					return true;
				}

				uint camEndCmd[] =
				{
					SU_DD_CAMERA_1( dd::CAM_OFF ),
					SU_DD_CAMERA_2( &camDesc ),
					SU_DD_END(  )
				};
				dd->StartAndWaitCmdList( camEndCmd );
			}

			imgAnalyzer->Release();
			imgAnalyzer = NULL;
		}
		dd = NULL;
		pool = NULL;
		return false;
	}

	void    ra_shut				(							)
	{
		if( raIsRealized )
		{
			pool->Free( camRgbData );
			camRgbData = NULL;

			SU_ASSERT( imgAnalyzer );
			imgAnalyzer->Release();
			imgAnalyzer = NULL;
         
			uint camEndCmd[] =
			{
				SU_DD_CAMERA_1( dd::CAM_OFF ),
				SU_DD_CAMERA_2( &camDesc ),

				SU_DD_END(  ),
			};
			dd->StartAndWaitCmdList( camEndCmd );
		
			dd->ResourceRelease( backPbiRA );
			dd->ResourceRelease( backQuad );
			backPbiRA = dd::INVALID_IDX;
			backQuad  = dd::INVALID_IDX;

			dd->Release();
			dd = NULL;

			pool = NULL;
		}
	}

	void	ra_fill_background	(	vector<uint>*	ioCmdA	)
	{
	//	BackQuad* bq = get_quad_back( &backQuadA, bck->width, bck->height, ddraw );
		SU_ASSERT( raIsRealized );
		
		ioCmdA->push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
		ioCmdA->push_back( SU_DD_RS_ZWRITE( 0 ) );
				
		setup_constant( ioCmdA, MatrixIdentity( new_matrix() ), "g_wvp" );
		ioCmdA->push_back( SU_DD_TEXTURE	( backPbiRA, 0, 0 ) );
		ioCmdA->push_back( SU_DD_PRIMIMM_1	( backQuad ) );
		ioCmdA->push_back( SU_DD_PRIMIMM_2	( dd::DP_Texture ) );
		ioCmdA->push_back( SU_DD_TEXTURE	( dd::INVALID_IDX, 0, 0 ) );
				
		ioCmdA->push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
		ioCmdA->push_back( SU_DD_RS_ZWRITE( 1 ) );
		
	}


	void	get_camera_image_rgb	(	byte*					ioRGBData,
										dd::CameraDescriptor	inDesc		)
	{
		byte* pIn = (byte*)inDesc.data;
		byte* pOu = ioRGBData;

		const int w = inDesc.width;
		const int h = inDesc.height;
		const int pixCount = w * h;

		bool bDone = false;

		if( inDesc.psm == PSM_B8_G8_R8 )
		{
			if( inDesc.origin == 0 )
			{
				if( inDesc.yFlipped )
				{
					for( int y=0; y<h; y++ )
					{
						byte* pInLine = pIn + ( (w*3) * (h-1-y));
						for( int x=0; x<w; x++ )
						{
							*pOu++ = pInLine[2];
							*pOu++ = pInLine[1];
							*pOu++ = pInLine[0];
							pInLine+=3;
						}
					}
				}
				else
				{
					byte* pOue = pOu + ( pixCount * 3 );
					while( pOu < pOue )
					{
						*pOu++ = pIn[2];
						*pOu++ = pIn[1];
						*pOu++ = pIn[0];
						pIn += 3;
					}
				}
				bDone = true;
			}
		}
		SU_ASSERT( bDone );	// else TODO
	}

	void	ra_get_viewproj		(	Matrix*			outView,
									Matrix*			outProj	)
	{
		SU_ASSERT( raIsRealized );

		uint camUpdCmd[] =
		{
			SU_DD_CAMERA_1	( dd::CAM_GRAB ),
			SU_DD_CAMERA_2	( &camDesc ),
			SU_DD_END		( )
		};
		dd->StartAndWaitCmdList( camUpdCmd );

		get_camera_image_rgb( camRgbData, camDesc );

		imgAnalyzer->Analyze( outView, outProj, camRgbData, camDesc.width, camDesc.height, 770.0f );

		uint texUpdCmd[] =
		{
			SU_DD_PUPDATE_1	( backPbiRA, PSM_R8_G8_B8, 0 ),
			SU_DD_PUPDATE_2	( 0, 0 ),
			SU_DD_PUPDATE_3	( camDesc.width, camDesc.height ),
			SU_DD_PUPDATE_4	( camRgbData ),
			SU_DD_END		( )
		};
		dd->StartAndWaitCmdList( texUpdCmd );
	}
#else
	bool    ra_init				(							)
	{
		return false;
	}
	void    ra_shut				(							)
	{}

	void	ra_fill_background	(	vector<uint>*	ioCmdA	)
	{}

	void	ra_get_viewproj		(	Matrix*			outView,
									Matrix*			outProj	)
	{}
#endif


}

