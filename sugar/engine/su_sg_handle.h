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


#ifndef _su_sg_handle_h_
#define _su_sg_handle_h_



namespace su { namespace sg
{
	struct SceneGraph;
	struct Core;

	struct Handle
	{
	public:
		static Handle*		Create			(	Handle*			inParent	);
		
		void				Retain			(								);
		uint				CountRef		(								);
		void				Release			(								);
		
		Handle*				GetParent		(								);
		uint				CountChild		(								);
		Handle*				EnumChild		(	uint			inIdx		);
		
		void				ReleaseChildren	(								);

		bool				ReleaseChild	(	Handle*			inHandle	);

		virtual SceneGraph*	GetSceneGraph	(								);
		
		Core*				GetCore			(								);
		Pool*				GetPool			(								);

		uint				GetAttributes	(								);
		void				SetAttributes	(	uint						);

		void				Show			(								);
		void				Hide			(								);

	protected:
		Pool*		pool;
		dg::Shadow*	shadow;
		Core*		core;
		uint		refCount;

		uint		attr;


		bool				SetCore			(	Core*			inCore		);
		bool				AddChild		(	Handle*			inChild		);
		
		virtual bool		Init			(	Handle*			inParent,
												Pool*			inPool		);
		virtual void		Shut			(								);
		
		friend struct Core;
	};

} }


#endif // _su_sg_handle_h_
