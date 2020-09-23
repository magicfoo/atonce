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



#ifndef _su_locals_h_
#define _su_locals_h_




namespace su { namespace locals
{


	struct Accessor
	{
		pcstr			key;
		Accessor*		next;
						Accessor		( );
		virtual pcstr	loadcontent		( pcstr first, pcstr end ) { return false; }
		virtual pcstr	savecontent		( pcstr first, pcstr end ) { return false; }
	};


	// bool

	struct BoolAccessor : Accessor
	{
		bool*			val;
						BoolAccessor	( pcstr ink, bool* inv );
		inline pcstr	loadcontent		( pcstr first, pcstr end );
		inline pcstr	savecontent		( pcstr first, pcstr end );
	};


	// int

	struct IntAccessor : Accessor
	{
		int*			val;
						IntAccessor		( pcstr ink, int* inv );
		inline pcstr	loadcontent		( pcstr first, pcstr end );
		inline pcstr	savecontent		( pcstr first, pcstr end );
	};


	// uint

	struct UIntAccessor : Accessor
	{
		uint*			val;
						UIntAccessor	( pcstr ink, uint* inv );
		inline pcstr	loadcontent		( pcstr first, pcstr end );
		inline pcstr	savecontent		( pcstr first, pcstr end );
	};


	// float

	struct FloatAccessor : Accessor
	{
		float*			val;
						FloatAccessor	( pcstr ink, float* inv );
		inline pcstr	loadcontent		( pcstr first, pcstr end );
		inline pcstr	savecontent		( pcstr first, pcstr end );
	};


	// string

	struct StringAccessor : Accessor
	{
		char**			val;
						StringAccessor	( pcstr ink, char** inv );
		inline pcstr	loadcontent		( pcstr first, pcstr end );
		inline pcstr	savecontent		( pcstr first, pcstr end );
	};


	// int list

	struct IListAccessor : Accessor
	{
		int**			val;
						IListAccessor	( pcstr ink, int** inv );
		inline pcstr	loadcontent		( pcstr first, pcstr end );
		inline pcstr	savecontent		( pcstr first, pcstr end );
	};


	// uint list

	struct UListAccessor : Accessor
	{
		uint**			val;
						UListAccessor	( pcstr ink, uint** inv );
		inline pcstr	loadcontent		( pcstr first, pcstr end );
		inline pcstr	savecontent		( pcstr first, pcstr end );
	};


	// real list

	struct FListAccessor : Accessor
	{
		float**			val;
						FListAccessor	( pcstr ink, float** inv );
		inline pcstr	loadcontent		( pcstr first, pcstr end );
		inline pcstr	savecontent		( pcstr first, pcstr end );
	};



	Accessor*			FirstAccessor	(								);
	Accessor*			NextAccessor	( Accessor*	inAcc				);
	Accessor*			FindAccessor	( pcstr     inkey				);

	pcstr				LoadContent		( pcstr inkey, pcstr first, pcstr end );
	pcstr				SaveContent		( pcstr inkey, pcstr first, pcstr end );


} }



#define su_local_bool( VARNAME, ID, DEFVAL ) \
			static bool ID = DEFVAL; \
			static su::locals::BoolAccessor ID##_acc( VARNAME, & ID );

#define su_local_int( VARNAME, ID, DEFVAL ) \
			static int ID = DEFVAL; \
			static su::locals::IntAccessor ID##_acc( VARNAME, & ID );

#define su_local_uint( VARNAME, ID, DEFVAL ) \
			static uint ID = DEFVAL; \
			static su::locals::UIntAccessor ID##_acc( VARNAME, & ID );

#define su_local_float( VARNAME, ID, DEFVAL ) \
			static float ID = DEFVAL; \
			static su::locals::FloatAccessor ID##_acc( VARNAME, & ID );

#define su_local_string( VARNAME, ID, DEFVAL ) \
			static char* ID = DEFVAL; \
			static su::locals::StringAccessor ID##_acc( VARNAME, & ID );

#define su_local_ilist( VARNAME, ID, DEFVAL ) \
			static int* ID = DEFVAL; \
			static su::locals::IListAccessor ID##_acc( VARNAME, & ID );

#define su_local_ulist( VARNAME, ID, DEFVAL ) \
			static uint* ID = DEFVAL; \
			static su::locals::UListAccessor ID##_acc( VARNAME, & ID );

#define su_local_flist( VARNAME, ID, DEFVAL ) \
			static float* ID = DEFVAL; \
			static su::locals::FListAccessor ID##_acc( VARNAME, & ID );



#endif // _su_locals_h_



