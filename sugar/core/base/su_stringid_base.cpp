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


#include "../su_base.h"
using namespace su;
using namespace su::sid;



struct sid_def { uint32 hash; pcstr str; };

int			su_sid_count();
void*		su_sid_list();
sid_def&	su_sid_def( int i ) { SU_ASSERT(i>=0); SU_ASSERT(i<su_sid_count()); sid_def* l=(sid_def*)su_sid_list(); return l[i]; }


int su::sid::Count ( )
{
	return su_sid_count();
}

uint32 su::sid::Hash ( StringId id )
{
	return su_sid_def(id).hash;
}

pcstr su::sid::Str ( StringId id )
{
	return su_sid_def(id).str;
}

bool su::sid::Equals ( StringId id, pcstr inref	)
{
	return util::Strequals( inref, su_sid_def(id).str );
}

StringId su::sid::Find ( uint32 inhash )
{
	if( inhash==0 )
		return 0;

	int i=0, j=su_sid_count()-1, k;

	uint32 h;
	for( ;; ) {
		k = (i+j)>>1;
		h = su_sid_def(k).hash;
		if( h == inhash )	return k;
		if( i==j )			return null;
		if( inhash > h )	i=k+1;
		else				j=k-1;
	}
}

StringId su::sid::Find ( pcstr instr )
{
	uint32 h = util::hmac_crc32_w( instr );
	return Find( h );
}



