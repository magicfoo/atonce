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



using namespace su;




namespace
{

	struct SIDL_STUB_SUPERNAME
	{

	private:

		SU_DD_CMD_DESC*		_desca;
		uint				_descbs;


	public:

		template < typename T >
		bool Init ( T*& outHAL, Pool* p, AnyList* o, SU_DD_CMD_DESC* desca=NULL, uint descbs=0 )
		{
			SU_REQUIRE_OR_NULL( o );

			_desca  = desca;
			_descbs = descbs;

			outHAL = o->asObject<T>( "ddraw" );
			if( !outHAL )
				return false;

			return true;
		}


		void Shut ( )
		{
			//
		}


		uint CountAction (  )
		{
			return _descbs / sizeof(SU_DD_CMD_DESC);
		}


		bool EnumAction		(	uint					inIndex,
								byte&					outCode,
								IDirectShader::Action&	outAction,
								void*&					outUData		)
		{
			if( inIndex >= CountAction() )
				return false;
			outCode   = _desca[inIndex].code;
			outAction = _desca[inIndex].act;
			outUData  = this;
			return true;
		}


		void OnStart (  )
		{
			//
		}

		void OnBeginFrame (  )
		{
			//
		}


		void OnEndFrame (  )
		{
			//
		}


		void OnFlush (  )
		{
			//
		}

	};


}


