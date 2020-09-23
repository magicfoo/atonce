/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2008 AtOnce Technologies
**
** This file is part of the Sugar Framework.
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



#ifndef _su_app_h_
#define _su_app_h_




namespace su { namespace core
{

	su_ie bool			LoadProfile				(	pcstr			inFilename,
													pcstr			inSectionname		);

	su_ie bool			LoadProfileFromMemory	(	byte*			inContentBegin,
													byte*			inContentEnd,
													pcstr			inSectionname		);

	su_ie bool			Init					(										);
	su_ie void			Update					(										);
	su_ie void			Shut					(										);

	su_ie void			Exit					(	int				inCode				);

	su_ie void			ProcessEvent			(										);
	su_ie void			WaitEvent				(										);
	su_ie void			SetWaitInterval			(	uint			inMsTimeout			);

	su_ie void			Sleep					(	uint			inMsTimeout			);

	su_ie pcstr			GetBuildLevel			(										);
	su_ie pcstr			GetBuildPlatform		(										);
	su_ie pcstr			GetBuildRevisionNumber	(										);

} }




#endif // _su_app_h_



