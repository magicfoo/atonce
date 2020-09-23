

import os, sys, os.path, string, re
import atmake
from .components import *
from .error import *



_re_cash      = re.compile( '\$\$' )										# $$
_re_name_val1 = re.compile( '\$([_a-zA-Z][_a-zA-Z0-9]*)' )					# $NAME
_re_name_val2 = re.compile( '\${([_a-zA-Z][_a-zA-Z0-9]*)}' )				# ${NAME}
_re_name_func = re.compile( '\${([_a-zA-Z][_a-zA-Z0-9]*)\(([^$]*)\)}' )		# ${NAME(...)}
_cash_placeholder = '\xf4'



def Subst( env, pattern, loc=None, warn=None ) :

	def _get ( sym, arg=None ) :
		# get variable
		try :
			s = env[ sym ]
		except KeyError :
			if warn :
				SIDL_Error( "Undefined symbol <%s> !" % sym, loc, donotexit=True )
			return ''
		# get result
		if atmake.IsCallable(s) :		r = s( env=env, loc=loc, arg=arg )
		else :					r = s
		# check result
		if r is None :			return ''
		elif atmake.IsCallable(r) :	return r
		elif atmake.IsList(r) :		return string.join( r, ';' )
		else :					return str( r )

	def _fmt ( so, su ) :
		pre = so.string[:so.start()]
		suf = so.string[so.end():]
		if pre.isspace() :		pre = ''
		if suf.isspace() :		suf = ''
		if pre or suf :			return pre + su + suf
		else :					return su

	def _subst1 ( pattern ) :
		if atmake.IsString( pattern ) :
			# $$ (substituted by _cash_placeholder)
			so = _re_cash.search( pattern )
			if so :
				return ( _fmt(so,_cash_placeholder), True )

			# ${NAME(...)}
			so = _re_name_func.search(pattern)
			if so :
				name = so.group(1)
				arg = _substrec( so.group(2) )
				su = _substrec( _get(name,arg) )
				return ( _fmt(so,su), True )

			# $NAME or ${NAME}
			so = _re_name_val1.search(pattern) or _re_name_val2.search(pattern)
			if so :
				name = so.group(1)
				su = _substrec( _get(name) )
				return ( _fmt(so,su), True )

		# no more subst to process ...
		return ( pattern, False )

	def _substrec ( pattern ) :
		cnt = True
		while cnt :
			pattern, cnt = _subst1( pattern )
		return pattern

	s = _substrec( pattern )
	if atmake.IsString(s) :
		return s.replace(_cash_placeholder,'$')
	else :
		return s


