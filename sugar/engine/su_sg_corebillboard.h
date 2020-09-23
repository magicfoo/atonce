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


#ifndef _su_sg_corebillboard_h_
#define _su_sg_corebillboard_h_



namespace su { namespace sg
{

	struct Billboard: public Core
	{
		static const uint Type = 0xa1233c34;		// "su::sg::Billboard"

		static Billboard*	Create		(	Handle*				inHandle,
											IDirectDraw*		inDD		);


		virtual uint		GetType		(									)	{	return Type;	}
		virtual bool		IsType		(	uint				inType		)	{	return Type == inType;	}



		void				SetPbi					(	uint				inPbi		);

		void				SetSize					(	const Vec2&			inSize		);
		void				SetUV					(	const Vec4&			inU0V0U1V1	);
		void				SetParam				(	const Vec2&			inSize,
														const Vec4&			inU0V0U1V1	);
		
		void				EnableAlphaThreshold	(	float				inTh,
														const Color32&		inColor		);

		void				SetToScreenSpace		(									);		// [-1, 1]
		void				SetToWorldSpace			(									);

		bool				IsInWorldspace			(									);
		
		uint				GetPbi					(									);
		uint				GetVbo					(									);
		bool				GetAlphaThresholdParam	(	float*				ioTH,
														Color32*			ioColor		);

	protected:
		virtual void		Shut					(									);

		IDirectDraw* dd;
		Vec2	size;
		Vec4	uv;
		bool	isInWorld;


		bool	alphaTH;
		float	alphaTHV;
		Color32	colorTH;

		uint	vbo;
		uint	pbi;
	};

} }



#endif // _su_sg_corecamera_h_
