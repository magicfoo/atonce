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


using namespace su;


#define CONSOLE_DEFAULT_LOG		"su_console.log"




namespace
{
	
	PString cur_logfile;


	struct SIDL_STUB_CLASSNAME
	{
		Pool*	pool;

		bool Init ( Pool* p, AnyList* o )
		{
		#if !defined(SU_WINDOWS)
			return false;
		#endif

			SU_REQUIRE_OR_FALSE( p );

			pool = p;
			cur_logfile.clear();

			return true;
		}


		void Shut ( )
		{
		}


		::su::IConsole::Action AskUserAction ( ::pcstr inMessage, ::pcstr inFilename, ::pcstr inFctname, int inLineno )
		{
			return IConsole::A_DONTCARE;
		}


		bool resolve_logfile ( PString& outFilename )
		{
			SU_REQUIRE_OR_FALSE( globals::fs );
			
		#if !defined(SU_WINDOWS)
			return false;
		#endif
			outFilename.clear();

			pcstr defname = globals::consolefilename;
			if( util::Strlen(defname)==0 )
				defname = CONSOLE_DEFAULT_LOG;

			if( globals::fs->IsFullname(defname) )
			{
				outFilename = defname;
			}
			else
			{
			#if defined(SU_HW_WINDOWS)
				PString dname = globals::fs->GetVirtualPath( globals::fs->GetDirname( globals::win_modulefilename ) );
				outFilename = dname + "/" + defname;
			#endif
			}

			return true;
		}


		bool Output ( ::pcstr inMessage, ::pcstr inFilename, ::pcstr inFctname, int inLineno )
		{
			PString logfile;
			if( !resolve_logfile(logfile) )
				return false;

			bool restarted = !logfile.equals( cur_logfile );
			bool append = restarted ? globals::consolefileappend : true;


			IFileWriter* fw = globals::fs->OpenWriter( logfile, pool, true, append );
			SU_REQUIRE_OR_FALSE( fw );
			if( append )
				fw->SetPos( fw->GetSize() );

			// write header's lines
			if( restarted )
			{
				Date d;
				clock::GetDate( &d );
				PString dt = PString( "%%-%%-%% %%h%%m%%s").argl( d.mday ).argl( d.mon ).argl( d.year).argl( d.dhour ).argl( d.dmin ).argl( d.dsec );
	
				IFileWriter* fw_old = globals::fs->OpenWriter( cur_logfile, pool, true, true );
				if( fw_old )
				{
					fw_old->SetPos( fw_old->GetSize() );
					fw_old->Write( PString( "Log file changed at %% to %%" ).arg( dt ).arg( globals::fs->GetHostPath( logfile ) ) );
					SafeRelease( fw_old );
				}

				fw->Write( "\n---\n\n" );
				fw->Write( PString( "session-date: %%\n" ).arg( dt ) );
				fw->Write( "session-log:\n" );
			}

			// write message
			fw->Write( "- " );
			fw->Write( inMessage );

			SafeRelease( fw );
			cur_logfile = logfile;
			return true;
		}


		bool Input ( char & outChar )
		{
			// Your code here.
			return false;
		}


	};


}


