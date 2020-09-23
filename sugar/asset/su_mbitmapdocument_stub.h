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


#include <sufw/codec/su_icodec.h>
#include <sufw/su_mapping.h>
using namespace su;
using namespace su::map;



#define MAX_WIDTH	4096
#define MAX_HEIGHT	4096




namespace
{
	struct SIDL_STUB_CLASSNAME
	{
		Pool*		m_pool;

		struct DocData
		{
			PString		path;
			Pool*		pool;
			byte*		data;
			uint		bsize;

			rsc_collection_bitmap	desc;

			bool	Create	(	pcstr		inPath,
								Pool*		inPool,
								const void*	inData,
								uint		inBSize	);
			void	Release	(						);

			bool	IsEmpty	(						);
		};

		DocData		m_ddata;


		bool Init ( Pool* p, AnyList* o )
		{
			m_pool = p;

			m_ddata.path  = "";
			m_ddata.pool  = NULL;
			m_ddata.data  = NULL;
			m_ddata.bsize = 0;

			return true;
		}

		void Shut ( )
		{
			m_ddata.Release();
			m_pool = NULL;
		}


		bool	OpenDocument	(	pcstr		inSrcFile,
									Pool*		inPool			)
		{
			m_ddata.Release();
			SU_ASSERT( globals::fs );
			SU_REQUIRE_OR_FALSE( inPool );

			Pool* buildPool = inPool->CreateSub();
			SU_ASSERT( buildPool );

			uint fBSize;
			void* fData;
			if( globals::fs->DumpFromFile( fBSize, fData, inSrcFile, buildPool ) )
			{
				SU_ASSERT( fData );
				SU_ASSERT( fBSize );

				uint oBSize = ( MAX_WIDTH * MAX_HEIGHT * 4 ) + 1024;
				void* oData = buildPool->Alloc( oBSize );
				SU_ASSERT( oData );

				if( oData )
				{
					addin::Builder* cb = addin::Builder::FirstConcern( uid::ICodec );
					while( cb )
					{
						MCursor ci( fData, fBSize );
						MCursor co( oData, oBSize );
						if( ICodec::Convert( co, ci, inPool, cb->GetMUID() ) )
						{
							rsc_collection c;
							if( c.load( (byte*)oData ) )
							{
								if( m_ddata.Create( inSrcFile, inPool, oData, c.bsize() ) )
								{
									break;
								}
							}
						}

						cb = addin::Builder::NextConcern( uid::ICodec, cb );
					}
					buildPool->Free( oData );
				}
				buildPool->Free( fData );
			}

			Pool::Destroy( buildPool );
			return ( !m_ddata.IsEmpty() );
		}



		
		bool	_GetLayer	(	rsc_collection_bitmap_layer*	ioLay,
								uint							inIdx	)
		{
			SU_REQUIRE_OR_FALSE( !m_ddata.IsEmpty() );
			
			rsc_collection c;
			SU_REQUIRE_OR_FALSE( c.load( m_ddata.data ) );

			uint count = 0;
			int co = c.tag_first();
			while( co > 0 )
			{
				if( co == rsc_collection_bitmap_layer::CODE )
				{
					if( count == inIdx )
					{
						c.tag_load( *ioLay );
						return true;
					}
					count++;
				}
				co = c.tag_next();
			}

			return false;
		}


		uint	GetWidth			(						)
		{
			SU_REQUIRE_OR_0( !m_ddata.IsEmpty() );
			return m_ddata.desc.width;
		}

		uint	GetHeight			(						)
		{
			SU_REQUIRE_OR_0( !m_ddata.IsEmpty() );
			return m_ddata.desc.height;
		}

		uint	CountLayer			(						)
		{
			SU_REQUIRE_OR_0( !m_ddata.IsEmpty() );

			rsc_collection c;
			SU_REQUIRE_OR_0( c.load( m_ddata.data ) );

			uint count = 0;
			int co = c.tag_first();
			while( co > 0 )
			{
				if( co == rsc_collection_bitmap_layer::CODE )
				{
					count++;
				}
				co = c.tag_next();
			}

			return count;
		}

		SString	GetLayerName		(	uint	inLayIdx	)
		{
			rsc_collection_bitmap_layer lay;
			SU_REQUIRE_OR_RETURN_V( _GetLayer( &lay, inLayIdx ), SString() );
			SString ss;
			ss.copy( (pcstr)lay.name.text.p, 0, lay.name.len );
			return ss;
		}

		Color32	GetLayerPixel		(	uint	inLayIdx,
										uint	inX,
										uint	inY			)
		{
			rsc_collection_bitmap_layer lay;
			SU_REQUIRE_OR_RETURN_V( _GetLayer( &lay, inLayIdx ), Color32( 0 ) );

			Psm	psm = (Psm)lay.psm.get();
			byte* data = lay.data.p;

			uint w = m_ddata.desc.width;
			uint h = m_ddata.desc.height;

			uint rgba = 0;
			if( ( inX < w ) && ( inY < h ) )
			{
				uint pIdx = inY * w + inX;
				uint byteOff = 0;

				uint bpp = GetBPP( psm );
				if( ( bpp % 8 ) == 0 )
				{
					uint Bpp = bpp/8;
					byteOff = pIdx * Bpp;
				}
				else
				{
					SU_BREAK();		// TODO
				}

				Psm oPsm = PSM_R8_G8_B8_A8;		// r, g, b, a in memory
				
				byte* ib = data + byteOff;
				if( oPsm == psm )
				{
					rgba = *((uint*)ib);
				}
				else
				{
					byte* rgba_b = (byte*)&rgba;
					ConvertPixels( rgba_b, oPsm, ib, psm, 1 );
				}
			}
			
			byte* ret = (byte*)(&rgba);
			return Color32( ret[0], ret[1], ret[2], ret[3] );
		}




		bool	GetLayerLine		(	byte*	outLine,
										uint	inLayIdx,
										uint	inY			)
		{
			rsc_collection_bitmap_layer lay;
			SU_REQUIRE_OR_FALSE( _GetLayer( &lay, inLayIdx ) );

			uint w = m_ddata.desc.width;
			uint h = m_ddata.desc.height;
			SU_REQUIRE_OR_FALSE( inY < h );


			Psm	psm = (Psm)lay.psm.get();
			byte* data = lay.data.p;


			uint byteOff = 0;

			uint bpp = GetBPP( psm );
			if( ( bpp % 8 ) == 0 )
			{
				byteOff = ( inY * w ) * ( bpp >> 3 );
			}
			else
			{
				SU_BREAK();		// TODO acording format
			}

			Psm oPsm = PSM_R8_G8_B8_A8;		// r, g, b, a in memory
			

			byte* ib = data + byteOff;
			if( oPsm == psm )
			{
				util::Memcopy( outLine, ib, ( w * 4 ) );
			}
			else
			{
				ConvertPixels( outLine, oPsm, ib, psm, w );
			}
			return true;
		}

	};


	bool	SIDL_STUB_CLASSNAME::DocData::Create	(	pcstr		inPath,
														Pool*		inPool,
														const void*	inData,
														uint		inBSize	)
	{
		SU_ASSERT( pool == NULL );
		SU_REQUIRE_OR_FALSE( inPool );
		SU_REQUIRE_OR_FALSE( inData );

		data = (byte*)inPool->Alloc( inBSize );
		SU_ASSERT( data );
		SU_REQUIRE_OR_FALSE( data );
		
		path = inPath;
		pool = inPool;
		util::Memcopy( data, inData, inBSize );
		bsize = inBSize;

		rsc_collection c;
		if( c.load( data ) )
		{
			int co = c.tag_first();
			while( co > 0 )
			{
				if( co == rsc_collection_bitmap::CODE )
				{
					c.tag_load( desc );
					return true;
				}
				co = c.tag_next();
			}
		}

		pool->Free( data );
		data  = NULL;
		bsize = 0;
		pool  = NULL;
		path  = "";

		return false;
	}

	void	SIDL_STUB_CLASSNAME::DocData::Release	(						)
	{
		if( !IsEmpty() )
		{
			pool->Free( data );
			data  = NULL;
			bsize = 0;
			pool  = NULL;
			path  = "";
		}
	}

	bool	SIDL_STUB_CLASSNAME::DocData::IsEmpty	(						)
	{
		return( !pool && !data && !bsize );
	}

}


