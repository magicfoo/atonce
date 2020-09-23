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


//
// WARNING for MIPS processors :
//
// For mips processor, stack-backtracing process uses several heuristics based
// on the "SYSTEM V ABI Mips processor" rules.
// The convenient alloca() memory allocation doesn't respect theses rules and
// you must be awared that theses heuristics *will fatally failed*
// in a function calling alloca() and undeterminated results can be expected !
//
//
// All the functions of this namespace have no implementation in MASTER mode
// (i.e. returns FALSE or NULL)
//


#ifndef _su_stack_h_
#define _su_stack_h_




namespace su { namespace stack
{

	struct Frame
	{
		pointer			ptr;		// pointer
		uint			bsize;		// size in bytes
		pointer			retAddr;	// return address
		pointer			locAddr;	// local address
	};


	// Stack backtracing
	su_ie bool			GetFrame			(	Frame&		outF			);
	su_ie bool			GetBackFrame		(	Frame&		outBackF,
												Frame&		inF				);

	// Local informations
	su_ie pointer		GetReturnAddr		(								);
	su_ie pointer		GetLocalAddr		(								);

} }




#endif // _su_stack_h_



