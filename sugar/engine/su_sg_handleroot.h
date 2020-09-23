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


#ifndef _su_sg_handleroot_h_
#define _su_sg_handleroot_h_



namespace su { namespace sg
{
	struct Handle;

	struct Root: public Handle
	{
		static Root*		Create			(	SceneGraph*		inSG	);

		virtual SceneGraph*	GetSceneGraph	(							);

	protected:
		SceneGraph*	sg;
	};

} }



#endif // _su_sg_handleroot_h_
