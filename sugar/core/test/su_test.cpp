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



#if !defined(SU_FRAMEWORK)


#include "su_test.h"
using namespace su;



namespace
{

	char			test_project_title[ 1024 ];
	int				test_warncpt		= 0;
	int				test_errcpt			= 0;
	int				test_fatalcpt		= 0;
	int				test_suitcpt		= 0;
	int				test_unitcpt		= 0;

	Pool*			test_pool			= NULL;
	IFileSystem*	test_fs				= NULL;

	tsuit_base*		tsuit_addr			= NULL;
	int				tsuit_unitcpt		= 0;
	int				tsuit_warncpt		= 0;
	int				tsuit_errcpt		= 0;
	int				tsuit_fatalcpt		= 0;

	tunit_base*		tunit_addr			= NULL;
	int				tunit_warncpt		= 0;
	int				tunit_errcpt		= 0;
	int				tunit_fatalcpt		= 0;


	void reset_tunit ( )
	{
		tunit_addr		= NULL;
		tunit_warncpt	= 0;
		tunit_errcpt	= 0;
		tunit_fatalcpt	= 0;
	}

	void reset_tsuit ( )
	{
		tsuit_addr		= NULL;
		tsuit_unitcpt	= 0;
		tsuit_warncpt	= 0;
		tsuit_errcpt	= 0;
		tsuit_fatalcpt	= 0;
		reset_tunit();
	}

	void reset_all( const char*		inProjectTitle )
	{
		test_project_title[0] = 0;
		if( inProjectTitle )
			util::Strcopy( test_project_title, inProjectTitle );

		test_warncpt	= 0;
		test_errcpt		= 0;
		test_fatalcpt	= 0;
		test_suitcpt	= 0;
		test_unitcpt	= 0;

		reset_tsuit();
	}

}



void
tsuit_base::add	(	tunit_base* c	)
{
	if( c )
	{
		c->next = NULL;
		if( first )
		{
			tunit_base* l = first;
			while( l->next )
				l = l->next;
			l->next = c;
		}
		else
		{
			first = c;
		}
	}
}



void
tunit_base::setup	( const char* _uname, const char* _file, int _lineno, tsuit_base* _owner )
{
	info.uname	= _uname;
	info.file	= _file;
	info.lineno	= _lineno;
	owner		= _owner;
	_owner->add( this );
}





bool
test::Init	(	const char*			inProjectTitle,
				su::Pool*			inTestPool,
				su::IFileSystem*	inTestFS	)
{
	test_pool = inTestPool;

	test_fs = inTestFS;
	if( test_fs )
		test_fs->Retain();

	reset_all( inProjectTitle );
	char msg[4096];
	util::Strformat( msg, "====== Project %s\n", inProjectTitle );
	Report( msg );

	return true;
}



void
test::Shut	(	)
{
	char msg[4096];
	util::Strformat( msg, "====== Project %s - %d units on %d suites, %d fatal(s), %d error(s), %d warning(s)\n",
		test_project_title,
		test_unitcpt,
		test_suitcpt,
		test_fatalcpt,
		test_errcpt,
		test_warncpt );
	Report( msg );

	SafeRelease( test_fs );
}




bool
test::Success()
{
	return (test_errcpt+test_fatalcpt) == 0;
}

bool
test::Failed()
{
	return (test_errcpt+test_fatalcpt) > 0;
}

bool
test::Fatal()
{
	return test_fatalcpt > 0;
}



bool
test::EnterSuite	(	tsuit_base*		inSuite,
						const char*		inName,
						const char*		inFilename,
						int				inFilelineno		)
{
	if( !inSuite )	return false;
	if( !inName )	return FALSE;

	reset_tsuit();

	inSuite->info.sname		= inName;
	inSuite->info.file		= inFilename;
	inSuite->info.lineno	= inFilelineno;

	tsuit_addr = inSuite;
	test_suitcpt++;

	if( test_pool )
	{
		inSuite->p = test_pool->CreateSub();
		SU_ASSERT( inSuite->p );
	}

	if( test_fs )
	{
		test_fs->Retain();
		inSuite->fs = test_fs;
	}

	return true;
}


void
test::LeaveSuite	(			)
{
	if( tsuit_addr )
	{
		su::Pool::Destroy( tsuit_addr->p );
		su::SafeRelease( tsuit_addr->fs );
	}

	reset_tsuit();
}


void
test::EnterUnit		(	tunit_base*		inUnit	)
{
	if( tsuit_addr )
	{
		reset_tunit();
		if( inUnit )
		{
			tunit_addr = inUnit;
			tsuit_unitcpt++;
			test_unitcpt++;
		}
	}
}


void
test::LeaveUnit		(									)
{
	char msg[4096];

	if( tunit_addr )
	{
		if( tunit_fatalcpt || tunit_errcpt )
		{
			util::Strformat( msg, "[%s] %s - FAILED, %d fatal(s), %d error(s), %d warning(s)\n",
				tsuit_addr->info.sname,
				tunit_addr->info.uname,
				tunit_fatalcpt,
				tunit_errcpt,
				tunit_warncpt	);
			Report( msg );
		}
		else if( tunit_warncpt )
		{
			util::Strformat( msg, "[%s] %s - ok, %d warning(s)\n",
				tsuit_addr->info.sname,
				tunit_addr->info.uname,
				tunit_warncpt	);
			Report( msg );
		}
		else
		{
			util::Strformat( msg, "[%s] %s - ok\n",
				tsuit_addr->info.sname,
				tunit_addr->info.uname	);
			Report( msg );
		}

		reset_tunit();
	}
}



void
test::Report	(	const char*		inMessage	)
{
	if( inMessage && globals::cdbg )
		globals::cdbg->Output( inMessage );
}



void
test::Report	(	ReportLevel		inLevel,
					const char*		inFile,
					const int		inLineno,
					const char*		inMessage	)
{
	static char msg[1024*8];

	if( inLevel == RL_MESSAGE )
	{
		util::Strformat( msg, "[%s] %s - message: %s\n",
			tsuit_addr ? tsuit_addr->info.sname : "?",
			tunit_addr ? tunit_addr->info.uname : "?",
			inMessage		);
		Report( msg );
	}
	else if( inLevel == RL_WARN )
	{
		util::Strformat( msg, "\n[%s] %s - warning: %s\n where: %s(%d)\n from: %s(%d)\n\n",
			tsuit_addr ? tsuit_addr->info.sname : "?",
			tunit_addr ? tunit_addr->info.uname : "?",
			inMessage,
			inFile		? inFile : "?",
			inLineno>=0 ? inLineno : -1,
			tunit_addr ? tunit_addr->info.file  : "-",
			tunit_addr ? tunit_addr->info.lineno: -1	);
		if( tsuit_addr )	tsuit_warncpt++;
		if( tunit_addr )	tunit_warncpt++;
							test_warncpt++;
		Report( msg );
	}
	else if( inLevel == RL_ERROR )
	{
		util::Strformat( msg, "\n[%s] %s - ERROR: %s\n where: %s(%d)\n from: %s(%d)\n\n",
			tsuit_addr ? tsuit_addr->info.sname : "?",
			tunit_addr ? tunit_addr->info.uname : "?",
			inMessage,
			inFile		? inFile : "?",
			inLineno>=0 ? inLineno : -1,
			tunit_addr ? tunit_addr->info.file  : "-",
			tunit_addr ? tunit_addr->info.lineno: -1	);
		if( tsuit_addr )	tsuit_errcpt++;
		if( tunit_addr )	tunit_errcpt++;
							test_errcpt++;
		Report( msg );
		SU_BREAK();
	}
	else if( inLevel == RL_FATAL )
	{
		util::Strformat( msg, "\n[%s] %s - FATAL: %s\n where: %s(%d)\n from: %s(%d)\n\n",
			tsuit_addr ? tsuit_addr->info.sname : "?",
			tunit_addr ? tunit_addr->info.uname : "?",
			inMessage,
			inFile		? inFile : "?",
			inLineno>=0 ? inLineno : -1,
			tunit_addr ? tunit_addr->info.file  : "-",
			tunit_addr ? tunit_addr->info.lineno: -1	);
		if( tsuit_addr )	tsuit_fatalcpt++;
		if( tunit_addr )	tunit_fatalcpt++;
							test_fatalcpt++;
		Report( msg );
		SU_BREAK();
	}
	else if( inLevel == RL_LOG )
	{
		util::Strformat( msg, "[%s] %s - log: %s\n",
			tsuit_addr ? tsuit_addr->info.sname : "?",
			tunit_addr ? tunit_addr->info.uname : "?",
			inMessage		);
		Report( msg );
	}
}



test::Timer::Timer ( int inmsdelay, int inmode )
{
	Start( inmsdelay, inmode );
}


void test::Timer::Start( int inmsdelay, int inmode )
{
	msdelay = inmsdelay;
	mode = inmode;
	if( mode==1 )
		core::SetWaitInterval( inmsdelay );
	clock::GetTime( &t0 );
}


float test::Timer::Elapsed ( )
{
	clock::GetTime( &t1 );
	return (t1-t0);
}


float test::Timer::Progress ( )
{
	clock::GetTime( &t1 );
	return (t1-t0)*1000.f / float(msdelay);
}

bool test::Timer::Loop ( )
{
	if( mode==0 )	core::ProcessEvent();
	else			core::WaitEvent();
	float pg = Progress();
	return pg<1.f;
}

void test::Timer::Wait ( )
{
	for( ;; )
	{
		if( !Loop() )
			break;
	}
}



#endif


