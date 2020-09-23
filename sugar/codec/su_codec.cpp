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


#include <sufw/core/su_core.h>
#include <sufw/su_mapping.h>
#include "su_icodec.h"
#include "su_codec.h"

using namespace su;



bool su::ICodec::Convert ( MCursor& cout, MCursor& cin, Pool* pool, classid muid )
{
	SU_REQUIRE_OR_FALSE( pool )
	SU_REQUIRE_OR_FALSE( muid )

	Pool* wp = pool->CreateSub();
	SU_REQUIRE_OR_FALSE( wp );

	bool done = false;

	ICodec* c = factory::CreateObject<ICodec>( muid, wp );

	if( c )
	{
		if( c->Realize(cin,M_DECODE) )
		{
			done = true;

			while( c->IsRealized() )
			{
				Result r = c->Convert( cout, cin );

				if( r != R_OK )
				{
					done = false;
					break;
				}
			}

			c->Unrealize(cin);
		}

		SafeRelease( c );
	}

	Pool::Destroy( wp );
	return done;
}




bool su::ICodec::Load ( map::rsc_collection& c, MCursor& cout, MCursor& cin, Pool* pool )
{
	SU_REQUIRE_OR_FALSE( pool );

	uint64 cout_pos0 = cout.pos();
	uint64 cin_pos0 = cin.pos();

	addin::Builder* cb = addin::Builder::FirstConcern( uid::ICodec );

	while( cb )
	{
		cout.seek( cout_pos0 );
		cin.seek( cin_pos0 );

		byte* cout_addr = cout.addrB();

		if( ICodec::Convert(cout,cin,pool,cb->GetMUID()) )
		{
			if( c.load(cout_addr) )
			{
				return true;
			}
		}

		cb = addin::Builder::NextConcern( uid::ICodec, cb );
	}

	return false;
}


