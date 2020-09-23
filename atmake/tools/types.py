""" ATMake_

Types tools

"""


import os, sys, os.path, glob, string
import fnmatch
import atmake


def AppendList ( li, v ) :
	if atmake.IsEmpty(li) :		li = []
	elif atmake.IsString(li) :		li = li.split()
	for i in atmake.ToList(v) :
		if not i in li :
			li.append( i )
	return li


def PrependList ( li, v ) :
	if atmake.IsEmpty(li) :		li = []
	elif atmake.IsString(li) :		li = li.split()
	for i in atmake.ToList(v) :
		if not i in li :
			li.insert( 0, i )
	return li



def AppendString ( li, v ) :
	return string.join( AppendList(li,v) )



def PrependString ( cur, v ) :
	return string.join( PrependList(li,v) )

def FlattenList( x ) :
	result = []
	for el in x:
		if el :
			#if isinstance(el, (list, tuple)):
			if hasattr(el, "__iter__") and not isinstance(el, str):
				result.extend(FlattenList(el))
			else:
				result.append(el)
	return result

def sort_by_rules ( in_list, in_rules ) :
	# sort strings according to a list of rules
	# rule is a string 'pattern0 < pattern1' such as strings matching pattern0
	# always occur in list before strings matching pattern1

	def __id ( li, it ) :
		if it in li :
			return li.index( it )
		else :
			li.append( it )
			return len(li)-1

	def __add ( d, lid, rid ) :
		if lid in d :	d[lid].append( rid )
		else :			d[lid] = [ rid ]
		if rid not in d :	d[rid] = []

	def __extdict ( d, r, k ) :
		for i in d[k] :
			r[i] = True
			__extdict( d, r, i )

	def __extlist ( d, k ) :
		r = {}
		__extdict( d, r, k )
		return r.keys()

	def __fnmatch ( li, path ) :
		d = {}
		for i in range(len(li)) :
			pi = li[i]
			if fnmatch.fnmatch(path,pi) :
				d[ pi ] = i
		return [ d[k] for k in sorted(d.keys()) ]

	def __rcmp ( r, r0, r1 ) :
		if len(r0)==0 and len(r1)==0 :
			return 0
		if r0[0] == r1[0] :
			return __rcmp( r, r0[1:], r1[1:] )
		c = 0
		for i0 in r0 :
			for i1 in r1 :
				if i1 in r[i0] :	_c = -1	# i0 before i1
				elif i0 in r[i1] :	_c = +1	# i0 after i1
				else :		_c = 0
				if _c!=0 :
					atmake.Assert( c==0 or (_c==c), 'Circular rules found !' )
					c = _c
		return c

	show_dbg = False

	patterns = []
	rules = {}
	for r in in_rules :
		if r.find('<') > 0 :	left, sep, right = r.partition('<')
		elif r.find('>') > 0 :	right, sep, left = r.partition('>')
		else :			atmake.Error( 'Invalid rule "%s" !'%r )
		left, right = left.strip().lower(), right.strip().lower()
		atmake.Assert( left, 'Invalid rule "%s" !'%r )
		atmake.Assert( right, 'Invalid rule "%s" !'%r  )
		lid = __id( patterns, left )
		rid = __id( patterns, right )
		__add( rules, lid, rid )
		if show_dbg :
			print(r, lid, rid, patterns, rules)


	def __cmp ( s0, s1 ) :
		c = __rcmp( rules, s0[1], s1[1] )
		if show_dbg :
			if c < 0 :	print('(%s)   <   (%s)' % (s0[0],s1[0]))
			elif c > 0 :	print('(%s)   >   (%s)' % (s0[0],s1[0]))
			else :	print('(%s)   =   (%s)' % (s0[0],s1[0]))
		return c

	# items is list of preprocessed ( path, path-pattern-ids )
	items = [ (i,__fnmatch(patterns,i.lower())) for i in in_list ]
	sitems = sorted( items, cmp=__cmp )
	out_list = [ i[0] for i in sitems ]

	if show_dbg :
		print("Before ....")
		for s in in_list :
			print(s)
		print("After ...")
		for s in out_list :
			print(s)

	return out_list



def UnindentLines ( lines ) :
	def findcommon(a,b) :
		if a is None:	return b
		if b is None:	return a
		la,lb,s = len(a), len(b), 0
		while s<la and s<lb and a[s]==b[s] :
			s += 1
		return a[:s]
	lines = '\n'.join( atmake.ToList(lines) ).splitlines()
	if len(lines)>=2 :
		if len(lines[0].strip())==0:	lines = lines[1:]
		if len(lines[-1].strip())==0:	lines = lines[:-1]
	d = None
	for l in lines :
		if len(l.strip()) :
			s = l.lstrip()
			t = len(l)-len(s)
			r = l[:t]
			d = findcommon( d, r )
	if d:	return [ l[len(d):] for l in lines ]
	else:	return lines


