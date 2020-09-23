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



#ifndef _su_hfs_h_
#define _su_hfs_h_




namespace su { namespace hfs
{

	enum State
	{
		S_INVALID		= -1,
		S_DISCONNECTED	= 0,
		S_CONNECTED		= 1,
		S_REQUESTING	= 2,
		S_RECEIVING		= 3,
	};


	int				Create			(	pcstr				inAddr,
										uint16				inPort		= 80		);

	void			Abort			(	int					inCli					);

	void			Release			(	int					inCli					);

	bool			Request			(	int					inCli,
										pcstr				inURL,
										uint				inBOffset	= 0,
										uint				inBSize		= 0			);

	bool			Receive			(	int					inCli,
										byte*				inBAddr,
										uint				inBCapacity,
										uint&				outBSize				);

	bool			RequestContent	(	int					inCli,
										pcstr				inURL,
										Pool*				inPool,
										uint&				outBSize,
										byte*&				outBAddr				);

	bool			RequestContent	(	int					inCli,
										pcstr				inURL,
										byte*				inBAddr,
										uint				inBCapacity,
										uint				inBOffset,
										uint				inBSize,
										uint&				outBSize				);

	State			GetState		(	int					inCli					);
	bool			IsState			(	int					inCli,
										State				inState					);
	bool			IsReceiving		(	int					inCli					);
	bool			IsConnected		(	int					inCli					);

} }




#endif	// _su_hfs_h_


