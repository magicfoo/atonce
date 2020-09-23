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



#ifndef _su_parameter_h_
#define _su_parameter_h_




namespace su
{


	struct_ie Parameter
	{
							Parameter	(												);
							Parameter	(	const Parameter&							);
							Parameter	(	const Any&									);
							Parameter	(	const SString&								);
							Parameter	(	const SString&, const Any&					);

		bool				isNull		(												) const;
		bool				isNamed		(	const SString&, bool cs=0					) const;
		bool				isType		(	Any::Type									) const;
		bool				isDefinedAs	(	Any::Type, const SString&, bool cs=0		) const;

		Any::Type			getType		(												) const;
		pcstr				getName		(												) const;
		const Any&			getValue	(														) const;
		bool				setName		(	const SString&								);
		bool				setValue	(	const Any&									);
		bool				set			(	const SString&, const Any&					);
		bool				set			(	const Parameter&							);

	protected:
		SString				_name;
		Any					_value;
		Unit				_unit;

		SU_NO_OPERATORS( Parameter )
	};


}


#define ParameterList(inArray) \
			&inArray[0], (sizeof(inArray)/sizeof(Parameter))




#endif // _su_parameter_h_


