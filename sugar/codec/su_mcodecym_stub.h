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



using namespace su;




namespace
{

	struct SIDL_STUB_CLASSNAME
	{

		bool Init ( Pool* p, AnyList* o )
		{
			// Your code here.
			return true;
		}


		void Shut ( )
		{
			// Your code here.
		}


		bool IsRealized ( )
		{
			// Your code here.
			return false;
		}


		bool Realize ( MCursor& inInOut, ICodec::Mode inMode)
		{
			// Your code here.
			return false;
		}


		void Unrealize ( MCursor& inInOut)
		{
			// Your code here.
		}


		ICodec::Result Convert ( MCursor& inOutput, MCursor& inInput)
		{
			return ICodec::R_DATA_ERROR;
		}


	};


}


