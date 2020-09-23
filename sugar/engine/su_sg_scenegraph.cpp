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

#include <sufw/su_core.h>
#include <sufw/codec/su_icodec.h>
#include <sufw/directdriver/su_directdriver.h>
#include <sufw/media/su_iimageanalyzer.h>

#include "su_collection.h"
#include "su_sg_scenegraph.h"

#include "su_sg_handle.h"
#include "su_sg_handleroot.h"

#include "su_sg_core.h"
#include "su_sg_coreframe.h"
#include "su_sg_corescale.h"
#include "su_sg_coreuscale.h"
#include "su_sg_corecamera.h"
#include "su_sg_coregeometry.h"
#include "su_sg_corebillboard.h"
#include "su_sg_coreprimitive.h"
#include "su_sg_corepostfx.h"
#include "su_sg_coreviewport.h"




using namespace su;
using namespace su::sg;


#include "su_sg_scenegraph_rsc.h"
#include "su_sg_scenegraph_ppu.h"
#include "su_sg_scenegraph_ra.h"


// #define DBG_PICKING


namespace
{
	bool	find_free_layer		(	byte*			outLayId	)
	{
		for( uint lId=0; lId<256; lId++ )
		{
			if( !dg::IsLayerExist( lId ) )
			{
				*outLayId = lId;
				return true;
			}
		}
		return false;
	}


	struct DrawCall
	{
		Matrix	world;
		Core*	core;
	};

	void	setup_drawlist_rec	(	vector<DrawCall>*	ioDrawListFx,
									vector<DrawCall>*	ioDrawList,
									vector<DrawCall>**	ioCDLPtr,
                                    sg::Handle*			inCHnd,
									Matrix				inWorld = Matrix::UNIT	)
	{
		SU_ASSERT( ioDrawListFx );
		SU_ASSERT( ioDrawList );
		SU_ASSERT( inCHnd );

		uint hAttr = inCHnd->GetAttributes();
		if( hAttr & HA_Visibility_Show )
		{
			Core* cc = inCHnd->GetCore();
			if( cc )
			{
				if( cc->IsType( Frame::Type ) )
				{
					inWorld = ((Frame*)cc)->GetTR() * inWorld;
				}
				else if( cc->IsType( Scale::Type ) )
				{
					Vec3 sc = ((Scale*)cc)->GetScale();
					MatrixScale( &inWorld, &inWorld, sc.x, sc.y, sc.z );
				}
				else if( cc->IsType( Uscale::Type ) )
				{
					float sc = ((Uscale*)cc)->GetUScale();
					MatrixScale( &inWorld, &inWorld, sc, sc, sc );
				}
				else if( cc->IsType( PostFx::Type ) )
				{
					*ioCDLPtr = ioDrawList;
				}
				else if( cc->IsType( Geometry::Type ) )
				{
					float sc = ((Geometry*)cc)->GetScale();
					MatrixScale( &inWorld, &inWorld, sc, sc, sc );
					DrawCall ndc = { inWorld, cc };
					(*ioCDLPtr)->push_back( ndc );
				}
				else if( cc->IsType( Billboard::Type ) ||
						 cc->IsType( Primitive::Type )	)
				{
					DrawCall ndc = { inWorld, cc };
					(*ioCDLPtr)->push_back( ndc );
				}
			}

			const uint chCount = inCHnd->CountChild();
			for( uint i=0; i<chCount; i++ )
			{
				setup_drawlist_rec( ioDrawListFx, ioDrawList, ioCDLPtr, inCHnd->EnumChild( i ), inWorld );
			}
		}
	}


	void	build_geometry		(	vector<uint>*			ioCmdA,
									Geometry*				inGeom,
									const Matrix&			inWorld,
									const Matrix&			inViewProj		)
	{
		collection::Mesh* mesh = inGeom->GetMesh();
		SU_ASSERT( mesh );

		setup_constant( ioCmdA, MatrixMul( new_matrix(), &inWorld, &inViewProj ), "g_wvp" );
		uint32* cmdAddr = mesh->draw_opcodes();
		SU_ASSERT( cmdAddr );
		if( cmdAddr )
		{
			ioCmdA->push_back( SU_DD_CALL_1() );
			ioCmdA->push_back( SU_DD_CALL_2( cmdAddr ) );
		}
	}

	void	build_buildboard	(	vector<uint>*			ioCmdA,
									Billboard*				inBillboard,
									const Matrix&			inWorld,
									const Matrix&			inViewProj		)
	{
		bool bIn3d = inBillboard->IsInWorldspace();
		if( bIn3d )
		{
			setup_constant( ioCmdA, MatrixMul( new_matrix(), &inWorld, &inViewProj ), "g_wvp" );
		}
		else
		{
			ioCmdA->push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
			ioCmdA->push_back( SU_DD_RS_ZTEST_STATE( 0 ) );
			setup_constant( ioCmdA, MatrixCopy( new_matrix(), &inWorld ), "g_wvp" );
		}
		

		ioCmdA->push_back( SU_DD_TEXTURE( inBillboard->GetPbi(), 0, 0 ) );

		Color32 cth;
		float ath;
		if( inBillboard->GetAlphaThresholdParam( &ath, &cth ) )
		{
			const float i255 = 1.0f/255.0f;
			Vec4 col( float(cth.r)*i255, float(cth.g)*i255, float(cth.b)*i255, float(cth.a)*i255 );
			setup_constant( ioCmdA, col, "color" );
			setup_constant( ioCmdA, ath, "alphaTH" );
			ioCmdA->push_back( SU_DD_PRIMIMM_1( inBillboard->GetVbo() ) );
			ioCmdA->push_back( SU_DD_PRIMIMM_2( dd::DP_AlphaThreshold ) );
		}
		else
		{
			ioCmdA->push_back( SU_DD_PRIMIMM_1( inBillboard->GetVbo() ) );
			ioCmdA->push_back( SU_DD_PRIMIMM_2( dd::DP_Texture ) );
		}
		ioCmdA->push_back( SU_DD_TEXTURE( dd::INVALID_IDX, 0, 0 ) );

		if( !bIn3d )
		{
			ioCmdA->push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
			ioCmdA->push_back( SU_DD_RS_ZTEST_STATE( 1 ) );
		}
	}

	void	build_primitive		(	vector<uint>*			ioCmdA,
									Primitive*				inPrimitive,
									const Matrix&			inWorld,
									const Matrix&			inViewProj		)
	{
		Matrix* nm = MatrixMul( new_matrix(), &inWorld, &inViewProj );
		setup_constant( ioCmdA, nm, "g_wvp" );
		ioCmdA->push_back( SU_DD_PRIM( inPrimitive->GetSrf() ) );
	}
	
	void	build_drawlist_opcode	(	vector<uint>*			ioCmdA,
										const vector<DrawCall>&	inDrawList,
										const Matrix&			inViewProj,
										bool					inAlphaStatus	)
	{
		const uint dc = inDrawList.size();
		for( uint i=0; i<dc; i++ )
		{
			DrawCall& cdc = inDrawList[i];
			if( cdc.core->IsType( Geometry::Type ) )
			{
				Geometry* g = (Geometry*)cdc.core;
				collection::Mesh* mesh = g->GetMesh();
				SU_ASSERT( mesh );
				if( mesh->hasalpha() == inAlphaStatus )
				{
					build_geometry( ioCmdA, g, cdc.world, inViewProj );
				}
			}
			else if( cdc.core->IsType( Billboard::Type ) && inAlphaStatus )
			{
				build_buildboard( ioCmdA, (Billboard*)(cdc.core), cdc.world, inViewProj );
			}
			else if( cdc.core->IsType( Primitive::Type ) && !inAlphaStatus )
			{
				build_primitive( ioCmdA, (Primitive*)(cdc.core), cdc.world, inViewProj );
			}
		}
	}

	void	build_opcode_rec	(	vector<uint>*	ioCmdA,
                                    sg::Handle*		inCHnd,
									const Matrix&	inWorld,
									const Matrix&	inViewProj,
									bool			inAlphaPassStatus	)
	{
		SU_ASSERT( inCHnd );
		SU_ASSERT( ioCmdA );

		Matrix cw = inWorld;

		uint hAttr = inCHnd->GetAttributes();
		if( hAttr & HA_Visibility_Show )
		{
			Core* cc = inCHnd->GetCore();
			if( cc )
			{
				if( cc->IsType( Frame::Type ) )
				{
					Frame* f = (Frame*)cc;
					cw = f->GetTR() * inWorld;
				}
				else if( cc->IsType( Scale::Type ) )
				{
					Vec3 sc = ((Scale*)cc)->GetScale();
					MatrixScale( &cw, &inWorld, sc.x, sc.y, sc.z );
				}
				else if( cc->IsType( Uscale::Type ) )
				{
					float sc = ((Uscale*)cc)->GetUScale();
					MatrixScale( &cw, &inWorld, sc, sc, sc );
				}
				else if( cc->IsType( Geometry::Type ) )
				{
					Geometry* g = (Geometry*)cc;
					collection::Mesh* mesh = g->GetMesh();
					SU_ASSERT( mesh );
					if( mesh->hasalpha() == inAlphaPassStatus )
					{
						float sc = g->GetScale();
						MatrixScale( &cw, &inWorld, sc, sc, sc );

						build_geometry( ioCmdA, g, cw, inViewProj );
					}
				}
				else if( cc->IsType( Billboard::Type ) && inAlphaPassStatus )
				{
					build_buildboard( ioCmdA, (Billboard*)cc, cw, inViewProj );
				}
				else if( cc->IsType( Primitive::Type ) && !inAlphaPassStatus )
				{
					build_primitive( ioCmdA, (Primitive*)cc, cw, inViewProj );
				}
			}

			uint chCount = inCHnd->CountChild();
			for( uint i=0; i<chCount; i++ )
			{
				build_opcode_rec( ioCmdA, inCHnd->EnumChild( i ), cw, inViewProj, inAlphaPassStatus );
			}
		}
	}

	void	build_opcode		(	vector<uint>*	outCmdA,
                                    sg::Handle*		inHandle,
									const Matrix&	inViewProj		)
	{
		build_opcode_rec( outCmdA, inHandle, Matrix::UNIT, inViewProj, false );

		outCmdA->push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
		outCmdA->push_back( SU_DD_RS_BLEND( dd::BM_StdAphaBlend ) );
		build_opcode_rec( outCmdA, inHandle, Matrix::UNIT, inViewProj, true );
		outCmdA->push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
		outCmdA->push_back( SU_DD_RS_BLEND( dd::BM_Disabled ) );
	}

	sg::Handle*	get_handle_rec	(	sg::Handle*		inCHnd,
									uint			inRGB,
									uint&			inCHandleCol,
									uint			inStepCol	)
	{
		SU_ASSERT( inCHnd );

		inCHandleCol += inStepCol;

		if( inCHandleCol == ( inRGB & 0xffffff ) )
			return inCHnd;

		const uint chCount = inCHnd->CountChild();
		for( uint i=0; i<chCount; i++ )
		{
            sg::Handle* ch = get_handle_rec( inCHnd->EnumChild( i ), inRGB, inCHandleCol, inStepCol );
			if( ch )
				return ch;
		}
		return NULL;
	}



	bool	get_color_rec		(	Vec4*			ioColor,
									sg::Handle*		inHandle,
                                    sg::Handle*		inCHnd,
									uint&			inCHandleCol,
									uint			inStepCol		)
	{
		inCHandleCol += inStepCol;

		Core* cc = inCHnd->GetCore();

		if( inCHnd == inHandle )
		{
			byte re = ( inCHandleCol )		& 0xff;
			byte gr = ( inCHandleCol>>8 )	& 0xff;
			byte bl = ( inCHandleCol>>16 )	& 0xff;
			*ioColor = Vec4( float(re)/255.0f, float(gr)/255.0f, float(bl)/255.0f, 1.0f );

			return true;
		}

		const uint chCount = inCHnd->CountChild();
		for( uint i=0; i<chCount; i++ )
		{
			if( get_color_rec( ioColor, inHandle, inCHnd->EnumChild( i ), inCHandleCol, inStepCol ) )
			{
				return true;
			}
		}
		return false;
	}

	bool	get_color			(	Vec4*			ioColor,
									sg::Handle*		inHandle,
									uint			inHandleCount,
									sg::Handle*		inRoot		)
	{
		uint cco = 0;
		return get_color_rec( ioColor, inHandle, inRoot, cco, ( 0xffffff / inHandleCount ) );
	}

	sg::Handle* get_handle_pick	(	sg::Handle*		inRoot,
									uint			inHandleCount,
									uint			inRGB			)
	{
		uint cco = 0;
		uint stepCol = ( 0xffffff / inHandleCount );
		return get_handle_rec( inRoot, ( inRGB & 0xffffff ), cco, stepCol );
	}

	
	void	build_opcodepick_rec(	bool*			haveSelection,
									Vec4*			selColId,
                                    sg::Handle*		inCHnd,
									const Matrix&	inWorld,
									const Matrix&	inViewProj,
									vector<uint>*	ioCmdA,
									uint&			inCHandleCol,
									uint			inStepCol		)
	{
		SU_ASSERT( inCHnd );
		SU_ASSERT( ioCmdA );

		inCHandleCol += inStepCol;

		Matrix cw = inWorld;

		
		uint hAttr = inCHnd->GetAttributes();
		if( hAttr & HA_Visibility_Show )
		{
			Core* cc = inCHnd->GetCore();
			if( cc )
			{
				if( cc->IsType( Frame::Type ) )
				{
					Frame* f = (Frame*)cc;
					cw = f->GetTR() * inWorld;
				}
				else if( cc->IsType( Scale::Type ) )
				{
					Vec3 sc = ((Scale*)cc)->GetScale();
					MatrixScale( &cw, &inWorld, sc.x, sc.y, sc.z );
				}
				else if( cc->IsType( Uscale::Type ) )
				{
					float sc = ((Uscale*)cc)->GetUScale();
					MatrixScale( &cw, &inWorld, sc, sc, sc );
				}

				else if( cc->IsType( Geometry::Type ) )
				{
					Geometry* g = (Geometry*)cc;

					byte re = ( inCHandleCol )		& 0xff;
					byte gr = ( inCHandleCol>>8 )	& 0xff;
					byte bl = ( inCHandleCol>>16 )	& 0xff;
					Vec4 c( float(re)/255.0f, float(gr)/255.0f, float(bl)/255.0f, 1.0f );

					if( g->IsSelected() )
					{
						*haveSelection = true;
						*selColId = c;
					}

					collection::Mesh* mesh = g->GetMesh();
					SU_ASSERT( mesh );

					Matrix* nm = MatrixMul( new_matrix(), &cw, &inViewProj );
					setup_constant( ioCmdA, nm, "g_wvp" );

					uint sc = mesh->countsrf();
					for( uint i=0; i<sc; i++ )
					{
						uint cvbo;
						if( mesh->enumsrf( NULL, &cvbo, i ) )
						{
							setup_constant( ioCmdA, c, "color" );
							ioCmdA->push_back( SU_DD_PRIMIMM_1( cvbo ) );
							ioCmdA->push_back( SU_DD_PRIMIMM_2( dd::DP_Color ) );
						}
					}
				}
				else if( cc->IsType( Billboard::Type ) )
				{
					Billboard* bb = (Billboard*)cc;
					bool bIn3d = bb->IsInWorldspace();
					if( bIn3d )
					{
						setup_constant( ioCmdA, MatrixMul( new_matrix(), &inWorld, &inViewProj ), "g_wvp" );
					}
					else
					{
						ioCmdA->push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
						ioCmdA->push_back( SU_DD_RS_ZTEST_STATE( 0 ) );
						setup_constant( ioCmdA, MatrixCopy( new_matrix(), &inWorld ), "g_wvp" );
					}

					byte re = ( inCHandleCol )		& 0xff;
					byte gr = ( inCHandleCol>>8 )	& 0xff;
					byte bl = ( inCHandleCol>>16 )	& 0xff;
					Vec4 c( float(re)/255.0f, float(gr)/255.0f, float(bl)/255.0f, 1.0f );

					setup_constant( ioCmdA, c, "color" );
					ioCmdA->push_back( SU_DD_PRIMIMM_1( bb->GetVbo() ) );
					ioCmdA->push_back( SU_DD_PRIMIMM_2( dd::DP_Color ) );

					if( !bIn3d )
					{
						ioCmdA->push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
						ioCmdA->push_back( SU_DD_RS_ZTEST_STATE( 1 ) );
					}
				}
			}

			uint chCount = inCHnd->CountChild();
			for( uint i=0; i<chCount; i++ )
			{
				build_opcodepick_rec( haveSelection, selColId, inCHnd->EnumChild( i ), cw, inViewProj, ioCmdA, inCHandleCol, inStepCol );
			}
		}
	}

	void	build_tr_rec		(	Matrix*			ioMatrix,
                                    sg::Handle*		inCHandle	)
	{
		if( inCHandle )
		{
			Core* cc = inCHandle->GetCore();
			if( cc )
			{
				if( cc->IsType( Frame::Type ) )
				{
					Frame* f = (Frame*)cc;
					Matrix tr = f->GetTR();
					MatrixMul( ioMatrix, &tr, ioMatrix );
				}
				else if( cc->IsType( Scale::Type ) )
				{
					Vec3 sc = ((Scale*)cc)->GetScale();
					MatrixScale( ioMatrix, ioMatrix, sc.x, sc.y, sc.z );
				}
				else if( cc->IsType( Uscale::Type ) )
				{
					float sc = ((Uscale*)cc)->GetUScale();
					MatrixScale( ioMatrix, ioMatrix, sc, sc, sc );
				}
			}

			build_tr_rec( ioMatrix, inCHandle->GetParent() );
		}
	}

	void	safe_concat_box		(	bool&			ioEmpty,
									Box3*			ioB0,
									const Box3*		inB1		)
	{
		if( ioEmpty )
		{
			ioEmpty = false;
			*ioB0 = *inB1;
		}
		else
			Box3Concat( ioB0, ioB0, inB1 );
	}

	bool	build_bbox_rec		(	Box3*			ioBox,
									sg::Handle*		inCHandle	)
	{
		SU_REQUIRE_OR_FALSE( inCHandle );

		Box3 cBBox;
		bool bEmpty = true;
		uint cc = inCHandle->CountChild();
		for( uint i=0; i<cc; i++ )
		{
			Box3 childBBox;
			if( build_bbox_rec( &childBBox, inCHandle->EnumChild( i ) ) )
			{
				safe_concat_box( bEmpty, &cBBox, &childBBox );
			}
		}

		Core* core = inCHandle->GetCore();
		if( core )
		{
			if( core->IsType( Geometry::Type ) )
			{
				collection::Mesh* mesh = ((Geometry*)core)->GetMesh();
				if( mesh )
				{
					Box3 box = mesh->bbox();
					safe_concat_box( bEmpty, &cBBox, &box );
				}
			}
			else if( core->IsType( Frame::Type ) && (!bEmpty) )
			{
				Matrix tr = ((Frame*)core)->GetTR();
				Box3Apply( &cBBox, &cBBox, &tr );
			}
		}
		*ioBox = cBBox;
		return (!bEmpty);
	}

	

	//
	// --- Helpers
	
	void	build_warp			(	vector<uint>*		outCmdA,
									uint				inSrf,
									const Quat&			inInvViewRot,
									const Matrix&		inProj			)
	{
		SU_REQUIRE_OR_RETURN( inSrf != dd::INVALID_IDX );

		Matrix viewWarp;
		MatrixRotation		( &viewWarp, &inInvViewRot );
		MatrixFastInverse	( &viewWarp, &viewWarp );
		Matrix* warpWVP = MatrixMul( new_matrix(), &viewWarp, &inProj );

		setup_constant( outCmdA, warpWVP, "g_wvp" );
		outCmdA->push_back( SU_DD_PRIM( inSrf ) );
		outCmdA->push_back( SU_DD_CLEAR	( 0, 0, 1 ) );	// clear depth
	}

	void	build_grid			(	vector<uint>*		outCmdA,
									uint				inSrf,
									sg::Handle*			inGridTarget,
									const Matrix&		inInvView,
									const Matrix&		inProj,
									const Matrix&		inViewProj		)
	{
		SU_REQUIRE_OR_RETURN( inSrf != dd::INVALID_IDX );

		Matrix tw( Matrix::UNIT );
		if( inGridTarget )
			build_tr_rec( &tw, inGridTarget );

		float s0, s1, a0, a1;
		if( get_grid_param( &s0, &s1, &a0, &a1, *(tw.location()), inProj, inInvView )	)
		{
			Matrix w0, w1;
			MatrixScale( &w0, &tw, s0, s0, s0 );
			MatrixScale( &w1, &tw, s1, s1, s1 );
			Matrix* wvp0 = MatrixMul( new_matrix(), &w0, &inViewProj );
			Matrix* wvp1 = MatrixMul( new_matrix(), &w1, &inViewProj );

			outCmdA->push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
			outCmdA->push_back( SU_DD_RS_BLEND( dd::BM_StdAphaBlend ) );

			setup_constant( outCmdA, wvp0, "g_wvp" );
			setup_constant( outCmdA, a0, "alpha" );
			outCmdA->push_back( SU_DD_PRIM( inSrf ) );

			setup_constant( outCmdA, wvp1, "g_wvp" );
			setup_constant( outCmdA, a1, "alpha" );
			outCmdA->push_back( SU_DD_PRIM( inSrf ) );

			outCmdA->push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
			outCmdA->push_back( SU_DD_RS_BLEND( dd::BM_Disabled ) );
		}
	}
	
	void	build_arcball		(	vector<uint>*		outCmdA,
									uint*				inSrfA,
									const Vec4&			inABRect,
									const Vec4&			inVPRect		)
	{
		SU_REQUIRE_OR_RETURN( inSrfA );
		SU_REQUIRE_OR_RETURN( inSrfA[0] != dd::INVALID_IDX );
		SU_REQUIRE_OR_RETURN( inSrfA[1] != dd::INVALID_IDX );

		Matrix* abwvp = build_arcball_wvp( new_matrix(), inABRect, inVPRect );

		outCmdA->push_back( SU_DD_RENDERSTATE_1( 2, 0 ) );
		outCmdA->push_back( SU_DD_RS_ZTEST_STATE( 0 ) );
		outCmdA->push_back( SU_DD_RS_BLEND( dd::BM_StdAphaBlend ) );

		setup_constant	( outCmdA, abwvp, "g_wvp" );
		outCmdA->push_back( SU_DD_PRIM( inSrfA[0] ) );
		outCmdA->push_back( SU_DD_PRIM( inSrfA[1] ) );

		outCmdA->push_back( SU_DD_RENDERSTATE_1( 2, 0 ) );
		outCmdA->push_back( SU_DD_RS_ZTEST_STATE( 1 ) );
		outCmdA->push_back( SU_DD_RS_BLEND( dd::BM_Disabled ) );
	}

	void	build_hcube			(	vector<uint>*		outCmdA,
									uint				inSrf,
									uint				inPbi,
									uint				inHLSrf,
									Frame*				inTarget,
									const Vec4&			inRect			)
	{
		SU_ASSERT( inTarget );
		sg::Handle* thd = inTarget->GetHandle();
		SU_REQUIRE_OR_RETURN( thd );

		SU_REQUIRE_OR_RETURN( inSrf != dd::INVALID_IDX );
		SU_REQUIRE_OR_RETURN( inPbi != dd::INVALID_IDX );


		Matrix w( Matrix::UNIT );
		build_tr_rec( &w, thd );

		Quat rot;
		MatrixGetTR( NULL, &rot, NULL, &w );

		Matrix v, p;
		MatrixTranslation	( &v, 0.0f, 0.0f, -1.6f );
		MatrixRotate		( &v, &v, &rot );
		MatrixFastInverse	( &v, &v );
		MatrixPerspectiveLH	( &p, Pi*0.4f, 1.0f, 0.01f, 5.0f );

		Matrix* wvp = MatrixMul( new_matrix(), &v, &p );

		outCmdA->push_back( SU_DD_VIEWPORT_1( (int)inRect.x, (int)inRect.y ) );
		outCmdA->push_back( SU_DD_VIEWPORT_2( (int)inRect.z, (int)inRect.w ) );

		setup_constant( outCmdA, wvp, "g_wvp" );

		outCmdA->push_back( SU_DD_TEXTURE( inPbi, 0, 0 ) );
		outCmdA->push_back( SU_DD_PRIM	( inSrf ) );
		outCmdA->push_back( SU_DD_TEXTURE( dd::INVALID_IDX, 0, 0 ) );


		if( inHLSrf != dd::INVALID_IDX )
		{
			outCmdA->push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
			outCmdA->push_back( SU_DD_RS_BLEND( dd::BM_StdAphaBlend ) );
			outCmdA->push_back( SU_DD_PRIM( inHLSrf ) );
			outCmdA->push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
			outCmdA->push_back( SU_DD_RS_BLEND( dd::BM_Disabled ) );
		}
	}

	

	void	build_cmd_selection	(	vector<uint>*		outCmdA,
									bool*				outHaveSelection,
									Vec4*				outSelColor,
									uint				inPbiSrf,
									sg::Handle*			inRoot,
									uint				inStepCol,
									vector<Viewport*>&	inVPA			)
	{
		outCmdA->push_back( SU_DD_BEGINFRAME	(  ) );
		outCmdA->push_back( SU_DD_COLOR_1		( 0, 1 ) );
		outCmdA->push_back( SU_DD_COLOR_n		( 0, 0, 0, 0 ) );
		outCmdA->push_back( SU_DD_TARGET		( inPbiSrf ) );
		outCmdA->push_back( SU_DD_CLEAR			( 0, 1, 1 ) );

		uint vpc = inVPA.size();
		for( uint i=0; i<vpc; i++ )
		{
			Camera* ccam = inVPA[i]->GetCamera();
			if( ccam )
			{
				Matrix invView	= ccam->GetLookatTR();
				Vec4 vpRect		= inVPA[i]->GetRect();


				Matrix proj = ccam->GetPerspective( vpRect.z/vpRect.w );

				Matrix viewProj;
				MatrixMul( &viewProj, MatrixFastInverse( &viewProj, &invView ), &proj );

				bool sel = false;
				Vec4 cId;
				uint cco = 0;
				build_opcodepick_rec( &sel, &cId, inRoot, Matrix::UNIT, viewProj, outCmdA, cco, inStepCol );
				if( sel )
				{
					if( outHaveSelection )	*outHaveSelection	= true;
					if( outSelColor )		*outSelColor		= cId;
				}
			}
		}
		// write pickRT
		outCmdA->push_back( SU_DD_ENDFRAME	( ) );
		outCmdA->push_back( SU_DD_END		( ) );
	}
	

	SceneGraph::BackQuad*	get_quad_back(	vector<SceneGraph::BackQuad>*	ioQuadA,
											uint							inW,
											uint							inH,
											IDirectDraw*					inDD		)
	{
		uint qc = ioQuadA->size();
		for( uint i=0; i<qc; i++ )
		{
			if( (*ioQuadA)[i].w == inW &&
				(*ioQuadA)[i].h == inH	)
			{
				return &((*ioQuadA)[i]);
			}
		}

		// push it
		float p2w = Pow( 2, util::GetCeilPow2( inW ) );
		float p2h = Pow( 2, util::GetCeilPow2( inH ) );

		SceneGraph::BackQuad bq;
		bq.w = inW;
		bq.h = inH;

		bq.quad = create_vbquad( inDD, inW/p2w, inH/p2h, false );

		ioQuadA->push_back( bq );
		return &((*ioQuadA)[qc]);
	}


#if defined( SU_HW_WINDOWS )
	static const uint s_pickRTSize = 1024;
#else
	static const uint s_pickRTSize = 512;
#endif

	uint create_pick_rt( IDirectDraw* inDDraw )
	{
		uint rt = dd::INVALID_IDX;
		uint cmdA[] =
		{
			SU_DD_PCREATE_1( s_pickRTSize>>4, s_pickRTSize>>4, 1, 1, 1, 1, 0 ),
			SU_DD_PCREATE_2( &rt ),
		
			SU_DD_END()
		};
		inDDraw->StartAndWaitCmdList( cmdA );

		uint cmdClear[] =
		{
			SU_DD_BEGINFRAME	(  ),
			SU_DD_COLOR_1		( 0, 1 ),
			SU_DD_COLOR_n		( 0, 0, 0, 0 ),
			SU_DD_TARGET		( rt ),
			SU_DD_CLEAR			( 0, 1, 1 ),
			SU_DD_ENDFRAME		( ),
			SU_DD_END			( )
		};
		inDDraw->StartAndWaitCmdList( cmdClear );

		return rt;
	}
    

	
	vector<DrawCall> g_drawlistA;
	vector<DrawCall> g_drawlistFXA;
}


SceneGraph*	SceneGraph::Create			(	Pool*			inPool,
											IDirectDraw*	inDDraw		)
{
	SU_REQUIRE_OR_NULL( inPool );
	SU_REQUIRE_OR_NULL( inDDraw );

	byte lid;
	bool bFid = find_free_layer( &lid );
	SU_ASSERT( bFid );
	SU_REQUIRE_OR_NULL( bFid );


	SceneGraph* nsg = inPool->New<SceneGraph>();
	SU_ASSERT( nsg );
	SU_REQUIRE_OR_NULL( nsg );


	nsg->pool		= inPool;
	nsg->layerid	= lid;
	nsg->viewPbi	= dd::INVALID_IDX;

	nsg->opCodeA.relocate( inPool );
	nsg->viewportA.relocate( inPool );
	nsg->backQuadA.relocate( inPool );
	
	nsg->root = Root::Create( nsg );
	if( !nsg->root )
	{
		inPool->Delete( nsg );
		return NULL;
	}

	nsg->ddraw = inDDraw;
	nsg->ddraw->Retain();

	// init rscs
	nsg->warpSrf = dd::INVALID_IDX;
	nsg->gridSrf = dd::INVALID_IDX;

	nsg->abSrfA[0]	= dd::INVALID_IDX;
	nsg->abSrfA[1]	= dd::INVALID_IDX;
	nsg->abVboRect	= dd::INVALID_IDX;
	nsg->abRectHL	= -1;
	nsg->cubeSrf	= dd::INVALID_IDX;
	nsg->cubePbi	= dd::INVALID_IDX;

	nsg->noisePbi	= dd::INVALID_IDX;
	nsg->pickRT		= dd::INVALID_IDX;
	nsg->viewSize	= Vec2( 0, 0 );

	nsg->mainRT		= dd::INVALID_IDX;
	nsg->quadSS		= dd::INVALID_IDX;
	nsg->quadFS		= create_vbquad( inDDraw );


	nsg->renderMode	= RM_Render;
	nsg->handleCount= 1;			// root
	
	// post fx
	
	nsg->pFxEnable			= false;
	nsg->pFxProgId			= dd::INVALID_IDX;
	nsg->pFxBlurBlendFactor = 0.0f;
	nsg->pFxBlurRadius		= 2.0f;
	nsg->pFxBlurOpCodeDirty	= true;
	nsg->pFxBlurOpCodeA[0]	= SU_DD_RET();

	ppu_init( &(nsg->ppu), inPool, inDDraw );

    nsg->onCameraVideoMode = false;
	
	g_drawlistA.relocate( inPool );
	g_drawlistFXA.relocate( inPool );

	return nsg;
}



void		SceneGraph::Release			(									)
{
	SU_ASSERT( pool );

	for( uint i=0; i<backQuadA.size(); i++ )
	{
		ddraw->ResourceRelease( backQuadA[i].quad );
	}
	backQuadA.reserve( 0 );

	
	g_drawlistA.reserve( 0 );
	g_drawlistFXA.reserve( 0 );


	// release rscs
	ddraw->ResourceRelease( warpSrf );
	ddraw->ResourceRelease( gridSrf );
	ddraw->ResourceRelease( cubeSrf );
	ddraw->ResourceRelease( cubePbi );
	
	ddraw->ResourceRelease( viewPbi );
	ddraw->ResourceRelease( pickRT );
	
	ddraw->ResourceRelease( mainRT );
	ddraw->ResourceRelease( quadFS );
	ddraw->ResourceRelease( quadSS );
	
	
	ddraw->ResourceRelease( pFxProgId );

	ppu_shut( &ppu );

	SafeRelease( root );
	SafeRelease( ddraw );
	layerid = 0;
	opCodeA.reserve( 0 );

	SU_ASSERT( viewportA.size() == 0 );

	pool->Delete( this );
}


void		SceneGraph::SetView			(	uint			inViewPbi,
											const Vec2&		inViewSize	)
{
	SU_ASSERT( ddraw );

	if( viewPbi != dd::INVALID_IDX )
	{
		ddraw->ResourceRelease( viewPbi );
	}

	viewSize = inViewSize;
	viewPbi  = inViewPbi;
	if( viewPbi != dd::INVALID_IDX )
	{
		ddraw->ResourceAddRef( viewPbi );

		uint w = Pow( 2, util::GetCeilPow2( uint( inViewSize.x ) ) );
		uint h = Pow( 2, util::GetCeilPow2( uint( inViewSize.y ) ) );

		uint cmdA[] =
		{
			SU_DD_PCREATE_1( w>>4, h>>4, 1, 1, 1, 1, 0 ),
			SU_DD_PCREATE_2( &mainRT ),
			SU_DD_END()
		};
		ddraw->StartAndWaitCmdList( cmdA );
	
		quadSS = create_vbquad( ddraw, inViewSize.x / float(w), inViewSize.y / float(h) );


		ppu_realize( &ppu, w, h );
	}
}

uint		SceneGraph::GetView			(								)
{
	return viewPbi;
}


Pool*		SceneGraph::GetPool			(								)	{ return pool;		}
sg::Handle*	SceneGraph::GetRoot			(								)	{ return root;		}
byte		SceneGraph::GetLayerId		(								)	{ return layerid;	}

IDirectDraw* SceneGraph::GetDirectDraw	(								)	{ return ddraw;		}

Matrix*		SceneGraph::BuildWorldTR	(	Matrix*			outTR,
											Handle*			inHandle	)
{
	SU_REQUIRE_OR_NULL( outTR );
	
	MatrixIdentity( outTR );
	SU_REQUIRE_OR_RETURN_V( inHandle, outTR );

	build_tr_rec( outTR, inHandle );
	return outTR;
}


Box3*		SceneGraph::BuildBBox		(	Box3*			outBox,
											Handle*			inHandle	)
{
	return ( build_bbox_rec( outBox, inHandle ) ) ? outBox : NULL;
}


#if defined( DBG_PICKING )
	vector<Vec4> g_selColIdA;
#endif

bool		SceneGraph::Draw			(								)
{
	SU_ASSERT( ddraw );
	SU_ASSERT( viewPbi != dd::INVALID_IDX );
	SU_REQUIRE_OR_FALSE( ddraw && viewPbi != dd::INVALID_IDX );


	bool doRender, doPicking, doSelection;
	Vec4 selColId;

	if( renderMode == RM_Render )
	{
		doRender	= true;
		doPicking	= false;
		doSelection = false;
	}
	else if( renderMode == RM_Selection )
	{
		doRender	= false;
		doPicking	= true;
		doSelection = false;
	}
	else if( renderMode == RM_Edition )
	{
		doRender	= true;
		doPicking	= true;
		doSelection = true;		// may turn to false after if there isn't selection
	}
	else if( renderMode == RM_Feedback )
	{
		doRender	= false;
		doPicking	= false;
		doSelection = false;
	}
	
	
	uint vpc = viewportA.size();
	SU_ASSERT( vpc );
	SU_REQUIRE_OR_FALSE( vpc );

	
	//
	// --- update
	
	for( uint i=0; i<vpc; i++ )
	{
		bool hasArcball	= viewportA[i]->IsArcBallShown();
		bool hasHCube	= viewportA[i]->IsHCubeShown();
		
		if( hasArcball )
		{
			Frame* abt = viewportA[i]->GetArcBallTarget();
			if( abt )
				abt->SetRotation( viewportA[i]->GetArcBallRot() );
		}
		if( hasHCube && viewportA[i]->IsHCubeMoving() )
		{
			Frame* hct = viewportA[i]->GetHCubeTarget();
			if( hct )
				hct->SetRotation( viewportA[i]->GetHCubeRot() );
		}
	}
	
	if( doPicking )
	{

		if( pickRT == dd::INVALID_IDX )
		{
			pickRT = create_pick_rt( ddraw );
		}
		if( pickRT != dd::INVALID_IDX )
		{
			bool hasSel = false;
			uint stepCol = ( 0xffffff / handleCount );

			vector<uint> pickOpCodeA( pool );
			build_cmd_selection( &pickOpCodeA, &hasSel, &selColId, pickRT, root, stepCol, viewportA );
			ddraw->StartAndWaitCmdList( pickOpCodeA.data() );
			
			doSelection = ( doSelection && hasSel );
		}
	}
	

	if( doRender )
	{
		if( pFxEnable )
		{
			vector<DrawCall>* pvec = &g_drawlistFXA;
			g_drawlistFXA.resize( 0 );
			g_drawlistA.resize( 0 );
			setup_drawlist_rec( &g_drawlistFXA, &g_drawlistA, &pvec, root );
		}

		opCodeA.resize( 0 );
		opCodeA.push_back( SU_DD_BEGINFRAME	(  ) );
		opCodeA.push_back( SU_DD_COLOR_1	( 0, 1 ) );
		opCodeA.push_back( SU_DD_COLOR_n	( 0, 0, 0, 0 ) );

		opCodeA.push_back( SU_DD_TARGET		( viewPbi ) );
		opCodeA.push_back( SU_DD_CLEAR		( 0, 1, 1 ) );


        if( onCameraVideoMode )
        {
			ra_fill_background( &opCodeA );
        }
        
		for( uint i=0; i<vpc; i++ )
		{
			Camera* ccam = viewportA[i]->GetCamera();
			Vec4 rect  = viewportA[i]->GetRect();

			if( !ccam )
				continue;

			Matrix viewProj;
			Quat invViewRot;

			bool hasArcball	= viewportA[i]->IsArcBallShown();
			bool hasHCube	= viewportA[i]->IsHCubeShown();
			bool hasWarp	= viewportA[i]->IsWarpShown();
			bool hasGrid	= viewportA[i]->IsGridShown();
			
			opCodeA.push_back( SU_DD_VIEWPORT_1( (uint)rect.x, (uint)rect.y ) );
			opCodeA.push_back( SU_DD_VIEWPORT_2( (uint)rect.z, (uint)rect.w ) );
			opCodeA.push_back( SU_DD_SCISSOR_1( (uint)rect.x, (uint)rect.y ) );
			opCodeA.push_back( SU_DD_SCISSOR_2( (uint)rect.z, (uint)rect.w ) );
			

			Matrix proj  = ccam->GetPerspective( rect.z/rect.w );
			Matrix iView = ccam->GetLookatTR();

			MatrixMul( &viewProj, MatrixFastInverse( &viewProj, &iView ), &proj );
			MatrixGetTR( NULL, &invViewRot, NULL, &iView );


            
            if( !onCameraVideoMode )
            {    
                Viewport::BackInfo* bck = viewportA[i]->GetBackground();
                if( bck )
                {
                    BackQuad* bq = get_quad_back( &backQuadA, bck->width, bck->height, ddraw );
				
                    opCodeA.push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
                    opCodeA.push_back( SU_DD_RS_ZWRITE( 0 ) );
				
                    setup_constant( &opCodeA, MatrixIdentity( new_matrix() ), "g_wvp" );
                    opCodeA.push_back( SU_DD_TEXTURE	( bck->pbi, 0, 0 ) );
                    opCodeA.push_back( SU_DD_PRIMIMM_1	( bq->quad ) );
                    opCodeA.push_back( SU_DD_PRIMIMM_2	( dd::DP_Texture ) );
                    opCodeA.push_back( SU_DD_TEXTURE	( dd::INVALID_IDX, 0, 0 ) );
				
                    opCodeA.push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
                    opCodeA.push_back( SU_DD_RS_ZWRITE( 1 ) );
                }
                else if( hasWarp )
                {
                    if( warpSrf == dd::INVALID_IDX )
                    {
                        Color32 tc, bc;
                        viewportA[i]->GetWarpColor( &tc, &bc );
                        load_warp( &warpSrf, tc, bc, ddraw );
                    }
                    build_warp( &opCodeA, warpSrf, invViewRot, proj );
                }
            }
                

			if( hasGrid )
			{
				if( gridSrf == dd::INVALID_IDX )
					load_grid( &gridSrf, ddraw );

				build_grid( &opCodeA, gridSrf, viewportA[i]->GetGridTarget(), iView, proj, viewProj );
			}

			if( hasArcball )
			{
				if( abSrfA[0] == dd::INVALID_IDX )
					load_arcball( abSrfA, &abVboRect, ddraw );

				int hlIdx = viewportA[i]->GetHightlightArcBall();
				if( hlIdx != abRectHL )
				{
					update_highlightarcball( hlIdx, abVboRect, ddraw );
					abRectHL = hlIdx;
				}
				build_arcball( &opCodeA, abSrfA, viewportA[i]->GetArcBallRect(), viewportA[i]->GetRect() );
			}


            if( onCameraVideoMode )
            {
				Matrix v, p;
				ra_get_viewproj( &v, &p );
				viewProj = v*p;
			}
			
            if( !pFxEnable )
			{
				build_opcode( &opCodeA, root, viewProj );
			}
			else
			{
				build_drawlist_opcode( &opCodeA, g_drawlistFXA, viewProj, false );
				opCodeA.push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
				opCodeA.push_back( SU_DD_RS_BLEND( dd::BM_StdAphaBlend ) );
				build_drawlist_opcode( &opCodeA, g_drawlistFXA, viewProj, true );
				opCodeA.push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
				opCodeA.push_back( SU_DD_RS_BLEND( dd::BM_Disabled ) );
			}


			Frame* cht;
			if( hasHCube && ( cht = viewportA[i]->GetHCubeTarget() ) )
			{
				if( cubeSrf == dd::INVALID_IDX )
					load_cube( &cubeSrf, &cubePbi, cubeHLSrfA, cubeHLVboA, pool, ddraw );

				Vec3 hlDir = viewportA[i]->GetHightlightHCubeDir();
				uint hlSrf = update_highlightcube( hlDir, cubeHLSrfA, cubeHLVboA, ddraw );

				opCodeA.push_back( SU_DD_CLEAR( 0, 0, 1 ) );	// clear depth

				build_hcube( &opCodeA, cubeSrf, cubePbi, hlSrf, cht, viewportA[i]->GetHCubeRect() );
			}
		}
		

		if( !onCameraVideoMode && pFxEnable )
		{
			if( pFxProgId == dd::INVALID_IDX )
			{
				load_postfx( &pFxProgId, ddraw );
			}

			if( pFxProgId != dd::INVALID_IDX )
			{
                /*
				if( pFxBlurOpCodeDirty )
				{
					build_blur( pFxBlurOpCodeA, sizeof(pFxBlurOpCodeA), &pFxBlurPbi_int, &pFxBlurMipBlend_int, &ppu, mainRT, quadFS, pFxBlurRadius );
					pFxBlurOpCodeDirty = false;
				}
                */

				opCodeA[3] = SU_DD_TARGET( mainRT );						// switch scene rendering into mainRT
                
			//	opCodeA.push_back( SU_DD_CALL_1() );
			//	opCodeA.push_back( SU_DD_CALL_2( pFxBlurOpCodeA ) );		// blur mainRT
                
                
				Time ct;
				su::clock::GetTime( &ct );

                float ctf = ct;
                float twinkleFact =  0.97f+0.03f*Sin( 110.0f*ctf );
                
				opCodeA.push_back( SU_DD_TARGET	( viewPbi ) );
				opCodeA.push_back( SU_DD_CLEAR	( 0, 1, 1 ) );
                
				setup_constant( &opCodeA, MatrixIdentity( new_matrix() ), "g_wvp" );
				setup_constant( &opCodeA, viewSize, "screenSize" );
				setup_constant( &opCodeA, ctf, "time" );
				setup_constant( &opCodeA, twinkleFact, "twinkleFact" );
			//	setup_constant( &opCodeA, pFxBlurMipBlend_int, "blurMipBlend" );
			//	setup_constant( &opCodeA, pFxBlurBlendFactor, "blurBlend" );

				opCodeA.push_back( SU_DD_VIEWPORT_1( 0, 0 ) );
				opCodeA.push_back( SU_DD_VIEWPORT_2( (uint)viewSize.x, (uint)viewSize.y ) );
				
				opCodeA.push_back( SU_DD_TEXTURE	( mainRT, 0, 0 ) );
			//	opCodeA.push_back( SU_DD_TEXTURE	( pFxBlurPbi_int, 1, 0 ) );
				opCodeA.push_back( SU_DD_PRIMIMM_1	( quadSS ) );
				opCodeA.push_back( SU_DD_PRIMIMM_2	( pFxProgId ) );
			//	opCodeA.push_back( SU_DD_TEXTURE	( dd::INVALID_IDX, 1, 0 ) );
				opCodeA.push_back( SU_DD_TEXTURE	( dd::INVALID_IDX, 0, 0 ) );
			}
		}


		if( doSelection )
		{	
			// draw scene with pp
			
			float stepUV = 1.0f/s_pickRTSize;
			setup_constant( &opCodeA, MatrixIdentity( new_matrix() ), "g_wvp" );
			setup_constant( &opCodeA, selColId, "color" );
			setup_constant( &opCodeA, Vec2( stepUV, stepUV ), "stepUV" );
			
			opCodeA.push_back( SU_DD_VIEWPORT_1( 0, 0 ) );
			opCodeA.push_back( SU_DD_VIEWPORT_2( (uint)viewSize.x, (uint)viewSize.y ) );
			opCodeA.push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
			opCodeA.push_back( SU_DD_RS_BLEND( dd::BM_Additive ) );
			
			opCodeA.push_back( SU_DD_TEXTURE( pickRT, 0, 0 ) );
			opCodeA.push_back( SU_DD_PRIMIMM_1( quadFS ) );
			opCodeA.push_back( SU_DD_PRIMIMM_2( dd::DP_Outlining ) );
			opCodeA.push_back( SU_DD_TEXTURE( dd::INVALID_IDX, 0, 0 ) );
			
			opCodeA.push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
			opCodeA.push_back( SU_DD_RS_BLEND( dd::BM_Disabled ) );
		}


		#if defined( DBG_PICKING )
		if( pickRT )
		{
			{
				vector<uint> pickOpCodeA( pool );
				build_cmd_selection( &pickOpCodeA, NULL, NULL, pickRT, root, ( 0xffffff / handleCount ), viewportA );
				ddraw->StartAndWaitCmdList( pickOpCodeA.data() );
			}

			/*
			setup_constant( &opCodeA, MatrixIdentity( new_matrix() ), "g_wvp" );
			opCodeA.push_back( SU_DD_TEXTURE( pickRT, 0, 0 ) );
			opCodeA.push_back( SU_DD_PRIMIMM_1( quadFS ) );
			opCodeA.push_back( SU_DD_PRIMIMM_2( dd::DP_Texture ) );
			opCodeA.push_back( SU_DD_TEXTURE( dd::INVALID_IDX, 0, 0 ) );
			*/

			float stepUV = 1.0f/s_pickRTSize;
			
			opCodeA.push_back( SU_DD_RENDERSTATE_1( 2, 0 ) );
			opCodeA.push_back( SU_DD_RS_BLEND( dd::BM_Additive ) );
			opCodeA.push_back( SU_DD_RS_ZTEST_STATE( 0 ) );

			setup_constant( &opCodeA, MatrixIdentity( new_matrix() ), "g_wvp" );
			setup_constant( &opCodeA, Vec2( stepUV, stepUV ), "stepUV" );

		//	SU_STDOUT( LString( "%%\n" ).argl( g_selColIdA.size() ) );


			for( uint i=0; i<g_selColIdA.size(); i++ )
			{
				setup_constant( &opCodeA, g_selColIdA[i], "color" );

				opCodeA.push_back( SU_DD_TEXTURE( pickRT, 0, 0 ) );
				opCodeA.push_back( SU_DD_PRIMIMM_1( quadFS ) );
				opCodeA.push_back( SU_DD_PRIMIMM_2( dd::DP_Outlining ) );
				opCodeA.push_back( SU_DD_TEXTURE( dd::INVALID_IDX, 0, 0 ) );
			}

			opCodeA.push_back( SU_DD_RENDERSTATE_1( 2, 0 ) );
			opCodeA.push_back( SU_DD_RS_BLEND( dd::BM_Disabled ) );
			opCodeA.push_back( SU_DD_RS_ZTEST_STATE( 1 ) );
		}
		#endif


		//
		// --- Draw unaffected post fx
		
		if( pFxEnable )
		{
			for( uint i=0; i<vpc; i++ )
			{
				Camera* ccam = viewportA[i]->GetCamera();
				Vec4 rect  = viewportA[i]->GetRect();
				if( ccam )
				{
					opCodeA.push_back( SU_DD_VIEWPORT_1( (uint)rect.x, (uint)rect.y ) );
					opCodeA.push_back( SU_DD_VIEWPORT_2( (uint)rect.z, (uint)rect.w ) );
					opCodeA.push_back( SU_DD_SCISSOR_1( (uint)rect.x, (uint)rect.y ) );
					opCodeA.push_back( SU_DD_SCISSOR_2( (uint)rect.z, (uint)rect.w ) );
			
					Matrix proj  = ccam->GetPerspective( rect.z/rect.w );
					Matrix iView = ccam->GetLookatTR();
					Matrix viewProj;
					MatrixMul( &viewProj, MatrixFastInverse( &viewProj, &iView ), &proj );

					build_drawlist_opcode( &opCodeA, g_drawlistA, viewProj, false );
					opCodeA.push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
					opCodeA.push_back( SU_DD_RS_BLEND( dd::BM_StdAphaBlend ) );
					build_drawlist_opcode( &opCodeA, g_drawlistA, viewProj, true );
					opCodeA.push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
					opCodeA.push_back( SU_DD_RS_BLEND( dd::BM_Disabled ) );
				}
			}
		}
		//
		// --- Draw Tiles

		opCodeA.push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
		opCodeA.push_back( SU_DD_RS_BLEND( dd::BM_StdAphaBlend ) );

		uint groupCmdA[64];
		MCursor groupCmdC( groupCmdA, sizeof(groupCmdA) );
		bool updGroup = false;;

		for( uint i=0; i<vpc; i++ )
		{
			Viewport::TGInfo* tgA;
			uint tgCount;
			viewportA[i]->GetTileGroup( &tgA, &tgCount );

			for( uint j=0; j<tgCount; j++ )
			{
				if( tgA[j].dirty )
				{
					groupCmdC.u32 << SU_DD_TILEGROUP_1( tgA[j].groupIdx, dd::TGD_Dirty );
					groupCmdC.u32 << SU_DD_TILEGROUP_2_Dirty();
					tgA[j].dirty = false;
					updGroup = true;
				}
				if( tgA[j].upLoc )
				{
					groupCmdC.u32 << SU_DD_TILEGROUP_1( tgA[j].groupIdx, dd::TGD_Locate );
					groupCmdC.u32 << SU_DD_TILEGROUP_2_Locate( tgA[j].ox, tgA[j].oy );
					tgA[j].upLoc = false;
					updGroup = true;
				}
			}

			Viewport::TLInfo* tlA;
			uint tlCount;
			viewportA[i]->GetTileList( &tlA, &tlCount );

			for( uint j=0; j<tlCount; j++ )
			{
				if( tlA[j].show )
				{
					opCodeA.push_back( SU_DD_TILE_1( tlA[j].group, tlA[j].pbi ) );
					opCodeA.push_back( SU_DD_TILE_2( tlA[j].tl ) );
				}
			}
		}

		if( updGroup )
		{
			groupCmdC.u32 << SU_DD_END();
			ddraw->StartAndWaitCmdList( groupCmdA );
		}

		opCodeA.push_back( SU_DD_RENDERSTATE_1( 1, 0 ) );
		opCodeA.push_back( SU_DD_RS_BLEND( dd::BM_Disabled ) );

		opCodeA.push_back( SU_DD_ENDFRAME	( ) );
		opCodeA.push_back( SU_DD_PRESENT	( viewPbi ) );
		opCodeA.push_back( SU_DD_END		( ) );

		ddraw->StartAndWaitCmdList( opCodeA.data() );
	}

	return true;
}




sg::Handle*		SceneGraph::Pick		(	Viewport*			inViewport,
											const Vec2&			inCoord		)
{
	SU_ASSERT( ddraw );
	
	SU_REQUIRE_OR_NULL( inCoord.x >= 0.0f && inCoord.x <= viewSize.x );
	SU_REQUIRE_OR_NULL( inCoord.y >= 0.0f && inCoord.y <= viewSize.y );

	//
	// --- build cmdList
	
	if( renderMode == RM_Render )
	{
		if( pickRT == dd::INVALID_IDX )
		{
			pickRT = create_pick_rt( ddraw );
		}
		if( pickRT != dd::INVALID_IDX )
		{
			uint stepCol = ( 0xffffff / handleCount );
			Vec4 selColId;
			
			vector<uint> pickOpCodeA( pool );
			build_cmd_selection( &pickOpCodeA, NULL, &selColId, pickRT, root, stepCol, viewportA );
			ddraw->StartAndWaitCmdList( pickOpCodeA.data() );
		}
	}

	uint tcx =				  uint( ( inCoord.x / viewSize.x ) * s_pickRTSize );
	uint tcy = s_pickRTSize - uint( ( inCoord.y / viewSize.y ) * s_pickRTSize );
	uint rgb = 0;

	uint cmdA[] =
	{
		SU_DD_READPIXELS_1( tcx, tcy ),
		SU_DD_READPIXELS_2( 1, 1 ),
		SU_DD_READPIXELS_3( pickRT ),
		SU_DD_READPIXELS_4( &rgb ),

		SU_DD_END		( )
	};
	ddraw->StartAndWaitCmdList( cmdA );

	return get_handle_pick( root, handleCount, ( rgb & 0xffffff ) );
}

void		SceneGraph::Pick			(	vector<Handle*>*	ioHandleA,
											Viewport*			inViewport,
											const Vec2&			inCoord,
											const Vec2&			inSize			)
{
	SU_ASSERT( ddraw );

	//
	// --- build cmdList
	
	if( renderMode == RM_Render )
	{
		if( pickRT == dd::INVALID_IDX )
		{
			pickRT = create_pick_rt( ddraw );
		}
		if( pickRT != dd::INVALID_IDX )
		{
			uint stepCol = ( 0xffffff / handleCount );
			Vec4 selColId;
			
			vector<uint> pickOpCodeA( pool );
			build_cmd_selection( &pickOpCodeA, NULL, &selColId, pickRT, root, stepCol, viewportA );
			ddraw->StartAndWaitCmdList( pickOpCodeA.data() );
		}
	}


	PString log;

	float x0 = inCoord.x - ( inSize.x * 0.5f );
	float y0 = inCoord.y - ( inSize.y * 0.5f );
	int tcx =				 uint( ( x0 / viewSize.x ) * s_pickRTSize );
	int tcy = s_pickRTSize - uint( ( y0 / viewSize.y ) * s_pickRTSize );
	int w = ( inSize.x / viewSize.x ) * s_pickRTSize;
	int h = ( inSize.y / viewSize.y ) * s_pickRTSize; 

	tcy -= h;
	if( tcx < 0 )
	{
		w += tcx;
		tcx = 0;
	}
	if( tcy < 0 )
	{
		h += tcy;
		tcy = 0;
	}

	void* memAddr = pool->Alloc( w * h * 4 );
	uint cmdA[] =
	{
		SU_DD_READPIXELS_1( tcx, tcy ),
		SU_DD_READPIXELS_2( w, h ),
		SU_DD_READPIXELS_3( pickRT ),
		SU_DD_READPIXELS_4( memAddr ),

		SU_DD_END		( )
	};
	ddraw->StartAndWaitCmdList( cmdA );

	uint* inD = (uint*)memAddr;


#if defined( DBG_PICKING )
	static bool ftime = true;
	if( ftime )
	{
		ftime = false;
		g_selColIdA.relocate( pool );
	}
	g_selColIdA.resize( 0 );
#endif


	uint hdCount = 0;
	for( uint y=0; y<h; y++ )
	{
		for( uint x=0; x<w; x++ )
		{
			uint cRGB = ( ( *inD++ ) & 0xffffff );
			Handle* hd = get_handle_pick( root, handleCount, cRGB );
			if( hd )
			{
				bool alreadyHave = false;
				for( uint i=0; i<hdCount; i++ )
				{
					if( (*ioHandleA)[i] == hd )
					{
						alreadyHave = true;
						break;
					}
				}

				if( !alreadyHave )
				{
					hdCount++;
					ioHandleA->push_back( hd );

				#if defined( DBG_PICKING )
					Vec4 cc;
					if( get_color( &cc, hd, handleCount, root ) )
					{
						g_selColIdA.push_back( cc );
					}
				#endif
				}
			}
		}
	}

	pool->Free( memAddr );
}

void		SceneGraph::SetRenderMode	(	RenderMode		inRenderMode	)	{	renderMode = inRenderMode;	}
RenderMode	SceneGraph::GetRenderMode	(									)	{	return renderMode;			}


void		SceneGraph::EnablePostFx	(	bool			inOnOff			)
{
	pFxEnable = inOnOff;
}

void		SceneGraph::SetPostFxValuef	(	PostFxValue		inOpt,
											float			inVal			)
{
	if( inOpt == PFV_BlurRadius )
	{
		pFxBlurRadius = inVal;
		pFxBlurOpCodeDirty = true;
	}
	else if( inOpt == PFV_BlurBlendFactor )
	{
		pFxBlurBlendFactor = inVal;
	}
}

void		SceneGraph::SetPostFxCode	(	pcstr			inPixelCode		)
{
	ddraw->ResourceRelease( pFxProgId );
	load_postfx( &pFxProgId, ddraw, inPixelCode );
}





bool	SceneGraph::SetCameraVideoMode	(	bool        inOnOff         )
{
    if( inOnOff )
    {
        if( !onCameraVideoMode )
        {
            onCameraVideoMode = ra_init( pool, ddraw );
            return onCameraVideoMode;
        }
    }
    else
    {
        if( onCameraVideoMode )
        {
            ra_shut();
            onCameraVideoMode = false;
            return true;
        }
    }

    return ( onCameraVideoMode == inOnOff );
}


void	SceneGraph::OnEvent				(	Handle*			inHandle,
											EventReason		inReason	)
{
	if( inReason == ER_New )
	{
		handleCount++;
	}
	else if( inReason == ER_Delete )
	{
		SU_ASSERT( handleCount );
		handleCount--;
	}
}

void	SceneGraph::OnEvent				(	Core*			inCore,
											EventReason		inReason	)
{
	if( inCore->IsType( Viewport::Type ) )
	{
		Viewport* inVP = (Viewport*)inCore;

		if( inReason == ER_New )
		{
			viewportA.push_back( inVP );
		}
		else if( inReason == ER_Delete )
		{
			vector<Viewport*>::iterator it;
			for( it = viewportA.begin(); it != viewportA.end(); it++ )
				if( (*it) == inVP )
					break;

			SU_ASSERT( it != viewportA.end() );

			if( it != viewportA.end() )
			{
				viewportA.erase( it );
			}
		}
	}
}