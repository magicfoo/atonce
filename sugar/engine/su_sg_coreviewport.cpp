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
#include <sufw/directdriver/su_directdriver.h>

#include "su_sg_core.h"
#include "su_sg_coreviewport.h"
#include "su_sg_corecamera.h"
#include "su_sg_coreframe.h"
#include "su_sg_handle.h"
#include "su_sg_scenegraph.h"


using namespace su;
using namespace su::sg;



namespace
{
	Vec3	ray_direction		(	const Vec2&		inHomogeneous,
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


	static const Vec3 g_vtxNrmA[6] =
	{
		-Vec3::UNIT_X,
		-Vec3::UNIT_Y,
		-Vec3::UNIT_Z,
		 Vec3::UNIT_X,
		 Vec3::UNIT_Y,
		 Vec3::UNIT_Z
	};

	bool	get_face_projection	(	uint*			outPickFace,
									Vec3*			outProj,
									const Vec2&		inHCoord,
									const Matrix&	inProj,
									const Matrix&	inInvView		)
	{

		Vec3* camPos = inInvView.location();
		Vec3 mouseDir = ray_direction( inHCoord, inProj, inInvView );
		
		for( uint i=0; i<6; i++ )
		{
			if( Vec3Dot( &mouseDir, &(g_vtxNrmA[i]) ) < 0.0f )		// the plan is in front of ray direction
			{
				static const float hSize = 0.5f;

				Vec3 cVtx = g_vtxNrmA[i] * hSize;

				Vec3 cProj;
				if( intersection( &cProj, *camPos, mouseDir, cVtx, g_vtxNrmA[i] ) )
				{
					int im3 = i%3;
					bool bX = ( im3 == 0 ) ? true : ( cProj.x >= -hSize && cProj.x <= hSize );
					bool bY = ( im3 == 1 ) ? true : ( cProj.y >= -hSize && cProj.y <= hSize );
					bool bZ = ( im3 == 2 ) ? true : ( cProj.z >= -hSize && cProj.z <= hSize );
					if( bX && bY && bZ )
					{
						*outPickFace = i;
						*outProj = cProj;
						return true;
					}
				}
				
			}
		}
		return false;
	}

	inline
	Vec2	arcball_hcoord	(	const Vec2&		inPixCoord,
								const Vec4&		inABRect		)
	{
		float rad	= inABRect.z * 0.5f;
		Vec2 center	= Vec2( inABRect.x + rad, inABRect.y + rad );
		return Vec2( inPixCoord.x-center.x, center.y-inPixCoord.y ) / rad;
	}
}




Viewport*	Viewport::Create		(	Handle*			inHandle	)
{
	SU_ASSERT( inHandle );
	SU_ASSERT( inHandle->GetSceneGraph() );
	Pool* pool = inHandle->GetSceneGraph()->GetPool();
	SU_ASSERT( pool );

	Viewport* nv = pool->New<Viewport>();
	SU_ASSERT( nv );
	SU_REQUIRE_OR_NULL( nv );

	nv->refCount = 1;
	nv->handle	 = NULL;
	nv->pool	 = pool;
	nv->camera	 = NULL;
	nv->rect	 = Vec4( 0.0f, 0.0f, 10.0f, 10.0f );

	nv->bshowWarp	= false;
	nv->warpTopCol	= Color32( 87, 72, 53 );
	nv->warpBotCol	= Color32( 57, 65, 70 );

	nv->backEnable	= false;

	nv->bshowGrid	= false;
	nv->gridTarget	= NULL;

	nv->bshowArcBall	= false;
	nv->arcBallTarget	= NULL;
	nv->arBallHLight	= -1;
	
	nv->bshowCube	 = false;
	nv->cubeHLDir	 = Vec3::ZERO;
	nv->cubeViewSize = 100;
	nv->cubeTarget	 = NULL;
	nv->cubeIsMoving = false;

	nv->tileListA.relocate( pool );
	nv->tileGroupA.relocate( pool );

	if( !nv->Init( inHandle ) )
	{
		pool->Delete( nv );
		return NULL;
	}

	SU_ASSERT( nv->handle == inHandle );
	return nv;
}


void		Viewport::SetRect			(	const Vec4&		inRect		)	{	rect = inRect;	}
Vec4		Viewport::GetRect			(								)	{	return rect;	}

void		Viewport::SetCamera			(	Camera*			inCamera	)
{
	if( camera )	camera->Release();
	camera = inCamera;
	if( camera )	camera->Retain();
}
Camera*		Viewport::GetCamera			(								)	{	return camera;	}






bool find_group_info	(	vector<sg::Viewport::TGInfo>::iterator&	outIt,
							const vector<Viewport::TGInfo>&			inArray,
							int										inGpIdx		)
{
	vector<Viewport::TGInfo>::iterator it;
	const vector<Viewport::TGInfo>::iterator& itEnd = inArray.end();
	for( it = inArray.begin(); it != itEnd; it++ )
	{
		if( it->groupIdx == inGpIdx )
		{
			outIt = it;
			return true;
		}
	}
	return false;
}


//
// --- TileList

int			Viewport::AttachTileList	(	dd::TileList*	inTileList,
											uint			inGroupIdx	)
{
	static int s_id = 0;
	s_id++;

	TLInfo nm;
	nm.tl		= inTileList;
	nm.pbi		= dd::INVALID_IDX;
	nm.idx		= s_id;
	nm.zOrder	= 0;
	nm.group	= inGroupIdx;
	nm.show		= true;

	tileListA.push_back( nm );

	vector<TGInfo>::iterator gpIt;
	if( find_group_info( gpIt, tileGroupA, inGroupIdx ) )
	{
		gpIt->tlCount++;
		gpIt->dirty = true;
	}
	else
	{
		TGInfo ng;
		ng.groupIdx = inGroupIdx;
		ng.tlCount = 1;
		ng.dirty = true;
		ng.upLoc = false;
		ng.ox = 0;
		ng.oy = 0;
		tileGroupA.push_back( ng );
	}

	return nm.idx;
}

void		Viewport::DetachTileList	(	int				inIdx		)
{
	
	vector<TLInfo>::iterator it;
	const vector<TLInfo>::iterator& itEnd = tileListA.end();
	for( it = tileListA.begin(); it != itEnd; it++ )
		if( it->idx == inIdx )
			break;

	if( it != itEnd )
	{
		vector<TGInfo>::iterator gpIt;
		if( find_group_info( gpIt, tileGroupA, it->group ) )
			if( gpIt->tlCount == 1 )
				tileGroupA.erase( gpIt );

		tileListA.erase( it );
	}
}

void		Viewport::ShowTileList		(	int				inIdx,
											bool			inOnOff	)
{
	vector<TLInfo>::iterator it;
	const vector<TLInfo>::iterator& itEnd = tileListA.end();
	for( it = tileListA.begin(); it != itEnd; it++ )
	{
		if( it->idx == inIdx )
		{
			if( it->show != inOnOff )
			{
				it->show	= inOnOff;

				vector<TGInfo>::iterator gpIt;
				if( find_group_info( gpIt, tileGroupA, it->group ) )
				{
					gpIt->dirty = true;
				}
			}
			break;
		}
	}
}

void		Viewport::SetTileListOrder	(	int				inIdx,
											int				inZOrder	)
{
	vector<TLInfo>::iterator it;
	const vector<TLInfo>::iterator& itEnd = tileListA.end();
	for( it = tileListA.begin(); it != itEnd; it++ )
	{
		if( it->idx == inIdx )
		{
			it->zOrder = inZOrder;
			break;
		}
	}
}

void		Viewport::SetTileListBitmap	(	int				inIdx,
											uint			inPbi		)
{
	vector<TLInfo>::iterator it;
	const vector<TLInfo>::iterator& itEnd = tileListA.end();
	for( it = tileListA.begin(); it != itEnd; it++ )
	{
		if( it->idx == inIdx )
		{
			it->pbi = inPbi;
			break;
		}
	}
}


void		Viewport::GetTileList		(	TLInfo**		outArray,
											uint*			outCount	)
{
	if( outArray )	*outArray = tileListA.data();
	if( outCount )	*outCount = tileListA.size();
}

void		Viewport::SetTileGoupDirty		(	int				inGpIdx		)
{
	vector<TGInfo>::iterator gpIt;
	if( find_group_info( gpIt, tileGroupA, inGpIdx ) )
	{
		gpIt->dirty = true;
	}
}
void		Viewport::LocateTileGroup         (	int				inGpIdx,
												short			inOx,
                                                short           inOy        )
{
	vector<TGInfo>::iterator gpIt;
	if( find_group_info( gpIt, tileGroupA, inGpIdx ) )
	{
		gpIt->upLoc = true;
		gpIt->ox = inOx;
		gpIt->oy = inOy;
	}
}

void		Viewport::GetTileGroup		(	TGInfo**		outArray,
											uint*			outCount	)
{
	if( outArray )	*outArray = tileGroupA.data();
	if( outCount )	*outCount = tileGroupA.size();
}



//
// --- Grid

void		Viewport::ShowGrid			(	bool			inOnOff		)	{	bshowGrid = inOnOff;		}
bool		Viewport::IsGridShown		(								)	{	return bshowGrid;			}
void		Viewport::SetGridTarget		(	Handle*			inHandle	)
{
	if( gridTarget )	gridTarget->Release();
	gridTarget = inHandle;
	if( gridTarget )	gridTarget->Retain();
}
Handle*		Viewport::GetGridTarget		(								)	{	return gridTarget;			}

void		Viewport::ShowWarp			(	bool			inOnOff		)	{	bshowWarp = inOnOff;		}
bool		Viewport::IsWarpShown		(								)	{	return bshowWarp;			}

void		Viewport::SetWarpColor		(	const Color32&	inTop,
											const Color32&	inBottom	)
{
	warpTopCol = inTop;
	warpBotCol = inBottom;
}

void		Viewport::GetWarpColor		(	Color32*		outTop,
											Color32*		outBot		)
{
	if( outTop )	*outTop = warpTopCol;
	if( outBot )	*outBot = warpBotCol;
}

void				Viewport::EnableBackground	(	uint			inImgWidth,
													uint			inImgHeight,
													uint			inPbi		)
{
	backInfo.width	= inImgWidth;
	backInfo.height	= inImgHeight;
	backInfo.pbi	= inPbi;
	backEnable = true;
}
void				Viewport::DisableBackground	(								)
{
	backEnable = false;
}
Viewport::BackInfo*	Viewport::GetBackground		(								)
{
	return ( ( backEnable ) ? &backInfo : NULL );
}


//
// --- Arcball
void		Viewport::ShowArcBall		(	bool			inOnOff		)	{	bshowArcBall = inOnOff;		}
bool		Viewport::IsArcBallShown	(								)	{	return bshowArcBall;		}
Vec4		Viewport::GetArcBallRect	(								)
{
	float size = 0.75f * Min( rect.z, rect.w );
	float sx = ( rect.z - size ) * 0.5f;
	float sy = ( rect.w - size ) * 0.5f;
	return Vec4( sx, sy, size, size );
}
void		Viewport::SetArcBallTarget	(	Frame*			inHandle	)
{
	if( arcBallTarget )	arcBallTarget->Release();
	arcBallTarget = inHandle;
	if( arcBallTarget )
	{
		arcBallTarget->Retain();
		arcBall.Reset( arcBallTarget->GetRotation() );
	}
}
Frame*		Viewport::GetArcBallTarget	(								)	{	return arcBallTarget;		}


void		Viewport::BeginArcBall			(	const Vec2&		inCoord		)
{
	if( arcBallTarget )
	{
		arcBall.Reset( arcBallTarget->GetRotation() );
	}

	Vec2 lAxis = Vec2::ZERO;
	int abhl = GetHightlightArcBall();
	if( abhl != -1 )
	{
		if( abhl%2 == 0 )	lAxis = Vec2::UNIT_Y;
		else				lAxis = Vec2::UNIT_X;
	}

	Time ct;
	clock::GetTime( &ct );
	arcBall.Begin( ct.asSecond(), arcball_hcoord( inCoord, GetArcBallRect() ), lAxis );
}
void		Viewport::MoveArcBall			(	const Vec2&		inCoord		)
{
	Time ct;
	clock::GetTime( &ct );
	arcBall.Move( ct.asSecond(), arcball_hcoord( inCoord, GetArcBallRect() ) );
}
void		Viewport::EndArcBall			(	const Vec2&		inCoord		)
{
	Time ct;
	clock::GetTime( &ct );
	arcBall.End( ct.asSecond(), arcball_hcoord( inCoord, GetArcBallRect() ) );
}
Quat		Viewport::GetArcBallRot			(								)
{
	Time ct;
	clock::GetTime( &ct );
	return arcBall.GetState( ct.asSecond() );
}

int		Viewport::PickArcBallTrigger		(	const Vec2&		inCoord		)
{
	Vec4 abRect = GetArcBallRect();
	float rad	= abRect.z * 0.5f;
	Vec2 center	= Vec2( abRect.x + rad, abRect.y + rad );

	float rs  = rad * 0.05f;
	float rhs = rs * 0.5f;
	const Vec2 recCenterA[4] = 
	{
		Vec2( rad, 0.0f ),
		Vec2( 0.0f, rad ),
		Vec2(-rad, 0.0f ),
		Vec2( 0.0f,-rad )
	};
	for( int i=0; i<4; i++ )
	{
		Vec2 cCenter = center + recCenterA[i];

		if( ( inCoord.x >= ( cCenter.x - rhs ) ) &&
			( inCoord.x <= ( cCenter.x + rhs ) ) &&
			( inCoord.y >= ( cCenter.y - rhs ) ) &&
			( inCoord.y <= ( cCenter.y + rhs ) ) )
		{
			return i;
		}
	}

	return -1;
}
void		Viewport::HighlightArcBallTrigger	(	int			inIdx			)	{	arBallHLight = inIdx;	}
int			Viewport::GetHightlightArcBall		(								)	{	return arBallHLight;	}



//
// --- Cube Helper
void		Viewport::ShowHCube				(	bool			inOnOff		)	{	bshowCube = inOnOff;		}
bool		Viewport::IsHCubeShown			(								)	{	return bshowCube;			}

Vec4		Viewport::GetHCubeRect			(								)
{
	return Vec4( rect.z - cubeViewSize, 0.0f, cubeViewSize, cubeViewSize );
}
void		Viewport::ScaleHCube			(	float			inScale		)
{
	cubeViewSize *= inScale;
}

void		Viewport::SetHCubeTarget		(	Frame*			inHandle	)
{
	if( cubeTarget )	cubeTarget->Release();
	cubeTarget = inHandle;
	if( cubeTarget )	cubeTarget->Retain();
}
Frame*		Viewport::GetHCubeTarget		(								)	{	return cubeTarget;		}



Vec3		Viewport::PickHCubeTrigger		(	const Vec2&		inCoord		)
{
	SU_REQUIRE_OR_RETURN_V( cubeTarget, Vec3::ZERO );

	// Compute coord in cube viewport space
	float cx = inCoord.x - (rect.z - cubeViewSize );
	float cy = inCoord.y;

	SU_REQUIRE_OR_RETURN_V( cx > 0.0f && cx < cubeViewSize, Vec3::ZERO );
	SU_REQUIRE_OR_RETURN_V( cy > 0.0f && cy < cubeViewSize, Vec3::ZERO );


	Matrix invView;
	handle->GetSceneGraph()->BuildWorldTR( &invView, cubeTarget->GetHandle() );
	Quat invViewRot;
	MatrixGetTR( NULL, &invViewRot, NULL, &invView );

	Matrix invViewCube, projCube;
	MatrixTranslation	( &invViewCube, 0.0f, 0.0f, -1.6f );
	MatrixRotate		( &invViewCube, &invViewCube, &invViewRot );
	MatrixPerspectiveLH	( &projCube, Pi*0.4f, 1.0f, 0.01f, 5.0f );


	// and homogeneous
	Vec2 hCoord( ( ( ( cx / cubeViewSize ) * 2.0f ) - 1.0f ),
				-( ( ( cy / cubeViewSize ) * 2.0f ) - 1.0f ) );

	uint hitFace;
	Vec3 projection;
	if( get_face_projection( &hitFace, &projection, hCoord, projCube, invViewCube ) )
	{
		uint fm3 = hitFace%3;
		float pos = (hitFace<3) ?-0.5f : 0.5f;
		Vec3 locA[4];
		Vec2 coA[] = { Vec2( -0.5f, 0.5f ), Vec2( -0.5f, -0.5f ), Vec2( 0.5f, -0.5f ), Vec2( 0.5f, 0.5f ) };
		for( uint i=0; i<4; i++ )
		{
			if( fm3 == 0 )		locA[i] = Vec3( pos, coA[i].x, coA[i].y );
			else if( fm3 == 1 )	locA[i] = Vec3( coA[i].x, pos,-coA[i].y );
			else if( fm3 == 2 )	locA[i] = Vec3( coA[i].x, coA[i].y, pos );
		}

		// Test for vertex
		uint nearVtxIdx = 0;
        Vec3 tmp;
		for( ; nearVtxIdx<4; nearVtxIdx++ )
        {
            tmp = locA[nearVtxIdx] - projection;
			if( Vec3Norm( &tmp ) <= 0.3f )
				break;
        }
        
		if( nearVtxIdx < 4 )
		{
			// Vertex
			return locA[nearVtxIdx];
		}
		else
		{
			for( uint i=0; i<4; i++ )
			{
				uint i1 = (i+1)%4;
				
				Vec3 d0 = locA[i1] - locA[i];
				Vec3 d1 = projection - locA[i];
				float dot = Vec3Dot( &d1, &d0 );
				Vec3 P = locA[i] + dot * d0;

                Vec3 dl = projection - P;
				if( Vec3Norm( &dl ) <= 0.2f )
				{
					// Edge
					return locA[i] + ( d0 * 0.5f );
				}
			}

			// Quad
			return g_vtxNrmA[hitFace] * 0.5f;
		}
	}

	return Vec3::ZERO;
}
void		Viewport::HighlightHCubeTrigger	(	const Vec3&		inDir		)	{	cubeHLDir = inDir;	}
Vec3		Viewport::GetHightlightHCubeDir	(								)	{	return cubeHLDir;	}


void		Viewport::MoveHCubeTo			(	const Vec3&		inDir		)
{
	SU_REQUIRE_OR_RETURN( cubeTarget );
	SU_REQUIRE_OR_RETURN( !Vec3Near( &inDir, &(Vec3::ZERO), 0.1f ) );

	cubeIsMoving = true;
	cubeMoveR0 = cubeTarget->GetRotation();

	// cubeMoveR1
	{
		Vec3 idi;
		Vec3Normalize( &idi, &inDir );
		idi = -idi;
		Vec3 up( Vec3::UNIT_Y ), right;
		float d = Vec3Dot( &up, &idi );
		if( d == 1.0f || d == -1.0f )
			up = Vec3::UNIT_Z * (-d);

		Vec3Normalize( Vec3Cross( &right, &up, &idi ), &right );
		Vec3Normalize( Vec3Cross( &up, &idi, &right ), &up );
		Matrix mr;
		MatrixSetAxis( MatrixIdentity( &mr ), &right, &up, &idi );
		QuatFromMatrix( &cubeMoveR1, &mr );
		QuatNormalize( &cubeMoveR1, &cubeMoveR1 );
	}
	
	Time ct;
	clock::GetTime( &ct );
	cubeMoveT0 = ct.asSecond();
}

bool		Viewport::IsHCubeMoving			(								)
{
	return cubeIsMoving;
}
Quat		Viewport::GetHCubeRot			(								)
{
	static const float hcDuration = 0.5f;
	if( cubeIsMoving )
	{
		Time ct;
		clock::GetTime( &ct );
		float dt = ct.asSecond() - cubeMoveT0;
		if( dt >= hcDuration )
		{
			cubeIsMoving = false;

			if( arcBallTarget == cubeTarget )
				arcBall.Reset( cubeMoveR1 );

			return cubeMoveR1;
		}
		else
		{
			float co = ( dt / hcDuration );
			co = (co * Pi) - (HalfPi);							// [0; 1] -> [-Pi/2; Pi/2]
			co = Sinf( co );
			co = (co+1.0f) / 2.0f;								// [-1; 1] -> [0; 1]

			Quat r;
			QuatSlerp( &r, &cubeMoveR0, &cubeMoveR1, co );

			if( arcBallTarget == cubeTarget )
				arcBall.Reset( cubeMoveR1 );

			return r;
		}
	}
	return Quat::UNIT;
}

void		Viewport::Shut			(								)
{
	tileListA.reserve( 0 );
	tileGroupA.reserve( 0 );

	SafeRelease( camera );
	SafeRelease( gridTarget );
	SafeRelease( arcBallTarget );
	SafeRelease( cubeTarget );
}
