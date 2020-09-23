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


#include <sufw/core/su_core.h>
#include <sufw/su_mapping.h>
#include "su_icodec.h"
#include "su_codec.h"
#include <sufw-build/lzma/C/LzmaDec.h>

// http://www.asawicki.info/news_1368_lzma_sdk_-_how_to_use.html


using namespace su;

#define	HEADER_SIZE		(LZMA_PROPS_SIZE + 8)
#define	IN_BUF_SIZE		(1<<16)					//64K
#define	OUT_BUF_SIZE	(1<<16)

namespace
{
	bool			sz_inuse = false;
	Pool*			sz_pool = NULL;
	
	ISzAlloc		sz_allocator;
	CLzmaDec		state;
	bool			thereIsSize;
	uint64			unpackSize;
	uint64			unpacked;
	uint64			total_unpackSize;

	
	void* sz_alloc( void* p, size_t size )
	{
		p = p;
		SU_ASSERT( sz_pool );
		return size ? sz_pool->Alloc(size) : NULL;
	}

	void sz_free( void* p, void* address )
	{
		p = p;
		SU_ASSERT( sz_pool );
		sz_pool->Free(address);
	}

	bool sz_init ( Pool* inp, byte* inheader )
	{
		SU_ASSERT( inp );
		SU_ASSERT( inheader );
		SU_ASSERT( !sz_inuse );

		LzmaDec_Construct(&state);

		sz_pool = inp->CreateSub( Pool::RM_Retained );
		SU_REQUIRE_OR_FALSE( sz_pool );

		sz_allocator.Alloc = sz_alloc;
		sz_allocator.Free = sz_free;

		SRes res = LzmaDec_Allocate( &state, inheader, LZMA_PROPS_SIZE, &sz_allocator );
		if( res == SZ_OK )
		{
			LzmaDec_Init(&state);
			sz_inuse = true;
			return true;
		}
		else
		{
			// failed for any reason !
			LzmaDec_Free( &state, &sz_allocator );
			Pool::Destroy( sz_pool );
			return false;
		}
	}

	void sz_shut ( )
	{
		SU_ASSERT( sz_inuse );
		LzmaDec_Free(&state, &sz_allocator);

		Pool::Destroy( sz_pool );
		sz_inuse = false;
	}

}



bool	su::init_lzma_buffer(	Pool*		inp,
								byte*		inInputBuffer,
								uint		inInputBSize	)
{
	SU_REQUIRE_OR_FALSE( inp );
	SU_REQUIRE_OR_FALSE( !sz_inuse );
	SU_REQUIRE_OR_FALSE( inInputBSize >= HEADER_SIZE );
	
	unpackSize = 0;
	for( int i=0 ; i<8 ; i++ )
		unpackSize += (UInt64)inInputBuffer[LZMA_PROPS_SIZE + i] << (i * 8);

	if( !sz_init( inp, inInputBuffer ) )
		return false;

	thereIsSize = (unpackSize != (UInt64)(Int64)-1);
	
	unpacked = 0;
	total_unpackSize = unpackSize;

	return true;
}


int		su::decode_lzma_buffer(	byte*		inOutputBuffer,
								uint*		ioOutputBSize,
								byte*		inInputBuffer,
								uint*		ioInputBSize		)
{
	SU_REQUIRE_OR_NEG( sz_inuse );

	size_t	inSize = *ioInputBSize;
	


	SRes res;
	SizeT inProcessed = inSize;
	SizeT outProcessed = *ioOutputBSize;
	ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
	ELzmaStatus status;
	if (thereIsSize && outProcessed > unpackSize)
	{
		outProcessed = (SizeT)unpackSize;
		finishMode = LZMA_FINISH_END;
	}

	res = LzmaDec_DecodeToBuf(&state, inOutputBuffer, &outProcessed,
	inInputBuffer, &inProcessed, finishMode, &status);

	unpackSize -= outProcessed;
	unpacked += outProcessed;

	(*ioOutputBSize) = outProcessed;
	(*ioInputBSize) = inProcessed;

	if (res != SZ_OK )
	{
		sz_shut();
		return -1;
	}

	if( thereIsSize && unpackSize == 0 || status == LZMA_STATUS_FINISHED_WITH_MARK )
	{
		sz_shut();
		return 100;	// decode finished
	}
      
	if (inProcessed == 0 && outProcessed == 0)
	{
		if (thereIsSize || status != LZMA_STATUS_FINISHED_WITH_MARK)
		{
			sz_shut();
			return -1;
		}
	}
	
	int completed = (unpacked*100) / total_unpackSize;
	if( completed >= 100 )
		completed = 99;

	return completed;
}



void su::abort_lzma_buffer ( )
{
	SU_REQUIRE_OR_RETURN( sz_inuse );
	sz_shut();
}






namespace
{	
	Byte			inBuf[IN_BUF_SIZE];
	Byte			outBuf[OUT_BUF_SIZE];
	size_t			inPos = 0;
	size_t			outPos = 0;
	
	IFileWriter*	outFile;
	IFileReader*	inFile;
}



bool su::init_lzma_file( Pool* inp, IFileWriter* oFile, IFileReader* iFile )
{
	SU_REQUIRE_OR_FALSE( oFile );
	SU_REQUIRE_OR_FALSE( iFile );
	SU_REQUIRE_OR_FALSE( inp );
	
	inFile = NULL;
	outFile = NULL;

	// load headers
	unsigned char header[HEADER_SIZE];
	if( iFile->Read(header,sizeof(header)) != sizeof(header) )
		return false;

	if( !init_lzma_buffer( inp, header, sizeof(header) ) )
		return false;
	
	inPos = HEADER_SIZE;
	outPos = 0;

	inFile = iFile;
	inFile->Retain();
	inFile->SetPos( inPos );

	outFile = oFile;
	outFile->Retain();
	outFile->SetPos( outPos );

	return true;
}


int su::decode_lzma_file( )
{
	uint inProcessed = IN_BUF_SIZE;
	uint outProcessed = OUT_BUF_SIZE;

	inFile->Read( inBuf, IN_BUF_SIZE );
      
	int res = decode_lzma_buffer( outBuf, &outProcessed, inBuf, &inProcessed );

	if( res < 0 )
		return res;

	inPos += inProcessed;
	inFile->SetPos( inPos );

	outPos += outProcessed;    
	outFile->Write( outBuf, outProcessed );

	if( res >= 100 || res < 0 )
	{
		SafeRelease( outFile );
		SafeRelease( inFile );
	}

	return res;
}


void su::abort_lzma_file( )
{
	SafeRelease( outFile );
	SafeRelease( inFile );
	abort_lzma_buffer( );
}