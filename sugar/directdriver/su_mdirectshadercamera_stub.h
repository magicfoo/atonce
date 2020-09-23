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

#define TIME_OUT		2.0f			// Time out in second [OnStart-]


namespace
{
	SU_DD_ACT_DECL( camera );
	
	SU_DD_CMD_DESC ddcmd[] =
	{
		{ SU_DD_CAMERA_CODE, SU_DD_ACT_NAME(camera) },
	};
	
	
	struct SIDL_STUB_CLASSNAME : public SIDL_STUB_SUPERNAME
	{
		typedef SIDL_STUB_SUPERNAME abstract;

		Pool*						m_pool;
		ICameraGrabber*				m_camGrabber;
		ICameraGrabber::Descriptor	m_desc;

		bool				m_isPrepared;

		Time				m_tLastCall;
		

		bool Init ( Pool* p, AnyList* o )
		{
			IDirectDraw* hal;
			SU_REQUIRE_OR_FALSE( abstract::Init(hal,p,o,ddcmd,sizeof(ddcmd)) );
			
			m_pool = p;

			m_camGrabber = NULL;
			util::Zero( &m_desc, sizeof( ICameraGrabber::Descriptor ) );
			
			m_isPrepared = false;
			
			return true;
		}

		void	OnInitResources()
		{}

		void Shut ( )
		{
			Off();
			
			abstract::Shut();
			m_pool = NULL;
		}


		inline void ResetTimer	(		)
		{
			su::clock::GetTime( &m_tLastCall );
		}

		void OnStart (  )
		{
			if( m_isPrepared )
			{
				Time now;
				su::clock::GetTime( &now );
				if( now - m_tLastCall > TIME_OUT )
				{
					Off();
				}
			}
		}

		bool	Prepare	(			)
		{
			if( m_isPrepared )
			{
				SU_ASSERT( m_camGrabber );
				return true;
			}

			SU_ASSERT( !m_camGrabber );

			m_camGrabber = factory::CreateObject<ICameraGrabber>( globals::camgrabber, m_pool );
			SU_ASSERT( m_camGrabber );
			SU_REQUIRE_OR_FALSE( m_camGrabber );

			if( !m_camGrabber->Prepare( m_pool ) )
			{
				SafeRelease( m_camGrabber );
				return false;
			}

			m_camGrabber->Grab( &m_desc );

			m_isPrepared = true;

			m_camGrabber->Start();
			ResetTimer();

			return true;
		}

		void Off (	)
		{
			SU_REQUIRE_OR_RETURN( m_isPrepared );
			SU_ASSERT( m_camGrabber );

			m_camGrabber->Stop();
			m_camGrabber->UnPrepare();
			SafeRelease( m_camGrabber );		// ?

			util::Zero( &m_desc, sizeof( ICameraGrabber::Descriptor ) );

			m_isPrepared = false;
		}

		bool	Grab()
		{
			Prepare();
			SU_REQUIRE_OR_FALSE( m_isPrepared );

			SU_ASSERT( m_camGrabber );

			return m_camGrabber->Grab( &m_desc );
		}

		void GetCamDesc( dd::CameraDescriptor* cdesc )
		{
			SU_REQUIRE_OR_RETURN( cdesc );

			cdesc->origin	= m_desc.origin;
			cdesc->psm		= m_desc.psm;
			cdesc->width	= m_desc.width;
			cdesc->height	= m_desc.height;
			cdesc->data		= m_desc.data;
			cdesc->yFlipped	= m_desc.yFlipped;
		}
	};




	SU_DD_ACT_DEF( camera, 3, SU_DD_CAMERA_REV )
	{
		byte	mode	= SU_DD_UI( opcode, 0, 8 );

		dd::CameraDescriptor* cdesc = (dd::CameraDescriptor*)ip[1];

		SIDL_STUB_CLASSNAME* shd = (SIDL_STUB_CLASSNAME*) udata;
		SU_REQUIRE_OR_NULL( shd );

		shd->ResetTimer();

		bool ok = false;

		if( mode == 0 )
		{
			shd->Off();
			ok = true;
		}
		else if( mode == 1 )
		{
			ok = shd->Prepare();
		}
		else if( mode == 2 )
		{
			ok = shd->Grab();
			ok = true;
		}

		shd->GetCamDesc( cdesc );

		return ok ? ip+2: NULL;
	}


}


