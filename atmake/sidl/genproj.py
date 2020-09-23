
import os, sys, glob, imp
import atmake
from .components import *


def SIDL_GenProj ( sidl=None, solutionname=None, verbose=None, precompiled=None ) :

	filedir  = os.path.dirname( __file__ )
	genfiles = glob.glob( '%s/genproj_*.py' % filedir )

	if precompiled is None :
		precompiled = True

	for f in genfiles :
		modname = os.path.splitext( os.path.basename(f) )[0]
		modpath = 'atmake.sidl.%s' % modname
		fp, pathname, description = imp.find_module( modname, [filedir] )
		mod = imp.load_module( modpath, fp, f, description )
		fp.close()
		mod.Generate( sidl=sidl, solutionname=solutionname, verbose=verbose, precompiled=precompiled )

	sidl.ShelfSync()


