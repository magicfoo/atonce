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

#ifndef _su_codec_h_
#define _su_codec_h_

namespace su
{
	
#if defined( SU_HW_WINDOWS )
	bool	decompress_pvrtc	(	void*			inOutputBuffer,			// rgba32 in memory, bsize = ( w * h * 4 );
									byte*			inInputBuffer,			// pvrtc without header (raw data)
									uint			inWidth,
									uint			inHeight,
									bool			inAlpha,
									uint			inBitPP				);		// inBitPP = 2 or inBitPP = 4
#endif

	// Lzma decode using buffers
	bool	init_lzma_buffer	(	Pool*			inp,
									byte*			inInputBuffer,
									uint			inInputBSize		);

	int		decode_lzma_buffer	(	byte*			inOutputBuffer,
									uint*			ioOutputBSize,
									byte*			inInputBuffer,
									uint*			ioInputBSize		);

	void	abort_lzma_buffer	(										);
	


	// Lzma decode using files
	bool	init_lzma_file		(	Pool*			inp,
									IFileWriter*	oFile,
									IFileReader*	iFile				);

	int		decode_lzma_file	(										);

	void	abort_lzma_file		(										);

}

#endif // _su_codec_h_