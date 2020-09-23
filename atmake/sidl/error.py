
import os, sys



def SIDL_Error ( reason, loc=None, donotexit=None ) :
	if loc:		print("sidl-error: %s: %s" % (loc.Here(), reason))
	else  :		print("sidl-error: ?: %s" % (reason))

	if donotexit :
		#raise yacc.SyntaxError
		pass
	else :
		sys.exit(-1)





