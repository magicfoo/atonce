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




su::Parameter::Parameter	(		)
{
	//
}


su::Parameter::Parameter	(	const Parameter&	a	)
{
	_name .copy( a._name );
	_value.set( a._value );
}


su::Parameter::Parameter	(	const Any&	v		)
{
	_value.set( v );
}


su::Parameter::Parameter	(	const SString&	s	)
{
	_name.copy( s );
}


su::Parameter::Parameter	(	const SString& s, const Any&	v		)
{
	_name.copy( s );
	_value.copy( v );
}


bool
su::Parameter::isNamed	(	const SString&	s,	bool cs	) const
{
	return _name.compare(s,cs) == 0;
}


bool
su::Parameter::isType	(	Any::Type t ) const
{
	return _value.isType( t );
}


bool
su::Parameter::isDefinedAs	(	Any::Type t, const SString& s, bool cs	) const
{
	return isType(t) && isNamed(s,cs);
}


su::Any::Type
su::Parameter::getType	(	) const
{
	return _value.getType();
}


pcstr
su::Parameter::getName	(	) const
{
	return _name.cstr();
}


const su::Any&
su::Parameter::getValue	(	) const
{
	return _value;
}


bool
su::Parameter::setName	(	const SString&	s	)
{
	_name.copy( s );
	return TRUE;
}


bool
su::Parameter::setValue	(	const Any&	v	)
{
	return _value.set( v );
}


bool
su::Parameter::set	(	const SString& s, const Any& v	)
{
	return setValue(v) && setName(s);
}


bool
su::Parameter::set	(	const Parameter& a	)
{
	return setValue(a._value) && setName(a._name);
}




