

import os, sys, os.path



class IniFile ( ) :
	'''
		fpath
		seclimit
		section
	'''

	def __init__ ( self, inpath, inlimit=50 ) :
		self.fpath = inpath
		self.seclimit = inlimit
		self.section = {}

	def load ( self ) :
		if os.path.isfile( self.fpath ) :
			lines = open(self.fpath).readlines()
			sname = None
			slines = None
			for l in lines :
				if l.startswith('[') :
					if sname:
						self.section[sname] = slines
					sname = l[1:].split(']')[0]
					slines = {}
				elif not l.isspace() :
					k, v = l.split('=',1)
					k = k.strip()
					v = v.strip()
					if k.isdigit() :
						k = int(k)
					slines[k] = v
			if sname:
				self.section[sname] = slines

	def addsection ( self, sname ) :
		if not sname in self.section :
			self.section[sname] = {}

	def save ( self ) :
		lines = []
		lines.append( '\n' )
		for sname,svalue in self.section.items() :
			lines.append( '[%s]\n' % sname )
			for i,k in enumerate(sorted(svalue.keys())) :
				assert( isinstance(i,int) )
				assert( isinstance(k,int) )
				if i>=self.seclimit:	break
				lines.append( '%s = %s\n' % (str(k),svalue[k]) )
			lines.append( '\n' )
		open(self.fpath,'w').writelines( lines )


