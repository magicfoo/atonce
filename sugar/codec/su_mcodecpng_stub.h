/*LIC-HDR********************************************************************
**
** Copyright (C) 2005-2011 AtOnce Technologies
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




#include <png.h>

#include <sufw/su_mapping.h>


using namespace su;


#define PNGSIGSIZE 8


namespace
{

	png_voidp
	su_png_malloc ( png_structp inPng, png_size_t	inBSize )
	{
		void* ud = png_get_mem_ptr( inPng );
		SU_ASSERT( ud );
		Pool* inPool = (Pool*)ud;

		return inPool->Alloc( inBSize );
	}
	
	void
	su_png_free( png_structp inPng, png_voidp inPtr )
	{
		void* ud = png_get_mem_ptr( inPng );
		SU_ASSERT( ud );
		Pool* inPool = (Pool*)ud;

		inPool->Free( inPtr );
	}

	void
	su_png_warning ( png_structp inPng, png_const_charp message )
	{
	   pcstr name = "UNKNOWN (ERROR!)";
	   pcstr test = (pcstr)png_get_error_ptr( inPng );
	   SU_STDOUT( PString( "%% : libpng warning -> %%\n" ).arg( (test) ? test : name ).arg( message ) );
	}
	void
	su_png_error( png_structp inPng, png_const_charp message)
	{
	   su_png_warning( inPng, message );
	}

	void png_read_data( png_structp pngPtr, png_bytep data, png_size_t length )
	{
		//Here we get our IO pointer back from the read struct.
		//This is the parameter we passed to the png_set_read_fn() function.
		//Our std::istream pointer.
		png_voidp a = png_get_io_ptr( pngPtr );
		//Cast the pointer to std::istream* and read 'length' bytes into 'data'
		MCursor* inC = (MCursor*)a;

		inC->readB( data, length );
	//	((std::istream*)a)->read((char*)data, length);
	}


	struct SIDL_STUB_CLASSNAME
	{
		Pool*		m_pool;
		bool		m_isRealized;
		

		png_structp	m_pngPtr;
		png_infop	m_infoPtr;
		
		uint		m_width;
		uint		m_height;
		uint		m_colorComp;
		Psm			m_oPsm;
		bool		m_hasAlpha;

		map::rsc_collection m_col;



		bool Init ( Pool* p, AnyList* o )
		{
			m_pool = p;

			m_isRealized = false;
			m_pngPtr  = NULL;
			m_infoPtr = NULL;

			m_width = 0;
			m_height = 0;
			m_colorComp = 0;
			m_oPsm = PSM_UNKNOWN;
			m_hasAlpha = false;


			return true;
		}


		void Shut ( )
		{
			_unrealize();
		}

		bool IsRealized ( )
		{
			return m_isRealized;
		}

		bool Realize ( MCursor& inInOut, ICodec::Mode inMode )
		{
			Shut();
            
			m_pngPtr = png_create_read_struct_2( PNG_LIBPNG_VER_STRING, (void*)"MCodecPNG", su_png_error, su_png_warning, m_pool, su_png_malloc, su_png_free );
			SU_ASSERT( m_pngPtr );
			SU_REQUIRE_OR_FALSE( m_pngPtr );

			m_infoPtr = png_create_info_struct( m_pngPtr );
			if( m_infoPtr )
			{
				if( setjmp( png_jmpbuf( m_pngPtr ) ) )
				{
					png_destroy_read_struct( &m_pngPtr, NULL, NULL );
					return false;
				}

				png_set_read_fn( m_pngPtr, (void*)&inInOut, png_read_data );
				png_read_info( m_pngPtr, m_infoPtr );

				m_width  = png_get_image_width ( m_pngPtr, m_infoPtr );
				m_height = png_get_image_height( m_pngPtr, m_infoPtr );

				if( m_width && m_height )
				{
					m_colorComp = png_get_channels( m_pngPtr, m_infoPtr );

					uint color_type = png_get_color_type( m_pngPtr, m_infoPtr );
					switch( color_type )
					{
						case PNG_COLOR_TYPE_PALETTE:
						{
							png_set_palette_to_rgb( m_pngPtr );

							m_colorComp = png_get_channels( m_pngPtr, m_infoPtr );

							color_type = png_get_color_type( m_pngPtr, m_infoPtr );
							SU_ASSERT( color_type != PNG_COLOR_TYPE_PALETTE );

							break;
						}
						case PNG_COLOR_TYPE_GRAY:
						{
							SU_ASSERT( m_colorComp == 1 );
							uint bd = png_get_bit_depth( m_pngPtr, m_infoPtr );
							if( bd < 8 )
							{
								png_set_expand_gray_1_2_4_to_8( m_pngPtr );
							}
							
							png_set_gray_to_rgb( m_pngPtr );
							m_colorComp = 3;
							break;
						}
						case PNG_COLOR_TYPE_GRAY_ALPHA:
						{
							SU_ASSERT( m_colorComp == 2 );

							png_set_gray_to_rgb( m_pngPtr );
							m_colorComp = 4;
							break;
						}
					}

					// if the image has a transperancy set.. convert it to a full Alpha channel..
					if( png_get_valid( m_pngPtr, m_infoPtr, PNG_INFO_tRNS ) )
					{
						png_set_tRNS_to_alpha( m_pngPtr );
						m_colorComp += 1;
					}
			
					uint bitdepth = png_get_bit_depth( m_pngPtr, m_infoPtr );
					if( bitdepth == 16 )
					{
						png_set_strip_16( m_pngPtr );
						bitdepth = png_get_bit_depth( m_pngPtr, m_infoPtr );
					}
					SU_ASSERT( bitdepth == 8 );
					SU_ASSERT( m_colorComp == 3 || m_colorComp == 4 );

					if		( m_colorComp == 1 )		m_oPsm = PSM_L8;
					else if	( m_colorComp == 2 )		m_oPsm = PSM_A8L8;
					else if	( m_colorComp == 3 )		m_oPsm = PSM_R8_G8_B8;
					else if	( m_colorComp == 4 )		m_oPsm = PSM_R8_G8_B8_A8;
					else								m_oPsm = PSM_UNKNOWN;

					if( bitdepth == 8 && m_oPsm != PSM_UNKNOWN )
					{
						m_hasAlpha = ( m_oPsm == PSM_A8L8 ) || ( m_oPsm == PSM_R8_G8_B8_A8 );
						m_isRealized = true;
						return true;
					}
				}
				png_destroy_info_struct( m_pngPtr, &m_infoPtr );
			}
			png_destroy_read_struct( &m_pngPtr, NULL, NULL );
			return false;
		}





		void Unrealize ( MCursor& inInOut )
		{
			_unrealize();
		}

		
		void _unrealize(  )
		{
			if( m_pngPtr )
			{
				png_destroy_info_struct( m_pngPtr, &m_infoPtr );
				png_destroy_read_struct( &m_pngPtr, NULL, NULL );
			}
			m_isRealized = false;
			m_width = 0;
			m_height = 0;
			m_colorComp = 0;
		}



		ICodec::Result Convert ( MCursor& inOutput, MCursor& inInput )
		{
			SU_REQUIRE_OR_RETURN_V( m_isRealized, ICodec::R_INVALID_CALL );

			bool bSpaceOk = inOutput.leftB( _EstimeMapBSize() );
			SU_REQUIRE_OR_RETURN_V( bSpaceOk, ICodec::R_NEED_OUTPUT );

			ICodec::Result res;
			if( _Convert( inOutput, inInput, &res ) )
				return ICodec::R_OK;

			return res;


			// Your code here.
			return ICodec::R_OK;
		}

		uint	_EstimeMapBSize	(			)
		{
		//	sizeof ->  (  rsc_bitmap { rsc_bitmap_desc, rsc_bitmap_editlayer }  )
			uint dataBSize = m_colorComp * m_width * m_height;
			uint mapEstim = 1024;
			return ( dataBSize + mapEstim );
		}

		void	_AddDesc	(					)
		{
			map::rsc_collection_bitmap desc;
			m_col.tag_add( desc );

		//	uint aStatus = ( m_hasAlpha ) ? map::rsc_collection_bitmap::AS_VARIED : map::rsc_collection_bitmap::AS_OPAQUE;
			uint aStatus = ( m_hasAlpha ) ? 2 : 0;

			desc.width.set		( m_width );
			desc.height.set		( m_height );
			desc.wpad.set		( 0 );
			desc.hpad.set		( 0 );
		//	desc.bpp.set		( m_tga.GetBitsPerPixel() );
			desc.astatus.set	( aStatus );
		}


		bool	_AddData	(	MCursor&		inInput,
								ICodec::Result*	ioErr		)
		{
			SU_ASSERT( ioErr );
			
			uint lineBS	= m_width * m_colorComp;
			uint bSize	= lineBS * m_height;

			pcstr name = "PNG_Codec";
			uint nameBS = util::Strlen( name );

			map::rsc_collection_bitmap_layer lay;
			map::rsc_collection_bitmap_layer::requirements	lay_req;
			lay_req.name_len = nameBS;
			lay_req.size	 = bSize;

			m_col.tag_add( lay, lay_req );

			util::Memcopy( lay.name.text.p, name, nameBS );

			int psm = PSM_A8L8;
			lay.psm.set( m_oPsm );

			byte* od = lay.data.p;

			byte** liAddr = m_pool->Array<byte*>( m_height );
			SU_ASSERT( liAddr );

			byte*  liI = od;
			byte** liO = liAddr;
			byte** liOE = liO + m_height;
			while( liO < liOE )
			{
				*liO++ = liI;
				liI += lineBS;
			}
			png_read_image( m_pngPtr, liAddr );

			m_pool->Free( liAddr );

			return true;
		}

		bool	_Convert	(	MCursor&		inOutput,
								MCursor&		inInput,
								ICodec::Result*	ioErr		)
		{
			m_col.create( inOutput.addrB() );
			_AddDesc();

			SU_REQUIRE_OR_FALSE( _AddData( inInput, ioErr ) );

			inOutput.goforward( m_col.bsize() << 3 );
			_unrealize();

			return true;
		}

	};


}


