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



#ifndef _su_mapping_base_h_
#define _su_mapping_base_h_




namespace su { namespace map
{


	// r/w variable-length attributes
	// signed integer, up to 32bits

	struct_ie si_attr
	{
		byte *p;		// addr
		short s,w;		// shift, width
		int	  n;		// count
		bool  ro;		// read-only
		typedef int					value_type;
		int							setup				( byte* _p, int _s, int _w, int _n );
		int							get					( int i=0 ) const;
		void						set					( int v, int i=0 );
		void						blit				( byte*, int );
		template<typename T> void	copy				( const T& );
		template<typename T> void	load				( T& );
		template<typename T> void	setarray			( T* );
									operator int		( ) const;
	};


	// unsigned integer, up to 32bits

	struct_ie ui_attr
	{
		byte *p;		// addr
		short s,w;		// shift, width
		int	  n;		// count
		bool  ro;		// read-only
		typedef uint				value_type;
		int							setup				( byte* _p, int _s, int _w, int _n );
		uint						get					( int i=0 ) const;
		void						set					( uint v, int i=0 );
		void						blit				( byte*, int );
		template<typename T> void	copy				( const T& );
		template<typename T> void	load				( T& );
		template<typename T> void	setarray			( T* );
									operator uint		( ) const;
	};


	// fixed-point number, up to 32bits

	struct_ie fx_attr
	{
		byte *p;		// addr
		short s,w;		// shift, width
		int	  n;		// count
		bool  ro;		// read-only
		typedef float				value_type;
		int							setup				( byte* _p, int _s, int _w, int _n );
		float						get					( int i=0 ) const;
		void						set					( float v, int i=0 );
		void						blit				( byte*, int );
		template<typename T> void	copy				( const T& );
		template<typename T> void	load				( T& );
		template<typename T> void	setarray			( T* );
									operator float		( ) const;
	};


	// floating-point number, up to 32bits

	struct_ie fp_attr
	{
		byte *p;		// addr
		short s,w;		// shift, width
		int	  n;		// count
		bool  ro;		// read-only
		typedef float				value_type;
		int							setup				( byte* _p, int _s, int _w, int _n );
		float						get					( int i=0 ) const;
		void						set					( float v, int i=0 );
		void						blit				( byte*, int );
		template<typename T> void	copy				( const T& );
		template<typename T> void	load				( T& );
		template<typename T> void	setarray			( T* );
									operator float		( ) const;
	};


	// the rw context

	struct_ie context
	{
		enum ErrorCode
		{
			EC_SUCCESS			= 0,
			EC_TAGID_MISSING	= 1,
			EC_MEMBER_MISSING	= 2,
			EC_INVALID_MAP		= 3,
			EC_MAP_EXPECTED		= 4,
			EC_STRING_8BITS		= 5,
			EC_STRING_OVERFLOW	= 6,
			EC_BLOB_OVERFLOW	= 7,
			EC_SEQ_OVERFLOW		= 8,
			EC_INVALID_TYPE		= 9
		};

		ErrorCode		errcode;
		char			errmsg[256];
		char			mapname[256];
		char			tagname[256];
		char			recname[256];
		char			varname[256];

						context()		{ errcode=EC_SUCCESS; }
		bool			hassuccessed()	{ return errcode==EC_SUCCESS; }
		bool			hasfailed()		{ return errcode!=EC_SUCCESS; }
	};
	
	
	// the base-tag
	
	struct_ie base_tag
	{
		ui_attr			code;
		ui_attr			slen;
		ui_attr			llen;

		static int		getcode			( byte* p );
		static byte*	getnext			( byte* p );

		int				create			( byte* p, bool forcelong );
		int				load			( byte* p );
		byte*			addr			( );
		uint			bsize			( );
	};
	
	
	// the base-map
	
	struct_ie base_map
	{
		ui_attr			csign;			// 4-byte class signature
		ui_attr			fsign;			// 4-byte format signature
		ui_attr			flen;			// length of entire map in bytes
		uint			align;			// map alignement in bytes
		base_tag		tag;			// tag cursor
	
		int				create			( byte* p, uint inalign );
		int				load			( byte* p, uint inalign );
		byte*			addr			( );
		uint			bsize			( );
		byte*			data_begin		( );
		byte*			data_end		( );
		bool			data_check_addr	( byte* pstart, byte* pend );
		int				tag_code		( );
		bool			tag_isfirst		( );
		bool			tag_islast		( );
		byte*			tag_addr		( );
		uint			tag_bsize		( );
		int				tag_seek		( byte* t );
		bool			tag_seek		( int code );
		int				tag_first		( );
		int				tag_next		( );
	
		template <typename T> int	tag_load	( T& t );
		template <typename T> bool	tag_seek	( T& t );
		template <typename T> int	tag_add		( T& t );
		template <typename T> int	tag_add		( T& t, typename T::requirements &r );
	};
	

} }



#include "su_mapping_basei.h"
#endif // _su_mapping_base_h_



