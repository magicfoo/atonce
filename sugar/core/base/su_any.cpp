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


#include <sufw/core/su_core.h>
using namespace su;



const su::Any	su::Any::True( true );
const su::Any	su::Any::False( false );
const su::Any	su::Any::Zero( 0U );
const su::Any	su::Any::Null( (void*)0 );
const su::Any	su::Any::NullString( (pcstr)0 );




su::Any::Any	(					)
{
	util::Zero( _data );
	_type = Undefined;
}


su::Any::Any	(	const Any&	v	)
{
	_type = v._type;
	_data = v._data;
}


su::Any::Any	(	bool	v	)
{
	util::Zero( _data );
	_type = Boolean;
	_data.b = v;
}


su::Any::Any	(	int8	v	)
{
	util::Zero( _data );
	_type = Byte;
	_data.i[0] = v;
}


su::Any::Any	(	int8 v0, int8 v1	)
{
	util::Zero( _data );
	_type = Byte2;
	_data.i[0] = v0;
	_data.i[1] = v1;
}


su::Any::Any	(	int8 v0, int8 v1, int8 v2	)
{
	util::Zero( _data );
	_type = Byte3;
	_data.i[0] = v0;
	_data.i[1] = v1;
	_data.i[2] = v2;
}


su::Any::Any	(	int16	v	)
{
	util::Zero( _data );
	_type = Short;
	_data.i[0] = v;
}


su::Any::Any	(	int16 v0, int16 v1	)
{
	util::Zero( _data );
	_type = Short2;
	_data.i[0] = v0;
	_data.i[1] = v1;
}


su::Any::Any	(	int16 v0, int16 v1, int16 v2	)
{
	util::Zero( _data );
	_type = Short3;
	_data.i[0] = v0;
	_data.i[1] = v1;
	_data.i[2] = v2;
}


su::Any::Any	(	int32	v	)
{
	util::Zero( _data );
	_type = Int;
	_data.i[0] = v;
}


su::Any::Any	(	int32 v0, int32 v1	)
{
	util::Zero( _data );
	_type = Int2;
	_data.i[0] = v0;
	_data.i[1] = v1;
}


su::Any::Any	(	int32 v0, int32 v1, int32 v2	)
{
	util::Zero( _data );
	_type = Int3;
	_data.i[0] = v0;
	_data.i[1] = v1;
	_data.i[2] = v2;
}


su::Any::Any	(	uint8	v	)
{
	util::Zero( _data );
	_type = ByteU;
	_data.i[0] = v;
}


su::Any::Any	(	uint8 v0, uint8 v1	)
{
	util::Zero( _data );
	_type = Byte2U;
	_data.i[0] = v0;
	_data.i[1] = v1;
}


su::Any::Any	(	uint8 v0, uint8 v1, uint8 v2	)
{
	util::Zero( _data );
	_type = Byte3U;
	_data.i[0] = v0;
	_data.i[1] = v1;
	_data.i[2] = v2;
}


su::Any::Any	(	uint16	v	)
{
	util::Zero( _data );
	_type = ShortU;
	_data.i[0] = v;
}


su::Any::Any	(	uint16 v0, uint16 v1	)
{
	util::Zero( _data );
	_type = Short2U;
	_data.i[0] = v0;
	_data.i[1] = v1;
}


su::Any::Any	(	uint16 v0, uint16 v1, uint16 v2	)
{
	util::Zero( _data );
	_type = Short3U;
	_data.i[0] = v0;
	_data.i[1] = v1;
	_data.i[2] = v2;
}


su::Any::Any	(	uint32	v	)
{
	util::Zero( _data );
	_type = IntU;
	_data.i[0] = v;
}


su::Any::Any	(	uint32 v0, uint32 v1	)
{
	util::Zero( _data );
	_type = Int2U;
	_data.i[0] = v0;
	_data.i[1] = v1;
}


su::Any::Any	(	uint32 v0, uint32 v1, uint32 v2	)
{
	util::Zero( _data );
	_type = Int3U;
	_data.i[0] = v0;
	_data.i[1] = v1;
	_data.i[2] = v2;
}


su::Any::Any	(	float	v	)
{
	util::Zero( _data );
	_type = Float;
	_data.f[0] = v;
}


su::Any::Any	(	float v0, float v1	)
{
	util::Zero( _data );
	_type = Float2;
	_data.f[0] = v0;
	_data.f[1] = v1;
}


su::Any::Any	(	float v0, float v1, float v2	)
{
	util::Zero( _data );
	_type = Float3;
	_data.f[0] = v0;
	_data.f[1] = v1;
	_data.f[2] = v2;
}


su::Any::Any	(	float v0, float v1, float v2, float v3	)
{
	util::Zero( _data );
	_type = Float4;
	_data.f[0] = v0;
	_data.f[1] = v1;
	_data.f[2] = v2;
	_data.f[3] = v3;
}


su::Any::Any	(	const Vec2 &	v	)
{
	util::Zero( _data );
	_type = Float2;
	_data.f[0] = v.x;
	_data.f[1] = v.y;
}


su::Any::Any	(	const Vec3 &	v	)
{
	util::Zero( _data );
	_type = Float3;
	_data.f[0] = v.x;
	_data.f[1] = v.y;
	_data.f[2] = v.z;
}


su::Any::Any	(	const Vec4 &	v	)
{
	util::Zero( _data );
	_type = Float4;
	_data.f[0] = v.x;
	_data.f[1] = v.y;
	_data.f[2] = v.z;
	_data.f[3] = v.w;
}


su::Any::Any	(	const Quat &	v	)
{
	util::Zero( _data );
	_type = Float4;
	_data.f[0] = v.x;
	_data.f[1] = v.y;
	_data.f[2] = v.z;
	_data.f[3] = v.w;
}


su::Any::Any	(	const Matrix &	v	)
{
	util::Zero( _data );
	_type = Matrix44;
	MatrixStore( _data.f, (Matrix*)&v );
}


su::Any::Any	(	pcstr		v	)
{
	util::Zero( _data );
	_type = String;
	_data.addr = (void*)v;
}


su::Any::Any	(	void*		v	)
{
	util::Zero( _data );
	_type = Addr;
	_data.addr = v;
}


su::Any::Any	(	IInterface*		v	)
{
	util::Zero( _data );
	_type = Object;
	_data.addr = v;
}


void su::Any::copy	( const Any& v )
{
	_type = v._type;
	_data = v._data;
}



void su::Any::undef ( )
{
	util::Zero( _data );
	_type = Undefined;
}


su::Any::Type
su::Any::getType	(		) const
{
	return _type;
}


bool
su::Any::isType	(	Type	t		) const
{
	return _type==t;
}


bool
su::Any::isDefined	(			) const
{
	return _type!=Undefined;
}


bool
su::Any::isBool	(			) const
{
	return _type==Boolean;
}


bool
su::Any::isByte	(			) const
{
	return (_type>=Byte && _type<=Byte3) || (_type>=ByteU && _type<=Byte3U);
}


bool
su::Any::isShort	(			) const
{
	return (_type>=Short && _type<=Short3) || (_type>=ShortU && _type<=Short3U);
}


bool
su::Any::isInt	(			) const
{
	return (_type>=Int && _type<=Int3) || (_type>=IntU && _type<=Int3U);
}


bool
su::Any::isSigned	(			) const
{
	return (_type>=Byte && _type<=Int3) || (_type>=Float && _type<=Float4);
}


bool
su::Any::isFloat	(			) const
{
	return (_type>=Float && _type<=Float4);
}


bool
su::Any::isNumeric	(		)	const
{
	return (_type>=Byte && _type<=Float4);
}


bool
su::Any::isMatrix	(			) const
{
	return _type==Matrix44;
}


bool
su::Any::isString	(			) const
{
	return _type==String;
}


bool
su::Any::isAddr	(			) const
{
	return _type==Addr;
}


bool
su::Any::isObject	(			) const
{
	return _type==Object;
}


bool
su::Any::isPointer	(			) const
{
	return (_type>=String && _type<=Object);
}


bool
su::Any::isNull	(			) const
{
	return isPointer() && (_data.addr==NULL);
}


bool
su::Any::isNotNull	(			) const
{
	return isPointer() && (_data.addr!=NULL);
}


bool
su::Any::isClassId	(	classid		cid		) const
{
	IInterface* ci = (IInterface*) _data.addr;
	return isObject() && ci && ci->HasInterface(cid);
}


bool
su::Any::isSingle	(			) const
{
	return isBool() || isPointer() || (_type==Byte) || (_type==ByteU) || (_type==Short) || (_type==ShortU) || (_type==Int) || (_type==IntU) || (_type==Float);
}


bool
su::Any::isPair	(			) const
{
	return (_type==Byte2) || (_type==Byte2U) || (_type==Short2) || (_type==Short2U) || (_type==Int2) || (_type==Int2U) || (_type==Float2);
}


bool
su::Any::isTriple	(			) const
{
	return (_type==Byte3) || (_type==Byte3U) || (_type==Short3) || (_type==Short3U) || (_type==Int3) || (_type==Int2U) || (_type==Float3);
}


bool
su::Any::isQuad	(			) const
{
	return (_type==Float4);
}


bool
su::Any::isTuple	(	uint	n	) const
{
	if( n==1 )		return isSingle();
	if( n==2 )		return isPair();
	if( n==3 )		return isTriple();
	if( n==4 )		return isQuad();
	return false;
}


uint
su::Any::getDim	(	) const
{
	if( isSingle() )	return 1;
	if( isPair() )		return 2;
	if( isTriple() )	return 3;
	if( isQuad() )		return 3;
	return 0;
}


bool
su::Any::lockObject	(			)
{
	IInterface* ci = (IInterface*) _data.addr;
	if( isObject() && ci )
	{
		ci->Retain();
		return true;
	}
	else
	{
		return false;
	}
}


bool
su::Any::unlockObject	(		)
{
	IInterface* ci = (IInterface*) _data.addr;
	if( isObject() && ci )
	{
		uint cr = ci->CountRef();
		SU_ASSERT( cr >= 1 );
		ci->Release();
		if( cr==1 )	_data.addr = 0;
		return true;
	}
	else
	{
		return false;
	}
}


void*
su::Any::data		(		) const
{
	return (void*)&_data;
}


bool
su::Any::asBool		(			) const
{
	if( isBool() )		return _data.b;
	if( isFloat() )		return _data.f[0]!=0.f;
	if( isNumeric() )	return _data.i[0]!=0;
	if( isPointer() )	return _data.addr!=NULL;
	return false;
}


int32
su::Any::asInt		(			) const
{
	if( isBool() )		return _data.b;
	if( isFloat() )		return _data.f[0];
	if( isNumeric() )	return _data.i[0];
	if( isPointer() )	return (int32)_data.addr;
	return 0;
}


uint32
su::Any::asIntU		(			) const
{
	if( isBool() )		return _data.b;
	if( isFloat() )		return _data.f[0];
	if( isNumeric() )	return _data.i[0];
	if( isPointer() )	return (uint32)_data.addr;
	return 0U;
}


float
su::Any::asFloat		(			) const
{
	if( isBool() )		return _data.b;
	if( isFloat() )		return _data.f[0];
	if( isNumeric() )	return _data.i[0];
	return 0.f;
}


Vec2
su::Any::asVec2		(			) const
{
	if( isBool() )		return Vec2(_data.b,0);
	if( isFloat() )		return Vec2(_data.f[0],_data.f[1]);
	if( isNumeric() )	return Vec2(_data.i[0],_data.i[1]);
	return Vec2::ZERO;
}


Vec3
su::Any::asVec3		(			) const
{
	if( isBool() )		return Vec3(_data.b,0,0);
	if( isFloat() )		return Vec3(_data.f[0],_data.f[1],_data.f[2]);
	if( isNumeric() )	return Vec3(_data.i[0],_data.i[1],_data.i[2]);
	return Vec3::ZERO;
}


Vec4
su::Any::asVec4		(			) const
{
	if( isBool() )		return Vec4(_data.b,0,0,0);
	if( isFloat() )		return Vec4(_data.f[0],_data.f[1],_data.f[2],_data.f[3]);
	if( isNumeric() )	return Vec4(_data.i[0],_data.i[1],_data.i[2],0);
	return Vec4::ZERO;
}


Quat
su::Any::asQuat		(			) const
{
	return Quat( asVec4() );
}


Matrix
su::Any::asMatrix		(			) const
{
	if( isMatrix() )	return Matrix( &_data.f[0] );
	else				return Matrix::ZERO;
}


pcstr
su::Any::asString		(			) const
{
	if( isString() )	return (pcstr) _data.addr;
	else				return NULL;
}


void*
su::Any::asAddr		(			) const
{
	if( isAddr() )		return _data.addr;
	else				return NULL;
}


IInterface*
su::Any::asObject		(			) const
{
	if( isObject() )	return (IInterface*) _data.addr;
	else				return NULL;
}


void*
su::Any::asPointer	(			) const
{
	if( isPointer() )	return _data.addr;
	else				return NULL;
}


bool
su::Any::get			(	bool&		v		) const
{
	if( _type==Boolean )
	{
		v = _data.b;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get	(	uint8&	v	) const
{
	if( _type==ByteU )
	{
		v = _data.i[0];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get	(	uint8& v0, uint8& v1	) const
{
	if( _type==Byte2U )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get	(	uint8& v0, uint8& v1, uint8& v2	) const
{
	if( _type==Byte3U )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		v2 = _data.i[2];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get	(	uint16& v	) const
{
	if( _type==ByteU || _type==ShortU )
	{
		v = _data.i[0];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get	(	uint16& v0, uint16& v1	) const
{
	if( _type==Byte2U || _type==Short2U )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get	(	uint16& v0, uint16& v1, uint16& v2	) const
{
	if( _type==Byte3U || _type==Short3U )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		v2 = _data.i[2];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get		(	uint32&	v	) const
{
	if( _type==ByteU || _type==ShortU || _type==IntU )
	{
		v = _data.i[0];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	uint32& v0, uint32& v1	) const
{
	if( _type==Byte2U || _type==Short2U || _type==Int2U )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	uint32& v0, uint32& v1, uint32& v2	) const
{
	if( _type==Byte3U || _type==Short3U || _type==Int3U )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		v2 = _data.i[2];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	int8&	v	) const
{
	if( _type==Byte )
	{
		v = _data.i[0];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	int8& v0, int8& v1		) const
{
	if( _type==Byte2 )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	int8& v0, int8& v1, int8& v2	) const
{
	if( _type==Byte3 )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		v2 = _data.i[2];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	int16& v	) const
{
	if( _type==Byte || _type==Short || _type==ByteU )
	{
		v = _data.i[0];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	int16& v0, int16& v1	) const
{
	if( _type==Byte2 || _type==Short2 || _type==Byte2U )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	int16& v0, int16& v1, int16& v2	) const
{
	if( _type==Byte3 || _type==Short3 || _type==Byte3U )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		v2 = _data.i[2];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	int32&	v	) const
{
	if( _type==Byte || _type==Short || _type==Int || _type==ByteU || _type==ShortU )
	{
		v = _data.i[0];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	int32& v0, int32& v1	) const
{
	if( _type==Byte2 || _type==Short2 || _type==Int2 || _type==Byte2U || _type==Short2U )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	int32& v0, int32& v1, int32& v2	) const
{
	if( _type==Byte3 || _type==Short3 || _type==Int3 || _type==Byte3U || _type==Short3U )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		v2 = _data.i[2];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	float& v	) const
{
	if( _type==Float )
	{
		v = _data.f[0];
		return TRUE;
	}
	else if( _type==Byte || _type==Short || _type==ByteU || _type==ShortU )
	{
		v = _data.i[0];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get		(	float& v0, float& v1	) const
{
	if( _type==Float2 )
	{
		v0 = _data.f[0];
		v1 = _data.f[1];
		return TRUE;
	}
	else if( _type==Byte2 || _type==Short2 || _type==Byte2U || _type==Short2U )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	float& v0, float& v1, float& v2		) const
{
	if( _type==Float3 )
	{
		v0 = _data.f[0];
		v1 = _data.f[1];
		v2 = _data.f[2];
		return TRUE;
	}
	else if( _type==Byte3 || _type==Short3 || _type==Byte3U || _type==Short3U )
	{
		v0 = _data.i[0];
		v1 = _data.i[1];
		v2 = _data.i[2];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get		(	float& v0, float& v1, float& v2, float& v3	) const
{
	if( _type==Float4 )
	{
		v0 = _data.f[0];
		v1 = _data.f[1];
		v2 = _data.f[2];
		v3 = _data.f[3];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get		(	Vec2& v	) const
{
	return get(v.x,v.y);
}


bool
su::Any::get		(	Vec3& v	) const
{
	return get(v.x,v.y,v.z);
}


bool
su::Any::get		(	Vec4& v	) const
{
	return get(v.x,v.y,v.z,v.w);
}


bool
su::Any::get		(	Quat& v	) const
{
	return get(v.x,v.y,v.z,v.w);
}


bool
su::Any::get		(	Matrix&		v		) const
{
	if( _type==Matrix44 )
	{
		MatrixLoad( &v, (float*)_data.f );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get		(	pcstr&	v	) const
{
	if( _type==String )
	{
		v = (pcstr)_data.addr;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	void*&		v		) const
{
	if( _type==Addr )
	{
		v = _data.addr;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::get			(	IInterface*&	v	) const
{
	if( _type==Object )
	{
		v = (IInterface*)_data.addr;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



bool
su::Any::set		(	const Any&	v	)
{
	_type = v._type;
	_data = v._data;
	return TRUE;
}


bool
su::Any::set		(	bool	v		)
{
	util::Zero( _data );
	_type = Boolean;
	_data.b = v;
	return TRUE;
}


bool
su::Any::set		(	uint8	v		)
{
	util::Zero( _data );
	_type = ByteU;
	_data.i[0] = v;
	return TRUE;
}


bool
su::Any::set		(	uint8 v0, uint8 v1		)
{
	util::Zero( _data );
	_type = Byte2U;
	_data.i[0] = v0;
	_data.i[1] = v1;
	return TRUE;
}


bool
su::Any::set		(	uint8 v0, uint8 v1, uint8 v2	)
{
	util::Zero( _data );
	_type = Byte3U;
	_data.i[0] = v0;
	_data.i[1] = v1;
	_data.i[2] = v2;
	return TRUE;
}


bool
su::Any::set		(	uint16	v		)
{
	util::Zero( _data );
	_type = ShortU;
	_data.i[0] = v;
	return TRUE;
}


bool
su::Any::set		(	uint16 v0, uint16 v1		)
{
	util::Zero( _data );
	_type = Short2U;
	_data.i[0] = v0;
	_data.i[1] = v1;
	return TRUE;
}


bool
su::Any::set		(	uint16 v0, uint16 v1, uint16 v2	)
{
	util::Zero( _data );
	_type = Short3U;
	_data.i[0] = v0;
	_data.i[1] = v1;
	_data.i[2] = v2;
	return TRUE;
}


bool
su::Any::set		(	uint32	v		)
{
	util::Zero( _data );
	_type = IntU;
	_data.i[0] = v;
	return TRUE;
}


bool
su::Any::set		(	uint32 v0, uint32 v1		)
{
	util::Zero( _data );
	_type = Int2U;
	_data.i[0] = v0;
	_data.i[1] = v1;
	return TRUE;
}


bool
su::Any::set		(	uint32 v0, uint32 v1, uint32 v2	)
{
	util::Zero( _data );
	_type = Int3U;
	_data.i[0] = v0;
	_data.i[1] = v1;
	_data.i[2] = v2;
	return TRUE;
}



bool
su::Any::set		(	int8	v		)
{
	util::Zero( _data );
	_type = Byte;
	_data.i[0] = v;
	return TRUE;
}


bool
su::Any::set		(	int8 v0, int8 v1		)
{
	util::Zero( _data );
	_type = Byte2;
	_data.i[0] = v0;
	_data.i[1] = v1;
	return TRUE;
}


bool
su::Any::set		(	int8 v0, int8 v1, int8 v2	)
{
	util::Zero( _data );
	_type = Byte3;
	_data.i[0] = v0;
	_data.i[1] = v1;
	_data.i[2] = v2;
	return TRUE;
}


bool
su::Any::set		(	int16	v		)
{
	util::Zero( _data );
	_type = Short;
	_data.i[0] = v;
	return TRUE;
}


bool
su::Any::set		(	int16 v0, int16 v1		)
{
	util::Zero( _data );
	_type = Short2;
	_data.i[0] = v0;
	_data.i[1] = v1;
	return TRUE;
}


bool
su::Any::set		(	int16 v0, int16 v1, int16 v2	)
{
	util::Zero( _data );
	_type = Short3;
	_data.i[0] = v0;
	_data.i[1] = v1;
	_data.i[2] = v2;
	return TRUE;
}


bool
su::Any::set		(	int32	v		)
{
	util::Zero( _data );
	_type = Int;
	_data.i[0] = v;
	return TRUE;
}


bool
su::Any::set		(	int32 v0, int32 v1		)
{
	util::Zero( _data );
	_type = Int2;
	_data.i[0] = v0;
	_data.i[1] = v1;
	return TRUE;
}


bool
su::Any::set		(	int32 v0, int32 v1, int32 v2	)
{
	util::Zero( _data );
	_type = Int3;
	_data.i[0] = v0;
	_data.i[1] = v1;
	_data.i[2] = v2;
	return TRUE;
}


bool
su::Any::set		(	float v		)
{
	util::Zero( _data );
	_type = Float;
	_data.f[0] = v;
	return TRUE;
}


bool
su::Any::set		(	float v0, float v1	)
{
	util::Zero( _data );
	_type = Float2;
	_data.f[0] = v0;
	_data.f[1] = v1;
	return TRUE;
}


bool
su::Any::set		(	float v0, float v1, float v2	)
{
	util::Zero( _data );
	_type = Float3;
	_data.f[0] = v0;
	_data.f[1] = v1;
	_data.f[2] = v2;
	return TRUE;
}


bool
su::Any::set		(	float v0, float v1, float v2, float v3	)
{
	util::Zero( _data );
	_type = Float4;
	_data.f[0] = v0;
	_data.f[1] = v1;
	_data.f[2] = v2;
	_data.f[3] = v3;
	return TRUE;
}


bool
su::Any::set		(	const Vec2& v	)
{
	return set( v.x, v.y );
}


bool
su::Any::set		(	const Vec3& v	)
{
	return set( v.x, v.y, v.z );
}


bool
su::Any::set		(	const Vec4& v	)
{
	return set( v.x, v.y, v.z );
}


bool
su::Any::set		(	const Quat& v	)
{
	return set( v.x, v.y, v.z, v.w );
}


bool
su::Any::set	(	const Matrix&	v	)
{
	util::Zero( _data );
	_type = Matrix44;
	MatrixStore( &_data.f[0], (Matrix*)&v );
	return TRUE;
}


bool
su::Any::set	(	pcstr	v		)
{
	util::Zero( _data );
	_type = String;
	_data.addr = (void*)v;
	return TRUE;
}


bool
su::Any::set		(	void*	v		)
{
	util::Zero( _data );
	_type = Addr;
	_data.addr = v;
	return TRUE;
}


bool
su::Any::set		(	IInterface*		v	)
{
	util::Zero( _data );
	_type = Object;
	_data.addr = v;
	return TRUE;
}


bool
su::Any::assign	(	bool b	)
{
	if( _type==Boolean )
	{
		_data.b = b;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	uint8	v	)
{
	if( _type==ByteU || _type==ShortU || _type==IntU || _type==Short || _type==Int )
	{
		_data.i[0] = v;
		return TRUE;
	}
	else if( _type==Float )
	{
		_data.f[0] = v;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	uint8	v0, uint8	v1	)
{
	if( _type==Byte2U || _type==Short2U || _type==Int2U || _type==Short2 || _type==Int2 )
	{
		_data.i[0] = v0;
		_data.i[1] = v1;
		return TRUE;
	}
	else if( _type==Float2 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	uint8	v0, uint8	v1, uint8	v2	)
{
	if( _type==Byte3U || _type==Short3U || _type==Int3U || _type==Short3 || _type==Int3 )
	{
		_data.i[0] = v0;
		_data.i[1] = v1;
		_data.i[2] = v2;
		return TRUE;
	}
	else if( _type==Float3 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		_data.f[2] = v2;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	uint16	v	)
{
	if( _type==ShortU || _type==IntU || _type==Int )
	{
		_data.i[0] = v;
		return TRUE;
	}
	else if( _type==Float )
	{
		_data.f[0] = v;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	uint16	v0, uint16	v1	)
{
	if( _type==Short2U || _type==Int2U || _type==Int2 )
	{
		_data.i[0] = v0;
		_data.i[1] = v1;
		return TRUE;
	}
	else if( _type==Float2 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	uint16	v0, uint16	v1, uint16	v2	)
{
	if( _type==Short3U || _type==Int3U || _type==Int3 )
	{
		_data.i[0] = v0;
		_data.i[1] = v1;
		_data.i[2] = v2;
		return TRUE;
	}
	else if( _type==Float3 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		_data.f[2] = v2;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	uint32	v	)
{
	if( _type==IntU )
	{
		_data.i[0] = v;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	uint32	v0, uint32	v1	)
{
	if( _type==Int2U )
	{
		_data.i[0] = v0;
		_data.i[1] = v1;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	uint32	v0, uint32	v1, uint32	v2	)
{
	if( _type==Int3U )
	{
		_data.i[0] = v0;
		_data.i[1] = v1;
		_data.i[2] = v2;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	int8	v	)
{
	if( _type==Byte || _type==Short || _type==Int )
	{
		_data.i[0] = v;
		return TRUE;
	}
	else if( _type==Float )
	{
		_data.f[0] = v;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	int8	v0, int8	v1	)
{
	if( _type==Byte2 || _type==Short2 || _type==Int2 )
	{
		_data.i[0] = v0;
		_data.i[1] = v1;
		return TRUE;
	}
	else if( _type==Float2 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	int8	v0, int8	v1, int8	v2	)
{
	if( _type==Byte3 || _type==Short3 || _type==Int3 )
	{
		_data.i[0] = v0;
		_data.i[1] = v1;
		_data.i[2] = v2;
		return TRUE;
	}
	else if( _type==Float3 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		_data.f[2] = v2;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	int16	v	)
{
	if( _type==Short || _type==Int )
	{
		_data.i[0] = v;
		return TRUE;
	}
	else if( _type==Float )
	{
		_data.f[0] = v;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	int16	v0, int16	v1	)
{
	if( _type==Short2 || _type==Int2 )
	{
		_data.i[0] = v0;
		_data.i[1] = v1;
		return TRUE;
	}
	else if( _type==Float2 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	int16	v0, int16	v1, int16	v2	)
{
	if( _type==Short3 || _type==Int3 )
	{
		_data.i[0] = v0;
		_data.i[1] = v1;
		_data.i[2] = v2;
		return TRUE;
	}
	else if( _type==Float3 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		_data.f[2] = v2;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	int32	v	)
{
	if( _type==Int )
	{
		_data.i[0] = v;
		return TRUE;
	}
	else if( _type==Float )
	{
		_data.f[0] = v;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	int32	v0, int32	v1	)
{
	if( _type==Int2 )
	{
		_data.i[0] = v0;
		_data.i[1] = v1;
		return TRUE;
	}
	else if( _type==Float2 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	int32	v0, int32	v1, int32	v2	)
{
	if( _type==Int3 )
	{
		_data.i[0] = v0;
		_data.i[1] = v1;
		_data.i[2] = v2;
		return TRUE;
	}
	else if( _type==Float3 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		_data.f[2] = v2;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	float	v	)
{
	if( _type==Float )
	{
		_data.f[0] = v;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	float	v0, float	v1	)
{
	if( _type==Float2 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	float	v0, float	v1, float	v2	)
{
	if( _type==Float3 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		_data.f[2] = v2;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	float	v0, float	v1, float	v2, float	v3	)
{
	if( _type==Float3 )
	{
		_data.f[0] = v0;
		_data.f[1] = v1;
		_data.f[2] = v2;
		_data.f[3] = v3;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	const Vec2&	v	)
{
	return assign( v.x, v.y );
}


bool
su::Any::assign	(	const Vec3&	v	)
{
	return assign( v.x, v.y, v.z );
}


bool
su::Any::assign	(	const Vec4&	v	)
{
	return assign( v.x, v.y, v.z, v.w );
}


bool
su::Any::assign	(	const Quat&	v	)
{
	return assign( v.x, v.y, v.z, v.w );
}


bool
su::Any::assign	(	const Matrix& v	)
{
	if( _type==Matrix44 )
	{
		MatrixStore( _data.f, (Matrix*)&v );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	pcstr v	)
{
	if( _type==String )
	{
		_data.addr = (void*) v;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	void* v	)
{
	if( _type==Addr )
	{
		_data.addr = v;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	IInterface* v	)
{
	if( _type==Object )
	{
		_data.addr = v;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool
su::Any::assign	(	const Any&	v	)
{
	if( v._type==Boolean )		return assign( (bool)v._data.b );
	if( v._type==Byte )			return assign( (int8)v._data.i[0] );
	if( v._type==Byte2 )		return assign( (int8)v._data.i[0], (int8)v._data.i[1] );
	if( v._type==Byte3 )		return assign( (int8)v._data.i[0], (int8)v._data.i[1], (int8)v._data.i[2] );
	if( v._type==Short )		return assign( (int16)v._data.i[0] );
	if( v._type==Short2 )		return assign( (int16)v._data.i[0], (int16)v._data.i[1] );
	if( v._type==Short3 )		return assign( (int16)v._data.i[0], (int16)v._data.i[1], (int16)v._data.i[2] );
	if( v._type==Int )			return assign( (int32)v._data.i[0] );
	if( v._type==Int2 )			return assign( (int32)v._data.i[0], (int32)v._data.i[1] );
	if( v._type==Int3 )			return assign( (int32)v._data.i[0], (int32)v._data.i[1], (int32)v._data.i[2] );
	if( v._type==ByteU )		return assign( (uint8)v._data.i[0] );
	if( v._type==Byte2U )		return assign( (uint8)v._data.i[0], (uint8)v._data.i[1] );
	if( v._type==Byte3U )		return assign( (uint8)v._data.i[0], (uint8)v._data.i[1], (uint8)v._data.i[2] );
	if( v._type==ShortU )		return assign( (uint16)v._data.i[0] );
	if( v._type==Short2U )		return assign( (uint16)v._data.i[0], (uint16)v._data.i[1] );
	if( v._type==Short3U )		return assign( (uint16)v._data.i[0], (uint16)v._data.i[1], (uint16)v._data.i[2] );
	if( v._type==IntU )			return assign( (uint32)v._data.i[0] );
	if( v._type==Int2U )		return assign( (uint32)v._data.i[0], (uint32)v._data.i[1] );
	if( v._type==Int3U )		return assign( (uint32)v._data.i[0], (uint32)v._data.i[1], (uint32)v._data.i[2] );
	if( v._type==Float )		return assign( v._data.f[0] );
	if( v._type==Float2 )		return assign( v._data.f[0], v._data.f[1] );
	if( v._type==Float3 )		return assign( v._data.f[0], v._data.f[1], v._data.f[2] );
	if( v._type==Float4 )		return assign( v._data.f[0], v._data.f[1], v._data.f[2], v._data.f[3] );
	if( v._type==Matrix44 )		return assign( Matrix(v._data.f) );
	if( v._type==String )		return assign( (pcstr)v._data.addr );
	if( v._type==Addr )			return assign( v._data.addr );
	if( v._type==Object )		return assign( (IInterface*)v._data.addr );
	return FALSE;
}




su::AnyList::AnyList ( )
{
	vfirst = vend = NULL;
}
						

su::AnyList::AnyList ( AnyVar* infirst, AnyVar* inend )
{
	SU_ASSERT( infirst );
	SU_ASSERT( inend );
	vfirst = infirst;
	vend   = inend;
}


uint
su::AnyList::size ( ) const
{
	return vend - vfirst;
}


su::Any*
su::AnyList::operator [] ( int i ) const
{
	if( i>=0 && i<(int)size() )
		return &(vfirst[i].value);
	else
		return NULL;
}


su::AnyVar*
su::AnyList::get ( int i ) const
{
	if( i>=0 && i<(int)size() )
		return &vfirst[i];
	else
		return NULL;
}


su::Any*
su::AnyList::get ( pcstr n, bool cs ) const
{
	AnyVar* v = vfirst;
	while( v < vend )
	{
		if( util::Strcompare(v->name,n,cs)==0 )
			return &v->value;
		v++;
	}
	return NULL;
}


bool
su::AnyList::isType ( pcstr n, Any::Type t, bool cs ) const
{
	Any* a = get( n, cs );
	if( !a )	return false;
	else		return a->isType(t);
}


su::Any*
su::AnyList::get ( pcstr n, Any::Type t, bool cs ) const
{
	Any* a = get( n, cs );
	if( !a )			return NULL;
	if( !a->isType(t) )	return NULL;
	return a;
}



su::AnyVar::AnyVar ( )
{
	name = NULL;
}




bool
su::AnyList::asBool ( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asBool();
	else		return false;
}


int32
su::AnyList::asInt( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asInt();
	else		return 0;
}


uint32
su::AnyList::asIntU( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asIntU();
	else		return 0U;
}


float
su::AnyList::asFloat( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asFloat();
	else		return 0.f;
}


su::Vec2
su::AnyList::asVec2( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asVec2();
	else		return Vec2::ZERO;
}


su::Vec3
su::AnyList::asVec3( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asVec3();
	else		return Vec3::ZERO;
}


su::Vec4
su::AnyList::asVec4( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asVec4();
	else		return Vec4::ZERO;
}


su::Quat
su::AnyList::asQuat( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asQuat();
	else		return Quat::ZERO;
}


su::Matrix
su::AnyList::asMatrix( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asMatrix();
	else		return Matrix::ZERO;
}


pcstr
su::AnyList::asString( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asString();
	else		return NULL;
}


void*
su::AnyList::asAddr( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asAddr();
	else		return NULL;
}


su::IInterface*
su::AnyList::asObject( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asObject();
	else		return NULL;
}


void*
su::AnyList::asPointer( pcstr n, bool cs ) const
{
	Any* a = get(n,cs);
	if( a )		return a->asPointer();
	else		return NULL;
}



