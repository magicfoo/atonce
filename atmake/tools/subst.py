""" ATMake_

Subst base class

"""


import os, sys, os.path, imp, string, tempfile, re, itertools
from atmake import *
from atmake.tools import types




_re_cash  = re.compile( '\$\$' )										# $$
_re_name1 = re.compile( '\$([_a-zA-Z][_a-zA-Z0-9]*)' )					# $NAME
_re_name2 = re.compile( '\${([_a-zA-Z][_a-zA-Z0-9]*)}' )				# ${NAME}
_re_name3 = re.compile( '\${([_a-zA-Z][_a-zA-Z0-9]*)\(([^$]*)\)}' )		# ${NAME(...)}



def Subst( env, pattern, bf=None, fw=None, warn=False ) :

	def _get ( sym, arg=None ) :
		# get variable
		try :
			s = env[ sym ]
		except KeyError :
			if warn :
				atmake.Warning( "Undefined symbol <%s> !" % sym )
			return ''
		# get result
		if IsCallable(s) :		r = s( fw, bf, arg )
		else :					r = s
		# check result
		if r is None :			return ''
		elif IsCallable(r) :	return r
		elif IsList(r) :		return ';'.join( types.FlattenList(r) )
		else :					return str( r )

	def _fmt ( so, su ) :
		pre = so.string[:so.start()]
		suf = so.string[so.end():]
		if pre.isspace() :		pre = ''
		if suf.isspace() :		suf = ''
		if pre or suf :			return pre + su + suf
		else :					return su

	def _subst ( pattern ) :
		if IsString( pattern ) :
			# ${NAME(...)}
			so = _re_name3.search(pattern)
			if so :
				name = so.group( 1 )
				arg = Subst( env, so.group(2), bf=bf, fw=fw )
				su = Subst( env, _get(name,arg), bf=bf, fw=fw )
				return ( _fmt(so,su), True )

			# $NAME or ${NAME}
			so = _re_name1.search(pattern) or _re_name2.search(pattern)
			if so :
				name = so.group( 1 )
				su = Subst( env, _get(name), bf=bf, fw=fw )
				return ( _fmt(so,su), True )

			# $$
			so = _re_cash.search( pattern )
			if so :
				return ( _fmt(so,'$'), True )

		# no more subst to process ...
		return ( pattern, False )

	cnt = True
	while cnt :
		pattern, cnt = _subst( pattern )
	return pattern



