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



#ifndef _su_iinterface_h_
#define _su_iinterface_h_


su_ns_type( addin, Builder )



namespace su
{

	struct IInterface
	{
		SU_DECL_INTERFACE( void, 0x171AA280 )

		virtual classbase					GetBase				(									) = 0;
		virtual addin::Builder*				GetBuilder			(									) = 0;

		virtual	void						Retain				(									) = 0;
		virtual	uint						CountRef			(									) = 0;
		virtual	void						Release				(									) = 0;

		virtual uint						CountInterface		(									) = 0;

		virtual bool						EnumInterface		(	uint			inIdx,
																	classid*		outCid,
																	pcstr*			outCname		) = 0;

		virtual IInterface*					GetInterface		(	classid			inCid			) = 0;

		template <typename T> T*			GetInterface		(									);
		bool								HasInterface		(	classid			inCid			);
		template <typename T> bool			HasInterface		(									);
	};


	template <typename T> T*				SafeInterface		(	IInterface* inITF				);
	template <typename T> void				SafeRelease			(	T*&	ioITF						);
	template <typename T> bool				SafeRetain			(	T*&	ioITF, T* inRefITF			);		// returns TRUE if ioITF has changed !
	template <typename T> bool				SafeNotNullRetain	(	T*& ioITF, T* inRefITF			);		// returns TRUE if ioITF has changed !
	template <typename T> void				SafeDelete			(	T*& ioObject					);

	template <typename T> IInterface*		BaseInterface		(	T* inITF, classid inClassid		);
	IInterface*								BaseInterface		(	void*,    classid inClassid		);

}




#include "su_iinterfacei.h"
#endif // _su_iinterface_h_



