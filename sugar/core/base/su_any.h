/*LIC-HDR***********_su_any_h_*********************************************************
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



#ifndef _su_any_h_
#define _su_any_h_




namespace su
{


	//
	// Any: a basic multi-typed content object

	struct_ie Any
	{

		enum Type
		{
			Undefined=0,
			Boolean,
			// signed
			Byte,
			Byte2,
			Byte3,
			Short,
			Short2,
			Short3,
			Int,
			Int2,
			Int3,
			// unsigned
			ByteU,
			Byte2U,
			Byte3U,
			ShortU,
			Short2U,
			Short3U,
			IntU,
			Int2U,
			Int3U,
			// fp
			Float,
			Float2,
			Float3,
			Float4,
			Matrix44,
			// pointer
			String,
			Addr,
			Object
		};


							Any			(									);
							Any			(	const Any&						);
							Any			(	bool							);
							Any			(	uint8							);
							Any			(	uint8, uint8					);
							Any			(	uint8, uint8, uint8				);
							Any			(	uint16							);
							Any			(	uint16, uint16					);
							Any			(	uint16, uint16, uint16			);
							Any			(	uint32							);
							Any			(	uint32, uint32					);
							Any			(	uint32, uint32, uint32			);
							Any			(	int8							);
							Any			(	int8, int8						);
							Any			(	int8, int8, int8				);
							Any			(	int16							);
							Any			(	int16, int16					);
							Any			(	int16, int16, int16				);
							Any			(	int32							);
							Any			(	int32, int32					);
							Any			(	int32, int32, int32				);
							Any			(	float							);
							Any			(	float, float					);
							Any			(	float, float, float				);
							Any			(	float, float, float, float		);
							Any			(	const Vec2&						);
							Any			(	const Vec3&						);
							Any			(	const Vec4&						);
							Any			(	const Quat&						);
							Any			(	const Matrix&					);
							Any			(	pcstr							);
							Any			(	void*							);
							Any			(	IInterface*						);

		void				undef		(									);
		void				copy		(	const Any&						);

		bool				isType		(	Type 							) const;
		bool				isDefined	(									) const;
		bool				isBool		(									) const;
		bool				isByte		(									) const;
		bool				isShort		(									) const;
		bool				isInt		(									) const;
		bool				isSigned	(									) const;
		bool				isFloat		(									) const;
		bool				isNumeric	(									) const;
		bool				isMatrix	(									) const;
		bool				isString	(									) const;
		bool				isAddr		(									) const;
		bool				isObject	(									) const;
		bool				isPointer	(									) const;
		bool				isNull		(									) const;
		bool				isNotNull	(									) const;
		bool				isClassId	(	classid							) const;
		bool				isSingle	(									) const;
		bool				isPair		(									) const;
		bool				isTriple	(									) const;
		bool				isQuad		(									) const;
		bool				isTuple		(	uint							) const;
		uint				getDim		(									) const;
		Type				getType		(									) const;
		bool				lockObject	(									);
		bool				unlockObject(									);
		void*				data		(									) const;


		bool				asBool		(									) const;
		int32				asInt		(									) const;
		uint32				asIntU		(									) const;
		float				asFloat		(									) const;
		Vec2				asVec2		(									) const;
		Vec3				asVec3		(									) const;
		Vec4				asVec4		(									) const;
		Quat				asQuat		(									) const;
		Matrix				asMatrix	(									) const;
		pcstr				asString	(									) const;
		void*				asAddr		(									) const;
		IInterface*			asObject	(									) const;
		template <typename T>
		T*					asObject	(									) const;
		void*				asPointer	(									) const;

		bool				get			(	bool&							) const;
		bool				get			(	uint8&							) const;
		bool				get			(	uint8&, uint8&					) const;
		bool				get			(	uint8&, uint8&, uint8&			) const;
		bool				get			(	uint16&							) const;
		bool				get			(	uint16&, uint16&				) const;
		bool				get			(	uint16&, uint16&, uint16&		) const;
		bool				get			(	uint32&							) const;
		bool				get			(	uint32&, uint32&				) const;
		bool				get			(	uint32&, uint32&, uint32&		) const;
		bool				get			(	int8&							) const;
		bool				get			(	int8&, int8&					) const;
		bool				get			(	int8&, int8&, int8&				) const;
		bool				get			(	int16&							) const;
		bool				get			(	int16&, int16&					) const;
		bool				get			(	int16&, int16&, int16&			) const;
		bool				get			(	int32&							) const;
		bool				get			(	int32&, int32&					) const;
		bool				get			(	int32&, int32&, int32&			) const;
		bool				get			(	float&							) const;
		bool				get			(	float&, float&					) const;
		bool				get			(	float&, float&, float&			) const;
		bool				get			(	float&, float&, float&, float&	) const;
		bool				get			(	Vec2&							) const;
		bool				get			(	Vec3&							) const;
		bool				get			(	Vec4&							) const;
		bool				get			(	Quat&							) const;
		bool				get			(	Matrix&							) const;
		bool				get			(	pcstr&							) const;
		bool				get			(	void*&							) const;
		bool				get			(	IInterface*&					) const;

		// change type and data
		bool				set			(	const Any&						);
		bool				set			(	bool							);
		bool				set			(	uint8							);
		bool				set			(	uint8, uint8					);
		bool				set			(	uint8, uint8, uint8				);
		bool				set			(	uint16							);
		bool				set			(	uint16, uint16					);
		bool				set			(	uint16, uint16, uint16			);
		bool				set			(	uint32							);
		bool				set			(	uint32, uint32					);
		bool				set			(	uint32, uint32, uint32			);
		bool				set			(	int8							);
		bool				set			(	int8, int8						);
		bool				set			(	int8, int8, int8				);
		bool				set			(	int16							);
		bool				set			(	int16, int16					);
		bool				set			(	int16, int16, int16				);
		bool				set			(	int32							);
		bool				set			(	int32, int32					);
		bool				set			(	int32, int32, int32				);
		bool				set			(	float							);
		bool				set			(	float, float					);
		bool				set			(	float, float, float				);
		bool				set			(	float, float, float, float		);
		bool				set			(	const Vec2&						);
		bool				set			(	const Vec3&						);
		bool				set			(	const Vec4&						);
		bool				set			(	const Quat&						);
		bool				set			(	const Matrix&					);
		bool				set			(	pcstr							);
		bool				set			(	void*							);
		bool				set			(	IInterface*						);

		// change data only. returns FALSE if data can't be entirely stored & restored
		bool				assign		(	bool							);
		bool				assign		(	uint8							);
		bool				assign		(	uint8, uint8					);
		bool				assign		(	uint8, uint8, uint8				);
		bool				assign		(	uint16							);
		bool				assign		(	uint16, uint16					);
		bool				assign		(	uint16, uint16, uint16			);
		bool				assign		(	uint32							);
		bool				assign		(	uint32, uint32					);
		bool				assign		(	uint32, uint32, uint32			);
		bool				assign		(	int8							);
		bool				assign		(	int8, int8						);
		bool				assign		(	int8, int8, int8				);
		bool				assign		(	int16							);
		bool				assign		(	int16, int16					);
		bool				assign		(	int16, int16, int16				);
		bool				assign		(	int32							);
		bool				assign		(	int32, int32					);
		bool				assign		(	int32, int32, int32				);
		bool				assign		(	float							);
		bool				assign		(	float, float					);
		bool				assign		(	float, float, float				);
		bool				assign		(	float, float, float, float		);
		bool				assign		(	const Vec2&						);
		bool				assign		(	const Vec3&						);
		bool				assign		(	const Vec4&						);
		bool				assign		(	const Quat&						);
		bool				assign		(	const Matrix&					);
		bool				assign		(	pcstr							);
		bool				assign		(	void*							);
		bool				assign		(	IInterface*						);
		bool				assign		(	const Any&						);

		static const Any	True;
		static const Any	False;
		static const Any	Zero;
		static const Any	Null;
		static const Any	NullString;

	protected:
		union Data
		{
			bool			b;
			int				i[3];
			float			f[16];
			pointer			addr;
		};
		Type				_type;
		Data				_data;

		SU_NO_CMP_OPERATORS( Any )
	};



	//
	// AnyVar: A named Any value

	struct_ie AnyVar
	{
							AnyVar		(									);
		pcstr				name;
		Any					value;
	};



	//
	// AnyList: An interface to an AnyVar list

	struct_ie AnyList
	{
							AnyList		(									);
							AnyList		(	AnyVar*, AnyVar*				);

		uint				size		(									) const;
		Any*				operator []	(	int								) const;

		AnyVar*				get			(	int								) const;
		Any*				get			(	pcstr,            bool cs=false ) const;
		Any*				get			(	pcstr, Any::Type, bool cs=false	) const;
		bool				isType		(	pcstr, Any::Type, bool cs=false	) const;

		bool				asBool		(	pcstr,			  bool cs=false	) const;
		int32				asInt		(	pcstr,			  bool cs=false	) const;
		uint32				asIntU		(	pcstr,			  bool cs=false	) const;
		float				asFloat		(	pcstr,			  bool cs=false	) const;
		Vec2				asVec2		(	pcstr,			  bool cs=false	) const;
		Vec3				asVec3		(	pcstr,			  bool cs=false	) const;
		Vec4				asVec4		(	pcstr,			  bool cs=false	) const;
		Quat				asQuat		(	pcstr,			  bool cs=false	) const;
		Matrix				asMatrix	(	pcstr,			  bool cs=false	) const;
		pcstr				asString	(	pcstr,			  bool cs=false	) const;
		void*				asAddr		(	pcstr,			  bool cs=false	) const;
		IInterface*			asObject	(	pcstr,			  bool cs=false	) const;
		template <typename T>
		T*					asObject	(	pcstr,			  bool cs=false	) const;
		void*				asPointer	(	pcstr,			  bool cs=false	) const;

	protected:
		AnyVar				*vfirst, *vend;
	};



	//
	// AnyArray: An array of AnyVar

	template < uint Bound >
	struct_ie AnyArray : public AnyList
	{
							AnyArray	(									);
		void				set			(	int, pcstr, const Any&			);
		AnyList				list		(	int nb, int first=0				);
	protected:
		AnyVar				var[ Bound ];
	};


}



#include "su_anyi.h"
#endif // _su_any_h_


