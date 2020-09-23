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

#include "su_collection.h"

#include "su_sg_core.h"
#include "su_sg_corecollection.h"
#include "su_sg_coreframe.h"
#include "su_sg_coregeometry.h"
#include "su_sg_corecamera.h"

#include "su_sg_handle.h"
#include "su_sg_scenegraph.h"


using namespace su;
using namespace su::sg;



namespace
{
	struct MeshDef
	{
		uint				id;
		collection::Mesh*	mesh;
	};

	collection::Mesh*
			find_mesh		(	const vector<MeshDef>&	inMeshDefA,
								uint					inMeshId	)
	{
		uint mc = inMeshDefA.size();
		for( uint i=0; i<mc; i++ )
			if( inMeshDefA[i].id == inMeshId )
				return inMeshDefA[i].mesh;
		return NULL;
	}

	bool	find_list_idx	(	uint*					ioIdxLst,
								pcstr					inListName,
								collection::Resource*	inRsc		)
	{
		uint lstCount = inRsc->countlist();
		for( uint i=0; i<lstCount; i++ )
		{
			collection::List lst = inRsc->enumlist( i );
			if( util::Strstartswith( lst.name, inListName ) )
			{
				*ioIdxLst = i;
				return true;
			}
		}
		return false;
	}


#define SAFE_PARENTIDX( T, it, op )	\
	if( it.code == T::CODE )	{	T ct;	it.load( ct );	*op = ct.parentidx;	return true;	}
	
#define SAFE_TRS( T, it, op )	\
	if( it.code == T::CODE )	{	T ct;	it.load( ct );	ct.trs.coeff.load( *op );	return true;	}
	

	bool	get_parentidx	(	int*					outPID,
								const collection::Tag&	inTag		)
	{
		SAFE_PARENTIDX( map::rsc_collection_litem_node,		inTag, outPID );
		SAFE_PARENTIDX( map::rsc_collection_litem_geometry,	inTag, outPID );
		SAFE_PARENTIDX( map::rsc_collection_litem_camera,	inTag, outPID );
		SAFE_PARENTIDX( map::rsc_collection_litem_light,	inTag, outPID );
		SAFE_PARENTIDX( map::rsc_collection_litem_helper,	inTag, outPID );
		SAFE_PARENTIDX( map::rsc_collection_litem_bone,		inTag, outPID );
		SAFE_PARENTIDX( map::rsc_collection_litem_character,inTag, outPID );
		SU_ERROR( "unknow litem type" );
		return false;
	}
	bool	get_trs			(	Matrix*					outTRS,
								const collection::Tag&	inTag		)
	{
		SAFE_TRS( map::rsc_collection_litem_node,		inTag, outTRS );
		SAFE_TRS( map::rsc_collection_litem_geometry,	inTag, outTRS );
		SAFE_TRS( map::rsc_collection_litem_camera,		inTag, outTRS );
		SAFE_TRS( map::rsc_collection_litem_light,		inTag, outTRS );
		SAFE_TRS( map::rsc_collection_litem_helper,		inTag, outTRS );
		SAFE_TRS( map::rsc_collection_litem_bone,		inTag, outTRS );
		SAFE_TRS( map::rsc_collection_litem_character,	inTag, outTRS );
		SU_ERROR( "unknow litem type" );
		return false;
	}

	uint	count_childnode	(	uint					inParentIdx,
								uint					inLstIdx,
								collection::Resource*	inRsc		)
	{
		collection::List lst = inRsc->enumlist( inLstIdx );
		uint chCount = 0;
		for( uint i=0; i<lst.len; i++ )
		{
			collection::Tag cItem = inRsc->iterlist( inLstIdx, i );

			int pIdx;
			if( get_parentidx( &pIdx, cItem ) && ( pIdx == inParentIdx ) )
			{
				chCount++;
			}
		}
		return chCount;
	}

	bool	enum_childnode	(	uint*					ioTagLstIdx,
								uint					inChildIdx,
								uint					inParentIdx,
								uint					inLstIdx,
								collection::Resource*	inRsc		)
	{
		collection::List lst = inRsc->enumlist( inLstIdx );
		uint chCount = 0;
		for( uint i=0; i<lst.len; i++ )
		{
			collection::Tag cItem = inRsc->iterlist( inLstIdx, i );

			int pIdx;
			if( get_parentidx( &pIdx, cItem ) && ( pIdx == inParentIdx ) )
			{
				if( chCount == inChildIdx )
				{
					*ioTagLstIdx = i;
					return true;
				}
				chCount++;
			}
		}
		return false;
	}


	
	bool	create_core_from_tag(	const collection::Tag&	inTag,
									Handle*&				ioHandle,
									collection::Resource*	inRsc,
									const vector<MeshDef>&	inMeshDefA	)
	{
		SU_ASSERT( inTag.code != map::rsc_collection_litem_node::CODE );
		SU_REQUIRE_OR_NULL( inTag.code != map::rsc_collection_litem_node::CODE );

		switch( inTag.code )
		{
			case map::rsc_collection_litem_geometry::CODE:
			{
				map::rsc_collection_litem_geometry cg;
				inTag.load( cg );
			
				//
				// --- Add Offset TR
				{
					Handle* nh = Handle::Create( ioHandle );
					SU_ASSERT( nh );

					Matrix tr;
					cg.offset.coeff.load( tr );

					Frame* frameC = Frame::Create( nh );
					frameC->SetTR( tr );
					frameC->Release();


					ioHandle->Release();
					ioHandle = nh;
				}

				//
				// --- Add mesh
				{
					Handle* nh = Handle::Create( ioHandle );
					SU_ASSERT( nh );

					Geometry* ng = Geometry::Create( nh );
					SU_ASSERT( ng );

					// Vec4 col( cg.color.get( 0 ), cg.color.get( 1 ), cg.color.get( 2 ), cg.color.get( 3 ) );
					// col *= * 1.0f/255.0f;

					collection::Mesh* mesh = find_mesh( inMeshDefA, cg.meshid );
					SU_ASSERT( mesh );
					ng->SetMesh( mesh );

					SU_ASSERT( ng->CountRef() == 2 );
					ng->Release();

					ioHandle->Release();
					ioHandle = nh;
				}

				break;
			}

			case map::rsc_collection_litem_camera::CODE:
			{
				map::rsc_collection_litem_camera cc;
				inTag.load( cc );

				Handle* nh = Handle::Create( ioHandle );
				SU_ASSERT( nh );

				Camera* nc = Camera::Create( nh );
				SU_ASSERT( nc );

				nc->SetPerspective( cc.fov, Vec2( cc.envrange.get( 0 ), cc.envrange.get( 1 ) ) );

				ioHandle->Release();
				ioHandle = nh;

				break;
			}
			case map::rsc_collection_litem_light::CODE:		{	SU_BREAK();		break;	}		// TODO
			case map::rsc_collection_litem_helper::CODE:	{	SU_BREAK();		break;	}		// TODO
			case map::rsc_collection_litem_bone::CODE:		{	SU_BREAK();		break;	}		// TODO
			case map::rsc_collection_litem_character::CODE:	{	SU_BREAK();		break;	}		// TODO
		}
		return true;
	}

	void	build_node_rec	(	Handle*					inFather,
								const Matrix&			inGlobal,
								uint					inCTagIdx,
								uint					inLstIdx,
								collection::Resource*	inRsc,
								const vector<MeshDef>&	inMeshDefA	)
	{
		SU_ASSERT( inFather );
		collection::Tag ct = inRsc->iterlist( inLstIdx, inCTagIdx );


		Handle* cHnd = Handle::Create( inFather );
		SU_ASSERT( cHnd );

		Matrix cglob;

		//	cc.name
		//	cc.note
		get_trs( &cglob, ct );


		// Add Frame Core
		{
			Matrix invFather;
			MatrixFastInverse( &invFather, &inGlobal );
			Matrix clocal = cglob * invFather;

			Frame* frameC = Frame::Create( cHnd );
			frameC->SetTR( clocal );
			frameC->Release();
		}

		if( ct.code != map::rsc_collection_litem_node::CODE )
		{
			bool bSucc = create_core_from_tag( ct, cHnd, inRsc, inMeshDefA );
			SU_ASSERT( bSucc );
		}

		// recursive call
		uint cc = count_childnode( inCTagIdx, inLstIdx, inRsc );
		for( uint i=0; i<cc; i++ )
		{
			uint tagLstIdx;
			bool bEnumCh = enum_childnode( &tagLstIdx, i, inCTagIdx, inLstIdx, inRsc );
			SU_ASSERT( bEnumCh );
			if( bEnumCh )
			{
				build_node_rec( cHnd, cglob, tagLstIdx, inLstIdx, inRsc, inMeshDefA );
			}
		}
		
		SU_ASSERT( cHnd->CountRef() == 2 );
		cHnd->Release();
	}
}






Collection*	Collection::Create		(	Handle*		inHandle			)
{
	SU_ASSERT( inHandle );
	SU_ASSERT( inHandle->GetSceneGraph() );
	Pool* pool = inHandle->GetSceneGraph()->GetPool();
	SU_ASSERT( pool );

	Collection* nf = pool->New<Collection>();
	SU_ASSERT( nf );
	SU_REQUIRE_OR_NULL( nf );

	nf->refCount = 1;
	nf->handle	 = NULL;
	nf->pool	 = pool;
	nf->rsc		 = NULL;

	if( !nf->Init( inHandle ) )
	{
		pool->Delete( nf );
		nf = NULL;
	}
	else
	{
		SU_ASSERT( nf->handle == inHandle );
	}
	return nf;
}

bool		Collection::Realize		(	collection::Resource*	inRsc	)
{
	Unrealize();
	SU_REQUIRE_OR_FALSE( inRsc );

	SU_REQUIRE_OR_FALSE( handle );
	SU_ASSERT( handle->CountChild() == 0 );

	SceneGraph* sg = handle->GetSceneGraph();
	SU_ASSERT( sg );
	SU_REQUIRE_OR_FALSE( sg );


	vector<MeshDef> meshDefA( pool );

	uint defCount = inRsc->countdef();
	for( uint i=0; i<defCount; i++ )
	{
		collection::Define cDef	= inRsc->enumdef( i );
		collection::Tag	   dTag	= inRsc->enumtag( cDef.tag.index+1 );
				
		if		( dTag.code == map::rsc_collection_bitmap::CODE )			{}
		else if	( dTag.code == map::rsc_collection_shading_effect::CODE )	{}
		else if	( dTag.code == map::rsc_collection_mesh::CODE )
		{
			collection::Mesh* m = collection::create_mesh( inRsc, dTag, sg->GetDirectDraw() );
			if( m )
			{
				MeshDef md;
				md.id = cDef.id;
				md.mesh = m;
					
				meshDefA.push_back( md );
			}
		}
	}

	uint lstIdx;
	SU_REQUIRE_OR_FALSE( find_list_idx( &lstIdx, "display", inRsc ) );

	Matrix ident;
	MatrixIdentity( &ident );
	build_node_rec( handle, ident, 0, lstIdx, inRsc, meshDefA );

	for( uint i=0; i<meshDefA.size(); i++ )
	{
		SU_ASSERT( meshDefA[i].mesh );
		meshDefA[i].mesh->release();		// used mesh are retain
	}



	rsc = inRsc;
	rsc->retain();
	return true;
}

void		Collection::Unrealize	(									)
{
	if( rsc )
	{
		handle->ReleaseChildren();
		uint cc = handle->CountChild();
		rsc->release();
		rsc = NULL;
	}
}

void		Collection::Shut		(									)
{
	Unrealize();
}

