/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2010 AtOnce Technologies
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



#ifndef _su_mcursor_h_
#define _su_mcursor_h_




namespace su
{

	struct IFileView;


	struct_ie MCursor
	{

		template <typename T>
		struct tpointer
		{
			T*							ptr;
			uint						bit;
			T*							operator ()		(			);
			T&							operator *		(			);
			T&							operator []		( int		);
			tpointer&					operator <<		( const T&	);
			tpointer&					operator >>		( T&		);
			tpointer&					operator =		( T*		);
			bool						operator ==		( T*		);
			tpointer&					operator ++		(			);
			tpointer					operator ++		( int		);
			tpointer&					operator --		(			);
			tpointer					operator --		( int		);
			tpointer					operator +		( int		) const;
			tpointer					operator -		( int		) const;
			tpointer&					operator +=		( int		);
			tpointer&					operator -=		( int		);
			int							operator -		( T*		);
										operator T*		(			);
		};


		union
		{
			tpointer<void*>				ptr;
			tpointer<byte>				by;
			tpointer<uint8>				u8;
			tpointer<int8>				s8;
			tpointer<uint16>			u16;
			tpointer<int16>				s16;
			tpointer<uint32>			u32;
			tpointer<int32>				s32;
			tpointer<float>				f32;
			tpointer<uint64>			u64;
			tpointer<uint128>			u128;
		};


										MCursor				(												);
										MCursor				(	void*, uint,	uint64 inbitpos=0			);
										MCursor				(	const MCursor&, uint64 inbitpos=0			);

		void							setup				(	void*, uint,    uint64 inbitpos=0			);
		void							setup				(	const MCursor&, uint64 inbitpos=0			);

		void							clear				(												);


		// bit ops

		uint64							size				(												) const;

		uint64							pos					(												) const;
		uint64							seek				(	uint64 inbits								);

		uint64							backward			(												) const;
		bool							hasbackward			(	uint64 inbits								) const;
		void							gobackward			(	uint64 inbits								);

		uint64							forward				(												) const;
		bool							hasforward			(	uint64 inbits								) const;
		void							goforward			(	uint64 inbits								);

		void							align				(	uint inbits									);
		template <typename T> void		align				(												);

		void							fill				(	uint inbits									);

		uint							uread				(	uint inbits									);
		int								sread				(	uint inbits									);

		void							write				(	byte* insrc, uint inbits					);
		void							write				(	uint invalue, uint inbits					);


		// byte ops

		uint							sizeB				(												) const;

		byte*							addrB				(												) const;
		byte*							addrB				(	uint64 inbitpos								);

		uint64							posB				(	byte*										) const;
		uint64							seekB				(	byte*										);

		uint							usedB				(												) const;
		bool							usedB				(	uint inbytecount							) const;
		
		uint							leftB				(												) const;
		bool							leftB				(	uint inbytecount							) const;

		bool							findB				(	uint64& outpos, byte inValue				);
		byte*							findB				(	byte inValue								);

		void							readB				(	byte* outdst, uint inbytecount				);
		template <typename T> T			readB				(												);

		void							writeB				(	byte* insrc, uint inbytecount				);
		template <typename T> void		writeB				(	const T&									);


		// misc ops

		template <typename T> T			readmsb				(												);
		template <typename T> T			readlsb				(												);

		template <typename T> void		writemsb			(	const T&									);
		template <typename T> void		writelsb			(	const T&									);

		template <typename T> void		readstring			(	T& outString								);
		template <typename T> void		readstring			(	T& outString, uint incapacity				);


		bool							checkintegrity		(												) const;

	protected:
		byte*							bstart;
		byte*							bend;
	};

}




#include "su_mcursori.h"
#endif // _su_mcursor_h_


