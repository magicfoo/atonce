/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2010 AtOnce Technologies
**
** This file is part of the B3D project.
**
** This file and the associated product may be used and distributed
** under the terms of a current License as defined by AtOnce Technologies
** and appearing in the file LICENSE.TXT included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.atonce-technologies.com or email info@atonce-technologies.com
** for informations about the B3D project and materials and AtOnce Technologies others products
** License Agreements.
**
** Contact info@atonce-technologies.com if any conditions of this license
** are not clear to you.
**
*****************************************************************LIC-HDR*/



#include <sufw/su_core.h>
#include "su_logic.h"

using namespace su;
using namespace su::fsm;


#define NULL_STATE		((State*)NULL)
#define STOP_STATE		((State*)1)



namespace
{

	bool is_super_of( State* ins, State* insuper )
	{
		return ins && ins->super==insuper;
	}

	bool is_child_of( State* ins, State* inparent )
	{
		if( is_super_of(ins,inparent) )
			return true;
		SU_REQUIRE_OR_FALSE( ins );
		SU_REQUIRE_OR_FALSE( inparent );
		return is_child_of( ins->super, inparent );
	}

	State* get_child_of( State* inleaf, State* inparent )
	{
		while( inleaf && inleaf->super!=inparent )
			inleaf = inleaf->super;
		return inleaf;
	}

	int get_depth( State* ins )
	{
		int d = 0;
		while( ins && ins->super ) {
			ins = ins->super;
			d++;
		}
		return d;
	}

	State* common_parent( State* ina, State* inb )
	{
		int da = get_depth(ina);
		int db = get_depth(inb);
		if( da > db )	util::Swap(ina,inb);
		while( ina )
		{
			if( is_child_of(inb,ina) )
				return ina;
			ina = ina->super;
		}
		return ina;
	}


	void trace_output ( pcstr m, Automaton*a, State* s )
	{
		if( a->tracing )
		{
			util::SafeBuffer<char,64> buff;

			char xid[16];
			util::Strformat( xid, "0x%lx", ulong(a) );

			if( s )
			{
				StateDesc d = a->enumstate(s->stateid);
				util::Strformat( buff, "fsm(%s): %s(%s,#%d)\n", a->traceid?a->traceid:xid, m, d.name, s->stateid );
			}
			else
			{
				util::Strformat( buff, "fsm(%s): %s\n", a->traceid?a->traceid:xid, m );
			}

			SU_STDOUT( buff );
		}
	}


}





bool State::init ( Pool* inp, int inid, Automaton* ina, State* insu )
{
	SU_ASSERT( ina );
	SU_ASSERT( inid >= 0 );
	stateid = inid;
	automaton = ina;
	super = insu;
	return true;
}


void State::shut ( )
{
	//
}


void State::open ( )
{
	//
}


void State::close ( )
{
	//
}


void State::enter ( )
{
	//
}


bool State::entering ( )
{
	return true;
}


void State::leave ( )
{
	//
}


bool State::leaving ( )
{
	return true;
}


void State::exit ( )
{
	leave();
}


bool State::exiting ( )
{
	return leaving();
}


void State::process ( )
{
	gotonext();
}


bool State::manage ( )
{
	return super? super->manage() : true;
}


void State::gotonext ( )
{
	SU_ASSERT( automaton );
	int n = automaton->next(stateid);
	if( n<0 )
		automaton->stop();
	else
		automaton->gotostate(n);
}


void State::gotostate ( int s )
{
	automaton->gotostate(s);
}


pcstr State::getname ( )
{
	SU_ASSERT( automaton );
	return automaton->name( stateid );
}


int State::getnext ( )
{
	SU_ASSERT( automaton );
	return automaton->next( stateid );
}


Phase State::getphase ( )
{
	SU_ASSERT( automaton );
	if( automaton->curstate == this )
		return automaton->curphase;
	else
		return PH_NONE;
}





bool Automaton::init ( Pool* inp )
{
	pool = inp ? inp->CreateSub() : NULL;

	curphase = PH_NONE;
	curstate = NULL;
	trgstate = NULL;
	running = false;
	tracing = false;
	traceid = NULL;
	signals = 0;

	if( !alloc(pool) )
	{
		Pool::Destroy( pool );
		return false;
	}

	int n = countstate();
	SU_ASSERT( n );

	// init states

	int i;
	for( i=0 ; i<n ; i++ )
	{
		StateDesc di = enumstate( i );

		SU_ASSERT( di.obj );
		SU_ASSERT( di.name );
		SU_ASSERT( di.superid <  n );
		SU_ASSERT( di.nextid  <  n );

		State* su = NULL;
		if( di.superid >= 0 )
		{
			SU_ASSERT( di.superid < i );
			StateDesc du = enumstate( di.superid );
			su = du.obj;
		}

		if( !di.obj->init(pool,i,this,su) )
			break;
	}

	// failed => shut states

	if( i<n )
	{
		for( int j=0 ; j<i ; j++ )
		{
			StateDesc dj = enumstate( j );
			dj.obj->shut();
		}
		Pool::Destroy( pool );
		return false;
	}

	return true;
}


void Automaton::shut ( )
{
	stop();

	int n = countstate();
	SU_ASSERT( n );

	for( int i=0 ; i<n ; i++ )
	{
		StateDesc di = enumstate(i);
		di.obj->shut();
	}

	dealloc(pool);

	Pool::Destroy( pool );
}


bool Automaton::alloc ( Pool* )
{
	return true;
}


void Automaton::dealloc ( Pool* )
{
	//
}


int Automaton::state ( )
{
	if( running && curstate )
		return curstate->stateid;
	else
		return -1;
}


Phase Automaton::phase ( )
{
	if( running && curstate )
		return curphase;
	else
		return PH_NONE;
}


int Automaton::next ( int stateid )
{
	SU_ASSERT( stateid >= 0 );
	SU_ASSERT( stateid < countstate() );

	StateDesc d = enumstate( stateid );
	return d.nextid;
}


pcstr Automaton::name ( int stateid )
{
	SU_ASSERT( stateid >= 0 );
	SU_ASSERT( stateid < countstate() );

	StateDesc d = enumstate( stateid );
	return d.name;
}


int Automaton::super ( int stateid )
{
	SU_ASSERT( stateid >= 0 );
	SU_ASSERT( stateid < countstate() );

	StateDesc d = enumstate( stateid );
	return d.superid;
}


bool Automaton::start ( )
{
	for( uint i=0 ; i<countstate() ; i++ ) {
		StateDesc s = enumstate(i);
		if( s.startpoint )
			return start(i);
	}
	return start(0);
}


bool Automaton::start ( int stateid )
{
	SU_ASSERT( stateid >= 0 );
	SU_ASSERT( stateid < countstate() );
	SU_REQUIRE_OR_FALSE( !running );

	StateDesc s = enumstate(stateid);

	trgstate = s.obj;
	curphase = PH_NONE;
	curstate = NULL;
	running = true;
	signals = 0;

	trace_output("start",this,NULL);

	step();
	return true;
}


void Automaton::stop ( )
{
	if( running )
	{
		trace_output("stop",this,NULL);
		trgstate = STOP_STATE;
	}
}


bool Automaton::isrunning ( )
{
	return running;
}


void Automaton::trace ( bool onoff, pcstr label )
{
	tracing = onoff;
	traceid = label;
}


void Automaton::gotostate ( int stateid )
{
	SU_ASSERT( stateid >= 0 );
	SU_ASSERT( stateid < countstate() );

	if( running )
	{
		StateDesc s = enumstate(stateid);
		if( s.obj != curstate )
			trgstate = s.obj;
	}
}


void Automaton::raise ( uint s )
{
	SU_ASSERT( s < 32 );
	signals |= 1UL<<s;
}


bool Automaton::raised ( uint s )
{
	SU_ASSERT( s < 32 );
	uint m = 1UL<<s;
	return (signals & m) != 0;
}


void Automaton::clear ( uint s )
{
	SU_ASSERT( s < 32 );
	uint m = 1UL<<s;
	signals &= ~m;
}


void Automaton::step ( )
{
	SU_REQUIRE_OR_RETURN( running );

	for( uint loop=0 ;; loop++ )
	{
		switch( curphase )
		{
			case PH_NONE :
			{
				if( trgstate == STOP_STATE )
					trgstate = NULL;

				if( !curstate && !trgstate )
				{
					running = false;
					return;
				}

				State *s, *t, *co;

				s  = curstate;
				t  = trgstate;
				co = common_parent(s,t);

				// step-up to close all super nodes ?!
				while( s != co )
				{
					trace_output("close",this,s);
					s->close();
					s = s->super;
				}

				// step-down to open all super nodes ?!
				while( s != t )
				{
					s = get_child_of(t,s);
					trace_output("open",this,s);
					s->open();
				}

				trgstate = NULL;
				curstate = s;
				if( s ) curphase = PH_ENTER;
				break;
			}

			case PH_ENTER :
			{
				SU_ASSERT( curstate );
				trace_output("enter",this,curstate);
				curstate->enter();
				curphase = PH_ENTERING;
				break;
			}

			case PH_ENTERING :
			{
				SU_ASSERT( curstate );
				trace_output("entering",this,curstate);
				if( !curstate->entering() )
					return;
				curphase = PH_PROCESSING;
				break;
			}

			case PH_PROCESSING :
			{
				SU_ASSERT( curstate );

				// leave ?
				if( trgstate )
				{
					curphase = PH_LEAVE;
					break;
				}

				// manage super ?
				if( curstate->super && !curstate->super->manage() )
				{
					trace_output("manage has failed!",this,curstate);
					curphase = PH_EXIT;
					break;
				}

				trace_output("processing",this,curstate);
				curstate->process();
				return;
			}

			case PH_LEAVE :
			{
				SU_ASSERT( curstate );
				trace_output("leave",this,curstate);
				curstate->leave();
				curphase = PH_LEAVING;
				break;
			}

			case PH_LEAVING :
			{
				SU_ASSERT( curstate );
				trace_output("leaving",this,curstate);
				if( !curstate->leaving() )
					return;
				curphase = PH_NONE;
				break;
			}

			case PH_EXIT :
			{
				SU_ASSERT( curstate );
				trace_output("exit",this,curstate);
				curstate->exit();
				curphase = PH_EXITING;
				break;
			}

			case PH_EXITING :
			{
				SU_ASSERT( curstate );
				trace_output("exiting",this,curstate);
				if( !curstate->exiting() )
					return;
				curphase = PH_NONE;
				break;
			}
		}
	}
}




