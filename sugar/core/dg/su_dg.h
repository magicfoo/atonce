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



#ifndef _su_dg_h_
#define _su_dg_h_




namespace su { namespace dg
{

	struct Shadow;
	struct Edge;


	struct_ie Node
	{
		Shadow*			shadows;
		void*			udata;

		static Node*	create	(	void*	inUserData	);
		void			destroy	(						);
		
		Shadow*			cast	(	byte	inLayer		);		// create or get
		Shadow*			shadow	(	byte	inLayer		);		// NULL if not existing
	};


	struct_ie Shadow
	{
		Node*			node;
		byte			layer;
		Shadow*			next;
		Edge*			inputs;
		Edge*			outputs;
		
		void			destroy		(						);
		Edge*			connectto	(	Shadow*	inTarget	);
		void			disconnect	(	Shadow*	inTarget	);
	};


	struct_ie Edge
	{
		Shadow*			shadow;
		Edge*			next;
		Edge*			dual;
		byte			input:1;
		
		static Edge*	create		(	Shadow*	inStart,
										Shadow*	inEnd		);
		void			destroy		(						);

	};


	su_ie bool			IsLayerExist	(	byte			inLayer			);
	su_ie uint			CountLayer		(									);
	su_ie byte			EnumLayer		(	uint			inIdx			);		// [0,CountLayer()[

} }




#endif	// _su_dg_h_



