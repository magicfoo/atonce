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



#ifndef _su_addin_h_
#define _su_addin_h_




namespace su { namespace addin
{


	struct_ie Starter
	{
		enum Stage
		{
			STAGE_INITIALIZE	= (1),
			STAGE_CREATE		= (2),
			STAGE_START			= (3),
		//	...
			STAGE_RUN			= (100),
		//  ...
			STAGE_TERMINATE		= (-3),
			STAGE_RELEASE		= (-2),
			STAGE_SHUT			= (-1),
		};

		Starter*				next;

								Starter				(									);
		virtual	void			OnStage				(	Stage stage						) = 0;
		static Starter*			First				(									);
		static Starter*			Next				(	Starter*						);
		static void				Setup				(	Stage stage						);
	};



	struct_ie Builder
	{
		Builder*				next[4];

								Builder				(									);
		virtual classid			GetMUID				(									) = 0;
		virtual pcstr			GetMClassname		(									) = 0;

		virtual uint32			GetRevision			(									) { return 0; }
		virtual pcstr			GetPackage			(									) { return NULL; }

		virtual uint			CountInterface		(									) { return 0; }
		virtual bool			EnumInterface		(	uint			inIdx,
														classid*		outCid,
														pcstr*			outCname		) { return false; }

		virtual uint			CountSwitch			(									) { return 0; }
		virtual bool			EnumSwitch			(	uint			inIdx,
														ISwitch::Desc&	outDesc			) { return false; }

		virtual	IInterface*		CreateObject		(	Pool*			inNestPool,
														Pool*			inDataPool,
														AnyList*		inOptions		) { return NULL; }

		virtual IInterface*		FirstInterface		(	void*			inBase			) { return NULL; }
		virtual IInterface*		GetInterface		(	void*			inBase,
														classid			inCid			) { return NULL; }
		template <typename I>I*	GetInterface		(	void*			inBase			);

		virtual void*			BaseToStub			(	void*			inBase			) { return NULL; }
		virtual void*			StubToBase			(	void*			inStub			) { return NULL; }

		static Builder*			Find				(	classid			inMUID			);
		static Builder*			First				(									);
		static Builder*			Next				(	Builder*		inFrom			);
		static Builder*			FirstConcern		(	classid			inCUID			);
		static Builder*			NextConcern			(	classid			inCUID,
														Builder*		inFrom			);
	};


} }




#include "su_addini.h"
#endif // _su_addin_h_



