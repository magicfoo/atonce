""" ATMake_

Utils base class

"""


import os, sys, os.path
import atmake


def get_name8_3 ( path ) :
	dir    = os.path.dirname( path )
	base = os.path.basename( path )
	file, ext = os.path.splitext( base )
	if len(ext) > 3 :
		ext = ext[-3:]
	if len(file) > 8 :
		for i in range(1,99) :
			base83 = r'%s~%s.%s' % (file[:5],str(i),ext)
			path83 = os.path.join( dir, base83 )
			if not os.path.isfile(path83) :
				return path83
	return None


def append1 ( outlist, initem ) :
	if not initem in outlist :
		outlist.append( initem )
	return outlist


def extend1 ( outlist, inlist ) :
	for i in inlist :
		append1( outlist, i )
	return outlist


	
	