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


#ifndef _su_sg_coreviewport_h_
#define _su_sg_coreviewport_h_

#include "su_sg_scenegraph_enum.h"


namespace su { namespace dd { struct TileList; } }

namespace su { namespace sg
{
	struct Handle;
	struct Camera;
	struct Frame;

	struct Viewport: public Core
	{
		struct TLInfo
		{
			dd::TileList*	tl;
			int				pbi;
			int				idx;
			int				zOrder;
			bool			show;
			uint			group;
		};

		struct TGInfo
		{
			int		groupIdx;
			int		tlCount;
			bool	dirty;
			bool	upLoc;
			short	ox, oy;
		};

		struct BackInfo
		{
			int		width;
			int		height;
			int		pbi;
		};

		static const uint Type = 0x479789fa;		// su::sg::Viewport

		static Viewport*	Create					(	Handle*			inHandle	);

		void				SetRect					(	const Vec4&		inRect		);
		Vec4				GetRect					(								);

		void				SetCamera				(	Camera*			inCamera	);
		Camera*				GetCamera				(								);


		// Tile List
		int					AttachTileList			(	dd::TileList*	inTileList,
														uint			inGroupIdx	);
		void				DetachTileList			(	int				inIdx		);
        
		void				ShowTileList			(	int				inIdx,
														bool			inOnOff		);
		void				SetTileListOrder		(	int				inIdx,
														int				inZOrder	);
		void				SetTileListBitmap		(	int				inIdx,
														uint			inPbi		);
        
		void				GetTileList				(	TLInfo**		outArray,
                                                        uint*			outCount	);
        
        // Group op
		void				SetTileGoupDirty		(	int				inGpIdx		);
		void				LocateTileGroup         (	int				inGpIdx,
														short			inOx,
                                                        short           inOy        );
		
		void				GetTileGroup			(	TGInfo**		outArray,
                                                        uint*			outCount	);




		// Grid
		void				ShowGrid				(	bool			inOnOff		);
		bool				IsGridShown				(								);
		void				SetGridTarget			(	Handle*			inHandle	);
		Handle*				GetGridTarget			(								);


		// Warp
		void				ShowWarp				(	bool			inOnOff		);
		bool				IsWarpShown				(								);
		void				SetWarpColor			(	const Color32&	inTop,
														const Color32&	inBottom	);
		void				GetWarpColor			(	Color32*		outTop,
														Color32*		outBottom	);


		void				EnableBackground		(	uint			inImgWidth,
														uint			inImgHeight,
														uint			inPbi		);
		void				DisableBackground		(								);
		BackInfo*			GetBackground			(								);


		// Arcball
		void				ShowArcBall				(	bool			inOnOff		);
		bool				IsArcBallShown			(								);
		Vec4				GetArcBallRect			(								);
		void				SetArcBallTarget		(	Frame*			inFrame		);
		Frame*				GetArcBallTarget		(								);

		int					PickArcBallTrigger		(	const Vec2&		inCoord		);
		void				HighlightArcBallTrigger	(	int				inIdx		);
		int					GetHightlightArcBall	(								);

		void				BeginArcBall			(	const Vec2&		inCoord		);
		void				MoveArcBall				(	const Vec2&		inCoord		);
		void				EndArcBall				(	const Vec2&		inCoord		);
		Quat				GetArcBallRot			(								);

		// Cube Helper
		void				ShowHCube				(	bool			inOnOff		);
		bool				IsHCubeShown			(								);

		Vec4				GetHCubeRect			(								);
		void				ScaleHCube				(	float			inScale		);

		void				SetHCubeTarget			(	Frame*			inFrame		);
		Frame*				GetHCubeTarget			(								);

		Vec3				PickHCubeTrigger		(	const Vec2&		inCoord		);
		void				HighlightHCubeTrigger	(	const Vec3&		inDir		);
		Vec3				GetHightlightHCubeDir	(								);
		void				MoveHCubeTo				(	const Vec3&		inDir		);
		bool				IsHCubeMoving			(								);
		Quat				GetHCubeRot				(								);


		virtual uint		GetType					(								)	{	return Type;	}
		virtual bool		IsType					(	uint			inType		)	{	return Type == inType;	}

	protected:
		Camera*		camera;
		Vec4		rect;

		bool		bshowGrid;
		Handle*		gridTarget;

		bool		bshowWarp;
		Color32		warpTopCol;
		Color32		warpBotCol;

		BackInfo	backInfo;
		bool		backEnable;

		bool		bshowArcBall;
		Frame*		arcBallTarget;
		ArcBall		arcBall;
		int			arBallHLight;

		bool		bshowCube;
		Vec3		cubeHLDir;
		uint		cubeViewSize;
		Frame*		cubeTarget;

		bool		cubeIsMoving;
		float		cubeMoveT0;
		Quat		cubeMoveR0, cubeMoveR1;



		
		vector<TLInfo>	tileListA;
		vector<TGInfo>	tileGroupA;


		virtual void		Shut			(								);
	};

} }



#endif // _su_sg_coreviewport_h_
