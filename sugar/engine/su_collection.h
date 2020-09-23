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




#ifndef _su_collection_h_
#define _su_collection_h_


#include <sufw/su_mapping.h>


namespace su{ struct IRscManager; }
namespace su{ struct IDirectDraw; }


namespace su { namespace collection {


	struct Resource;


	struct Tag
	{
		byte*						addr;
		short						code;
		short						index;
		template <typename T> T*	load ( T& outtag ) const;
	};


	struct Define
	{
		Tag							tag;
		short						id;
	};


	struct Export
	{
		Tag							tag;
		short						objectid;
		pcstr						name;
	};


	struct List
	{
		Tag							tag;
		uint						len;
		pcstr						name;
	};

/*
	struct Animation
	{
		enum ORT
		{
			ORT_UNDEF			= (map::rsc_collection_anim::ORT_UNDEF),
			ORT_CONSTANT		= (map::rsc_collection_anim::ORT_CONSTANT),
			ORT_CYCLE			= (map::rsc_collection_anim::ORT_CYCLE),
			ORT_OSCILLATE		= (map::rsc_collection_anim::ORT_OSCILLATE),
			ORT_LINEAR			= (map::rsc_collection_anim::ORT_LINEAR),
			ORT_REL_CYCLE		= (map::rsc_collection_anim::ORT_REL_CYCLE),
		};

		enum
		{
			COMP_FLOAT			= (1<<0),
			COMP_LOC			= (1<<1),
			COMP_ROT			= (1<<2),
			COMP_SCALE			= (1<<3),
			COMP_NOTE			= (1<<4),
			COMP_SKELBONES		= (1<<5),
			COMP_MORPHWEIGHTS	= (1<<6),
		};

		Tag							tag;
		Box3						bbox;
		uint8						components;
		ORT							beforeOrt, afterOrt;
		float						beginTime, endTime;
		float						next_key		( float inT );
		float						eval_float		( float inT );
		Vec3						eval_location	( float inT );
		Quat						eval_rotation	( float inT );
		Vec3						eval_scale		( float inT );
		bool						eval_trs		( float inT, Matrix& outM );
		pcstr						eval_note		( float inT );
	};

*/

	struct Animation
	{
	};


	struct Skeleton
	{
		virtual Matrix*				bones ( ) = 0;
		virtual Matrix*				tobones ( ) = 0;

		virtual bool				bind_animation	( Animation* inAnim ) = 0;
		virtual bool				compute			( float inT ) = 0;
	};

	
	struct Mesh
	{
		virtual void				retain				( )	= 0;
		virtual void				release				( )	= 0;

		virtual Box3				bbox				( ) = 0;

		virtual uint				countsrf			( ) = 0;
		virtual bool				enumsrf				( uint* osrf, uint* ovbo, uint idx ) = 0;
		virtual uint				countsrftex			( uint srfidx ) = 0;
		virtual bool				enumsrftex			( uint* opbi, uint texIdx, uint srfidx ) = 0;
		virtual bool				hasalpha			( ) = 0;

		virtual uint*				draw_opcodes		( ) = 0;
		virtual uint*				bbox_opcodes		( ) = 0;
		virtual uint*				outlining_opcodes	( ) = 0;

		virtual bool				bind_skeleton		( Skeleton* inSkel ) = 0;
		virtual void				compute				( ) = 0;
	};



	struct Resource
	{
		virtual Pool*				pool				( ) = 0;
		virtual IRscManager*		rscmanager			( ) = 0;
		virtual uint32				rid					( ) = 0;
		virtual void				retain				( )	= 0;
		virtual void				release				( )	= 0;
		virtual uint				countref			( )	= 0;

		virtual uint				counttag			( )	= 0;
		virtual Tag					enumtag				( uint ) = 0;
		virtual Tag					firsttag			( )	= 0;
		virtual Tag					nexttag				( const Tag& ) = 0;
		virtual Tag					findtag				( int code ) = 0;

		virtual uint				countdef			( ) = 0;
		virtual Define				enumdef				( uint ) = 0;
		virtual Define				finddef				( int id ) = 0;

		virtual uint				countexport			( ) = 0;
		virtual Export				enumexport			( uint ) = 0;
		virtual Export				findexport			( pcstr inname ) = 0;

		virtual uint				countlist			( ) = 0;
		virtual List				enumlist			( uint ) = 0;
		virtual Tag					iterlist			( uint li, uint idx ) = 0;
	};

	
	Resource*						create			( IRscManager*, uint32, Pool* );
	Mesh*							create_mesh		( Resource*, const Tag&, IDirectDraw* );

} }




#include "su_collectioni.h"
#endif // _su_collection_h_







