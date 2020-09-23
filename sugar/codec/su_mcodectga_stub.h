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


#include <sufw/su_mapping.h>

using namespace su;
using namespace su::map;




namespace
{
	struct TgaHeader
	{
		byte  idlength;
		byte  colourmaptype;
		byte  datatypecode;
		short colourmaporigin;
		short colourmaplength;
		byte  colourmapdepth;
		short x_origin;
		short y_origin;
		short width;
		short height;
		byte  bitsperpixel;
		byte  imagedescriptor;

		TgaHeader		(						)
		{
			this->Clear();
		}

		void	Clear	(						)
		{
			util::Memset( this, 0, sizeof(*this) );
		}

		bool	Read	(	MCursor&	inCur	)
		{
			SU_REQUIRE_OR_FALSE( inCur.leftB( 18 ) );
		//	
			idlength		= inCur.readlsb<byte>();
			colourmaptype	= inCur.readlsb<byte>();
			datatypecode	= inCur.readlsb<byte>();
			colourmaporigin	= inCur.readlsb<short>();
			colourmaplength	= inCur.readlsb<short>();
			colourmapdepth	= inCur.readlsb<byte>();
			x_origin		= inCur.readlsb<short>();
			y_origin		= inCur.readlsb<short>();
			width			= inCur.readlsb<short>();
			height			= inCur.readlsb<short>();
			bitsperpixel	= inCur.readlsb<byte>();
			imagedescriptor	= inCur.readlsb<byte>();

			return true;
		}
	};

	struct Tga
	{
		TgaHeader	m_header;

		Tga						(							)
		{
			util::Memset( this, 0, sizeof(*this) );
		}

		bool	IsYFlipped		(							)
		{
			return !( m_header.imagedescriptor & 0x20 );
		}
		bool	Realize			(	MCursor&		ioCur	)
		{
			SU_REQUIRE_OR_FALSE( m_header.Read( ioCur ) );
		
			if( m_header.datatypecode != 2 &&
				m_header.datatypecode != 10		)
			{
				this->UnRealize();
				return false;
			}
			if( m_header.colourmaptype != 0 &&
				m_header.colourmaptype != 1		)
			{
				this->UnRealize();
				return false;
			}

			bool bppOk = false;
			int flag = ( m_header.imagedescriptor & 0xF );

			if( m_header.bitsperpixel == 16 )
			{
				bppOk = ( flag == 0 ) || ( flag == 1 );
			}
			else if( m_header.bitsperpixel == 24 )
			{
				bppOk = ( flag == 0 );
			}
			else if( m_header.bitsperpixel == 32 )
			{
				bppOk = ( flag == 8 );
			}

			if( !bppOk )
			{
				this->UnRealize();
				return false;
			}

			if( m_header.x_origin != 0 ||
				m_header.y_origin != 0	)
			{
				this->UnRealize();
				return false;
			}

			return true;
		}


		void	UnRealize		(							)	{	m_header.Clear();						}
		uint	GetWidth		(							)	{	return m_header.width;					}
		uint	GetHeight		(							)	{	return m_header.height;					}
		uint	GetBytePerPixel	(							)	{	return ( m_header.bitsperpixel / 8 );	}
		uint	GetBitsPerPixel	(							)	{	return m_header.bitsperpixel;			}

		uint	GetBSkip		(							)
		{
			return m_header.idlength + ( m_header.colourmaptype * m_header.colourmaplength );
		}
		uint	GetNumComponent	(							)
		{
			if( m_header.bitsperpixel ==  16 )		return 4;
			if( m_header.bitsperpixel ==  24 )		return 3;
			if( m_header.bitsperpixel ==  32 )		return 4;
			return 0;
		}


		bool	read16line		( ui_attr& ioAttr, uint& ioAttrByteIdx, MCursor& ioInput, int inLine )
		{
		//	byte re[2*512];

			uint cLine = IsYFlipped() ? m_header.height - (inLine+1) : inLine;
			uint lineOffset = m_header.width*16 * cLine;

			ioInput.goforward( lineOffset );
			
			int i = 0;
			while( i < m_header.width )
			{
				byte* oAddr = ioAttr.p + ioAttrByteIdx;
				int toTreat = Min( int(m_header.width) - i, 512 );

			//	ioInput.readB( re, 2*toTreat );
				byte* re = ioInput.by.ptr;
				ioInput.goforward( 2*toTreat << 3 );
				
				for( int j=0; j<toTreat; j++ )
				{
					int j2 = j*2;
					int j4 = j*4;

					oAddr[j4+0] = (re[j2+1] & 0x7c) << 1;
					oAddr[j4+1] = ((re[j2+1] & 0x03) << 6) | ((re[j2] & 0xe0) >> 2);
					oAddr[j4+2] = (re[j2] & 0x1f) << 3;
					oAddr[j4+3] = (re[j2+1] & 0x80);
				}
				ioAttrByteIdx += (toTreat * 4);
				i += toTreat;
			}

			return true;
		}

		bool	read24line		( ui_attr& ioAttr, uint& ioAttrByteIdx, MCursor& ioInput, int inLine )
		{
		//	byte re[3*512];
			
			uint cLine = IsYFlipped() ? m_header.height - (inLine+1) : inLine;
			uint lineOffset = m_header.width*24 * cLine;
			ioInput.goforward( lineOffset );

			int i=0;
			while( i<m_header.width )
			{
				byte* oAddr = ioAttr.p + ioAttrByteIdx;

				int toTreat = Min<int>( int(m_header.width) - i, 512 );

			//	ioInput.readB( re, 3*toTreat );
				byte* re = ioInput.by.ptr;
				ioInput.goforward( 3*toTreat << 3 );
				
				for( int j=0; j<toTreat; j++ )
				{
					int j3 = j*3;
					oAddr[j3+0] = re[j3+2];
					oAddr[j3+1] = re[j3+1];
					oAddr[j3+2] = re[j3  ];
				}
				ioAttrByteIdx += ( 3 * toTreat );
				i += toTreat;
			}

			return true;
		}

		bool	read32line		( ui_attr& ioAttr, uint& ioAttrByteIdx, MCursor& ioInput, int inLine )
		{
		//	byte re[4*512];
			
			
			uint cLine = IsYFlipped() ? m_header.height - (inLine+1) : inLine;
			uint lineOffset = m_header.width*32 * cLine;

			ioInput.goforward( lineOffset );

			int i = 0;
			while( i < m_header.width )
			{
				byte* oAddr = ioAttr.p + ioAttrByteIdx;
				int toTreat = Min( int(m_header.width) - i, 512 );

			//	ioInput.readB( re, 4*toTreat );
				byte* re = ioInput.by.ptr;
				ioInput.goforward( 4*toTreat << 3 );
				
				for( int j=0; j<toTreat; j++ )
				{
					int j4 = j*4;
					oAddr[j4+0] = re[j4+2];
					oAddr[j4+1] = re[j4+1];
					oAddr[j4+2] = re[j4+0];
					oAddr[j4+3] = re[j4+3];
				}
				ioAttrByteIdx += (toTreat * 4);
				i += toTreat;
			}

			return true;
		}

		bool	ReadLine		( ui_attr& ioAttr, uint& ioAttrByteIdx, MCursor& ioInput, int inLine )
		{
			// At this point, ioInput must pointed to the first line
			if( m_header.bitsperpixel ==  16 )
				return read16line( ioAttr, ioAttrByteIdx, ioInput, inLine );
			if( m_header.bitsperpixel ==  24 )
				return read24line( ioAttr, ioAttrByteIdx, ioInput, inLine );
			if( m_header.bitsperpixel ==  32 )
				return read32line( ioAttr, ioAttrByteIdx, ioInput, inLine );

			return false;
		}
	};


	struct SIDL_STUB_CLASSNAME
	{
		bool				m_isRealized;

		Tga					m_tga;
		
		map::rsc_collection m_col;
		uint64				m_inputOffSetData;



		bool Init ( Pool* p, AnyList* o )
		{
			m_isRealized	= false;

			m_inputOffSetData	= 0;

			return true;
		}

		bool    Realize ( MCursor& inInOut, ICodec::Mode inMode )
		{
			SU_REQUIRE_OR_FALSE( !m_isRealized );
			SU_REQUIRE_OR_FALSE( inMode == ICodec::M_DECODE );

			return m_isRealized = m_tga.Realize( inInOut );			
		}

		void Shut ( )
		{
			_unrealize();
		}


		void _unrealize(  )
		{
			m_tga.UnRealize();
			m_isRealized = false;
		}

		bool    IsRealized (  )
		{
			return m_isRealized;
		}



		void    Unrealize ( MCursor& inInOut )
		{
			_unrealize();
		}









		uint	_EstimeMapBSize	(			)
		{
		//	sizeof ->  (  rsc_bitmap { rsc_bitmap_desc, rsc_bitmap_editlayer }  )

			uint dataBSize = m_tga.GetNumComponent() * m_tga.GetWidth() * m_tga.GetHeight();
			uint mapEstim = 1024;
			return ( dataBSize + mapEstim );
		}

		void	_AddDesc	(					)
		{
			map::rsc_collection_bitmap desc;
			m_col.tag_add( desc );

			uint aStatus;
			if( m_tga.GetNumComponent() == 4 )	aStatus = map::rsc_collection_bitmap::AS_VARIED;
			else								aStatus = map::rsc_collection_bitmap::AS_OPAQUE;

			desc.width.set		( m_tga.GetWidth() );
			desc.height.set		( m_tga.GetHeight() );
			desc.wpad.set		( 0 );
			desc.hpad.set		( 0 );
		//	desc.bpp.set		( m_tga.GetBitsPerPixel() );
			desc.astatus.set	( aStatus );
		}


		bool	_AddData	(	MCursor&		inInput,
								ICodec::Result*	ioErr		)
		{
			SU_ASSERT( ioErr );

			uint outLineBS	= m_tga.GetWidth() * m_tga.GetNumComponent();
			uint inLineBS	= m_tga.GetWidth() * m_tga.GetBytePerPixel();
			uint bSize		= outLineBS * m_tga.GetHeight();

			pcstr name = "TGA_Codec";
			uint nameBS = util::Strlen( name );

			map::rsc_collection_bitmap_layer lay;
			map::rsc_collection_bitmap_layer::requirements	lay_req;
			lay_req.name_len = nameBS;
			lay_req.size	 = bSize;

			m_col.tag_add( lay, lay_req );

			util::Memcopy( lay.name.text.p, name, nameBS );
			lay.psm.set( ( m_tga.GetNumComponent() == 3 ) ? PSM_R8_G8_B8 : PSM_R8_G8_B8_A8 );
			
			uint dy = 0;
			uint bOff = 0;
			while( true )
			{
				inInput.seek( m_inputOffSetData );
				if( !inInput.leftB( inLineBS ) )
				{
					*ioErr = ICodec::R_NEED_INPUT;
					return false;
				}

				// Fill
				m_tga.ReadLine( lay.data, bOff, inInput, dy );


				// changeline
				dy++;
				if( dy >= m_tga.GetHeight() )
				{
					break;
				}
			}
			return true;
		}

		bool	_Convert	(	MCursor&		inOutput,
								MCursor&		inInput,
								ICodec::Result*	ioErr		)
		{
			SU_ASSERT( ioErr );

			m_col.create( inOutput.addrB() );
			_AddDesc();

			inInput.goforward( m_tga.GetBSkip() << 3 );
			m_inputOffSetData = inInput.pos();

			SU_REQUIRE_OR_FALSE( _AddData( inInput, ioErr ) );

			inOutput.goforward( m_col.bsize() << 3 );
			_unrealize();

			return true;
		}

		ICodec::Result    Convert ( MCursor& inOutput, MCursor& inInput )
		{
			SU_REQUIRE_OR_RETURN_V( m_isRealized, ICodec::R_INVALID_CALL );

			bool bSpaceOk = inOutput.leftB( _EstimeMapBSize() );
			SU_REQUIRE_OR_RETURN_V( bSpaceOk, ICodec::R_NEED_OUTPUT );

			ICodec::Result res;
			if( _Convert( inOutput, inInput, &res ) )
				return ICodec::R_OK;

			return res;
		}
	};


}


