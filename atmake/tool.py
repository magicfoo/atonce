

import os, sys, getpass
from atmake import *
from atmake.tools import bin2h
from atmake.tools import fs
from atmake.tools import genuid as mod_genuid



def donothing ( ) :
	""" Simply returns.
	"""
	pass



def copyifnotexists ( ifile, ofile ) :
	""" Copy file if not already exist.
		<ifile>		The input file path
		<ofile>		The output file path
	"""
	if not os.path.isfile(ofile) :
		atmake.CpFile( ifile, ofile )


def uname ( lower=None ) :
	""" Outputs the python username.
	"""
	u = getpass.getuser()
	if lower:	print(u.lower())
	else:		print(u)


def bin2h ( ifile, ofile=None, arrayname=None ) :
	""" Convert file into C includable header data file.
		<ifile>		The input file path
		<ofile>		The output file path
	"""
	if not ofile :
		ofile = ifile + '.bin2h'
	tools.bin2h.UpdateConvert( ofile, ifile, arrayname=arrayname )



def obj2h ( ifile, ofile=None ) :
	""" Convert the obj file into a C header file.
		<ifile>		The input file path
		<ofile>		The optional output file path
	"""
	if not ofile :
		ofile = ifile + '.bin2h'

	ilines = file(ifile).readlines()
	if not ilines :
		print("File not found or empty !")
		return

	name = 'undefined'
	vl = []
	tl = []
	nl = []
	fl = []

	for l in ilines :
		if l.startswith('vt') :
			tl.append( l.split()[1:] )
		elif l.startswith('vn') :
			nl.append( l.split()[1:] )
		elif l.startswith('v') :
			vl.append( l.split()[1:] )
		elif l.startswith('f') :
			f = []
			for ll in l.split()[1:] :
				v = ll.split('/')
				li,ti,ni = None,None,None
				if   len(v)==1 :	li = int(v[0])
				elif len(v)==2 :	li,ti = int(v[0]),int(v[1])
				elif len(v)==3 :
					if len(v[1]) :	li,ti,ni = int(v[0]),int(v[1]),int(v[2])
					else :			li,ni = int(v[0]),int(v[2])
				f.append( (li,ti,ni) )
			fl.append( f )
		elif l.startswith('g') :
			name = l[2:].strip()

	olines = []
	olines.append( '\n'*2 )
	if 1 :
		olines.append( 'char* name = "%s";\n' % name )
		olines.append( '\n' )
	if 1 :
		olines.append( 'float locations[] = {\n' )
		olines.append( '\t// x, y, z\n' )
		for v in vl :
			olines.append( '\t%sf, %sf, %sf,\n' % (v[0],v[1],v[2]) )
		olines.append( '};\n' )
		olines.append( '\n' )
	if 1 :
		olines.append( 'float uvs[] = {\n' )
		olines.append( '\t// x, y\n' )
		for t in tl :
			olines.append( '\t%sf, %sf,\n' % (t[0],t[1]) )
		olines.append( '};\n' )
		olines.append( '\n' )
	if 1 :
		olines.append( 'float normals[] = {\n' )
		olines.append( '\t// x, y, z\n' )
		if nl :
			for n in nl :
				olines.append( '\t%sf, %sf, %sf,\n' % (n[0],n[1],n[2]) )
		else :
			olines.append( '\t0.0f\n' )
		olines.append( '};\n' )
		olines.append( '\n' )
	if 1 :
		olines.append( 'int faces[] = {\n' )
		olines.append( '\t// v0-li,v0-ti,v0-ni, v1-li,v1-ti,v1-ni, v2-li,v2-ti,v2-ni\n' )
		for f in fl :
			l = '\t'
			for v in f :
				li,ti,ni = v
				if not li:		li = -1
				elif li<0:		li += len(vl)
				else:			li -= 1
				if not ti:		ti = -1
				elif ti<0:		ti += len(tl)
				else:			ti -= 1
				if not ni:		ni = -1
				elif ni<0:		ni += len(nl)
				else:			ni -= 1
				l += '%d,%d,%d, ' % (li,ti,ni)
			olines.append( l+'\n' )
		olines.append( '};\n' )
	olines.append( '\n'*2 )

	file(ofile,'wb').writelines(olines)


	

def genuid ( classname ) :

	"""	Generates UID for sugar classes.
		<classname>  String class name
	"""

	cuid = mod_genuid.genuid(classname)
	print('0x%s' % str('%x'%cuid).upper())


