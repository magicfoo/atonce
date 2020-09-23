

import os, sys, re
from atmake import *
import atmake.utilities
from . import token
from . import spawn
import tempfile




def GetRevision( inpath ) :

	app = atmake.utilities.FindApp('svn/svnversion')
	cl  = r'%s {%s}' % (app,inpath)

	res = spawn.Spawn( cl )

	# ( exec_success, exit_code, stdout lines, stderr lines )
	if (not res) or (not res[0]) or (res[1]!=0) or (res[3]) :
		return None
	
	revs = res[2][0].strip()
	revi = re.match('\d+',revs)
	if not revi :
		return None

	return int( revi.group(0) )



