""" ATMake

Builder base class

"""


import os, sys, os.path
import atmake
from atmake.tools import sign
from atmake.tools import scanner
from atmake.tools import fs
from atmake.sidl import subst_build




class BuildFiles :

	target	= None
	sources	= None


	def __init__	(	self,
					target	= None,
					sources	= None	) :
		self.target = target
		self.sources = atmake.iif( sources, atmake.ToList(sources), [] )


	def AddSource ( self, f ) :
		if f and not f in self.sources :
			self.sources.append( f )






class Builder :

	name		= None
	one2one	= None
	target_ext	= None
	source_ext	= None
	actions	= None


	def __init__ (	self,
				targetExt,
				sourceExt,
				actions,
				one2one,
				name		= None	) :
		"""
			string			targetExt	: target extension (ex 'obj')
			string/[]		sourceExt	: source extension list (ex 'cpp', ['cpp','c'], 'cpp c'])
			[]			actions	: list of actions to process
			bool			one2one	: [True] each source generates a target / [False] single target for all sources
		"""

		self.name		= name
		self.one2one	= atmake.iif( one2one, True, False )
		self.target_ext	= self.__fmt_target_ext( targetExt )
		self.source_ext	= self.__fmt_source_ext( sourceExt )
		self.actions		= self.__fmt_actions( actions )


	def IsOne2One( self ) :
		return self.one2one


	def GetTargetExt ( self ) :
		return self.target_ext


	def GetSourceExt ( self ) :
		return self.source_ext


	def AddSourceExt ( self, sourceExt ) :
		extl = self.__fmt_source_ext( sourceExt )
		for e in extl :
			if not e in self.source_ext :
				self.source_ext.append( e )


	def GetSourceTmpTarget ( self, fw, source ) :
		"""
			Framework		fw
			str			source
		"""
		if not source :	return None
		_src = fw.ToSourceFile( source )
		_trg = fw.ToTmpFile( atmake.ToFileExt(_src,self.target_ext) )
		return _trg


	def GetSourceTarget ( self, fw, source, target=None ) :
		"""
			Framework		fw
			str			source
			str			target
		"""
		if self.one2one :
			# one source, one target
			_trg = self.GetSourceTmpTarget( fw, source )
			if target and (target != _trg) :
				atmake.Error( "One2One builder can't use the specific target <%s> !" % target )
		else :
			# multi sources to one target
			if not target :
				atmake.Error( "Target file is missing !" )
			_trg = fw.ToTargetFile( target )
		return _trg


	def SubstActions ( self, ctxt ) :
		"""
			Context			ctxt
		"""
		actions = []
		for a in self.actions :
			if a :
				sa = subst_build.Subst( ctxt, a )
				if sa :
					actions.extend( atmake.ToList(sa) )
		if not actions :	return None
		else :		return actions


	def __fmt_target_ext ( self, ext ) :
		if not atmake.IsString(ext) :
			atmake.Error( "Target extension must be a string !" )
		if ext.startswith('.') :		return ext[1:]
		else :				return ext

	def __fmt_source_ext ( self, ext ) :
		if atmake.IsString(ext) :	extl = ext.split()
		else :		extl = atmake.ToList(ext)
		extr = []
		for e in extl :
			if not atmake.IsString(e) :
				atmake.Error( "Source extension must be a string !" )
			if e.startswith('.') :	extr.append( e[1:] )
			else :			extr.append( e )
		return extr

	def __fmt_actions ( self, actions ) :
		actions = [ e.replace('\\','/') for e in actions ]
		return atmake.ToList( actions )





class BuildNode :

	builder	= None
	files		= None
	nid		= None
	rids		= None


	def __init__ ( self, builder ) :
		self.builder	= builder
		self.files	= []			# list of build files
		self.nid	= None			# node id
		self.rids	= []			# node run ids (i.e. sub-node ids)


	def Count ( self ) :
		return len(self.files)


	def GetSources ( self ) :
		s = []
		for bf in self.files :
			s.extend( bf.sources )
		return s


	def GetTargets ( self ) :
		return [bf.target for bf in self.files]


	def FindTarget ( self, target	) :
		if not target :
			return None
		for i in range( len(self.files) ) :
			if self.files[i].target == target :
				return i


	def HasTarget ( self, target ) :
		ti = self.FindTarget( target )
		return not (ti is None)


	def AddTarget ( self, target ) :
		if not target :
			return None
		ti = self.FindTarget( target )
		if ti is None :
			self.files.append( BuildFiles(target) )
			ti = len(self.files) - 1
			atmake.Assert( (ti==0) or self.builder.IsOne2One(), 'Invalid state !' )
		return ti


	def AddSource ( self, target, source ) :
		atmake.Assert( target, 'Invalid target (src=%s) !' % source )
		if target and source :
			ti = self.AddTarget( target )
			self.files[ti].AddSource( source )


	def dbg_output ( self ) :
		print()
		print('----[build node %d]-----------------' % self.nid)
		print('run-ids: %s' % string.join( [str(x) for x in self.rids] ))
		print('targets: %d' % len(self.files))
		for i in range(len(self.files)) :
			print(r'[%d]' % i)
			print(r'    target:')
			print(r'        %s' % self.files[i].target)
			print()
			print(r'    sources:')
			for s in self.files[i].sources :
				print(r'        %s' % s)
			print()
		print()