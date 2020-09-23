

import os, sys, os.path
import fnmatch
import atmake


# t is the target string
# infilter is formed as 'pattern1 -pattern2 +pattern3 pattern4 ...'
# filtering pattern separators could be any of ' ;:,'
# pattern could be any fnmatch expressions like '*', '*flash*', '*test??.graphics*'
# including pattern starts with '+' or nothing (i.e. doesn't start with '-')
# excluding pattern starts with '-'
#
# return true if t passes the pattern pipeline.

def mfilters_list ( infilter ) :
	fl = []
	for f1 in atmake.ToList(infilter) :
		for f2 in f1.split() :
			for f3 in f2.split(';') :
				for f4 in f3.split(':') :
					for f5 in f4.split(',') :
						fl.append( f5 )
	return fl


def mfilters ( t, infilter, defkeep=None ) :

	keep = defkeep
	fl = mfilters_list( infilter )

	for f in fl :
		if f.startswith('-') :
			if fnmatch.fnmatch(t,f[1:]):	keep = False
		elif f.startswith('+') :
			if fnmatch.fnmatch(t,f[1:]):	keep = True
		else :
			if fnmatch.fnmatch(t,f):		keep = True
	return (keep is True)


