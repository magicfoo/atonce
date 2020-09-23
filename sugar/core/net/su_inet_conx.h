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






struct SockBuffer
{
	byte			bytes[INET_SND_RCV_BUFFER*2];
	uint			size;
					SockBuffer		(									);
	bool			send			( int so, byte* inaddr, uint inbs	);
	bool			sendf			( int so, pcstr format, ...			);
	bool			flush			( int so							);
	void			cleanup			(									);
	bool			empty			(									);
	void*			addr			(									);
	uint			capacity		(									);
};



struct SockLine
{
	PString			line;
	bool			readline		( int so							);
	void			cleanup			(									);
};



struct Http
{
	int				step;
	bool			post;				// PUT or POST ?
	bool			header;				// manage header
	uint			read_bs;
	uint			write_bs;
	LString			request;
	LString			response;
	int				respcode;
	bool			respuntilclosed;	// Connection: close ?
	bool			respchunked;		// Transfer-Encoding: chunked ?  (http://en.wikipedia.org/wiki/Chunked_transfer_encoding)
	long			respchunkbs;
	void			cleanup				(	);
};



struct Conx
{
	State			stt;
	int				sock;
	SockBuffer		sockb;
	SockLine		sockl;
	URL				url;
	Http			http;
	bool			try_connecting;
	Time			t0;
	Any				opt[inet::O_LAST];

	bool			init			( );
	void			shut			( );

	bool			so_connected	( );
	bool			so_connecting	( );
	bool			check_connected	( );

	void			updstate		( );
	State			getstate		( );
	void			cleanup			( );

	bool			setopt			( inet::Option, const Any& );
	Any				getopt			( inet::Option );
	void			delopt			( inet::Option );

	int				perform			( uint );
	int				perform_http	( uint );

	bool			connect			( );
	void			disconnect		( );
	int				send			( void*, uint );
	int				receive			( void*, uint );

	void			call_hook		( int, void* ud=NULL );
	void			start_timer		( );
	bool			check_timer		( uint );
};






SockBuffer::SockBuffer ( )
{
	size = 0;
}


bool SockBuffer::send ( int so, byte* inaddr, uint inbs	)
{
	SU_REQUIRE_OR_TRUE( inaddr );
	SU_REQUIRE_OR_TRUE( inbs );

	if( size+inbs > sizeof(bytes) )
		return false;

	util::Memcopy( bytes+size, inaddr, inbs );
	size += inbs;

	flush( so );
	return true;
}


bool SockBuffer::sendf ( int so, pcstr format, ... )
{
	SU_REQUIRE_OR_TRUE( format );

	util::SafeBuffer<char,1024>	line;

	va_list	arg;
	va_start( arg, format );
	uint s = vsprintf( line.addr(), format, arg );
	va_end( arg );

	return send( so, (byte*)line.addr(), s );
}


bool SockBuffer::flush ( int so )
{
	if( size ) {
		uint32 obs = 0;
		if( socket::Send(so,obs,bytes,size) )
		{
			SU_ASSERT( obs <= size );
			size -= obs;
			if( size )
				util::Memmove( bytes, bytes+obs, size );
		}
	}
	return (size==0);
}


void SockBuffer::cleanup ( )
{
	size = 0;
}


bool SockBuffer::empty ( )
{
	return (size==0);
}


void* SockBuffer::addr ( )
{
	return bytes;
}


uint SockBuffer::capacity ( )
{
	return sizeof(bytes);
}





bool SockLine::readline ( int so )
{
	char c;
	uint s;

	while( socket::Receive(so,s,&c,1) && s )
	{
		line.append( c );
		if( line.endswith("\r\n") )
			return true;
	}

	return false;
}


void SockLine::cleanup ( )
{
	line.clear();
}






void Http::cleanup ( )
{
	step = 0;
	post = false;
	header = true;
	read_bs = 0;
	write_bs = 0;
	request.clear();
	response.clear();
	respcode = 0;
	respchunked = false;
	respuntilclosed = false;
}



void free_any ( Any& a, Pool* p )
{
	if( a.isString() )
		p->Free( (void*)a.asString() );
	a.undef();
}



void assign_any ( Any& a, const Any& ref, Pool* p )
{
	free_any( a, p );

	pcstr r = ref.asString();
	if( r && *r )
	{
		int l = util::Strlen(r);
		pstr k = (pstr) p->Alloc(l+1);
		if( k )
		{
			util::Strcopy(k,r);
			a.set( k );
		}
	}
	else
	{
		a.copy( ref );
	}
}





bool Conx::init ( )
{
	stt = S_INVALID;
	try_connecting = false;

	sock = socket::Create( socket::T_TCP );
	SU_REQUIRE_OR_FALSE( sock >= 0 );

	util::Zero( opt );

	cleanup();
	sockb.cleanup();
	return true;
}


void Conx::shut ( )
{
	cleanup();
	socket::Release( sock );
	sock = -1;
	stt = S_INVALID;
}


void Conx::cleanup ( )
{
	disconnect();

	for( uint i=0 ; i<inet::O_LAST ; i++ )
		delopt( (inet::Option)i );

	opt[O_VERBOSE].set( false );
	opt[O_HEADER].set( true );
	opt[O_TIMEOUT_MS].set( 10 );
	opt[O_HTTP_COOKIE].set( true );
}


bool Conx::so_connected ( )
{
	socket::State so = socket::GetState(sock);
	return (so == socket::S_CONNECTED);
}


bool Conx::so_connecting ( )
{
	socket::State so = socket::GetState(sock);
	return (so == socket::S_CONNECTING);
}


bool Conx::check_connected ( )
{
	if( so_connected() )
	{
		return true;
	}
	else
	{
		disconnect();
		call_hook( HR_DISCONNECTED );
		return false;
	}
}


void Conx::updstate ( )
{
	if( stt <= S_CONNECTED )
	{
		if( so_connected() )
			stt = S_CONNECTED;
		else if( so_connecting() )
			stt = S_CONNECTING;
		else
			stt = S_DISCONNECTED;
	}
}


inet::State Conx::getstate ( )
{
	updstate();
	return stt;
}


void Conx::delopt ( inet::Option ino )
{
	SU_REQUIRE_OR_RETURN( ino < inet::O_LAST );
	free_any( opt[ino], inet_pool() );
}


bool Conx::setopt ( inet::Option ino, const Any& invalue )
{
	SU_REQUIRE_OR_FALSE( ino < inet::O_LAST );

	if( ino == inet::O_URL )
	{
		// check the url compliance !
		URL u;
		if( !DecomposeURL(u,invalue.asString()) )
			return false;
	}

	if( ino == inet::O_URL )		disconnect();
	if( ino == inet::O_PORT )		disconnect();
	if( ino == inet::O_USERNAME )	disconnect();
	if( ino == inet::O_USERPWD )	disconnect();

	assign_any( opt[ino], invalue, inet_pool() );
	return true;
}


Any Conx::getopt ( inet::Option ino )
{
	SU_REQUIRE_OR_RETURN_V( ino<inet::O_LAST, Any() );
	return opt[ino];
}


int Conx::receive ( void* inbuffer, uint inbs )
{
	SU_REQUIRE_OR_NEG( check_connected() );

	uint32 bs = 0;
	if( socket::Receive(sock,bs,inbuffer,inbs) )
		return bs;

	return 0;
}


int Conx::send ( void* inbuffer, uint inbs )
{
	SU_REQUIRE_OR_NEG( check_connected() );

	if( !sockb.flush(sock) )
		return 0;

	uint32 bs = 0;
	if( socket::Send(sock,bs,inbuffer,inbs) )
		return bs;

	return 0;
}


bool Conx::connect ( )
{
	updstate();

	if( stt > S_DISCONNECTED )
		return true;

	if( !DecomposeURL(url,opt[O_URL].asString()) )
		return false;

	if( opt[O_PROTOCOL].isString() )
		url.proto = opt[O_PROTOCOL].asString();

	url.proto.tolower();

	if( url.port==0 && url.proto.size() )
	{
		if( url.proto.equals("http") )		url.port = 80;
		if( url.proto.equals("https") )		url.port = 8080;
		if( url.proto.equals("ftp") )		url.port = 21;
	}

	if( opt[O_PORT].isInt() )
		url.port = opt[O_PORT].asInt();

	if( opt[O_USERNAME].isString() )
		url.uname = opt[O_USERNAME].asString();

	if( opt[O_USERPWD].isString() )
		url.passwd = opt[O_USERPWD].asString();

	pcstr addr = url.hname.cstr();

	if( !socket::Connect(sock,addr,url.port) )
		return false;

	updstate();
	return true;
}


void Conx::disconnect ( )
{
	socket::Close( sock );
	stt = S_DISCONNECTED;
	try_connecting = false;
}


void Conx::call_hook ( int reason, void* data )
{
	typedef void HookFunc( int, void*, void* );
	void* hf = opt[O_HOOK_FUNC].asPointer();
	void* ud = opt[O_HOOK_UDATA].asPointer();
	if( hf )
		((HookFunc*)hf)( reason, data, ud );
}


void Conx::start_timer ( )
{
	su::clock::GetTime( &t0 );
}


bool Conx::check_timer ( uint inmaxmstime )
{
	Time t;
	su::clock::GetTime( &t );
	float dt = t-t0;
	SU_ASSERT( dt >= 0.f );
	uint mst = int(dt*1000.f);
	return (mst > inmaxmstime);
}



int Conx::perform ( uint inmaxmstime )
{
	updstate();

	if( stt <= S_DISCONNECTED )
	{
		if( try_connecting )
		{
			// try connecting has failed !
			try_connecting = false;
			return -1;
		}
		else
		{
			// connect !
			if( connect() )
			{
				// try connecting
				try_connecting = true;
				return 0;
			}
			else
			{
				return -1;
			}
		}
	}
	else if( stt == S_CONNECTING )
	{
		// be patient ...
		SU_ASSERT( try_connecting );
		return 0;
	}
	else if( stt >= S_CONNECTED )
	{
		try_connecting = false;

		// flush the sock-buffer first !
		if( !sockb.empty() )
		{
			if( !check_connected() )
			{
				return -1;
			}
			
			if( !sockb.flush(sock) )
				return 0;
		}

		// perform http connection
		if( url.proto.equals("http") )
			return perform_http(inmaxmstime);
	}
	
	return -1;
}


int Conx::perform_http ( uint inmaxmstime )
{
	// start requesting
	if( stt == S_CONNECTED )
		http.cleanup();

	start_timer();
	
	// Automaton
	switch( http.step )
	{

	case 0:
		// init request
		{
			// post ?
			http.post = opt[O_READ_LENGTH].asInt() > 0;

			// send header ?
			http.header = opt[O_HEADER].asBool();

			call_hook( HR_START_REQUESTING );

			stt = S_REQUESTING;
			http.step++;
		}
		break;


	case 1 :
		// send request
		{
			if( !check_connected() )
				return -1;

			// GET /path?id=2#anch HTTP/x.y\r\n
			TString req;

			req += http.post ? "POST /" : "GET /";

			// append path ?
			if( url.path.size() )
				req += url.path;

			// append query ?
			if( url.query.size() ) {
				req += "?";
				req += url.query;
			}

			// append fragment ?
		/*	if( url.frag.size() ) {
				req += "#";
				req += url.frag;
			}	*/

			// append version
			pcstr http_v = opt[O_HTTP_VERSION].asString();
			req += " ";
			req += http_v ? http_v : "HTTP/1.1";
			req += "\r\n";

			if( sockb.send(sock,(byte*)req.cstr(),req.size()) )
			{
				http.request = req;
				http.request.strip();
				http.step++;
			}
		}
		break;


	case 2 :
		// send request-header
		{
			if( http.header )
			{
				SU_ASSERT_RESULT( sockb.sendf( sock, "User-Agent: SUGAR-APP/r%ld\r\n", (long)SU_BUILD_REVISION ) )

				// need Host header-field (for HTTP/1.1+)
				pcstr v = opt[O_HTTP_VERSION].asString();
				pcstr h = opt[O_HTTP_HOST].asString();
				if( !v || h )
				{
					if( !h )	h = url.hname.cstr();
					SU_ASSERT_RESULT( sockb.sendf( sock, "Host: %s\r\n", h ) )
				}

				// has body ?
				if( http.post )
				{
					SU_ASSERT_RESULT( sockb.sendf( sock, "Content-Length: %ld\r\n", (long)opt[O_READ_LENGTH].asIntU() ) )

					if( opt[O_READ_TYPE].isString() )
						SU_ASSERT_RESULT( sockb.sendf( sock, "Content-Type: %s\r\n", (char*)opt[O_READ_TYPE].asString() ) )
					else
						SU_ASSERT_RESULT( sockb.sendf( sock, "Content-Type: application/x-www-form-urlencoded\r\n" ) )

					if( opt[O_RANGE].asString() )
						SU_ASSERT_RESULT( sockb.sendf( sock, "Content-Range: bytes %s\r\n", (char*)opt[O_RANGE].asString() ) )
				}

				// cookie ?
				if( opt[O_HTTP_COOKIE].asBool() )
				{
					PString ck = cookjar.collect( url );
					if( ck.size() )
						SU_ASSERT_RESULT( sockb.sendf( sock, "Cookie: %s\r\n", ck.cstr() ) )
				}

				// User properties ?
				pcstr o_props = opt[O_HTTP_PROPERTIES].asString();
				if( o_props )
					SU_ASSERT_RESULT( sockb.sendf( sock, o_props, util::Strlen(o_props) ) )
			}

			if( http.header || !http.post )
				SU_ASSERT_RESULT( sockb.sendf(sock,"\r\n") )

			http.step++;
		}
		break;


	case 3 :
		// send request-body
		{
			uint s;
			byte snd[INET_SND_RCV_BUFFER];

			typedef uint ReadFunc( void*, uint, void* );
			ReadFunc*	rfunc	= (ReadFunc*) opt[O_READ_FUNC].asPointer();
			void*		rud		= opt[O_READ_UDATA].asPointer();
			uint		rlength	= opt[O_READ_LENGTH].asIntU();
			byte*		raddr	= (byte*) opt[O_READ_ADDR].asPointer();

			for( ;; )
			{
				// request-body awaited ?
				if( !http.post || rlength==0 )
				{
					http.step++;
					break;
				}

				// check connection
				if( !check_connected() )
					return -1;

				if( !sockb.flush(sock) )
					break;

				// using a read callback ?
				if( rfunc )
				{
					s = rfunc( snd, Min((uint)sizeof(snd),rlength), rud );
					if( s==0 )
						break;	// no more bytes at the moment !
					SU_ASSERT_RESULT( sockb.send(sock,snd,s) );
				}

				// using an input buffer .
				else if( raddr )
				{
					s = Min( (uint)sizeof(snd), rlength );
					SU_ASSERT_RESULT( sockb.send(sock,raddr,s) );
					raddr += s;
				}

				SU_ASSERT( s <= rlength );
				rlength -= s;
				http.read_bs += s;

				if( check_timer(inmaxmstime) )
					break;
			}

			opt[O_READ_ADDR].set( raddr );
			opt[O_READ_LENGTH].set( rlength );
		}
		break;


	case 4 :
		// starts receiving
		stt = S_RECEIVING;
		delopt( O_WRITE_LENGTH );
		sockl.cleanup();
		call_hook( HR_START_RECEIVING );
		http.step++;
		break;


	case 5 :
		// get response
		{
			if( !check_connected() )
				return -1;

			if( sockl.readline(sock) )
			{
				http.response = sockl.line.strip();
				http.respcode = util::Strtou10( http.response.cstr()+8 );
				sockl.cleanup();
				http.step++;
			}
		}
		break;


	case 6 :
		// get response-header
		{
			// check connection
			if( !check_connected() )
				return -1;

			while( sockl.readline(sock) )
			{
				sockl.line.strip();
				pcstr hl = sockl.line.cstr();

				// end of header ?
				if( sockl.line.empty() )
				{
					uint wlength = opt[O_WRITE_LENGTH].asIntU();
					if( wlength > 0 )
					{
						http.respuntilclosed = false;
						http.respchunked = false;
					}
					
					sockl.cleanup();
					http.step++;
					break;
				}

				// Set-Cookie: LSID=DQAAAK…Eaem_vYg; Domain=docs.foo.com; Path=/accounts; Expires=Wed, 13-Jan-2021 22:23:01 GMT; Secure; HttpOnly
				if( sockl.line.startswith("Set-Cookie:") )
				{
					// enable cookie mng
					if( opt[O_HTTP_COOKIE].asBool() )
					{
						Cookie ck;
						if( ck.set(hl+12) )
						{
							// default domain is hostname
							if( ck.domain.empty() )
								ck.domain = url.hname;
							// save in cookiejar
							cookjar.add( ck );
						}
					}
				}

				// Content-Length: 2544
				if( sockl.line.startswith("Content-Length:") )
				{
					long clength = util::Strtou10( hl+15 );
					opt[O_WRITE_LENGTH].set( (int)clength );
				}

				// Connection: close
				if( sockl.line.startswith("Connection:") )
				{
					http.respuntilclosed = true;
				}

				// Transfer-Encoding: chunked
				if( sockl.line.startswith("Transfer-Encoding:") )
				{
					if( util::Strstartswith(hl+19,"chunked",false) )
					{
						http.respchunked = true;
						http.respchunkbs = 0;
					}
				}

				// present header line ?
				if( opt[O_HEADER_FUNC].asPointer() )
				{
					typedef void HeaderFunc( pcstr, void* );
					void* hf = opt[O_HEADER_FUNC].asPointer();
					void* ud = opt[O_HEADER_UDATA].asPointer();
					((HeaderFunc*)hf)( hl, ud );
				}

				sockl.cleanup();

				if( check_timer(inmaxmstime) )
					break;
			}
		}
		break;


	case 7 :
		// get response-body
		{
			uint s;
			byte rcv[INET_SND_RCV_BUFFER];

			typedef bool WriteFunc( void*, uint, void* );
			WriteFunc*	wf		= (WriteFunc*) opt[O_WRITE_FUNC].asPointer();
			void*		wud		= opt[O_WRITE_UDATA].asPointer();
			uint		wlength	= opt[O_WRITE_LENGTH].asIntU();
			byte*		waddr	= (byte*) opt[O_WRITE_ADDR].asPointer();
			uint		wcap	= opt[O_WRITE_CAPACITY].asIntU();

			for( ;; )
			{
				if( http.respchunked )
				{
					if( !check_connected() )
						return -1;

					if( http.respchunkbs == 0 )
					{
						// waiting the chunk line ?
						if( !sockl.readline(sock) )
							break;

						sockl.line.strip();
						if( sockl.line.empty() )
						{
							// end of chunked body !
							http.step++;
							break;
						}

						http.respchunkbs = util::Strtolong( sockl.line.cstr(), NULL, 16 );
						sockl.cleanup();
						break;
					}
					else
					{
						// awaiting the chunk bytes ?
						if( !socket::Receive(sock,s,rcv,Min((long)sizeof(rcv),Abs(http.respchunkbs))) || s==0 )
							break;

						if( http.respchunkbs < 0 )
						{
							http.respchunkbs += s;
							break;
						}
						else
						{
							http.respchunkbs -= s;
							// end of chunk ? => remove the final CRLF !
							if( http.respchunkbs == 0 )
								http.respchunkbs = -2;
						}
					}
				}

				else if( http.respuntilclosed )
				{
					SU_ASSERT( wlength==0 );
					if( !socket::Receive(sock,s,rcv,sizeof(rcv)) || s==0 )
					{
						if( so_connected() )
						{
							// wait ...
							break;
						}
						else
						{
							// normal disconnection ...
							http.step++;
							break;
						}
					}
				}

				
				else
				{
					if( wlength==0 )
					{
						http.step++;
						break;
					}

					if( !check_connected() )
						return -1;

					if( !socket::Receive(sock,s,rcv,Min((uint)sizeof(rcv),wlength)) || s==0 )
					{
						break;
					}
				}

				// using a write callback ?
				if( wf )
				{
					if( !wf(rcv,s,wud) )
					{
						// storage error !
						disconnect();
						call_hook( HR_WRITE_FAILED );
						return -1;
					}
				}

				// using an output buffer .
				else if( waddr && wcap )
				{
					if( s > wcap )
					{
						// overflow !
						disconnect();
						call_hook( HR_WRITE_FAILED );
						return -1;
					}
					else
					{
						util::Memcopy( waddr, rcv, s );
						waddr += s;
						wcap -= s;
					}
				}

				if( wlength )	wlength -= s;
				http.write_bs += s;

				if( check_timer(inmaxmstime) )
					break;
			}

			opt[O_WRITE_ADDR].set( waddr );
			opt[O_WRITE_CAPACITY].set( wcap );
			opt[O_WRITE_LENGTH].set( wlength );
		}
		break;


	case 8 :
		// end of request
		{
			http.step = -1;
			updstate();
			stt = so_connected() ? S_CONNECTED : S_DISCONNECTED;
			call_hook( HR_SUCCESSED );
			return http.respcode;
		}
		break;
	};

	return 0;	// continue ...
}



