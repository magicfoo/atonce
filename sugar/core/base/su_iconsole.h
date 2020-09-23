/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2009 AtOnce Technologies
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



#ifndef _su_iconsole_h_
#define _su_iconsole_h_



namespace su
{

	struct IConsole : public IInterface
	{

		SU_DECL_INTERFACE( IInterface, 0xC7DAE87DU )

		enum Action
		{
			A_DONTCARE	= (0),
			A_CONTINUE	= (1),
			A_BREAK		= (2),
			A_EXIT		= (3)
		};

		virtual Action  AskUserAction	(	pcstr  inMessage,
											pcstr  inFilename = NULL,
											pcstr  inFctname  = NULL,
											int    inLineno   = -1    ) = 0;

		virtual bool    Output			(   pcstr  inMessage,
											pcstr  inFilename = NULL,
											pcstr  inFctname  = NULL,
											int    inLineno   = -1    ) = 0;

		virtual bool    Input			(   char&  outChar            ) = 0;

	};

}



#endif // _su_iconsole_h_



