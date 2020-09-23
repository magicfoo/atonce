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


#ifndef _su_sg_scenegraph_h_
#define _su_sg_scenegraph_h_

#include "su_sg_scenegraph_enum.h"


namespace su{ struct IDirectDraw; }

namespace su { namespace sg
{
	struct SceneGraph
	{
	public:
		static SceneGraph*	Create			(	Pool*				inPool,
												IDirectDraw*		inDD			);
		void				Release			(										);

		void				SetView			(	uint				inViewPbi,
												const Vec2&			inViewSize		);
		uint				GetView			(										);

		bool				Draw			(										);
		
		Pool*				GetPool			(										);

		Handle*				GetRoot			(										);
		byte				GetLayerId		(										);
		
		IDirectDraw*		GetDirectDraw	(										);

		Matrix*				BuildWorldTR	(	Matrix*				outTR,
												Handle*				inHandle		);

		Box3*				BuildBBox		(	Box3*				outBox,
												Handle*				inHandle		);

		Handle*				Pick			(	Viewport*			inViewport,
												const Vec2&			inCoord			);

		
		void				Pick			(	vector<Handle*>*	ioHandleA,
												Viewport*			inViewport,
												const Vec2&			inCoord,
												const Vec2&			inSize			);

		void				SetRenderMode	(	RenderMode			inRenderMode	);
		RenderMode			GetRenderMode	(										);


		
		enum PostFxValue
		{
			PFV_BlurRadius,
			PFV_BlurBlendFactor,
		};


		void				EnablePostFx	(	bool				inOnOff			);

		void				SetPostFxValuef	(	PostFxValue			inOpt,
												float				inVal			);

		void				SetPostFxCode	(	pcstr				inPixelCode		);


		
		bool				SetCameraVideoMode	(	bool			inOnOff         );




		struct PostProcessUtil
		{
			Pool*			pool;
			IDirectDraw*	dd;

			uint	w, h;
			
			struct Mip		// start to w/2, h/2
			{
				int		rt;
				uint	w, h;
			};
			Mip*	mipA;
			uint	mipCount;
		};

		struct BackQuad
		{
			uint w, h;
			uint quad;
		};


	protected:

		virtual void		OnEvent			(	Handle*				inHandle,
												EventReason			inReason		);
		virtual void		OnEvent			(	Core*				inCore,
												EventReason			inReason		);



		Pool*				pool;

		Root*				root;
		byte				layerid;

		IDirectDraw*		ddraw;
		uint				viewPbi;
		Vec2				viewSize;

		RenderMode			renderMode;
		
		vector<uint>		opCodeA;
		vector<Viewport*>	viewportA;



		//
		// --- resources ( warp, grid, cube... )

		uint				noisePbi;
		
		uint				gridSrf;
		uint				warpSrf;

		uint				abSrfA[2];
		uint				abVboRect;
		int					abRectHL;

		uint				cubeSrf;
		uint				cubePbi;
		uint				cubeHLSrfA[3];
		uint				cubeHLVboA[3];

		uint				pickRT;
		uint				handleCount;

		bool				pFxEnable;
		uint				pFxProgId;
		float				pFxBlurBlendFactor;
		float				pFxBlurRadius;
		bool				pFxBlurOpCodeDirty;
		uint				pFxBlurOpCodeA[256];
		
		float				pFxBlurMipBlend_int;
		uint				pFxBlurPbi_int;


		int					mainRT;

		uint				quadSS;
		uint				quadFS;
		
		
		vector<BackQuad>	backQuadA;
		
		PostProcessUtil		ppu;

        bool                onCameraVideoMode;
        

		friend struct Handle;
		friend struct Root;
		friend struct Core;
	};

} }


#endif // _su_sg_scenegraph_h_
