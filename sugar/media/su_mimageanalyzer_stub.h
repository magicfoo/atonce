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



using namespace su;




namespace
{

	void rgb_to_gray (byte* iodata, byte* indata, int width, int height)
	{ 
		float Rfactor= 0.299f;
		float Gfactor= 0.587f;
		float Bfactor= 0.114f;

		for (int i=0;i<height*width;i++)
		{
			byte ir = indata[i*3+0];
			byte ig = indata[i*3+1];
			byte ib = indata[i*3+2];

			iodata[i] = Rfactor*ir + Gfactor*ig + Bfactor*ib;
		}
	}
	/*
	void rgb_to_gray ( byte* ioData, byte* inRGBData, int inWidth, int inHeight )
	{ 
		static const float Rf = 0.299f;
		static const float Gf = 0.587f;
		static const float Bf = 0.114f;

		int ci = 0;
		const int pc = inWidth*inHeight;
		for( int i=0; i<pc; i++ )
		{
			float ir = inRGBData[ci++];
			float ig = inRGBData[ci++];
			float ib = inRGBData[ci++];

			ioData[i] = (byte)( Rf*ir + Gf*ig + Bf*ib );
		}
	}
	*/


	void under_sampling (byte* iodata, byte* inData, int inWidth, int inHeight, int inSamplingFactor )
	{
		int outH = inHeight/inSamplingFactor;
		int outW = inWidth/inSamplingFactor;

		for( int y=0; y<outH; y++ )
		{	
			for( int x=0; x<outW; x++ )
			{
				int cIdx = (inSamplingFactor*y)*inWidth + (inSamplingFactor*x);

				float cp = 0.0f;
				for( int my=0; my<inSamplingFactor; my++ )
				{
					for( int mx=0; mx<inSamplingFactor; mx++ )
					{
						cp += inData[ cIdx	+ inWidth*my + mx];
					} 
				}

				iodata[ y*outW+x ] = (byte)(cp/(inSamplingFactor*inSamplingFactor));
			}
		}
	}
	/*
	void under_sampling ( byte* ioData, byte* inGrayData, int inWidth, int inHeight, int inSamplingFactor )
	{
		const int outH = inHeight / inSamplingFactor;
		const int outW = inWidth  / inSamplingFactor;

		const float oSCount = 1.0f/((float)(inSamplingFactor*inSamplingFactor));

		int ci = 0;
		for( int y=0; y<outH; y++ )
		{	
			for( int x=0; x<outW; x++ )
			{
				int si = (inSamplingFactor*y)*inWidth + (inSamplingFactor*x);
				float cp = 0.0f;
				for( int my=0; my<inSamplingFactor; my++ )
					for( int mx=0; mx<inSamplingFactor; mx++ )
						cp += inGrayData[ si	+ inWidth*my + mx];

				ioData[ci++] = (byte)( cp * oSCount );
			}
		}
	}
	*/


	void adapt_thresholding( byte* iodata, byte* indata, int inWidth, int inHeight )
	{
		float t = 15.0f;

		float m[9] = 
		{
			0.0192f, 0.1001f, 0.0192f,
			0.1001f, 0.5228f, 0.1001f,
			0.0192f, 0.1001f, 0.0192f
		};

		for( int y=1; y<inHeight-1; y++ )
		{
			for( int x=1; x<inWidth-1; x++ )
			{
				int cIdx = y*inWidth+x;
				
				float c0 = indata[ cIdx - inWidth		- 1	]	* m[0];
				float c1 = indata[ cIdx					- 1	]	* m[3];
				float c2 = indata[ cIdx	+ inWidth		- 1	]	* m[6];

				float c3 = indata[ cIdx	- inWidth			]	* m[1];
				float c4 = indata[ cIdx						]	* m[4];
				float c5 = indata[ cIdx	+ inWidth			]	* m[7];
				
				float c6 = indata[ cIdx	- inWidth		+ 1	]	* m[2];
				float c7 = indata[ cIdx					+ 1	]	* m[5];
				float c8 = indata[ cIdx	+ inWidth		+ 1	]	* m[8];

				float cpix = c0 + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8;

				if( indata[cIdx] > cpix-t )
				{
					iodata[cIdx] = 0;
				}
				else
				{
					iodata[cIdx] = 255;
				}
			}
		}
	}


	/*
	void adapt_thresholding ( byte* ioData, byte* inData, int inWidth, int inHeight )
	{
		static const float th = 15.0f;
		static const float m[9] = 
		{
			0.0192f, 0.1001f, 0.0192f,
			0.1001f, 0.5228f, 0.1001f,
			0.0192f, 0.1001f, 0.0192f
		};

		for( int y=1; y<inHeight-1; y++ )
		{
			for( int x=1; x<inWidth-1; x++ )
			{
				int cIdx = y*inWidth+x;
					
				float cPix =	(	inData[ cIdx - inWidth	- 1	]	* m[0]	)	+
								(	inData[ cIdx - inWidth		]	* m[1]	)	+
								(	inData[ cIdx - inWidth	+ 1	]	* m[2]	)	+

								(	inData[ cIdx			- 1	]	* m[3]	)	+
								(	inData[ cIdx				]	* m[4]	)	+
								(	inData[ cIdx			+ 1	]	* m[5]	)	+

								(	inData[ cIdx + inWidth	- 1	]	* m[6]	)	+
								(	inData[ cIdx + inWidth		]	* m[7]	)	+
								(	inData[ cIdx + inWidth	+ 1	]	* m[8]	);

				ioData[cIdx] = ( inData[cIdx] > ( cPix - th ) ) ? 0 : 255;
			}
		}
	}
	*/

	void binary_thresholding ( byte* ioData, byte* inData, int inWidth, int inHeight, byte inThresold )
	{
		const int pc = inWidth * inHeight;
		byte* od = ioData;
		byte* id = inData;
		for( int i=0; i<pc; i++ )
		{
			*od++ = ( *id++ >= inThresold ) ? 255 : 0;
		}
	}

	void blur_5x5 ( byte* ioData, byte* inData, int inWidth, int inHeight )
	{
		static const float ic = 1.0f/25.0f;
		float c;

		const int iw = inWidth;
		const int i2w = 2*iw;

		const int sy = inHeight- 3;
		const int sx = inWidth - 3;

		for( int y=2; y<sy; y++ )
		{
			int cly = y*inWidth;
			for( int x=2; x<sx; x++ )
			{
				int cIdx = cly + x;
				
				byte* ind = inData + cIdx;
				byte* ind_s2 = ind - i2w;
				byte* ind_s1 = ind - iw;
				byte* ind_a1 = ind + iw;
				byte* ind_a2 = ind + i2w;

				c =		*( ind_s2	- 2 ) + 
						*( ind_s1	- 2 ) + 
						*( ind		- 2 ) + 
						*( ind_a1	- 2 ) + 
						*( ind_a2	- 2 );

				c+=		*( ind_s2	- 1 ) + 
						*( ind_s1	- 1 ) + 
						*( ind		- 1 ) + 
						*( ind_a1	- 1 ) + 
						*( ind_a2	- 1 );

				c+=		*( ind_s2		) + 
						*( ind_s1		) + 
						*( ind			) + 
						*( ind_a1		) + 
						*( ind_a2		);

				c+=		*( ind_s2	+ 1 ) + 
						*( ind_s1	+ 1 ) + 
						*( ind		+ 1 ) + 
						*( ind_a1	+ 1 ) + 
						*( ind_a2	+ 1 );

				c+=		*( ind_s2	+ 2 ) + 
						*( ind_s1	+ 2 ) + 
						*( ind		+ 2 ) + 
						*( ind_a1	+ 2 ) + 
						*( ind_a2	+ 2 );


				/*	Function before optimisation :
					for( int i=-2; i<3; i++ )
					{
						if( x-i >= 0 && x+i<inWidth )
						{
							for( int j=-2; j<3; j++ )
							{
								if( y-j >= 0 && y+j<inHeight )
								{
									c += inData[ cIdx	+ inWidth*j + i];
								}
							}
						}
					}
				*/

				ioData[cIdx] = c * ic;
			}
		}
	}

	void subdivise_to_area ( int* outAreaCount, int* ioIdxAreaA, byte* inData, uint inWidth, uint inHeight )
	{
		const int margin = 1;
		int regCount = 0;

		for( uint y=0; y<inHeight; y++ )
		{
			for( uint x=0; x<inWidth; x++ )
			{
				int cIdx = y*inWidth + x;


				if( ( inData[cIdx] == 0 )	||
					( x <= margin )			||
					( x > inWidth-margin )	||
					( y <= margin )			||
					( y > inHeight-margin )	)
				{
					ioIdxAreaA[cIdx] = -1;
					continue;
				}

				SU_ASSERT( inData[cIdx] == 255 );

				int lReg = ioIdxAreaA[cIdx-1];
				int tReg = ioIdxAreaA[cIdx-inWidth];
				bool lEmpty = ( lReg == -1 );
				bool tEmpty = ( tReg == -1 );

				if( lEmpty && tEmpty )
				{
					// new region
					ioIdxAreaA[cIdx] = regCount++;
				}
				else if( !lEmpty && tEmpty )
				{
					// left region
					ioIdxAreaA[cIdx] = lReg;
				}
				else if( lEmpty && !tEmpty )
				{
					// top region
					ioIdxAreaA[cIdx] = tReg;
				}
				else
				{
					SU_ASSERT( inData[cIdx-1] == 255 && inData[cIdx-inWidth] == 255 );
					SU_ASSERT( lReg >= 0 && tReg >= 0 );

					ioIdxAreaA[cIdx] = tReg;		// 
					if( lReg != tReg )
					{
						const int* ed = ioIdxAreaA;
						int* od = ioIdxAreaA + cIdx;

						while( od >= ed )
						{
							if( *od == lReg )
								*od = tReg;
							od--;
						}
					}
				}
			}
		}
		*outAreaCount = regCount;
	}


	int get_dominant_area( int* inIdxAreaA, uint inWidth, uint inHeight, int* ioAreaCountA, int inAreaCount )
	{
		util::Memset( ioAreaCountA, 0, inAreaCount * sizeof(int) );

		const int pc = inWidth*inHeight;
		for( int i=0; i<pc; i++ )
		{
			int cIdx = inIdxAreaA[i];
			if( cIdx != -1 )
			{
				SU_ASSERT( ( cIdx >= 0 ) && ( cIdx < inAreaCount ) );
				ioAreaCountA[cIdx]++;
			}
		}

		int maxRegIdx	= 0;
		int maxCount	= 0;
		for( int i=0; i<inAreaCount; i++ )
		{
			if( ioAreaCountA[i] > maxCount )
			{
				maxCount = ioAreaCountA[i];
				maxRegIdx = i;
			}
		}

		return maxRegIdx;
	}

	Vec4 quadrangle ( int inRegIdx, int* inRegIdxA, uint inWidth, uint inHeight, int inMargin, int inSamplingFactor )
	{	
		int Xmin = inWidth;
		int Xmax = 0;
		int Ymin = inHeight;
		int Ymax = 0;

		int cIdx = 0;
		for( uint y=0; y<inHeight; y++ )
		{
			for( uint x=0; x<inWidth; x++ )
			{
				if( inRegIdxA[cIdx] == inRegIdx )
				{
					if( x > Xmax )		Xmax = x;
					if( x < Xmin )		Xmin = x;
					if( y > Ymax )		Ymax = y;
					if( y < Ymin )		Ymin = y;
				}
				cIdx++;
			}
		}

		int xmin = inSamplingFactor * Xmin - inMargin;
		int xmax = inSamplingFactor * Xmax + inMargin;
		int ymin = inSamplingFactor * Ymin - inMargin;
		int ymax = inSamplingFactor * Ymax + inMargin;

		if( xmin < 0 )								xmin = 0;
		if( xmax >= inWidth * inSamplingFactor )	xmax = inWidth * inSamplingFactor - 1;
		if( ymin < 0 )								ymin = 0;
		if( ymax >= inHeight * inSamplingFactor )	ymax = inHeight * inSamplingFactor - 1;

		SU_ASSERT( xmin < xmax );
		SU_ASSERT( ymin < ymax );

		return Vec4( xmin, ymin, xmax-xmin, ymax-ymin );
	}


	void extract_roi( byte* ioROIData, const Vec4& inROIQuad, byte* inData, uint inWidth, uint inHeight )
	{
		byte* od = ioROIData;
		const int ys = inROIQuad.y;
		const int ye = inROIQuad.y + inROIQuad.w;
		const int xs = inROIQuad.x;
		const int xe = inROIQuad.x + inROIQuad.z;

		for( int y=ys; y<ye; y++ )
		{
			const int cl = y*inWidth;
			for( int x=xs; x<xe; x++ )
			{
				*od++ = inData[cl+x] ;
			}
		}
	}




	/*
	bool find_start ( Vec2* outStart, byte* ioROIData, int inW, int inH, int inY )
	{
		byte* sd = ioROIData + ( inY * inW );
		byte* ed = sd + inW;
		byte* cd = sd;

		while( cd < ed )
		{
			if( *cd )
				break;
			cd++;
		}
		SU_REQUIRE_OR_FALSE( cd < ed );

		while( cd < ed )
		{
			if( !(*cd) )
				break;
			cd++;
		}
		SU_REQUIRE_OR_FALSE( cd < ed );

		int x = cd - sd;
		SU_ASSERT( x < inW );

		*outStart = Vec2( x, inY );
		return true;
	}
	bool find_start	( Vec2* outStart, byte* ioROIData, const Vec4& inROIQuad )
	{
		int w = inROIQuad.z;
		int h = inROIQuad.w;
		int hy  = h/2;

		if( find_start( outStart, ioROIData, w, h, hy ) )	return true;
		if( find_start( outStart, ioROIData, w, h, hy+1 ) )	return true;
		if( find_start( outStart, ioROIData, w, h, hy-1 ) )	return true;

		return false;
	}
	*/
	bool find_start	(	Vec2*			outStart,
						byte*			ioROIData,
						const Vec4&		inROIQuad		)
	{
		int w = inROIQuad.z;
		int h = inROIQuad.w;

		int cx = 0;
		int y  = h/2;

		int sIdx = y * w;
		
		while( ioROIData[sIdx+cx] == 0 )
		{
			cx++;
			if( cx >= w )
			{
				return false;
			}
		}

		while( ioROIData[sIdx+cx] != 0 )
		{
			cx++;
			if( cx >= w )
			{
			//	SU_ASSERT( false );
				return false;
			}
		}

		*outStart = Vec2( cx, y );
		return true;
	}



	bool edge_track ( int* ioIdxA, int* outIdxCount, int inIdxMax, byte* inROIData, const Vec4& inROIQuad, const Vec2& inStartPoint )
	{
		const int w = inROIQuad.z;
		const int h = inROIQuad.w;

		const int sIdx = inStartPoint.y * w + inStartPoint.x;
		int cIdx = sIdx;

		SU_ASSERT( inROIData[sIdx] == 0 );

		int R[8];
		int T[8];
		byte M8[32];
		for( uint i=0; i<32; i++ )
		{
			M8[i] = i%8;
		}

		int IR[8];
		int idxCount = 0;
		int j = 0;

		while( true )
		{
			T[1] = cIdx - w - 1;	T[2] = cIdx - w;	T[3] = cIdx - w + 1;
			T[0] = cIdx		- 1;						T[4] = cIdx		+ 1;
			T[7] = cIdx + w - 1;	T[6] = cIdx + w;	T[5] = cIdx + w + 1;

			if( T[1] < 0 || T[5] >= (w*h) )
			{
				return false;
			}

			for( int i=0; i<8; i++ )
			{
				R[i]  = T[M8[j+i]];
				IR[i] = inROIData[R[i]];
			}
			int r = 7;
			
		//	if		( IR[0] < IR[7] )		r = 0;
			if		( IR[1] < IR[0] )		r = 1;
			else if	( IR[2] < IR[1] )		r = 2;
			else if	( IR[3] < IR[2] )		r = 3;
			else if	( IR[4] < IR[3] )		r = 4;
			else if	( IR[5] < IR[4] )		r = 5;
			else if	( IR[6] < IR[5] )		r = 6;
			else if	( IR[7] < IR[6] )		r = 7;
			
			
			/*
			{
				T[1] = R[r] - w - 1;	T[2] = R[r] - w;	T[3] = R[r] - w + 1;
				T[0] = R[r]		- 1;						T[4] = R[r]		+ 1;
				T[7] = R[r] + w - 1;	T[6] = R[r] + w;	T[5] = R[r] + w + 1;
				
				int cp = 0;
				for( int ii=0; ii<8; ii++ )	cp += inROIData[T[ii]];
				
				if( cp == 7*255 )
				{
					inROIData[R[r]] = 255;
					continue;
				}
			}
			*/

			ioIdxA[idxCount++] = R[r];
			
			if( idxCount >= inIdxMax )
			{
				return false;
			}

			cIdx = R[r];
			j = M8[j+r+4];

			if( cIdx == sIdx )
			{
				*outIdxCount = idxCount;
				return true;
			}
		}
	}



	bool get_corner_index ( int* outIdxCornersA, int* inEdgeIdxA, int inEdgeIdxCount, const Vec4& inROIQuad )
	{
		const int w = inROIQuad.z;
		const int h = inROIQuad.w;

		int x0 = inEdgeIdxA[0] % w;
		float max = 0.0f;

		int	 corn1_ind = -1;
		Vec2 corn1_coord;


		Vec2 startPos( x0, ( inEdgeIdxA[0] - x0 ) / w );

		for( int i=0; i<inEdgeIdxCount; i++ )
		{
			int ci = inEdgeIdxA[i];
			int x = ci%w;
			int y = (ci-x)/w;

			Vec2 cPose( x, y );
			Vec2 dir = cPose - startPos;
			float L = Vec2Norm2( &dir );
			
			if( L > max )
			{
				max = L;
				corn1_ind = i;
				corn1_coord = cPose;
			}
		}
		SU_REQUIRE_OR_FALSE( corn1_ind != -1 );
	
		
		Vec2 imgCenter( w/2, h/2 );
		Vec3 C1toCenter = Vec3(imgCenter) - Vec3(corn1_coord);
		float NormC1toCenter = Vec3Norm2(&C1toCenter);

		float a = (corn1_coord.y -  imgCenter.y)/( corn1_coord.x -  imgCenter.x);
		int b = imgCenter.y - a* imgCenter.x;
		

		float max2 = 0.0f;
		int		corn2_ind = -1;
		Vec2	corn2_coord; 

		float max3 = 0.0f;
		int		corn3_ind = -1;
		Vec2	corn3_coord; 


		for( int i=0; i<inEdgeIdxCount; i++ )
		{
			int ci = inEdgeIdxA[i];
			int x = ci%w;
			int y = (ci-x)/w;

			Vec3 cdir = Vec3(x,y,0) -  Vec3(corn1_coord);

			Vec3 cross;
								
			Vec3Cross(&cross,&C1toCenter,&cdir);

			float L = Vec3Norm2(&cross)/NormC1toCenter;

			if( y < a*x + b )
			{
				if( L > max3 )
				{
					max3 = L;
					corn3_ind = i;
					corn3_coord = Vec2(x,y);
				}
			}
			else 
			{
				if( L > max2 )
				{
					max2 = L;
					corn2_ind = i;
					corn2_coord = Vec2(x,y);
				}
			}
		}

	//	SU_ASSERT( corn2_ind != -1 );
	//	SU_ASSERT( corn3_ind != -1 );
		SU_REQUIRE_OR_FALSE( corn2_ind != -1 );
		SU_REQUIRE_OR_FALSE( corn3_ind != -1 );


		float m = ( corn3_coord.y -  corn2_coord.y )/( corn3_coord.x -  corn2_coord.x );
		int c = corn3_coord.y - m* corn3_coord.x; 
		Vec3 C2toC3 = Vec3(corn3_coord) - Vec3(corn2_coord);
		float NormC2toC3 = Vec3Norm2(&C2toC3);



		bool isUnder = ( corn1_coord.y < m*corn1_coord.x +c );
		

		float max4 = 0.0f;
		int		corn4_ind = -1;
		Vec2	corn4_coord; 
		

		for (int i=0; i<inEdgeIdxCount;i++)
		{
			int ci = inEdgeIdxA[i];
			int x = ci%w;
			int y = (ci-x)/w;

			bool CurrIsUnder = (y < m*x + c);
			
			if( ( !isUnder && CurrIsUnder ) || ( isUnder && !CurrIsUnder )  )
			{
				Vec3 cdir = Vec3(x,y,0) -  Vec3(corn2_coord);
				Vec3 cross;				
				Vec3Cross(&cross,&C2toC3,&cdir);

				float L = Vec3Norm2(&cross)/NormC2toC3;

				if (L > max4)
				{
					max4= L;
					corn4_ind = i;
					corn4_coord = Vec2(x,y);
				}
			}
		}

	//	SU_ASSERT( corn4_ind != -1 );
		SU_REQUIRE_OR_FALSE( corn4_ind != -1 );

		outIdxCornersA[0] = corn1_ind;
		outIdxCornersA[1] = corn2_ind;  
		outIdxCornersA[2] = corn3_ind;  
		outIdxCornersA[3] = corn4_ind;  


		int nb_loops = 4 - 1;
		bool finish = false;

		while( nb_loops > 0 && !finish )
		{
			finish = true;
			for( int i=0; i<3; i++ )
			{
				if( outIdxCornersA[i] > outIdxCornersA[i+1] )
				{
					int temp = outIdxCornersA[i];
					outIdxCornersA[i] = outIdxCornersA[i+1];
					outIdxCornersA[i+1] = temp;
					finish = false;
				}
			}
			
			nb_loops--;
		}

		return true;
	}



	void get_least_square ( float* outM, float* outC, int* outA, Vec2* inVtxA, int inVtxCount )
	{
		Vec2 average(0.0f,0.0f);
		for (int i=0;i<inVtxCount;i++)
		{
			average += inVtxA[i];
		}
		average /= inVtxCount;
		

		float mN = 0.0f;
		float mD = 0.0f;

		for (int i=0;i<inVtxCount;i++)
		{
			mN += ((inVtxA[i].x - average.x)*( inVtxA[i].y - average.y ));
			mD += (inVtxA[i].x - average.x)*(inVtxA[i].x - average.x);
		}

		if( mD == 0.0f )
		{
			*outA = 0;
			*outM = -1;
			*outC = average.x;
		}
		else
		{
			*outM = mN/mD;
			*outC = average.y - (*outM)*(average.x);
			*outA = 1;
		}
	}

	struct LineEquation
	{
		float	m, c;
		int		a;
	};

	bool get_intersection	(	Vec2*					outInter,
								const LineEquation&		inLE0,
								const LineEquation&		inLE1	)
	{
		float d = inLE0.a*inLE1.m - inLE1.a*inLE0.m;
		if( d != 0.0f )
		{
			float x = ( inLE1.a*inLE0.c - inLE0.a*inLE1.c) / d;
			float y = ( inLE0.c*inLE1.m - inLE1.c*inLE0.m) / d;
			*outInter = Vec2( x, y );
			return true;
		}
		return false;
	}

	bool get_corner_coordinate( Vec2* outCornerCoordA, int* inCornersIdxA, int* inIdxA, int inIdxCount, const Vec4& inROIQuad, Vec2* inCoordBuffer )
	{
		SU_REQUIRE_OR_FALSE( inIdxCount );

		int imgW = inROIQuad.z;
		int imgH = inROIQuad.w;

		static const Vec2 maskA[4] = 
		{
			Vec2( 0.0f,-1.0f ),
			Vec2( 1.0f, 0.0f ),
			Vec2( 0.0f, 1.0f ),
			Vec2(-1.0f, 0.0f )
		};

		LineEquation leA[4];
		for( int i=0; i<4; i++ )
		{
			int idxStart = inCornersIdxA[i];
			int idxCount;

			if( i == 3 )	idxCount = ( inIdxCount - idxStart ) + inCornersIdxA[0];
			else			idxCount = inCornersIdxA[i+1] - idxStart;

			int vtxCount = 0;
			for( int j=0; j<idxCount; j++ )
			{
				int cIdx = (idxStart+j) % inIdxCount;

				int imgIdx = inIdxA[cIdx];

				int x = (imgIdx)%imgW;
				Vec2 cp( x, (imgIdx-x)/imgW );

				inCoordBuffer[vtxCount++] = cp;
				inCoordBuffer[vtxCount++] = cp + maskA[i];
			}
			get_least_square( &(leA[i].m), &(leA[i].c), &(leA[i].a), inCoordBuffer, vtxCount );
		}

		bool b0 = get_intersection( &(outCornerCoordA[0]), leA[3], leA[0] );
		bool b1 = get_intersection( &(outCornerCoordA[1]), leA[0], leA[1] );
		bool b2 = get_intersection( &(outCornerCoordA[2]), leA[1], leA[2] );
		bool b3 = get_intersection( &(outCornerCoordA[3]), leA[2], leA[3] );

		return ( b0 && b1 && b2 && b3 );
	}

	void roi_to_image ( Vec2* ioCornerCoordA, const Vec4& inROIQuad )
	{
		Vec2 offs = Vec2( inROIQuad.x, inROIQuad.y );
		ioCornerCoordA[0] += offs;
		ioCornerCoordA[1] += offs;
		ioCornerCoordA[2] += offs;
		ioCornerCoordA[3] += offs;
	}

	
	template<class T>
	inline const T SIGN(const T &a, const T &b)
	{
		return ( b >= 0 ) ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);
	}

	void	Normalize2DVtx4	(	float&			outScale,
								Vec2&			outCenter,
								Vec3*			outVtxA,
								const Vec2*		inVtxA	)
	{
		Vec2 av( 0.0f, 0.0f );
		for( uint i=0; i<4; i++ )
		{
			av += inVtxA[i];
		}
		av /= 4.0f;

		Vec2 npA[4];
		float avD = 0.0f;
		for( uint i=0; i<4; i++ )
		{
			npA[i] = inVtxA[i] - av;
			avD += Vec2Norm( &npA[i] );
		}
		avD /= 4.0f;

		static const float r2 = Sqrtf( 2.0f );
		float scale = r2 / avD;

		for( uint i=0; i<4; i++ )
		{
			outVtxA[i] = Vec3( npA[i] * scale );
			outVtxA[i].z = 1.0f;
		}

		outScale	= scale;
		outCenter	= av;
	}




	float pythag( float inA, float inB )
	{
		float absa = Abs(inA);
		float absb = Abs(inB);

		if( absa > absb )
		{
			float d0 = absb/absa;
			return absa*Sqrtf( 1.0+d0*d0 );
		}

		if( absb == 0.0f )
		{
			return 0.0f;
		}

		float d1 = absa/absb;
		return absb*Sqrtf( 1.0f+d1*d1 );
	}

	bool SVD_decompose ( float ioV[9][9], float* ioW, float inA[12][9] )
	{
		bool flag;
		int i, its, j, jj, k, l, nm;
		float anorm, c, f, g, h, s, scale, x, y, z;

	//	int m=a.nrows();
	//	int n=a.ncols();
	//	Vec_DP rv1(n);

		int m = 12;
		int n = 9;

		float rv1[9];

		g = scale = anorm = 0.0f;

		for( i=0;i<n;i++ )
		{
			l=i+2;
			rv1[i]=scale*g;
			g=s=scale=0.0;
			if( i < m )
			{
				for (k=i;k<m;k++)
					scale += Abs( inA[k][i] );

				if (scale != 0.0f)
				{
					for (k=i;k<m;k++)
					{
						inA[k][i] /= scale;
						s += inA[k][i]*inA[k][i];
					}
					f=inA[i][i];
					g = -SIGN(Sqrtf(s),f);
					h=f*g-s;
					inA[i][i]=f-g;
					for (j=l-1;j<n;j++)
					{
						for (s=0.0,k=i;k<m;k++)
							s += inA[k][i]*inA[k][j];

						f=s/h;

						for (k=i;k<m;k++)
							inA[k][j] += f*inA[k][i];
					}

					for (k=i;k<m;k++)
						inA[k][i] *= scale;
				}
			}
			ioW[i] = scale * g;
			g = s = scale = 0.0f;
			if (i+1 <= m && i+1 != n)
			{
				for( k=l-1; k<n; k++ )
				{
					scale += Abs( inA[i][k] );
				}

				if( scale != 0.0f )
				{
					for (k=l-1;k<n;k++)
					{
						inA[i][k] /= scale;
						s += inA[i][k]*inA[i][k];
					}

					f = inA[i][l-1];
					g = -SIGN( Sqrtf(s), f );
					h = ( f * g ) - s;

					inA[i][l-1] = f - g;

					for( k=l-1; k<n; k++ )
					{
						rv1[k] = inA[i][k] / h;
					}

					for( j=l-1; j<m; j++ )
					{
						for( s=0.0,k=l-1; k<n; k++ )	s			+= inA[j][k]*inA[i][k];
						for( k=l-1; k<n; k++ )			inA[j][k]	+= s*rv1[k];
					}

					for (k=l-1;k<n;k++)
						inA[i][k] *= scale;
				}
			}
			anorm = Max( anorm, ( Abs(ioW[i]) + Abs(rv1[i]) ) );
		}
		for( i=n-1; i>=0; i-- )
		{
			if (i < n-1)
			{
				if (g != 0.0)
				{
					for (j=l;j<n;j++)
						ioV[j][i]=(inA[i][j]/inA[i][l])/g;

					for (j=l;j<n;j++)
					{
						for (s=0.0,k=l;k<n;k++)		s			+= inA[i][k]*ioV[k][j];
						for (k=l;k<n;k++)			ioV[k][j]	+= s*ioV[k][i];
					}
				}
				for( j=l; j<n; j++ )
				{
					ioV[i][j]=ioV[j][i] = 0.0f;
				}
			}
			ioV[i][i] = 1.0f;
			g = rv1[i];
			l = i;
		}
		for( i=Min(m,n)-1; i>=0; i-- )
		{
			l = i+1;
			g = ioW[i];
			for( j=l; j<n; j++ )
				inA[i][j] = 0.0f;

			if( g != 0.0f )
			{
				g = 1.0f/g;
				for( j=l; j<n; j++ )
				{
					for( s=0.0,k=l; k<m; k++ )
						s += inA[k][i]*inA[k][j];

					f=(s/inA[i][i])*g;

					for (k=i;k<m;k++)
						inA[k][j] += f*inA[k][i];
				}
				for( j=i; j<m; j++ )
					inA[j][i] *= g;

			}
			else
			{
				for( j=i; j<m; j++ )
					inA[j][i]=0.0f;
			}
			++inA[i][i];
		}
		for( k=n-1; k>=0; k-- )
		{
			for (its=0;its<30;its++)
			{
				flag=true;
				for( l=k; l>=0; l-- )
				{
					nm=l-1;
					if( Abs(rv1[l])+anorm == anorm )
					{
						flag=false;
						break;
					}
					if( Abs(ioW[nm])+anorm == anorm )
					{
						break;
					}
				}
				if (flag)
				{
					c=0.0;
					s=1.0;
					for (i=l;i<k+1;i++)
					{
						f=s*rv1[i];
						rv1[i]=c*rv1[i];
						if (Abs(f)+anorm == anorm)
						{
							break;
						}

						g=ioW[i];
						h=pythag(f,g);
						ioW[i]=h;
						h=1.0/h;
						c=g*h;
						s = -f*h;
						for (j=0;j<m;j++)
						{
							y = inA[j][nm];
							z = inA[j][i];

							inA[j][nm] = y*c+z*s;
							inA[j][i]  = z*c-y*s;
						}
					}
				}
				z = ioW[k];
				if (l == k)
				{
					if (z < 0.0)
					{
						ioW[k] = -z;
						for (j=0;j<n;j++)
							ioV[j][k] = -ioV[j][k];
					}
					break;
				}

				if (its == 29)
					return false;

				x =	ioW[l];
				nm=	k-1;
				y =	ioW[nm];
				g =	rv1[nm];
				h =	rv1[k];
				f =	((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
				g =	pythag(f,1.0);
				f =	((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
				c = s = 1.0f;

				for( j=l; j<=nm; j++ )
				{
					i = j+1;
					g = rv1[i];
					y = ioW[i];
					h = s*g;
					g = c*g;
					z = pythag(f,h);

					rv1[j] = z;

					c = f/z;
					s = h/z;
					f = x*c + g*s;
					g = g*c - x*s;
					h = y*s;
					y *= c;

					for( jj=0; jj<n; jj++ )
					{
						x = ioV[jj][j];
						z = ioV[jj][i];

						ioV[jj][j] = x*c + z*s;
						ioV[jj][i] = z*c - x*s;
					}
					z = pythag( f, h );
					ioW[j] = z;
					if( z )
					{
						z = 1.0f/z;
						c = f*z;
						s = h*z;
					}
					f = c*g + s*y;
					x = c*y - s*g;
					for( jj=0; jj<m; jj++ )
					{
						y = inA[jj][j];
						z = inA[jj][i];
						inA[jj][j] = y*c + z*s;
						inA[jj][i] = z*c - y*s;
					}
				}
				rv1[l] = 0.0f;
				rv1[k] = f;
				ioW[k] = x;
			}
		}
		return true;
	}


	void SVD_reorder ( float ioV[9][9], float* ioW, float ioA[12][9] )
	{
		const int m=12;
		const int n=9;
		int i,j,k,inc=1;
		float sw;
		float su[m], sv[n];
		do { inc *= 3; inc++; } while (inc <= n);

		do
		{
			inc /= 3;
			for (i=inc;i<n;i++)
			{
				sw = ioW[i];
				for (k=0;k<m;k++) su[k] = ioA[k][i];
				for (k=0;k<n;k++) sv[k] = ioV[k][i];
				j = i;
				while (ioW[j-inc] < sw)
				{
					ioW[j] = ioW[j-inc];
					for (k=0;k<m;k++) ioA[k][j] = ioA[k][j-inc];
					for (k=0;k<n;k++) ioV[k][j] = ioV[k][j-inc];
					j -= inc;
					if (j < inc) break;
				}
				ioW[j] = sw;
				for (k=0;k<m;k++) ioA[k][j] = su[k];
				for (k=0;k<n;k++) ioV[k][j] = sv[k];
			}
		} while (inc > 1);

		/*
		for (k=0;k<n;k++)
		{
			s=0;
			for (i=0;i<m;i++) if (ioA[i][k] < 0.0f) s++;
			for (j=0;j<n;j++) if (ioV[j][k] < 0.0f) s++;
			if (s > (m+n)/2)
			{
				for (i=0;i<m;i++) ioA[i][k] = -ioA[i][k];
				for (j=0;j<n;j++) ioV[j][k] = -ioV[j][k];
			}
		}
		*/

		for( k=0; k<n; k++ )
		{
			ioV[k][3] = -ioV[k][3];
			ioV[k][6] = -ioV[k][6];
			ioV[k][8] = -ioV[k][8];
		}
	}

	bool compute_homography ( Matrix* outHomography, Vec2 ioCornerCoordA[4] )
	{
		float mi = -1.0f;
		float ma =  1.0f;
		Vec2 x1VtxA[4] = 
		{
			Vec2( mi, mi ),
			Vec2( mi, ma ),
			Vec2( ma, ma ),
			Vec2( ma, mi ),
		};

		Vec3 x1[4];
		Vec3 x2[4];

		float sc1, sc2;
		Vec2 av1, av2;
		Normalize2DVtx4( sc1, av1, x1, x1VtxA );
		Normalize2DVtx4( sc2, av2, x2, ioCornerCoordA );

		float A[12][9];

		for( uint i=0; i<4;i++ )
		{
			uint c0 = i*3+0;
			uint c1 = i*3+1;
			uint c2 = i*3+2;
			
		// L1
			A[c0][0] = 0.0f;				A[c0][1] = 0.0f;				A[c0][2] = 0.0f;
			A[c0][3] =-x1[i].x;				A[c0][4] =-x1[i].y;				A[c0][5] =-x1[i].z;
			A[c0][6] = x2[i].y * x1[i].x;	A[c0][7] = x2[i].y * x1[i].y;	A[c0][8] = x2[i].y * x1[i].z;

		// L2
			A[c1][0] = x1[i].x;				A[c1][1] = x1[i].y;				A[c1][2] = x1[i].z;
			A[c1][3] = 0.0f;				A[c1][4] = 0.0f;				A[c1][5] = 0.0f;
			A[c1][6] =-x2[i].x * x1[i].x;	A[c1][7] =-x2[i].x * x1[i].y;	A[c1][8] =-x2[i].x * x1[i].z;

		// L3
			A[c2][0] =-x2[i].y * x1[i].x;	A[c2][1] =-x2[i].y * x1[i].y;	A[c2][2] =-x2[i].y * x1[i].z;
			A[c2][3] = x2[i].x * x1[i].x;	A[c2][4] = x2[i].x * x1[i].y;	A[c2][5] = x2[i].x * x1[i].z;
			A[c2][6] = 0.0f;				A[c2][7] = 0.0f;				A[c2][8] = 0.0f;
		}

		float V[9][9];
		float W[9];

		SVD_decompose( V, W, A );
		SVD_reorder( V, W, A );

		float hom[16] =
		{
			V[0][8],	V[1][8],	V[2][8],	0.0f,
			V[3][8],	V[4][8],	V[5][8],	0.0f,
			V[6][8],	V[7][8],	V[8][8],	0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
		};
		float t1[16] = 
		{
			sc1,	0.0f,	-av1.x*sc1,	0.0f,
			0.0f,	sc1,	-av1.y*sc1,	0.0f,
			0.0f,	0.0f,	 1.0f,		0.0f,
			0.0f,	0.0f,	 0.0f,		1.0f
		};
		float t2[16] = 
		{
			sc2,	0.0f,	-av2.x*sc2,	0.0f,
			0.0f,	sc2,	-av2.y*sc2,	0.0f,
			0.0f,	0.0f,	 1.0f,		0.0f,
			0.0f,	0.0f,	 0.0f,		1.0f
		};
		
		Matrix matHom( hom );
		Matrix matT1( t1 );
		Matrix matT2( t2 );

		Matrix ivT2;
		MatrixInverse( &ivT2, NULL, &matT2 );

		*outHomography = (ivT2 * matHom) * matT1;
		MatrixTranspose( outHomography, outHomography );

		return true;
	}


	void compute_matrix ( Matrix* outViewMatrix, Matrix* outProjMatrix, const Matrix& inHomography, float inPixelFocalLength, float inWidth, float inHeight )
	{
		Matrix intermHomography = inHomography/inHomography.m33;
		
		Vec3 h1( intermHomography.m11, intermHomography.m12, intermHomography.m13 );
		Vec3 h2( intermHomography.m21, intermHomography.m22, intermHomography.m23 );
		Vec3 h3( intermHomography.m31, intermHomography.m32, intermHomography.m33 );

		float Intrinsics[16] = 
		{
			inPixelFocalLength,	0.0f,				0.0f,	0.0f,
			0.0f,				inPixelFocalLength,	0.0f,	0.0f,
			inWidth*0.5f,		inHeight*0.5f,		1.0f,   0.0f,
			0.0f,				0.0f,				0.0f,	1.0f
		};
		Matrix matIntrinsics(Intrinsics), invIntrinsics;
		MatrixInverse( &invIntrinsics, NULL, &matIntrinsics );

		Vec3 R1, R2, R3, T;
		Vec3Apply( &R1, &h1, &invIntrinsics );
		Vec3Apply( &R2, &h2, &invIntrinsics );
		Vec3Apply( &T,  &h3, &invIntrinsics );
		
		float sca = 1.0f/Vec3Norm( &R1 );			
		R1 *= sca;
		R2 *= sca;
		Vec3Cross( &R3, &R1, &R2 );
		T *= sca;
		
		/*
		if( outViewMatrix )
		{
			float nView[16] = 
			{
				R2.x,	R2.y,	-R2.z,	0.0f,
				R3.x,	R3.y,	-R3.z,	0.0f,
				R1.x,	R1.y,	-R1.z,	0.0f,
				T.x,	T.y,	-T.z,	1.0f
			};
			*outViewMatrix = Matrix( nView );
		}
		if( outProjMatrix )
		{
			float near = 0.01f;
			float far = 100.0f;
			
			outProjMatrix->m11 = inPixelFocalLength*2.0f / inWidth;
			outProjMatrix->m12 = 0.0f;
			outProjMatrix->m13 = 0.0f;
			outProjMatrix->m14 = 0.0f;
			
			outProjMatrix->m21 = 0.0f;
			outProjMatrix->m22 = -inPixelFocalLength*2.0f / inHeight;
			outProjMatrix->m23 = 0.0f;
			outProjMatrix->m24 = 0.0f;
			
			outProjMatrix->m31 = 0.0f;
			outProjMatrix->m32 = 0.0f;
			outProjMatrix->m33 = -(near+far)/(far-near);
			outProjMatrix->m34 = -1.0f;
			
			outProjMatrix->m41 = 0.0f;
			outProjMatrix->m42 = 0.0f;
			outProjMatrix->m43 = -2.0*(near*far)/(far-near);
			outProjMatrix->m44 = 0.0f;
		}
		*/

		if( outViewMatrix )
		{
			float nView[16] = 
			{
				R2.x,	R2.y,	R2.z,	0.0f,
				R3.x,	R3.y,	R3.z,	0.0f,
				-R1.x,	-R1.y,	-R1.z,	0.0f,
				T.x,	T.y,	T.z,	1.0f
			};
			*outViewMatrix = Matrix( nView );
		}
		if( outProjMatrix )
		{
			float near = 0.01f;
			float far = 100.0f;
			

			float p = far / ( far - near );
			float q = near * p;

			outProjMatrix->m11 = inPixelFocalLength*2.0f / inWidth;
			outProjMatrix->m12 = 0.0f;
			outProjMatrix->m13 = 0.0f;
			outProjMatrix->m14 = 0.0f;
			
			outProjMatrix->m21 = 0.0f;
			outProjMatrix->m22 = -inPixelFocalLength*2.0f / inHeight;
			outProjMatrix->m23 = 0.0f;
			outProjMatrix->m24 = 0.0f;
			
			outProjMatrix->m31 = 0.0f;
			outProjMatrix->m32 = 0.0f;
			outProjMatrix->m33 = (2.0f*p-1.0f);
			outProjMatrix->m34 = 1.0f;
			
			outProjMatrix->m41 = 0.0f;
			outProjMatrix->m42 = 0.0f;
			outProjMatrix->m43 = -2.0f*q;
			outProjMatrix->m44 = 0.0f;
		}
	}




	struct SIDL_STUB_CLASSNAME
	{
		Pool* m_pool;

		struct Context
		{
			uint	pixCount;
			uint	samplFactor;

			byte*	grayData;			// w * h
			byte*	sampledGrayData;	// sw * sh
			byte*	binarySampledData;	// sw * sh
			int*	idxAreaA;			// sw * sh

			byte*	roiData;			// < w * h

			int*	areaCountBuffer;
			uint	areaCountCurCount;

			int*	edgeIdxA;
			uint	edgeIdxCount;
			Vec2*	coordBuffer;		// edgeIdxCount * 2

				

			void	Setup				(	uint	inWidth,
											uint	inHeight,
											Pool*	inPool		)
			{
				uint inPC = inWidth * inHeight;

				if( inPC > pixCount )
				{
					if( grayData )			inPool->Free( grayData );
					if( sampledGrayData )	inPool->Free( sampledGrayData );
					
					if( binarySampledData )	inPool->Free( binarySampledData );
					if( idxAreaA )			inPool->Free( idxAreaA );
					if( roiData )			inPool->Free( roiData );
					

					pixCount = inPC;

					grayData			= inPool->Array<byte>( pixCount );
					sampledGrayData		= inPool->Array<byte>( pixCount );
					binarySampledData	= inPool->Array<byte>( pixCount );
					idxAreaA			= inPool->Array<int>( pixCount );
					roiData				= inPool->Array<byte>( pixCount );
				}
			}

			void	SetupAreaCount		(	uint	inCount,
											Pool*	inPool		)
			{
				if( inCount > areaCountCurCount )
				{
					if( areaCountBuffer )
						inPool->Free( areaCountBuffer );

					areaCountCurCount	= inCount;
					areaCountBuffer		= inPool->Array<int>( areaCountCurCount );
				}
			}
			
			void	SetupEdgeIdxCount	(	uint	inCount,
											Pool*	inPool		)
			{
				if( inCount > edgeIdxCount )
				{
					if( edgeIdxA )		inPool->Free( edgeIdxA );
					if( coordBuffer )	inPool->Free( coordBuffer );

					edgeIdxCount	= inCount;
					edgeIdxA		= inPool->Array<int>( edgeIdxCount );
					coordBuffer		= inPool->Array<Vec2>( edgeIdxCount * 2 );
				}
			}

			void	Free				(	Pool*	inPool		)
			{
				if( grayData )			inPool->Free( grayData );
				if( sampledGrayData )	inPool->Free( sampledGrayData );
				if( binarySampledData )	inPool->Free( binarySampledData );
				if( idxAreaA )			inPool->Free( idxAreaA );
				if( roiData )			inPool->Free( roiData );
				if( areaCountBuffer )	inPool->Free( areaCountBuffer );
				if( edgeIdxA )			inPool->Free( edgeIdxA );
				if( coordBuffer )		inPool->Free( coordBuffer );
				
				util::Zero( (*this) );
			}
		};

		Context m_context;

		uint	blurPassCount;



		bool Init ( Pool* p, AnyList* o )
		{
			m_pool = p;
			SU_ASSERT( m_pool );

			util::Zero( m_context );
			
			m_context.samplFactor = 2;
			blurPassCount = 3;


			return true;
		}


		void Shut ( )
		{
			m_context.Free( m_pool );
		}


		bool Analyze ( Matrix* outView, Matrix* outProj, byte* inRGBData, uint inWidth, uint inHeight, uint inFocal )
		{
			m_context.Setup( inWidth, inHeight, m_pool );


			int samplW = inWidth  / m_context.samplFactor;
			int samplH = inHeight / m_context.samplFactor;

			SU_ASSERT( m_context.grayData );
			SU_ASSERT( m_context.sampledGrayData );
			SU_ASSERT( m_context.binarySampledData );

			/*
			struct DeltaTime
			{
				float togray;
				float sampling;
				float adapt_threshold;
				float blur;
				float binTh;
				float subArea;
				float domiArea;

				float quadr;
				float extractroi;
				float binTh2;
				float findStart;
				float edgeTrack;
				float cornIdx;
				float cornCoord;
				float hogr;
				float matrix;
			};
			DeltaTime dt;
			*/
		//	Time t0, t1;
		//	su::clock::GetTime( &t0 );


			rgb_to_gray			( m_context.grayData, inRGBData, inWidth, inHeight );
		//	su::clock::GetTime( &t1 );
		//	dt.togray = t1-t0;

			under_sampling		( m_context.sampledGrayData, m_context.grayData, inWidth, inHeight, m_context.samplFactor );
		//	su::clock::GetTime( &t0 );
		//	dt.sampling = t0-t1;

			adapt_thresholding	( m_context.binarySampledData, m_context.sampledGrayData, samplW, samplH );
		//	su::clock::GetTime( &t1 );
		//	dt.adapt_threshold = t1-t0;

			for( uint i=0; i<blurPassCount; i++ )
			{
				blur_5x5( m_context.binarySampledData, m_context.binarySampledData, samplW, samplH );
			}
		//	su::clock::GetTime( &t0 );
		//	dt.blur = t0-t1;

			binary_thresholding( m_context.binarySampledData, m_context.binarySampledData, samplW, samplH, 26/*0.1f*255*/ );

			/*
			for( uint y=0; y<inHeight; y++ )
			{
				for( uint x=0; x<inWidth; x++ )
				{
					byte c = 255;
					if( y<samplH && x<samplW )
					{
						uint cps = y*samplW+x;
						c = m_context.binarySampledData[cps];
					}

					uint cp = y*inWidth+x;
					inRGBData[cp*3+0] = c;
					inRGBData[cp*3+1] = c;
					inRGBData[cp*3+2] = c;
				}
			}
			return true;
			*/

		//	su::clock::GetTime( &t1 );
		//	dt.binTh = t1-t0;


			int areaCount;
			subdivise_to_area( &areaCount, m_context.idxAreaA, m_context.binarySampledData, samplW, samplH );

		//	su::clock::GetTime( &t0 );
		//	dt.subArea = t0-t1;

			if( !areaCount )
			{
				SU_STDOUT( "Zero area found\n" );
				return false;
			}

			m_context.SetupAreaCount( areaCount, m_pool );
			int dominantArea = get_dominant_area( m_context.idxAreaA, samplW, samplH, m_context.areaCountBuffer, areaCount );

		//	su::clock::GetTime( &t1 );
		//	dt.domiArea = t1-t0;

			int quadrangleMargin = 35;
			Vec4 roiQuad = quadrangle( dominantArea, m_context.idxAreaA, samplW, samplH, quadrangleMargin, m_context.samplFactor );
		//	su::clock::GetTime( &t0 );
		//	dt.quadr = t0-t1;

			SU_ASSERT( roiQuad.z * roiQuad.w <= inWidth * inHeight );

			extract_roi( m_context.roiData, roiQuad, m_context.grayData, inWidth, inHeight );
		//	su::clock::GetTime( &t1 );
		//	dt.extractroi = t1-t0;

			binary_thresholding( m_context.roiData, m_context.roiData, roiQuad.z, roiQuad.w, 140 /*0.55f*255*/ );
		//	su::clock::GetTime( &t0 );
		//	dt.binTh2 = t0-t1;

			Vec2 startPoint;
			if( !find_start( &startPoint, m_context.roiData, roiQuad ) )
			{
				SU_STDOUT( "Failed to find_start\n" );
				return false;
			}
		//	su::clock::GetTime( &t1 );
		//	dt.findStart = t1-t0;

			
			int edgeIdxMax = ( roiQuad.z + roiQuad.w ) * 4;
			m_context.SetupEdgeIdxCount( edgeIdxMax, m_pool );
			SU_ASSERT( m_context.edgeIdxA );
			
			int edgeIdxCount = 0;
			if( !edge_track( m_context.edgeIdxA, &edgeIdxCount, edgeIdxMax, m_context.roiData, roiQuad, startPoint ) )
			{
				SU_STDOUT( "Failed to edge_track\n" );
				return false;
			}
		//	su::clock::GetTime( &t0 );
		//	dt.edgeTrack = t0-t1;
			
			int cornerIdxA[4];
			if( !get_corner_index( cornerIdxA, m_context.edgeIdxA, edgeIdxCount, roiQuad ) )
			{
				SU_STDOUT( "Failed to get_corner_index\n" );
				return false;
			}
		//	su::clock::GetTime( &t1 );
		//	dt.cornIdx = t1-t0;

			Vec2 cornerCoordA[4];
			if( !get_corner_coordinate( cornerCoordA, cornerIdxA, m_context.edgeIdxA, edgeIdxCount, roiQuad, m_context.coordBuffer ) )
			{
				SU_STDOUT( "Failed to get_corner_coordinate\n" );
				return false;
			}
		//	su::clock::GetTime( &t0 );
		//	dt.cornCoord = t0-t1;

			roi_to_image( cornerCoordA, roiQuad );


			Matrix homography;
			if( !compute_homography( &homography, cornerCoordA ) )
			{
				SU_STDOUT( "Failed to compute homography\n" );
				return false;
			}
		//	su::clock::GetTime( &t1 );
		//	dt.hogr = t1-t0;

			compute_matrix( outView, outProj, homography, inFocal, inWidth, inHeight );

		//	su::clock::GetTime( &t0 );
		//	dt.matrix = t0-t1;


			/*
			SU_STDOUT( "Algo Time log\n" );
			SU_STDOUT( LString( "%%\t togray\n" ).arg( dt.togray ) );
			SU_STDOUT( LString( "%%\t sampling\n" ).arg( dt.sampling ) );
			SU_STDOUT( LString( "%%\t adapt_threshold\n" ).arg( dt.adapt_threshold ) );
			SU_STDOUT( LString( "%%\t blur\n" ).arg( dt.blur ) );
			SU_STDOUT( LString( "%%\t binTh\n" ).arg( dt.binTh ) );
			SU_STDOUT( LString( "%%\t subArea\n" ).arg( dt.subArea ) );
			SU_STDOUT( LString( "%%\t domiArea\n" ).arg( dt.domiArea ) );
			SU_STDOUT( LString( "%%\t quadr\n" ).arg( dt.quadr ) );
			SU_STDOUT( LString( "%%\t extractroi\n" ).arg( dt.extractroi ) );
			SU_STDOUT( LString( "%%\t binTh2\n" ).arg( dt.binTh2 ) );
			SU_STDOUT( LString( "%%\t findStart\n" ).arg( dt.findStart ) );
			SU_STDOUT( LString( "%%\t edgeTrack\n" ).arg( dt.edgeTrack ) );
			SU_STDOUT( LString( "%%\t cornIdx\n" ).arg( dt.cornIdx ) );
			SU_STDOUT( LString( "%%\t cornCoord\n" ).arg( dt.cornCoord ) );
			SU_STDOUT( LString( "%%\t hogr\n" ).arg( dt.hogr ) );
			SU_STDOUT( LString( "%%\t matrix\n" ).arg( dt.matrix ) );
			SU_STDOUT( "Algo Time End\n" );
			*/

			return true;
		}


	};


}


