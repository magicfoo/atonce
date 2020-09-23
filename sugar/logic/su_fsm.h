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



#ifndef _su_fsm_h_
#define _su_fsm_h_



namespace su { namespace fsm {


	struct State;
	struct Automaton;


	enum Phase
	{
		PH_NONE = 0,
		PH_ENTER,
		PH_ENTERING,
		PH_PROCESSING,
		PH_LEAVE,
		PH_LEAVING,
		PH_EXIT,
		PH_EXITING,
	};


	struct State
	{
		int					stateid;
		Automaton*			automaton;
		State*				super;

		virtual bool		init			( Pool* inp, int inid, Automaton* ina, State* insu );
		virtual void		shut			( );

		virtual void		open			( );
		virtual void		close			( );

		virtual void		enter			( );
		virtual bool		entering		( );	// repeated while returns false

		virtual void		leave			( );
		virtual bool		leaving			( );	// repeated while returns false

		virtual void		exit			( );
		virtual bool		exiting			( );	// repeated while returns false

		virtual void		process			( );
		virtual bool		manage			( );	// returns false if fails !

		virtual void		gotonext		( );
		virtual void		gotostate		( int );
		virtual pcstr		getname			( );
		virtual int			getnext			( );
		virtual Phase		getphase		( );
	};


	struct StateDesc
	{
		State*				obj;
		pcstr				name;
		int					superid;
		int					nextid;
		bool				startpoint;
	};


	struct Automaton
	{
		Pool*				pool;
		Phase				curphase;
		State*				curstate;		// current
		State*				trgstate;		// target
		bool				running;
		bool				tracing;
		pcstr				traceid;
		uint32				signals;

		virtual bool		init			( Pool* inp				);
		virtual void		shut			(						);

		virtual bool		alloc			( Pool* inp				);
		virtual void		dealloc			( Pool* inp				);

		virtual int			countstate		(						) = 0;
		virtual StateDesc	enumstate		( int stateid			) = 0;

		virtual int			state			(						);
		virtual Phase		phase			(						);

		virtual int			next			( int stateid			);
		virtual pcstr		name			( int stateid			);
		virtual int			super			( int stateid			);

		virtual bool		start			(						);
		virtual bool		start			( int stateid			);
		virtual void		stop			(						);
		virtual bool		isrunning		(						);
		virtual void		step			(						);
		virtual void		gotostate		( int stateid			);
		virtual void		raise			( uint s				);		// an integer in [0,31]
		virtual bool		raised			( uint s				);		// an integer in [0,31]
		virtual void		clear			( uint s				);		// an integer in [0,31]

		virtual void		trace			( bool onoff, pcstr label = NULL );
	};


} }



#endif // _su_fsm_h_


