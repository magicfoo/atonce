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


#include <sufw-build/codec/jpegd/jpegdecoder.h>
#include <sufw/su_mapping.h>
using namespace su;


namespace 
{
	struct jpeg_decoder_mem_stream : public jpeg_decoder_stream
	{
		byte*	beg_addr;
		byte*	cur_addr;
		byte*	end_addr;

		void init ( byte* addr, uint bsize )
		{
			if( addr && bsize )
			{
				beg_addr = addr;
				cur_addr = addr;
				end_addr = addr + bsize;
			}
			else
			{
				beg_addr = NULL;
				cur_addr = NULL;
				end_addr = NULL;
			}
		}

		int read( uchar *Pbuf, int max_bytes_to_read, bool *Peof_flag )
		{
			if( !cur_addr || !end_addr )
				return -1;

			if( cur_addr >= end_addr )
			{
				if( Peof_flag )
					*Peof_flag = TRUE;
				return 0;
			}

			int nb = Min( max_bytes_to_read, int(end_addr-cur_addr) );

			util::Memcopy( Pbuf, cur_addr, nb );
			cur_addr += nb;

			return nb;
		}

		bool iseos ( )
		{
			return (cur_addr >= end_addr);
		}

		uint pos ( )
		{
			return cur_addr - beg_addr;
		}
	};


	struct jpeg_decoder_mem_heap : public jpeg_decoder_heap
	{
		Pool* p;

		void init ( Pool* inp )
		{
			p = inp->CreateSub();
			SU_ASSERT( p );
		}

		void shut ( )
		{
			Pool::Destroy( p );
		}

		void* alloc ( uint inbsize )
		{
			return p->Alloc( inbsize );
		}
	};
}





namespace
{

	struct SIDL_STUB_CLASSNAME
	{
		bool						realized;

		Pool*						pool;

		jpeg_decoder_mem_stream		dec_stream;
		jpeg_decoder_mem_heap		dec_heap;
		jpeg_decoder				dec;
		
		map::rsc_collection			m_col;
		int							dec_stage;
		int							dec_y;


		bool Init ( Pool* p, AnyList* o )
		{
			pool = p;
			realized = false;
			return TRUE;
		}

		void Shut ( )
		{
			_unrealize();
			pool = NULL;
		}

		void _unrealize ( )
		{
			if( realized )
			{
				dec_heap.shut();
				realized = false;
			}
		}


		bool Realize ( MCursor& inInOut, ICodec::Mode inMode )
		{
			SU_REQUIRE_OR_FALSE( !realized );
			SU_REQUIRE_OR_FALSE( inMode==ICodec::M_DECODE );

			dec_stream.init( inInOut.addrB(), inInOut.leftB() );
			dec_heap.init( pool );

			dec.decode_init( &dec_stream, &dec_heap, false, true );

			int errcode = dec.get_error_code();
			if( errcode==0 )
			{
				if( dec.get_width() && dec.get_height() )
				{
					if( dec.begin() == 0 )
					{
						realized = true;
						dec_stage = 0;
						return true;
					}

					errcode = dec.get_error_code();
				}
			}

			dec_heap.shut();
			return false;
		}

		void Unrealize ( MCursor& inInOut )
		{
			_unrealize();
		}

		bool IsRealized ( )
		{
			return realized;
		}





		uint	_EstimeMapBSize	(			)
		{
		//	sizeof ->  (  rsc_bitmap { rsc_bitmap_desc, rsc_bitmap_editlayer }  )

			uint nc = dec.get_num_components();
			uint dataBSize = nc * dec.get_width() * dec.get_height();
			uint mapEstim = 1024;
			return ( dataBSize + mapEstim );
		}


		void	_AddDesc	(					)
		{
			map::rsc_collection_bitmap desc;
			m_col.tag_add( desc );

			desc.width.set		( dec.get_width() );
			desc.height.set		( dec.get_height() );
			desc.wpad.set		( 0 );
			desc.hpad.set		( 0 );
		//	desc.bpp.set		( dec.get_num_components() * 8 );
			desc.astatus.set	( 0 );
		}


		bool	_AddData	(	ICodec::Result*	ioErr		)
		{
			SU_ASSERT( ioErr );

			int numco	= dec.get_num_components();
			int line_w  = dec.get_width();
			int line_bs = line_w * numco;
			uint bSize = numco * dec.get_width() * dec.get_height();
			SU_ASSERT( ( numco == 1 ) || ( numco == 3 ) || ( numco == 4 ) );

			Psm psm = ( numco == 1 ) ? PSM_L8 : PSM_R8_G8_B8;

			pcstr name = "JPEG_Codec";
			uint nameBS = util::Strlen( name );

			map::rsc_collection_bitmap_layer lay;
			map::rsc_collection_bitmap_layer::requirements	lay_req;
			lay_req.name_len = nameBS;
			lay_req.size	 = bSize;

			m_col.tag_add( lay, lay_req );

			util::Memcopy( lay.name.text.p, name, nameBS );
			lay.psm.set( psm );

			dec_y = 0;
			for( ;; )
			{
				if( dec_y >= dec.get_height() )
				{
					dec_stage = 2;
					break;
				}

				byte* pix = NULL;
				uint  len = 0;
				int errcode = dec.decode( (void**)&pix, &len );

				if( errcode != 0 )
				{
					_unrealize();
					*ioErr = ICodec::R_DATA_ERROR;

					return false;
				}

				int ci = dec_y * line_bs;
				SU_ASSERT( len == ( line_w * ( (numco==1)? 1:4 ) ) );
				
				for( uint x=0 ; x<line_w; x++ )
				{
					lay.data.set( *pix, ci++ );
					pix++;
					
					if( numco > 1 )
					{
						lay.data.set( *pix, ci++ );		pix++;
						lay.data.set( *pix, ci++ );		pix++;
						
						if( numco == 4 )
							lay.data.set( *pix, ci++ );
						
						/*Skip unused alpha*/
						pix++;
					}
				}

				dec_y++;
			}
			return true;
		}

		bool	_Convert	(	MCursor&		inOutput,
								ICodec::Result*	ioErr		)
		{
			m_col.create( inOutput.addrB() );

			_AddDesc();
			SU_REQUIRE_OR_FALSE( _AddData( ioErr ) );

			inOutput.seek( m_col.bsize() );
			_unrealize();

			return true;
		}

		ICodec::Result Convert ( MCursor& inOutput, MCursor& inInput )
		{
			SU_REQUIRE_OR_RETURN_V( realized, ICodec::R_INVALID_CALL );

			bool bSpaceOk = inOutput.leftB( _EstimeMapBSize() );
			SU_REQUIRE_OR_RETURN_V( bSpaceOk, ICodec::R_NEED_OUTPUT );

			ICodec::Result res;
			return _Convert( inOutput, &res ) ? ICodec::R_OK : res;
		}

	};


}


