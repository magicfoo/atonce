

import os, sys, os.path, glob, string, re, stat, shutil, zipfile, hashlib
import fnmatch
import atmake
from atmake import utilities
from . import spawn



if sys.platform == 'win32':

    def WhereIs(file, path=None, pathext=None):
        if path is None:
            path = os.environ['PATH']
        if atmake.IsString(path):
            path = string.split(path, os.pathsep)
        if pathext is None:
            try:
                pathext = os.environ['PATHEXT']
            except KeyError:
                pathext = '.COM;.EXE;.BAT;.CMD'
        if atmake.IsString(pathext):
            pathext = string.split(pathext, os.pathsep)
        for ext in pathext:
            if string.lower(ext) == string.lower(file[-len(ext):]):
                pathext = ['']
                break
        for dir in path:
            f = os.path.join(dir, file)
            for ext in pathext:
                fext = f + ext
                if os.path.isfile(fext):
                    return os.path.normpath(fext)
        return None

elif os.name == 'os2':

    def WhereIs(file, path=None, pathext=None):
        if path is None:
            path = os.environ['PATH']
        if atmake.IsString(path):
            path = string.split(path, os.pathsep)
        if pathext is None:
            pathext = ['.exe', '.cmd']
        for ext in pathext:
            if string.lower(ext) == string.lower(file[-len(ext):]):
                pathext = ['']
                break
        for dir in path:
            f = os.path.join(dir, file)
            for ext in pathext:
                fext = f + ext
                if os.path.isfile(fext):
                    return os.path.normpath(fext)
        return None

else:

    def WhereIs(file, path=None, pathext=None):
        if path is None:
            path = os.environ['PATH']
        if atmake.IsString(path):
            path = string.split(path, os.pathsep)
        for dir in path:
            f = os.path.join(dir, file)
            if os.path.isfile(f):
                try:
                    st = os.stat(f)
                except OSError:
                    continue
                if stat.S_IMODE(st[stat.ST_MODE]) & 0o111:
                    return os.path.normpath(f)
        return None




def Win32ToPosixPathFilter( cmdline, userhome ) :
	# 'd:\dir0\dir1' -> '~\dir0\dir1'
	for sep in [ None , ';' , '&&' , '||' , '>', '>>', '<', ',', '-', '.' ] :
		cmdlist = cmdline.split(sep)
		cmdline = ''
		if not sep :
			sep = ' '
		for c in cmdlist :
			pos0 = c.find( ':\\' )
			pos1 = c.find( ':/' )
			if pos0 > 0 :	pos = pos0
			else :			pos = pos1
			if pos > 0 :
				path = c[pos-1:]
				drive, path = os.path.splitdrive(path)
				c = c[:pos-1] + userhome + path
			cmdline += sep + c
		cmdline = cmdline[len(sep):]

	# '\' -> '/'
	cmdline = cmdline.replace( '\\', '/' )
	return cmdline


def IsPathExcluded( path, excldirs=[] ) :
	if atmake.IsString(excldirs) :
		excldirs = excldirs.split(';')

	blist = []
	while True :
		path, base = os.path.split( str(path) )
		if not base or len(base)==0 :
			break
		if base :
			blist.append( base )

	for e in excldirs :
		for b in blist :
			if b.startswith(e) or b.endswith(e) :
				return True
	return False


def ToStringPath( path, sep=None ) :
	""" 'A/B/C/D'		  -> 'A/B/C/D'
		'A B C D'         -> 'A/B/C/D'
		['A','B','C','D'] -> 'A/B/C/D'
	"""
	if sep is None :
		sep = os.sep
	if atmake.IsString( path ):
		if path.find( sep ) >= 0 :
			return path
		path = str(path).split()
	if not atmake.IsList( path ) or len(path) == 0 :
		atmake.Error( "Invalid path : %s !" % str(path) )
	j = ''
	for p in path:
		if len(str(p)) :
			j += sep + str(p)
	return j[1:]


def ToListPath( path, sep=None ):
	""" ['A','B','C','D'] -> ['A','B','C','D']
		'A/B/C/D'		  -> ['A','B','C','D']
		'A B C D'         -> ['A','B','C','D']
	"""
	if sep is None :
		sep = os.sep
	if atmake.IsList( path ) :
		return path
	if not atmake.IsString( path ):
		atmake.Error( "Invalid path : %s !" % str(path) )
	if path.find( os.sep ) >= 0 :
		# 'A/B/C/D' -> ['A','B','C','D']
		drive, path = os.path.splitdrive( path )
		p = []
		while len(path) :
			path, base = os.path.split( path )
			if len(base) :
				p = [base] + p
			# prevent os.path.split('\\\') looping
			if len(path) and path[-1] == sep :
				break
		if len(drive) :
			p = [drive] + p
		return p
	# 'A B C D'
	return path.split()
	

def CpFile( src, dst ) :
	try :
		shutil.copyfile( src, dst )
		return True
	except :
		return False


def RmFile( path ) :
	try :
		SetFileReadWrite( path )
		os.remove( path )
		return True
	except :
		return False


def RmFiles( filter ) :
	files = glob.glob( filter )
	for f in files :
		RmFile( f )
	return files


def Rm( filter ) :
	files = glob.glob( filter )
	for f in files :
		if os.path.isdir( f ) :
			Rmtree( f )
		elif os.path.isfile( f ) or os.path.islink( f ) :
			os.remove( f )


def RmTree( path ) :
	try :
		shutil.rmtree( path, True )
		return True
	except :
		return False

		
def SetCWD( path ) :
	if path :
		os.chdir( path )


def GetCWD() :
	return os.getcwd()


def TouchFile( filepath ) :
	filename = ToStringPath( filepath )
	if len(filename) == 0 :
		return
	try :
		lines = open( filename,'r').readlines()
	except :
		try :
			open(filename,'w').writelines()
			return True
		except :
			return False


def CreateDir( path, rec=False ) :
	try :
		if rec :	os.makedirs( path )
		else :		os.mkdir( path )
		return True
	except :
		return False


def CreateFile( path, rec=False, content=None ) :
	RmFile( path )
	dir = os.path.dirname( path )
	CreateDir( dir, rec )
	try :
		open(path,'w').writelines( content or [''] )
		return True
	except :
		return False


def GlobPaths( path ):
	""" GlobPaths( 'src *' )
		Returns matched files and directories
	"""
	return glob.glob( ToStringPath(path) )


def GlobFiles( path ):
	""" GlobFiles( 'src * *.cpp' )
		Returns matched files  
	"""
	pl = GlobPaths( path )
	fl = []
	for p in pl :
		if os.path.isfile( p ) :
			fl.append( p )
	return fl


def GlobDirs( path ):
	""" GlobDirs( 'src *' )
		Returns matched directories
	"""
	pl = GlobPaths( path )
	dl = []
	for p in pl :
		if os.path.isdir( p ) :
			dl.append( p )
	return dl


def FindDir( rootdir, basename ) :
	for path, dirs, files in os.walk( str(rootdir) ):
		if basename in dirs :
			return os.path.join(path,basename)
	return None


def FindDirs( rootdir, excldirs=None ):
	""" FindDirs( 'src', 'win32 ps2' )
		Returns not empty sub-directories not matching the exclude list if done.
	"""
	rootdir = Path( rootdir )
	dl = []
	for path, dirs, files in os.walk( rootdir ):
		if excldirs and IsPathExcluded(path,excldirs):
			continue
		if len(files) :
			dl.append( path )
	return dl


def FindFile( rootdir, basename ) :
	for path, dirs, files in os.walk( str(rootdir) ):
		if basename in files :
			return os.path.normpath( os.path.join(path,basename) )
	return None


def FindFileInDirs ( filename, listdirs ) :
	if os.path.isfile( filename ) :
		return os.path.normpath(filename)
	for dir in atmake.ToList(listdirs) :
		f = os.path.join( dir, filename )
		if os.path.isfile(f) :
			return os.path.normpath(f)
	return None


def FindFiles( rootdir, extlist=None, excldirs=None ):
	""" FindFiles( 'src', 'cpp h' 'win32 ps2' )
		Returns files in all sub-directories not matching the exclude list if done
		and with extensions in the extlist if done.
	"""
	rootdir = ToStringPath( rootdir )
	if extlist and atmake.IsString(extlist):
		extlist = extlist.split(';')
	fl = []
	for path, dirs, files in os.walk( rootdir ):
		if excldirs and IsPathExcluded(path,excldirs):
			continue
		for f in files :
			fe = atmake.GetFileExt( f )
			if (not extlist) or (fe in extlist) :
				fl.append( os.path.normpath(os.path.join(path,f)) )
	return fl


def IsPathExists ( path ) :
	if atmake.IsString(path):
		return os.path.exists( path )
	else:
		return False


def IsFilePath ( path ) :
	if atmake.IsString(path):
		return os.path.isfile( path )
	else:
		return False


def IsDirPath ( path ) :
	if atmake.IsString(path):
		return os.path.isdir( path )
	else:
		return False


def IsAbsPath( path ) :
	if atmake.IsString(path):
		return os.path.isabs( path )
	else:
		return False


def IsInDir ( refdir, path ) :
	n0 = os.path.normpath( path )
	n1 = os.path.normpath( refdir )
	p0 = ToListPath( n0.lower() )
	p1 = ToListPath( n1.lower() )
	return ( p0[:len(p1)] == p1 )


def RelativeFile ( refdir, path ) :
	if not refdir or len(refdir)==0 :
		return path
	if not IsAbsPath(path) :
		atmake.Error( "File pathname <%s> must be absolute !" % path )
	if not IsAbsPath(refdir) :
		atmake.Error( "Reference directory <%s> must be absolute !" % refdir )
	n0 = os.path.normpath( path )
	n1 = os.path.normpath( refdir )
	p0 = ToListPath( n0.lower() )
	p1 = ToListPath( n1.lower() )
	ret = []
	if p0 :
#		if p0[:len(p1)] != p1 :
#			atmake.Error( "File <%s> isn't in base directory <%s> !" % (path,refdir) )
		p0 = ToListPath( n0 )
		ret.extend(p0[len(p1):])
	return ToStringPath( ret )


def RelativeFiles ( refdir, paths ) :
	l = []
	for p in atmake.ToList(paths) :
		l.append( RelativeFile(refdir,p) )
	return l


def FileToFile ( tofile, fromfile ) :
	fromdir = ToListPath( os.path.normpath( os.path.dirname(fromfile) ) )
	todir   = ToListPath( os.path.normpath( tofile ) )
	while 1 :
		if len(todir)==0:
			break
		if len(fromdir)==0:
			break
		if todir[0].lower()!=fromdir[0].lower():
			break
		todir = todir[1:]
		fromdir = fromdir[1:]
	f2f = ['..']*len(fromdir) + todir
	return ToStringPath(f2f)


def AbsoluteFile ( basedir, path ) :
	if not IsAbsPath(basedir) :
		atmake.Error( "Base directory <%s> must be absolute !" % basedir )
	if IsAbsPath(path) :
		atmake.Error( "File pathname <%s> must be relative !" % path )
	return os.path.normpath( os.path.join( basedir, path ) )


def AbsoluteFiles ( basedir, paths ) :
	l = []
	for p in atmake.ToList(paths) :
		l.append( AbsoluteFile(basedir,p) )
	return l


def GetFileMTime ( path ) :
	try :		return os.path.getmtime( path )
	except :	return None


def GetFileSize ( path ) :
	try :		return os.path.getsize( path )
	except :	return None


def SetFileReadWrite ( path ) :
	try :
		mod = os.stat( path )[0]
		ro  = mod | stat.S_IWRITE
		os.chmod( path, ro )
	except :
		pass


def SetFileReadOnly ( path ) :
	try :
		mod = os.stat( path )[0]
		ro  = mod & (~stat.S_IWRITE)
		os.chmod( path, ro )
	except :
		pass


def FileReadlines ( path ) :
	lines = None
	try:
		import codecs
	except ImportError:
		pass
	else :
		try:
			f = codecs.open(path, 'r', 'utf16')
			encoder = codecs.getencoder('ascii')
			lines = list(map(lambda l, e=encoder: e(l)[0], f.readlines()))
		except (LookupError, UnicodeError):
			lines = codecs.open(path, 'r', 'utf8').readlines()
	if lines is None:
		lines = open(path, 'r').readlines()
	return lines


def AppendPath ( curpath, path ) :
	if not curpath :			curpath = []
	elif atmake.IsString(curpath) :	curpath = curpath.split( os.pathsep )
	elif atmake.IsList(curpath) :		pass
	else :						atmake.Error( 'Invalid current PATH type !' )

	if not path :				path = []
	elif atmake.IsString(path) :		path = path.split( os.pathsep )
	elif atmake.IsList(path) :			pass
	else :						atmake.Error( 'Invalid PATH type !' )

	for p in path :
		if p and not p in curpath :
			curpath.append( str(p) )

	return string.join( curpath, os.pathsep )


def PrependPath ( curpath, path ) :
	if not curpath :			curpath = []
	elif atmake.IsString(curpath) :	curpath = curpath.split( os.pathsep )
	elif atmake.IsList(curpath) :		pass
	else :						atmake.Error( 'Invalid current PATH type !' )

	if not path :				path = []
	elif atmake.IsString(path) :		path = path.split( os.pathsep )
	elif atmake.IsList(path) :			pass
	else :						atmake.Error( 'Invalid PATH type !' )

	for p in path :
		if p and not p in curpath :
			curpath.insert( 0, str(p) )

	return string.join( curpath, os.pathsep )


def BuildPath ( path ) :
	return AppendPath( [], path )


def ToAbsPath ( path ) :
	if os.path.isabs(path):		apath = path
	else:						apath = os.path.join( os.getcwd(), path )
	return Path( apath )

def ToAbsFile ( env, f ) :
		"""
			returns a file relative to base directory
		"""
		if not f :			return None
		elif IsAbsFile(f) :		return f
		else :			return AbsoluteFile( env['buildroot'], f )

def ToTmpFile ( env, f ) :
	if 'buildroot' in env :		tpath = os.path.join(env['buildroot'], 'buildtmp')
	else:						tpath = os.path.join( os.getcwd(), 'buildtmp' )
	return os.path.join( tpath, f )

def ToBaseFile(env, f):
	if IsAbsFile(f) and 'buildroot' in env :
		return RelativeFile( env['buildroot'], f )
	else :
		return f

def Path ( path ) :
	if path :
		return os.path.normpath(path).replace('\\','/')
	else :
		return path


def FileMove ( dst, src ) :
	os.rename( src, dst )

	
def DigestTree ( inpath ) :
	sign = hashlib.new()
	for root, dirs, files in os.walk(inpath) :
		for f in files :
			p = os.path.join(root,f)
			sign.update( open(p,'r').read() )
	return sign.hexdigest()


def SyncFolders ( in_destdir, in_sourcedir, filters=None, verbose=None, addonly=None ) :

	def accept_path ( p ) :
		keep = True
		for f in filters :
			if f.startswith('-') :
				if fnmatch.fnmatch(p,f[1:]):	keep = False
			elif f.startswith('+') :
				if fnmatch.fnmatch(p,f[1:]):	keep = True
			else :
				if fnmatch.fnmatch(p,f):		keep = True
		return keep

	filters = (filters or '').split()

	# parse source directory
	# build lists of all files and directories
	sdirs  = []
	sfiles = []
	for root, dirs, files in os.walk(in_sourcedir) :
		# files
		for f in files :
			p = os.path.join(root,f)
			r = RelativeFile(in_sourcedir,p)
			if accept_path(p) :
				sfiles.append(r)
		# dirs (carefull, the dirs list can be altered !)
		for d in list(dirs) :
			p = os.path.join(root,d)
			r = RelativeFile(in_sourcedir,p)
			if accept_path(p):	sdirs.append(r)
			else :				dirs.remove(d)

	#for d in sdirs:	print d
	#for f in sfiles:	print f

	# sync directories
	# step 1 : create the dest directory
	if not os.path.isdir(in_destdir) :
		os.makedirs(in_destdir)

	# step 2 : remove old directories
	# (carefull, dirs list can be altered !)
	if not addonly is True :
		for root, dirs, files in os.walk(in_destdir) :
			for d in list(dirs) :
				p = os.path.join(root,d)
				r = RelativeFile(in_destdir,p)
				if not r in sdirs :
					shutil.rmtree( p, ignore_errors=True )
					if verbose:	print('tree "%s" removed' % r)
					dirs.remove(d)

	# step 3 : create new directories
	for d in sdirs :
		p = os.path.join( in_destdir, d )
		if not os.path.isdir(p) :
			os.makedirs(p)
			if verbose:	print('tree "%s" added' % d)

	# sync files
	# step 4 : remove old files
	if not addonly is True :
		for root, dirs, files in os.walk(in_destdir) :
			for f in files :
				p = os.path.join(root,f)
				r = RelativeFile(in_destdir,p)
				if not r in sfiles :
					os.remove(p)
					if verbose:	print('file "%s" removed' % r)

	# step 5 : update files
	for f in sfiles :
		srcp = os.path.join( in_sourcedir, f )
		dstp = os.path.join( in_destdir, f )
		if not os.path.isfile(dstp) :
			shutil.copyfile( srcp, dstp )
			if verbose:	print('file "%s" added' % f)
		else :
			srcstamp = sign.GetFileStampSign( srcp )
			dststamp = sign.GetFileStampSign( dstp )
			if srcstamp==dststamp :
				if verbose:	print('file "%s" is up-to-date' % f)
			else :
				shutil.copyfile( srcp, dstp )
				if verbose:	print('file "%s" updated' % f)



cur_output_path = None

def OutputPath ( in_dir ) :
	CreateDir( in_dir, rec=True )
	assert in_dir and os.path.isdir(in_dir)
	global cur_output_path
	cur_output_path = in_dir

def OutputFiles ( in_pattern ) :
	assert cur_output_path and os.path.isdir(cur_output_path)
	flist = glob.glob( in_pattern )
	for f in flist :
		src_f = f
		dst_f = os.path.join( cur_output_path, os.path.basename(f) )
		CpFile( src_f, dst_f )

def OutputFile ( in_pattern, in_newname=None ) :
	assert cur_output_path and os.path.isdir(cur_output_path)
	flist = glob.glob( in_pattern )
	assert not flist or len(flist)==1
	src_f = flist[0]
	dst_b = in_newname or os.path.basename(flist[0])
	dst_f = os.path.join( cur_output_path, dst_b )
	CpFile( src_f, dst_f )
	
def IsAbsFile( path ) :
	if path :
		return os.path.isabs( path )
	else :	return False
