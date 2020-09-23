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



#if defined(USE_APPROX)

namespace
{

	#define K_BITSCOUNT_INVSQRT		7
	#define K_BITSCOUNT_ACOS		6
	#define K_TABLESIZE_COS			32
	#define K_TABLESIZE_INVSQRT		(1 << K_BITSCOUNT_INVSQRT)
	#define K_TABLESIZE_ACOS		(1 << K_BITSCOUNT_ACOS)

	float							gTable_Acos[K_TABLESIZE_ACOS];
	float							gTable_Cos[K_TABLESIZE_COS+1];
	unsigned char					gTable1_InvSqrt[K_TABLESIZE_INVSQRT];
	float							gTable2_InvSqrt[2] = { 1.F, 1.414213562F };

	/*
	 *	Approximations_Initialize
	 *	--------------------------------------------------------------------------------------- */

	void Approximations_Initialize()
	{
		int			index;

		for (index = 0; index <= K_TABLESIZE_COS; ++ index )
		{
			float						angle;
			
			angle = index * (HalfPi / K_TABLESIZE_COS);
			gTable_Cos[index] = (float) ::cosf (angle );
		}

		for (index = 0; index < K_TABLESIZE_INVSQRT; ++ index )
		{
			uint32						ieeeNumber;
			
			ieeeNumber = (127UL << 23) + (index << (23 - K_BITSCOUNT_INVSQRT));
			
			float						realNumber;
			
			realNumber = * (float *) &ieeeNumber;
			realNumber = (float) (1.0 / ::sqrtf (realNumber ) );
			ieeeNumber = *((uint32*)&realNumber);
			
			gTable1_InvSqrt[index] = (unsigned char) ((ieeeNumber >> (23 - 8)) & 0xffUL);
		}

		for (index = 0; index < K_TABLESIZE_ACOS; ++ index )
		{
			uint32						ieeeNumber;
			
			ieeeNumber = (0x7UL << 27) | (index << (27 - K_BITSCOUNT_ACOS));
			
			float						realNumber;
			
			realNumber = *((float*)&ieeeNumber);
			gTable_Acos[index] = (float) ::acosf (1.F - realNumber );
		}
	}

}

#endif // USE_APPROX



const float	su::Pi				= 3.141592654f;
const float	su::HalfPi			= 1.570796326f;
const float	su::TwoPi			= 6.283185307f;
const float	su::OOPi			= 0.318309886f;
const float	su::FpEpsilon		= 1.192092896e-07F;
const float	su::FpMin			= 1.175494351e-38F;
const float	su::FpMax			= 3.402823466e+38F;
const float	su::DegToRad		= 0.01745329252f;
const float	su::RadToDeg		= 57.29577951308f;




//
//	Accepte des valeurs en dehors du domaine de définition
//	Précision : erreur moyenne  0.057%
//				erreur maximale 0.536%	0.24 degree

float su::Acosf( float x )
{
#ifndef USE_APPROX

	return ::acosf( x );

#else

	float							cte_0_00;
	float							cte_1_00;
	float							usedx;

	cte_0_00 = 0.F;
	cte_1_00 = 1.F;

	uint32							value_int;

	usedx = cte_1_00 - Absf(x);
	value_int = (*((uint32*)&usedx)) & ~0x80000000UL;

	float							approx;
	float							value;
	float							derivate;
	float							parameter;
	
	parameter = usedx + usedx - usedx * usedx;
	derivate = Isqrtf(parameter);

	if (value_int < 0x38000000 )
	{
		approx = cte_0_00;
		value = cte_0_00;
		
		derivate += 0.5F * derivate * (cte_1_00 - parameter * derivate * derivate);
	}
	else
	{
		uint32						index;
		
		index = (value_int >> (27 - K_BITSCOUNT_ACOS)) & (K_TABLESIZE_ACOS - 1);
		value_int = (0x7UL << 27) | (index << (27 - K_BITSCOUNT_ACOS));
		
		value = *((float*)&value_int );
		approx = gTable_Acos[index];
	}

	approx += (usedx - value) * derivate;
	
	if (x < cte_0_00 )
	{
		approx = (float) Pi - approx;
	}
	
	return approx;

#endif
}


float su::Asinf( float a )
{
#ifndef USE_APPROX

	return ::asinf( a );

#else

	return ( HalfPi - Acosf(a) );

#endif
}


//
//	Dans le cas ou (x * x + y * y) = 1, il n'y pas de normalisation
//	et la précision des calculs est trés supérieure.
//	Précision : erreur moyenne	0.183%
//				erreur maximale 0.24 degrés

float su::Atan2f( float x, float y )
{
#ifndef USE_APPROX

	return ::atan2f( x, y );
	
#else

	float					cte_0_00;

	cte_0_00 = 0.F;
	if (x == cte_0_00 ) return (float) Pi;

	float					scale;					
	float					norm2;
	
	norm2 = x * x + y * y;
	scale = Isqrtf( norm2 );
	scale += 0.5F * (1.F - norm2 * scale * scale) * scale;
	
	x *= scale;
	y *= scale;
	
	float					value;

	value = Acosf( y );
	return (x < cte_0_00) ? -value : value;

#endif
}

float su::Atanf( float a )
{
#ifndef USE_APPROX

	return ::atanf( a );

#else

	return Atan2f( a, 1.0 );

#endif
}

//	
//	Bon comportement avec les cas particuliers
//		Inf -> Nan
//	Toutefois limité dans l'étendue des valeurs supportées (autour de 10e6)
//	Précision :	erreur moyenne  0.009%
//				erreur maximale 0.117%
float su::Cosf( float a )
{
#ifndef USE_APPROX

	return ::cosf( a );

#else
	
	float						ap;
	
	ap = Absf( a * (float) (K_TABLESIZE_COS / HalfPi) );

	int32						masterIndex;
	
	masterIndex = (int) ap;
	
	float						ratio1, ratio2;
	
	ratio2 = ap - masterIndex;
	ratio1 = 1.F - ratio2;
	
	int							index;
	
	index = (masterIndex & (4 * K_TABLESIZE_COS - 1)) & (K_TABLESIZE_COS - 1);
	
	float						value;
	
	if (masterIndex & K_TABLESIZE_COS )
	{
		index = K_TABLESIZE_COS - index;
		value = ratio1 * gTable_Cos[index] + ratio2 * gTable_Cos[index - 1];
	}
	else
	{
		value = ratio1 * gTable_Cos[index] + ratio2 * gTable_Cos[index + 1];
	}
	
	if ((masterIndex & K_TABLESIZE_COS) ^ ((masterIndex >> 1) & K_TABLESIZE_COS) )
	{
		value = -value;
	}

	return value;

#endif
}

float su::Sinf( float a )
{
#ifndef USE_APPROX

	return ::sinf( a );

#else

	return Cosf( a - HalfPi );

#endif
}

void su::Sincosf( float inA, float * outSin, float * outCos )
{
	if( outSin )	*outSin = Sinf( inA );
	if( outCos )	*outCos = Cosf( inA );
}

float
su::Tanf( float a )
{
#ifndef USE_APPROX

	return ::tanf( a );

#else

	float s1, c1;
	Sincosf( a, &s1, &c1 );
	return ( s1 / c1 );

#endif
}


//
// Erreur maximale de 0.007
// Lissage par approximation polynomiale de la mantisse au troisième ordre

float
su::Log2f( float v )
{
#ifndef USE_APPROX

	return ::logf(v) * 1.44269504F;		// 1/ln(2)

#else

   int32          x = *((int32*)&v);
   const int      logm2 = ((x >> 23) & 255) - 128;
   x &= ~(255 << 23);
   x +=   127 << 23;
   v  = *((float*)&x);
   v  = ((-1.0f/3.0f) * v + 2.0f) * v - (2.0f/3.0f);
   return (logm2 + v);

#endif
}


float
su::Logf( float v )
{
#ifndef USE_APPROX

	return ::logf(v);

#else

	return Log2f(v) * 0.69314718F;		// ln(2)

#endif
}


float
su::Log10f( float v )
{
#ifndef USE_APPROX

	return ::logf(v) * 0.4342944819F;				// 1/ln(10)

#else

	return Log2f(v) * 0.301029995664F;	// ln(2)/ln(10)

#endif
}


//
//	Bon comportement avec les cas particuliers
//		Inf -> valeur proche de 0
//		0 ou dénormalisé -> valeur très grande
//	Non testé avec valeurs négatives.
//	Précision : erreur moyenne 0.095%
//				erreur maximale 0.389% 

float
su::Isqrtf( float a )
{
#ifndef USE_APPROX

	return 1.0f / ::sqrtf( a );

#else

	uint32								ieeeNumber;
	
	ieeeNumber = *((uint32*)&a);
	
	int32								exponent;
	
	exponent = (int32) ((ieeeNumber >> 23) & 0xffUL) - 127;
	
	uint32								index;
	
	index = (ieeeNumber >> (23 - K_BITSCOUNT_INVSQRT)) & (K_TABLESIZE_INVSQRT - 1);

	uint32								resultExponent;
	
	resultExponent = (uint32) (((-exponent) >> 1) + 127);	
	if (index != 0 ) resultExponent -= 1;
		
	ieeeNumber = (resultExponent << 23) | ((uint32) gTable1_InvSqrt[index] << (23 - 8));
	a = * (float *) &ieeeNumber;
	
	return a * gTable2_InvSqrt[exponent & 1];

#endif
}



float
su::Powf( float inV, float inP )
{
	return ::powf( inV, inP );
}


float
su::Sqrtf( float inV )
{
	return ::sqrtf( inV );
}


float
su::Expf( float inA )
{
	return ::expf( inA );
}


int
su::Ftoi( float inV )
{
	return (int)inV;
}


float
su::Itof( int inV )
{
	return (float)inV;
}


float
su::Absf( float inV )
{
	*((unsigned int *)&inV) &= 0x7FFFFFFF;
	return inV;
}


float
su::Fmod(	float x, float y	)
{
	return ( x - float((int)(x/y))*y );
}


float
su::Ceil(	float x		)
{
	int    i = int(x);
	float fi = float(i);
	if( x < 0.0f || x == fi )
		return fi;
	else
		return (fi+1.0f);
}


float
su::Floor( float x	)
{
	int    i = int(x);
	float fi = float(i);
	if( x > 0.0f || x == fi )
		return fi;
	else
		return (fi-1.0f);
}

float
su::Round( float x )
{
	return float( int(x+0.5f) );
}


float
su::Near ( float x, float y, float eps )
{
	float d = Abs( x - y );
	return (d<eps);
}


float su::Asin	(	float x								)	{ return su::Asinf(x);				}
float su::Acos	(	float x								)	{ return su::Acosf(x);				}
float su::Atan	(	float x								)	{ return su::Tanf(x);				}
float su::Atan2	(	float x, float y					)	{ return su::Atan2f(x,y);			}
float su::Sin	(	float a								)	{ return su::Sinf(a);				}
float su::Cos	(	float a								)	{ return su::Cosf(a);				}
void  su::Sincos(	float a, float* outS, float* outC	)	{ su::Sincosf(a,outS,outC);			}
float su::Tan	(	float a								)	{ return su::Tanf(a);				}
float su::Exp	(	float x								)	{ return su::Expf(x);				}
float su::Log	(	float x								)	{ return su::Logf(x);				}
float su::Log2	(	float x								)	{ return su::Log2f(x);				}
float su::Log10	(	float x								)	{ return su::Log2f(x);				}
float su::Pow	(	float x, float y					)	{ return su::Powf(x,y);				}
float su::Isqrt	(	float x								)	{ return su::Isqrtf(x);				}
float su::Sqrt	(	float x								)	{ return su::Sqrtf(x);				}
float su::Abs	(	float x								)	{ return su::Absf(x);				}
float su::Fabs	(	float x								)	{ return su::Absf(x);				}
float su::Fabsf	(	float x								)	{ return su::Absf(x);				}



//--------------------------
// 2D Vector
//--------------------------


const su::Vec2	su::Vec2::ZERO(0.0f,0.0f);
const su::Vec2	su::Vec2::ONE(1.0f,1.0f);
const su::Vec2	su::Vec2::UNIT_X(1.0f,0.0f);
const su::Vec2	su::Vec2::UNIT_Y(0.0f,1.0f);
const su::Vec2	su::Vec2::POSMIN(FpMin,FpMin);
const su::Vec2	su::Vec2::POSMAX(FpMax,FpMax);
const su::Vec2	su::Vec2::NEGMIN(-FpMin,-FpMin);
const su::Vec2	su::Vec2::NEGMAX(-FpMax,-FpMax);



Vec2::Vec2(	const float * inFP )
{
	x = inFP[0];
	y = inFP[1];
}


Vec2::Vec2(	const Vec2 & inV )
{
	Vec2Copy( this, &inV );
}


Vec2::Vec2(	const Vec3 & inV )
{
	x = inV.x;
	y = inV.y;
}


Vec2::Vec2(	const Vec4 & inV )
{
	x = inV.x;
	y = inV.y;
}


Vec2::Vec2(	const Quat & inQ )
{
	x = inQ.x;
	y = inQ.y;
}


Vec2::Vec2(	float inX, float inY )
{
	x = inX;
	y = inY;
}


Vec2& Vec2::Set	( float inX, float inY )
{
	x = inX;
	y = inY;
	return *this;
}


Vec2& Vec2::operator = ( const Vec2 & inV )
{
	Vec2Copy( this, &inV );
	return *this;
}


Vec2& Vec2::operator += (	const Vec2 & inV )
{
	Vec2Add( this, this, &inV );
	return *this;
}


Vec2& Vec2::operator -= ( const Vec2 & inV )
{
	Vec2Sub( this, this, &inV );
	return *this;
}


Vec2& Vec2::operator *= ( float inF )
{
	Vec2Scale( this, this, inF );
	return *this;
}


Vec2& Vec2::operator /= (	float inF )
{
	SU_BREAK_IF( inF == 0.0f );
	float ooF = 1.0f / inF;
	Vec2Scale( this, this, ooF );
	return *this;
}


Vec2 Vec2::operator + (	) const
{
	return *this;
}


Vec2 Vec2::operator - (	) const
{
	return Vec2	( -x, -y );
}


Vec2 Vec2::operator + (	const Vec2 & inV ) const
{
	return Vec2	( x+inV.x, y+inV.y );
}


Vec2 Vec2::operator - (	const Vec2 & inV ) const
{
	return Vec2	( x-inV.x, y-inV.y );
}


float Vec2::operator * ( const Vec2 & inV ) const
{
	return Vec2Dot( this, (const Vec2*)&inV );
}


float Vec2::operator ^ ( const Vec2 & inV ) const
{
	return Vec2Cross( this, (const Vec2*)&inV );
}


Vec2 Vec2::operator * (	float inF ) const
{
	return Vec2	( x*inF, y*inF );
}


Vec2 Vec2::operator / (	float inF ) const
{
	SU_BREAK_IF( inF == 0.0f );
	float ooF = 1.0f / inF;
	return Vec2( x*ooF, y*ooF );
}


bool Vec2::operator == ( const Vec2 & inV ) const
{
	return ( (x==inV.x) && (y==inV.y) );
}


bool Vec2::operator != ( const Vec2 & inV ) const
{
	return ( (x!=inV.x) || (y!=inV.y) );
}


bool Vec2::isnear (	const Vec2 & inRef, float d ) const
{
	return Vec2Near( this, &inRef, d );
}


Vec2 Vec2::swizzle ( uint32 arr ) const
{
	int    i = ((arr>>0)&15)-'x';
	int    j = ((arr>>4)&15)-'x';
	const float* a = &x;
	return Vec2( a[j], a[i] );
}


#if !defined(VEC2_MUL_FLOAT_VEC2)
Vec2 su::operator * ( float inF, const Vec2 & inV	)
{
	return Vec2	( inV.x*inF, inV.y*inF );
}
#endif

#if !defined(VEC2_COPY)
su::Vec2*
su::Vec2Copy ( Vec2* outV, const Vec2* inV )
{
	if( outV == inV )
		return outV;

	outV->x = inV->x;
	outV->y = inV->y;
	return outV;
}
#endif

#if !defined(VEC2_ZERO)
su::Vec2* su::Vec2Zero ( Vec2* outV )
{
	outV->x = outV->y = 0;
	return outV;
}
#endif

#if !defined(VEC2_NORM)
float su::Vec2Norm ( const Vec2* inV )
{
	return Sqrtf( inV->x * inV->x + inV->y * inV->y );
}
#endif

#if !defined(VEC2_NORM2)
float su::Vec2Norm2 ( const Vec2* inV )
{
	float xx = inV->x * inV->x;
	float yy = inV->y * inV->y;
	float d  = xx + yy;
	return d;
}
#endif

#if !defined(VEC2_NORMALIZE)
su::Vec2* su::Vec2Normalize ( Vec2* outV, const Vec2* inV )
{
	float n = Vec2Norm( inV );
	SU_ASSERT( n != 0.f );
	float oon = 1.0f/n;
	outV->x = inV->x * oon;
	outV->y = inV->y * oon;
	return outV;
}
#endif

#if !defined(VEC2_CROSS)
float su::Vec2Cross ( const Vec2* inV0, const Vec2* inV1 )
{
	return ( inV0->x * inV1->y - inV0->y * inV1->x );
}
#endif

#if !defined(VEC2_DOT)
float su::Vec2Dot ( const Vec2* inV0, const Vec2* inV1 )
{
	float xx = inV0->x * inV1->x;
	float yy = inV0->y * inV1->y;
	float d  = xx + yy;
	return d;
}
#endif

#if !defined(VEC2_NEAR)
bool su::Vec2Near ( const Vec2* inV0, const Vec2* inV1, float d )
{
	Vec2 s;
	float ds = Vec2Norm2( Vec2Sub(&s,inV0,inV1) );
	return (ds<FpMin) || (ds<=(d*d));
}
#endif

#if !defined(VEC2_ADD)
su::Vec2* su::Vec2Add ( Vec2* outV, const Vec2* inV0, const Vec2* inV1 )
{
	outV->x = inV0->x + inV1->x;
	outV->y = inV0->y + inV1->y;
	return outV;
}
#endif

#if !defined(VEC2_SUB)
su::Vec2* su::Vec2Sub ( Vec2* outV, const Vec2* inV0, const Vec2* inV1 )
{
	outV->x = inV0->x - inV1->x;
	outV->y = inV0->y - inV1->y;
	return outV;
}
#endif

#if !defined(VEC2_MUL)
su::Vec2* su::Vec2Mul ( Vec2* outV, const Vec2* inV0, const Vec2* inV1 )
{
	outV->x = inV0->x * inV1->x;
	outV->y = inV0->y * inV1->y;
	return outV;
}
#endif

#if !defined(VEC2_MIN)
su::Vec2* su::Vec2Min ( Vec2* outV, const Vec2* inV0, const Vec2* inV1 )
{
	outV->x = Min( inV0->x , inV1->x );
	outV->y = Min( inV0->y , inV1->y );
	return outV;
}
#endif

#if !defined(VEC2_MAX)
su::Vec2* su::Vec2Max ( Vec2* outV, const Vec2* inV0, const Vec2* inV1 )
{
	outV->x = Max( inV0->x , inV1->x );
	outV->y = Max( inV0->y , inV1->y );
	return outV;
}
#endif

#if !defined(VEC2_SCALE)
su::Vec2* su::Vec2Scale ( Vec2* outV, const Vec2* inV,  float inF )
{
	outV->x = inV->x * inF;
	outV->y = inV->y * inF;
	return outV;
}
#endif

#if !defined(VEC2_LERP)
su::Vec2* su::Vec2Lerp ( Vec2* outV, const Vec2* inV0, const Vec2* inV1, float inF )
{
	outV->x = inV0->x + inF * ( inV1->x - inV0->x );
	outV->y = inV0->y + inF * ( inV1->y - inV0->y );
	return outV;
}
#endif

#if !defined(VEC2_APPLY)
su::Vec2* su::Vec2Apply ( Vec2* outV, const Vec2* inV,  const Matrix* inM )
{
	SU_CC_ALIGNED128( Vec4, v );
	v.x = inV->x;
	v.y = inV->y;
	v.z = 0.f;
	v.w = 1.f;
	Vec4Apply( &v, &v, inM );
	outV->x = v.x;
	outV->y = v.y;
	return outV;
}
#endif

#if !defined(VEC2_APPLYVECTOR)
su::Vec2* su::Vec2ApplyVector ( Vec2* outV, const Vec2* inV,  const Matrix* inM )
{
	Vec4 v( inV->x, inV->y, 0.f, 0.f );
	Vec4Apply( &v, &v, inM );
	outV->x = v.x;
	outV->y = v.y;
	return outV;
}
#endif



//--------------------------
// 3D Vector
//--------------------------


const su::Vec3	su::Vec3::ZERO(0.0f,0.0f,0.0f);
const su::Vec3	su::Vec3::ONE(1.0f,1.0f,1.0f);
const su::Vec3	su::Vec3::UNIT_X(1.0f,0.0f,0.0f);
const su::Vec3	su::Vec3::UNIT_Y(0.0f,1.0f,0.0f);
const su::Vec3	su::Vec3::UNIT_Z(0.0f,0.0f,1.0f);
const su::Vec3	su::Vec3::POSMIN(FpMin,FpMin,FpMin);
const su::Vec3	su::Vec3::POSMAX(FpMax,FpMax,FpMax);
const su::Vec3	su::Vec3::NEGMIN(-FpMin,-FpMin,-FpMin);
const su::Vec3	su::Vec3::NEGMAX(-FpMax,-FpMax,-FpMax);



Vec3::Vec3(	const float * inFP )
{
	x = inFP[0];
	y = inFP[1];
	z = inFP[2];
}


Vec3::Vec3(	const Vec3 & inV )
{
	Vec3Copy( this, &inV );
}


Vec3::Vec3(	const Vec2 & inV )
{
	x = inV.x;
	y = inV.y;
	z = 0.0f;
}


Vec3::Vec3(	const Vec4 & inV )
{
	x = inV.x;
	y = inV.y;
	z = inV.z;
}


Vec3::Vec3(	const Quat & inQ )
{
	x = inQ.x;
	y = inQ.y;
	z = inQ.z;
}


Vec3::Vec3(	float inX, float inY, float inZ )
{
	x = inX;
	y = inY;
	z = inZ;
}


Vec3& Vec3::Set	( float inX, float inY, float inZ )
{
	x = inX;
	y = inY;
	z = inZ;
	return *this;
}


Vec3& Vec3::operator = ( const Vec3 & inV )
{
	Vec3Copy( this, &inV );
	return *this;
}


Vec3& Vec3::operator += ( const Vec3 & inV )
{
	Vec3Add( this, this, &inV );
	return *this;
}


Vec3& Vec3::operator -= ( const Vec3 & inV )
{
	Vec3Sub( this, this, &inV );
	return *this;
}


Vec3& Vec3::operator *=	( const Matrix & inM )
{
	Vec3Apply( this, this, &inM );
	return *this;
}


Vec3& Vec3::operator *=	( const Quat & inQ )
{
	Vec3Apply( this, this, &inQ );
	return *this;
}


Vec3& Vec3::operator *= ( float inF )
{
	Vec3Scale( this, this, inF );
	return *this;
}


Vec3& Vec3::operator /= ( float inF )
{
	SU_BREAK_IF( inF == 0.0f );
	float ooF = 1.0f / inF;
	Vec3Scale( this, this, ooF );
	return *this;
}


Vec3 Vec3::operator + (	) const
{
	return *this;
}


Vec3 Vec3::operator - (	) const
{
	return Vec3	( -x, -y, -z );
}


Vec3 Vec3::operator + (	const Vec3 & inV ) const
{
	Vec3 v;
	Vec3Add( &v, this, &inV );
	return v;
}


Vec3 Vec3::operator - (	const Vec3 & inV ) const
{
	Vec3 v;
	Vec3Sub( &v, this, &inV );
	return v;
}


Vec3 Vec3::operator ^ (	const Vec3 & inV ) const
{
	Vec3 v;
	Vec3Cross( &v, this, &inV );
	return v;
}


float Vec3::operator * (	const Vec3 & inV ) const
{
	return Vec3Dot( this, &inV );
}


Vec3 Vec3::operator * (	const Matrix & inM	) const
{
	Vec3 v;
	Vec3Apply( &v, this, &inM );
	return v;
}


Vec3 Vec3::operator * (	const Quat & inQ ) const
{
	Vec3 v;
	Vec3Apply( &v, this, &inQ );
	return v;
}


Vec3 Vec3::operator * (	float inF ) const
{
	Vec3 v;
	Vec3Scale( &v, this, inF );
	return v;
}


Vec3 Vec3::operator / (	float inF ) const
{
	SU_BREAK_IF( inF == 0.0f );
	float ooF = 1.0f / inF;
	return Vec3( x*ooF, y*ooF, z*ooF );
}


bool Vec3::operator == ( const Vec3 & inV ) const
{
	return ( x==inV.x && y==inV.y && z==inV.z );
}


bool Vec3::operator != ( const Vec3 & inV ) const
{
	return ( x!=inV.x || y!=inV.y || z!=inV.z );
}


bool Vec3::isnear (	const Vec3 & inRef, float d ) const
{
	return Vec3Near( this, &inRef, d );
}



Vec3 Vec3::swizzle ( uint32 arr ) const
{
	int    i = ((arr>>0)&15)-'x';
	int    j = ((arr>>4)&15)-'x';
	int    k = ((arr>>8)&15)-'x';
	const float* a = &x;
	return Vec3( a[k], a[j], a[i] );
}


#if !defined(VEC3_FLOAT_VEC3)
Vec3 su::operator * ( float inF, const Vec3 & inV	)
{
	Vec3 v;
	Vec3Scale( &v, &inV, inF );
	return v;
}
#endif


#if !defined(VEC3_COPY)
su::Vec3* su::Vec3Copy ( Vec3* outV, const Vec3* inV )
{
	if( outV == inV )
		return outV;
	outV->x = inV->x;
	outV->y = inV->y;
	outV->z = inV->z;
	return outV;
}
#endif

#if !defined(VEC3_ZERO)
su::Vec3* su::Vec3Zero ( Vec3* outV )
{
	outV->x = outV->y = outV->z = 0;
	return outV;
}
#endif

#if !defined(VEC3_NORM)
float su::Vec3Norm ( const Vec3* inV )
{
	return Sqrtf( Vec3Dot(inV,inV) );
}
#endif

#if !defined(VEC3_NORM2)
float su::Vec3Norm2 ( const Vec3* inV )
{
	return Vec3Dot(inV,inV);
}
#endif

#if !defined(VEC3_NORMALIZE)
su::Vec3* su::Vec3Normalize ( Vec3* outV, const Vec3* inV )
{
	float n = Vec3Norm( inV );
	SU_ASSERT( n != 0.f );
	float oon = 1.f / n;
	return Vec3Scale( outV, inV, oon );
}
#endif

#if !defined(VEC3_CROSS)
su::Vec3* su::Vec3Cross ( Vec3* outV, const Vec3* inV0, const Vec3* inV1 )
{
	Vec3 tmp;
	tmp.x = inV0->y*inV1->z - inV0->z*inV1->y;
	tmp.y = inV0->z*inV1->x - inV0->x*inV1->z;
	tmp.z = inV0->x*inV1->y - inV0->y*inV1->x;
	return Vec3Copy( outV, &tmp );
}
#endif

#if !defined(VEC3_DOT)
float su::Vec3Dot ( const Vec3* inV0, const Vec3* inV1 )
{
	return ( inV0->x * inV1->x + inV0->y * inV1->y + inV0->z * inV1->z );
}
#endif

#if !defined(VEC3_NEAR)
bool su::Vec3Near ( const Vec3* inV0, const Vec3* inV1, float d )
{
	Vec3 s;
	float ds = Vec3Norm2( Vec3Sub(&s,inV0,inV1) );
	return ds <= (d*d);
}
#endif

#if !defined(VEC3_ADD)
su::Vec3* su::Vec3Add ( Vec3* outV, const Vec3* inV0, const Vec3* inV1 )
{
	outV->x = inV0->x + inV1->x;
	outV->y = inV0->y + inV1->y;
	outV->z = inV0->z + inV1->z;
	return outV;
}
#endif

#if !defined(VEC3_SUB)
su::Vec3* su::Vec3Sub ( Vec3* outV, const Vec3* inV0, const Vec3* inV1 )
{
	outV->x = inV0->x - inV1->x;
	outV->y = inV0->y - inV1->y;
	outV->z = inV0->z - inV1->z;
	return outV;
}
#endif

#if !defined(VEC3_MUL)
su::Vec3* su::Vec3Mul ( Vec3* outV, const Vec3* inV0, const Vec3* inV1 )
{
	outV->x = inV0->x * inV1->x;
	outV->y = inV0->y * inV1->y;
	outV->z = inV0->z * inV1->z;
	return outV;
}
#endif

#if !defined(VEC3_MIN)
su::Vec3* su::Vec3Min ( Vec3* outV, const Vec3* inV0, const Vec3* inV1 )
{
	outV->x = Min( inV0->x , inV1->x );
	outV->y = Min( inV0->y , inV1->y );
	outV->z = Min( inV0->z , inV1->z );
	return outV;
}
#endif

#if !defined(VEC3_MAX)
su::Vec3* su::Vec3Max ( Vec3* outV, const Vec3* inV0, const Vec3* inV1 )
{
	outV->x = Max( inV0->x , inV1->x );
	outV->y = Max( inV0->y , inV1->y );
	outV->z = Max( inV0->z , inV1->z );
	return outV;
}
#endif

#if !defined(VEC3_SCALE)
su::Vec3* su::Vec3Scale ( Vec3* outV, const Vec3* inV,  float inF )
{
	outV->x = inV->x * inF;
	outV->y = inV->y * inF;
	outV->z = inV->z * inF;
	return outV;
}
#endif

#if !defined(VEC3_LERP)
su::Vec3* su::Vec3Lerp ( Vec3* outV, const Vec3* inV0, const Vec3* inV1, float inF )
{
	outV->x = inV0->x + inF * ( inV1->x - inV0->x );
	outV->y = inV0->y + inF * ( inV1->y - inV0->y );
	outV->z = inV0->z + inF * ( inV1->z - inV0->z );
	return outV;
}
#endif

#if !defined(VEC3_APPLY_VEC3_VEC3_QUAT)
su::Vec3* su::Vec3Apply ( Vec3* outV, const Vec3* inV, const Quat* inQ )
{
	// V' = Q * Quat(V,0) * Q^-1
	Quat r, v( inV->x, inV->y, inV->z, 0.0f );
	QuatMul( &r, inQ, QuatMul( &r, &v, QuatFastInverse( &r, inQ ) ) );
	outV->x = r.x;
	outV->y = r.y;
	outV->z = r.z;
	return outV;
}
#endif

#if !defined(VEC3_APPLY_VEC3_VEC3_MAT)
su::Vec3* su::Vec3Apply ( Vec3* outV, const Vec3* inV, const Matrix* inM )
{
	Vec3 tmp;
	tmp.x = inV->x * inM->m11 + inV->y * inM->m21 + inV->z * inM->m31 + inM->m41;
	tmp.y = inV->x * inM->m12 + inV->y * inM->m22 + inV->z * inM->m32 + inM->m42;
	tmp.z = inV->x * inM->m13 + inV->y * inM->m23 + inV->z * inM->m33 + inM->m43;
	return Vec3Copy( outV, &tmp );
}
#endif

#if !defined(VEC3_APPLY_VEC3_VEC3_MAT_FLOAT)
su::Vec3* su::Vec3Apply ( Vec3* outV, const Vec3* inV,  const Matrix* inM, float inF )
{
	Vec3 tmp;
	tmp.x = ( inV->x * inM->m11 + inV->y * inM->m21 + inV->z * inM->m31 + inM->m41 ) * inF;
	tmp.y = ( inV->x * inM->m12 + inV->y * inM->m22 + inV->z * inM->m32 + inM->m42 ) * inF;
	tmp.z = ( inV->x * inM->m13 + inV->y * inM->m23 + inV->z * inM->m33 + inM->m43 ) * inF;
	return Vec3Copy( outV, &tmp );
}
#endif

#if !defined(VEC3_APPLYVECTOR_VEC3_VEC3_MAT)
su::Vec3* su::Vec3ApplyVector ( Vec3* outV, const Vec3* inV, const Matrix* inM )
{
	Vec3 tmp;
	tmp.x = inV->x * inM->m11 + inV->y * inM->m21 + inV->z * inM->m31;
	tmp.y = inV->x * inM->m12 + inV->y * inM->m22 + inV->z * inM->m32;
	tmp.z = inV->x * inM->m13 + inV->y * inM->m23 + inV->z * inM->m33;
	return Vec3Copy( outV, (const Vec3*)&tmp );
}
#endif

#if !defined(VEC3_APPLYVECTOR_VEC3_VEC3_MAT_FLOAT)
su::Vec3* su::Vec3ApplyVector ( Vec3* outV, const Vec3* inV,  const Matrix* inM, float inF )
{
	Vec3 tmp;
	tmp.x = ( inV->x * inM->m11 + inV->y * inM->m21 + inV->z * inM->m31 ) * inF;
	tmp.y = ( inV->x * inM->m12 + inV->y * inM->m22 + inV->z * inM->m32 ) * inF;
	tmp.z = ( inV->x * inM->m13 + inV->y * inM->m23 + inV->z * inM->m33 ) * inF;
	return Vec3Copy( outV, &tmp );
}
#endif

#if !defined(VEC3_BLEND_VEC3_VEC3_MAT)
su::Vec3* su::Vec3Blend ( Vec3* outV, const Vec3* inV,  const Matrix* inM )
{
	SU_CC_ALIGNED128( Vec3, tmp );
	return Vec3Add( outV, outV, Vec3Apply(&tmp,inV,inM) );
}
#endif

#if !defined(VEC3_BLEND_VEC3_VEC3_MAT_FLOAT)
su::Vec3* su::Vec3Blend ( Vec3* outV, const Vec3* inV,  const Matrix* inM, float inF )
{
	SU_CC_ALIGNED128( Vec3, tmp );
	return Vec3Add( outV, outV, Vec3Apply(&tmp,inV,inM,inF) );
}
#endif

#if !defined(VEC3_BLENDVECTOR_VEC3_VEC3_MAT)
su::Vec3* su::Vec3BlendVector ( Vec3* outV, const Vec3* inV,  const Matrix* inM )
{
	SU_CC_ALIGNED128( Vec3, tmp );
	return Vec3Add( outV, outV, Vec3ApplyVector(&tmp,inV,inM) );
}
#endif

#if !defined(VEC3_BLENDVECTOR_VEC3_VEC3_MAT_FLOAT)
su::Vec3* su::Vec3BlendVector ( Vec3* outV, const Vec3* inV,  const Matrix* inM, float inF )
{
	SU_CC_ALIGNED128( Vec3, tmp );
	return Vec3Add( outV, outV, Vec3ApplyVector(&tmp,inV,inM,inF) );
}
#endif

#if !defined(VEC3_GETBASEFROMBZ)
void su::Vec3GetBaseFromBZ ( Vec3* outBX, Vec3* outBY, Vec3* outBZ, const Vec3* inBZ )
{
	SU_CC_ALIGNED128( Vec3, BX );
	SU_CC_ALIGNED128( Vec3, BY );
	SU_CC_ALIGNED128( Vec3, BZ );

	Vec3Normalize( &BZ, inBZ );

	if( Absf(BZ.y) < 0.8f ) {
		
		BX = Vec3(0,1,0) ^ BZ;
		BY = BZ ^ BX;
	} else {
		BY = BZ ^ Vec3(1,0,0);
		BX = BY ^ BZ;
	}

	if( outBX )
		Vec3Normalize( outBX, &BX );
	if( outBY )
		Vec3Normalize( outBY, &BY );
	if( outBZ )
		*outBZ = BZ;
}
#endif

#if !defined(VEC3_GETBASEFROMBX)
void su::Vec3GetBaseFromBX ( Vec3* outBX, Vec3* outBY, Vec3* outBZ, const Vec3* inBX )
{
	Vec3GetBaseFromBZ( outBY, outBZ, outBX, inBX );
}
#endif

#if !defined(VEC3_GETBASEFROMBY)
void su::Vec3GetBaseFromBY ( Vec3* outBX, Vec3* outBY, Vec3* outBZ, const Vec3* inBY )
{
	Vec3GetBaseFromBZ( outBX, outBZ, outBY, inBY );
	if( outBZ )
		*outBZ = - *outBZ;
}
#endif







//--------------------------
// 4D Vector
//--------------------------



const su::Vec4	su::Vec4::ZERO(0.0f,0.0f,0.0f,0.0f);
const su::Vec4	su::Vec4::ONE(1.0f,1.0f,1.0f,1.0f);
const su::Vec4	su::Vec4::UNIT_X(1.0f,0.0f,0.0f,0.0f);
const su::Vec4	su::Vec4::UNIT_Y(0.0f,1.0f,0.0f,0.0f);
const su::Vec4	su::Vec4::UNIT_Z(0.0f,0.0f,1.0f,0.0f);
const su::Vec4	su::Vec4::UNIT_W(0.0f,0.0f,0.0f,1.0f);
const su::Vec4	su::Vec4::POSMIN(FpMin,FpMin,FpMin,FpMin);
const su::Vec4	su::Vec4::POSMAX(FpMax,FpMax,FpMax,FpMax);
const su::Vec4	su::Vec4::NEGMIN(-FpMin,-FpMin,-FpMin,-FpMin);
const su::Vec4	su::Vec4::NEGMAX(-FpMax,-FpMax,-FpMax,-FpMax);




Vec4::Vec4(	const float * inFP )
{
	x = inFP[0];
	y = inFP[1];
	z = inFP[2];
	w = inFP[3];
}


Vec4::Vec4(	const Vec4 & inV )
{
	Vec4Copy( this, &inV );
}


Vec4::Vec4(	const Vec3 & inV )
{
	x = inV.x;
	y = inV.y;
	z = inV.z;
	w = 1.0f;
}


Vec4::Vec4(	const Vec2 & inV )
{
	x = inV.x;
	y = inV.y;
	z = 0.0f;
	w = 1.0f;
}


Vec4::Vec4(	const Quat & inQ )
{
	x = inQ.x;
	y = inQ.y;
	z = inQ.z;
	w = inQ.w;
}


Vec4::Vec4(	float inX, float inY, float inZ, float inW )
{
	x = inX;
	y = inY;
	z = inZ;
	w = inW;
}


Vec4& Vec4::Set	( float inX,	float inY, float inZ, float inW	)
{
	x = inX;
	y = inY;
	z = inZ;
	w = inW;
	return *this;
}


Vec4& Vec4::operator = ( const Vec4 & inV )
{
	Vec4Copy( this, &inV );
	return *this;
}


Vec4& Vec4::operator += ( const Vec4 & inV )
{
	Vec4Add( this, this, &inV );
	return *this;
}


Vec4& Vec4::operator -= ( const Vec4 & inV )
{
	Vec4Sub( this, this, &inV );
	return *this;
}


Vec4& Vec4::operator *=	( const Matrix & inM )
{
	Vec4Apply( this, this, &inM );
	return *this;
}


Vec4& Vec4::operator *=	( const Quat & inQ )
{
	Vec4Apply( this, this, &inQ );
	return *this;
}


Vec4& Vec4::operator *= ( float inF )
{
	Vec4Scale( this, this, inF );
	return *this;
}


Vec4& Vec4::operator /= ( float inF )
{
	SU_BREAK_IF( inF == 0.0f );
	float ooF = 1.0f / inF;
	Vec4Scale( this, this, ooF );
	return *this;
}


Vec4 Vec4::operator + ( ) const
{
	return *this;
}


Vec4 Vec4::operator - ( ) const
{
	return Vec4	( -x, -y, -z, -w );
}


Vec4 Vec4::operator + ( const Vec4 & inV ) const
{
	return Vec4	( x+inV.x, y+inV.y, z+inV.z, w+inV.w );
}


Vec4 Vec4::operator - ( const Vec4 & inV ) const
{
	return Vec4	( x-inV.x, y-inV.y, z-inV.z, w-inV.w );
}


float Vec4::operator * ( const Vec4 & inV ) const
{
	return Vec4Dot( this, &inV );
}


Vec4 Vec4::operator * ( const Matrix & inM ) const
{
	Vec4 v;
	Vec4Apply( &v, this, &inM );
	return v;
}


Vec4 Vec4::operator * ( const Quat & inQ ) const
{
	Vec4 v;
	Vec4Apply( &v, this, &inQ );
	return v;
}


Vec4 Vec4::operator * ( float inF ) const
{
	return Vec4	( x*inF, y*inF, z*inF, w*inF );
}


Vec4 Vec4::operator / ( float inF ) const
{
	SU_BREAK_IF( inF == 0.0f );
	float ooF = 1.0f / inF;
	return Vec4( x*ooF, y*ooF, z*ooF, w*ooF );
}


bool Vec4::operator == ( const Vec4 & inV ) const
{
	return ( (x==inV.x) && (y==inV.y) && (z==inV.z) && (w==inV.w) );
}


bool Vec4::operator != ( const Vec4 & inV ) const
{
	return ( (x!=inV.x) || (y!=inV.y) || (z!=inV.z) || (w!=inV.w) );
}


bool Vec4::isnear ( const Vec4 & inRef, float d ) const
{
	return Vec4Near( this, &inRef, d );
}


uint Vec4::clip	( )	const
{
	return Vec4Clip( this );
}


Vec4 Vec4::swizzle ( uint32 arr ) const
{
	int    i = ((arr>>0)&15)-'x';
	int    j = ((arr>>4)&15)-'x';
	int    k = ((arr>>8)&15)-'x';
	int    l = ((arr>>12)&15)-'x';
	const float* a = &x;
	return Vec4( a[l], a[k], a[j], a[i] );
}


Vec2 Vec4::xy ( ) const
{
	return Vec2(x,y);
}


Vec2 Vec4::zw ( ) const
{
	return Vec2(z,w);
}


Vec2 Vec4::xz ( ) const
{
	return Vec2(x,z);
}


Vec2 Vec4::yw ( ) const
{
	return Vec2(y,w);
}


#if !defined(VEC4_MUL_FLOAT_VEC4)
Vec4 su::operator * ( float inF, const Vec4 & inV )
{
	return Vec4( inV.x*inF, inV.y*inF, inV.z*inF, inV.w*inF	);
}
#endif

#if !defined(VEC4_COPY)
su::Vec4* su::Vec4Copy ( Vec4* outV, const Vec4* inV )
{
	if( outV == inV )
		return outV;

	outV->x = inV->x;
	outV->y = inV->y;
	outV->z = inV->z;
	outV->w = inV->w;
	return outV;
}
#endif

#if !defined(VEC4_ZERO)
su::Vec4* su::Vec4Zero ( Vec4* outV )
{
	outV->x = outV->y = outV->z = outV->w = 0;
	return outV;
}
#endif

#if !defined(VEC4_NORM)
float su::Vec4Norm ( const Vec4* inV )
{
	return Sqrtf( Vec4Dot(inV,inV) );
}
#endif

#if !defined(VEC4_NORM2)
float su::Vec4Norm2 ( const Vec4* inV )
{
	return Vec4Dot( inV, inV );
}
#endif

#if !defined(VEC4_NORMALIZE)
su::Vec4* su::Vec4Normalize ( Vec4* outV, const Vec4* inV )
{
	float n	= Vec4Norm( inV );
	SU_ASSERT( n != 0.f );
	float oon = 1.f / n;
	return Vec4Scale( outV, inV, oon );
}
#endif

#if !defined(VEC4_CROSS)
su::Vec4* su::Vec4Cross ( Vec4* outV, const Vec4* inV0, const Vec4* inV1, const Vec4* inV2 )
{
	// "http://research.microsoft.com/~hollasch/thesis/chapter2.html"
	float A, B, C, D, E, F;
	A = (inV1->x * inV2->y) - (inV1->y * inV2->x);
	B = (inV1->x * inV2->z) - (inV1->z * inV2->x);
	C = (inV1->x * inV2->w) - (inV1->w * inV2->x);
	D = (inV1->y * inV2->z) - (inV1->z * inV2->y);
	E = (inV1->y * inV2->w) - (inV1->w * inV2->y);
	F = (inV1->z * inV2->w) - (inV1->w * inV2->z);

	Vec4 tmp;
	tmp.x =   (inV0->y * F) - (inV0->z * E) + (inV0->w * D);
	tmp.y = - (inV0->x * F) + (inV0->z * C) - (inV0->w * B);
	tmp.z =   (inV0->x * E) - (inV0->y * C) + (inV0->w * A);
	tmp.w = - (inV0->x * D) + (inV0->y * B) - (inV0->z * A);
	return Vec4Copy( outV, &tmp );
}
#endif

#if !defined(VEC4_DOT)
float su::Vec4Dot ( const Vec4* inV0, const Vec4* inV1 )
{
	return (	inV0->x * inV1->x
			+	inV0->y * inV1->y
			+	inV0->z * inV1->z
			+	inV0->w * inV1->w	);
}
#endif

#if !defined(VEC4_NEAR)
bool su::Vec4Near ( const Vec4* inV0, const Vec4* inV1, float d )
{
	Vec4 s;
	float ds = Vec4Norm2( Vec4Sub(&s,inV0,inV1) );
	return ds <= (d*d);
}
#endif

#if !defined(VEC4_ADD)
su::Vec4* su::Vec4Add ( Vec4* outV, const Vec4* inV0, const Vec4* inV1 )
{
	outV->x = inV0->x + inV1->x;
	outV->y = inV0->y + inV1->y;
	outV->z = inV0->z + inV1->z;
	outV->w = inV0->w + inV1->w;
	return outV;
}
#endif

#if !defined(VEC4_SUB)
su::Vec4* su::Vec4Sub ( Vec4* outV, const Vec4* inV0, const Vec4* inV1 )
{
	outV->x = inV0->x - inV1->x;
	outV->y = inV0->y - inV1->y;
	outV->z = inV0->z - inV1->z;
	outV->w = inV0->w - inV1->w;
	return outV;
}
#endif

#if !defined(VEC4_MUL)
su::Vec4* su::Vec4Mul ( Vec4* outV, const Vec4* inV0, const Vec4* inV1 )
{
	outV->x = inV0->x * inV1->x;
	outV->y = inV0->y * inV1->y;
	outV->z = inV0->z * inV1->z;
	outV->w = inV0->w * inV1->w;
	return outV;
}
#endif

#if !defined(VEC4_MIN)
su::Vec4* su::Vec4Min ( Vec4* outV, const Vec4* inV0, const Vec4* inV1 )
{
	outV->x = Min( inV0->x , inV1->x );
	outV->y = Min( inV0->y , inV1->y );
	outV->z = Min( inV0->z , inV1->z );
	outV->w = Min( inV0->w , inV1->w );
	return outV;
}
#endif

#if !defined(VEC4_MAX)
su::Vec4* su::Vec4Max ( Vec4* outV, const Vec4* inV0, const Vec4* inV1 )
{
	outV->x = Max( inV0->x , inV1->x );
	outV->y = Max( inV0->y , inV1->y );
	outV->z = Max( inV0->z , inV1->z );
	outV->w = Max( inV0->w , inV1->w );
	return outV;
}
#endif

#if !defined(VEC4_SCALE)
su::Vec4* su::Vec4Scale ( Vec4* outV, const Vec4* inV,  float inF )
{
	outV->x = inV->x * inF;
	outV->y = inV->y * inF;
	outV->z = inV->z * inF;
	outV->w = inV->w * inF;
	return outV;
}
#endif

#if !defined(VEC4_LERP)
su::Vec4* su::Vec4Lerp ( Vec4* outV, const Vec4* inV0, const Vec4* inV1, float inF )
{
	outV->x = inV0->x + inF * ( inV1->x - inV0->x );
	outV->y = inV0->y + inF * ( inV1->y - inV0->y );
	outV->z = inV0->z + inF * ( inV1->z - inV0->z );
	outV->w = inV0->w + inF * ( inV1->w - inV0->w );
	return outV;
}
#endif

#if !defined(VEC4_APPLY_VEC4_VEC4_MAT)
su::Vec4* su::Vec4Apply ( Vec4* outV, const Vec4* inV, const Matrix* inM )
{
	Vec4 tmp;
	tmp.x = inV->x * inM->m11 + inV->y * inM->m21 + inV->z * inM->m31 + inV->w * inM->m41;
	tmp.y = inV->x * inM->m12 + inV->y * inM->m22 + inV->z * inM->m32 + inV->w * inM->m42;
	tmp.z = inV->x * inM->m13 + inV->y * inM->m23 + inV->z * inM->m33 + inV->w * inM->m43;
	tmp.w = inV->x * inM->m14 + inV->y * inM->m24 + inV->z * inM->m34 + inV->w * inM->m44;
	return Vec4Copy( outV, &tmp );
}
#endif

#if !defined(VEC4_APPLY_VEC4_VEC4_MAT_FLOAT)
su::Vec4* su::Vec4Apply ( Vec4* outV, const Vec4* inV, const Matrix* inM, float inF )
{
	Vec4 tmp;
	tmp.x = ( inV->x * inM->m11 + inV->y * inM->m21 + inV->z * inM->m31 + inV->w * inM->m41 ) * inF;
	tmp.y = ( inV->x * inM->m12 + inV->y * inM->m22 + inV->z * inM->m32 + inV->w * inM->m42 ) * inF;
	tmp.z = ( inV->x * inM->m13 + inV->y * inM->m23 + inV->z * inM->m33 + inV->w * inM->m43 ) * inF;
	tmp.w = ( inV->x * inM->m14 + inV->y * inM->m24 + inV->z * inM->m34 + inV->w * inM->m44 ) * inF;
	return Vec4Copy( outV, &tmp );
}
#endif

#if !defined(VEC4_APPLY_VEC4_VEC4_QUAT)
su::Vec4* su::Vec4Apply ( Vec4* outV, const Vec4* inV, const Quat* inQ )
{
	// V' = Q * Quat(V) * Q^-1
	Quat r, v( inV->x, inV->y, inV->z, inV->w );
	QuatMul( &r, inQ, QuatMul( &r, &v, QuatFastInverse( &r, inQ ) ) );
	return Vec4Copy( outV, (Vec4*)&r );
}
#endif

#if !defined(VEC4_APPLYVECTOR_VEC4_VEC4_MAT)
su::Vec4* su::Vec4ApplyVector ( Vec4* outV, const Vec4* inV, const Matrix* inM )
{
	Vec4 tmp;
	tmp.x = inV->x * inM->m11 + inV->y * inM->m21 + inV->z * inM->m31;
	tmp.y = inV->x * inM->m12 + inV->y * inM->m22 + inV->z * inM->m32;
	tmp.z = inV->x * inM->m13 + inV->y * inM->m23 + inV->z * inM->m33;
	tmp.w = 1.f;
	return Vec4Copy( outV, &tmp );
}
#endif

#if !defined(VEC4_APPLYVECTOR_VEC4_VEC4_MAT_FLOAT)
su::Vec4* su::Vec4ApplyVector ( Vec4* outV, const Vec4* inV, const Matrix* inM, float inF )
{
	Vec4 tmp;
	tmp.x = ( inV->x * inM->m11 + inV->y * inM->m21 + inV->z * inM->m31 + inV->w ) * inF;
	tmp.y = ( inV->x * inM->m12 + inV->y * inM->m22 + inV->z * inM->m32 + inV->w ) * inF;
	tmp.z = ( inV->x * inM->m13 + inV->y * inM->m23 + inV->z * inM->m33 + inV->w ) * inF;
	tmp.w = 1.f;
	return Vec4Copy( outV, &tmp );
}
#endif

#if !defined(VEC4_BLEND_VEC4_VEC4_MAT)
su::Vec4* su::Vec4Blend ( Vec4* outV, const Vec4* inV, const Matrix* inM )
{
	SU_CC_ALIGNED128( Vec4, tmp );
	return Vec4Add( outV, outV, Vec4Apply(&tmp,inV,inM) );
}
#endif

#if !defined(VEC4_BLEND_VEC4_VEC4_MAT_FLOAT)
su::Vec4* su::Vec4Blend ( Vec4* outV, const Vec4* inV, const Matrix* inM, float inF )
{
	SU_CC_ALIGNED128( Vec4, tmp );
	return Vec4Add( outV, outV, Vec4Apply(&tmp,inV,inM,inF) );
}
#endif

#if !defined(VEC4_BLENDVECTOR_VEC4_VEC4_MAT)
su::Vec4* su::Vec4BlendVector ( Vec4* outV, const Vec4* inV, const Matrix* inM )
{
	SU_CC_ALIGNED128( Vec4, tmp );
	return Vec4Add( outV, outV, Vec4ApplyVector(&tmp,inV,inM) );
}
#endif

#if !defined(VEC4_BLENDVECTOR_VEC4_VEC4_MAT_FLOAT)
su::Vec4* su::Vec4BlendVector ( Vec4* outV, const Vec4* inV, const Matrix* inM, float inF )
{
	SU_CC_ALIGNED128( Vec4, tmp );
	return Vec4Add( outV, outV, Vec4ApplyVector(&tmp,inV,inM,inF) );
}
#endif

#if !defined(VEC4_CLIP)
uint su::Vec4Clip ( const Vec4* inV )
{
	uint cf = 0;
	float w = Fabs( inV->w );
	if( inV->x >  w )		cf |= (1<<0);
	if( inV->x < -w )		cf |= (1<<1);
	if( inV->y >  w )		cf |= (1<<2);
	if( inV->y < -w )		cf |= (1<<3);
	if( inV->z >  w )		cf |= (1<<4);
	if( inV->z < -w )		cf |= (1<<5);
	return cf;
}
#endif











//--------------------------
// Matrix
//--------------------------




const su::Matrix	su::Matrix::ZERO(	0.0f,0.0f,0.0f,0.0f,
										0.0f,0.0f,0.0f,0.0f,
										0.0f,0.0f,0.0f,0.0f,
										0.0f,0.0f,0.0f,0.0f		);

const su::Matrix	su::Matrix::UNIT(	1.0f,0.0f,0.0f,0.0f,
										0.0f,1.0f,0.0f,0.0f,
										0.0f,0.0f,1.0f,0.0f,
										0.0f,0.0f,0.0f,1.0f		);



Matrix::Matrix(	const float * inFP )
{
	m11=inFP[0];  m12=inFP[1];  m13=inFP[2];  m14=inFP[3];
	m21=inFP[4];  m22=inFP[5];  m23=inFP[6];  m24=inFP[7];
	m31=inFP[8];  m32=inFP[9];  m33=inFP[10]; m34=inFP[11];
	m41=inFP[12]; m42=inFP[13]; m43=inFP[14]; m44=inFP[15];
}


Matrix::Matrix	( const Matrix & inM )
{
	MatrixCopy( this, &inM );
}


Matrix::Matrix	(	float in11, float in12, float in13, float in14,
					float in21, float in22, float in23, float in24,
					float in31, float in32, float in33, float in34,
					float in41, float in42, float in43, float in44	)
{
	m11=in11; m12=in12; m13=in13; m14=in14;
	m21=in21; m22=in22; m23=in23; m24=in24;
	m31=in31; m32=in32; m33=in33; m34=in34;
	m41=in41; m42=in42; m43=in43; m44=in44;
}


Matrix::Matrix ( const Quat & inQ )
{
	MatrixRotation( this, &inQ );
}


Matrix::Matrix ( const Vec3 & inV )
{
	MatrixIdentity( this );
	m41 = inV.x;
	m42 = inV.y;
	m43 = inV.z;
}


float& Matrix::operator () ( int Row, int Col )
{
	return m[Row][Col];
}


float Matrix::operator () (	int Row, int Col ) const
{
	return m[Row][Col];
}


Matrix& Matrix::operator = ( const Matrix & inM )
{
	MatrixCopy( this, &inM );
	return *this;
}


Matrix& Matrix::operator *= ( const Matrix & inM )
{
	MatrixMul( this, this, &inM );
	return *this;
}


Matrix& Matrix::operator +=	( const Matrix & inM )
{
	MatrixAdd( this, this, &inM );
	return *this;
}


Matrix& Matrix::operator -=	( const Matrix & inM )
{
	MatrixSub( this, this, &inM );
	return *this;
}


Matrix& Matrix::operator *=	( float inF )
{
	MatrixScalar( this, this, inF );
	return *this;
}


Matrix& Matrix::operator /= ( float inF	)
{
	SU_BREAK_IF( inF == 0.0f );
	float ooF = 1.0f / inF;
	MatrixScalar( this, this, ooF );
	return *this;
}


Matrix Matrix::operator + ( ) const
{
	return *this;
}


Matrix Matrix::operator - ( ) const
{
	Matrix m;
	MatrixScalar( &m, this, -1.f );
	return m;		
}


Matrix Matrix::operator * ( float inF ) const
{
	Matrix m;
	MatrixScalar( &m, this, inF );
	return m;		
}


Matrix Matrix::operator / ( float inF ) const
{
	SU_BREAK_IF( inF == 0.0f );
	float ooF = 1.0f / inF;
	Matrix m;
	MatrixScalar( &m, this, ooF );
	return m;		
}


Matrix Matrix::operator * ( const Matrix & inM ) const
{
	Matrix m;
	MatrixMul( &m, this, &inM );
	return m;
}


Matrix Matrix::operator + ( const Matrix & inM ) const
{
	Matrix m;
	MatrixAdd( &m, this, &inM );
	return m;
}


Matrix Matrix::operator - ( const Matrix & inM ) const
{
	Matrix m;
	MatrixSub( &m, this, &inM );
	return m;
}


bool Matrix::operator == ( const Matrix & inM ) const
{
	return su::util::Memcompare( (pvoid)this, (pvoid)&inM, sizeof(Matrix)) == 0;
}


bool Matrix::operator != ( const Matrix & inM ) const
{
	return su::util::Memcompare( (pvoid)this, (pvoid)&inM, sizeof(Matrix)) != 0;
}


Vec3* Matrix::right ( ) const
{
	return (Vec3*)&m11;
}


Vec3* Matrix::up ( ) const
{
	return (Vec3*)&m21;
}


Vec3* Matrix::front ( ) const
{
	return (Vec3*)&m31;
}


Vec3* Matrix::location ( ) const
{
	return (Vec3*)&m41;
}


#if !defined(MAT_MUL_FLOAT_MAT)
Matrix su::operator * ( float inF, const Matrix & inM )
{
	Matrix m;
	MatrixScalar( &m, &inM, inF );
	return m;		
}
#endif


#if !defined(MAT_COPY)
su::Matrix* su::MatrixCopy ( Matrix* outM, const Matrix* inM )
{
	if( outM == inM )
		return outM;

	outM->m11 = inM->m11; outM->m12 = inM->m12; outM->m13 = inM->m13; outM->m14 = inM->m14;
	outM->m21 = inM->m21; outM->m22 = inM->m22; outM->m23 = inM->m23; outM->m24 = inM->m24;
	outM->m31 = inM->m31; outM->m32 = inM->m32; outM->m33 = inM->m33; outM->m34 = inM->m34;
	outM->m41 = inM->m41; outM->m42 = inM->m42; outM->m43 = inM->m43; outM->m44 = inM->m44;
	return outM;
}
#endif

#if !defined(MAT_ZERO)
su::Matrix* su::MatrixZero ( Matrix* outM )
{
	outM->m11 = outM->m12 = outM->m13 = outM->m14 = 0;
	outM->m21 = outM->m22 = outM->m23 = outM->m24 = 0;
	outM->m31 = outM->m32 = outM->m33 = outM->m34 = 0;
	outM->m41 = outM->m42 = outM->m43 = outM->m44 = 0;
	return outM;
}
#endif

#if !defined(MAT_IDENTITY)
su::Matrix* su::MatrixIdentity ( Matrix* outM )
{
	outM->m11=1.0f; outM->m12=0.0f; outM->m13=0.0f; outM->m14=0.0f;
	outM->m21=0.0f; outM->m22=1.0f; outM->m23=0.0f; outM->m24=0.0f;
	outM->m31=0.0f; outM->m32=0.0f; outM->m33=1.0f; outM->m34=0.0f;
	outM->m41=0.0f; outM->m42=0.0f; outM->m43=0.0f; outM->m44=1.0f;
	return outM;
}
#endif

#if !defined(MAT_ISIDENTITY)
bool su::MatrixIsIdentity ( const Matrix* inM )
{
	return (   inM->m11==1.0f && inM->m12==0.0f && inM->m13==0.0f && inM->m14==0.0f
			&& inM->m21==0.0f && inM->m22==1.0f && inM->m23==0.0f && inM->m24==0.0f
			&& inM->m31==0.0f && inM->m32==0.0f && inM->m33==1.0f && inM->m34==0.0f
			&& inM->m41==0.0f && inM->m42==0.0f && inM->m43==0.0f && inM->m44==1.0f	);
}
#endif

#if !defined(MAT_DETERMINANT)
float su::MatrixDeterminant ( const Matrix* inM )
{
	float det0 = inM->m33 * inM->m44 - inM->m43 * inM->m34;
	float det1 = inM->m32 * inM->m44 - inM->m42 * inM->m34;
	float det2 = inM->m32 * inM->m43 - inM->m42 * inM->m33;
	float det3 = inM->m31 * inM->m44 - inM->m41 * inM->m34;
	float det4 = inM->m31 * inM->m43 - inM->m41 * inM->m33;
	float det5 = inM->m31 * inM->m42 - inM->m41 * inM->m32;

	float det;
	det	=  inM->m11 * ( inM->m22 * det0 - inM->m23 * det1 + inM->m24 * det2 )
		 - inM->m12 * ( inM->m21 * det0 - inM->m23 * det3 + inM->m24 * det4 )
		 + inM->m13 * ( inM->m21 * det1 - inM->m22 * det3 + inM->m24 * det5 )
		 - inM->m14 * ( inM->m21 * det2 - inM->m22 * det4 + inM->m23 * det5 );
	return det;
}
#endif

#if !defined(MAT_ADD)
su::Matrix* su::MatrixAdd ( Matrix* outM, const Matrix* inM0, const Matrix* inM1 )
{
	outM->m11 = inM0->m11 + inM1->m11; outM->m12 = inM0->m12 + inM1->m12; outM->m13 = inM0->m13 + inM1->m13; outM->m14 = inM0->m14 + inM1->m14;
	outM->m21 = inM0->m21 + inM1->m21; outM->m22 = inM0->m22 + inM1->m22; outM->m23 = inM0->m23 + inM1->m23; outM->m24 = inM0->m24 + inM1->m24;
	outM->m31 = inM0->m31 + inM1->m31; outM->m32 = inM0->m32 + inM1->m32; outM->m33 = inM0->m33 + inM1->m33; outM->m34 = inM0->m34 + inM1->m34;
	outM->m41 = inM0->m41 + inM1->m41; outM->m42 = inM0->m42 + inM1->m42; outM->m43 = inM0->m43 + inM1->m43; outM->m44 = inM0->m44 + inM1->m44;
	return outM;
}
#endif

#if !defined(MAT_SUB)
su::Matrix* su::MatrixSub ( Matrix* outM, const Matrix* inM0, const Matrix* inM1 )
{
	outM->m11 = inM0->m11 - inM1->m11; outM->m12 = inM0->m12 - inM1->m12; outM->m13 = inM0->m13 - inM1->m13; outM->m14 = inM0->m14 - inM1->m14;
	outM->m21 = inM0->m21 - inM1->m21; outM->m22 = inM0->m22 - inM1->m22; outM->m23 = inM0->m23 - inM1->m23; outM->m24 = inM0->m24 - inM1->m24;
	outM->m31 = inM0->m31 - inM1->m31; outM->m32 = inM0->m32 - inM1->m32; outM->m33 = inM0->m33 - inM1->m33; outM->m34 = inM0->m34 - inM1->m34;
	outM->m41 = inM0->m41 - inM1->m41; outM->m42 = inM0->m42 - inM1->m42; outM->m43 = inM0->m43 - inM1->m43; outM->m44 = inM0->m44 - inM1->m44;
	return outM;
}
#endif

#if !defined(MAT_SCALAR)
su::Matrix* su::MatrixScalar ( Matrix* outM, const Matrix* inM, float inF )
{
	outM->m11 = inM->m11 * inF; outM->m12 = inM->m12 * inF; outM->m13 = inM->m13 * inF; outM->m14 = inM->m14 * inF;
	outM->m21 = inM->m21 * inF; outM->m22 = inM->m22 * inF; outM->m23 = inM->m23 * inF; outM->m24 = inM->m24 * inF;
	outM->m31 = inM->m31 * inF; outM->m32 = inM->m32 * inF; outM->m33 = inM->m33 * inF; outM->m34 = inM->m34 * inF;
	outM->m41 = inM->m41 * inF; outM->m42 = inM->m42 * inF; outM->m43 = inM->m43 * inF; outM->m44 = inM->m44 * inF;
	return outM;
}
#endif

#if !defined(MAT_MUL)
su::Matrix* su::MatrixMul ( Matrix* outM, const Matrix* inM0, const Matrix* inM1 )
{
	Matrix tmp;
	for( uint r = 0 ; r < 4 ; r++ ) {
		for( uint c = 0 ; c < 4 ; c++ ) {
			tmp.m[r][c] = inM0->m[r][0] * inM1->m[0][c]
						+ inM0->m[r][1] * inM1->m[1][c]
						+ inM0->m[r][2] * inM1->m[2][c]
						+ inM0->m[r][3] * inM1->m[3][c];
		}
	}
	return MatrixCopy( outM, &tmp );
}
#endif

#if !defined(MAT_TRANSPOSE)
su::Matrix* su::MatrixTranspose ( Matrix* outM, const Matrix* inM )
{
	Matrix tmp;
	tmp.m11 = inM->m11; tmp.m12 = inM->m21; tmp.m13 = inM->m31; tmp.m14 = inM->m41;
	tmp.m21 = inM->m12; tmp.m22 = inM->m22; tmp.m23 = inM->m32; tmp.m24 = inM->m42;
	tmp.m31 = inM->m13; tmp.m32 = inM->m23; tmp.m33 = inM->m33; tmp.m34 = inM->m43;
	tmp.m41 = inM->m14; tmp.m42 = inM->m24; tmp.m43 = inM->m34; tmp.m44 = inM->m44;
	return MatrixCopy( outM, &tmp );
}
#endif

#if !defined(MAT_INVERSE)
su::Matrix* su::MatrixInverse ( Matrix* outM, float * outDeterminant, const Matrix* inM )
{
	Matrix tmp;
	// Calculate the determinant of the 3x3 rotation matrix
	float det=0.f;
	det += inM->m11 * inM->m22 * inM->m33;
	det += inM->m21 * inM->m32 * inM->m13;
	det += inM->m31 * inM->m12 * inM->m23;
	det -= inM->m31 * inM->m22 * inM->m13;
	det -= inM->m21 * inM->m12 * inM->m33;
	det -= inM->m11 * inM->m32 * inM->m23;
	if( outDeterminant )
		*outDeterminant = det;

	// Invert the 3x3 rotation matrix
	SU_ASSERT( det != 0.f );
	float oodet = 1.f / det;
	tmp.m11 =  (inM->m22 * inM->m33 - inM->m32 * inM->m23) * oodet;
	tmp.m12 = -(inM->m12 * inM->m33 - inM->m32 * inM->m13) * oodet;
	tmp.m13 =  (inM->m12 * inM->m23 - inM->m22 * inM->m13) * oodet;
	tmp.m21 = -(inM->m21 * inM->m33 - inM->m31 * inM->m23) * oodet;
	tmp.m22 =  (inM->m11 * inM->m33 - inM->m31 * inM->m13) * oodet;
	tmp.m23 = -(inM->m11 * inM->m23 - inM->m21 * inM->m13) * oodet;
	tmp.m31 =  (inM->m21 * inM->m32 - inM->m31 * inM->m22) * oodet;
	tmp.m32 = -(inM->m11 * inM->m32 - inM->m31 * inM->m12) * oodet;
	tmp.m33 =  (inM->m11 * inM->m22 - inM->m21 * inM->m12) * oodet;

	// Translation
	tmp.m41 = -(tmp.m11 * inM->m41 + tmp.m21 * inM->m42 + tmp.m31 * inM->m43);
	tmp.m42 = -(tmp.m12 * inM->m41 + tmp.m22 * inM->m42 + tmp.m32 * inM->m43);
	tmp.m43 = -(tmp.m13 * inM->m41 + tmp.m23 * inM->m42 + tmp.m33 * inM->m43);

	tmp.m14 = 0.f;
	tmp.m24 = 0.f;
	tmp.m34 = 0.f;
	tmp.m44 = 1.f;
	return MatrixCopy( outM, &tmp );
}
#endif

#if !defined(MAT_FASTINVERSE)
su::Matrix* su::MatrixFastInverse ( Matrix* outM, const Matrix* inM )
{
	// R^-1
	MatrixTranspose( outM, inM );
	// T o R
	if( outM->m14 || outM->m24 || outM->m34 ) {
		outM->m41 = - ( outM->m14*outM->m11 + outM->m24*outM->m21 + outM->m34*outM->m31 );
		outM->m42 = - ( outM->m14*outM->m12 + outM->m24*outM->m22 + outM->m34*outM->m32 );
		outM->m43 = - ( outM->m14*outM->m13 + outM->m24*outM->m23 + outM->m34*outM->m33 );
		outM->m14 = outM->m24 = outM->m34 = 0.0f;
	}
	return outM;
}
#endif

#if !defined(MAT_ORHONORMALIZE)
su::Matrix* su::MatrixOrthoNormalize ( Matrix* outM, const Matrix* inM )
{
	Vec3Cross( outM->right(), inM->up(), inM->front() );
	Vec3Cross( outM->front(), outM->right(), inM->up() );
	if( outM != inM )
		Vec3Copy( outM->up(), inM->up() );
	return MatrixNormalize( outM, inM );
}
#endif

#if !defined(MAT_NORMALIZE)
su::Matrix* su::MatrixNormalize ( Matrix* outM, const Matrix* inM )
{
	Vec3Normalize( outM->right(), inM->right() );
	Vec3Normalize( outM->up(),    inM->up()    );
	Vec3Normalize( outM->front(), inM->front() );
	if( outM != inM ) {
		outM->m14 = inM->m14;
		outM->m24 = inM->m24;
		outM->m34 = inM->m34;
		outM->m41 = inM->m41;
		outM->m42 = inM->m42;
		outM->m43 = inM->m43;
		outM->m44 = inM->m44;
	}
	return outM;
}
#endif

#if !defined(MAT_SCALE_MAT_MAT_FLOAT_FLOAT_FLOAT)
su::Matrix* su::MatrixScale ( Matrix* outM, const Matrix* inM, float inSX, float inSY, float inSZ )
{
	Matrix m;
	MatrixScaling( &m, inSX, inSY, inSZ );
	return MatrixMul( outM, inM, &m );
}
#endif

#if !defined(MAT_SCALE_MAT_MAT_VEC3)
su::Matrix* su::MatrixScale (Matrix* outM, const Matrix* inM, const Vec3* inV )
{
	return MatrixScale( outM, inM, inV->x, inV->y, inV->z );
}
#endif

#if !defined(MAT_SCALE_MAT_MAT_MAT)
su::Matrix* su::MatrixScale ( Matrix* outM, const Matrix* inM, const Matrix* inS )
{
	Matrix m;
	MatrixScaling( &m, inS );
	return MatrixMul( outM, inM, &m );
}
#endif

#if !defined(MAT_PRESCALE_MAT_MAT_FLOAT_FLOAT_FLOAT)
su::Matrix* su::MatrixPreScale ( Matrix* outM, const Matrix* inM, float inSX, float inSY, float inSZ )
{
	Matrix m;
	MatrixScaling( &m, inSX, inSY, inSZ );
	return MatrixMul( outM, &m, inM );
}
#endif

#if !defined(MAT_PRESCALE_MAT_MAT_VEC3)
su::Matrix* su::MatrixPreScale ( Matrix* outM, const Matrix* inM, const Vec3* inV )
{
	return MatrixPreScale( outM, inM, inV->x, inV->y, inV->z );
}
#endif

#if !defined(MAT_PRESCALE_MAT_MAT_MAT)
su::Matrix* su::MatrixPreScale ( Matrix* outM, const Matrix* inM, const Matrix* inS )
{
	Matrix m;
	MatrixScaling( &m, inS );
	return MatrixMul( outM, &m, inM );
}
#endif

#if !defined(MAT_TRANSLATE_MAT_MAT_FLOAT_FLOAT_FLOAT)
su::Matrix* su::MatrixTranslate ( Matrix* outM, const Matrix* inM, float inTX, float inTY, float inTZ )
{
	Matrix m;
	MatrixTranslation( &m, inTX, inTY, inTZ );
	return MatrixMul( outM, inM, &m );
}
#endif

#if !defined(MAT_TRANSLATE_MAT_MAT_VEC3)
su::Matrix* su::MatrixTranslate ( Matrix* outM, const Matrix* inM, const Vec3* inV )
{
	return MatrixTranslate( outM, inM, inV->x, inV->y, inV->z );
}
#endif

#if !defined(MAT_TRANSLATE_MAT_MAT_VEC3)
su::Matrix* su::MatrixTranslate ( Matrix* outM, const Matrix* inM, const Matrix* inT )
{
	Matrix m;
	MatrixTranslation( &m, inT );
	return MatrixMul( outM, inM, &m );
}
#endif

#if !defined(MAT_PRETRANSLATE_MAT_MAT_FLOAT_FLOAT_FLOAT)
su::Matrix* su::MatrixPreTranslate ( Matrix* outM, const Matrix* inM, float inTX, float inTY, float inTZ )
{
	Matrix m;
	MatrixTranslation( &m, inTX, inTY, inTZ );
	return MatrixMul( outM, &m, inM );
}
#endif

#if !defined(MAT_PRETRANSLATE_MAT_MAT_VEC3)
su::Matrix* su::MatrixPreTranslate ( Matrix* outM, const Matrix* inM, const Vec3* inV )
{
	return MatrixPreTranslate( outM, inM, inV->x, inV->y, inV->z );
}
#endif

#if !defined(MAT_PRETRANSLATE_MAT_MAT_MAT)
su::Matrix* su::MatrixPreTranslate ( Matrix* outM, const Matrix* inM, const Matrix* inT )
{
	Matrix m;
	MatrixTranslation( &m, inT );
	return MatrixMul( outM, &m, inM );
}
#endif

#if !defined(MAT_ROTATE_MAT_MAT_QUAT)
su::Matrix* su::MatrixRotate ( Matrix* outM, const Matrix* inM, const Quat* inQ )
{
	Matrix m;
	MatrixRotation( &m, inQ );
	return MatrixMul( outM, inM, &m );
}
#endif

#if !defined(MAT_ROTATE_MAT_MAT_VEC3_FLOAT)
su::Matrix* su::MatrixRotate ( Matrix* outM, const Matrix* inM, const Vec3* inAxis, float inAngle )
{
	Matrix m;
	MatrixRotation( &m, inAxis, inAngle );
	return MatrixMul( outM, inM, &m );
}
#endif

#if !defined(MAT_ROTATE_MAT_MAT_MAT)
su::Matrix* su::MatrixRotate ( Matrix* outM, const Matrix* inM, const Matrix* inR )
{
	Matrix m;
	MatrixRotation( &m, inR );
	return MatrixMul( outM, inM, &m );
}
#endif

#if !defined(MAT_PREROTATE_MAT_MAT_QUAT)
su::Matrix* su::MatrixPreRotate ( Matrix* outM, const Matrix* inM, const Quat* inQ )
{
	Matrix m;
	MatrixRotation( &m, inQ );
	return MatrixMul( outM, &m, inM );
}
#endif

#if !defined(MAT_PREROTATE_MAT_MAT_VEC3_FLOAT)
su::Matrix* su::MatrixPreRotate ( Matrix* outM, const Matrix* inM, const Vec3* inAxis, float inAngle )
{
	Matrix m;
	MatrixRotation( &m, inAxis, inAngle );
	return MatrixMul( outM, &m, inM );
}
#endif

#if !defined(MAT_PREROTATE_MAT_MAT_MAT)
su::Matrix* su::MatrixPreRotate ( Matrix* outM, const Matrix* inM, const Matrix* inR )
{
	Matrix m;
	MatrixRotation( &m, inR );
	return MatrixMul( outM, &m, inM );
}
#endif

#if !defined(MAT_ROTATEX)
su::Matrix* su::MatrixRotateX ( Matrix* outM, const Matrix* inM, float inAngle )
{
	Matrix m;
	MatrixRotationX( &m, inAngle );
	return MatrixMul( outM, inM, &m );
}
#endif

#if !defined(MAT_PREROTATEX)
su::Matrix* su::MatrixPreRotateX ( Matrix* outM, const Matrix* inM, float inAngle )
{
	Matrix m;
	MatrixRotationX( &m, inAngle );
	return MatrixMul( outM, &m, inM );
}
#endif

#if !defined(MAT_ROTATEY)
su::Matrix* su::MatrixRotateY ( Matrix* outM, const Matrix* inM, float inAngle )
{
	Matrix m;
	MatrixRotationY( &m, inAngle );
	return MatrixMul( outM, inM, &m );
}
#endif

#if !defined(MAT_PREROTATEY)
su::Matrix* su::MatrixPreRotateY ( Matrix* outM, const Matrix* inM, float inAngle )
{
	Matrix m;
	MatrixRotationY( &m, inAngle );
	return MatrixMul( outM, &m, inM );
}
#endif

#if !defined(MAT_ROTATEZ)
su::Matrix* su::MatrixRotateZ ( Matrix* outM, const Matrix* inM, float inAngle )
{
	Matrix m;
	MatrixRotationZ( &m, inAngle );
	return MatrixMul( outM, inM, &m );
}
#endif

#if !defined(MAT_PREROTATEZ)
su::Matrix* su::MatrixPreRotateZ ( Matrix* outM, const Matrix* inM, float inAngle )
{
	Matrix m;
	MatrixRotationZ( &m, inAngle );
	return MatrixMul( outM, &m, inM );
}
#endif

#if !defined(MAT_SCALING_MAT_FLOAT_FLOAT_FLOAT)
su::Matrix* su::MatrixScaling ( Matrix* outM, float inSX, float inSY, float inSZ )
{
	MatrixZero( outM );
	outM->m11 = inSX;
	outM->m22 = inSY;
	outM->m33 = inSZ;
	outM->m44 = 1.0f;
	return outM;
}
#endif

#if !defined(MAT_SCALING_MAT_VEC3)
su::Matrix* su::MatrixScaling ( Matrix* outM, const Vec3* inV )
{
	return MatrixScaling( outM, inV->x, inV->y, inV->z );
}
#endif

#if !defined(MAT_SCALING_MAT_MAT)
su::Matrix* su::MatrixScaling ( Matrix* outM, const Matrix* inM )
{
	Vec3 s;
	MatrixGetTR( NULL, NULL, &s, inM );
	return MatrixScaling( outM, &s );
}
#endif

#if !defined(MAT_TRANSLATION_MAT_FLOAT_FLOAT_FLOAT)
su::Matrix* su::MatrixTranslation ( Matrix* outM, float inTX, float inTY, float inTZ )
{
	MatrixIdentity( outM );
	outM->m41 = inTX;
	outM->m42 = inTY;
	outM->m43 = inTZ;
	return outM;
}
#endif

#if !defined(MAT_TRANSLATION_MAT_VEC3)
su::Matrix* su::MatrixTranslation ( Matrix* outM, const Vec3* inV )
{
	return MatrixTranslation( outM, inV->x, inV->y, inV->z );
}
#endif

#if !defined(MAT_TRANSLATION_MAT_MAT)
su::Matrix* su::MatrixTranslation ( Matrix* outM, const Matrix* inM )
{
	return MatrixTranslation( outM, inM->m41, inM->m42, inM->m43 );
}
#endif

#if !defined(MAT_ROTATIONX)
su::Matrix* su::MatrixRotationX ( Matrix* outM, float inAngle )
{
	float s, c;
	Sincosf( inAngle, &s, &c );
	MatrixIdentity( outM );
	outM->m22 =  c;
	outM->m23 =  s;
	outM->m32 = -s;
	outM->m33 =  c;
	return outM;
}
#endif

#if !defined(MAT_ROTATIONY)
su::Matrix* su::MatrixRotationY ( Matrix* outM, float inAngle )
{
	float s, c;
	Sincosf( inAngle, &s, &c );
	MatrixIdentity( outM );
	outM->m11 =  c;
	outM->m13 = -s;
	outM->m31 =  s;
	outM->m33 =  c;
	return outM;
}
#endif

#if !defined(MAT_ROTATIONZ)
su::Matrix* su::MatrixRotationZ ( Matrix* outM, float inAngle )
{
	float s, c;
	Sincosf( inAngle, &s, &c );
	MatrixIdentity( outM );
	outM->m11 =  c;
	outM->m12 =  s;
	outM->m21 = -s;
	outM->m22 =  c;
	return outM;
}
#endif

#if !defined(MAT_ROTATION_MAT_VEC3_FLOAT)
su::Matrix* su::MatrixRotation ( Matrix* outM, const Vec3* inAxis, float inAngle )
{
	Quat q( inAngle, *inAxis );
	return MatrixRotation( outM, &q );
}
#endif

#if !defined(MAT_ROTATION_MAT_QUAT)
su::Matrix* su::MatrixRotation ( Matrix* outM, const Quat* inQ )
{
	float s  = 2.f;
	float xs = inQ->x*s;
	float ys = inQ->y*s;
	float zs = inQ->z*s;
	float wx = inQ->w*xs;
	float wy = inQ->w*ys;
	float wz = inQ->w*zs;
	float xx = inQ->x*xs;
	float xy = inQ->x*ys;
	float xz = inQ->x*zs;
	float yy = inQ->y*ys;
	float yz = inQ->y*zs;
	float zz = inQ->z*zs;

	outM->m11 = (1.0f-(yy+zz));
	outM->m12 = (xy+wz);
	outM->m13 = (xz-wy);
	outM->m21 = (xy-wz);
	outM->m22 = (1.0f-(xx+zz));
	outM->m23 = (yz+wx);
	outM->m31 = (xz+wy);
	outM->m32 = (yz-wx);
	outM->m33 = (1.0f-(xx+yy));
	outM->m41 = outM->m42 = outM->m43 = 0.0f;
	outM->m14 = outM->m24 = outM->m34 = 0.0f;
	outM->m44 = 1.0f;
	return outM;
}
#endif

#if !defined(MAT_ROTATION_MAT_MAT)
su::Matrix* su::MatrixRotation ( Matrix* outM, const Matrix* inM )
{
	if( outM != inM )
		MatrixCopy( outM, inM );
	outM->m41 = outM->m42 = outM->m43 = 0.0f;
	return outM;
}
#endif

#if !defined(MAT_BUILDTR)
su::Matrix* su::MatrixBuildTR ( Matrix*	outM, const Vec3* inT, const Quat* inR, const Vec3* inS )
{
	if( inR )	MatrixRotation( outM, inR );
	else		MatrixIdentity( outM );
	if( inT ) {
		outM->m41 = inT->x;
		outM->m42 = inT->y;
		outM->m43 = inT->z;
	}
	if( inS )
		MatrixPreScale( outM, outM, inS );
	return outM;
}
#endif

#if !defined(MAT_GETTR)
void su::MatrixGetTR ( Vec3*	outT, Quat* outR, Vec3*	outS, const Matrix*	inM )
{
	if( outT ) {
		outT->x = inM->m41;
		outT->y = inM->m42;
		outT->z = inM->m43;
	}
	if( outR || outS ) {
		Matrix m;
		float sx = Sqrtf( inM->m11*inM->m11 + inM->m12*inM->m12 + inM->m13*inM->m13 );
		float sy = Sqrtf( inM->m21*inM->m21 + inM->m22*inM->m22 + inM->m23*inM->m23 );
		float sz = Sqrtf( inM->m31*inM->m31 + inM->m32*inM->m32 + inM->m33*inM->m33 );
		if( outR ) {
			float oosx = (sx==0.f) ? 0.f : 1.f/sx;
			float oosy = (sy==0.f) ? 0.f : 1.f/sy;
			float oosz = (sz==0.f) ? 0.f : 1.f/sz;
			m.m11 = inM->m11 * oosx;	m.m12 = inM->m12 * oosx;	m.m13 = inM->m13 * oosx;
			m.m21 = inM->m21 * oosy;	m.m22 = inM->m22 * oosy;	m.m23 = inM->m23 * oosy;
			m.m31 = inM->m31 * oosz;	m.m32 = inM->m32 * oosz;	m.m33 = inM->m33 * oosz;
			QuatFromMatrix( outR, &m );
		}
		if( outS ) {
			outS->x = sx;
			outS->y = sy;
			outS->z = sz;
		}
	}
}
#endif

#if !defined(MAT_SETTR)
su::Matrix* su::MatrixSetTR ( Matrix*	outM, const Vec3* inT, const Quat* inR, const Vec3* inS, const Matrix* inM )
{
	if( !inM )
		return MatrixBuildTR( outM, inT, inR, inS );

	// t ?
	if( inT ) {
		outM->m41 = inT->x;
		outM->m42 = inT->y;
		outM->m43 = inT->z;
	} else if( inM != outM ) {
		outM->m41 = inM->m41;
		outM->m42 = inM->m42;
		outM->m43 = inM->m43;
	}

	// r ?
	if( inR ) {
		float s  = 2.f;
		float xs = inR->x*s;
		float ys = inR->y*s;
		float zs = inR->z*s;
		float wx = inR->w*xs;
		float wy = inR->w*ys;
		float wz = inR->w*zs;
		float xx = inR->x*xs;
		float xy = inR->x*ys;
		float xz = inR->x*zs;
		float yy = inR->y*ys;
		float yz = inR->y*zs;
		float zz = inR->z*zs;
		if( inS ) {
			outM->m11 = inS->x * (1.0f-(yy+zz));
			outM->m12 = inS->x * (xy+wz);
			outM->m13 = inS->x * (xz-wy);
			outM->m21 = inS->y * (xy-wz);
			outM->m22 = inS->y * (1.0f-(xx+zz));
			outM->m23 = inS->y * (yz+wx);
			outM->m31 = inS->z * (xz+wy);
			outM->m32 = inS->z * (yz-wx);
			outM->m33 = inS->z * (1.0f-(xx+yy));
		} else {
			outM->m11 = (1.0f-(yy+zz));
			outM->m12 = (xy+wz);
			outM->m13 = (xz-wy);
			outM->m21 = (xy-wz);
			outM->m22 = (1.0f-(xx+zz));
			outM->m23 = (yz+wx);
			outM->m31 = (xz+wy);
			outM->m32 = (yz-wx);
			outM->m33 = (1.0f-(xx+yy));
		}
		return outM;
	}

	// no-r & s ?
	if( inS ) {
		// get scale in inM
		float sx2 = inM->m11*inM->m11 + inM->m12*inM->m12 + inM->m13*inM->m13;
		float sy2 = inM->m21*inM->m21 + inM->m22*inM->m22 + inM->m23*inM->m23;
		float sz2 = inM->m31*inM->m31 + inM->m32*inM->m32 + inM->m33*inM->m33;
		if( sx2>0.f ) {
			float sx = Abs(1.f-sx2)>0.1f ? inS->x/Sqrtf(sx2) : inS->x;
			outM->m11 = sx * inM->m11;
			outM->m12 = sx * inM->m12;
			outM->m13 = sx * inM->m13;
		} else if( inM != outM ) {
			outM->m11 = inM->m11;
			outM->m12 = inM->m12;
			outM->m13 = inM->m13;
		}
		if( sy2>0.f ) {
			float sy = Abs(1.f-sy2)>0.1f ? inS->y/Sqrtf(sy2) : inS->y;
			outM->m21 = sy * inM->m21;
			outM->m22 = sy * inM->m22;
			outM->m23 = sy * inM->m23;
		} else if( inM != outM ) {
			outM->m21 = inM->m21;
			outM->m22 = inM->m22;
			outM->m23 = inM->m23;
		}
		if( sz2>0.f ) {
			float sz = Abs(1.f-sz2)>0.1f ? inS->z/Sqrtf(sz2) : inS->z;
			outM->m31 = sz * inM->m31;
			outM->m32 = sz * inM->m32;
			outM->m33 = sz * inM->m33;
		} else if( inM != outM ) {
			outM->m31 = inM->m31;
			outM->m32 = inM->m32;
			outM->m33 = inM->m33;
		}
		return outM;
	}

	// no-r & no-s
	if( inM != outM ) {
		outM->m11 = inM->m11;
		outM->m12 = inM->m12;
		outM->m13 = inM->m13;
		outM->m21 = inM->m21;
		outM->m22 = inM->m22;
		outM->m23 = inM->m23;
		outM->m31 = inM->m31;
		outM->m32 = inM->m32;
		outM->m33 = inM->m33;
	}

	return outM;
}
#endif

#if !defined(MAT_GETAXIS)
void su::MatrixGetAxis ( Vec3*	outRight, Vec3* outUp, Vec3* outFront, const Matrix* inM )
{
	if( outRight ) {
		outRight->x = inM->m11;
		outRight->y = inM->m12;
		outRight->z = inM->m13;
	}
	if( outUp ) {
		outUp->x = inM->m21;
		outUp->y = inM->m22;
		outUp->z = inM->m23;
	}
	if( outFront ) {
		outFront->x = inM->m31;
		outFront->y = inM->m32;
		outFront->z = inM->m33;
	}
}
#endif

#if !defined(MAT_SETAXIS)
su::Matrix* su::MatrixSetAxis ( Matrix*	outM, const Vec3* inRight, const Vec3* inUp, const Vec3* inFront )
{
	if( inRight ) {
		outM->m11 = inRight->x;
		outM->m12 = inRight->y;
		outM->m13 = inRight->z;
	}
	if( inUp ) {
		outM->m21 = inUp->x;
		outM->m22 = inUp->y;
		outM->m23 = inUp->z;
	}
	if( inFront ) {
		outM->m31 = inFront->x;
		outM->m32 = inFront->y;
		outM->m33 = inFront->z;
	}
	return outM;
}
#endif

#if !defined(MAT_ORTHORH)
su::Matrix* su::MatrixOrthoRH ( Matrix*	outM, float inW, float inH, float inZNear, float inZFar )
{
	SU_ASSERT( inZFar > inZNear );
	MatrixZero( outM );
	float p = 1.0f / ( inZFar - inZNear );
	float q = inZNear * p;
	outM->m11 = 2.0f/inW;
	outM->m22 = 2.0f/inH;
	outM->m33 = -(2.0f*p);
	outM->m43 = -2.0f*q-1.0f;
	outM->m44 = 1.0f;
	return outM;
}
#endif

#if !defined(MAT_ORTHOLH)
su::Matrix* su::MatrixOrthoLH ( Matrix*	outM, float inW, float inH, float inZNear, float inZFar )
{
	SU_ASSERT( inZFar > inZNear );
	MatrixZero( outM );
	float p = 1.0f / ( inZFar - inZNear );
	float q = inZNear * p;
	outM->m11 = 2.0f/inW;
	outM->m22 = 2.0f/inH;
	outM->m33 = (2.0f*p);
	outM->m43 = -2.0f*q-1.0f;
	outM->m44 = 1.0f;
	return outM;
}
#endif

#if !defined(MAT_PERSPECTIVERH)
su::Matrix* su::MatrixPerspectiveRH ( Matrix*	outM, float inFovx, float inAspect, float inZNear, float inZFar )
{
	SU_ASSERT( inZFar > inZNear );
	MatrixZero( outM );
	float s = 1.0f / Tanf( inFovx * 0.5f );
	float p = inZFar / ( inZFar - inZNear );
	float q = inZNear * p;
	outM->m11 = s;
	outM->m22 = s*inAspect;
	outM->m33 = -(2.0f*p-1.0f);
	outM->m34 = -1.0f;
	outM->m43 = -2.0f*q;
	return outM;
}
#endif

#if !defined(MAT_PERSPECTIVELH)
su::Matrix* su::MatrixPerspectiveLH ( Matrix*	outM, float inFovx, float inAspect, float inZNear, float inZFar )
{
	SU_ASSERT( inZFar > inZNear );
	MatrixZero( outM );
	float s = 1.0f / Tanf( inFovx * 0.5f );
	float p = inZFar / ( inZFar - inZNear );
	float q = inZNear * p;
	outM->m11 = s;
	outM->m22 = s*inAspect;
	outM->m33 = (2.0f*p-1.0f);
	outM->m34 = +1.0f;
	outM->m43 = -2.0f*q;
	return outM;
}
#endif

#if !defined(MAT_ISPROJRH)
bool su::MatrixIsProjRH ( Matrix* inM )
{
	if( inM->m34 < 0 )		return TRUE;	// Checks MatrixPerspectiveRH
	if( inM->m34 > 0 )		return FALSE;	// Checks MatrixPerspectiveLH
	if( inM->m33 < 0 )		return TRUE;	// Checks MatrixOrthoRH
	else					return FALSE;	// Else   MatrixOrthoLH
}
#endif

#if !defined(MAT_ISPROJLH)
bool su::MatrixIsProjLH ( Matrix*	inM )
{
	if( inM->m34 > 0 )		return TRUE;	// Checks MatrixPerspectiveLH
	if( inM->m34 < 0 )		return FALSE;	// Checks MatrixPerspectiveRH
	if( inM->m33 > 0 )		return TRUE;	// Checks MatrixOrthoLH
	else					return FALSE;	// Else   MatrixOrthoRH
}
#endif

#if !defined(MAT_STORE)
void su::MatrixStore ( float* outF, Matrix* inM )
{
	SU_COMPILE_TIME_ASSERT( sizeof(Matrix) == 64U );
	SU_COMPILE_TIME_ASSERT( sizeof(inM->m) == 64U );
	util::Memcopy( (void*)outF, (void*)&inM->m[0][0], 64 );
}
#endif

#if !defined(MAT_LOAD)
void su::MatrixLoad ( Matrix* outM, float* inF )
{
	SU_COMPILE_TIME_ASSERT( sizeof(Matrix) == 64U );
	SU_COMPILE_TIME_ASSERT( sizeof(outM->m) == 64U );
	util::Memcopy( (void*)&outM->m[0][0], (void*)inF, 64 );
}
#endif

#if !defined(MAT_ARRAYSETTR_MAT_TR_INT)
void su::MatrixArraySetTR	(		Matrix*		outMArray,
									const TR*	inTRArray,
									uint		inCount	)
{
	Matrix* out = outMArray, *outParent;
	TR *    cur = (TR*)inTRArray;
	TR *    end = (TR*)inTRArray + inCount;

	while( cur != end )
	{
		MatrixBuildTR( out, &cur->t, &cur->r, NULL );
		if( cur->parentIdx >= 0 ) {
			outParent = outMArray + cur->parentIdx;
			SU_ASSERT( outParent < out );
			MatrixMul( out, out, outParent );
		}
		cur++;
		out++;
	}
}
#endif

#if !defined(MAT_ARRAYSETTR_MAT_TRS_INT)
void su::MatrixArraySetTR (	Matrix*		outMArray,
							const TRS*	inTRSArray,
							uint		inCount		)
{
	Matrix* out = outMArray, *outParent;
	TRS *   cur = (TRS*)inTRSArray;
	TRS *   end = (TRS*)inTRSArray + inCount;

	while( cur != end )
	{
		MatrixBuildTR( out, &cur->t, &cur->r, NULL );
		if( cur->parentIdx >= 0 ) {
			outParent = outMArray + cur->parentIdx;
			SU_ASSERT( outParent < out );
			MatrixMul( out, out, outParent );
		}
		cur++;
		out++;
	}
}
#endif

#if !defined(MAT_ARRAYSETTRS)
void su::MatrixArraySetTRS (	Matrix*		outMArray,
								const TRS*	inTRSArray,
								uint		inCount		)
{
	Matrix* out = outMArray, *outParent;
	TRS *   cur = (TRS*)inTRSArray;
	TRS *   end = (TRS*)inTRSArray + inCount;

	while( cur != end )
	{
		MatrixBuildTR( out, &cur->t, &cur->r, &cur->s );
		if( cur->parentIdx >= 0 ) {
			outParent = outMArray + cur->parentIdx;
			SU_ASSERT( outParent < out );
			MatrixMul( out, out, outParent );
		}
		cur++;
		out++;
	}
}
#endif

#if !defined(MAT_ARRAYMUL)
void su::MatrixArrayMul (	Matrix*		outMArray,
							Matrix*		inM0Array,
							Matrix*		inM1Array,
							uint		inCount	)
{
	Matrix* out  = outMArray;
	Matrix* end  = outMArray + inCount;
	Matrix* cur1 = inM1Array;
	Matrix* cur0 = inM0Array;

	while( out != end ) {
		MatrixMul( out, cur0, cur1 );
		cur0++;
		cur1++;
		out++;
	}
}
#endif




//--------------------------
// Quaternion
//--------------------------



const su::Quat	su::Quat::ZERO(	0.0f,0.0f,0.0f,0.0f );
const su::Quat	su::Quat::ONE(	1.0f,1.0f,1.0f,1.0f );
const su::Quat	su::Quat::UNIT(	0.0f,0.0f,0.0f,1.0f );



Quat::Quat(	const float * inFP	)
{
	x = inFP[0];
	y = inFP[1];
	z = inFP[2];
	w = inFP[3];
}


Quat::Quat ( const Quat & inQ )
{
	QuatCopy( this, &inQ );
}


Quat::Quat ( float inX, float inY, float inZ, float inW )
{
	x = inX;
	y = inY;
	z = inZ;
	w = inW;
}


Quat::Quat ( float theta, const Vec3 & inU )
{
	QuatFromAxisAngle( this, &inU, theta );
}


Quat::Quat ( const Vec2 & inV )
{
	x = inV.x;
	y = inV.y;
	z = 0.0f;
	w = 0.0f;
}


Quat::Quat ( const Vec3 & inV )
{
	x = inV.x;
	y = inV.y;
	z = inV.z;
	w = 0.0f;
}


Quat::Quat ( const Vec4 & inV )
{
	x = inV.x;
	y = inV.y;
	z = inV.z;
	w = inV.w;
}


Quat::Quat ( const Matrix & inM )
{
	QuatFromMatrix( this, &inM );
}


Quat& Quat::Set ( float x, float y, float z, float w )
{
	x = x;
	y = y;
	z = z;
	w = w;
	return *this;
}


Quat& Quat::operator = ( const Quat & inQ )
{
	QuatCopy( this, &inQ );
	return *this;
}


Quat& Quat::operator *= ( const Quat & inQ )
{
	QuatMul( this, this, &inQ );
	return *this;
}


Quat& Quat::operator += ( const Quat & inQ )
{
	x += inQ.x;
	y += inQ.y;
	z += inQ.z;
	w += inQ.w;
	return *this;
}


Quat& Quat::operator -= ( const Quat & inQ )
{
	x -= inQ.x;
	y -= inQ.y;
	z -= inQ.z;
	w -= inQ.w;
	return *this;
}


Quat& Quat::operator *= ( float inF )
{
	x *= inF;
	y *= inF;
	z *= inF;
	w *= inF;
	return *this;
}


Quat& Quat::operator /= ( float inF )
{
	SU_BREAK_IF( inF == 0.0f );
	float ooF = 1.0f / inF;
	x *= ooF;
	y *= ooF;
	z *= ooF;
	w *= ooF;
	return *this;
}


Quat Quat::operator + ( ) const
{
	return *this;
}


Quat Quat::operator - ( ) const
{
	return Quat( -x, -y, -z ,-w );
}


Quat Quat::operator * ( const Quat & inQ ) const
{
	Quat q;
	QuatMul( &q, this, &inQ );
	return q;
}


Quat Quat::operator + ( const Quat & inQ ) const
{
	return Quat( x+inQ.x, y+inQ.y, z+inQ.z, w+inQ.w );
}


Quat Quat::operator - ( const Quat & inQ ) const
{
	return Quat( x-inQ.x, y-inQ.y, z-inQ.z, w-inQ.w );
}


Quat Quat::operator * ( float inF ) const
{
	return Quat( x*inF, y*inF, z*inF, w*inF );
}


Quat Quat::operator / ( float inF ) const
{
	SU_BREAK_IF( inF == 0.0f );
	float ooF = 1.0f / inF;
	return Quat( x*ooF, y*ooF, z*ooF, w*ooF );
}


bool Quat::operator == ( const Quat & inQ ) const
{
	return ( x==inQ.x && y==inQ.y && inQ.z==z && inQ.w==w );
}


bool Quat::isnear ( const Quat& inRef, float d ) const
{
	return QuatNear( this, &inRef, d );
}


bool Quat::operator != (	const Quat & inQ	) const
{
	return ( x!=inQ.x || y!=inQ.y || inQ.z!=z || inQ.w!=w );
}



#if !defined(QUAT_MUL_FLOAT_QUAT)
Quat su::operator * ( float inF, const Quat & inQ )
{
	return Quat( inF*inQ.x, inF*inQ.y, inF*inQ.z, inF*inQ.w );
}
#endif

#if !defined(QUAT_COPY)
su::Quat* su::QuatCopy ( Quat* outQ, const Quat* inQ )
{
	if( outQ == inQ )
		return outQ;

	outQ->x		= inQ->x;
	outQ->y		= inQ->y;
	outQ->z		= inQ->z;
	outQ->w		= inQ->w;
	return outQ;
}
#endif

#if !defined(QUAT_ZERO)
su::Quat* su::QuatZero ( Quat* outQ )
{
	outQ->x	= 0.0f;
	outQ->y	= 0.0f;
	outQ->z	= 0.0f;
	outQ->w	= 0.0f;
	return outQ;
}
#endif

#if !defined(QUAT_IDENTITY)
su::Quat* su::QuatIdentity ( Quat* outQ )
{
	outQ->x	= 0.0f;
	outQ->y	= 0.0f;
	outQ->z	= 0.0f;
	outQ->w	= 1.0f;
	return outQ;
}
#endif

#if !defined(QUAT_ISIDENTITY)
bool su::QuatIsIdentity ( const Quat* inQ )
{
	return	(	inQ->x == 0.0f
			&&	inQ->y == 0.0f
			&&	inQ->z == 0.0f
			&&	inQ->w == 1.0f	);
}
#endif

#if !defined(QUAT_NORM)
float su::QuatNorm ( const Quat* inQ )
{
	return Vec4Norm( (const Vec4*)inQ );
}
#endif

#if !defined(QUAT_NORM2)
float su::QuatNorm2 ( const Quat* inQ )
{
	return Vec4Norm2( (const Vec4*)inQ );
}
#endif

#if !defined(QUAT_NORMALIZE)
su::Quat* su::QuatNormalize ( Quat* outQ, const Quat* inQ )
{
	return (Quat*) Vec4Normalize( (Vec4*)outQ, (const Vec4*)inQ );
}
#endif

#if !defined(QUAT_NEAR)
bool su::QuatNear ( const Quat* inQ0, const Quat* inQ1, float d )
{
	Quat q;
	float dq = QuatNorm2( QuatSub(&q,inQ0,inQ1) );
	return dq <= (d*d);
}
#endif

#if !defined(QUAT_ADD)
su::Quat* su::QuatAdd ( Quat* outQ, const Quat* inQ0, const Quat* inQ1 )
{
	outQ->x = inQ0->x + inQ1->x;
	outQ->y = inQ0->y + inQ1->y;
	outQ->z = inQ0->z + inQ1->z;
	outQ->w = inQ0->w + inQ1->w;
	return outQ;
}
#endif

#if !defined(QUAT_SUB)
su::Quat* su::QuatSub ( Quat* outQ, const Quat* inQ0, const Quat* inQ1 )
{
	outQ->x = inQ0->x - inQ1->x;
	outQ->y = inQ0->y - inQ1->y;
	outQ->z = inQ0->z - inQ1->z;
	outQ->w = inQ0->w - inQ1->w;
	return outQ;
}
#endif

#if !defined(QUAT_DOT)
float su::QuatDot ( const Quat* inQ0, const Quat* inQ1 )
{
return Vec4Dot( (const Vec4*)inQ0, (const Vec4*)inQ1 );
}
#endif

#if !defined(QUAT_CONJUGATE)
su::Quat* su::QuatConjugate ( Quat* outQ, const Quat* inQ )
{
	outQ->x = - inQ->x;
	outQ->y = - inQ->y;
	outQ->z = - inQ->z;
	outQ->w =   inQ->w;
	return outQ;
}
#endif

#if !defined(QUAT_FASTINVERSE)
su::Quat* su::QuatFastInverse ( Quat* outQ, const Quat* inQ )
{
	return QuatConjugate( outQ, inQ );
}
#endif

#if !defined(QUAT_INVERSE)
su::Quat* su::QuatInverse ( Quat* outQ, const Quat* inQ )
{
	// q^-1 = q' / (q'*q) = q' / ||q||^2
	float n2 = (	inQ->x * inQ->x
				+	inQ->y * inQ->y
				+	inQ->z * inQ->z
				+	inQ->w * inQ->w	);
	float oon2 = (n2==0.0f) ? 1.0f : 1.0f/n2;

	outQ->x = - inQ->x * oon2;
	outQ->y = - inQ->y * oon2;
	outQ->z = - inQ->z * oon2;
	outQ->w =   inQ->w;
	return outQ;
}
#endif

#if !defined(QUAT_TOAXISANGLE)
void su::QuatToAxisAngle ( Vec3* outV, float * outAngle, const Quat* inQ )
{
  	float c = inQ->w;
	float a = Acosf(c) * 2.0f;

	if( outAngle )
		*outAngle = a;

	if( outV ) {
		float f = Sinf( a );
		float oof = (Absf(f)>FpEpsilon) ? 1.0f/f : 1.0f;
		outV->x = inQ->x * oof;
		outV->y = inQ->y * oof;
		outV->z = inQ->z * oof;
	}
}
#endif

#if !defined(QUAT_FROMAXISANGLE)
su::Quat* su::QuatFromAxisAngle ( Quat* outQ, const Vec3* inV, float inAngle )
{
	float c, s;
	Sincosf( inAngle*0.5f, &s, &c );
	outQ->x = inV->x * s;
	outQ->y = inV->y * s;
	outQ->z = inV->z * s;
	outQ->w = c;
	return outQ;
}
#endif

#if !defined(QUAT_FROMMATRIX)
su::Quat* su::QuatFromMatrix ( Quat* outQ, const Matrix* inM )
{
	float trace = inM->m[0][0] + inM->m[1][1] + inM->m[2][2];

	if( trace > 0.0f)
	{
		float s = Sqrt( trace + 1.0f );
		outQ->w = s * 0.5f;
		s = 0.5f / s;
		outQ->x = -(inM->m[2][1] - inM->m[1][2]) * s;
		outQ->y = -(inM->m[0][2] - inM->m[2][0]) * s;
		outQ->z = -(inM->m[1][0] - inM->m[0][1]) * s;
	}
	else 
	{
		int i=0, j=1, k=2;
		if (inM->m[1][1] > inM->m[0][0]) { i=1; j=2; k=0; }
		if (inM->m[2][2] > inM->m[i][i]) { i=2; j=0; k=1; }

		float s = Sqrt( inM->m[i][i] - (inM->m[j][j] + inM->m[k][k]) + 1.0f);
		float*q = & outQ->x;
		q[i] = - s * 0.5f;
		if( s != 0.0f )
			s = 0.5f / s;
		q[j] = - (inM->m[j][i] + inM->m[i][j]) * s;
		q[k] = - (inM->m[k][i] + inM->m[i][k]) * s;
		q[3] = + (inM->m[k][j] - inM->m[j][k]) * s;
	}
	return outQ;
}
#endif

#if !defined(QUAT_MUL)
su::Quat* su::QuatMul ( Quat* outQ, const Quat* inQ0, const Quat* inQ1 )
{
	Quat tmp;
	tmp.w = inQ0->w * inQ1->w - inQ0->x * inQ1->x - inQ0->y * inQ1->y - inQ0->z * inQ1->z;
	tmp.x = inQ0->w * inQ1->x + inQ0->x * inQ1->w + inQ0->y * inQ1->z - inQ0->z * inQ1->y;
	tmp.y = inQ0->w * inQ1->y + inQ0->y * inQ1->w + inQ0->z * inQ1->x - inQ0->x * inQ1->z;
	tmp.z = inQ0->w * inQ1->z + inQ0->z * inQ1->w + inQ0->x * inQ1->y - inQ0->y * inQ1->x;
	return QuatCopy( outQ, &tmp );
}
#endif

#if !defined(QUAT_LN)
su::Quat* su::QuatLn ( Quat* outQ, const Quat* inQ )
{
	Quat tmp;
	tmp.w = 0.0f;

	if( Absf(inQ->w) < 1.0f )
	{
		float a = Acosf( inQ->w );
		float s = Sinf( a );
		if( Absf(s) >= FpEpsilon )
		{
			float f = a/s;
			tmp.x = inQ->x * f;
			tmp.y = inQ->y * f;
			tmp.z = inQ->z * f;
		}
	}
	else
	{
		tmp.x = inQ->x;
		tmp.y = inQ->y;
		tmp.z = inQ->z;
	}

	return QuatCopy( outQ, &tmp );
}
#endif

#if !defined(QUAT_EXP)
su::Quat* su::QuatExp ( Quat* outQ, const Quat* inQ )
{
	Quat tmp;

	float a = Sqrt( inQ->x*inQ->x+inQ->y*inQ->y+inQ->z*inQ->z);
	float s, c;
	Sincosf( a, &s, &c );
	tmp.w = c;

	if( Absf(s) >= FpEpsilon )
	{
		float f = s/a;
		tmp.x = inQ->x * f;
		tmp.y = inQ->y * f;
		tmp.z = inQ->z * f;
	}
	else
	{
		tmp.x = inQ->x;
		tmp.y = inQ->y;
		tmp.z = inQ->z;
	}

	return QuatCopy( outQ, &tmp );
}
#endif

#if !defined(QUAT_SLERP)
su::Quat* su::QuatSlerp ( Quat* outQ, const Quat* inQ0, const Quat* inQ1, float inT )
{
	SU_ASSERT( inT >= 0.0f );
	SU_ASSERT( inT <= 1.0f );

	if( inT == 0.0f ) {
		*outQ = *inQ0;
		return outQ;
	}
	if( inT == 1.0f ) {
		*outQ = *inQ1;
		return outQ;
	}

	float omega, cosom, sinom, scale0, scale1, flip;

	// calc cosine
	cosom =		inQ0->x * inQ1->x
			+	inQ0->y * inQ1->y
			+	inQ0->z * inQ1->z
			+	inQ0->w * inQ1->w;

	// adjust signs
	if( cosom < 0.0 ) {
		cosom  = - cosom;
		flip = -1.0f;
	} else {
		flip = 1.0f;
	}

	// calculate coefficients
	if( (1.0f - cosom) > FpEpsilon ) {
		// standard case (slerp)
		omega = Acosf(cosom);
		sinom = 1.0f/Sinf(omega);
		scale0 = Sinf((1.0f - inT) * omega) * sinom;
		scale1 = flip * Sinf(inT * omega) * sinom;
	} else {        
		// "p" and "q" quaternions are very close 
		//  ... so we can do a linear interpolation
		scale0 = 1.0f - inT;
		scale1 = flip * inT;
	}

	// calculate final values
	outQ->x = scale0 * inQ0->x + scale1 * inQ1->x;
	outQ->y = scale0 * inQ0->y + scale1 * inQ1->y;
	outQ->z = scale0 * inQ0->z + scale1 * inQ1->z;
	outQ->w = scale0 * inQ0->w + scale1 * inQ1->w;

	return outQ;
}
#endif

#if !defined(QUAT_EULER)
su::Quat* su::QuatEuler ( Quat* outQ, float inEx, float inEy, float inEz )
{
	float cx, sx;
	float cy, sy;
	float cz, sz;
	Sincos( inEx * 0.5f, &sx, &cx );
	Sincos( inEy * 0.5f, &sy, &cy );
	Sincos( inEz * 0.5f, &sz, &cz );

	float cycz, sysz, sycz, cysz;
	cycz = cy*cz;   
	sysz = sy*sz;
	sycz = sy*cz;
	cysz = cy*sz;
	outQ->w = cycz*cx + sysz*sx;
	outQ->x = cycz*sx + sysz*cx;
	outQ->y = sycz*cx - cysz*sx;
	outQ->z = cysz*cx - sycz*sx;
	return outQ;
}
#endif







//--------------------------
// Box3
//--------------------------




Box3::Box3(	const Box3& inB )
{
	Box3Copy( this, &inB );
}



Box3::Box3(	const Sph3& inS )
{
	center		= inS.center;
	rotation	= Quat::UNIT;
	float r		= inS.radius * 2.0f;
	length		= Vec3( r, r, r );
}



Box3& Box3::operator = ( const Box3& inB )
{
	Box3Copy( this, &inB );
	return *this;
}



Box3& Box3::operator += ( const Box3& inB )
{
	Box3Concat( this, this, &inB );
	return *this;
}



Box3 Box3::operator + ( const Box3& inB ) const
{
	Box3 b;
	Box3Concat( &b, this, &inB );
	return b;
}



Box3& Box3::operator *= ( const Matrix& inM )
{
	Box3Apply( this, this, &inM );
	return *this;
}



Box3 Box3::operator * ( const Matrix& inM ) const
{
	Box3 b;
	Box3Apply( &b, this, &inM );
	return b;
}



bool Box3::operator == ( const Box3 & inB ) const
{
	return		( inB.center	== center	)
			&&	( inB.rotation	== rotation )
			&&	( inB.length	== length	);
}



bool Box3::operator != ( const Box3 & inB ) const
{
	return		( inB.center	!= center	)
			||	( inB.rotation	!= rotation )
			||	( inB.length	!= length	);
}





#if !defined(BOX3_COPY)
su::Box3* su::Box3Copy ( Box3* outB, const Box3* inB )
{
	if( outB == inB )
		return outB;

	outB->center   = inB->center;
	outB->rotation = inB->rotation;
	outB->length   = inB->length;
	return outB;
}
#endif

#if !defined(BOX3_CONCAT)
su::Box3* su::Box3Concat ( Box3* outB, const Box3* inB0, const Box3* inB1 )
{
	// Rotation resultante q = moyenne des rotations
	Quat q0, q1, q, iq;
	QuatNormalize( &q0, &inB0->rotation );
	QuatNormalize( &q1, &inB1->rotation );
	QuatSlerp( &q, &q0, &q1, 0.5f );
	QuatNormalize( &q, &q );
	QuatFastInverse( &iq, &q );

	// 16 coins ds l'espace de rotation
	Vec3  v, vmin, vmax;
	float W, H, D;
	uint i;

	// Box0
	W	= inB0->length.x * 0.5f;
	H	= inB0->length.y * 0.5f;
	D	= inB0->length.z * 0.5f;
	for( i = 0 ; i < 8 ; i++ ) {
		v.x = i&1 ? W : -W;
		v.y = i&2 ? H : -H;
		v.z = i&4 ? D : -D;
		v   = ( (v*q0) + inB0->center ) * iq;
		if( i== 0 )	{
			vmin = vmax = v;
		}
		else {
			Vec3Min( &vmin, &vmin, &v );
			Vec3Max( &vmax, &vmax, &v );
		}
	}

	// Box1
	W	= inB1->length.x * 0.5f;
	H	= inB1->length.y * 0.5f;
	D	= inB1->length.z * 0.5f;
	for( i = 0 ; i < 8 ; i++ ) {
		v.x = i&1 ? W : -W;
		v.y = i&2 ? H : -H;
		v.z = i&4 ? D : -D;
		v   = ( (v*q1) + inB1->center ) * iq;
		Vec3Min( &vmin, &vmin, &v );
		Vec3Max( &vmax, &vmax, &v );
	}

	outB->rotation = q;
	outB->length   = ( vmax - vmin );
	outB->center   = ( vmax + vmin ) * 0.5f;
	outB->center  *= q;
	return outB;
}
#endif

#if !defined(BOX3_AXYSALIGN)
su::Box3* su::Box3AxysAlign ( Box3* outB, const Box3* inB )
{
	// 8 coins ds le world
	Vec3  v, vmin, vmax;

	float W	= inB->length.x * 0.5f;
	float H	= inB->length.y * 0.5f;
	float D	= inB->length.z * 0.5f;
	for( uint i = 0 ; i < 8 ; i++ ) {
		v.x = i&1 ? W : -W;
		v.y = i&2 ? H : -H;
		v.z = i&4 ? D : -D;
		v   = (v*inB->rotation) + inB->center;
		if( i== 0 )	{
			vmin = vmax = v;
		}
		else {
			Vec3Min( &vmin, &vmin, &v );
			Vec3Max( &vmax, &vmax, &v );
		}
	}

	outB->rotation = Quat::UNIT;
	outB->length   = ( vmax - vmin );
	outB->center   = ( vmax + vmin ) * 0.5f;
	return outB;
}
#endif

#if !defined(BOX3_APPLY)
su::Box3* su::Box3Apply ( Box3* outB, const Box3* inB,  const Matrix* inM )
{
	Matrix TR;
	MatrixBuildTR( &TR, &inB->center, &inB->rotation, &inB->length );
	MatrixMul( &TR, &TR, inM );
	MatrixGetTR( &outB->center, &outB->rotation, &outB->length, &TR );
	return outB;
}
#endif











//--------------------------
// Sph3
//--------------------------



Sph3::Sph3(	const Sph3 & inS )
{
	Sph3Copy( this, &inS );
}



Sph3::Sph3(	const Box3 & inB )
{
	center	= inB.center;
	radius	= inB.length.x;
	radius  = inB.length.y > radius ? inB.length.y : radius;
	radius  = inB.length.z > radius ? inB.length.z : radius;
	radius *= 0.5f;
}



Sph3& Sph3::operator = ( const Sph3& inS )
{
	Sph3Copy( this, &inS );
	return *this;
}



Sph3& Sph3::operator += ( const Sph3& inS )
{
	Sph3Concat( this, this, &inS );
	return *this;
}



Sph3 Sph3::operator + ( const Sph3& inS ) const
{
	Sph3 s;
	Sph3Concat( &s, this, &inS );
	return s;
}



Sph3& Sph3::operator *= ( const Matrix& inM )
{
	Sph3Apply( this, this, &inM );
	return *this;
}



Sph3 Sph3::operator * ( const Matrix& inM ) const
{
	Sph3 s;
	Sph3Apply( &s, this, &inM );
	return s;
}



bool Sph3::operator == ( const Sph3& inS ) const
{
	return		( inS.center	== center	)
			&&	( inS.radius	== radius	);
}



bool Sph3::operator != ( const Sph3& inS ) const
{
	return		( inS.center	!= center	)
			||	( inS.radius	!= radius	);
}




#if !defined(SPH3_COPY)
su::Sph3* su::Sph3Copy ( Sph3* outS, const Sph3* inS )
{
	if( outS == inS )
		return outS;

	outS->center = inS->center;
	outS->radius = inS->radius;
	return outS;
}
#endif

#if !defined(SPH3_CONCAT)
su::Sph3* su::Sph3Concat ( Sph3* outS, const Sph3* inB0, const Sph3* inB1 )
{
	Vec3 c0 = inB0->center;
	Vec3 c1 = inB1->center;
	Vec3 c  = ( c0 + c1 ) * 0.5f;
	Vec3 dc = ( c0 - c1 );
	outS->center = c;
	outS->radius = Vec3Norm(&dc) + inB0->radius + inB1->radius;
	return outS;
}
#endif

#if !defined(SPH3_APPLY)
su::Sph3* su::Sph3Apply ( Sph3* outS, const Sph3* inS,  const Matrix* inM )
{
	Vec3Apply( &outS->center, &inS->center, inM );
	outS->radius = inS->radius;
	return outS;
}
#endif





//--------------------------
// Frustum3
//--------------------------




Frustum3::Frustum3(	const Frustum3& inF )
{
	Frustum3Copy( this, &inF );
}


Frustum3::Frustum3(	const Box3& inB )
{
	Frustum3Build( this, &inB );
}


Frustum3::Frustum3( const Sph3& inS )
{
	Frustum3Build( this, &inS );
}


Frustum3::Frustum3(	const Matrix& inM )
{
	Frustum3Build( this, &inM );
}


bool Frustum3::IsInside ( const Box3& inB ) const
{
	float hw = inB.length.x * 0.5f;
	float hh = inB.length.y * 0.5f;
	float hd = inB.length.z * 0.5f;
	Vec3 l;
	uint clipAnd = 0x3F;
	l = Vec3( -hw, -hh, -hd ) * inB.rotation + inB.center;
	clipAnd &= Frustum3Clip( this, &l );
	l = Vec3( +hw, -hh, -hd ) * inB.rotation + inB.center;
	clipAnd &= Frustum3Clip( this, &l );
	l = Vec3( +hw, -hh, +hd ) * inB.rotation + inB.center;
	clipAnd &= Frustum3Clip( this, &l );
	l = Vec3( -hw, -hh, +hd ) * inB.rotation + inB.center;
	clipAnd &= Frustum3Clip( this, &l );
	l = Vec3( -hw, +hh, -hd ) * inB.rotation + inB.center;
	clipAnd &= Frustum3Clip( this, &l );
	l = Vec3( +hw, +hh, -hd ) * inB.rotation + inB.center;
	clipAnd &= Frustum3Clip( this, &l );
	l = Vec3( +hw, +hh, +hd ) * inB.rotation + inB.center;
	clipAnd &= Frustum3Clip( this, &l );
	l = Vec3( -hw, +hh, +hd ) * inB.rotation + inB.center;
	clipAnd &= Frustum3Clip( this, &l );
	// All outside ?
	return clipAnd ? FALSE : TRUE;
}


bool Frustum3::IsInside ( const Sph3& inS ) const
{
	return Frustum3Inside( this, &inS.center, inS.radius );
}


bool Frustum3::IsInside ( const Vec3 & inV ) const
{
	return Frustum3Inside( this, &inV );
}


bool Frustum3::IsInside ( const Vec4 & inV ) const
{
	return Frustum3Inside( this, &inV );
}

#if !defined(FRUSTRUM3_COPY)
su::Frustum3* su::Frustum3Copy ( Frustum3* outF, const Frustum3* inF )
{
	if( outF != inF )
		util::Memcopy( (void*)outF->plane, (void*)inF->plane, sizeof(outF->plane) );
	return outF;
}
#endif

#if !defined(FRUSTRUM3_BUILD_FRUSTRUM3_BOX3)
su::Frustum3* su::Frustum3Build ( Frustum3* outF, const Box3* inB )
{
	float hw = inB->length.x * 0.5f;
	float hh = inB->length.y * 0.5f;
	float hd = inB->length.z * 0.5f;
	Vec3 l[8];
	l[0] = Vec3( -hw, -hh, -hd ) * inB->rotation + inB->center;
	l[1] = Vec3( +hw, -hh, -hd ) * inB->rotation + inB->center;
	l[2] = Vec3( +hw, -hh, +hd ) * inB->rotation + inB->center;
	l[3] = Vec3( -hw, -hh, +hd ) * inB->rotation + inB->center;
	l[4] = Vec3( -hw, +hh, -hd ) * inB->rotation + inB->center;
	l[5] = Vec3( +hw, +hh, -hd ) * inB->rotation + inB->center;
	l[6] = Vec3( +hw, +hh, +hd ) * inB->rotation + inB->center;
	l[7] = Vec3( -hw, +hh, +hd ) * inB->rotation + inB->center;
	Vec3 n[6], p[6];
	n[0] = (l[0]-l[1]) ^ (l[2]-l[1]);	p[0] = l[1];
	n[1] = (l[1]-l[5]) ^ (l[6]-l[5]);	p[1] = l[5];
	n[2] = (l[2]-l[6]) ^ (l[7]-l[6]);	p[2] = l[6];
	n[3] = (l[3]-l[7]) ^ (l[4]-l[7]);	p[3] = l[7];
	n[4] = (l[0]-l[4]) ^ (l[5]-l[4]);	p[4] = l[4];
	n[5] = (l[5]-l[4]) ^ (l[7]-l[4]);	p[5] = l[4];
	for( int i = 0 ; i < 6 ; i++ ) {
		Vec3Normalize( &n[i], &n[i] );
		outF->plane[i].x =   n[i].x;
		outF->plane[i].y =   n[i].y;
		outF->plane[i].z =   n[i].z;
		outF->plane[i].w = - n[i] * p[i];
	}
	SU_ASSERT( outF->IsInside(inB->center) );
	return outF;
}
#endif

#if !defined(FRUSTRUM3_FRUSTRUM3_SPH3)
su::Frustum3* su::Frustum3Build ( Frustum3* outF, const Sph3* inS )
{
	Box3 b( *inS );
	return Frustum3Build( outF, &b );
}
#endif

#if !defined(FRUSTRUM3_FRUSTRUM3_MAT)
su::Frustum3* su::Frustum3Build	( Frustum3* outF, const Matrix* inM )
{
	// planes extraction
	outF->plane[0].x = (inM->m14 + inM->m11);
	outF->plane[0].y = (inM->m24 + inM->m21);
	outF->plane[0].z = (inM->m34 + inM->m31);
	outF->plane[0].w = (inM->m44 + inM->m41);
	outF->plane[1].x = (inM->m14 - inM->m11);
	outF->plane[1].y = (inM->m24 - inM->m21);
	outF->plane[1].z = (inM->m34 - inM->m31);
	outF->plane[1].w = (inM->m44 - inM->m41);
	outF->plane[2].x = (inM->m14 + inM->m12);
	outF->plane[2].y = (inM->m24 + inM->m22);
	outF->plane[2].z = (inM->m34 + inM->m32);
	outF->plane[2].w = (inM->m44 + inM->m42);
	outF->plane[3].x = (inM->m14 - inM->m12);
	outF->plane[3].y = (inM->m24 - inM->m22);
	outF->plane[3].z = (inM->m34 - inM->m32);
	outF->plane[3].w = (inM->m44 - inM->m42);
	outF->plane[4].x = (inM->m14 + inM->m13);
	outF->plane[4].y = (inM->m24 + inM->m23);
	outF->plane[4].z = (inM->m34 + inM->m33);
	outF->plane[4].w = (inM->m44 + inM->m43);
	outF->plane[5].x = (inM->m14 - inM->m13);
	outF->plane[5].y = (inM->m24 - inM->m23);
	outF->plane[5].z = (inM->m34 - inM->m33);
	outF->plane[5].w = (inM->m44 - inM->m43);
	// normalisation
	for( int i = 0 ; i < 6 ; i++ ) {
		float n   = Vec3Norm( (Vec3*)&outF->plane[i] );
		float oon = (n==0.f) ? 1.f : 1.f/n;
		Vec4Scale( &outF->plane[i], &outF->plane[i], oon );
	}
	return outF;
}
#endif

#if !defined(FRUSTRUM3_APPLY)
su::Frustum3* su::Frustum3Apply ( Frustum3* outF, const Frustum3* inF,  const Matrix* inM )
{
	for( int i = 0 ; i < 6 ; i++ ) {
		Vec3 n, p;
		n.x		= inF->plane[i].x;
		n.y		= inF->plane[i].y;
		n.z		= inF->plane[i].z;
		float d	= inF->plane[i].w;
		p.x = p.y = p.z = 0;
		if( n.x != 0.f )	p.x = -d / n.x;
		if( n.y != 0.f )	p.y = -d / n.y;
		if( n.z != 0.f )	p.z = -d / n.z;
		Vec3ApplyVector( &n, &n, inM );
		Vec3Apply( &p, &p, inM );
		outF->plane[i].x =   n.x;
		outF->plane[i].y =   n.y;
		outF->plane[i].z =   n.z;
		outF->plane[i].w = - n * p;
	}
	return outF;
}
#endif


#if !defined(FRUSTUM3CLIP_FRUSTUM3_VEC3_FLOAT)
uint su::Frustum3Clip ( const Frustum3* inF, const Vec3* inV, float inRadius )
{
	Vec4 v( *inV );
	return Frustum3Clip( inF, &v, inRadius );
}
#endif


#if !defined(FRUSTUM3CLIP_FRUSTUM3_VEC4_FLOAT)
uint su::Frustum3Clip ( const Frustum3* inF, const Vec4* inV, float inRadius )
{
	uint cf = 0;
	inRadius = Fabs( inRadius );
	if( Vec4Dot(&inF->plane[0],inV) < -inRadius )	cf |= (1<<0);
	if( Vec4Dot(&inF->plane[1],inV) < -inRadius )	cf |= (1<<1);
	if( Vec4Dot(&inF->plane[2],inV) < -inRadius )	cf |= (1<<2);
	if( Vec4Dot(&inF->plane[3],inV) < -inRadius )	cf |= (1<<3);
	if( Vec4Dot(&inF->plane[4],inV) < -inRadius )	cf |= (1<<4);
	if( Vec4Dot(&inF->plane[5],inV) < -inRadius )	cf |= (1<<5);
	return cf;
}
#endif



#if !defined(FRUSTUM3INSIDE_FRUSTUM3_VEC3_FLOAT)
bool su::Frustum3Inside ( const Frustum3* inF, const Vec3* inV, float inRadius )
{
	Vec4 v( *inV );
	return Frustum3Inside( inF, &v, inRadius );
}
#endif



#if !defined(FRUSTUM3INSIDE_FRUSTUM3_VEC4_FLOAT)
bool su::Frustum3Inside	( const Frustum3* inF, const Vec4* inV, float inRadius	)
{
	inRadius = Fabs( inRadius );
	if( Vec4Dot(&inF->plane[4],inV) < -inRadius )	return FALSE;
	if( Vec4Dot(&inF->plane[5],inV) < -inRadius )	return FALSE;
	if( Vec4Dot(&inF->plane[0],inV) < -inRadius )	return FALSE;
	if( Vec4Dot(&inF->plane[1],inV) < -inRadius )	return FALSE;
	if( Vec4Dot(&inF->plane[2],inV) < -inRadius )	return FALSE;
	if( Vec4Dot(&inF->plane[3],inV) < -inRadius )	return FALSE;
	return TRUE;
}
#endif




//--------------------------
// Curve
//--------------------------

#if !defined(CURVE_HERMITEBASIS)
void su::ComputeHermiteBasis ( float* outB, float inU )
{
	float u,u2,u3;
	u  = inU;
	u2 = u*u;
	u3 = u2*u;
	outB[0] =  2.0f*u3 - 3.0f*u2     + 1.0f	;
	outB[1] = -2.0f*u3 + 3.0f*u2			;
	outB[2] =       u3 - 2.0f*u2 + u		;
	outB[3] =       u3 -      u2			;
}
#endif

#if !defined(CURVE_BEZIERBASIS)
void su::ComputeBezierBasis	( float* outB, float inU )
{

	float u,u2,u3;
	u  = inU;
	u2 = u*u;
	u3 = u2*u;
	outB[0] = -     u3 + 3.0f*u2 - 3.0f*u + 1.0f;
	outB[1] =  3.0f*u3 - 6.0f*u2 + 3.0f*u		;
	outB[2] = -3.0f*u3 + 3.0f*u2				;
	outB[3] =       u3							;
}
#endif

#if !defined(CURVE_SPLINEINTERP_FLOAT_FLOAT_FLOAT_FLOAT_FLOAT_FLOAT)
void su::ComputeSplineInterp ( float& outP, float* inB/*[4]*/, float K0, float K1, float K2, float K3 )
{
	outP  = K0 * inB[0];
	outP += K1 * inB[1];
	outP += K2 * inB[2];
	outP += K3 * inB[3];
}
#endif

#if !defined(CURVE_SPLINEINTERP_VEC2_FLOAT_VEC2_VEC2_VEC2_VEC2)
void su::ComputeSplineInterp ( Vec2& outP, float* inB/*[4]*/, Vec2& K0, Vec2& K1, Vec2& K2, Vec2& K3 )
{
	outP  = K0 * inB[0];
	outP += K1 * inB[1];
	outP += K2 * inB[2];
	outP += K3 * inB[3];
}
#endif

#if !defined(CURVE_SPLINEINTERP_VEC3_FLOAT_VEC3_VEC3_VEC3_VEC3)
void su::ComputeSplineInterp ( Vec3& outP, float* inB/*[4]*/, Vec3& K0, Vec3& K1, Vec3& K2, Vec3& K3 )
{
	outP  = K0 * inB[0];
	outP += K1 * inB[1];
	outP += K2 * inB[2];
	outP += K3 * inB[3];
}
#endif

#if !defined(CURVE_HERMITESMOOTHSTEP)
// Smooth step function with hermite interpolation
float su::ComputeHermiteSmoothStep ( float x0, float x1, float v )
{
   if (v<=x0) return(0.0f);
   else if (v>=x1) return(1.0f);
   else {
      float u = (v-x0)/(x1-x0);
      return(u*u*(3-2*u));
   }
}
#endif

/*
void
su::ComputeSplineInterp( Vec3 & outP, float u, Vec3 * knot, float m1, float m2 )
{
	float v[4];
	ComputeHermiteBasis(v,u);
	float c[4];
	c[0] = -v[2]*m2;
	c[1] = v[0] + v[2]*(m2-m1) - v[3]*m1;
	c[2] = v[1] + v[2]*m1 + v[3]*(m1-m2);
	c[3] = v[3]*m2;
	outP =  knot[0]*c[0] + knot[1]*c[1] + knot[2]*c[2] + knot[3]*c[3];
}



void CubicMorphCont::CalcFirstCoef(float *v,float *c)
	{
	float G, H, J;

	H = .5f*(1.0f-keys[0].tens)*v[2];
	J = 3.0f*H;
	G = v[3] - H;
	c[0] = v[0] - J - G * keys[1].k;
	c[1] = v[1] + J + G*(keys[1].k-keys[1].l);
	c[2] = G*keys[1].l;
	}

void CubicMorphCont::CalcLastCoef(float *v,float *c)
	{
	int nkeys = keys.Count();
	float G, H, J;

	H = .5f*(1.0f-keys[nkeys-1].tens)*v[3];
	J = 3.0f*H;
	G = v[2]-H;
	c[0] = -G*keys[nkeys-2].m;
	c[1] = v[0] - J + G*(keys[nkeys-2].m-keys[nkeys-2].n);
	c[2] = v[1] + J + G*keys[nkeys-2].n;
	}

void CubicMorphCont::CalcMiddleCoef(float *v,int *knum,float *c)
	{
	c[0] = -v[2]*keys[knum[1]].m;
	c[1] = v[0] + v[2]*(keys[knum[1]].m-keys[knum[1]].n) - v[3]*keys[knum[2]].k;
	c[2] = v[1] + v[2]*keys[knum[1]].n + v[3]*(keys[knum[2]].k-keys[knum[2]].l);
	c[3] = v[3]*keys[knum[2]].l;
	}


*/


float su::GaussianUnit ( float inDeviation, float inDistance )
{
	SU_ASSERT( inDeviation > 0.0f );
	return Exp( -(inDistance*inDistance) / (2.0f*inDeviation*inDeviation) );
}


float su::Gaussian ( float inDeviation, float inDistance )
{
	SU_ASSERT( inDeviation > 0.0f );
	float scalar = inDeviation * Sqrt(TwoPi);
	return GaussianUnit(inDeviation,inDistance) / scalar;
}


float su::GaussianDeviation ( float inRadius )
{
	// Correct gaussian sampling with width = 4 * deviation
	return ( inRadius + 1.0f ) * 0.25f;
}


int su::GaussianWidth ( float inDeviation )
{
	// Correct gaussian sampling with width = 4 * deviation
	if( inDeviation <= 0.0f )
		return 0;
	return int( Ceil(inDeviation*4.0f) );
}


int su::GaussianNormSequence ( float* outG, float inDeviation, bool inNormalize )
{
	int width = GaussianWidth( inDeviation );
	if( width <= 0 )
		return 0;

	// Gaussian factors
	float sum = 0;
	for( int i = 0 ; i < width ; i++ ) {
		float g = GaussianUnit( inDeviation, (float)i );
		outG[i] = g;
		sum    += g;
	}

	if( inNormalize ) {
		// Gaussian factors -> normalised weights
		float oo_sum = 1.0f / sum;
		for( int i = 0 ; i < width ; i++ )
			outG[i] *= oo_sum;
	}

	return width;
}



// ArcBall

namespace
{
	Quat	ab_get_rot		(	const Vec3&		inArcBegin,
								const Vec3&		inArcEnd,
								const Vec3&		inLockAxis,
								const Vec2&		inLockXY	)
	{
		bool bLock = !Vec3Near( &inLockAxis, &(Vec3::ZERO), 0.1f );
		bool bLock2DX = ( inLockXY == Vec2::UNIT_Y );
		bool bLock2DY = ( inLockXY == Vec2::UNIT_X );
		
		if( bLock2DX || bLock2DY )
		{
			Quat r;
			float a = (bLock2DX) ? ( inArcEnd.x - inArcBegin.x ) : ( inArcBegin.y - inArcEnd.y ) ;
			QuatFromAxisAngle( &r, ( (bLock2DX) ? &(Vec3::UNIT_Y) : &(Vec3::UNIT_X) ), a );
			return r;
		}

		Vec3 a0, a1, u;
		Vec3Normalize( &a0, &inArcBegin );
		Vec3Normalize( &a1, &inArcEnd );
		
		if( a0 == a1 )
		{
			return Quat::UNIT;
		}

		Vec3Cross( &u, &a0, &a1 );
		if( Vec3Norm( &u ) == 0.0f )
		{
			a0.z = 1.0f;
			a1.z = 1.0f;
			Vec3Normalize( &a0, &a0 );
			Vec3Normalize( &a1, &a1 );
			Vec3Cross( &u, &a0, &a1 );
		}
		Vec3Normalize( &u, &u );

		float cos_s		= a0 * a1;
		float cos_so2	= Sqrtf( (1.0f+cos_s)*0.5F );
		float sin_so2	= Sqrtf( (1.0f-cos_s)*0.5F );

		return Quat( u.x * sin_so2,
					 u.y * sin_so2,
					 u.z * sin_so2,
					 -cos_so2		);
	}

	void	ab_fill_point	(	ArcBall::Point*	outPoint,
								const Vec2&		inCoord,
								float			inTime,
								const Vec3&		inLockAxis,
								const Vec2&		inLockXY		)
	{
		bool bLock	 = !Vec3Near( &inLockAxis, &(Vec3::ZERO), 0.1f );
		bool bLock2DX = ( inLockXY == Vec2::UNIT_Y );
		bool bLock2DY = ( inLockXY == Vec2::UNIT_X );

		
		Vec3 p;
		if( bLock2DX || bLock2DY )
		{
			p = Vec3(	( bLock2DY ) ? 0.0f : inCoord.x,
						( bLock2DX ) ? 0.0f : inCoord.y, -0.1f );
		}
		else
		{
			p = Vec3( inCoord.x, inCoord.y, 0.0f );

			float l2 = Vec3Norm2( &p );
			if( l2 > 1.0f )
			{
				l2 = 1.0f;
			//	float r2 = 1.0f;
			//	float n = r2 / Sqrtf( l2 );	p*=n;
			}
			p.z = -Sqrtf( 1.0f - l2 );

			if( bLock )
			{
				const Vec3& n = inLockAxis;
				float k = -( n.x*p.x + n.y*p.y + n.z*p.z ) / ( n.x*n.x + n.y*n.y + n.z*n.z );
				p = p + k * n;
				if( Vec3Near( &p, &(Vec3::ZERO), 0.1f ) )
					p = Vec3::UNIT_Z;
			}
		}
		
		outPoint->p2 = inCoord;
		outPoint->p3 = p;
		outPoint->t  = inTime;
	}
}
ArcBall::ArcBall():
	began		( false ),
	movecount	( 0 ),
	ispeed		( 0.0f ),
	iangle		( 0.0f )
{
	QuatIdentity( &base );
	QuatIdentity( &state );
	Vec3Zero( &lockaxis );
	Vec2Zero( &lockXY );
}

void	ArcBall::Reset		(	const Quat&		inQuat		)
{
	began = false;
	base = inQuat;
	QuatIdentity( &state );
	Vec3Zero( &lockaxis );
	Vec2Zero( &lockXY );
	movecount = 0;
	ispeed = 0.0f;
	iangle = 0.0f;
}

void	ArcBall::Begin		(	float inT, const Vec2& inHomogeneousCoord, const Vec3& inLockAxis )
{
	SU_REQUIRE_OR_RETURN( !began );

	began		= true;
	lockaxis	= inLockAxis;
	Vec2Zero( &lockXY );
	QuatIdentity( &state );	
	ispeed		= 0.0f;

	ab_fill_point( &beginpoint, inHomogeneousCoord, inT, lockaxis, lockXY );
	movecount	= 0;
}


void	ArcBall::Begin		(	float inT, const Vec2& inHomogeneousCoord, const Vec2& inLockXY )
{
	SU_REQUIRE_OR_RETURN( !began );

	began		= true;
	Vec3Zero( &lockaxis );
	lockXY		= inLockXY;
	QuatIdentity( &state );	
	ispeed		= 0.0f;

	ab_fill_point( &beginpoint, inHomogeneousCoord, inT, lockaxis, lockXY );
	movecount	= 0;
}

void	ArcBall::Move		(	float inT, const Vec2& inHomogeneousCoord )
{
	SU_REQUIRE_OR_RETURN( began );

	uint decIdx = Min( movecount, 7U );
	for( uint i=decIdx; i>0; i-- )
		movepoints[i] = movepoints[i-1];

	ab_fill_point( movepoints, inHomogeneousCoord, inT, lockaxis, lockXY );
	if( movecount<8 )
		movecount++;

	state = ab_get_rot( beginpoint.p3, movepoints[0].p3, lockaxis, lockXY );
}

void	ArcBall::End		(	float inT, const Vec2& inHomogeneousCoord )
{
	SU_REQUIRE_OR_RETURN( began );

	ab_fill_point( &endpoint, inHomogeneousCoord, inT, lockaxis, lockXY );
	
	state = ab_get_rot( beginpoint.p3, endpoint.p3, lockaxis, lockXY );
	QuatNormalize( &state, &state );

	base *= state; 
	QuatNormalize( &base, &base );
	QuatIdentity( &state );


	ispeed = 0.0f;
	if( movecount )
	{
		for( uint i=0; i<movecount-1; i++ )
		{
			Vec2 di	 = movepoints[i+1].p2 - movepoints[i].p2;
			float d  = Vec2Norm( &di );
			float dt = movepoints[i].t - movepoints[i+1].t;
			if( dt != 0.0f )
			{
				ispeed += (d/dt);
			}
		}
		ispeed /= movecount;
		ispeed = Min( ispeed, 80.0f );
	}
	
	iangle = 0.0f;
	itime = inT;

	began = false;
}

Quat	ArcBall::GetState	(	float inT	)
{
	if( began )
	{
		return base * state;
	}
	else if( endpoint.t > beginpoint.t )
	{
		if( !movecount )
		{
			return base;
		}
		float dt = inT - itime;
		itime = inT;
		
		ispeed -= (ispeed*5.0f) * dt;
		if( ispeed < 0.01f )
			ispeed = 0.0f;

		iangle += ( ispeed * dt );

		Quat nq = ab_get_rot( movepoints[movecount-1].p3, movepoints[0].p3, lockaxis, lockXY );
		Vec3 ax;
		QuatToAxisAngle( &ax, NULL, &nq );

		Quat inertiaRot;
		QuatFromAxisAngle( &inertiaRot, &ax, iangle );
		QuatNormalize( &inertiaRot, &inertiaRot );
		return base * inertiaRot;
	}
	return base;
}






SU_BEGIN_STARTER( CoreMath )


void OnStage ( Stage stage )
{
	if( stage == STAGE_INITIALIZE )
	{
		#if defined(USE_APPROX)
		Approximations_Initialize();
		#endif

		SU_COMPILE_TIME_ASSERT( sizeof(Vec2) == 8  );
		SU_COMPILE_TIME_ASSERT( sizeof(Vec3) == 12 );
		SU_COMPILE_TIME_ASSERT( sizeof(Vec4) == 16 );
		SU_COMPILE_TIME_ASSERT( sizeof(Matrix) == 64 );
		SU_COMPILE_TIME_ASSERT( sizeof(Quat) == 16 );
	}
}


SU_END_STARTER( CoreMath )



