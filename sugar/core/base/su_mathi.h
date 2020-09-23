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



#ifndef _su_mathi_h_
#define _su_mathi_h_



//--------------------------
// Math Fct
//--------------------------

namespace su
{

	template < typename T > inline
	T Min ( const T & inA, const T & inB )
	{
		return ( inA < inB ? inA : inB );
	}

	template < typename T > inline
	T Max ( const T & inA, const T & inB )
	{
		return ( inA > inB ? inA : inB );
	}

	template < typename T > inline
	T Clamp( const T & inV, const T & inMin, const T & inMax )
	{
		return Min( Max( inV, inMin ) , inMax );
	}

	template < typename T > inline
	T Abs( const T & inV )
	{
		return (inV < 0) ? -inV : inV;
	}



	template < uint C > inline
	DataMatrix<C>::DataMatrix( int inColCount, int inRawCount	)
	{
		colCount = inColCount;
		rawCount = inRawCount;
	}


	template <uint C> inline
	DataMatrix<C>::DataMatrix( int inColCount, int inRawCount, const byte*	inBA	)
	{
		// SU_ASSERT( inRawCount*inColCount <= Capacity )
		colCount = inColCount;
		rawCount = inRawCount;
		util::Memcopy( byteA, inBA, rawCount*colCount );
	}

	template <uint C> inline
	DataMatrix<C>::DataMatrix ( const DataMatrix<C>& inM )
	{
		DataMatrixCopy( this, &inM );
	}

	template <uint C> inline
	DataMatrix<C>& DataMatrix<C>::operator = ( const DataMatrix<C>& inM )
	{
		DataMatrixCopy( this, &inM );
		return *this;
	}

	template <uint C> inline
	byte& DataMatrix<C>::operator () ( int Col, int Raw )
	{
		return byteA[Raw*colCount + Col];
	}

	template <uint C> inline
	byte DataMatrix<C>::operator () ( int Col, int Raw ) const
	{
		return byteA[Raw*colCount + Col];
	}

	template <uint C> inline
	bool DataMatrix<C>::operator == ( const DataMatrix<C>& inM ) const
	{
		SU_REQUIRE_OR_FALSE( inM.rawCount == rawCount );
		SU_REQUIRE_OR_FALSE( inM.colCount == colCount );
		return util::Memcompare( (pvoid)byteA, (pvoid)(inM.byteA), rawCount*colCount) == 0;
	}

	template <uint C> inline
	bool DataMatrix<C>::operator != ( const DataMatrix<C>& inM ) const
	{
		if( inM.rawCount != rawCount )		return true;
		if( inM.colCount != colCount )		return true;
		return util::Memcompare( (pvoid)byteA, (pvoid)(inM.byteA), rawCount*colCount) != 0;
	}

	template <uint C> inline
	DataMatrix<C>* DataMatrixCopy (	DataMatrix<C>* outM, const DataMatrix<C>* inM )
	{
		util::Memcopy( outM->byteA, inM->byteA, inM->colCount*inM->rawCount );
		outM->colCount = inM->colCount;
		outM->rawCount = inM->rawCount;
		return outM;
	}

	template <uint C> inline
	DataMatrix<C>* DataMatrixRotateLeft ( DataMatrix<C>* outM, const DataMatrix<C>*	inM	)
	{
		DataMatrix<C> tmp(inM->rawCount,inM->colCount);

		int iw = inM->colCount;
		int ih = inM->rawCount;
		int ow = ih;
		int oh = iw;

		for( int y=0; y<oh; y++ )
		{
			for( int x=0; x<ow; x++ )
			{
				int ciO = ( y*ow + x );
				int ciI = ( x*iw + ((iw-1)-y) );

				tmp.byteA[ciO] = inM->byteA[ciI];
			}
		}

		return DataMatrixCopy( outM, &tmp );
	}

	template <uint C> inline
	DataMatrix<C>* DataMatrixRotateRight ( DataMatrix<C>* outM, const DataMatrix<C>* inM )
	{
		DataMatrix<C> tmp(inM->rawCount,inM->colCount);

		int iw = inM->colCount;
		int ih = inM->rawCount;
		int ow = ih;
		int oh = iw;

		for( int y=0; y<oh; y++ )
		{
			for( int x=0; x<ow; x++ )
			{
				int ciO = ( y*ow + x );
				int ciI = ( ((ih-1)-x)*iw + y );

				tmp.byteA[ciO] = inM->byteA[ciI];
			}
		}
		
		return DataMatrixCopy( outM, &tmp );
	}

	template <uint C> inline
	DataMatrix<C>* DataMatrixHorizontalFlip	( DataMatrix<C>* outM, const DataMatrix<C>*	inM	)
	{
		DataMatrix<C> tmp(inM->colCount,inM->rawCount);

		int w = inM->colCount;
		int h = inM->rawCount;

		for( int y=0; y<h; y++ )
		{
			for( int x=0; x<w; x++ )
			{
				int ciO = ( y*w + x );
				int ciI = ( y*w + ((w-1)-x) );

				tmp.byteA[ciO] = inM->byteA[ciI];
			}
		}

		return DataMatrixCopy( outM, &tmp );
	}

	template <uint C> inline
	DataMatrix<C>* DataMatrixVerticalFlip (	DataMatrix<C>* outM, const DataMatrix<C>* inM	)
	{
		DataMatrix<C> tmp(inM->colCount,inM->rawCount);

		int w = inM->colCount;
		int h = inM->rawCount;

		for( int y=0; y<h; y++ )
		{
			for( int x=0; x<w; x++ )
			{
				int ciO = ( y*w + x );
				int ciI = ( ((h-1)-y)*w + x );

				tmp.byteA[ciO] = inM->byteA[ciI];
			}
		}

		return DataMatrixCopy( outM, &tmp );
	}

}




#endif // _su_mathi_h_


