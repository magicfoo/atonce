/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2009 AtOnce Technologies
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




namespace
{

	struct SIDL_STUB_CLASSNAME
	{
		Pool*	m_pool;
		int		m_sockid;

		char	m_label[16];
		char	m_saddr[16];
		char	m_type[16];


		pcstr get_opt ( AnyList* inopt, pcstr optname, pstr optbuffer, uint optlen )
		{
			pcstr opt = inopt->asString(optname);
			SU_REQUIRE_OR_NULL( opt );
			SU_REQUIRE_OR_NULL( util::Strlen(opt) );
			SU_REQUIRE_OR_NULL( util::Strlen(opt)<optlen );
			util::Strcopy( optbuffer, opt, optlen );
			return optbuffer;
		}


		bool send_to_socket	( pcstr inText, bool inEOS = false )
		{
			uint bytosend, bysent;
			bytosend = util::Strlen(inText) + (inEOS?1:0);

			if( !socket::Send(m_sockid,bysent,inText,bytosend) )
				return FALSE;

			return (bysent == bytosend);
		}


		bool Init ( Pool* p, AnyList* inOpt )
		{
			SU_REQUIRE_OR_FALSE( inOpt );

			SU_REQUIRE_OR_FALSE( get_opt(inOpt,"label",m_label,sizeof(m_label)) );
			SU_REQUIRE_OR_FALSE( get_opt(inOpt,"saddr",m_saddr,sizeof(m_saddr)) );
			SU_REQUIRE_OR_FALSE( get_opt(inOpt,"type",m_type,sizeof(m_type)) );

			m_sockid = socket::Create( socket::T_TCP );
			SU_REQUIRE_OR_FALSE( m_sockid != -1 );

			if( !socket::ConnectWait( m_sockid, 100, m_saddr, 8200 ) )
			{
				socket::Release( m_sockid );
				return false;
			}

			m_pool		= p;

			bool bSuccess	=	send_to_socket( "<session name=\"" )
							&&	send_to_socket( globals::sessionname )
							&&	send_to_socket( "\" label=\"" )
							&&	send_to_socket( m_label )
							&&	send_to_socket( "\"/>", true );

			if( !bSuccess )
				socket::Release( m_sockid );

			return bSuccess;
		}


		void Shut ( )
		{
			socket::Release( m_sockid );

			m_sockid = -1;
			m_pool = NULL;
		}


		IConsole::Action    AskUserAction	(	pcstr		inMessage,
												pcstr		inFilename	= 0,
												pcstr		inFctname	= 0,
												int			inLineno	= -1	)
		{

			// clear input buffer
			while( socket::IsRcvPending( m_sockid ) )
			{
				char c;	uint rbs;
				socket::Receive( m_sockid, rbs, &c, 1 );
			}

			SU_REQUIRE_OR_RETURN_V( send_to_socket( "<askuseraction>" ),	IConsole::A_DONTCARE );

			char stmp[ 1024 ];

			send_to_socket( "\n\n" );
			send_to_socket( "User action asked !\n\n" );

			send_to_socket( "Filename : " );
			send_to_socket( inFilename ? inFilename : "-" );
			send_to_socket( "\n" );

			send_to_socket( "Function name : " );
			send_to_socket( inFctname ? inFctname : "-");
			send_to_socket( "\n" );

			util::Strformat( stmp, "%d", inLineno );
			send_to_socket( "Line no : " );
			send_to_socket( inLineno<0 ? "-" : stmp );
			send_to_socket( "\n" );


			// Output call-stack
			stack::Frame sf;
			if( stack::GetFrame(sf) )
			{
				send_to_socket( "call-stack :\n"	);
				uint depth = 0;
				for( ;; )
				{
					util::Strformat(	stmp, "[%d] %08X .t=%08X sp=%08X\n",
										depth++,
										uint32(sf.locAddr),
										uint32(sf.locAddr),
										uint32(sf.ptr)	);
					send_to_socket( stmp	);
					if( !stack::GetBackFrame(sf,sf) )
						break;
				}
			}

			if( !send_to_socket(inMessage) )
				return IConsole::A_DONTCARE;

			if( !send_to_socket("</askuseraction>",true) )
				return IConsole::A_DONTCARE;

			char c;
			if( socket::ReceiveWait( m_sockid, 1000*60*2, &c, 1 ) )
				if( ( c >= 0 ) && ( c < 4 ) )
					return (su::IConsole::Action)c;

			return IConsole::A_DONTCARE;
		}


		bool	Output	(	pcstr		inMessage,
							pcstr		inFilename,
							pcstr		inFctname,
							int			inLineno		)
		{
			if( inMessage && util::Strlen(inMessage) )
			{
				char open_s[64];
				util::Strformat( open_s, "<message type=\"%s\">", m_type );
				SU_REQUIRE_OR_FALSE( send_to_socket( open_s ) );
				SU_REQUIRE_OR_FALSE( send_to_socket( inMessage ) );
				SU_REQUIRE_OR_FALSE( send_to_socket( "</message>", true ) );
			}
			return true;
		}


		bool	Input	(	char&		outChar		)
		{
			uint outCount = 0;
			char c;
			SU_REQUIRE_OR_FALSE( socket::Receive( m_sockid, outCount, &c, 1 ) );
			return( outCount > 0 );
		}



	};


}


