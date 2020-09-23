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
#include <sufw/asset/su_asset.h>
#include <sufw/directdriver/su_directdriver.h>
#include <sufw/su_mapping.h>
#include "su_engine.h"

using namespace su;
using namespace su::map;
using namespace su::collection;



namespace
{
	void	load_box			(	Box3*			ioBox,
									map::box3&		inBox		)
	{
		if( ioBox )
		{
			inBox.center.load( ioBox->center );
			inBox.rotation.load( ioBox->rotation );
			inBox.length.load( ioBox->length );
		}
	}

	bool	create_srf		(	uint*			outSrf,
								uint			inVbo,
								uint			inProgId,
								IDirectDraw*	inDD		)
	{
		uint ddcmd[16];
		MCursor ddc( ddcmd, sizeof(ddcmd) );

		int sf = dd::INVALID_IDX;
		ddc.seek( 0 );

		ddc.u32 << SU_DD_SURFACE_1( 1, inProgId );
		ddc.u32 << SU_DD_SURFACE_2( &sf );
		ddc.u32 << SU_DD_SURFACE_n( inVbo, 0 );

		ddc.u32 << SU_DD_END();

		SU_ASSERT( ddc.forward() );

		inDD->StartAndWaitCmdList( ddcmd );

		*outSrf = sf;
		return ( sf != dd::INVALID_IDX );
	}

	struct MyMesh: public Mesh
	{
		Pool*			pool;
		IDirectDraw*	dd;
		uint			refCount;

		Box3			bBox;

		bool			alpha;

		uint*			texCountSrf;
		uint*			texSrf;

		uint*			vboIdA;
		uint*			srfIdA;
		uint			srfCount;

		uint*			texIdA;
		uint			texCount;

		uint*			cmdDrawA;
		uint*			cmdBBoxA;
		uint*			cmdOutlA;


		Resource*			resource			( );
		void				retain				( );
		void				release				( );

		Box3				bbox				( );

		uint				countsrf			( );
		bool				enumsrf				( uint* osrf, uint* ovbo, uint srfidx );
		uint				countsrftex			( uint srfidx );
		bool				enumsrftex			( uint* opbi, uint texIdx, uint srfidx );
		bool				hasalpha			( );

		uint*				draw_opcodes		( );
		uint*				bbox_opcodes		( );
		uint*				outlining_opcodes	( );

		bool				bind_skeleton		( Skeleton* inSkel );
		void				compute				( );
	};

	void		MyMesh::retain				(					)	{	refCount++;			}

	Box3		MyMesh::bbox				(					)	{	return bBox;		}
	uint		MyMesh::countsrf			(					)	{	return srfCount;	}
	bool		MyMesh::enumsrf				(	uint* osrf,
												uint* ovbo,
												uint  idx		)
	{
		SU_REQUIRE_OR_FALSE( idx < srfCount );
		if( osrf )	*osrf = srfIdA[idx];
		if( ovbo )	*ovbo = vboIdA[idx];
		return true;
	}


	uint		MyMesh::countsrftex			( uint srfidx )
	{
		SU_REQUIRE_OR_0( srfidx < srfCount );
		return texCountSrf[srfidx];
	}
	bool		MyMesh::enumsrftex			( uint* opbi, uint texIdx, uint srfidx )
	{
		SU_REQUIRE_OR_FALSE( srfidx < srfCount );
		SU_REQUIRE_OR_FALSE( texIdx < countsrftex( srfidx ) );
		uint ctex = 0;
		uint csrfi = 0;
		while( csrfi < srfidx )
			ctex += texCountSrf[csrfi++];
		*opbi = texSrf[ctex+texIdx];
		return true;
	}

	bool		MyMesh::hasalpha			(					)	{	return alpha;	}

	uint*		MyMesh::draw_opcodes		(					)	{	return cmdDrawA;	}
	uint*		MyMesh::bbox_opcodes		(					)	{	return cmdBBoxA;	}
	uint*		MyMesh::outlining_opcodes	(					)	{	return cmdOutlA;	}

	bool		MyMesh::bind_skeleton		( Skeleton*	inSkel	)	{	/*TODO*/return false;	}
	void		MyMesh::compute				(					)	{	/*TODO*/				}

	void		MyMesh::release				(					)
	{
		SU_ASSERT( refCount );
		if( --refCount == 0 )
		{
			SU_ASSERT( dd );
			for( uint i=0; i<srfCount; i++ )
			{
				dd->ResourceRelease( vboIdA[i] );
				dd->ResourceRelease( srfIdA[i] );
			}
			for( uint i=0; i<texCount; i++ )
			{
				dd->ResourceRelease( texIdA[i] );
			}

			SU_ASSERT( pool );
			pool->Delete( this );
		}
	}






	

	//
	// --- Build on stack just for mesh creation

	struct TexRegister
	{
		struct TexMap
		{
			uint	defId;
			uint	pbiId;
			bool	alpha;
		};
		vector<TexMap>	mapA;

		TexRegister( Pool* inPool )
		{
			mapA.relocate( inPool );
		}

		bool	Have		(	uint			inDefId		)
		{
			for( uint i=0; i<mapA.size(); i++ )
				if( mapA[i].defId == inDefId )
					return true;
			return false;
		}
		bool	Get			(	uint*			ioPbiId,
								bool*			ioAlpha,
								uint			inDefId		)
		{
			for( uint i=0; i<mapA.size(); i++ )
			{
				if( mapA[i].defId == inDefId )
				{
					*ioPbiId = mapA[i].pbiId;
					*ioAlpha = mapA[i].alpha;
					return true;
				}
			}
			return false;
		}
		bool	Reg			(	uint*			ioPbiId,
								uint			inDefId,
								Resource*		inRsc,
								IDirectDraw*	inDD		)
		{
			uint defCount = inRsc->countdef();
			for( uint i=0; i<defCount; i++ )
			{
				Define cDef = inRsc->enumdef( i );
				if( cDef.id == inDefId )
				{
					Tag cTag = inRsc->enumtag( cDef.tag.index+1 );
					SU_ASSERT( cTag.code == rsc_collection_bitmap::CODE );
					
					if( cTag.code == rsc_collection_bitmap::CODE )
					{
						*ioPbiId = dd::INVALID_IDX;

						rsc_collection_bitmap ct;
						ct.load( cTag.addr );
						
						uint cmdA[] = 
						{
							SU_DD_PLOAD_1	(  ),
							SU_DD_PLOAD_2	( cTag.addr ),
							SU_DD_PLOAD_3	( ioPbiId ),
							SU_DD_END		(  )
						};
						inDD->StartAndWaitCmdList( cmdA );
						
						TexMap ntm;
						ntm.defId = inDefId;
						ntm.pbiId = *ioPbiId;
						ntm.alpha = ct.astatus != rsc_collection_bitmap::AS_OPAQUE;

						mapA.push_back( ntm );

						return ( *ioPbiId != dd::INVALID_IDX );
					}
				}
			}
			return false;
		}

		bool	SafeReg		(	uint			inDefId,
								Resource*		inRsc,
								IDirectDraw*	inDD		)
		{
			if( !Have( inDefId ) )
			{
				uint pbi;
				return Reg( &pbi, inDefId, inRsc, inDD );
			}
			return true;
		}

		void	Clean		(								)
		{
			mapA.reserve( 0 );
			mapA.relocate( NULL );
		}
		uint	EnumPbi		(	uint			inIdx		)
		{
			SU_REQUIRE_OR_FALSE( inIdx < mapA.size() );
			return mapA[inIdx].pbiId;
		}
		uint	Count		(								)
		{
			return mapA.size();
		}
	};



	bool	create_buffers	(	vector<uint>*	ioVboA,
								TexRegister*	ioTexReg,
								uint*			ioSrfCount,
								uint*			ioTotalUpTexCount,
								Resource*		inRsc,
								const Tag&		inTag,
								IDirectDraw*	inDD		)
	{
		SU_ASSERT( inTag.code == rsc_collection_mesh::CODE );

		uint srfCount	= 0;
		uint texUpCount	= 0;

		Tag cTag = inRsc->nexttag( inTag );
		while( true )
		{
			if( cTag.code == rsc_collection_mesh_vbo::CODE )
			{
				uint vbo = dd::INVALID_IDX;
				uint cmdLoad[] =
				{
					SU_DD_VBOLOAD_1( ),
					SU_DD_VBOLOAD_2( cTag.addr ),
					SU_DD_VBOLOAD_3( &vbo ),
					SU_DD_END()
				};

				inDD->StartAndWaitCmdList( cmdLoad );
				SU_ASSERT( vbo != dd::INVALID_IDX );

				ioVboA->push_back( vbo );
			}
			else if( cTag.code == rsc_collection_mesh_surface::CODE )
			{
				rsc_collection_mesh_surface srf;
				cTag.load( srf );

				srfCount++;
				texUpCount += srf.texids.count;
				for( uint i=0; i<srf.texids.count; i++ )
				{
					bool bLoaded = ioTexReg->SafeReg( srf.texids.ids.get( i ), inRsc, inDD );
					SU_ASSERT( bLoaded );
				}
			}
			else if	( cTag.code == rsc_collection_mesh_blendprocess::CODE )
			{
				while( cTag.code == rsc_collection_mesh_blendpass::CODE )
					cTag = inRsc->nexttag( cTag );
			}
			// Attributes supported
			else if	( cTag.code == rsc_collection_sourcefile::CODE )	{}	// Ignored for now
			else if	( cTag.code == rsc_collection_hotbox::CODE )		{}	// Ignored for now
			else if	( cTag.code == rsc_collection_hotpoint::CODE )		{}	// Ignored for now
			else if	( cTag.code == rsc_collection_hotsegment::CODE )	{}	// Ignored for now
			else if	( cTag.code == rsc_collection_hotcloud::CODE )		{}	// Ignored for now
			else if	( cTag.code == rsc_collection_hotspline::CODE )		{}	// Ignored for now
			else if	( cTag.code == rsc_collection_hotsphere::CODE )		{}	// Ignored for now
			else
			{
				break;
			}
		
			cTag = inRsc->nexttag( cTag );
		}

		*ioSrfCount	= srfCount;
		*ioTotalUpTexCount = texUpCount;

		return ( srfCount > 0 );
	}
}



Mesh*	collection::create_mesh	( Resource*	inRsc, const Tag& inTag, IDirectDraw* inDD )
{
	SU_ASSERT( inTag.code == rsc_collection_mesh::CODE );
	SU_ASSERT( inRsc );
	SU_ASSERT( inDD );

	Pool* p = inRsc->pool();
	SU_REQUIRE_OR_NULL( p );


	Pool* buildPool = p->CreateSub();		// Used for temporary builder object
	
	TexRegister	texReg( buildPool );
	vector<uint> vboA( buildPool );

	uint srfCount = 0;
	uint upCodeTexCount = 0;

	MyMesh* m = NULL;
	if( create_buffers( &vboA, &texReg, &srfCount, &upCodeTexCount, inRsc, inTag, inDD ) )
	{
		uint texCount	  = texReg.Count();
		uint cmdDrawCount = upCodeTexCount*2 + srfCount*1 + 1;		// nt*( bind, unbind ) + ns*( prim ) + SU_DD_RET
		uint cmdBBoxCount = 1;										// SU_DD_RET();
		uint cmdOutlCount = 1;										// SU_DD_RET();

		uint bsTexCountSrf	= srfCount		* sizeof(uint);
		uint bsTexSrf		= upCodeTexCount* sizeof(uint);
		uint bsVboIdA		= srfCount		* sizeof(uint);
		uint bsSrfIdA		= srfCount		* sizeof(uint);
		uint bsTexIdA		= texCount		* sizeof(uint);
		uint bsCmdDraw		= cmdDrawCount  * sizeof(uint);
		uint bsCmdBBox		= cmdBBoxCount  * sizeof(uint);
		uint bsCmdOutl		= cmdOutlCount  * sizeof(uint);

		m = p->New<MyMesh>( bsTexCountSrf + bsTexSrf + bsVboIdA + bsSrfIdA + bsTexIdA + bsCmdDraw + bsCmdBBox + bsCmdOutl );
	
		m->pool		= p;
		m->dd		= inDD;
		m->refCount	= 1;

		rsc_collection_mesh tm;
		inTag.load( tm );
		load_box( &(m->bBox), tm.bbox );
		

		// Relocate ptrs
		byte* dataAddr = (byte*)(m + 1);
		uint bo = 0;

		m->texCountSrf	= (uint*)(dataAddr + bo);						bo += bsTexCountSrf;
		m->texSrf		= (uint*)(dataAddr + bo);						bo += bsTexSrf;
		m->vboIdA		= (uint*)(dataAddr + bo);						bo += bsVboIdA;
		m->srfIdA		= (uint*)(dataAddr + bo);						bo += bsSrfIdA;
		m->texIdA		= texCount ? (uint*)(dataAddr + bo) : NULL;		bo += bsTexIdA;
		m->cmdDrawA		= (uint*)(dataAddr + bo);						bo += bsCmdDraw;
		m->cmdBBoxA		= (uint*)(dataAddr + bo);						bo += bsCmdBBox;
		m->cmdOutlA		= (uint*)(dataAddr + bo);						bo += bsCmdOutl;


		// fill them

		// Check Var
		m->srfCount		= 0;
		uint cmdDrawCo	= 0;
		uint cmdBBoxCo	= 0;
		uint cmdOutlCo	= 0;
		uint texSrfCo	= 0;

		Tag cTag = inRsc->nexttag( inTag );
		while( true )
		{
			if( cTag.code == rsc_collection_mesh_vbo::CODE ){}
			else if( cTag.code == rsc_collection_mesh_surface::CODE )
			{
				rsc_collection_mesh_surface srf;
				cTag.load( srf );

				uint vboidx	= srf.vboidx;
				uint progId = dd::INVALID_IDX;	// find_program( srf.effectid, srf.techniqueidx, inEffectA, inEffectCount );

				uint texCount = srf.texids.count;
				SU_ASSERT( texCount <= 8 );


				SU_ASSERT( vboidx < vboA.size() );
				if( vboidx >= vboA.size() )
				{
					SU_ERROR( "Buffers index aren't valid" );
				}
				else
				{
					if( progId == dd::INVALID_IDX )
					{
						if( texCount == 1 )
						{
							progId = dd::DP_Texture;
						}
						else if( texCount > 1 )
						{
							progId = dd::DP_MultiTex_Multiplicative;
						}
						else
						{
						//	if( fvf | VF_COL ) -> dd::VertexColor;
							progId = dd::DP_Position;
						}
					}

					uint sf;
					if( create_srf( &sf, vboA[vboidx], progId, inDD ) )
					{
						m->texCountSrf[m->srfCount] = texCount;
						m->vboIdA[m->srfCount] = vboA[vboidx];
						m->srfIdA[m->srfCount] = sf;
						m->srfCount++;

						inDD->ResourceAddRef( vboA[vboidx] );
						m->alpha = false;
						//
						// --- BindTex
						for( uint i=0; i<texCount; i++ )
						{
							uint pbi = dd::INVALID_IDX;
							bool alpha;
							bool bTex = texReg.Get( &pbi, &alpha, srf.texids.ids.get(i) );
							SU_ASSERT( bTex );
							if( alpha )
								m->alpha = true;
							m->cmdDrawA[cmdDrawCo++] = SU_DD_TEXTURE( pbi, i, 0 );

							m->texSrf[texSrfCo++] = pbi;
						}
						//
						// --- Prim
						m->cmdDrawA[cmdDrawCo++] = SU_DD_PRIM( sf );

						//
						// --- UnBindTex
						for( int i=texCount-1; i>=0; i-- )
						{
							m->cmdDrawA[cmdDrawCo++] = SU_DD_TEXTURE( dd::INVALID_IDX, i, 0 );
						}
					}
				}
			}
			else if	( cTag.code == rsc_collection_mesh_blendprocess::CODE )
			{
				while( cTag.code == rsc_collection_mesh_blendpass::CODE )
					cTag = inRsc->nexttag( cTag );
			}
			// Attributes supported
			else if	( cTag.code == rsc_collection_sourcefile::CODE )	{}	// Ignored for now
			else if	( cTag.code == rsc_collection_hotbox::CODE )		{}	// Ignored for now
			else if	( cTag.code == rsc_collection_hotpoint::CODE )		{}	// Ignored for now
			else if	( cTag.code == rsc_collection_hotsegment::CODE )	{}	// Ignored for now
			else if	( cTag.code == rsc_collection_hotcloud::CODE )		{}	// Ignored for now
			else if	( cTag.code == rsc_collection_hotspline::CODE )		{}	// Ignored for now
			else if	( cTag.code == rsc_collection_hotsphere::CODE )		{}	// Ignored for now
			else
			{
				break;
			}
		
			cTag = inRsc->nexttag( cTag );
		}


		m->texCount = 0;
		for( uint i=0; i<texReg.Count(); i++ )
		{
			m->texIdA[m->texCount++] = texReg.EnumPbi( i );
		}

		m->cmdDrawA[cmdDrawCo++] = SU_DD_RET();
		m->cmdBBoxA[cmdBBoxCo++] = SU_DD_RET();
		m->cmdOutlA[cmdOutlCo++] = SU_DD_RET();
		
		SU_ASSERT( texSrfCo == upCodeTexCount );
		SU_ASSERT( m->srfCount == srfCount );
		SU_ASSERT( m->texCount == texCount );
		SU_ASSERT( cmdDrawCo == cmdDrawCount );
		SU_ASSERT( cmdBBoxCo == cmdBBoxCount );
		SU_ASSERT( cmdOutlCo == cmdOutlCount );
	}

	// Do not release vbo here if you want to keep it
	for( uint i=0; i<vboA.size(); i++ )
	{
		inDD->ResourceRelease( vboA[i] );		// vbo kept are retained
	}
	// Clean Build Pool
	vboA.reserve( 0 );
	texReg.Clean();
	Pool::Destroy( buildPool );
	
	return m;
}