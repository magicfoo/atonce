/*LIC-HDR*****************************************************************
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



#ifndef _su_globals_h_
#define _su_globals_h_


su_type( Pool		 )
su_type( IDepGraph	 )
su_type( IFileSystem )
su_type( IAllocator	 )

#define var		extern su_ie
#define vol		extern volatile su_ie




namespace su { namespace globals
{

	var byte				corestate;			// (0) not initialized, (1) initialized, (2) shuted
	var	byte*				profilelo;
	var	byte*				profilehi;
	var	Pool*				syspool;			// system pool, used to allocate system objects like consoles, stackpool, ...
	var	Pool*				obspool;			// shared obstack pool
	var	uint				obspoolbs;			// capacity byte size of the obstack
	var	uint				authmode;			// authoring mode. 0:disabled 1:enabled
	var	Pool*				authpool;			// unlimited authoring pool (NULL in retailed version)
	var	pcstr				sessionname;
	var	uint32				consoleclassid;		// uid of the console implementation (0 for the default)
	var pstr				consolenetaddr;
	var pstr				consolefilename;
	var bool				consolefileappend;
	var	IConsole*			cout;
	var	IConsole*			cerr;
	var	IConsole*			cdbg;
	var	IFileSystem*		fs;
	var	uint32				revision_number;
	var	pcstr				buildlevel;
	var uint				maxsock;			// limit number of net socket opened at the same
	var uint				maxhfs;				// limit number of http-fs client
	var	uint				camgrabber;			// default camera grabber
	var	uint				binarizemaxbs;
	var uint				rscmanbufferbs;		// capacity in bytes of the rsc-manager (one per instance)
	var uint				camgrabquality;		// [0;3]


	// Microsoft Windows
	#if defined(SU_HW_WINDOWS)
	var	pcstr				win_modulefilename;
	var	pointer				win_hinstance;		// HINSTANCE __*
	var	pcstr				win_cmdline;
	var	int					win_showcmd;
	var	pointer				win_basesp;
	#endif


	// Sony PSP2
	#if defined(SU_HW_PSP2)
	var	int					psp2_argc;
	var	char**				psp2_argv;
	var	uint32				psp2_basesp;
	var uint				psp2_parametersbs;
	var uint				psp2_vdmringbs;
	var uint				psp2_vertexringbs;
	var uint				psp2_fragmentringbs;
	var uint				psp2_fragmentusseringbs;
	#endif


	// Nintendo RVL2
	#if defined(SU_HW_RVL2)
	var	int					rvl2_argc;
	var	char**				rvl2_argv;
	var	uint32				rvl2_basesp;
	#endif


	// Apple Darwin
	#if defined(SU_HW_DARWIN)
	var	int					drw_argc;
	var	char**				drw_argv;
	var	void*				drw_mtx0;
	var	void*				drw_mtx1;
	var	void*				drw_cfrunloop;
	var	uint				drw_dsbuffbsize;
	#endif


	// Apple iOS
	#if defined(SU_HW_IOS)
	typedef void (*ios_appdeleg_func_t)( void* );
	var	int					ios_argc;
	var	char**				ios_argv;
	var	void*				ios_appdeleg;
	vol	ios_appdeleg_func_t	ios_appdeleg_func;
	vol	pointer				ios_appdeleg_param;
	var	void*				ios_mtx0;
	var	void*				ios_mtx1;
	vol	uint				ios_cbflags;
	vol	void*				ios_ti;
	vol	float				ios_tiper;
	vol	float				ios_tinewper;
	vol	uint				ios_ticount;
	var	void*				ios_thid;
	var	char*				ios_bundlepath;
	var	void*				ios_cfrunloop;
	var	uint				ios_dsbuffbsize;
	var	pointer				ios_mainwindow;
	var int					ios_devorientation;			// get 0:portrait 1:landscape 2:inversed-portrait 3:inversed-landscape
	var bool				ios_devisipad;
	var bool				ios_devisipad1;
	var bool				ios_devisipad2;
	var bool				ios_devisiph4;
	var bool				ios_devisiph3;
    var bool				ios_havecamera;
    var bool                ios_haveretina;
	var float				ios_version;
	var uint				ios_screen_w;
	var uint				ios_screen_h;
	
	var	bool				ios_sbarvisible;
	var int					ios_sbarorientation;		// get,set	0:top 1:right 2:bottom 3:left
	var bool				ios_enablehd;               // user set
	
	inline void				ios_appdeleg_call( ios_appdeleg_func_t infunc, pointer inparam );
	#endif


	// Google Android
	#if defined( SU_HW_ANDROID )
	var void*				and_jnienv;
	var void*				and_activity;
	var void*				and_app;
	var void*				and_holdercb;
	var void*				and_jfs;
	var void*				and_swapf;
	var const char*			and_apkpath;
	var const char*			and_tmppath;
	var const char*			and_appdatapath;
	var	void*				and_mtx0;
	var	void*				and_mtx1;
	var	bool				and_bexit;
	#endif


	// DirectDriver
	var	pointer				dd_win_embedded_hwnd;
	var	pointer				dd_win_focus_hwnd;
	var	bool				dd_fullscreen;
	var	uint				dd_fullscreen_width;
	var	uint				dd_fullscreen_height;
	var	bool				dd_soft_vtx_processing;
	var	uint				dd_ddraw;
	var	uint				dd_dsound;
	var	uint				dd_dx9_adapter;
	var	uint*				dd_dx9_shade_tree;
	var	uint*				dd_gl_shade_tree;


	// DirectApp
	var	bool				dapp_fullscreen;
	var	uint				dapp_width;
	var	uint				dapp_height;
	var uint				dapp_sound_channel_count;
} }




#undef	var
#undef	vol


#include "su_globalsi.h"
#endif // _su_globals_h_



