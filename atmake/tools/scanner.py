""" ATMake

Builder base class

"""



import os, sys, os.path, re
import atmake
from . import *




class ScanFiles :

	fname	= None
	sources	= None		# source files to append to the global build process
	deps	= None		# dependency files
	builds 	= None		# BuildFiles [] of auxiliaries targets to build before this source


	def __init__ ( self, fname=None ) :
		self.fname = fname
		self.sources = []
		self.deps = []
		self.builds = []


	def AddSource ( self, f ) :
		if f and not f in self.sources :
			self.sources.append( f )


	def AddDepend ( self, f ) :
		if f and not f in self.deps :
			self.deps.append( f )


	def AddBuild ( self, target, sources ) :
		if target and sources :
			bf = builder.BuildFiles( target, atmake.ToList(sources) )
			self.builds.append( bf )


	def Count ( self ) :
		return len(self.sources) + len(self.deps) + len(self.builds)


	def GetAllFiles ( self ) :
		s  = []
		s.extend( self.sources )
		s.extend( self.deps )
		for bf in self.builds :
			s.append( bf.target )
		return s


	def Append( self, ref ) :
		for s in ref.sources :
			self.AddSource( s )
		for d in ref.deps :
			self.AddDepend( d )
		for bf in ref.builds :
			self.AddBuild( bf.target, bf.sources )






_re_cfile_inc1 = re.compile( '#include[^"]*"([^"]*)"' )		#   #inlude "..."
_re_cfile_inc2 = re.compile( '#include[^<]*<([^>]*)>' )		#   #inlude <...>



def CFileDepends ( fnames, incdirs=None, recurse=True ) :

	def __find_usr_inc( srcfile, inc ) :
		# include "..."
		# The include file could not exist, for example toto.bin2h is generated and doesn't exist before ...
		if not inc or not srcfile :
			return None

		if fs.IsFilePath(inc) :
			# "c:\\..."
			afile = os.path.normpath( inc )
			# print '[%s] #include "%s" -> [%s]' % (srcfile,inc,str(afile))
			return afile
		else :
			adir = os.path.dirname( srcfile )
			afile = os.path.normpath( os.path.join( adir, inc ) )
			# print '[%s] #include "%s" -> [%s]' % (fname,inc,str(afile))
			return afile

	def __find_sys_inc( srcfile, inc, incdirs ) :
		# include <...>
		# The include file must exist !
		if not inc or not srcfile :
			return None

		if fs.IsFilePath(inc) :
			# <c:\\...>
			afile = os.path.normpath( inc )
			#print '[%s] #include <%s> -> [%s]' % (fname,inc,str(afile))
			return afile

		afile = fs.FindFileInDirs( inc, incdirs )
		if fs.IsFilePath(afile) :
			#print '[%s] #include <%s> -> [%s]' % (fname,inc,str(afile))
			return os.path.normpath( afile )

		# not found !
		#print '[%s] #include <%s> -> not found !' % (fname,inc)
		return os.path.normpath( inc )

	def __recurse_inc ( fname, incdirs, iodeps ) :
		srcfile = fs.FindFileInDirs( fname, incdirs )
		try :		srclines = open(srcfile,'r').readlines()
		except :	return None
		for l in srclines :
			dep = None
			so = _re_cfile_inc1.search(l)
			if so :
				dep = __find_usr_inc( srcfile, so.group(1) )
			else :
				so = _re_cfile_inc2.search(l)
				if so :
					dep = __find_sys_inc( srcfile, so.group(1), incdirs )
			if dep and dep not in iodeps :
				iodeps[ dep ] = 1
				__recurse_inc( dep, incdirs, iodeps )

	deps = {}
	for fname in atmake.ToList(fnames) :
		__recurse_inc( fname, incdirs, deps )
	return list(deps.keys())




def CFileScanner ( outScan, fw, fname ) :

	srcfile = fw.ToAbsFile( fname )

	incdirs = []
	includes = fw.env['INCLUDE']
	atmake.Assert( atmake.IsString(includes) )
	for d in includes.split( os.pathsep ) :
		if atmake.IsString(d) :
			incdirs.append( fw.ToAbsFile(d) )

	deps = CFileDepends ( srcfile, incdirs=incdirs, recurse=False )
	for dep in atmake.ToList(deps) :
		outScan.AddDepend( dep )
	




