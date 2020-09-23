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



#ifndef _su_socket_h_
#define _su_socket_h_



namespace su { namespace socket
{

	enum Type
	{
		T_UNDEF			= -1,
		T_TCP			= 0,
		T_UDP			= 1
	};

	enum State
	{
		S_INVALID		= -1,
		S_CLOSED		= 0,
		S_OPENED		= 1,
		S_LISTENING		= 2,
		S_CONNECTING	= 3,
		S_CONNECTED		= 4
	};

	enum ShutDownMode
	{
		SDM_Read		= 0,
		SDM_Write		= 1,
		SDM_ReadWrite	= 2
	};


	//
	// Host informations

	uint			GetHostAddrCpt		(	pcstr				inName					);

	bool			GetHostAddr			(	pcstr				inName,
											uint				inAddrNo,
											pstr				outAddr					);		// char [16]


	//
	// Socket

	int				Create				(	Type				inType					);
	void			Release				(	int					inSockId				);

	bool			Open				(	int					inSockId,
											uint16				inPort			= 0,
											pcstr				inAddr			= NULL	);

	void			Close				(	int					inSockId				);

	void			ShutDown			(	int					inSockId,
											ShutDownMode		inMode					);

	bool			Listen				(	int					inSockId,
											uint32				inPendingClients = 5	);

	bool			Accept				(	int					inSockId,
											int&				outSockId				);

	bool			Connect				(	int					inSockId,
											pcstr				inAddr,
											uint16				inPort					);
	bool			ConnectWait			(	int					inSockId,
											uint				inMsTimeout,					// in ms
											pcstr				inAddr,
											uint16				inPort					);

	bool			Receive				(	int					inSockId,
											uint32&				outRcvBSize,
											pvoid				inBuffer,
											uint32				inBSize,
											bool				peek			= FALSE	);
	bool			Send				(	int					inSockId,
											uint32&				outSndBSize,
											const void*			inBuffer,
											uint32				inBSize					);

	bool			ReceiveWait			(	int					inSockId,
											uint				inMsTimeout,					// in ms
											pvoid				inBuffer,
											uint32				inBSize					);
	bool			SendWait			(	int					inSockId,
											uint				inMsTimeout,					// in ms
											pvoid				inBuffer,
											uint32				inBSize					);

	bool			GetPeerAddr			(	int					inSockId,
											pstr*				outAddr			= NULL,
											uint16*				outPort			= NULL	);
	bool			GetSockAddr			(	int					inSockId,
											pstr*				outAddr			= NULL,
											uint16*				outPort			= NULL	);

	bool			IsRcvPending		(	int					inSockId,
											uint				inBSize			= 1		);
	bool			IsClientPending		(	int					inSockId				);

	int32			GetMTUBSize			(	int					inSockId				);
	Type			GetType				(	int					inSockId				);
	State			GetState			(	int					inSockId				);

	inline bool		IsConnected			(	int					inSockId				)	{	return GetState(inSockId) == S_CONNECTED;	}
	inline bool		IsClosed			(	int					inSockId				)	{	return GetState(inSockId) <= S_CLOSED;		}
	inline bool		IsActive			(	int					inSockId				)	{	return GetState(inSockId) >  S_OPENED;		}

} }


#endif	// _su_socket_h_




