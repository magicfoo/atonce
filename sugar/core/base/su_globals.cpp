/*LIC-HDR*****************************************************************
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


#include <sufw/core/su_core.h>
#include <sufw/su_uids.h>
#include <sufw/su_settings.h>
#include <sufw/su_system.h>
using namespace su;
using namespace su::globals;



#if defined(SU_HW_WINDOWS) || defined(SU_HW_DARWIN)
	#define DEF_AUTHMODE		1
	#define DEF_BINARIZEMAXBS	(256<<20)
#else
	#define DEF_AUTHMODE		0
	#define DEF_BINARIZEMAXBS	(0)
#endif


#define	DEF_RSCMANBUFFERBS	(64*1024)





byte					su::globals::corestate				= 0;
byte*					su::globals::profilelo				= NULL;
byte*					su::globals::profilehi				= NULL;
Pool*					su::globals::syspool				= NULL;
Pool*					su::globals::obspool				= NULL;
uint					su::globals::obspoolbs				= 1024*32;
uint					su::globals::authmode				= DEF_AUTHMODE;
Pool*					su::globals::authpool				= NULL;
pcstr					su::globals::sessionname			= "sugar";
uint32					su::globals::consoleclassid			= 0;
pstr					su::globals::consolenetaddr			= NULL;
pstr					su::globals::consolefilename		= NULL;
bool					su::globals::consolefileappend		= false;
IConsole*				su::globals::cout					= NULL;
IConsole*				su::globals::cerr					= NULL;
IConsole*				su::globals::cdbg					= NULL;
IFileSystem*			su::globals::fs						= NULL;
uint32					su::globals::revision_number		= SU_BUILD_REVISION;
pcstr					su::globals::buildlevel				= SU_BUILD_LEVEL_S;
uint					su::globals::maxsock				= 8;
uint					su::globals::maxhfs					= 8;
uint					su::globals::camgrabber				= uid::MCameraGrabber;
uint					su::globals::binarizemaxbs			= DEF_BINARIZEMAXBS;
uint					su::globals::rscmanbufferbs			= DEF_RSCMANBUFFERBS;
uint					su::globals::camgrabquality			= 0;



// Microsoft Windows
#if defined(SU_HW_WINDOWS)
pcstr					su::globals::win_modulefilename		= NULL;
pointer					su::globals::win_hinstance			= NULL;
pcstr					su::globals::win_cmdline			= NULL;
int						su::globals::win_showcmd			= NULL;
pointer					su::globals::win_basesp				= NULL;
#endif


// Sony PSP2
#if defined(SU_HW_PSP2)
int						su::globals::psp2_argc				= NULL;
char**					su::globals::psp2_argv				= NULL;
uint32					su::globals::psp2_basesp			= NULL;
uint					su::globals::psp2_parametersbs		= SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;				// 16 Mo
uint					su::globals::psp2_vdmringbs			= SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE;					// 128 Ko
uint					su::globals::psp2_vertexringbs		= SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE;				// 2 Mo
uint					su::globals::psp2_fragmentringbs	= SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE;			// 512 Ko
uint					su::globals::psp2_fragmentusseringbs= SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE;		// 16 Ko
#endif


// Nintendo RVL2
#if defined(SU_HW_RVL2)
int						su::globals::rvl2_argc				= NULL;
char**					su::globals::rvl2_argv				= NULL;
uint32					su::globals::rvl2_basesp			= NULL;
#endif


// Apple Darwin
#if defined(SU_HW_DARWIN)
int						su::globals::drw_argc				= NULL;
char**					su::globals::drw_argv				= NULL;
void*					su::globals::drw_mtx0				= NULL;
void*					su::globals::drw_mtx1				= NULL;
void*					su::globals::drw_cfrunloop			= NULL;
uint					su::globals::drw_dsbuffbsize		= 1024*64;
#endif


// Apple iOS
#if defined(SU_HW_IOS)
int						su::globals::ios_argc				= NULL;
char**					su::globals::ios_argv				= NULL;
void*					su::globals::ios_appdeleg			= NULL;
volatile ios_appdeleg_func_t su::globals::ios_appdeleg_func	= NULL;
volatile pointer		su::globals::ios_appdeleg_param		= NULL;

void*					su::globals::ios_mtx0				= NULL;
void*					su::globals::ios_mtx1				= NULL;
volatile void*			su::globals::ios_ti					= NULL;
volatile float			su::globals::ios_tiper				= 0.0f;
volatile float			su::globals::ios_tinewper			= 0.0f;
volatile uint			su::globals::ios_ticount			= 0;
volatile uint			su::globals::ios_cbflags			= 0;
void*					su::globals::ios_thid				= NULL;
char*					su::globals::ios_bundlepath			= NULL;
void*					su::globals::ios_cfrunloop			= NULL;
uint					su::globals::ios_dsbuffbsize		= 1024*64;
bool					su::globals::ios_sbarvisible		= true;
int						su::globals::ios_sbarorientation	= 0;
int						su::globals::ios_devorientation		= 0;
pointer					su::globals::ios_mainwindow			= NULL;
bool					su::globals::ios_devisipad			= false;
bool					su::globals::ios_devisipad1			= false;
bool					su::globals::ios_devisipad2			= false;
bool					su::globals::ios_devisiph4			= false;
bool					su::globals::ios_devisiph3			= false;
bool					su::globals::ios_havecamera			= false;
bool					su::globals::ios_haveretina			= false;
float					su::globals::ios_version			= 0.0f;
bool					su::globals::ios_enablehd			= false;
uint					su::globals::ios_screen_w			= 0;
uint					su::globals::ios_screen_h			= 0;
#endif

// Google Android
#if defined( SU_HW_ANDROID )
void*					su::globals::and_jnienv				= NULL;
void*					su::globals::and_activity			= NULL;
void*					su::globals::and_app				= NULL;
void*					su::globals::and_holdercb			= NULL;
void*					su::globals::and_jfs				= NULL;
void*					su::globals::and_swapf				= NULL;
const char*				su::globals::and_apkpath			= NULL;
const char*				su::globals::and_tmppath			= NULL;
const char*				su::globals::and_appdatapath		= NULL;
void*					su::globals::and_mtx0				= NULL;
void*					su::globals::and_mtx1				= NULL;
bool					su::globals::and_bexit				= false;
#endif

void*					su::globals::dd_win_embedded_hwnd	= NULL;
void*					su::globals::dd_win_focus_hwnd		= NULL;
bool					su::globals::dd_fullscreen			= false;
uint					su::globals::dd_fullscreen_width	= 0;
uint					su::globals::dd_fullscreen_height	= 0;
bool					su::globals::dd_soft_vtx_processing	= false;
uint					su::globals::dd_ddraw				= uid::MDirectDraw;
uint					su::globals::dd_dsound				= uid::MDirectSound;
uint					su::globals::dd_dx9_adapter			= 0;
uint*					su::globals::dd_dx9_shade_tree		= NULL;
uint*					su::globals::dd_gl_shade_tree		= NULL;


bool					su::globals::dapp_fullscreen		= false;
uint					su::globals::dapp_width				= 0;
uint					su::globals::dapp_height			= 0;
uint					su::globals::dapp_sound_channel_count = 1;



// accessors

#define su_global_acc( Type, ID ) \
	static locals::Type##Accessor ID##_acc ( "g_"#ID, &su::globals::ID );

su_global_acc( UInt, obspoolbs )
su_global_acc( UInt, authmode )
su_global_acc( UInt, consoleclassid )
su_global_acc( String, consolefilename )
su_global_acc( Bool, consolefileappend )
su_global_acc( UInt, maxsock )
su_global_acc( UInt, maxhfs )
su_global_acc( UInt, binarizemaxbs )
su_global_acc( UInt, rscmanbufferbs )



#if defined(SU_HW_PSP2)
su_global_acc( UInt, psp2_parametersbs )
su_global_acc( UInt, psp2_vdmringbs )
su_global_acc( UInt, psp2_vertexringbs )
su_global_acc( UInt, psp2_fragmentringbs )
su_global_acc( UInt, psp2_fragmentusseringbs )
#endif

#if defined(SU_HW_IOS)
su_global_acc( UInt, ios_dsbuffbsize )
su_global_acc( Bool, ios_sbarvisible )
su_global_acc( Int,	 ios_sbarorientation )
su_global_acc( Bool, ios_enablehd )
#endif

#if defined(SU_HW_DARWIN)
su_global_acc( UInt, drw_dsbuffbsize )
#endif

su_global_acc( Bool, dd_fullscreen )
su_global_acc( UInt, dd_fullscreen_width )
su_global_acc( UInt, dd_fullscreen_height )
su_global_acc( Bool, dd_soft_vtx_processing )
su_global_acc( UInt, dd_ddraw )
su_global_acc( UInt, dd_dsound )
su_global_acc( UInt, dd_dx9_adapter )
su_global_acc( UList, dd_dx9_shade_tree )
su_global_acc( UList, dd_gl_shade_tree )

su_global_acc( Bool, dapp_fullscreen )
su_global_acc( UInt, dapp_width )
su_global_acc( UInt, dapp_height )
su_global_acc( UInt, dapp_sound_channel_count )



SU_BEGIN_STARTER( CoreGlobals )



void OnStage ( Stage stage )
{
	if( stage == STAGE_CREATE )
	{
		Pool* p = globals::syspool;
		SU_ASSERT( p );

		// open consoles

		if( globals::consoleclassid )
		{
			if( globals::consoleclassid == uid::MConsoleNet )
			{
				AnyArray<2> opt_cnet;
				opt_cnet.set( 0, "saddr", "127.0.0.1" );

				opt_cnet.set( 1, "label", "stdout"    );
				globals::cout = factory::CreateObject<IConsole>( uid::MConsoleNet, p, p, &opt_cnet );

				opt_cnet.set( 1, "label", "stderr" );
				globals::cerr = factory::CreateObject<IConsole>( uid::MConsoleNet, p, p, &opt_cnet );

				#if (SU_BUILD_CONSOLE)
				opt_cnet.set( 1, "label", "stddbg" );
				globals::cdbg = factory::CreateObject<IConsole>( uid::MConsoleNet, p, p, &opt_cnet );
				#endif
			}
			else
			{
				AnyArray<1> opt;
				opt.set( 0, "label", "stdout" );
				globals::cout = factory::CreateObject<IConsole>( globals::consoleclassid, p, p, &opt );

				opt.set( 0, "label", "stderr" );
				globals::cerr = factory::CreateObject<IConsole>( globals::consoleclassid, p, p, &opt );

				#if (SU_BUILD_CONSOLE)
				opt.set( 0, "label", "stddbg" );
				globals::cdbg = factory::CreateObject<IConsole>( globals::consoleclassid, p, p, &opt );
				#endif
			}
		
			if( !globals::cout || !globals::cerr )
			{
				SafeRelease( globals::cout );
				SafeRelease( globals::cerr );
				SafeRelease( globals::cdbg );
			}
		}

		// default std out
		if( !globals::cout )
		{
			AnyArray<1> opt;
			opt.set( 0, "label", "stdout" );
			globals::cout = factory::CreateObject<IConsole>( uid::MConsoleStd, p, p, &opt );
			SU_ASSERT( globals::cout );
		}
		
		// default std err
		if( !globals::cerr )
		{
			AnyArray<1> opt;
			opt.set( 0, "label", "stderr" );
			globals::cerr = factory::CreateObject<IConsole>( uid::MConsoleStd, p, p, &opt );
			SU_ASSERT( globals::cerr );
		}

		#if (SU_BUILD_CONSOLE)
		// default std debug
		if( !globals::cdbg )
		{
			AnyArray<1> opt;
			opt.set( 0, "label", "stddbg" );
			globals::cdbg = factory::CreateObject<IConsole>( uid::MConsoleStd, p, p, &opt );
			SU_ASSERT( globals::cdbg );
		}
		#endif
	}
	else if( stage == STAGE_RELEASE )
	{
		SafeRelease( globals::cout );
		SafeRelease( globals::cerr );
		SafeRelease( globals::cdbg );
	}
}


SU_END_STARTER( CoreGlobals )



