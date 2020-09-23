/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2009 AtOnce Technologies
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



#ifndef _su_iswitch_h_
#define _su_iswitch_h_




namespace su
{

	struct_ie Any;


	struct ISwitch : public IInterface
	{
		SU_DECL_INTERFACE( void, 0x622CC2E9 )

		struct Desc
		{
			pcstr		name;		// name
			pcstr		func;		// Cpp function name
			pcstr		sign;		// Cpp function signature
			pcstr		types;		// Cpp function types
			pcstr		concern;	// interface name
			pcstr		mark;		// mark
		};


		virtual uint			CountSwitch		(													) = 0;

		virtual bool			EnumSwitch		(	uint		inIdx,
													Desc&		outDesc								) = 0;

		virtual int				FindSwitch		(	pcstr		inNname								) = 0;

		virtual bool			CallSwitch		(	uint		inIdx,		void*,		...			) = 0;
		virtual bool			CallSwitchMem	(	uint		inIdx,		Any*,		int, Any*	) = 0;
	};

}




#endif // _su_iswitch_h_



