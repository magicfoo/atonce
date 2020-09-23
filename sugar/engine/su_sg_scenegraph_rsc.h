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

#include "noise_tga.h"


namespace
{

	namespace CubeHelperData
	{
		#include "cubeHelper_obj.h"
		static byte texA[] =		// tga : working on all platform
		{
			#include "cubeHelper_tga.h"
		};
	}


    /* All
	static const char glsl_post_effect_default_code[] =
	"	#ifdef GL_ES														\n"
	"		precision highp float;											\n"
	"		precision lowp sampler2D;										\n"
	"	#endif																\n"
	"																		\n"
	"	/*																	\n"
	"	SU_UNIFORM( s0 sampler )											\n"
	"	SU_UNIFORM( s1 sampler )											\n"
	"	SU_UNIFORM( time 1 1 1 )											\n"
	"	SU_UNIFORM( twinkleFact 1 1 1 )										\n"
	"	SU_UNIFORM( screenSize 2 1 1 )										\n"
	"	SU_UNIFORM( blurMipBlend 1 1 1 )									\n"
	"	SU_UNIFORM( blurBlend 1 1 1 )										\n"
	"	/																	\n"
	"																		\n"
	"	uniform sampler2D s0;												\n"
	"	uniform sampler2D s1;												\n"
	"	uniform float time;													\n"
	"	uniform float twinkleFact;											\n"
	"	uniform vec2 screenSize;											\n"
	"	uniform float blurMipBlend;											\n"
	"	uniform float blurBlend;											\n"
	"																		\n"
	"	varying	vec2 outUV0;												\n"
	"																		\n"
	"	void main()															\n"
	"	{																	\n"
    "       highp vec2 uv = 0.5 + (outUV0-0.5)*(0.97 + 0.03*sin(0.3*time));	\n"
	"		float co_circle = 0.4 + 19.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y);	\n"
	"		float co_line = 1.0 + 0.1*sin( 3.1415 * screenSize.y * uv.y );	\n"
	"		float co_glob = co_circle * co_line * twinkleFact;				\n"
	"		vec3 cf = vec3( 0.7, 0.8, 1.0 ) * co_glob;                      \n"
	"																		\n"
	"																		\n"
	"		vec3 colClean = texture2D( s0, uv ).xyz;                        \n"
	"		vec3 colMipBlur  = texture2D( s1, uv ).xyz;                     \n"
	"	//	vec3 colBlur = mix( colClean, colMipBlur, blurMipBlend );		\n"
	"		vec3 colScene = mix( colClean, colMipBlur, blurBlend );         \n"
	"																		\n"
	"		gl_FragColor = vec4( colScene * cf, 1.0 );                      \n"
	"	}																	\n"
	"																		\n";
    */
    
    /* blur test
    static const char glsl_post_effect_default_code[] =
	"	#ifdef GL_ES														\n"
	"		precision lowp float;											\n"
	"		precision lowp sampler2D;										\n"
	"	#endif																\n"
	"																		\n"
	"	/*																	\n"
	"	SU_UNIFORM( s0 sampler )											\n"
	"	SU_UNIFORM( s1 sampler )											\n"
	"	SU_UNIFORM( blurBlend 1 1 1 )										\n"
	"	/																	\n"
	"																		\n"
	"	uniform sampler2D s0;												\n"
	"	uniform sampler2D s1;												\n"
	"	uniform float blurBlend;											\n"
	"																		\n"
	"	varying	highp vec2 outUV0;											\n"
	"																		\n"
	"	void main()															\n"
	"	{																	\n"
	"		vec3 colClean = texture2D( s0, outUV0 ).xyz;                    \n"
	"		vec3 colMipBlur  = texture2D( s1, outUV0 ).xyz;                 \n"
	"		vec3 colScene = mix( colClean, colMipBlur, blurBlend );         \n"
	"		gl_FragColor = vec4( colScene, 1.0 );                           \n"
	"	}																	\n"
	"																		\n";
	*/
    
    // tv only
    static const char glsl_post_effect_default_code[] =
	"	#ifdef GL_ES														\n"
	"		precision highp float;											\n"
	"		precision lowp sampler2D;										\n"
	"	#endif																\n"
	"																		\n"
	"	/*																	\n"
	"	SU_UNIFORM( s0 sampler )											\n"
	"	SU_UNIFORM( time 1 1 1 )											\n"
	"	SU_UNIFORM( twinkleFact 1 1 1 )										\n"
	"	SU_UNIFORM( screenSize 2 1 1 )										\n"
	"	*/																	\n"
	"																		\n"
	"	uniform sampler2D s0;												\n"
	"	uniform float time;													\n"
	"	uniform float twinkleFact;											\n"
	"	uniform vec2 screenSize;											\n"
	"																		\n"
	"	varying	vec2 outUV0;												\n"
	"																		\n"
	"	void main()															\n"
	"	{																	\n"
    "       highp vec2 uv = 0.5 + (outUV0-0.5)*(0.97 + 0.03*sin(0.3*time));	\n"
	"		float co_circle = 0.4 + 19.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y);	\n"
	"		float co_line = 1.0 + 0.1*sin( 3.1415 * screenSize.y * uv.y );	\n"
	"		float co_glob = co_circle * co_line * twinkleFact;				\n"
	"																		\n"
	"		vec3 colClean = texture2D( s0, uv ).xyz;                        \n"
	"																		\n"
	"		gl_FragColor = vec4( colClean * co_glob, 1.0 );                 \n"
	"	}																	\n"
	"																		\n";
	
	template<typename T>
	void swap( T* inA, T* inB )
	{
		T t = *inA;
		*inA = *inB;
		*inB = t;
	}


	bool	create_surface	(	uint*			outSrf,
								uint*			outVBO,			// Keep it if you want to update it later else give NULL
								void*			inVtxData,
								uint			inVtxCount,
								uint			inVtxFmt,
								uint			inProgId,
								dd::PrimType	inPType,
								IDirectDraw*	inDD			);

	Vec3	ray_direction	(	const Vec2&		inHomogeneous,
								const Matrix&	inProj,
								const Matrix&	inInvView		);
	
	bool	intersection	(	Vec3*			outIntersection,
								const Vec3&		inRayStart,
								const Vec3&		inRayDir,
								const Vec3&		inPlanPoint,
								const Vec3&		inPlanNormal	);


	bool	load_warp		(	uint*			ioSrf,
								const Vec3&		inTopCol,
								const Vec3&		inBotCol,
								IDirectDraw*	inDD			);
	

	bool	load_grid		(	uint*			ioSrf,
								IDirectDraw*	inDD			);

	
	bool	load_cube		(	uint*			ioSrf,
								uint*			ioPbi,
								Pool*			inPool,
								IDirectDraw*	inDD			);


	bool	get_grid_param	(	float*			outSize,
								float*			outA0,
								float*			outA1,
								const Vec3&		inPos,
								const Matrix&	inProj,
								const Matrix&	inInvView		);





	bool	create_surface	(	uint*			outSrf,
								uint*			outVBO,
								void*			inVtxData,
								uint			inVtxCount,
								uint			inVtxFmt,
								uint			inProgId,
								dd::PrimType	inPType,
								IDirectDraw*	inDD			)
	{
		int vbo = dd::INVALID_IDX;
		uint cmdVbo[] = 
		{
			SU_DD_VBOCREATE_1( inVtxFmt, inPType, 0 ),
			SU_DD_VBOCREATE_2( inVtxCount ),
			SU_DD_VBOCREATE_3( &vbo ),
			SU_DD_END()
		};

		inDD->StartAndWaitCmdList( cmdVbo );
		SU_REQUIRE_OR_FALSE( vbo != dd::INVALID_IDX );

		int sf = dd::INVALID_IDX;
		
		uint ddcmd[16];
		MCursor ddc( ddcmd, sizeof(ddcmd) );

		if( inVtxData )
		{
			ddc.u32 << SU_DD_VBOFILL_1( vbo );
			ddc.u32 << SU_DD_VBOFILL_2( inVtxFmt );
			ddc.u32 << SU_DD_VBOFILL_3( inVtxData );
		}

		ddc.u32 << SU_DD_SURFACE_1( 1, inProgId );
		ddc.u32 << SU_DD_SURFACE_2( &sf );
		ddc.u32 << SU_DD_SURFACE_n( vbo, 0 );

		ddc.u32 << SU_DD_END();

		SU_ASSERT( ddc.forward() );

		inDD->StartAndWaitCmdList( ddcmd );
		if( sf != dd::INVALID_IDX )
		{
			if( outVBO ) *outVBO = vbo;
			else		 inDD->ResourceRelease( vbo );

			*outSrf = sf;
			return true;
		}
		inDD->ResourceRelease( vbo );
		return false;
	}

	Vec3	ray_direction	(	const Vec2&		inHomogeneous,
								const Matrix&	inProj,
								const Matrix&	inInvView		)
	{
		Vec3 v( inHomogeneous.x/inProj.m11,
				inHomogeneous.y/inProj.m22,
				1.0f	);

		Vec3 dir;
		Vec3ApplyVector( &dir, &v, &inInvView );
		Vec3Normalize( &dir, &dir );
		return dir;
	}
	
	bool	intersection	(	Vec3*			outIntersection,
								const Vec3&		inRayStart,
								const Vec3&		inRayDir,
								const Vec3&		inPlanPoint,
								const Vec3&		inPlanNormal	)
		
	{
		float dot = Vec3Dot( &inRayDir, &inPlanNormal );
		SU_REQUIRE_OR_FALSE( dot != 0.0f );
		
		float d = - Vec3Dot( &inPlanPoint, &inPlanNormal );
		float k = -( d + Vec3Dot( &inRayStart, &inPlanNormal ) ) / dot;

		if( outIntersection )	*outIntersection = inRayStart + ( k * inRayDir );
		return true;
	}


	bool	load_warp		(	uint*			ioSrf,
								const Color32&	inTopCol,
								const Color32&	inBotCol,
								IDirectDraw*	inDD			)
	{
		SU_ASSERT( inDD );
		const int res = 16;

		const int vtxCount = (res-2) * res + 2;
		const int triCount = (res-1) * res * 2;
		const int idxCount = triCount * 3;

		dd::Vertex_L3_COL vtxA[vtxCount];

		float r = 2.0f;
		const float yThresold = r*0.15f;
		float incA = Pi / res;
		float incB = 2.0f * Pi / res;
		uint topCol = inDD->GetPackedColor( inTopCol.r, inTopCol.g, inTopCol.b );
		uint botCol = inDD->GetPackedColor( inBotCol.r, inBotCol.g, inBotCol.b );

		vtxA[0].loc = Vec3( 0, r, 0 );
		vtxA[0].c = topCol;
		int ci = 1;
		for( uint j=1; j<res-1; j++ )
		{
			float a = Pi*0.5f - j*incA;
			float y = Sin( a ) * r;
		
			uint col;
			if( y > 0.0f && y < yThresold )
			{
				Vec3 tc3( inTopCol.r, inTopCol.g, inTopCol.b );
				Vec3 bc3( inBotCol.r, inBotCol.g, inBotCol.b );

				float co = y / yThresold;
				Vec3 cc = ( bc3 + ( co * ( tc3 - bc3 ) ) ) / 255.0f;
				col = inDD->GetPackedColor( cc.x, cc.y, cc.z );
			}
			else
			{
				col = ( y <= 0.0f ) ? botCol : topCol;
			}
		
			for( uint i=0; i<res; i++ )
			{
				float b = i*incB;
				float x = Cos( a ) * Cos( b ) * r;
				float z = Cos( a ) * Sin( b ) * r;

				vtxA[ci].loc = Vec3( x, y, z );
				vtxA[ci].c = col;
				ci++;
			}
		}
		vtxA[ci].loc = Vec3( 0, -r, 0 );
		vtxA[ci].c = botCol;
		ci++;
		SU_ASSERT( ci == vtxCount );

		ci = 0;
		const uint quadCount = idxCount/6;
		const uint vtxStripCount = quadCount*4;
		dd::Vertex_L3_COL_K vtxStrA[vtxStripCount];

		for( uint j=0; j<res-1; j++ )
		{
			for( uint i=0; i<res; i++ )
			{
				short tplA[4];
				if( j==0 )
				{
					tplA[0] = i+1;
					tplA[1] = 0;
					tplA[2] = ( i == res-1 ) ? 1 : i+2;
					tplA[3] = 0;
				}
				else
				{
					short incNextX = ( i == res-1 ) ? -(res-1): 1;
					short i0 = 1 + ((j-1) * res) + i;
					tplA[0] = ( j == res-2 ) ? vtxCount-1 : i0+res;
					tplA[1] = i0;
					tplA[2] = ( j == res-2 ) ? vtxCount-1 : i0+res+incNextX;
					tplA[3] = i0+incNextX;
				}

				// i0, i1, i3, i2;
				for( uint k=0; k<4; k++ )
				{
					vtxStrA[ci+k].loc	= vtxA[tplA[k]].loc;
					vtxStrA[ci+k].c		= vtxA[tplA[k]].c;
					vtxStrA[ci+k].kick	= (k > 1);
				}
				ci += 4;
			}
		}
		SU_ASSERT( ci == vtxStripCount );
	
		return create_surface(	ioSrf,
								NULL,
								vtxStrA,
								vtxStripCount,
								dd::Vertex_L3_COL_K::fvf,
								dd::DP_VertexColor,
								dd::PRIM_TRIANGLE,
								inDD					);
	}


	bool	load_grid		(	uint*			ioSrf,
								IDirectDraw*	inDD			)
	{
		SU_REQUIRE_OR_FALSE( inDD );
		dd::Vertex_L3_COL vtxA[44];

		const float s = 0.5f;
		const float inc = s*0.2f;
		uint col = inDD->GetPackedColor( 0.4f, 0.4f, 0.4f );
		uint lim = inDD->GetPackedColor( 0.6f, 0.6f, 0.6f );

		int ci = 0;
		for( int i=0; i<11; i++ )
		{
			float step = inc*i;
			uint c = (i%5==0) ? lim : col;

			vtxA[ci+0].loc = Vec3( -s + step, 0.0f, s );
			vtxA[ci+1].loc = Vec3( -s + step, 0.0f,-s );

			vtxA[ci+0].c = vtxA[ci+1].c = c;
			vtxA[ci+1].c = c;

			vtxA[ci+2].loc = Vec3( s, 0.0f,-s + step );
			vtxA[ci+3].loc = Vec3(-s, 0.0f,-s + step );

			vtxA[ci+2].c = c;
			vtxA[ci+3].c = c;

			ci += 4;
		}
		uint pg = dd::DP_VertexColorSetAlpha;
		return create_surface( ioSrf, NULL, vtxA, 44, vtxA->fvf, pg, dd::PRIM_LINE, inDD );
	}


	bool	get_grid_param	(	float*			outS0,
								float*			outS1,
								float*			outA0,
								float*			outA1,
								const Vec3&		inPos,
								const Matrix&	inProj,
								const Matrix&	inInvView		)
	{
		Vec3 camPos, camDir;
		MatrixGetTR( &camPos, NULL, NULL, &inInvView );
		MatrixGetAxis( NULL, NULL, &camDir, &inInvView );

		static const float gridStep = 5.0f;

		Vec3 d0 = ray_direction( Vec2( -1.0f, 0.0f ), inProj, inInvView );
		Vec3 d1 = ray_direction( Vec2(  1.0f, 0.0f ), inProj, inInvView );

		Vec3 p0, p1;
		SU_REQUIRE_OR_FALSE( intersection( &p0, camPos, d0, inPos, camDir ) );
		SU_REQUIRE_OR_FALSE( intersection( &p1, camPos, d1, inPos, camDir ) );

        Vec3 lt = p1-p0;
		float length = Vec3Norm( &lt );

		float gridMinSizeBase = length*0.11f;
		float gridMaxSizeBase = length*0.22f;

		float gmins = gridMinSizeBase;
		float ns = 0.1f;

		while( gmins > 0.1f )
		{
			gmins /= gridStep;
			ns *= gridStep;
		}
		*outS0 = ns;
		*outS1 = ns * gridStep;
		*outA0 = Clamp( 1.0f - gridMinSizeBase / ns, 0.0f, 1.0f );
		*outA1 = Clamp( gridMaxSizeBase / (*outS1),	 0.0f, 1.0f );

		return true;
	}

	
	
	bool	load_tga		(	uint*			ioPbi,
								void*			inData,
								uint			inBSize,
								Pool*			inPool,
								IDirectDraw*	inDD		)
	{
		uint  os = 1024*1024*4 + 100;
		void* od = inPool->Alloc( os );
		*ioPbi = dd::INVALID_IDX;
		MCursor i( inData, inBSize );
		MCursor o( od, os );
		if( ICodec::Convert( o, i, inPool, uid::MCodecTGA ) )
		{
			map::rsc_collection col;
			col.load( (byte*)od );
			col.tag_first();
			uint cmdLoadA[5] = 
			{
				SU_DD_PLOAD_1	(),
				SU_DD_PLOAD_2	( col.tag_addr() ),
				SU_DD_PLOAD_3	( ioPbi ),
				SU_DD_END		(	)
			};
			inDD->StartAndWaitCmdList( cmdLoadA );
		}
		inPool->Free( od );
		return ( *ioPbi != dd::INVALID_IDX );
	}

	bool	load_noise		(	uint*			ioPbi,
								Pool*			inPool,
								IDirectDraw*	inDD		)
	{
		return load_tga( ioPbi, NoiseA, sizeof( NoiseA ), inPool, inDD );
	}

	bool	load_cube		(	uint*			ioSrf,
								uint*			ioPbi,
								uint*			ioHLSrfA,
								uint*			ioHLVboA,
								Pool*			inPool,
								IDirectDraw*	inDD			)
	{
		SU_REQUIRE_OR_FALSE( inDD );

		// Load Texture
		load_tga( ioPbi, (void*)CubeHelperData::texA, sizeof( CubeHelperData::texA ), inPool, inDD );

		// Load Mesh
		Vec3* locA = (Vec3*)CubeHelperData::locations;
		Vec2* uv0A = (Vec2*)CubeHelperData::uvs;

		const uint locCount	 = sizeof(CubeHelperData::locations)/ (sizeof(float)*3);
		const uint uv0Count	 = sizeof(CubeHelperData::uvs)		/ (sizeof(float)*2);
		const uint faceCount = sizeof(CubeHelperData::faces)	/ (sizeof(int)*3);


		dd::Vertex_L3_UV0_K vtxA[faceCount];
		for( uint i=0; i<faceCount; i++ )
		{
			int ii = i;
			if( i%6 >= 3 )		// inverse one triangle on two
			{
				if( i%3 == 0 )			ii = i;
				else if( i%3 == 1 )		ii = i+1;
				else if( i%3 == 2 )		ii = i-1;
			}
			int locI = CubeHelperData::faces[(ii*3)+0];
			int uv0I = CubeHelperData::faces[(ii*3)+1];

			SU_ASSERT( locI >= 0 && locI < locCount );
			SU_ASSERT( uv0Count >= 0 && uv0I < uv0Count );
			
			vtxA[i].loc = locA[locI];
			vtxA[i].uv0 = uv0A[uv0I];

			// Swap Z :		loc.z = -z
			vtxA[i].loc.z = -vtxA[i].loc.z;
			
			// Inverse V :	v = 1 - v
			vtxA[i].uv0.y = 1.0f - vtxA[i].uv0.y;

			vtxA[i].kick = ( i%3 == 2 );
		}
		create_surface( ioSrf, NULL, vtxA, faceCount, dd::Vertex_L3_UV0_K::fvf, dd::DP_Texture, dd::PRIM_TRIANGLE, inDD );

		
		uint pick_fvf = dd::VF_LOC3 | dd::VF_COL;
		uint pick_prog = dd::DP_VertexColor;

		create_surface( ioHLSrfA+0,	ioHLVboA+0,	NULL, 4,  pick_fvf, pick_prog, dd::PRIM_TRIANGLE, inDD );	// face
		create_surface( ioHLSrfA+1, ioHLVboA+1, NULL, 6,  pick_fvf, pick_prog, dd::PRIM_TRIANGLE, inDD );	// edge
		create_surface( ioHLSrfA+2, ioHLVboA+2, NULL, 10, pick_fvf, pick_prog, dd::PRIM_TRIANGLE, inDD );	// vertex

		return true;
	}



	void	update_hlight_face			(	const Vec3&		inDir,
											uint			inVbo,
											IDirectDraw*	inDD			)
	{
		uint faceNum;
		if( inDir.x != 0.0f )		faceNum = ( inDir.x < 0.0f ) ? 0 : 3;
		else if( inDir.y != 0.0f )	faceNum = ( inDir.y < 0.0f ) ? 1 : 4;
		else if( inDir.z != 0.0f )	faceNum = ( inDir.z < 0.0f ) ? 2 : 5;

		bool bNeg = ( faceNum < 3 );
		uint fm3 = faceNum%3;
		float pos = 0.52f * ( bNeg ? -1.0f : 1.0f);
		static const float dec = 0.3f;

		Vec3 locA[4];
		if( fm3 == 0 )
		{
			locA[0] = Vec3( pos,-dec, dec );
			locA[1] = Vec3( pos,-dec,-dec );
			locA[2] = Vec3( pos, dec, dec );
			locA[3] = Vec3( pos, dec,-dec );
		}
		else if( fm3 == 1 )
		{
			locA[0] = Vec3(-dec, pos,-dec );
			locA[1] = Vec3(-dec, pos, dec );
			locA[2] = Vec3( dec, pos,-dec );
			locA[3] = Vec3( dec, pos, dec );
		}
		else
		{
			locA[0] = Vec3(-dec, dec, pos );
			locA[1] = Vec3(-dec,-dec, pos );
			locA[2] = Vec3( dec, dec, pos );
			locA[3] = Vec3( dec,-dec, pos );
		}

		byte tpl[4];
		if( !bNeg )	{	tpl[0] = 0;	tpl[1] = 1;	tpl[2] = 2;	tpl[3] = 3;	}
		else		{	tpl[0] = 1;	tpl[1] = 0;	tpl[2] = 3;	tpl[3] = 2;	}


		uint c = inDD->GetPackedColor( (byte)200, 200, 255, 100 );

		dd::Vertex_L3_COL vtxA[4];
		for( uint i=0; i<4; i++ )
		{
			vtxA[i].loc	 = locA[tpl[i]];
			vtxA[i].c	 = c;
		}

		uint cmdUpd[] =
		{
			SU_DD_VBOFILL_1( inVbo ),
			SU_DD_VBOFILL_2( dd::Vertex_L3_COL::fvf ),
			SU_DD_VBOFILL_3( vtxA ),
			SU_DD_END(  )
		};
		inDD->StartAndWaitCmdList( cmdUpd );
	}

	void	update_hlight_edge			(	const Vec3&		inDir,
											uint			inVbo,
											IDirectDraw*	inDD			)
	{
		Vec3 e0( inDir ), e1( inDir );
		if( inDir.x == 0.0f ) {
			e0.x =-0.5f;
			e1.x = 0.5f;
		}
		else if( inDir.y == 0.0f ) {
			e0.y =-0.5f;
			e1.y = 0.5f;
		}
		else {
			e0.z =-0.5f;
			e1.z = 0.5f;
		}

		uint c = inDD->GetPackedColor( (byte)200, 200, 255, 100 );
		Vec3 AB = e1 - e0;

		float oX = ( e0.x < 0.0f && e1.x < 0.0f ) ? 0.2f:-0.2f;
		float oY = ( e0.y < 0.0f && e1.y < 0.0f ) ? 0.2f:-0.2f;
		float oZ = ( e0.z < 0.0f && e1.z < 0.0f ) ? 0.2f:-0.2f;
			
		Vec3 s0 = e0 + AB*0.2f;
		Vec3 s1 = e0 + AB*0.8f;

		Vec3 locA[6];

		bool switchIdx = false;
		float ax = Abs( AB.x );
		float ay = Abs( AB.y );
		float az = Abs( AB.z );
		if( ax > ay && ax > az )
		{
			if( s0.x < s1.x )
				swap( &s0, &s1 );

			switchIdx = (	( (oY<0.0f) && (oZ<0.0f) ) ||
							( (oY>0.0f) && (oZ>0.0f) ) );
			
			locA[0] = s0;
			locA[1] = Vec3( s0.x, s0.y + oY, s0.z );
			locA[2] = Vec3( s1.x, s1.y + oY, s1.z );
			locA[3] = s1;
			locA[4] = Vec3( s1.x, s1.y, s1.z + oZ );
			locA[5] = Vec3( s0.x, s0.y, s0.z + oZ );
		}
		else if( ay > ax && ay > az )
		{
			if( s0.y < s1.y )
				swap( &s0, &s1 );

			switchIdx = !(	( (oX<0.0f) && (oZ<0.0f) ) ||
							( (oX>0.0f) && (oZ>0.0f) ) );
			
			locA[0] = s0;
			locA[1] = Vec3( s0.x + oX, s0.y, s0.z );
			locA[2] = Vec3( s1.x + oX, s1.y, s1.z );
			locA[3] = s1;
			locA[4] = Vec3( s1.x, s1.y, s1.z + oZ );
			locA[5] = Vec3( s0.x, s0.y, s0.z + oZ );
		}
		else
		{
			if( s0.z < s1.z )
				swap( &s0, &s1 );

			switchIdx = (	( (oX<0.0f) && (oY<0.0f) ) ||
							( (oX>0.0f) && (oY>0.0f) ) );
			
			locA[0] = s0;
			locA[1] = Vec3( s0.x + oX, s0.y, s0.z );
			locA[2] = Vec3( s1.x + oX, s1.y, s1.z );
			locA[3] = s1;
			locA[4] = Vec3( s1.x, s1.y + oY, s1.z );
			locA[5] = Vec3( s0.x, s0.y + oY, s0.z );
		}
		

		Vec3 zOffset = inDir * 0.005f;

		static const byte tplA[6]   = { 1, 2, 0, 3, 5, 4 };
		static const byte tplA_s[6] = { 2, 1, 3, 0, 4, 5 };
		const byte* idxA = ( switchIdx ) ? tplA_s : tplA;
	
		dd::Vertex_L3_COL vtxA[6];
		for( uint i=0; i<6; i++ )
		{
			vtxA[i].loc	 = locA[idxA[i]] + zOffset;
			vtxA[i].c	 = c;
		}

		uint cmdUpd[] =
		{
			SU_DD_VBOFILL_1( inVbo ),
			SU_DD_VBOFILL_2( dd::Vertex_L3_COL::fvf ),
			SU_DD_VBOFILL_3( vtxA ),
			SU_DD_END(  )
		};
		inDD->StartAndWaitCmdList( cmdUpd );
	}
	void	update_hlight_vertex		(	const Vec3&		inDir,
											uint			inVbo,
											IDirectDraw*	inDD			)
	{
		SU_REQUIRE_OR_RETURN( inDD );
		uint c = inDD->GetPackedColor( (byte)200, 200, 255, 100 );

		float oX = ( inDir.x < 0.0f ) ? 0.2f:-0.2f;
		float oY = ( inDir.y < 0.0f ) ? 0.2f:-0.2f;
		float oZ = ( inDir.z < 0.0f ) ? 0.2f:-0.2f;

		Vec3 locA[7];
		locA[0] = inDir;
		locA[1] = inDir + Vec3( oX,		0.0f,	0.0f );
		locA[2] = inDir + Vec3( oX,		oY,		0.0f );
		locA[3] = inDir + Vec3( 0.0f,	oY,		0.0f );
		locA[4] = inDir + Vec3( 0.0f,	oY,		oZ );
		locA[5] = inDir + Vec3( 0.0f,	0.0f,	oZ );
		locA[6] = inDir + Vec3( oX,		0.0f,	oZ );


        Vec3 upY = Vec3( 0.0f, oY, 0.0f );
        Vec3 crX = Vec3( oX, 0.0f, 0.0f );
		Vec3 cr;
		Vec3Cross( &cr, &crX, &upY );
		bool bswitch =	( cr.z > 0.0f && oZ > 0.0f ) ||
						( cr.z < 0.0f && oZ < 0.0f );

		byte tplA[10]	= { 3, 4, 0, 5, 1, 6, 2, 3, 1, 0 };
		byte tplA_s[10]	= { 4, 3, 5, 0, 6, 1, 3, 2, 0, 1 };
		byte* idxA = ( bswitch ) ? tplA_s : tplA;

		Vec3 zOffset = inDir * 0.005f;
		dd::Vertex_L3_COL vtxA[10];
		for( uint i=0; i<10; i++ )
		{
			vtxA[i].loc	 = locA[idxA[i]] + zOffset;
			vtxA[i].c	 = c;
		}

		uint cmdUpd[] =
		{
			SU_DD_VBOFILL_1( inVbo ),
			SU_DD_VBOFILL_2( dd::Vertex_L3_COL::fvf ),
			SU_DD_VBOFILL_3( vtxA ),
			SU_DD_END(  )
		};
		inDD->StartAndWaitCmdList( cmdUpd );
	}
	uint	update_highlightcube		(	const Vec3&		inDir,
											uint*			inHLSrfA,
											uint*			ioHLVboA,
											IDirectDraw*	inDD			)
	{
		SU_REQUIRE_OR_RETURN_V( !Vec3Near( &inDir, &(Vec3::ZERO), 0.1f ), dd::INVALID_IDX );

		float n2 = Vec3Norm2( &inDir );

		// face		: 0.25
		// edge		: 0.5
		// vertex	: 0.75

		if( n2 < 0.3f )
		{
			update_hlight_face( inDir, ioHLVboA[0], inDD );
			return inHLSrfA[0];
		}
		else if( n2 < 0.6f )
		{
			update_hlight_edge( inDir, ioHLVboA[1], inDD );
			return inHLSrfA[1];
		}

		update_hlight_vertex( inDir, ioHLVboA[2], inDD );
		return inHLSrfA[2];
	}





	static const uint g_arcRes = 16;
	static const Vec4 g_abColor( 1.0f, 0.5f, 0.1f, 0.3f );

	void	load_arcball				(	uint*			ioSrfA,
											uint*			ioVboRect,
											IDirectDraw*	inDD			)
	{
		SU_ASSERT( inDD );

		uint vf = dd::VF_LOC3 | dd::VF_COL;

		const float rad = 1.0f;
		float handleSize = rad*0.05f;
		float handleHSize = handleSize*0.5f;

		const Vec3 handleCenterA[4] = 
		{
			Vec3( rad, 0.0f, 0.0f ),
			Vec3( 0.0f, rad, 0.0f ),
			Vec3(-rad, 0.0f, 0.0f ),
			Vec3( 0.0f,-rad, 0.0f )
		};
		const Vec3 handleOffsA[4] = 
		{
			Vec3( handleHSize, handleHSize, 0.0f ),
			Vec3(-handleHSize, handleHSize, 0.0f ),
			Vec3(-handleHSize,-handleHSize, 0.0f ),
			Vec3( handleHSize,-handleHSize, 0.0f )
		};

		// Update circle
		float handleOAngle = Acosf( handleHSize/rad );
		uint color = inDD->GetPackedColor( g_abColor.x, g_abColor.y, g_abColor.z, g_abColor.w );

		float incTheta = ( HalfPi - handleOAngle*2.0f )/(g_arcRes-1);

		dd::Vertex_L3_COL_K vtxA_circ[g_arcRes*4];
		for( uint i=0; i<4; i++ )
		{
			int ci = i*g_arcRes;
			float theta = i*HalfPi + handleOAngle;
			for( uint j=0; j<g_arcRes; j++ )
			{
				int cv = ci + j;
				vtxA_circ[cv].loc = Vec3( Cosf( theta ), Sinf( theta ), 0.0f ) * rad;
				vtxA_circ[cv].c = color;
				vtxA_circ[cv].kick = (j != 0);

				theta += incTheta;
			}
		}

		// Update handles
		dd::Vertex_L3_COL_K vtxA_rect[4*5];
		for( uint i=0; i<4; i++ )
		{
			int ci = i*5;
			for( uint j=0; j<5; j++ )
			{
				int k = j%4;
				vtxA_rect[ci+j].loc = handleCenterA[i] + handleOffsA[k];
				vtxA_rect[ci+j].c = color;
				vtxA_rect[ci+j].kick = (j != 0);
			}
		}

		create_surface( ioSrfA+0, NULL,		 vtxA_circ, g_arcRes*4,	dd::Vertex_L3_COL_K::fvf, dd::DP_VertexColor, dd::PRIM_LINE, inDD );
		create_surface( ioSrfA+1, ioVboRect, vtxA_rect, 5*4,		dd::Vertex_L3_COL_K::fvf, dd::DP_VertexColor, dd::PRIM_LINE, inDD );
	}

	void	update_highlightarcball		(	int				inIdx,
											uint			inVbo,
											IDirectDraw*	inDD			)
	{
		if( inIdx == -1 )
		{
			uint c = inDD->GetPackedColor( g_abColor.x, g_abColor.y, g_abColor.z, g_abColor.w );
			uint cA[20] ={	c, c, c, c, c,
							c, c, c, c, c,
							c, c, c, c, c,
							c, c, c, c, c	};

			uint cmdUpd[] = 
			{
				SU_DD_VBOFILL_1( inVbo ),
				SU_DD_VBOFILL_2( dd::VF_COL ),
				SU_DD_VBOFILL_3( cA ),
				SU_DD_END(  )
			};

			inDD->StartAndWaitCmdList( cmdUpd );
		}
		else
		{
			uint c = inDD->GetPackedColor( g_abColor.x, g_abColor.y, g_abColor.z, g_abColor.w*2.0f );
			uint cA[5] = { c, c, c, c, c };

			uint cmdUpd[] = 
			{
				SU_DD_VBOUPDATE_1( inVbo ),
				SU_DD_VBOUPDATE_2( 5 ),
				SU_DD_VBOUPDATE_3( inIdx*5 ),
				SU_DD_VBOUPDATE_4( dd::VF_COL ),
				SU_DD_VBOUPDATE_5( cA ),

				SU_DD_END(  )
			};

			inDD->StartAndWaitCmdList( cmdUpd );
		}
	}

	Matrix*	build_arcball_wvp			(	Matrix*			ioWVP,
											const Vec4&		inAbRect,
											const Vec4&		inVpRect		)
	{
		float rad	= inAbRect.z * 0.5f;
		Vec2 center = Vec2( inAbRect.x + rad, inAbRect.y + rad );

		Matrix abw, abv, abp;
		MatrixOrthoLH( &abp, inVpRect.z, inVpRect.w, 1.0f, 20.0f );
		MatrixScaling( &abv, 1.0f, -1.0f, 1.0f );
		MatrixTranslate( &abv, &abv, center.x, center.y, -5.0f );
		MatrixFastInverse( &abv, &abv );

		MatrixScaling( &abw, rad, rad, 1.0f );
		MatrixTranslate( &abw, &abw, center.x, center.y, 0.0f );

		*ioWVP = abw * abv * abp;

		Matrix zc;
		MatrixIdentity( &zc );
		zc.m33 = 0.5f;
		zc.m43 = 0.5f;
		return MatrixMul( ioWVP, ioWVP, &zc );
	}


	bool	load_postfx					(	uint*			ioFxId,
											IDirectDraw*	inDD,
											pcstr			inPixelCode = NULL	)
	{
		void* codeAddr = (void*) ( inPixelCode ? inPixelCode : glsl_post_effect_default_code );
		uint prgId = dd::INVALID_IDX;
		uint cmdCreate[] = 
		{
			SU_DD_PROGRAMLOAD_1( 1 ),
			SU_DD_PROGRAMLOAD_2( codeAddr ),
			SU_DD_PROGRAMLOAD_3( &prgId ),
		
			SU_DD_END()
		};
		
		inDD->StartAndWaitCmdList( cmdCreate );
		*ioFxId = prgId;
		return ( prgId != dd::INVALID_IDX );
	}


	uint	create_vbquad				(	IDirectDraw*	inDD,
											float			inMaxU = 1.0f,
											float			inMaxV = 1.0f,
											bool			inFlipV = true	)
	{
		static const uint quadVF = dd::VF_LOC2 | dd::VF_UV0;
		struct VtxQuad
		{
			Vec2 loc, uv0;
		};

		uint vb = dd::INVALID_IDX;
		uint cmdCr[] =
		{
			SU_DD_VBOCREATE_1( quadVF, dd::PRIM_TRIANGLE, 0 ),
			SU_DD_VBOCREATE_2( 4 ),
			SU_DD_VBOCREATE_3( &vb ),
			SU_DD_END(  )
		};
		inDD->StartAndWaitCmdList( cmdCr );


		if( vb != dd::INVALID_IDX )
		{
		//	float v0 = 1.0f-inMaxV;
		//	float v1 = 1.0f;
			float v0 = inMaxV;
			float v1 = 0.0f;
			if( inFlipV )
			{
				v0 = 1.0f-inMaxV;
				v1 = 1.0f;
			}

			VtxQuad vtxA[4] =
			{
				{ Vec2( -1.0f, -1.0f ),	Vec2( 0.0f,		v0 )	},
				{ Vec2( -1.0f,  1.0f ),	Vec2( 0.0f,		v1  )	},
				{ Vec2(  1.0f, -1.0f ),	Vec2( inMaxU,	v0 )	},
				{ Vec2(  1.0f,  1.0f ),	Vec2( inMaxU,	v1  )	}
			};

			uint cmdUpd[] =
			{
				SU_DD_VBOFILL_1( vb ),
				SU_DD_VBOFILL_2( quadVF ),
				SU_DD_VBOFILL_3( vtxA ),

				SU_DD_END(  )
			};
			inDD->StartAndWaitCmdList( cmdUpd );
		};

		return vb;
	}

	int Compute_MipmapedGaussConvol    (    float*			ioblendA,	//[16]
											int				inSizeX,
											int				inSizeY,
											float			inRadius			)
	{
		//
		// Computes Gaussian normalized sequence

		float gWeights[128];
		float gSigma = GaussianDeviation( inRadius );
		int   gWidth = GaussianWidth( gSigma );
		if( gWidth < 2 || gWidth > 128 )
			return 0;
		GaussianNormSequence( gWeights, gSigma, TRUE );

		//
		// N mipmaps, from (inSizeX,inSizeY) -> (8,8)

		int mip_cpt = 0;
		int minSize = Min( inSizeX, inSizeY );
		while( (minSize>>(mip_cpt+1)) >= 8 )
			mip_cpt++;
		if( mip_cpt == 0 || mip_cpt >= 16 )
			return 0;

		//
		// Computes mipmaps weight

		float sum_mipw[16];
		sum_mipw[ mip_cpt+1 ] = 0.0f;

		float cur_avg = 0.0f;
		for( int mip = mip_cpt ; mip >= 0 ; mip-- )
		{
			// average mipmap weights
			int   w1  = Min( 1 << mip,    gWidth );
			int   w0  = Min( (1<<mip)>>1, gWidth );
			float s   = 0.0f;
			for( int wi = w0 ; wi < w1 ; wi++ )
				s += gWeights[wi];
			float avg  = (w1>w0) ? s / float( w1-w0 ) : 0.0f;

			// mipmap factor to reach the average from the current average
			float w			= 1.0f / float( 1<<mip );
			float mipw		= (avg - cur_avg) / w;
			cur_avg			= avg;
			sum_mipw[mip]	= sum_mipw[ mip+1 ] + mipw;
		}

		//       
		// Computes recursive blending alpha factors (a,b,c,...z),
		// such that :
		// mip(i-1) = mip(i  ) * a + mip(i-1) * (1-a)
		// mip(i-2) = mip(i-1) * b + mip(i-2) * (1-b)
		// ...
		// mip(0)   = mip(1)   * z + mip(0)   * (1-z)
		// fromBAddr += mip(0)
		//
		// a = wa      / (wa+wb)
		// b = (wa+wb) / (wa+wb+wc)
		// ...

		ioblendA[0] = int( sum_mipw[0] );    // ~1.0
		for( int mip = mip_cpt ; mip > 0 ; mip-- )
		{
			float sw0   = sum_mipw[ mip   ];
			float sw1   = sum_mipw[ mip-1 ];
			
			if( !sw0 || !sw1 )
			{
				mip_cpt = mip - 1;
			}
			else
			{
				ioblendA[mip] = sw0 / sw1;
			}
		}
		SU_ASSERT( mip_cpt > 0 );

		return mip_cpt;
	}



	static Matrix g_matA[256];
	Matrix* new_matrix			(								)
	{
		static uint c = 0;
		c = (c+1) & 255;
		return g_matA + c;
	}

	void	setup_constant		(	vector<uint>*	ioCmdA,
									Matrix*			inMatrix,
									pcstr			inName		)
	{
		ioCmdA->push_back( SU_DD_CONSTANT_1( 4, 0, 4, 1 ) );
		ioCmdA->push_back( SU_DD_CONSTANT_2( util::hmac_crc32_w( inName ) ) );
		ioCmdA->push_back( SU_DD_CONSTANT_n( inMatrix ) );
	}
	void	setup_constant		(	vector<uint>*	ioCmdA,
									float			inVal,
									pcstr			inName		)
	{

		ioCmdA->push_back( SU_DD_CONSTANT_1( 1, 0, 1, 0 ) );
		ioCmdA->push_back( SU_DD_CONSTANT_2( util::hmac_crc32_w( inName ) ) );
		ioCmdA->push_back( SU_DD_CONSTANT_n( inVal ) );
	}
	void	setup_constant		(	vector<uint>*	ioCmdA,
									const Vec4&		inVal,
									pcstr			inName		)
	{

		ioCmdA->push_back( SU_DD_CONSTANT_1( 1, 0, 4, 0 ) );
		ioCmdA->push_back( SU_DD_CONSTANT_2( util::hmac_crc32_w( inName ) ) );
		ioCmdA->push_back( SU_DD_CONSTANT_n( inVal.x ) );
		ioCmdA->push_back( SU_DD_CONSTANT_n( inVal.y ) );
		ioCmdA->push_back( SU_DD_CONSTANT_n( inVal.z ) );
		ioCmdA->push_back( SU_DD_CONSTANT_n( inVal.w ) );
	}

	void	setup_constant		(	vector<uint>*	ioCmdA,
									const Vec2&		inVal,
									pcstr			inName		)
	{

		ioCmdA->push_back( SU_DD_CONSTANT_1( 1, 0, 2, 0 ) );
		ioCmdA->push_back( SU_DD_CONSTANT_2( util::hmac_crc32_w( inName ) ) );
		ioCmdA->push_back( SU_DD_CONSTANT_n( inVal.x ) );
		ioCmdA->push_back( SU_DD_CONSTANT_n( inVal.y ) );
	}

}
