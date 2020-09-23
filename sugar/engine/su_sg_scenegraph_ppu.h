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



namespace
{
	void	ppu_init		(	SceneGraph::PostProcessUtil*	ioPPU,
								Pool*							inPool,
								IDirectDraw*					inDDraw			);
	void	ppu_realize		(	SceneGraph::PostProcessUtil*	ioPPU,
								uint							inW,
								uint							inH				);
	void	ppu_unrealize	(	SceneGraph::PostProcessUtil*	ioPPU			);
	void	ppu_shut		(	SceneGraph::PostProcessUtil*	ioPPU			);


	void	build_blur		(	uint*							inCmdAddr,
								uint							inBSize,
								uint*							outLastPbi,
								float*							outLastBlend,
								SceneGraph::PostProcessUtil*	inPPU,
								uint							inPbiToBlur,
								uint							inQuadFS,
								float							inRadius		);






	
	void	ppu_init		(	SceneGraph::PostProcessUtil*	ioPPU,
								Pool*							inPool,
								IDirectDraw*					inDDraw			)
	{
		ioPPU->pool = inPool;
		ioPPU->dd = inDDraw;
		ioPPU->dd->Retain();

		ioPPU->w = 0;
		ioPPU->h = 0;

		ioPPU->mipA = NULL;
		ioPPU->mipCount = 0;
	}
	
	void	ppu_realize		(	SceneGraph::PostProcessUtil*	ioPPU,
								uint							inW,
								uint							inH			)
	{
		SU_REQUIRE_OR_RETURN( ( inW != ioPPU->w ) || ( inH != ioPPU->h ) );
		ppu_unrealize( ioPPU );

		ioPPU->w = inW;
		ioPPU->h = inH;

		SU_ASSERT( ioPPU->dd );

		// Create all possible mipmap
		ioPPU->mipCount = util::GetCeilPow2( inW < inH ? inW : inH ) - 4;
		ioPPU->mipA = ioPPU->pool->Array<SceneGraph::PostProcessUtil::Mip>( ioPPU->mipCount );

		uint cw = inW;
		uint ch = inH;
		for( uint i=0; i<ioPPU->mipCount; i++ )
		{
			cw >>= 1;
			ch >>= 1;

			uint nrt = dd::INVALID_IDX;
			uint cmdA[] =
			{
				SU_DD_PCREATE_1( cw>>4, ch>>4, 1, 1, 1, 0, 0 ),
				SU_DD_PCREATE_2( &nrt ),
				SU_DD_END()
			};
			ioPPU->dd->StartAndWaitCmdList( cmdA );
			SU_ASSERT( nrt != dd::INVALID_IDX );

			ioPPU->mipA[i].rt	= nrt;
			ioPPU->mipA[i].w	= cw;
			ioPPU->mipA[i].h	= ch;
		}
	}
	
	void	ppu_unrealize	(	SceneGraph::PostProcessUtil*	ioPPU			)
	{
		if( ioPPU->dd )
		{
			if( ioPPU->mipA )
			{
				for( uint i=0; i<ioPPU->mipCount; i++ )
					ioPPU->dd->ResourceRelease( ioPPU->mipA[i].rt );
				ioPPU->pool->Free( ioPPU->mipA );
				ioPPU->mipA = NULL;
			}
		}
	}
	void	ppu_shut		(	SceneGraph::PostProcessUtil*	ioPPU			)
	{
		ppu_unrealize( ioPPU );
		SafeRelease( ioPPU->dd );
	}


	void	build_blur		(	uint*							inCmdAddr,
								uint							inBSize,
								uint*							outLastPbi,
								float*							outLastBlend,
								SceneGraph::PostProcessUtil*	inPPU,
								uint							inPbiToBlur,
								uint							inQuadFS,
								float							inRadius		)
	{
		float blendA[16];
		util::Zero( blendA );
		int mipUsed = Compute_MipmapedGaussConvol( blendA, inPPU->w, inPPU->h, inRadius );
		SU_ASSERT( mipUsed <= inPPU->mipCount );
		SU_ASSERT( mipUsed > 0 );


		MCursor crs( inCmdAddr, inBSize );

		crs.u32 << SU_DD_CONSTANT_1( 4, 0, 4, 0 );
		crs.u32 << SU_DD_CONSTANT_2( util::hmac_crc32_w( "g_wvp" ) );
		float* fltA = (float*)&(Matrix::UNIT);
		for( uint i=0; i<16; i++ )
		{
			crs.u32 << SU_DD_CONSTANT_n( fltA[i] );
		}

        
		//
		// --- Mipmap

		for( int i=0; i<mipUsed; i++ )
		{
			uint cTgt = inPPU->mipA[i].rt;
			uint cTex = ( i == 0 ) ? inPbiToBlur : inPPU->mipA[i-1].rt;

			crs.u32 << SU_DD_TARGET		( cTgt );
			crs.u32 << SU_DD_TEXTURE	( cTex, 0, 0 );
			crs.u32 << SU_DD_PRIMIMM_1	( inQuadFS );
			crs.u32 << SU_DD_PRIMIMM_2	( dd::DP_Texture );
		}

		// --- Blend

		crs.u32 << SU_DD_RENDERSTATE_1( 1, 0 );
		crs.u32 << SU_DD_RS_BLEND( dd::BM_Sc_x_Sa_add_Dc_x_ReverseSa );
		
		for( int i=(mipUsed-2); i>=0; i-- )
		{
			uint cTgt	= inPPU->mipA[i].rt;
			uint cTex	= inPPU->mipA[i+1].rt;
			float alpha	= blendA[i+2];				// blend[0] = blend main mbi

			crs.u32 << SU_DD_TARGET		( cTgt );
			crs.u32 << SU_DD_TEXTURE	( cTex, 0, 0 );
			crs.u32 << SU_DD_CONSTANT_1	( 1, 0, 1, 0 );
			crs.u32 << SU_DD_CONSTANT_2	( util::hmac_crc32_w( "alpha" ) );
			crs.u32 << SU_DD_CONSTANT_n	( alpha );
			crs.u32 << SU_DD_PRIMIMM_1	( inQuadFS );
			crs.u32 << SU_DD_PRIMIMM_2	( dd::DP_TextureSetAlpha );
		}
		
		crs.u32 << SU_DD_RENDERSTATE_1( 1, 0 );
		crs.u32 << SU_DD_RS_BLEND( dd::BM_Disabled );
        
		crs.u32 << SU_DD_TEXTURE	( dd::INVALID_IDX, 0, 0 );
		
		crs.u32 << SU_DD_RET();


		*outLastPbi = inPPU->mipA[0].rt;
		*outLastBlend = blendA[1];
	}



}