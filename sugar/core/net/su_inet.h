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



#ifndef _su_inet_h_
#define _su_inet_h_



su_type( IDatatree )



namespace su { namespace inet
{

	struct URL
	{
		// [proto://][username[:password]@](ip|hostname)[:port][/path/][?query][#fragment]
					URL() : port(0) {}
		SString		proto;
		SString		uname;
		SString		passwd;
		SString		ip;
		LString		hname;
		int			port;
		PString		path;
		TString		query;
		LString		frag;
	};


	enum State
	{
		S_INVALID		= -1,
		S_DISCONNECTED	= 0,
		S_CONNECTING	= 1,
		S_CONNECTED		= 2,
		S_REQUESTING	= 3,		// the request is performing
		S_RECEIVING		= 4,		// the response is performing
	};


	enum HookReason
	{
		HR_START_REQUESTING	= 0,
		HR_START_RECEIVING,
		HR_WRITE_FAILED,
		HR_DISCONNECTED,
		HR_FAILED,
		HR_SUCCESSED,
	};


	enum Option
	{
		// Behavior options

		O_VERBOSE = 0,				// Set the option to 1 to get inet to display a lot of verbose information about its operations

		O_HEADER,					// A parameter set to 1 tells inet to include the header in the body output. This is only relevant for protocols that actually have headers preceding the data (like HTTP).

		O_WRITE_LENGTH,				// Content length in bytes to receive from peer

		O_WRITE_FUNC,				// Function pointer that should match the following prototype:
									//		bool function( void* addr, uint bsize, void* userdata )
									// This function gets called by inet as soon as there is data received that needs to be taken care of.
									// Returns false to indicate a storage error.

		O_WRITE_UDATA,				// Data pointer to pass to the write function (see O_WRITE_FUNC)

		O_WRITE_ADDR,				// Start address of the input buffer that will receive the incoming body bytes

		O_WRITE_CAPACITY,			// Capacity in bytes of the input buffer that will receive the incoming body bytes

		O_READ_LENGTH,				// Content length in bytes to send to peer

		O_READ_TYPE,				// Pass a char* as the content-type of bytes to send to peer

		O_READ_FUNC,				// Function pointer that should match the following prototype:
									//		uint function( void* addr, uint bcapacity, void* userdata )
									// This function gets called by inet as soon as there is data required to send to the peer.
									// Returns amount of bytes to send. 0 is a valid value.

		O_READ_UDATA,				// Data pointer to pass to the read function (see O_READ_FUNC)

		O_READ_ADDR,				// Start address of the input buffer that will receive the incoming body bytes

		O_HEADER_FUNC,				// Function pointer that should match the following prototype:
									//		void function( pcstr header_line, void* userdata )
									// This function gets called by inet as soon as there is an incoming header line.

		O_HEADER_UDATA,				// Data pointer to pass to the header function (see O_HEADER_FUNC)

		O_HOOK_FUNC,				// Function pointer that should match the following prototype:
									//		void function( int reason, void* hookdata, void* userdata )
									// This function gets called by inet when some transaction point are reached.

		O_HOOK_UDATA,				// Data pointer to pass to the header function (see O_HEADER_FUNC)

		// Error and  debugging options

		O_LASTERROR,				// Defined by inet to store human readable error messages.
									// This buffer is self-managed by inet.

		// Network options

		O_URL,						// The actual URL to deal with.
									// This C string should match the following form :
									//		[proto://][username[:password]@](ip|hostname)[:port][/path/][?query][#fragment]

		O_PROTOCOL,					// Pass a string corresponding to the protocol to use by inet: "http", "ftp", "tftp", "smtp", ...

		O_PORT,						// Pass a long specifying witch remote port number to connect to, instead of the one specified in the URL or the default port for the used protocol.

		O_USERNAME,

		O_USERPWD,

		// HTTTP options

		O_HTTP_VERSION,				// Pass a char* as the explicit HTTP/x.y version

		O_HTTP_HOST,				// Pass a char* as the explicit HTTP/1.1 host-field

		O_HTTP_COOKIE,				// Pass 0/1 to enable/disable the cookie management

		O_HTTP_PROPERTIES,			// Pass a single string of the form "field1-name: field1-value\r\nfield2-name: field2-value".

		// SMTP options

		// TFTP options

		// FTP options

		// RTSP options

		// PROTOCOL options

		O_CRLF,						// Pass a long. If the value is set to 1 (one), inet converts Unix newlines to CRLF newlines on transfers. Disable this option again by setting the value to 0 (zero).

		O_RANGE,					// Pass a char* as parameter, which should contain the specified range you want. It should be in the format "X-Y", where X or Y may be left out. HTTP transfers also support several intervals, separated with commas as in "X-Y,N-M".

		// CONNECTION options

		O_TIMEOUT_MS,				// Pass a long as parameter containing the maximum time in seconds that you allow the iner transfer operation to take.

		O_LOW_SPEED_LIMIT,			// Pass a long as parameter. It contains the transfer speed in bytes per second that the transfer should be below during O_LOW_SPEED_TIME seconds for inet to consider it too slow and abort.

		O_LOW_SPEED_TIME,			// Pass a long as parameter. It contains the time in seconds that the transfer should be below the O_LOW_SPEED_LIMIT for the library to consider it too slow and abort.

		O_MAX_SEND_SPEED_LARGE,		// Pass a long as parameter. If an upload exceeds this speed (counted in bytes per second) on cumulative average during the transfer, the transfer will pause to keep the average rate less than or equal to the parameter value. Defaults to unlimited speed.

		O_MAX_RECV_SPEED_LARGE,		// Pass a long as parameter. If a download exceeds this speed (counted in bytes per second) on cumulative average during the transfer, the transfer will pause to keep the average rate less than or equal to the parameter value. Defaults to unlimited speed.

		// Security options

		O_LAST,						// Not a valid option.
	};


	// Connection

	int				Create				(														);
	void			Release				(	int			coid									);

	bool			SetOpt				(	int			coid, Option opt, const Any& value		);
	Any				GetOpt				(	int			coid, Option opt						);
	void			DelOpt				(	int			coid, Option opt						);

	State			GetState			(	int			coid									);
	void			Cleanup				(	int			coid									);

	bool			Connect				(	int			coid									);
	void			Disconnect			(	int			coid									);

	int				Send				(	int			coid, void* buffer, uint bsize			);
	int				Receive				(	int			coid, void* buffer, uint bsize			);

	int				Perform				(	int			coid, uint inmaxmstime=10				);		// <0:error 0:in-progress >0:finished

	bool			DecomposeURL		(	URL&		outurl, pcstr inurl						);


	// Cookie Jar

	void			CleanupCookies		(														);
	bool			PushCookies			(	IDatatree*	inDtt									);
	bool			LoadCookies			(	IDatatree*	inDtt									);
	pcstr			GetCookie			(	pcstr		indomain, pcstr inname					);

} }




#endif	// _su_inet_h_


