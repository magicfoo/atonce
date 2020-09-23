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



#ifndef _su_utils_c_h_
#define _su_utils_c_h_



#ifdef __cplusplus
extern "C" 
{
#endif


int		su_langc_isspace	( int c );
int		su_langc_isdigit	( int c );
float	su_langc_strtofloat	( const char* string, char** endp	);
void	su_langc_strcopy	( char*, char* );
char*	su_langc_strdup		( char* );
void*	su_langc_malloc		( int );
void*	su_langc_realloc	( void*, int );
void	su_langc_free		( void* );
void	su_langc_memset		( void*, int, int );
void	su_langc_memcopy	( void*, void*, int );


#ifdef __cplusplus
}
#endif




#endif




