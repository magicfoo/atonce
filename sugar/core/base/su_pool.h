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



#ifndef _su_pool_h_
#define _su_pool_h_



namespace su
{

	struct_ie Pool
	{

		enum RecyclingMode
		{
			RM_Immediate = 0,			// direct recycling to the allocator behind
			RM_Retained,				// keep allocated blocks until a manual recycling operation
			RM_Auto,					// partial recycling based on some heuristics.
			RM_Default = RM_Auto,
		};

		virtual IAllocator*				GetAllocator	(											) = 0;
		virtual Pool*					GetParent		(											) = 0;
		virtual bool					IsAncestorOf	(	Pool*			inChild					) = 0;

		virtual void*					Alloc			(	uint			inBSize					) = 0;
		virtual void*					AllocZero		(	uint			inBSize					) = 0;		// alloc and zero'ing
		virtual void*					AllocMore		(	void*			inOldAddr,
															uint			inOldBSize,
															uint			inNewBSize				) = 0;		// alloc and memcopy (new-bsize must be > old-bsize)
		virtual void*					AllocLess		(	void*			inOldAddr,
															uint			inOldBSize,
															uint			inNewBSize				) = 0;		// alloc and memcopy (new-bsize must be < old-bsize)
		virtual void					Free			(	void*			inAddr					) = 0;

		virtual pointer					DumpMemory		(	void*			inAddr,
															uint			inBSize,
															uint			inBSupply = 0			) = 0;		// Alloc -> Memcopy

		template <typename T> T*		Array			(	uint			inLen					);
		template <typename T> T*		New				(	uint			inBSupply = 0			);
		template <typename T> void		Delete			(	T*				inObject				);

		virtual void					Recycle			(											) = 0;		// Retained mode only 
		virtual void					RecycleDeep		(											) = 0;

		virtual void					SetTag			(	pcstr			inTag					) = 0;

		virtual	Pool*					CreateSub		(											) = 0;
		virtual	Pool*					CreateSub		(	RecyclingMode	inRMode					) = 0;

		virtual void					Clear			(											) = 0;
		virtual void					Release			(											) = 0;


		static	Pool*					Create			(	IAllocator*		inATOR	= NULL,
															RecyclingMode	inRMode	= RM_Retained	);

		static	void					Destroy			(	Pool*&			ioPool					);
		static	Pool*					Root			(											);
		static	void					RecycleAll		(											);

	};

}



#include "su_pooli.h"
#endif // _su_pool_h_



