/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2009 AtOnce Technologies
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


#include "../su_base.h"
using namespace su;



namespace
{
	#define	NOMATCH			1
	#define MAXNEST    		20
	#define EOS    			'\0'
	#define RANGE_MATCH     1
	#define RANGE_NOMATCH   0
	#define RANGE_ERROR     (-1)

	char * index(const char *p, int ch)
	{
         union {
                 const char *cp;
                 char *p;
         } u;
 
         u.cp = p;
         for (;; ++u.p) {
                 if (*u.p == ch)
                         return(u.p);
                 if (!*u.p)
                         return(NULL);
         }
         /* NOTREACHED */
	}

	int rangematch( const char * pattern, char test, int flags, char ** newp)
	{
         int negate, ok;
         char c, c2;
 
         /*
          * A bracket expression starting with an unquoted circumflex
          * character produces unspecified results (IEEE 1003.2-1992,
          * 3.13.2).  This implementation treats it like '!', for
          * consistency with the regular expression syntax.
          * J.T. Conklin (conklin@ngai.kaleida.com)
          */
         if ( (negate = (*pattern == '!' || *pattern == '^')) )
                 ++pattern;
 
		 if (flags & su::util::FNM_CASEFOLD)
			 test = util::Tolower((unsigned char)test);
 
         /*
          * A right bracket shall lose its special meaning and represent
          * itself in a bracket expression if it occurs first in the list.
          * -- POSIX.2 2.8.3.2
          */
         ok = 0;
         c = *pattern++;
         do {
				if (c == '\\' && !(flags & util::FNM_NOESCAPE))
                         c = *pattern++;
                 if (c == EOS)
                         return (RANGE_ERROR);
 
                 if (c == '/' && (flags & util::FNM_PATHNAME))
                         return (RANGE_NOMATCH);
 
				 if (flags & util::FNM_CASEFOLD)
                         c = util::Tolower((unsigned char)c);
 
                 if (*pattern == '-'
                     && (c2 = *(pattern+1)) != EOS && c2 != ']') {
                         pattern += 2;
                         if (c2 == '\\' && !(flags & util::FNM_NOESCAPE))
                                 c2 = *pattern++;
                         if (c2 == EOS)
                                 return (RANGE_ERROR);
 
                         if (flags & util::FNM_CASEFOLD)
							 c2 = util::Tolower((unsigned char)c2);
 
                         if (c <= test && test <= c2)
                                 ok = 1;
                 } else if (c == test)
                         ok = 1;
         } while ((c = *pattern++) != ']');
 
         *newp = (char*)pattern;
         return (ok == negate ? RANGE_NOMATCH : RANGE_MATCH);
	}

	int	_fnmatch(const char *pattern, const char *string, int flags, int nesting)
	{
         const char *stringstart;
         char *newp;
         char c, test;
 
         if (nesting == MAXNEST)
                 return (NOMATCH);
 
         for (stringstart = string;;) {
                 switch (c = *pattern++) {
                 case EOS:
                         if ((flags & util::FNM_LEADING_DIR) && *string == '/')
                                 return (0);
                         return (*string == EOS ? 0 : NOMATCH);
                 case '?':
                         if (*string == EOS)
                                 return (NOMATCH);
                         if (*string == '/' && (flags & util::FNM_PATHNAME))
                                 return (NOMATCH);
                         if (*string == '.' && (flags & util::FNM_PERIOD) &&
                             (string == stringstart ||
                             ((flags & util::FNM_PATHNAME) && *(string - 1) == '/'))) {
                                 return (NOMATCH);
                         }
                         ++string;
                         break;
                 case '*':
                         c = *pattern;
                         /* Collapse multiple stars. */
                         while (c == '*')
                                 c = *++pattern;
 
						 if (*string == '.' && (flags & util::FNM_PERIOD) &&
                             (string == stringstart ||
                             ((flags & util::FNM_PATHNAME) && *(string - 1) == '/'))) {
                                 return (NOMATCH);
                         }
 
                         /* Optimize for pattern with * at end or before /. */
                         if (c == EOS) {
                                 if (flags & util::FNM_PATHNAME) {
                                         return ((flags & util::FNM_LEADING_DIR) ||
                                             index(string, '/') == NULL ?
                                             0 : NOMATCH);
                                 } else {
                                         return (0);
                                 }
                         } else if (c == '/' && flags & util::FNM_PATHNAME) {
                                 if ((string = index(string, '/')) == NULL)
                                         return (NOMATCH);
                                 break;
                         }
 
                         /* General case, use recursion. */
                         while ((test = *string) != EOS) {
                                 if (!_fnmatch(pattern, string, flags & ~util::FNM_PERIOD, nesting + 1))
                                         return (0);
                                 if (test == '/' && flags & util::FNM_PATHNAME)
                                         break;
                                 ++string;
                         }
                         return (NOMATCH);
                 case '[':
                         if (*string == EOS)
                                 return (NOMATCH);
                         if (*string == '/' && (flags & util::FNM_PATHNAME))
                                 return (NOMATCH);
                         if (*string == '.' && (flags & util::FNM_PERIOD) &&
                             (string == stringstart ||
                             ((flags & util::FNM_PATHNAME) && *(string - 1) == '/')))
                                 return (NOMATCH);
 
                         switch (rangematch(pattern, *string, flags, &newp)) {
                         case RANGE_ERROR:
                                 goto norm;
                         case RANGE_MATCH:
                                 pattern = newp;
                                 break;
                         case RANGE_NOMATCH:
                                 return (NOMATCH);
                         }
                         ++string;
                         break;
                 case '\\':
                         if (!(flags & util::FNM_NOESCAPE)) {
                                 if ((c = *pattern++) == EOS) {
                                         c = '\\';
                                         --pattern;
                                 }
                         }
                         /* FALLTHROUGH */
                 default:
                 norm:
                         if (c == *string) {
                                 ;
                         } else if ((flags & util::FNM_CASEFOLD) &&
                                  (util::Tolower((unsigned char)c) ==
                                   util::Tolower((unsigned char)*string))) {
                                 ;
                         } else {
                                 return (NOMATCH);
                         }
                         string++;
                         break;
                 }
         }
         /* NOTREACHED */
	}
}


bool
su::util::Fnmatch	(	pcstr pattern, pcstr string, uint flags	)
{
	return _fnmatch( pattern, string, flags, 0 ) != NOMATCH;
}




