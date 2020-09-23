/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2010 AtOnce Technologies
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



#ifndef _su_obstackobjecti_h_
#define _su_obstackobjecti_h_




namespace su
{

	template < uint Bound > inline
	ObstackObject<Bound>::ObstackObject ( )
	{
		obspool = globals::obspool;
		obstor  = NULL;
		obstop  = NULL;
		ocount  = 0;

		if( obspool )
		{
			obstor = obspool->GetAllocator()->GetInterface<IAllocatorObstack>();
			if( obstor )
			{
				obstor->Retain();
				obstop = obstor->GetTop();
			}
		}
	}


	template < uint Bound > inline
	ObstackObject<Bound>::~ObstackObject ( )
	{
		if( obstor )
		{
			Clear();
			obstor->Release();
			obstor = NULL;
			obspool = NULL;
		}
	}


	template < uint Bound >
	ObstackObject<Bound>::operator Pool* ( ) const
	{
		return obspool;
	}


	template < uint Bound >
	ObstackObject<Bound>::operator IAllocator* ( ) const
	{
		return obstor;
	}


	template < uint Bound > inline
	Pool* ObstackObject<Bound>::GetPool ( )
	{
		return obspool;
	}


	template < uint Bound > inline
	IAllocator* ObstackObject<Bound>::GetAllocator ( )
	{
		return obstor;
	}


	template < uint Bound > inline
	void ObstackObject<Bound>::SetFatal ( bool inOnOff )
	{
		if( obstor )
		{
			obstor->SetFatal( inOnOff );
		}
	}


	template < uint Bound > inline
	void ObstackObject<Bound>::Clear ( )
	{
		if( obstor )
		{
			obstor->SetFatal( false );

			for( uint i=0 ; i<ocount ; i++ )
				olist[i]->Release();
			ocount = 0;

			obstor->SetTop( obstop );

			obstor->SetFatal( true );
		}
	}


	template < uint Bound > inline
	void* ObstackObject<Bound>::Alloc ( uint inbs )
	{
		SU_REQUIRE_OR_NULL( inbs );
		SU_REQUIRE_OR_NULL( obstor );

		return obstor->Alloc( inbs );
	}


	template < uint Bound >
	template < typename I > inline
	I* ObstackObject<Bound>::CreateObject ( classid inMUID, Pool* inDataPool, AnyList* inOptions )
	{
		SU_REQUIRE_OR_NULL( inMUID );
		SU_REQUIRE_OR_NULL( obstor );
		SU_REQUIRE_OR_NULL( obspool );
		SU_REQUIRE_OR_NULL( ocount<Bound );

		I* i = factory::CreateObject<I>( inMUID, obspool, inDataPool, inOptions );
		SU_REQUIRE_OR_NULL( i );

		olist[ocount++] = i;
		i->AddRef();

		return i;
	}

}



#endif // _su_obstackobjecti_h_


