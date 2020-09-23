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



#ifndef _su_math_h_
#define _su_math_h_



namespace su
{

	//--------------------------
	//
	// Constants
	//
	// Nan : 0xFFC00000
	// -Inf: 0xFF800000
	// +Inf: 0x7F800000
	//--------------------------

	su_ie extern const float	Pi;				// PI
	su_ie extern const float	HalfPi;			// PI/2
	su_ie extern const float	TwoPi;			// 2PI
	su_ie extern const float	OOPi;			// 1/PI
	su_ie extern const float	FpEpsilon;		// smallest such that 1.0+FpEpsilon != 1.0
	su_ie extern const float	FpMin;			// min positive value
	su_ie extern const float	FpMax;			// max value
	su_ie extern const float	DegToRad;		// Deg -> Rad
	su_ie extern const float	RadToDeg;		// Rad -> Deg



	//--------------------------
	//
	// Functions
	//
	//--------------------------

	su_ie float		Asin	(	float x								);
	su_ie float		Asinf	(	float x								);
	su_ie float		Acos	(	float x								);
	su_ie float		Acosf	(	float x								);
	su_ie float		Atan	(	float x								);
	su_ie float		Atanf	(	float x								);
	su_ie float		Atan2	(	float x, float y					);
	su_ie float		Atan2f	(	float x, float y					);
	su_ie float		Sin		(	float a								);
	su_ie float		Sinf	(	float a								);
	su_ie float		Cos		(	float a								);
	su_ie float		Cosf	(	float a								);
	su_ie void		Sincos	(	float a, float* outS, float* outC	);
	su_ie void		Sincosf	(	float a, float* outS, float* outC	);
	su_ie float		Tan		(	float a								);
	su_ie float		Tanf	(	float a								); 
	su_ie float		Exp		(	float x								);
	su_ie float		Expf	(	float x								);
	su_ie float		Log		(	float x								);
	su_ie float		Logf	(	float x								);
	su_ie float		Log2	(	float x								);
	su_ie float		Log2f	(	float x								);
	su_ie float		Log10	(	float x								);
	su_ie float		Log10f	(	float x								);
	su_ie float		Pow		(	float x, float y					);
	su_ie float		Powf	(	float x, float y					);
	su_ie float		Isqrt	(	float x								);
	su_ie float		Isqrtf	(	float x								);
	su_ie float		Sqrt	(	float x								);
	su_ie float		Sqrtf	(	float x								);
	su_ie int		Ftoi	(	float x								);
	su_ie float		Itof	(	int	  x								);
	su_ie float		Abs		(	float x								);
	su_ie float		Fabs	(	float x								);
	su_ie float		Fabsf	(	float x								);
	su_ie float		Absf	(	float x								);
	su_ie float		Fmod	(	float x, float y					);
	su_ie float		Ceil	(	float x								);
	su_ie float		Floor	(	float x								);
	su_ie float		Round	(	float x								);
	su_ie float		Near	(	float x, float y, float eps=0.0001f	);

	template <typename T>	T	Abs		( const T & inV );
	template <typename T>	T	Min		( const T & inA, const T & inB );
	template <typename T>	T	Max		( const T & inA, const T & inB );
	template <typename T>	T	Clamp	( const T & inV, const T & inMin, const T & inMax );



	struct	Vec2;
	struct	Vec3;
	struct	Vec4;
	struct	Quat;
	struct	Matrix;
	struct	Box3;
	struct	Sph3;





	//--------------------------
	//
	// 2D Vector ( x , y )
	//
	//--------------------------

	struct_ie Vec2
	{
					Vec2	(										) {}
					Vec2	(	const float *						);
					Vec2	(	const Vec2 &						);
					Vec2	(	float inX, float inY				);
		explicit	Vec2	(	const Vec3 &						);		// Vec2(x,y)
		explicit	Vec2	(	const Vec4 &						);		// Vec2(x,y)
		explicit	Vec2	(	const Quat &						);		// Vec2(x,y)

		// assignment operators
		Vec2&		Set		(	float inX, float inY				);
		Vec2&	operator  =	(	const Vec2 &						);
		Vec2&	operator +=	(	const Vec2 &						);
		Vec2&	operator -=	(	const Vec2 &						);
		Vec2&	operator *=	(	float								);
		Vec2&	operator /=	(	float								);

		// unary operators
		Vec2	operator +	(										) const;
		Vec2	operator -	(										) const;

		// binary operators
		Vec2	operator +	(	const Vec2 &						) const;
		Vec2	operator -	(	const Vec2 &						) const;
		float	operator ^	(	const Vec2 &						) const;
		float	operator *	(	const Vec2 &						) const;
		Vec2	operator *	(	float								) const;
		Vec2	operator /	(	float								) const;

		friend Vec2 operator * ( float, const Vec2 &				);

		// test operators
		bool	operator == (	const Vec2 &						) const;
		bool	operator != (	const Vec2 &						) const;
		bool	isnear		(	const Vec2 &, float d = FpEpsilon	) const;

		// others
		Vec2	swizzle		(	uint32 arr=SU_MCHAR2("xy")			) const;	// Swizzle('yx') = { y, x }

		float	x, y;

		static	const Vec2	ZERO;			// {0,0}
		static	const Vec2	ONE;			// {1,1}
		static	const Vec2	UNIT_X;			// {1,0}
		static	const Vec2	UNIT_Y;			// {0,1}
		static	const Vec2	POSMIN;			// {FpMin,FpMin}
		static	const Vec2	POSMAX;			// {FpMax,FpMax}
		static	const Vec2	NEGMIN;			// {-FpMin,-FpMin}
		static	const Vec2	NEGMAX;			// {-FpMax,-FpMax}
	};


	// Vec2Zero				= Vec2(0,0)
	// Vec2Norm				= ||V||
	// Vec2Norm2			= ||V||^2
	// Vec2Normalize		= V/||V||
	// Vec2Cross			= ( Vec3(V0,0) x Vec3(V1,0) ).z
	// Vec2Dot				= V0.V1
	// Vec2Near				= true if ||V0-V1||<=d²
	// Vec2Add				= V0+V1
	// Vec2Sub				= V0-V1
	// Vec2Mul				= Vec2( x0*x1, y0*y1 )
	// Vec2Min				= Vec2( min(x0,x1), min(y0,y1) )
	// Vec2Max				= Vec2( max(x0,x1), max(y0,y1) )
	// Vec2Scale			= Vec2( x*f, y*f )
	// Vec2Apply			= Vec2( Vec4(V,0,1) * M )
	// Vec2ApplyVector		= Vec3( Vec4(V,0,0) * M )			(rotation only)
	// Vec2Lerp				= V0 + F*(V1-V0) : Linear interpolation from V0 to V1

	su_ie Vec2*		Vec2Copy		(       Vec2* outV, const Vec2* inV									);
	su_ie Vec2*		Vec2Zero		(       Vec2* outV													);
	su_ie float		Vec2Norm		( const Vec2* inV													);
	su_ie float		Vec2Norm2		( const Vec2* inV													);
	su_ie Vec2*		Vec2Normalize	(       Vec2* outV, const Vec2* inV									);
	su_ie float		Vec2Cross		( const Vec2* inV0, const Vec2* inV1								);
	su_ie float		Vec2Dot			( const Vec2* inV0, const Vec2* inV1								);
	su_ie bool		Vec2Near		( const Vec2* inV0, const Vec2* inV1, float d = FpEpsilon			);
	su_ie Vec2*		Vec2Add			(       Vec2* outV, const Vec2* inV0, const Vec2* inV1				);
	su_ie Vec2*		Vec2Sub			(       Vec2* outV, const Vec2* inV0, const Vec2* inV1				);
	su_ie Vec2*		Vec2Mul			(       Vec2* outV, const Vec2* inV0, const Vec2* inV1				);
	su_ie Vec2*		Vec2Min			(       Vec2* outV, const Vec2* inV0, const Vec2* inV1				);
	su_ie Vec2*		Vec2Max			(       Vec2* outV, const Vec2* inV0, const Vec2* inV1				);
	su_ie Vec2*		Vec2Scale		(       Vec2* outV, const Vec2* inV,  float inF						);
	su_ie Vec2*		Vec2Lerp		(       Vec2* outV, const Vec2* inV0, const Vec2* inV1, float inF	);
	su_ie Vec2*		Vec2Apply		(       Vec2* outV, const Vec2* inV,  const Matrix* inM				);
	su_ie Vec2*		Vec2ApplyVector	(       Vec2* outV, const Vec2* inV,  const Matrix* inM				);
	su_ie Vec2		operator *		(		float, const Vec2 &											);





	//--------------------------
	//
	// 3D Vector ( x , y , z )
	//
	//--------------------------

	struct_ie Vec3
	{
					Vec3	(										) {}
					Vec3	(	const float *						);
					Vec3	(	const Vec3 &						);
					Vec3	(	float inX, float inY, float inZ		);
		explicit	Vec3	(	const Vec2 &						);		// Vec3(x,y,0)
		explicit	Vec3	(	const Vec4 &						);		// Vec3(x,y,z)
		explicit	Vec3	(	const Quat &						);		// Vec3(x,y,z)

		// assignment operators
		Vec3&		Set		(	float inX, float inY, float inZ		);
		Vec3&	operator  =	(	const Vec3 &						);
		Vec3&	operator +=	(	const Vec3 &						);
		Vec3&	operator -=	(	const Vec3 &						);
		Vec3&	operator *=	(	const Matrix &						);
		Vec3&	operator *=	(	const Quat &						);
		Vec3&	operator *=	(	float								);
		Vec3&	operator /=	(	float								);

		// unary operators
		Vec3	operator +	(										) const;
		Vec3	operator -	(										) const;

		// binary operators
		Vec3	operator +	(	const Vec3 &						) const;
		Vec3	operator -	(	const Vec3 &						) const;
		Vec3	operator ^	(	const Vec3 &						) const;
		float	operator *	(	const Vec3 &						) const;
		Vec3	operator *	(	const Matrix &						) const;
		Vec3	operator *	(	const Quat &						) const;
		Vec3	operator *	(	float								) const;
		Vec3	operator /	(	float								) const;

		friend Vec3 operator * ( float, const Vec3 &				);

		// test operators
		bool	operator == (	const Vec3 &						) const;
		bool	operator != (	const Vec3 &						) const;
		bool	isnear		(	const Vec3 &, float d = FpEpsilon	) const;

		// others
		Vec3	swizzle		(	uint32 arr=SU_MCHAR3("xyz")			) const;	// Swizzle('xyx') = { x, y, x }

		float	x, y, z;

		static	const Vec3	ZERO;			// {0,0,0}
		static	const Vec3	ONE;			// {1,1,1}
		static	const Vec3	UNIT_X;			// {1,0,0}
		static	const Vec3	UNIT_Y;			// {0,1,0}
		static	const Vec3	UNIT_Z;			// {0,0,1}
		static	const Vec3	POSMIN;			// {FpMin,FpMin,FpMin}
		static	const Vec3	POSMAX;			// {FpMax,FpMax,FpMax}
		static	const Vec3	NEGMIN;			// {-FpMin,-FpMin,-FpMin}
		static	const Vec3	NEGMAX;			// {-FpMax,-FpMax,-FpMax}
	};

	// Vec3Zero					= Vec3(0,0,0)
	// Vec3Norm					= ||V||
	// Vec3Norm2				= ||V||^2
	// Vec3Normalize			= V/||V||
	// Vec3CrossProduct			= V0xV1
	// Vec3DotProduct			= V0.V1
	// Vec3Near					= true if ||V0-V1||<=d²
	// Vec3Add					= V0+V1
	// Vec3Sub					= V0-V1
	// Vec3Mul					= Vec3( x0*x1, y0*y1, z0*z1 )
	// Vec3Min					= Vec3( min(x0,x1), min(y0,y1), min(z0,z1) )
	// Vec3Max					= Vec3( max(x0,x1), max(y0,y1), max(z0,z1) )
	// Vec3Scale				= Vec3( x*f, y*f, z*f )
	// Vec3Lerp					= V0 + F*(V1-V0) : Linear interpolation from V0 to V1
	// Vec3Apply				= Vec3( Vec4(V,1) * M )
	// Vec3Apply				= Vec3( Q * Quat(0,V) * Q^-1 )	(expects unit quaternion)
	// Vec3Apply				= Vec3( Vec4(V,1) * M ) * F
	// Vec3ApplyVector			= Vec3( Vec4(V,0) * M )			(rotation only)
	// Vec3ApplyVector			= Vec3( Vec4(V,0) * M ) * F		(rotation only)
	// Vec3Blend			   += Vec3( Vec4(V,1) * M )
	// Vec3Blend			   += Vec3( Vec4(V,1) * M ) * F
	// Vec3BlendVector		   += Vec3( Vec4(V,0) * M )			(rotation only)
	// Vec3BlendVector		   += Vec3( Vec4(V,0) * M ) * F		(rotation only)

	su_ie Vec3*		Vec3Copy			(       Vec3* outV, const Vec3* inV									);
	su_ie Vec3*		Vec3Zero			(       Vec3* outV													);
	su_ie float		Vec3Norm			( const Vec3* inV													);
	su_ie float		Vec3Norm2			( const Vec3* inV													);
	su_ie Vec3*		Vec3Normalize		(       Vec3* outV, const Vec3* inV									);
	su_ie Vec3*		Vec3Cross			(       Vec3* outV, const Vec3* inV0, const Vec3* inV1				);
	su_ie float		Vec3Dot				( const Vec3* inV0, const Vec3* inV1								);
	su_ie bool		Vec3Near			( const Vec3* inV0, const Vec3* inV1, float d = FpEpsilon			);
	su_ie Vec3*		Vec3Add				(       Vec3* outV, const Vec3* inV0, const Vec3* inV1				);
	su_ie Vec3*		Vec3Sub				(       Vec3* outV, const Vec3* inV0, const Vec3* inV1				);
	su_ie Vec3*		Vec3Mul				(       Vec3* outV, const Vec3* inV0, const Vec3* inV1				);
	su_ie Vec3*		Vec3Min				(       Vec3* outV, const Vec3* inV0, const Vec3* inV1				);
	su_ie Vec3*		Vec3Max				(       Vec3* outV, const Vec3* inV0, const Vec3* inV1				);
	su_ie Vec3*		Vec3Scale			(       Vec3* outV, const Vec3* inV,  float inF						);
	su_ie Vec3*		Vec3Lerp			(       Vec3* outV, const Vec3* inV0, const Vec3* inV1, float inF	);
	su_ie Vec3*		Vec3Apply			(       Vec3* outV, const Vec3* inV,  const Matrix* inM				);
	su_ie Vec3*		Vec3Apply			(       Vec3* outV, const Vec3* inV,  const Quat* inQ				);
	su_ie Vec3*		Vec3Apply			(       Vec3* outV, const Vec3* inV,  const Matrix* inM, float inF	);
	su_ie Vec3*		Vec3ApplyVector		(       Vec3* outV, const Vec3* inV,  const Matrix* inM				);
	su_ie Vec3*		Vec3ApplyVector		(       Vec3* outV, const Vec3* inV,  const Matrix* inM, float inF	);
	su_ie Vec3*		Vec3Blend			(       Vec3* outV, const Vec3* inV,  const Matrix* inM				);
	su_ie Vec3*		Vec3Blend			(       Vec3* outV, const Vec3* inV,  const Matrix* inM, float inF	);
	su_ie Vec3*		Vec3BlendVector		(       Vec3* outV, const Vec3* inV,  const Matrix* inM				);
	su_ie Vec3*		Vec3BlendVector		(       Vec3* outV, const Vec3* inV,  const Matrix* inM, float inF	);
	su_ie void		Vec3GetBaseFromBX	(		Vec3* outBX, Vec3* outBY, Vec3* outBZ, const Vec3* inBX		);
	su_ie void		Vec3GetBaseFromBY	(		Vec3* outBX, Vec3* outBY, Vec3* outBZ, const Vec3* inBY		);
	su_ie void		Vec3GetBaseFromBZ	(		Vec3* outBX, Vec3* outBY, Vec3* outBZ, const Vec3* inBZ		);
	su_ie Vec3		operator *			(		float, const Vec3 &											);






	//--------------------------
	//
	// 4D Vector ( x , y , z , w )
	//
	//--------------------------

	struct_ie Vec4
	{
					Vec4	(										) {}
					Vec4	(	const float *						);
					Vec4	(	const Vec4 &						);
					Vec4	(	float x, float y, float z, float w	);
		explicit	Vec4	(	const Vec3 &						);		// Vec4(x,y,z,1)
		explicit	Vec4	(	const Vec2 &						);		// Vec4(x,y,0,1)
		explicit	Vec4	(	const Quat &						);		// Vec4(x,y,z,w)

		// assignment operators
		Vec4&		Set		(	float x, float y, float z, float w	);
		Vec4&	operator  =	(	const Vec4 &						);
		Vec4&	operator +=	(	const Vec4 &						);
		Vec4&	operator -=	(	const Vec4 &						);
		Vec4&	operator *=	(	const Matrix &						);
		Vec4&	operator *=	(	const Quat &						);
		Vec4&	operator *=	(	float								);
		Vec4&	operator /=	(	float								);

		// unary operators
		Vec4	operator +	(										) const;
		Vec4	operator -	(										) const;

		// binary operators
		Vec4	operator +	(	const Vec4 &						) const;
		Vec4	operator -	(	const Vec4 &						) const;
		float	operator *	(	const Vec4 &						) const;
		Vec4	operator *	(	const Matrix &						) const;
		Vec4	operator *	(	const Quat &						) const;
		Vec4	operator *	(	float								) const;
		Vec4	operator /	(	float								) const;

		friend Vec4 operator * ( float, const Vec4 &				);

		// test operators
		bool	operator == (	const Vec4 &						) const;
		bool	operator != (	const Vec4 &						) const;
		bool	isnear		(	const Vec4 &, float d = FpEpsilon	) const;
		uint	clip		(										) const;	// {-z,+z,-y,+y,-x,+x}

		// others
		Vec2	xy			(										) const;	// {x,y}
		Vec2	zw			(										) const;	// {z,w}
		Vec2	xz			(										) const;	// {x,z}
		Vec2	yw			(										) const;	// {y,w}
		Vec4	swizzle		(	uint32 arr=SU_MCHAR4("xyzw")		) const;	// Swizzle('xxyy') = { x, x, y, y }

		float	x, y, z, w;

		static	const Vec4	ZERO;			// {0,0,0,0}
		static	const Vec4	ONE;			// {1,1,1,1}
		static	const Vec4	UNIT_X;			// {1,0,0,0}
		static	const Vec4	UNIT_Y;			// {0,1,0,0}
		static	const Vec4	UNIT_Z;			// {0,0,1,0}
		static	const Vec4	UNIT_W;			// {0,0,0,1}
		static	const Vec4	POSMIN;			// {FpMin,FpMin,FpMin,FpMin}
		static	const Vec4	POSMAX;			// {FpMax,FpMax,FpMax,FpMax}
		static	const Vec4	NEGMIN;			// {-FpMin,-FpMin,-FpMin,-FpMin}
		static	const Vec4	NEGMAX;			// {-FpMax,-FpMax,-FpMax,-FpMax}
	};

	// Vec4Zero					= Vec3(0,0,0,0)
	// Vec4Norm					= ||V||
	// Vec4Norm2				= ||V||^2
	// Vec4Normalize			= V/||V||
	// Vec4Cross				= x(V0,V1,V2)
	// Vec4Dot					= V0.V1
	// Vec4Near					= true if ||V0-V1||<=d²
	// Vec4Add					= V0+V1
	// Vec4Sub					= V0-V1
	// Vec4Mul					= Vec4( x0*x1, y0*y1, z0*z1, w0*w1 )
	// Vec4Min					= Vec4( min(x0,x1), min(y0,y1), min(z0,z1), min(w0,w1) )
	// Vec4Max					= Vec4( max(x0,x1), max(y0,y1), max(z0,z1), max(w0,w1) )
	// Vec4Scale				= Vec4( x*f, y*f, z*f, w*f )
	// Vec4Lerp					= V0 + F*(V1-V0) : Linear interpolation from V0 to V1
	// Vec4Apply				= V * M
	// Vec4Apply				= (V * M) * F
	// Vec4Apply				= Q * V * Q^-1
	// Vec4ApplyVector			= (V{.w=0} * M)				(rotation only, outputed w=1)
	// Vec4ApplyVector			= (V{.w=0} * M) * F			(rotation only, outputed w=1)
	// Vec4Blend			   += (V * M)
	// Vec4Blend			   += (V * M) * F
	// Vec4BlendVector		   += (V{.w=0} * M)				(rotation only, outputed w=1)
	// Vec4BlendVector		   += (V{.w=0} * M) * F			(rotation only, outputed w=1)
	// Vec4Clip					: returns bitfield {-z,+z,-y,+y,-x,+x}
	//							  -x if set to 1 when .x < -|w|
	//							  +x if set to 1 when .x > +|w|

	su_ie Vec4*		Vec4Copy			(       Vec4* outV, const Vec4* inV									);
	su_ie Vec4*		Vec4Zero			(       Vec4* outV													);
	su_ie float		Vec4Norm			( const Vec4* inV													);
	su_ie float		Vec4Norm2			( const Vec4* inV													);
	su_ie Vec4*		Vec4Normalize		(       Vec4* outV, const Vec4* inV									);
	su_ie Vec4*		Vec4Cross			(       Vec4* outV, const Vec4* inV0, const Vec4* inV1, const Vec4* inV2	);
	su_ie float		Vec4Dot				( const Vec4* inV0, const Vec4* inV1								);
	su_ie bool		Vec4Near			( const Vec4* inV0, const Vec4* inV1, float d = FpEpsilon			);
	su_ie Vec4*		Vec4Add				(       Vec4* outV, const Vec4* inV0, const Vec4* inV1				);
	su_ie Vec4*		Vec4Sub				(       Vec4* outV, const Vec4* inV0, const Vec4* inV1				);
	su_ie Vec4*		Vec4Mul				(       Vec4* outV, const Vec4* inV0, const Vec4* inV1				);
	su_ie Vec4*		Vec4Min				(       Vec4* outV, const Vec4* inV0, const Vec4* inV1				);
	su_ie Vec4*		Vec4Max				(       Vec4* outV, const Vec4* inV0, const Vec4* inV1				);
	su_ie Vec4*		Vec4Scale			(       Vec4* outV, const Vec4* inV,  float inF						);
	su_ie Vec4*		Vec4Lerp			(       Vec4* outV, const Vec4* inV0, const Vec4* inV1, float inF	);
	su_ie Vec4*		Vec4Apply			(       Vec4* outV, const Vec4* inV,  const Matrix* inM				);
	su_ie Vec4*		Vec4Apply			(       Vec4* outV, const Vec4* inV,  const Matrix* inM, float inF	);
	su_ie Vec4*		Vec4Apply			(       Vec4* outV, const Vec4* inV,  const Quat* inQ				);
	su_ie Vec4*		Vec4ApplyVector		(       Vec4* outV, const Vec4* inV,  const Matrix* inM				);
	su_ie Vec4*		Vec4ApplyVector		(       Vec4* outV, const Vec4* inV,  const Matrix* inM, float inF	);
	su_ie Vec4*		Vec4Blend			(       Vec4* outV, const Vec4* inV,  const Matrix* inM				);
	su_ie Vec4*		Vec4Blend			(       Vec4* outV, const Vec4* inV,  const Matrix* inM, float inF	);
	su_ie Vec4*		Vec4BlendVector		(       Vec4* outV, const Vec4* inV,  const Matrix* inM				);
	su_ie Vec4*		Vec4BlendVector		(       Vec4* outV, const Vec4* inV,  const Matrix* inM, float inF	);
	su_ie uint		Vec4Clip			(		const Vec4* inV												);
	su_ie Vec4		operator *			(		float, const Vec4 &											);




	//--------------------------
	//
	// Quaternion ( s , V )
	//
	//	rotation around U of theta radians.
	//  ( s , V ) = ( cos(theta/2) ,    sin(theta/2)*U    )
	//			  = (     q.w      , Vec3(q.x,.q.y,q.z)   )
	//
	// left <- right composition order
	// i.e.		P -> R0 -> R1 -> R2 -> P'
	//		=>	P' = (Q2*Q1*Q0) * (0,P) * (Q2*Q1*Q0)^-1
	//
	//--------------------------

	struct_ie Quat
	{
					Quat	(												) {}
					Quat	(	const float *								);
					Quat	(	const Quat &								);
					Quat	(	float x, float y, float z, float w			);
					Quat	(	float theta, const Vec3 & inU				);	// ( angle, axis );
		explicit	Quat	(	const Vec2 &								);	// Quat(x,y,0,0)
		explicit	Quat	(	const Vec3 &								);	// Quat(x,y,z,0)
		explicit	Quat	(	const Vec4 &								);	// Quat(x,y,z,w)
		explicit	Quat	(	const Matrix &								);  // Rotation only

		// assignment operators
		Quat&		Set		(	float x, float y, float z, float w			);
		Quat&	operator  =	(	const Quat &								);
		Quat&	operator *=	(	const Quat &								);
		Quat&	operator +=	(	const Quat &								);
		Quat&	operator -=	(	const Quat &								);
		Quat&	operator *=	(	float										);
		Quat&	operator /=	(	float										);

		// unary operators
		Quat	operator +	(												) const;
		Quat	operator -	(												) const;

		// binary operators
		Quat	operator *	(	const Quat &								) const;
		Quat	operator +	(	const Quat &								) const;
		Quat	operator -	(	const Quat &								) const;
		Quat	operator *	(	float										) const;
		Quat	operator /	(	float										) const;

		friend Quat operator * ( float, const Quat &						);

		// test operators
		bool	operator == (	const Quat &								) const;
		bool	operator != (	const Quat &								) const;
		bool	isnear		(	const Quat &, float d = FpEpsilon			) const;

		float	x, y, z, w;

		static	const Quat	ZERO;		// {0,0,0,0}
		static	const Quat	ONE;		// {1,1,1,1}
		static	const Quat	UNIT;		// {0,0,0,1}
	};

	// QuatZero					= Quat(0,0,0,0)
	// QuatIdentity				= Id (1,0) = Quat(0,0,0,1)
	// QuatIsIdentity			= Q==Id ?
	// QuatNorm					= ||Q||
	// QuatNorm2				= ||Q||^2
	// QuatNormalize			= Q/||Q||
	// QuatDot					= Q0.Q1
	// QuatAdd					= Q0+Q1
	// QuatSub					= Q0-Q1
	// QuatMul					= Q0*Q1
	// QuatNear					= true if ||V0-V1||<=d²
	// QuatConjugate			= Q' = Quat(-x,-y,-z,+w)	(i.e. -theta rotation <=> ( cos(-theta/2), sin(-theta/2)*U ) = ( cos(theta/2), -sin(theta/2)*U ) = ( s , -V ) )
	// QuatFastInverse			= Q'	(expects unit quaternion)
	// QuatInverse				= Q'/||Q||
	// QuatToAxisAngle			: get axis U and angle theta
	// QuatFromAxisAngle		: set axis U and angle theta
	// QuatFromMatrix			: Q <- MatrixRotation(M)
	// QuatEuler				: Yaw around the Y axis, pitch/X, roll/Z
	// QuatLn					: ln(q)=(0,theta/2*U)	(expects unit quaternion)
	// QuatExp					: exp(ln(q))=q	(expects pure quaternion (w=0). w is ignored in calculation)
	// QuatSlerp				: spherical linear interpolation from Q0(inT=0) to Q1(inT=1)	(expects unit quaternions)

	su_ie Quat*		QuatCopy			(       Quat* outQ, const Quat* inQ									);
	su_ie Quat*		QuatZero			(       Quat* outQ													);
	su_ie Quat*		QuatIdentity		(       Quat* outQ													);
	su_ie bool		QuatIsIdentity		( const Quat* inQ													);
	su_ie float		QuatNorm			( const Quat* inQ													);
	su_ie float		QuatNorm2			( const Quat* inQ													);
	su_ie Quat*		QuatNormalize		(       Quat* outQ, const Quat* inQ									);
	su_ie float		QuatDot				( const Quat* inQ0, const Quat* inQ1								);
	su_ie bool		QuatNear			( const Quat* inQ0, const Quat* inQ1, float d = FpEpsilon			);
	su_ie Quat*		QuatAdd				(		Quat* outQ, const Quat* inQ0, const Quat* inQ1				);
	su_ie Quat*		QuatSub				(		Quat* outQ, const Quat* inQ0, const Quat* inQ1				);
	su_ie Quat*		QuatMul				(		Quat* outQ, const Quat* inQ0, const Quat* inQ1				);
	su_ie Quat*		QuatConjugate		(       Quat* outQ, const Quat* inQ									);
	su_ie Quat*		QuatFastInverse		(       Quat* outQ, const Quat* inQ									);
	su_ie Quat*		QuatInverse			(       Quat* outQ, const Quat* inQ									);
	su_ie void		QuatToAxisAngle		(       Vec3* outV, float * outAngle, const Quat* inQ				);
	su_ie Quat*		QuatFromAxisAngle	(       Quat* outQ, const Vec3* inV, float inAngle					);
	su_ie Quat*		QuatFromMatrix		(       Quat* outQ, const Matrix* inM								);
	su_ie Quat*		QuatEuler			(		Quat* outQ, float inEx, float inEy, float inEz				);
	su_ie Quat*		QuatLn				(		Quat* outQ, const Quat* inQ									);
	su_ie Quat*		QuatExp				(		Quat* outQ, const Quat* inQ									);
	su_ie Quat*		QuatSlerp			(		Quat* outQ, const Quat* inQ0, const Quat* inQ1, float inT	);
	su_ie Quat		operator *			(		float, const Quat &											);
	#define	SU_ASSERT_UNIT_QUAT(inQP)	SU_ASSERT( Fabsf(QuatNorm2((inQP))-1.0f) < 0.001f )




	//--------------------------
	//
	// 4D Matrix
	//
	// Matrix use row-major order:
	//
	//	[ RX.x RX.y RX.z 0 ]
	//	[ RY.x RY.y RY.z 0 ]
	//	[ RZ.x RZ.y RZ.z 0 ]
	//	[ TX   TY   TZ   1 ]
	//
	// which is stored in memory like this:
	//
	//   float M = { RX.x, RX.y, RX.z, 0, ... }
	//
	// left -> right composition order
	//
	// i.e.		P -> M0 -> M1 -> M2 -> P'
	//		=>	P' = P * ( M0 * M1 * M2 )
	//
	// For information, PS2 & PS3 use row-major order.
	// Shader models use column-major order.
	//
	//--------------------------

	struct_ie Matrix
	{
					Matrix	(													) {}
					Matrix	(	const float *									);
					Matrix	(	const Matrix &									);
					Matrix	(	float in11, float in12, float in13, float in14,
								float in21, float in22, float in23, float in24,
								float in31, float in32, float in33, float in34,
								float in41, float in42, float in43, float in44	);
		explicit	Matrix	(	const Quat &									);	// pure rotation matrix
		explicit	Matrix	(	const Vec3 &									);	// pure translation matrix

		// access grants
		float&	operator ()	(	int Row, int Col								);
		float	operator () (	int Row, int Col								) const;

		// assignment operators
		Matrix&	operator  =	(	const Matrix &									);
		Matrix&	operator *=	(	const Matrix &									);
		Matrix&	operator +=	(	const Matrix &									);
		Matrix&	operator -=	(	const Matrix &									);
		Matrix&	operator *=	(	float											);
		Matrix&	operator /=	(	float											);

		// unary operators
		Matrix	operator +	(													) const;
		Matrix	operator -	(													) const;

		// binary operators
		Matrix	operator *	(	const Matrix &									) const;
		Matrix	operator +	(	const Matrix &									) const;
		Matrix	operator -	(	const Matrix &									) const;
		Matrix	operator *	(	float											) const;
		Matrix	operator /	(	float											) const;

		friend Matrix operator * ( float, const Matrix &						);

		// test operators
		bool	operator == (	const Matrix &									) const;
		bool	operator != (	const Matrix &									) const;

		// axis
		Vec3*	right		(													) const;
		Vec3*	up			(													) const;
		Vec3*	front		(													) const;
		Vec3*	location	(													) const;

		union {
			struct {
				float		m11, m12, m13, m14;
				float		m21, m22, m23, m24;
				float		m31, m32, m33, m34;
				float 		m41, m42, m43, m44;
			};
			float m[4][4];	// [row][col]
		};

		static const Matrix	ZERO;
		static const Matrix	UNIT;
	};

	// MatrixZero				= 0
	// MatrixIdentity			= Id
	// MatrixIsIdentity			= M==Id ?
	// MatrixDeterminant		= 4x4 determinant
	// MatrixAdd				= M0+M1
	// MatrixSub				= M0-M1
	// MatrixMul				= M0*M1
	// MatrixScalar				= M*f
	// MatrixTranspose			= M^t
	// MatrixInverse			= M^-1
	// MatrixFastInverse		= M^-1	(expects M is a regular affine matrix with uniform scale)
	// MatrixNormalize			: Matrix Normalisation (normalizes the rotation axis)
	// MatrixOrthoNormalize		: Matrix Orthonormalisation (orthogonizes & normalizes the rotation axis)
	// MatrixTranslate			= M * MatrixTranslation(T)
	// MatrixPreTranslate		= MatrixTranslation(T) * M
	// MatrixRotate				= M * MatrixRotation(R)
	// MatrixPreRotate			= MatrixRotation(R) * M
	// MatrixRotate[X|Y|Z]		= M * MatrixRotation[X|Y|Z]()
	// MatrixPreRotate[X|Y|Z]	= MatrixRotation[X|Y|Z]() * M
	// MatrixScale				= M * MatrixScaling(S)
	// MatrixPreScale			= MatrixScaling(S) * M
	// MatrixTranslation		: Builds a pure translation matrix
	// MatrixRotation[X|Y|Z]	: Builds a pure rotation matrix around [X|Y|Z] of angle radians
	// MatrixRotation			: Builds a pure rotation matrix
	// MatrixScaling			: Builds a pure scaling matrix
	// MatrixBuildTR			: Builds a matrix from identity
	// MatrixSetTR				: Set parts of the matrix
	// MatrixGetTR				: Get rotation and/or translation parts of the matrix
	// MatrixGetAxis			: Get rotation axis
	// MatrixSetAxis			: Set rotation axis
	// MatrixOrthoRH			: Builds a right-handed orthogonal projection matrix.
	// MatrixOrthoLH			: Builds a left-handed orthogonal projection matrix.
	// MatrixPerspectiveRH		: Builds a right-handed perspective projection matrix based on a field of view.
	// MatrixPerspectiveLH		: Builds a left-handed perspective projection matrix based on a field of view.
	// MatrixIsProjRH			: Checks if the projection matrix is right-handled ?
	// MatrixIsProjLH			: Checks if the projection matrix is left-handled ?

	su_ie Matrix*		MatrixCopy			(       Matrix* outM, const Matrix* inM											);
	su_ie Matrix*		MatrixZero			(       Matrix* outM															);
	su_ie Matrix*		MatrixIdentity		(       Matrix* outM															);
	su_ie bool			MatrixIsIdentity	( const Matrix*	inM																);
	su_ie float			MatrixDeterminant	( const Matrix* inM																);
	su_ie Matrix*		MatrixAdd			(       Matrix* outM, const Matrix* inM0, const Matrix* inM1					);
	su_ie Matrix*		MatrixSub			(       Matrix* outM, const Matrix* inM0, const Matrix* inM1					);
	su_ie Matrix*		MatrixMul			(       Matrix* outM, const Matrix* inM0, const Matrix* inM1					);
	su_ie Matrix*		MatrixScalar		(       Matrix* outM, const Matrix* inM, float inF								);
	su_ie Matrix*		MatrixTranspose		(       Matrix* outM, const Matrix* inM											);
	su_ie Matrix*		MatrixInverse		(		Matrix* outM, float * outDeterminant, const Matrix* inM					);
	su_ie Matrix*		MatrixFastInverse	(		Matrix* outM, const Matrix* inM											);
	su_ie Matrix*		MatrixNormalize		(		Matrix* outM, const Matrix* inM											);
	su_ie Matrix*		MatrixOrthoNormalize(		Matrix* outM, const Matrix* inM											);
	su_ie Matrix*		MatrixTranslate		(		Matrix* outM, const Matrix* inM, float inTX, float inTY, float inTZ		);
	su_ie Matrix*		MatrixTranslate		(		Matrix* outM, const Matrix* inM, const Vec3* inV						);
	su_ie Matrix*		MatrixTranslate		(		Matrix* outM, const Matrix* inM, const Matrix* inT						);
	su_ie Matrix*		MatrixPreTranslate	(		Matrix* outM, const Matrix* inM, float inTX, float inTY, float inTZ		);
	su_ie Matrix*		MatrixPreTranslate	(		Matrix* outM, const Matrix* inM, const Vec3* inV						);
	su_ie Matrix*		MatrixPreTranslate	(		Matrix* outM, const Matrix* inM, const Matrix* inT						);
	su_ie Matrix*		MatrixRotate		(		Matrix* outM, const Matrix* inM, const Quat* inQ						);
	su_ie Matrix*		MatrixRotate		(		Matrix* outM, const Matrix* inM, const Vec3* inAxis, float inAngle		);
	su_ie Matrix*		MatrixRotate		(		Matrix* outM, const Matrix* inM, const Matrix* inR						);
	su_ie Matrix*		MatrixPreRotate		(		Matrix* outM, const Matrix* inM, const Quat* inQ						);
	su_ie Matrix*		MatrixPreRotate		(		Matrix* outM, const Matrix* inM, const Vec3* inAxis, float inAngle		);
	su_ie Matrix*		MatrixPreRotate		(		Matrix* outM, const Matrix* inM, const Matrix* inR						);
	su_ie Matrix*		MatrixRotateX		(		Matrix* outM, const Matrix* inM, float inAngle							);
	su_ie Matrix*		MatrixRotateY		(		Matrix* outM, const Matrix* inM, float inAngle							);
	su_ie Matrix*		MatrixRotateZ		(		Matrix* outM, const Matrix* inM, float inAngle							);
	su_ie Matrix*		MatrixPreRotateX	(		Matrix* outM, const Matrix* inM, float inAngle							);
	su_ie Matrix*		MatrixPreRotateY	(		Matrix* outM, const Matrix* inM, float inAngle							);
	su_ie Matrix*		MatrixPreRotateZ	(		Matrix* outM, const Matrix* inM, float inAngle							);
	su_ie Matrix*		MatrixScale			(		Matrix* outM, const Matrix* inM, float inSX, float inSY, float inSZ		);
	su_ie Matrix*		MatrixScale			(		Matrix* outM, const Matrix* inM, const Vec3* inV						);
	su_ie Matrix*		MatrixScale			(		Matrix* outM, const Matrix* inM, const Matrix* inS						);
	su_ie Matrix*		MatrixPreScale		(		Matrix* outM, const Matrix* inM, float inSX, float inSY, float inSZ		);
	su_ie Matrix*		MatrixPreScale		(		Matrix* outM, const Matrix* inM, const Vec3* inV						);
	su_ie Matrix*		MatrixPreScale		(		Matrix* outM, const Matrix* inM, const Matrix* inS						);
	su_ie Matrix*		MatrixTranslation	(		Matrix* outM, float inTX, float inTY, float inTZ						);
	su_ie Matrix*		MatrixTranslation	(		Matrix* outM, const Vec3* inV											);
	su_ie Matrix*		MatrixTranslation	(		Matrix* outM, const Matrix* inM	 										);
	su_ie Matrix*		MatrixRotationX		(		Matrix* outM, float inAngle												);
	su_ie Matrix*		MatrixRotationY		(		Matrix* outM, float inAngle												);
	su_ie Matrix*		MatrixRotationZ		(		Matrix* outM, float inAngle												);
	su_ie Matrix*		MatrixRotation		(		Matrix* outM, const Vec3* inAxis, float inAngle							);
	su_ie Matrix*		MatrixRotation		(		Matrix* outM, const Quat* inQ											);
	su_ie Matrix*		MatrixRotation		(		Matrix* outM, const Matrix* inM	 										);
	su_ie Matrix*		MatrixScaling		(		Matrix* outM, float inSX, float inSY, float inSZ						);
	su_ie Matrix*		MatrixScaling		(		Matrix* outM, const Vec3* inV											);
	su_ie Matrix*		MatrixScaling		(		Matrix* outM, const Matrix* inM	 										);
	su_ie Matrix*		MatrixBuildTR		(		Matrix*	outM, const Vec3* inT, const Quat* inR, const Vec3* inS			);
	su_ie Matrix*		MatrixSetTR			(		Matrix*	outM, const Vec3* inT, const Quat* inR, const Vec3* inS, const Matrix* inM	);
	su_ie void			MatrixGetTR			(		Vec3*	outT, Quat* outR, Vec3*	outS, const Matrix*	inM					);
	su_ie void			MatrixGetAxis		(		Vec3*	outRight, Vec3* outUp, Vec3* outFront, const Matrix* inM		);
	su_ie Matrix*		MatrixSetAxis		(		Matrix*	outM, const Vec3* inRight, const Vec3* inUp, const Vec3* inFront);
	su_ie Matrix*		MatrixOrthoRH		(		Matrix*	outM, float inW, float inH, float inZNear, float inZFar			);
	su_ie Matrix*		MatrixOrthoLH		(		Matrix*	outM, float inW, float inH, float inZNear, float inZFar			);
	su_ie Matrix*		MatrixPerspectiveRH	(		Matrix*	outM, float inFovx, float inAspect, float inZNear, float inZFar	);
	su_ie Matrix*		MatrixPerspectiveLH	(		Matrix*	outM, float inFovx, float inAspect, float inZNear, float inZFar	);
	su_ie bool			MatrixIsProjRH		(		Matrix*	inM																);
	su_ie bool			MatrixIsProjLH		(		Matrix*	inM																);
	su_ie void			MatrixStore			(		float*	outF, Matrix* inM												);
	su_ie void			MatrixLoad			(		Matrix* outM, float*  inF 												);
	su_ie Matrix		operator *			(		float inF, const Matrix & inM											);



	//--------------------------
	//
	// T-R[-S] Array -> Matrix Array
	//
	// inUsePIdx = FALSE :
	//	M[i] = convert_to_matrix(TR[i])
	//
	// inUsePIdx = TRUE  :
	//	use the parent index.
	//	- (i==0) : M[0] = convert_to_matrix(TR[0])
	//	- (i >0) : M[i] = convert_to_matrix(TR[i]) * M[ TR[i].parentIdx ]
	//			   note 1 : asserting (TR[i].parentIdx <= i) !!!
	//			   note 2 : no-parent => parentIdx = -1
	//
	//--------------------------

	struct TR {		// 2QW
		Vec3	t;			// location
		int32	parentIdx;	// [optionnal parent index, none = -1]
		Quat	r;			// rotation
	};

	struct TRS {	// 3QW
		Vec3	t;			// location
		int32	parentIdx;	// [optionnal parent index, none = -1]
		Vec3	s;			// scale
		float	pad0;
		Quat	r;			// rotation
	};

	su_ie void		MatrixArraySetTR	(	Matrix*	outMArray, const TR*   inTRArray,   uint inCount );
	su_ie void		MatrixArraySetTR	(	Matrix*	outMArray, const TRS*  inTRSArray,  uint inCount );		// scale is ignored !
	su_ie void		MatrixArraySetTRS	(	Matrix*	outMArray, const TRS*  inTRSArray,  uint inCount );		// scale is applied !
	su_ie void		MatrixArrayMul		(	Matrix*	outMArray, Matrix* inM0Array, Matrix* inM1Array, uint inCount );





	template < uint Capacity >
	struct_ie DataMatrix
	{
					DataMatrix	(												) {}
					DataMatrix	(	int ColCount, int RawCount					);
					DataMatrix	(	int ColCount, int RawCount, const byte*		);
					DataMatrix	(	const DataMatrix<Capacity>&					);

		// assignment operators
		DataMatrix&	operator =	(	const DataMatrix<Capacity>&					);

		// access grants
		byte&	operator ()		(	int ColIdx, int RawIdx 						);
		byte	operator ()		(	int ColIdx, int RawIdx						) const;

		// test operators
		bool	operator ==		(	const DataMatrix<Capacity> &				) const;
		bool	operator !=		(	const DataMatrix<Capacity> &				) const;

		static const int capacity = Capacity;

		byte byteA[capacity];
		int rawCount, colCount;
	};

	typedef DataMatrix<10>		DataMatrix10;
	typedef DataMatrix<100>		DataMatrix100;
	typedef DataMatrix<1000>	DataMatrix1000;


	template<uint C>
	DataMatrix<C>*		DataMatrixCopy				(	DataMatrix<C>*	outM,	const DataMatrix<C>*	inM	);

	template<uint C>
	DataMatrix<C>*		DataMatrixRotateLeft		(	DataMatrix<C>*	outM,	const DataMatrix<C>*	inM	);

	template<uint C>
	DataMatrix<C>*		DataMatrixRotateRight		(	DataMatrix<C>*	outM,	const DataMatrix<C>*	inM	);

	template<uint C>
	DataMatrix<C>*		DataMatrixHorizontalFlip	(	DataMatrix<C>*	outM,	const DataMatrix<C>*	inM	);

	template<uint C>
	DataMatrix<C>*		DataMatrixVerticalFlip		(	DataMatrix<C>*	outM,	const DataMatrix<C>*	inM	);




	//--------------------------
	//
	// 3D Box ( center , rotation , length )
	//
	//--------------------------

	struct_ie Box3
	{
					Box3	(													) {}
					Box3	(	const Box3 &									);
		explicit	Box3	(	const Sph3 &									);

		// concat operators
		Box3&	operator  =	(	const Box3 &									);
		Box3&	operator +=	(	const Box3 &									);
		Box3	operator +	(	const Box3 &									) const;

		// TR
		Box3&	operator *=	(	const Matrix &									);
		Box3	operator *	(	const Matrix &									) const;

		// test operators
		bool	operator == (	const Box3 &									) const;
		bool	operator != (	const Box3 &									) const;

		Vec3	center;
		Quat	rotation;
		Vec3	length;		//	< X-width>=0, Y-height>=0, Z-depth>=0 > in the local box space
	};

	// Box3Concat				: Bounding box of b0 & b1
	// Box3AxysAlign			: Computes a no-rotation-bounding-box of b
	// Box3Apply				: Apply a TR on the box

	su_ie Box3*		Box3Copy		(	Box3* outB, const Box3* inB							);
	su_ie Box3*		Box3Concat		(	Box3* outB, const Box3* inB0, const Box3* inB1		);
	su_ie Box3*		Box3AxysAlign	(	Box3* outB, const Box3* inB							);
	su_ie Box3*		Box3Apply		(	Box3* outB, const Box3* inB,  const Matrix* inM		);





	//--------------------------
	//
	// 3D Sphere ( center , radius )
	//
	//--------------------------

	struct_ie Sph3
	{
					Sph3	(													) {}
					Sph3	(	const Sph3 &									);
		explicit	Sph3	(	const Box3 &									);

		// concat operators
		Sph3&	operator  =	(	const Sph3 &									);
		Sph3&	operator +=	(	const Sph3 &									);
		Sph3	operator +	(	const Sph3 &									) const;

		// TR
		Sph3&	operator *=	(	const Matrix &									);
		Sph3	operator *	(	const Matrix &									) const;

		// test operators
		bool	operator == (	const Sph3 &									) const;
		bool	operator != (	const Sph3 &									) const;

		Vec3	center;
		float	radius;		// >= 0
	};

	// Sph3Concat				: Bounding sphere of s0 & s1
	// Sph3Apply				: Apply a TR on the sphere

	su_ie Sph3*		Sph3Copy		(       Sph3* outS, const Sph3* inB									);
	su_ie Sph3*		Sph3Concat		(       Sph3* outS, const Sph3* inS0, const Sph3* inS1				);
	su_ie Sph3*		Sph3Apply		(       Sph3* outS, const Sph3* inS,  const Matrix* inM				);




	//--------------------------
	//
	// 3D Frustum (6 planes, inside oriented)
	//
	//--------------------------

	struct_ie Frustum3
	{
					Frustum3	(													) {}
					Frustum3	(	const Frustum3 &								);
		explicit	Frustum3	(	const Box3 &									);
		explicit	Frustum3	(	const Sph3 &									);
		explicit	Frustum3	(	const Matrix &									);

		// some tests
		bool		IsInside	(	const Box3 &									) const;
		bool		IsInside	(	const Sph3 &									) const;
		bool		IsInside	(	const Vec3 &									) const;
		bool		IsInside	(	const Vec4 &									) const;		// w is ignored

		Vec4		plane[6];	// left/right/bottom/top/near/far
	};

	// Frustum3Build			: Build from a box
	// Frustum3Build			: Build from a sphere
	// Frustum3Build			: Build from a projection matrix
	// Frustum3Apply			: Apply a TR on the frustum
	// FrustumClip				: returns bitfield {-z,+z,-y,+y,-x,+x}
	//							  -x if set to 1 when .x < -|w|
	//							  +x if set to 1 when .x > +|w|
	// FrustumInside			: returns TRUE is inside

	su_ie Frustum3*		Frustum3Copy	(       Frustum3* outF, const Frustum3* inF							);
	su_ie Frustum3*		Frustum3Build	(       Frustum3* outF, const Box3* inB								);
	su_ie Frustum3*		Frustum3Build	(       Frustum3* outF, const Sph3* inS								);
	su_ie Frustum3*		Frustum3Build	(       Frustum3* outF, const Matrix* inM							);
	su_ie Frustum3*		Frustum3Apply	(       Frustum3* outF, const Frustum3* inF,  const Matrix* inM		);
	su_ie uint			Frustum3Clip	(		const Frustum3* inF, const Vec3* inV, float inRadius = 0.f	);
	su_ie uint			Frustum3Clip	(		const Frustum3* inF, const Vec4* inV, float inRadius = 0.f	);
	su_ie bool			Frustum3Inside	(		const Frustum3* inF, const Vec3* inV, float inRadius = 0.f	);
	su_ie bool			Frustum3Inside	(		const Frustum3* inF, const Vec4* inV, float inRadius = 0.f	);




	//--------------------------
	// Curves
	//
	// ComputeHermiteBasis		: Compute Hermite coefficients for u in [0,1]
	// ComputeBezierBasis		: Compute Bezier  coefficients for u in [0,1]
	// ComputeSplineInterp		: Spline interpolation from basics coefficients
	//							  Hermite : <K0,K1,K2,K3> = <P0,P1,T0,T1>
	//							  Bezier  : <K0,K1,K2,K3> = <P0,P1,P2,P3>
	//--------------------------

	su_ie void		ComputeHermiteBasis		(	float *		outB/*[4]*/, float inU						);

	su_ie void		ComputeBezierBasis		(	float *		outB/*[4]*/, float inU						);

	su_ie void		ComputeSplineInterp		(	float &		outP,
												float *		inB/*[4]*/,
												float 		K0, float 	K1, float	K2,	float	K3		);

	su_ie void		ComputeSplineInterp		(	Vec2 &		outP,
												float *		inB/*[4]*/,
												Vec2 &		K0, Vec2 &	K1, Vec2 &	K2,	Vec2 &	K3		);

	su_ie void		ComputeSplineInterp		(	Vec3 &		outP,
												float *		inB/*[4]*/,
												Vec3 &		K0, Vec3 &	K1, Vec3 &	K2,	Vec3 &	K3		);

	su_ie float		ComputeHermiteSmoothStep(	float x0, float x1, float v								);

	su_ie float		GaussianUnit			(	float		inDeviation,
												float		inDistance									);
	su_ie float		Gaussian				(	float		inDeviation,
												float		inDistance									);
	su_ie float 	GaussianDeviation		(	float		inRadius									);
	su_ie int		GaussianWidth			(	float		inDeviation									);
	su_ie int		GaussianNormSequence	(	float*		outG/*[ GaussianWidth() ]*/,					// Output of (GaussianWidth()) samples
												float		inDeviation,
												bool		inNormalize									);	// Normalization ?




	//--------------------------
	// Rotation arcball
	//
	//--------------------------

	struct_ie ArcBall
	{
		struct Point
		{
			Vec3	p3;
			Vec2	p2;
			float	t;
		};

		bool		began;
		Quat		base;
		Quat		state;
		Vec3		lockaxis;			// not used if ZERO
		Vec2		lockXY;
		Point		beginpoint;
		Point		endpoint;
		Point		movepoints[8];
		uint		movecount;

		float		ispeed;
		float		iangle;
		float		itime;

		ArcBall					(				);
		void		Reset		(	const Quat&	inQuat );
		Quat		GetState	(	float inT	);
		void		Begin		(	float inT, const Vec2& inHomogeneousCoord, const Vec3& inLockAxis=Vec3::ZERO );		// [-1,+1]
		void		Begin		(	float inT, const Vec2& inHomogeneousCoord, const Vec2& inLockXY );					// [-1,+1]
		void		Move		(	float inT, const Vec2& inHomogeneousCoord );										// [-1,+1]
		void		End			(	float inT, const Vec2& inHomogeneousCoord );										// [-1,+1]
	};

}


#include "su_mathi.h"
#endif // _su_math_h_


