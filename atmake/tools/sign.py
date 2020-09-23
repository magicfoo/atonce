"""

	This file is part of the atmake package.
	(c) AtOnce Technologies

"""


import os, sys, os.path, shelve, pickle, shutil
from hashlib import sha1 as sha
import atmake
from atmake.tools import fs
from atmake.tools import types




def GetFileContentSign ( path ) :
	try :
		lines = open(path,'r').readlines()
	except :
		return None
	so = sha()
	for l in lines :
		so.update( l.encode() )
	return so.hexdigest()


def GetLinesSign ( lines ) :
	so = sha()
	for l in atmake.ToList(lines) :
		so.update( l.encode() )
	return so.hexdigest()


def GetFileStampSign ( path ) :
	try:
		mtime = os.path.getmtime( path )
		bsize = os.path.getsize( path )
	except:
		return
	so = sha()
	hstr = 'mtime[%s]' % str(mtime)
	so.update( hstr.encode() )
	hstr = 'bsize[%s]' % str(bsize)
	so.update( hstr.encode() )
	return so.hexdigest()




class FileSign :
	'''
		fname		filename
		stamp		stamp sign
		content		content sign
	'''

	def __init__ ( self, fname=None, stamp=None, content=None ) :
		self.fname = fname
		self.stamp = stamp
		self.content = content




class TargetSign :
	'''
		fname		target filename
		stamp		target file stamp sign
		content		target file content sign
		actions		string signature of actions used to build the target
		depends		list of FileSign, one for each file dependencies
	'''

	def __init__ ( self, fname=None, stamp=None, content=None, actions=None ) :
		self.fname = fname
		self.stamp = stamp
		self.content = content
		self.actions = actions
		self.depends = []

	def AddDepend ( self, fname, stamp=None, content=None ) :
		atmake.Assert( fname  )
		fsign = FileSign( fname, stamp, content )
		self.depends.append( fsign )



class BuildState :
	UP_TO_DATE,				\
	NO_CACHE_ENTRY,			\
	ACTIONS_CHANGED,		\
	TARGET_FILE_CHANGED,	\
	TARGET_FILE_DELETED,	\
	DEPENDS_CHANGED,		\
	DEPEND_FILE_CHANGED,	\
	DEPEND_FILE_DELETED		= list(range(8))



class BuildSignCache :

	sfile	= None
	d	= None

	def __init__ ( self, ctxt ) :
		self.ctxt = ctxt
		tmpdir = os.path.split(ctxt['TARGET'])[0]
		self.sfile = fs.ToAbsFile( ctxt['env'], os.path.join( tmpdir, '.atmakesign' ) )
		self.d = shelve.open( self.sfile, protocol=0 )


	def __del__ ( self ) :
		self.d.close()


	def __actions_sign ( self, actions ) :
		if actions :
			so = sha()
			for a in atmake.ToList(actions) :
				a = str(a)
				so.update( a.encode() )
			return so.hexdigest()
		else :
			return None


	def __cache_error ( self ) :
		RmFile( self.sfile )
		atmake.Error( "\n\n** The cache file <%s> seems to be corrupted !!!\n** This file has been automatically removed.\n** Please restart the whole operation to complete.\n\n\n\n" % self.sfile, 0 )


	def IsInCache ( self, fname ) :
		return fname in self.d


	def Uncache ( self, fname ) :
		try :		del self.d[fname]
		except :	pass


	def IsUptodate ( self, fname, depends, actions ) :
		'''
			str			fname	: The target filename
			[]			depends	: List of file dependencies
			[]			actions	: List of associated actions
		'''
		def __check_file ( afile, stamp, content ) :
			if GetFileStampSign(afile) == stamp :		return True
			if GetFileContentSign(afile) == content :	return True
			return False
		
		atarget = fs.ToAbsFile( self.ctxt['env'], fname )
		
		# target in cache ?
		if not self.IsInCache(fname) :
			self.d[ fname ] = TargetSign(	fname,
											GetFileStampSign(atarget),
											GetFileContentSign(atarget),
											self.__actions_sign(actions)	)
			return BuildState.NO_CACHE_ENTRY

		
		try:
			ctarget = self.d[ fname ]
		except pickle.UnpicklingError :
			self.__cache_error()

		# check actions
		if self.__actions_sign(actions) != ctarget.actions :
			self.d[ fname ] = TargetSign(	fname,
											GetFileStampSign(atarget),
											GetFileContentSign(atarget),
											self.__actions_sign(actions)	)
			return BuildState.ACTIONS_CHANGED

		# check target
		if not os.path.isfile(atarget) :
			return BuildState.TARGET_FILE_DELETED
		if not __check_file(atarget,ctarget.stamp,ctarget.content) :
			self.d[ fname ] = TargetSign(	fname,
											GetFileStampSign(atarget),
											GetFileContentSign(atarget),
											self.__actions_sign(actions)	)
			return BuildState.TARGET_FILE_CHANGED
		"""
		# check depends
		if len(depends) != len(ctarget.depends) :
			return BuildState.DEPENDS_CHANGED
		for dep in ctarget.depends :
			if not (dep.fname in depends) :
				return BuildState.DEPENDS_CHANGED
			afile = fs.ToAbsFile( self.ctxt['env'], dep.fname )
			if not __check_file(afile,dep.stamp,dep.content) :
				if not os.path.isfile(afile) :
					return BuildState.DEPEND_FILE_DELETED
				return BuildState.DEPEND_FILE_CHANGED
		"""
		return BuildState.UP_TO_DATE


	def SetUptodate ( self, fname, depends, actions ) :
		'''
			BuildFiles		bf
			[]				actions
		'''	
		# remove target from cache
		self.Uncache( fname )

		# setup target
		atarget = fs.ToAbsFile( self.ctxt['env'], fname )
		if not fs.IsFilePath( atarget ) :
			return False
		ctarget = TargetSign(	fname,
								GetFileStampSign(atarget),
								GetFileContentSign(atarget),
								self.__actions_sign(actions)	)
		"""
		# setup sources
		for dep in depends :
			afile = fs.ToAbsFile( self.ctxt['env'], dep )
			if fs.IsFilePath( afile ) :
				ctarget.AddDepend( dep, GetFileStampSign(afile), GetFileContentSign(afile) )
			else :
				ctarget.AddDepend( dep )
		"""

		self.d[ fname ] = ctarget
		return True




