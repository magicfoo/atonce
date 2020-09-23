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


#ifndef _su_iinterfacei_h_
#define _su_iinterfacei_h_




namespace su
{


	template < typename T > inline
	T* IInterface::GetInterface ( )
	{
		return (T*)this->GetInterface(T::ClassId);
	}


	inline bool IInterface::HasInterface ( classid inCid )
	{
		return this->GetInterface(inCid) != NULL;
	}


	template <typename I> inline
	bool IInterface::HasInterface ( )
	{
		return this->GetInterface(I::ClassId) != NULL;
	}


	template <typename T> inline
	T* SafeInterface ( IInterface* inITF )
	{
		return (inITF) ? (T*)inITF->GetInterface(T::ClassId) : NULL;
	}


	template < typename T > inline
	void SafeRelease ( T*& ioITF )
	{
		if( ioITF )
		{
			ioITF->Release();
			ioITF = NULL;
		}
	}


	template <typename T> inline
	void SafeDelete ( T*& ioObject )
	{
		if( ioObject )
		{
			delete ioObject;
			ioObject = NULL;
		}
	}


	template < typename T > inline
	bool SafeRetain ( T*& ioITF, T* inRefITF )
	{
		if( ioITF == inRefITF )
			return FALSE;
		if( inRefITF )
			inRefITF->Retain();
		if( ioITF )
			ioITF->Release();
		ioITF = inRefITF;
		return TRUE;
	}


	template < typename T > inline
	bool SafeNotNullRetain ( T*& ioITF,	T* inRefITF )
	{
		if( !inRefITF || ioITF==inRefITF )
			return FALSE;
		inRefITF->AddRef();
		if( ioITF )
			ioITF->Release();
		ioITF = inRefITF;
		return TRUE;
	}


	template <typename T> inline
	IInterface* BaseInterface ( T* inI, classid inCid )
	{
		if( T::ClassId == inCid )
			return inI;
		else
			return BaseInterface( (typename T::ClassBase*)inI, inCid );
	}


	inline IInterface* BaseInterface ( void*, classid )
	{
		return NULL;
	}

}




#endif // _su_iinterfacei_h_


