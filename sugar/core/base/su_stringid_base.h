/*LIC-HDR********************************************************************
**
** Copyright (C) 2005-2010 AtOnce Technologies
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


#ifndef _su_stringid_base_h_
#define _su_stringid_base_h_




namespace su
{

	typedef short StringId;
	
	
	struct StringEnum
	{
		virtual StringId	type		(				) const = 0;
		virtual StringId	first		(				) const = 0;
		virtual StringId	last		( 				) const = 0;
		virtual StringId	next		( StringId		) const = 0;	// sid::null otherwise
		virtual StringId	find		( pcstr			) const = 0;	// sid::null otherwise
		virtual bool		has			( pcstr			) const = 0;
		virtual bool		has			( StringId		) const = 0;
		virtual StringId	value		(				) const = 0;
	};

};




namespace su { namespace sid
{

	int					Count			( 					);
	uint32				Hash			( StringId			);
	pcstr				Str				( StringId			);
	bool				Equals		    ( StringId, pcstr	);
	StringId			Find			( uint32			);
	StringId			Find			( pcstr				);

	const StringId null = 0;

} }




#endif // _su_stringid_base_h_



