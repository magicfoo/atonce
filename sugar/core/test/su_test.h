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


#ifndef _su_test_h_
#define _su_test_h_

#include <sufw/su_core.h>


namespace su
{
	struct Pool;
	struct IFileSystem;
};


struct tunit_info;
struct tsuit_info;
struct tunit_base;
struct tsuit_base;



namespace test
{

	enum ReportLevel
	{
		RL_MESSAGE = 0,			// simple output to log
		RL_WARN,				// warning level
		RL_ERROR,				// error level, the unit test continues
		RL_FATAL,				// fatal error, the unit & suite test fails !
		RL_LOG					// outputed only before error or fatal outputs
	};


	struct Timer
	{
		int			mode;
		int			msdelay;
		su::Time	t0, t1;

				Timer			( int inmsdelay, int inmode=0 );
		void	Start			( int inmsdelay, int inmode=0 );
		float	Elapsed			( );
		float	Progress		( );
		bool	Loop			( );
		void	Wait			( );
	};


	bool		Init			(	const char*			inProjectTitle,
									su::Pool*			inPool = NULL,
									su::IFileSystem*	inFS   = NULL	);
	void		Shut			(										);

	bool		Success			(										);
	bool		Failed			(										);
	bool		Fatal			(										);

	void		Report			(	ReportLevel			inLevel,
									const char*			inFilename,
									const int			inFilelineno,
									const char*			inMessage		);

	void		Report			(	const char*			inMessage		);

	bool		EnterSuite		(	tsuit_base*			inSuite,
									const char*			inName,
									const char*			inFilename,
									const int			inFilelineno	);
	void		LeaveSuite		(										);

	void		EnterUnit		(	tunit_base*			inUnit			);
	void		LeaveUnit		(										);

}



//
// Test suite

struct tunit_info
{
	const char*			uname;		// unit name
	const char*			file;		// file name
	int					lineno;		// file line
};

struct tsuit_info
{
	const char*			sname;		// suite name
	const char*			file;		// file name
	int					lineno;		// file line
};

struct tunit_base
{
	tsuit_base*			owner;
	tunit_info			info;
	tunit_base*			next;
						tunit_base() : next(NULL), owner(NULL) {}
	virtual void		setup	( const char*, const char*, int, tsuit_base* );
	virtual void		run		( ) {}
};

struct tsuit_base
{
	su::Pool*			p;			// tsuit pool
	su::IFileSystem*	fs;			// tsuit filesystem
	tsuit_info			info;
	tunit_base*			first;
						tsuit_base() : first(NULL) {}
	virtual void		add		( tunit_base*	);
	virtual bool		onrun	()	{ return true; }
	virtual void		oninit	()	{              }
	virtual void		onshut	()	{              }
};

template < typename T >
inline
void tsuit_runner( const char* sname, const char* file, int   lineno	)
{
	T s;
	bool enterit = ::test::EnterSuite( &s, sname, file, lineno );
	if( enterit )
	{
		bool doit = s.onrun();
		if( doit )
		{
			s.oninit();
			tunit_base* u = s.first;
			while( u && ::test::Success() )
			{
				::test::EnterUnit( u );
				u->run();
				u = u->next;
				::test::LeaveUnit();
			}
			s.onshut();
		}
		else
		{
			char msg[ 2048 ];
			su::util::Strformat( msg, "[%s] skipped ...\n", s.info.sname );
			::test::Report( msg );
		}

		::test::LeaveSuite();
	}
}



#define	TestSuite( name ) \
	namespace test { namespace suite { namespace name { \
		const char*	file     = __FILE__; \
		const int	lineno   = __LINE__; \
	}}} \
	struct tsuit_##name; \
	typedef tsuit_##name tsuit; \
	struct tsuit_##name : public tsuit_base

#define	TestRun() \
	bool onrun( )

#define	TestInit() \
	void oninit( )

#define	TestShut() \
	void onshut( )

#define TestUnit( name ) \
	struct t_tunit_##name : public tunit_base { \
		t_tunit_##name()	{ setup(#name,__FILE__,__LINE__,SU_DELEGATE_BASEPTR(tsuit,tunit_##name,this)); } \
		virtual void run()	{ ((tsuit*)owner)->tunit_code_##name(); } \
	} tunit_##name; \
	void tunit_code_##name ()


#define	ExportSuite( name ) \
	namespace test { namespace suite { namespace name { \
		void run() { \
			return tsuit_runner<tsuit>( #name, file, (int)lineno ); \
		} \
	}}}

#define	ImportSuite( name ) \
	namespace test { namespace suite { namespace name { \
		extern const char*	file; \
		extern const int	lineno; \
		void				run(); \
	}}}

#define	RunSuite( name ) \
	if( ::test::Success() ) { \
		::test::suite::name::run(); \
	}




#define	TEST_MESSAGE( inMSG ) \
	{ \
		::test::Report( ::test::RL_MESSAGE, __FILE__, __LINE__, inMSG ); \
	}

#define	TEST_WARNING( inMSG ) \
	{ \
		::test::Report( ::test::RL_WARN, __FILE__, __LINE__, inMSG ); \
	}

#define	TEST_ERROR( inMSG ) \
	{ \
		::test::Report( ::test::RL_ERROR, __FILE__, __LINE__, inMSG ); \
	}

#define	TEST_FATAL( inMSG ) \
	{ \
		::test::Report( ::test::RL_FATAL, __FILE__, __LINE__, inMSG ); \
	}

#define	TEST_LOG( inMSG ) \
	{ \
		::test::Report( ::test::RL_LOG, __FILE__, __LINE__, inMSG ); \
	}

#define	TEST_ASSERT( inC ) \
	if( !(inC) ) { \
		::test::Report( ::test::RL_ERROR, __FILE__, __LINE__, "The condition ("#inC") is not verified !" ); \
	}

#define	TEST_REQUIRE( inC ) \
	if( !(inC) ) { \
		::test::Report( ::test::RL_FATAL, __FILE__, __LINE__, "The condition ("#inC") must be verified !" ); \
	}

#define	TEST_TRUE( inC ) \
	if( !(inC) ) { \
		::test::Report( ::test::RL_ERROR, __FILE__, __LINE__, "The condition ("#inC") must be true !" ); \
	}

#define	TEST_FALSE( inC ) \
	if( (inC) ) { \
		::test::Report( ::test::RL_ERROR, __FILE__, __LINE__, "The condition ("#inC") must be false !" ); \
	}

#define	TEST_NULL( inC ) \
	if( (inC)!=NULL ) { \
		::test::Report( ::test::RL_ERROR, __FILE__, __LINE__, "The value ("#inC") must be NULL !" ); \
	}

#define	TEST_NOTNULL( inC ) \
	if( (inC)==NULL ) { \
		::test::Report( ::test::RL_ERROR, __FILE__, __LINE__, "The value ("#inC") must not be NULL !" ); \
	}

#define	TEST_EQUAL( inA, inB ) \
	if( !((inA)==(inB)) ) { \
		::test::Report( ::test::RL_ERROR, __FILE__, __LINE__, "("#inA") == ("#inB") failed !" ); \
	}

#define	TEST_FEQUAL( inA, inB , inEpsilon) \
	if(  (Fabs((inA)-(inB)) >  inEpsilon) ) { \
		::test::Report( ::test::RL_ERROR, __FILE__, __LINE__, "("#inA") == ("#inB") failed !" ); \
	}

#define	TEST_NEQUAL( inA, inB ) \
	if( (inA)==(inB) ) { \
		::test::Report( ::test::RL_ERROR, __FILE__, __LINE__, "("#inA") != ("#inB") failed !" ); \
	}

#define	TEST_STREQUAL( inA, inB ) \
	if( !util::Strequals(inA,inB) ) { \
		::test::Report( ::test::RL_ERROR, __FILE__, __LINE__, "("#inA") == ("#inB") failed !" ); \
	}

#define	TEST_FPEQUAL( inX, inY ) \
	if( !su::Near(inX,inY) ) { \
		::test::Report( ::test::RL_ERROR, __FILE__, __LINE__, "("#inX") == ("#inX") failed !" ); \
	}

#define IF_SUCCESS		if( ::test::Success() )
#define IF_FAILED		if( ::test::Failed() )
#define IF_FATAL		if( ::test::Fatal() )
#define IF_NOT_FATAL	if( !::test::Fatal() )
#define	TEST_ROF		if( ::test::Failed() ) { return; }		// Return On Failed




#endif // _su_test_h_



