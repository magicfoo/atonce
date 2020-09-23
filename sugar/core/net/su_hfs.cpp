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




#include <sufw/core/su_core.h>
using namespace su;
using namespace su::rexp;


#define	DEF_WAIT_TIMEOUT	3000





namespace
{


	struct Cli
	{
		int					sock;
		hfs::State			state;
		char				addr[32];
		uint				port;
		// receiving attributes
		uint				rcvstep;
		uint				rcvbrange[2];
		uint				rcvbsize;
		uint				rcvbcount;
		// linking
		int					next;
	};

	su::vector<Cli>			cliA;
	int						cliFreeIdx;


	bool checkcon_cli ( Cli& cli )
	{
		if( cli.state == hfs::S_DISCONNECTED )
		{
			return false;
		}
		else if( socket::IsConnected(cli.sock) )
		{
			return true;
		}
		else
		{
			cli.state = hfs::S_DISCONNECTED;
			return false;
		}
	}


	bool con_cli( Cli& cli )
	{
		SU_REQUIRE_OR_FALSE( cli.sock >= 0 );

		checkcon_cli( cli );

		if( cli.state == hfs::S_DISCONNECTED )
		{
			if( !socket::ConnectWait( cli.sock, DEF_WAIT_TIMEOUT, cli.addr, cli.port ) )
			{
				return false;
			}
		}
		else
		{
			if( !socket::IsConnected( cli.sock ) )
			{
				cli.state = hfs::S_DISCONNECTED;
				return false;
			}
		}

		return true;
	}


	void discon_cli( Cli& cli )
	{
		socket::Close( cli.sock );
		cli.state = hfs::S_DISCONNECTED;
	}


	bool con_read_headerline ( Cli& cli, char* b0, char* b1 )
	{
		char* pb = b0;
		int nl = 0;

		for( ;; )
		{
			if( !socket::ReceiveWait(cli.sock,DEF_WAIT_TIMEOUT,pb,1) )
				return false;

			if( *pb=='\n' || *pb=='\r' )
				nl++;
			else
				pb++;

			if( pb >= b1 )
				return false;

			if( nl==2 )
				break;
		}

		*pb = 0;
		return true;
	}


}




int su::hfs::Create ( pcstr inAddr, uint16 inPort )
{
	SU_REQUIRE_OR_NEG( inAddr );
	SU_REQUIRE_OR_NEG( inPort );
	SU_REQUIRE_OR_NEG( util::Strlen(inAddr) < SU_MSIZE(Cli,addr) );
	SU_REQUIRE_OR_NEG( cliFreeIdx >= 0 );

	int so = socket::Create( socket::T_TCP );
	SU_REQUIRE_OR_NEG( so >= 0 );

	int cid = cliFreeIdx;
	cliFreeIdx = cliA[cid].next;
	SU_ASSERT( cliA[cid].state == S_INVALID );

	cliA[cid].state = S_DISCONNECTED;
	cliA[cid].sock  = so;
	cliA[cid].port  = inPort;
	util::Strcopy( cliA[cid].addr, inAddr );

	return cid;
}


void su::hfs::Abort ( int inCli )
{
	SU_REQUIRE_OR_RETURN( inCli >= 0 );
	SU_REQUIRE_OR_RETURN( inCli < int(cliA.size()) );
	SU_REQUIRE_OR_RETURN( cliA[inCli].state != S_INVALID );

	Cli& cli = cliA[inCli];

	discon_cli( cli );
}


void su::hfs::Release ( int inCli )
{
	SU_REQUIRE_OR_RETURN( inCli >= 0 );
	SU_REQUIRE_OR_RETURN( inCli < int(cliA.size()) );
	SU_REQUIRE_OR_RETURN( cliA[inCli].state != S_INVALID );

	int so = cliA[inCli].sock;
	socket::Release( so );

	cliA[inCli].state	= S_INVALID;
	cliA[inCli].next	= cliFreeIdx;
	cliFreeIdx			= inCli;
}


bool su::hfs::Request	(	int			inCli,
							pcstr		inURL,
							uint		inBOffset,
							uint		inBSize		)
{
	SU_REQUIRE_OR_FALSE( inCli >= 0 );
	SU_REQUIRE_OR_FALSE( inCli < int(cliA.size()) );
	SU_REQUIRE_OR_FALSE( cliA[inCli].state != S_INVALID );

	Cli& cli = cliA[inCli];
	int  so  = cli.sock;

	if( !con_cli(cli) )
		return false;

	char  buff[2048];
	char* pb = buff;

	// generate the request

	// escape the url
	PString encoded_url(inURL);
	encoded_url.replaceall( " ", "%20" );
	util::Strformat( pb, "GET %s HTTP/1.1\r\n", encoded_url.cstr() );
	pb += util::Strlen(pb);

	//utils::Strformat( pb, "Connection: close\r\n" );
	util::Strformat( pb, "Connection: keep-alive\r\n" );
	pb += util::Strlen(pb);

	if( inBOffset || inBSize )
	{
		if( inBSize )
			util::Strformat( pb, "Range: bytes=%d-%d\r\n", inBOffset, inBOffset+inBSize-1 );
		else
			util::Strformat( pb, "Range: bytes=%d-\r\n", inBOffset );
		pb += util::Strlen(pb);
	}

	util::Strformat( pb, "\r\n" );
	pb += util::Strlen(pb);

	// send it

	if( !socket::SendWait( so, DEF_WAIT_TIMEOUT, buff, pb-buff ) )
		return false;

	cli.state			= S_RECEIVING;
	cli.rcvstep			= 0;
	cli.rcvbsize		= 0;
	cli.rcvbcount		= 0;
	cli.rcvbrange[0]	= inBOffset;
	cli.rcvbrange[1]	= inBSize ? inBOffset+inBSize-1 : 0;

	return true;
}


bool su::hfs::Receive	(	int			inCli,
							byte*		inBAddr,
							uint		inBCapacity,
							uint&		outBSize		)
{
	SU_REQUIRE_OR_FALSE( inCli >= 0 );
	SU_REQUIRE_OR_FALSE( inCli < int(cliA.size()) );
	SU_REQUIRE_OR_FALSE( cliA[inCli].state == S_RECEIVING );

	outBSize = 0;

	Cli& cli = cliA[inCli];

	if( !checkcon_cli(cli) )
		return false;

	// STEP 0: read the response status-line

	if( cli.rcvstep == 0 )
	{
		char tmp[1024];
		if( !con_read_headerline( cli, tmp, tmp+sizeof(tmp) ) )
		{
			discon_cli( cli );
			return false;
		}

		if( util::Strequals(tmp,"HTTP/1.1 200 OK") )
		{
			cli.rcvstep = 1;
		}
		else if( util::Strequals(tmp,"HTTP/1.1 206 Partial Content") )
		{
			cli.rcvstep = 1;
		}
		else
		{
			discon_cli( cli );
			return false;
		}
	}

	// STEP 1: read the response header-lines

	if( cli.rcvstep == 1 )
	{
		char tmp[1024];

		for( ;; )
		{
			if( !con_read_headerline( cli, tmp, tmp+sizeof(tmp) ) )
			{
				discon_cli( cli );
				return false;
			}

			if( tmp[0]==0 )
			{
				cli.rcvstep = 2;
				break;
			}

			if( util::Strstartswith(tmp,"Content-Range: bytes") )
			{
				// Content-Range: bytes 2532-2543/2544
				char* t0 = (char*)( tmp+21 );
				char* t1 = t0 + util::Strlen(t0);

				typedef re_Type<pcstr,3>				crange_Type;
				typedef crange_Type::uinteger			crange_pos;
				typedef re_follow< crange_pos,
						re_follow< re_char<'-'>,
						re_follow< crange_pos,
						re_follow< re_char<'/'>,
								   crange_pos > > > >	crange_spec;

				crange_Type::Stack s;
				crange_spec::match( t0, t1, s );
				SU_ASSERT( s.size() == 3 );

				uint r0  = util::Strtou10( s[0].begin );
				uint r1  = util::Strtou10( s[1].begin );
				uint wr0 = cli.rcvbrange[0];
				uint wr1 = cli.rcvbrange[1];

				SU_ASSERT( r0==wr0 );
				SU_ASSERT( r1==wr1 || wr1==0 );
				if( !( r0==wr0 && (r1==wr1 || wr1==0)) )
				{
					discon_cli( cli );
					return false;
				}

				cli.rcvbsize = r1-r0+1;
			}

			if( util::Strstartswith(tmp,"Content-Length:") )
			{
				// Content-Length: 2544
				uint bs = util::Strtou10( tmp+15 );
				if( cli.rcvbsize )
				{
					SU_ASSERT( cli.rcvbsize == bs );
				}
				else
				{
					cli.rcvbsize = bs;
				}
			}
		};
	}

	// STEP 2: read the message body bytes

	if( cli.rcvstep == 2 )
	{
		if( cli.rcvbsize == 0 )
		{
			discon_cli( cli );
			return false;
		}

		uint rcvlimit = cli.rcvbsize - cli.rcvbcount;
		uint rcvbs = rcvlimit < inBCapacity ? rcvlimit : inBCapacity;

		if( !socket::Receive(cli.sock,outBSize,inBAddr,rcvbs) )
		{
			discon_cli( cli );
			return false;
		}

		cli.rcvbcount += outBSize;

		if( cli.rcvbcount == cli.rcvbsize )
		{
			cli.state = S_CONNECTED;
		}
	}

	return true;
}




su::hfs::State su::hfs::GetState ( int	inCli	)
{
	SU_REQUIRE_OR_RETURN_V( inCli >= 0, hfs::S_INVALID );
	SU_REQUIRE_OR_RETURN_V( inCli < int(cliA.size()), hfs::S_INVALID );

	Cli& cli = cliA[inCli];

	checkcon_cli( cli );

	return cli.state;
}



bool su::hfs::IsState	(	int			inCli,
							State		inState		)
{
	State s = GetState( inCli );
	return ( s == inState );
}


bool su::hfs::IsReceiving	(	int		inCli		)
{
	return IsState( inCli, S_RECEIVING );
}


bool su::hfs::IsConnected	(	int		inCli		)
{
	State s = GetState( inCli );
	return ( s > S_DISCONNECTED );
}



bool su::hfs::RequestContent	(	int					inCli,
									pcstr				inURL,
									Pool*				inPool,
									uint&				outBSize,
									byte*&				outBAddr		)
{
	SU_REQUIRE_OR_FALSE( inPool );
	SU_REQUIRE_OR_FALSE( inCli >= 0 );
	SU_REQUIRE_OR_FALSE( inURL );

	outBSize = 0;
	outBAddr = NULL;

	if( hfs::Request(inCli,inURL) )
	{
		uint  bcap  = 4096;
		byte* baddr = (byte*) inPool->Alloc( bcap );
		uint  bpos  = 0;

		for( ;; )
		{
			core::ProcessEvent();

			if( !baddr )
				break;

			if( bpos > bcap/2 )
			{
				byte* ba = (byte*) inPool->Alloc( bcap*2 );
				if( !ba )
					break;

				util::Memcopy( ba, baddr, bpos );
				baddr = ba;
				bcap *= 2;
			}

			uint bread;
			if( !hfs::Receive(inCli,baddr+bpos,bcap-bpos,bread) )
				break;

			bpos += bread;
			SU_ASSERT( bpos <= bcap );

			// finished ?
			if( !hfs::IsReceiving(inCli) )
			{
				outBSize = bpos;
				outBAddr = baddr;
				return true;
			}
		}

		inPool->Free( baddr );
	}

	Abort( inCli );
	return false;
}




bool su::hfs::RequestContent	(	int					inCli,
									pcstr				inURL,
									byte*				inBAddr,
									uint				inBCapacity,
									uint				inBOffset,
									uint				inBSize,
									uint&				outBSize		)
{
	SU_REQUIRE_OR_FALSE( inCli >= 0 );
	SU_REQUIRE_OR_FALSE( inURL );
	SU_REQUIRE_OR_FALSE( inBAddr );
	SU_REQUIRE_OR_FALSE( inBCapacity );

	outBSize = 0;

	if( hfs::Request(inCli,inURL,inBOffset,inBSize) )
	{
		uint bpos = 0;

		for( ;; )
		{
			core::ProcessEvent();

			uint bread;
			if( !hfs::Receive(inCli,inBAddr+bpos,inBCapacity-bpos,bread) )
				break;

			bpos += bread;
			SU_ASSERT( bpos <= inBCapacity );

			// finished ?
			if( !hfs::IsReceiving(inCli) )
			{
				outBSize = bpos;
				return true;
			}

			// overflow ?
			if( bpos == inBCapacity )
				break;
		}
	}

	Abort( inCli );
	return false;
}






SU_BEGIN_STARTER( CoreHFS )

void OnStage ( Stage stage )
{
	if( stage == STAGE_START )
	{
		uint maxhfs = su::globals::maxhfs;

		cliA.relocate( globals::syspool );
		cliA.resize( maxhfs );

		if( maxhfs )
		{
			for( uint i=0 ; i<maxhfs ; i++ )
			{
				cliA[i].next  = i+1;
				cliA[i].state = hfs::S_INVALID;
			}
			cliFreeIdx = 0;
		}
		else
		{
			cliFreeIdx = -1;
		}
	}
	else if( stage == STAGE_TERMINATE )
	{
		cliA.relocate( NULL );
		cliFreeIdx = -1;
	}
}

SU_END_STARTER( CoreHFS )



