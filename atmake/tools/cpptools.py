"""
	
	C++ tools

"""


import os, sys, re, glob
import atmake
import atmake.utilities
from . import fs
from . import token
from . import spawn
from . import sign



#
# Build PreCompiled Header File

def BuildPch( infile, incdirs=None, incfiles=None, defines=None, options=None ) :

	# call cpp0.exe
	cpp0 = atmake.utilities.FindApp('cpp0.exe')
	cl = r'%s %s ' % (cpp0, options if options else '')

	# -I path
	if incdirs :
		for incdir in atmake.ToList(incdirs) :
			cl += ' -I {%s}' % incdir

	# -include file
	if incfiles :
		for incfile in atmake.ToList(incfiles) :
			cl += ' -include {%s}' % incfile

	# -Dsym
	if defines :
		for sym in atmake.ToList(defines) :
			cl += ' {-D%s}' % sym

	# input cpp source file
	cl += ' {%s}' % infile

	# print cl
	res = spawn.Spawn( cl )

	# ( exec_success, exit_code, stdout lines, stderr lines )
	return res



def GeneratePch( ofile, infile, incdirs=None, incfiles=None, defines=None, options=None, readonly=True ) :
	if not os.path.isfile(infile) :
		print("Can't generate pch file for input '%s': doesn't exist !" % infile)
		exit(1)

	# build pch lines
	res = BuildPch( infile=infile, incdirs=incdirs, incfiles=incfiles, defines=defines, options=options )

	if not res or not res[1] or not res[2] :
		print("Can't generate pch file for input '%s': fatal error !" % infile)
		exit(1)

	if res[3] and 0 :
		print("CPP returns parsing errors for input '%s':" % infile)
		for l in ''.join(res[3]).splitlines() :
			print('\t%s' % l)
		exit(1)
	
	# write lines
	if res[2] :
		writeit = True
		if os.path.isfile(ofile) :
			file_md5  = sign.GetFileContentSign( ofile )
			lines_md5 = sign.GetLinesSign( res[2] )
			if file_md5==lines_md5 :
				writeit = False
		if writeit :
			try:
				fs.CreateFile( ofile, rec=True )
				open(ofile,'w').writelines(res[2])
				if readonly :
					fs.SetFileReadOnly( ofile )
			except:
				pass




#
# Build a header file rack

_re_cfile_inc1 = re.compile( '#include[^"]*"([^"]*)"' )		#   #inlude "..."
_re_cfile_inc2 = re.compile( '#include[^<]*<([^>]*)>' )		#   #inlude <...>

def RackHeaderFiles ( outputdir, inputs=None, readonly=True, prefix=None, extlist=None ) :

	inputs_dirs  = [x for x in atmake.ToList(inputs) if fs.IsDirPath(x)]
	inputs_files = [x for x in atmake.ToList(inputs) if fs.IsFilePath(x)]
	extlist = extlist or ['.h']
	fmap = {}

	def _map_name ( infile ) :
		i = 0
		d = os.path.dirname(infile)
		b,e = os.path.splitext( os.path.basename(infile) )
		f = '%s/%s%s%s' % (outputdir,prefix or '',b,e)
		while 1 :
			if not f in fmap :
				fmap[f] = infile
				return f
			f = '%s/%s%s_%d%s' % (d,prefix or '',b,i,e)
			i += 1

	def _map_file( infile ) :
		if not os.path.isfile(infile):
			return

		df = os.path.dirname(infile)
		nf = os.path.normpath(infile)
		if nf in fmap:
			return fmap[nf]

		fext = os.path.splitext(infile)[1]
		if not fext in extlist:
			return

		try:		lines = open(nf,'r').readlines()
		except:		return

		fmap[nf] = _map_name(infile)

		olines = []
		for l in lines :
			of = None

			# "..." ?
			so = _re_cfile_inc1.search(l)
			if so :
				f = '%s/%s' % (df,so.group(1))
				if os.path.isfile(f) :
					of = _map_file(f)

			# <...> ?
			so = _re_cfile_inc2.search(l)
			if so :
				for d in inputs_dirs:
					f = '%s/%s' % (d,so.group(1))
					if os.path.isfile(f) :
						of = _map_file(f)

			if of:	olines.append( '#include "%s"\n'%fs.FileToFile(of,fmap[nf]) )
			else:	olines.append( l )

		open(fmap[nf],'w').writelines(olines)
		return fmap[nf]



	fs.RmTree( outputdir )
	fs.CreateDir( outputdir, rec=True )
	if not fs.IsDirPath(outputdir) :
		print("Can't create output directory '%s' !" % outputdir)
		exit(1)

	for f in inputs_files:
		_map_file( f )
	for d in inputs_dirs:
		[ _map_file(f) for f in glob.glob('%s/*'%d) ]

		

