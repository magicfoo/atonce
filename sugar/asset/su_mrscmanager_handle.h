/*LIC-HDR********************************************************************
**
** Copyright (C) 2005-2010 AtOnce Technologies
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




struct HandleMan
{
	struct Desc
	{
		int			type;
		void*		obj;
	};

	Desc handles[ MAX_HANDLE ];

							HandleMan			( );

	int						findNonAffected		( );
	int						findObj				( void* inobj );
	void*					asObj				( int i, int intype );

	int						registerObj			( void* inobj, int intype );
	void					unregisterObj		( void* inobj );
	void					unregisterObj		( int i );
};






//
// Handle Manager
//


HandleMan::HandleMan ( )
{
	util::Zero( handles );
}


int HandleMan::findNonAffected ( )
{
	for( int i=0 ; i<MAX_HANDLE ; i++ )
		if( !handles[i].obj )
			return i;
	return -1;
}


int HandleMan::findObj ( void* inobj )
{
	if( inobj )
	{
		for( int i=0 ; i<MAX_HANDLE ; i++ )
			if( handles[i].obj==inobj )
				return i;
	}
	return -1;
}


void* HandleMan::asObj ( int i, int intype )
{
	SU_REQUIRE_OR_NULL( i>= 0 );
	SU_REQUIRE_OR_NULL( i<MAX_HANDLE );
	SU_REQUIRE_OR_NULL( handles[i].obj );
	SU_REQUIRE_OR_NULL( handles[i].type==intype );
	return handles[i].obj;
}


int HandleMan::registerObj ( void* inobj, int intype )
{
	if( inobj )
	{
		int i = findNonAffected();
		if( i >= 0 )
		{
			handles[i].obj = inobj;
			handles[i].type = intype;
			return i;
		}
	}
	return -1;
}


void HandleMan::unregisterObj ( void* inobj )
{
	int i = findObj( inobj );
	if( i >= 0 )
	{
		handles[i].obj = NULL;
		handles[i].type = 0;
	}
}


void HandleMan::unregisterObj ( int i )
{
	SU_ASSERT( i>=0 );
	SU_ASSERT( i<MAX_HANDLE );
	handles[i].obj = NULL;
	handles[i].type = 0;
}





