
import sys, os, copy, re, fnmatch, imp, queue, glob
import shelve
import ply.lex as lex
import ply.yacc as yacc
import atmake
from atmake.tools import fs
from atmake.tools import bin2h
from atmake.tools import types
from atmake.tools import sign
from atmake.sidl import subst
from atmake.sidl import error







re_typeid_p    = r'[a-zA-Z_]\w*'
re_typescope_p = r'((\s*::\s*%s)+|((%s\s*::\s*)+%s))' % (re_typeid_p,re_typeid_p,re_typeid_p)
re_type_scope  = re.compile( re_typescope_p )

def SIDL_ScopeFilter ( intext, lang='cpp' ) :
	def _cpp_scope_filter ( t ) :
		g0 = re_type_scope.search( t )
		if g0 :
			s0 = g0.group(0)
			t0 = s0.split('::')[-1]
			return _cpp_scope_filter( t.replace(s0,t0) )
		else :
			return t
	return _cpp_scope_filter( intext )


def clean_py_code ( line ) :
	lines = line.splitlines()
	lines = types.UnindentLines( lines )
	return '\n'.join( lines )






class SIDL_Env :
	'''
		globals		: dictionary of SIDL global symbols
		execenv		: environement for the exec command
	'''

	def __init__ ( self, env=None ) :
		def get_builtins() :
			return {	'True':bool(1),
						'False':bool(0),
						'bool':__builtins__['bool'],
						'__import__':__builtins__['__import__']		}
		self.globals = { '__builtins__':1 }
		self.execenv = { '__builtins__':get_builtins(), 'globals':self.globals }
		self.Update( env )
	
	def __str__(self):
		s = ''
		for k,v in self.globals.items():
			s += k+' => '+str(v)+'\n'
		return s

	def Copy ( self, env=None ) :
		if env and isinstance(env,SIDL_Env) :
			self.globals = env.globals.copy()

	def Update ( self, env=None ) :
		if env :
			if isinstance(env,SIDL_Env) :
				self.globals.update( env.globals )
			elif isinstance(env,dict) :
				self.globals.update( env )

	def Clone ( self ) :
		return SIDL_Env( env=self )

	def SetVar ( self, var, value ) :
		self.globals[var] = value

	def GetVar ( self, var, defvalue=None ) :
		return self.globals.get( var, defvalue )

	def GetVarlist ( self, var ) :
		svar = self.GetVar( var )
		return [ self.Subst(p) for p in atmake.ToList(svar) ]

	def Subst ( self, pattern, loc=None, moreglobals=None ) :
		if moreglobals :
			env = {}
			env.update( self.globals )
			env.update( moreglobals )
		else :
			env = self.globals
		return subst.Subst( env=env, pattern=pattern, loc=loc )

	def SubstVar ( self, var, loc=None ) :
		return self.Subst( r'$%s'%var, loc=loc )

	def SubstLines ( self, lines, loc=None ) :
		olines = []
		for l in atmake.ToList(lines) :
			olines.append( self.Subst(l,loc=loc) )
		return olines

	def GetPath ( self ) :
		return self.GetVarlist('path')

	def AppendVar ( self, var, value ) :
		cvalue = self.GetVar( var )
		if cvalue is None:		cvalue = []
		if atmake.IsString(value):		value = value.split(';')
		r = atmake.ToList(cvalue) + atmake.ToList(value)
		self.SetVar( var, r )

	def PrependVar ( self, var, value ) :
		cvalue = self.GetVar( var )
		if cvalue is None:		cvalue = []
		if atmake.IsString(value):		value = value.split(';')
		r = atmake.ToList(value) + atmake.ToList(cvalue)
		self.SetVar( var, r )

	def Eval ( self, expr ) :
		e = clean_py_code( expr )
		e = e.strip()
		e = e.replace( 'true',  'True'  )
		e = e.replace( 'false', 'False' )
		try:	r = eval( e, self.globals )
		except:	r = None
		return r

	def Exec ( self, code ) :
		c = clean_py_code( code )
		try:		exec(c, self.execenv)
		except:		pass





class SIDL_Location :
	'''
		SIDL parsing location

		filename		: str
		lineno			: int
		package			: str
		protocol		: str
	'''	

	def __init__ ( self, f=None, l=None, package=None ) :
		self.filename = f
		self.lineno   = l
		self.package  = package

	def __str__ ( self ) :
		return "sidl-loc '%s'[%d]" % (self.filename,self.lineno)

	def Here ( self ) :
		return '%s[%s]' % ( (self.filename or '?'), (self.lineno or '?') )

	def Scope ( self ) :
		return self.package

	def MatchScope ( self, scope ) :
		sc = self.Scope()
		if isinstance(scope,str) :
			return (sc==scope) or fnmatch.fnmatch(sc,scope)
		elif isinstance(scope,list) :
			for s in scope :
				if fnmatch.fnmatch(sc,s) :
					return True
		elif isinstance(scope,SIDL_Module) :
			return scope.MatchScope( self )



class SIDL_Object :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }
	'''

	def __init__ ( self, uri=None, loc=None, env=None, attributes=None ) :
		self.uri			= uri
		self.env			= SIDL_Env( env=env )
		self.loc			= loc or SIDL_Location()
		self.blockin		= None
		self.blockout		= None
		self.attributes		= attributes or {}

	def SetEnv( self, env=None ) :
		self.env = SIDL_Env( env=env )

	def SetBlock ( self, bin, bout ) :
		self.blockin  = bin
		self.blockout = bout

	def HasAttribute ( self, id ) :
		return id and id in self.attributes

	def GetAttribute ( self, id, defvalue=None ) :
		return self.attributes.get(id,defvalue)

	def SubstAttribute ( self, id, defvalue=None ) :
		attr = self.attributes.get(id,defvalue)
		if attr:	return self.env.Subst( attr )

	def AttributeEquals ( self, id, value, cs=None ) :
		v = self.attributes.get(id,None)
		if type(v)!=type(value) :
			return False
		if v==value :
			return True
		if v and value and atmake.IsString(v) and (cs is False) :
			return v.lower() == value.lower()
		return False

	def SetAttributes ( self, attrs ) :
		self.attributes = attrs or {}

	def GetAttributeList ( self, id ) :
		attr = self.GetAttribute( id )
		if atmake.IsString(attr):	return attr.split(';')
		else:				return atmake.ToList(attr)

	def MatchScope ( self, scope ) :
		return self.loc and self.loc.MatchScope( scope )




class SIDL_RawText ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		text			: str
		file			: str
	'''

	def __init__ ( self, uri=None, text=None, file=None, loc=None, env=None, attributes=None ) :
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.text = text
		self.file = file

	def __str__ ( self ) :
		s  = 'SIDL-rawtext\n'
		for k,v in self.attributes.items():
			s += ' | attribute %s=%s\n' % (str(k),str(v))
		if self.loc.lineno :
			s += ' | line %s\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		return s

	def GetInpath ( self ) :
		if self.file :
			return self.file

	def GetText ( self ) :
		try:
			if atmake.IsString(self.text) :
				return self.text
			if atmake.IsString(self.file) :
				return ''.join( file(self.file,'rb').readlines() )
		except:
			pass




class SIDL_Typedef ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		specifier		: 'enum', 'struct', 'delegate', ...
		identifier		: str
		super			: str
		declarations	: [ SIDL_Typedef or SIDL_Function or SIDL_RawText ]
	'''

	def __init__ ( self, specifier=None, identifier=None, super=None, declarations=None, loc=None, env=None, attributes=None ) :
		SIDL_Object.__init__( self, uri=identifier, loc=loc, env=env, attributes=attributes )
		self.specifier  = specifier
		self.identifier = identifier
		self.super = super
		self.declarations = atmake.ToList(declarations) or []

	def __str__ ( self ) :
		s  = 'SIDL-typedef\n'
		if self.specifier :
			s += ' | specifier %s\n' % (self.specifier or '?')
		if self.identifier :
			s += ' | identifier %s\n' % (self.identifier or '?')
		if self.super :
			s += ' | super %s\n' % (self.super or '?')
		return s

	def Filter ( self, TypeOf ) :
		return [x for x in self.declarations if isinstance(x,TypeOf)]

	def CollectSubTypes ( self ) :
		sub = []
		for t in self.Filter(SIDL_Typedef) :
			t_sub = [t.identifier] + t.CollectSubTypes()
			sub.extend( ['%s::%s'%(self.identifier,id) for id in t_sub] )
		return sub
	
	




class SIDL_Function ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		name			: str (function name)
		rtype			: str (return type)
		args			: [ (type-str,name-str,defvalue-str) ]
		code			: SIDL_RawText
		isconst			: true if const
		hasellipsis		: true if has ellipsis ('...') as the last argument
		declname		: relative declaration name (a classname) or None
		switched		: boolean
		switchname		: str (switch name)
		qualifier		: str ('static', 'inline', 'virtual', 'template', ...)
		mark			: str
	'''

	def __init__ ( self, name=None, loc=None, env=None, rtype=None, args=None, declname=None, qualifier=None, mark=None, switched=None, switchname=None, isconst=None, hasellipsis=None, attributes=None, code=None ) :
		SIDL_Object.__init__( self, uri=name, loc=loc, env=env, attributes=attributes )
		if qualifier=='static' and code :
			error.SIDL_Error( "The static function '%s' can't have a code section !"%name, loc=loc )
		self.name			= name
		self.rtype			= rtype or 'void'
		self.declname		= declname
		self.args			= args or []
		self.switched		= switched
		self.switchname		= switchname or name
		self.isconst		= isconst
		self.hasellipsis	= hasellipsis
		self.qualifier		= qualifier
		self.mark			= mark
		self.code			= None
		self.SetCode( code )

	def __str__ ( self ) :
		s  = 'SIDL-function %s %s%s()\n' % (self.rtype, ('%s::'%self.declname if self.declname else ''), (self.name or '?'))
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.isconst :
			s += ' | const\n'
		if self.hasellipsis :
			s += ' | ellipsis\n'
		if self.switched :
			s += ' | switch <%s>\n' % (self.switchname or self.name)
		if self.qualifier :
			s += ' | qualifier: %s\n' % self.qualifier
		if self.mark :
			s += ' | mark: %s\n' % self.mark
		if self.rtype :
			s += ' | output (%s)\n' % (self.rtype)
		for i,a in enumerate(self.args) :
			s += ' | input[%d] (%s) %s\n' % (i,a[0],a[1])
		for k,v in self.attributes.items():
			s += ' | attribute %s=%s\n' % (str(k),str(v))
		if self.code :
			s += ' | code :\n'
			for l in self.code.text.split('\n\r') :
				s += ' | %s\n' % l
		return s

	def PrependArg ( self, type=None, name=None, defvalue=None ) :
		if type :
			if type=='...' :
				self.hasellipsis = True
			else:
				self.args.insert( 0, (type,name,defvalue) )

	def AppendArg ( self, type=None, name=None, defvalue=None ) :
		if type :
			if type=='...' :
				self.hasellipsis = True
			else:
				self.args.append( (type,name,defvalue) )

	def SetCode ( self, c=None ) :
		if isinstance(c,SIDL_RawText) :
			self.code = c
		elif c and atmake.IsString(c) :
			self.code = SIDL_RawText(text=c)
		else :
			self.code = None

	def DefaultQualifier ( self, q=None ) :
		self.qualifier = self.qualifier or q

	def ReplaceType ( self, fromtype, totype ) :
		self.rtype = self.rtype.replace( fromtype, totype )
		self.args  = [ (a[0].replace(fromtype,totype),a[1], a[2]) for a in self.args ]

	def NoScope ( self ) :
		def _noscope(t) :
			return SIDL_ScopeFilter(t)
		f = copy.deepcopy( self )
		f.rtype = _noscope(f.rtype)
		f.args  = [ (_noscope(a[0]),a[1],a[2]) for a in self.args ]
		return f

	def GetComponents ( self ) :
		# [ name, rtype, arguments, ... ]
		co = []
		co.append( self.name )
		co.append( self.rtype )
		co.extend( [a[0] for a in self.args] )
		if self.qualifier:	co.append( self.qualifier )
		if self.isconst:	co.append( 'const' )
		return co

	def SameSignature ( self, f ) :
		# same components but the name
		co1 = f.GetComponents()
		co2 = self.GetComponents()
		return co1[1:] == co2[1:]

	def GetSignature ( self, sep='_', withrtype=None, nospace=None ) :
		# "name_[rtype_]arguments_[...]"
		co = self.GetComponents()
		if not withrtype:
			del co[1]
		s = sep.join(co)
		if nospace :
			s = s.replace(' ','')
		return s

	def GetMangled ( self ) :
		return self.GetSignature( sep='_', withrtype=True, nospace=True )

	def GetCppLines ( self, alignsign=None, compactsign=None, aligncode=None, indent=None, ljust=None ) :
		if ljust:		lqualifier,lrtype,lname,la0,la1,la2 = ljust
		else:			lqualifier=lrtype=lname=la0=la1=la2=0

		# qualifier part
		qualifier = ''
		if self.qualifier=='template' :
			qualifier = 'template<typename T>\n'

		# signature part
		sign = ''
		if self.qualifier and self.qualifier!='template':	sign += '%s ' % self.qualifier.ljust(lqualifier)
		elif lqualifier :									sign += ' '*(lqualifier+1)
		sign += '%s' % self.rtype.ljust(lrtype)
		sign += ' '*4
		sign += '%s' % self.name.ljust(lname)
		sign += ' '*4
		sign += '( '
		# with optional args
		args = ''
		if self.args :
			fargs = []
			for a in self.args :
				fa = []
				if 1:				fa.append( a[0].ljust(la0) )
				if a[1]:			fa.append( a[1].ljust(la1) )
				elif la1:			fa.append( ' '*la1 )
				if a[1] and a[2]:	fa.append( '=' )
				elif la2:			fa.append( ' ' )
				if a[1] and a[2]:	fa.append( a[2].ljust(la2) )
				elif la2:			fa.append( ' '*la2 )
				fargs.append( ' '.join(fa) )
			if self.hasellipsis:
				fargs.append( '...' )
			if alignsign:	args_sep = ', '
			else:			args_sep = ',\n'+' '*len(sign)
			args = args_sep.join(fargs)
		elif ljust :
			if alignsign:	args = ''
			else:			args = ' '*(la0+la1+la2+(1 if la1 else 0)+(1 if la2 else 0))
		# append args
		sign += args
		sign += ' )'
		if compactsign:
			sign = ' '.join( sign.split() )

		# body part
		body = ''
		if self.code :
			codetext = self.code.GetText()
			codelines = types.UnindentLines( codetext.splitlines() )
			if len(codelines)== 0 and (aligncode or aligncode==None) :
				body += ' { }'
			elif len(codelines)==1  and (aligncode or aligncode==None) :
				body += ' { %s }' % codelines[0]
			else :
				body += '\n{\n'
				body += ''.join( ['\t%s\n'%l for l in codelines] )
				body += '}\n'
			if aligncode:
				body = body.replace('\n',' ').replace('\t',' ').replace('  ',' ').replace('  ',' ')
		elif self.qualifier=='virtual' :
			body += ' = 0;'
		else :
			body += ';'

		decl = qualifier + sign + body
		return [ '%s%s\n'%(indent or '',l) for l in decl.splitlines() ]


def SIDL_Function_GetCppLines ( functions, alignsign=None, aligncode=None, indent=None, sep=None ) :
	lqualifier=lrtype=lname=la0=la1=la2=0
	for f in atmake.ToList(functions) :
		assert isinstance(f,SIDL_Function)
		lqualifier = max( lqualifier, len(f.qualifier or '') )
		lrtype     = max( lrtype, len(f.rtype) )
		lname      = max( lname, len(f.name) )
		for a in f.args :
			la0 = max( la0, len(a[0]) )
			la1 = max( la1, len(a[1] or '') )
			la2 = max( la2, len(a[2] or '') )
	if alignsign:	la0=la1=la2=0
	ljust=(lqualifier,lrtype,lname,la0,la1,la2)
	lines = []
	for f in atmake.ToList(functions) :
		if lines and sep:
			lines.append( sep )
		lines.extend( f.GetCppLines(alignsign=alignsign,aligncode=aligncode,indent=indent,ljust=ljust) )
	return lines


def SIDL_Function_YourCodeHere ( f, indent=None ) :
	assert isinstance(f,SIDL_Function)
	ff = copy.deepcopy(f)
	ff.qualifier = None
	ff.args = [(t,n,None) for t,n,_ in f.args]
	if   f.rtype=='void':		fcode = '// Your code here.\n'
	elif f.rtype=='bool':		fcode = '// Your code here.\nreturn false;\n'
	elif f.rtype=='float':		fcode = '// Your code here.\nreturn 0.f;\n'
	elif f.rtype=='pstr':		fcode = '// Your code here.\nreturn NULL;\n'
	elif f.rtype=='pcstr':		fcode = '// Your code here.\nreturn NULL;\n'
	elif '*' in f.rtype:		fcode = '// Your code here.\nreturn NULL;\n'
	else:						fcode = '// Your code here.\nreturn 0;\n'
	ff.SetCode( fcode )
	return ff.GetCppLines( alignsign=True, compactsign=True, aligncode=False, indent=indent )


def SIDL_Function_WithCode ( f, code, indent=None ) :
	assert isinstance(f,SIDL_Function)
	ff = copy.deepcopy(f)
	ff.qualifier = None
	ff.args = [(t,n,None) for t,n,_ in f.args]
	ff.SetCode( code )
	return ff.GetCppLines( alignsign=True, compactsign=True, aligncode=False, indent=indent )


	


class SIDL_Uid ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		id				: str
		alias			: str
	'''

	def __init__ ( self, id=None, alias=None, loc=None, env=None, attributes=None ) :
		if loc.package:		uri = '%s.%s' % (loc.package,id)
		else :				uri = id
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.id    = id
		self.alias = alias

	def __str__ ( self ) :
		s  = 'SIDL-uid %s\n' % (self.id or '?')
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		if self.alias :
			s += " | alias '%s'\n" % self.alias
		if self.loc.package :
			s += ' | package %s\n' % self.loc.package
		return s


		

		
class SIDL_StringId ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		name			: str
		string			: str
		private			: boolean
	'''

	def __init__ ( self, name=None, string=None, private=None, loc=None, env=None, attributes=None ) :
		if loc.package:		uri = '%s.%s' % (loc.package,name)
		else :				uri = name
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.name   = name
		self.string = string
		self.private = False if private is None else private

	def __str__ ( self ) :
		s  = 'SIDL-stringid %s\n' % (self.name or '?')
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		if 1 :
			s += " | string '%s'\n" % self.string
			s += " | private '%s'\n" % str(self.private)
		return s





class SIDL_StringEnum ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		name			: str
		members			: [ str-id ]
	'''

	def __init__ ( self, name=None, members=None, loc=None, env=None, attributes=None ) :
		if loc.package:		uri = '%s.%s' % (loc.package,name)
		else :				uri = name
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.name    = name
		self.members = members or []

	def __str__ ( self ) :
		s  = 'SIDL-stringenum %s\n' % (self.name or '?')
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		return s

		
		


	
	
class SIDL_CodeObject ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		usingfiles		: [ str ]
		sections		: { [SIDL_RawText] }
		subtypes		: [ SIDL_Typedef ]
	'''

	def __init__ ( self, uri=None, loc=None, env=None, attributes=None, sections=None ) :
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.subtypes	= []
		self.usingfiles = []
		if atmake.IsList(sections) :
			self.sections = {}
			for s in sections :
				self.sections[s] = []
		else:
			self.sections = sections or {}

	def AddUsingFile ( self, f=None ) :
		for fi in atmake.ToList(f) :
			if atmake.IsString(fi) :
				if not fi in self.usingfiles :
					self.usingfiles.append( fi )

	def AddSection ( self, id=None, rw=None ) :
		if atmake.IsString(id) and isinstance(rw,SIDL_RawText) :
			if id not in self.sections :
				error.SIDL_Error( "Section '%s' is not available for this code object !"%id, loc=rw.loc )
			self.sections[id].append( rw )

	def GetDependencies ( self ) :
		deps = []
		extend1( deps, self.usingfiles )
		return deps

	def AddSubType ( self, t ) :
		assert isinstance(t,SIDL_Typedef)
		self.subtypes.append( t )

	def FindSubType ( self, identifier ) :
		for s in self.subtypes :
			if s.identifier == identifier :
				return s





class SIDL_Interface ( SIDL_CodeObject ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }
		usingfiles		: [ str ]
		sections		: { [SIDL_RawText] }
		subtypes		: [ SIDL_Typedef ]

		fromfile		: str (* defined if imported)
		fromcpp			: declarations.class_t node (* defined if imported)
		classname		: str (including scope)
		classbase		: str (excluding scope)
		classpath		: str (the scope only)
		supername		: str
		super			: SIDL_Interface (corresponding to supername)
		functions		: [ SIDL_Function ]
		forwards		: [ uri ]
	'''

	def __init__ ( self, cname=None, loc=None, env=None, super=None, fromfile=None, attributes=None ) :
		assert( (super is None) or isinstance(super,SIDL_Interface) )
		if loc.package:		uri = '%s.%s' % (loc.package,cname)
		else :				uri = cname
		SIDL_CodeObject.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes, sections=['proper','code'] )
		self.fromfile  = fromfile
		self.fromcpp   = None
		self.classname  = cname
		self.classbase  = cname.split('::')[-1]
		self.classpath  = cname.split('::')[:-1]
		self.super      = super
		self.supername  = super.classname if super else None
		self.functions  = []
		self.proxies    = []
		self.forwards   = []
		self.fcache     = {}		# private
		self.env.SetVar( 'iclassname', self.classname )

	def __str__ ( self ) :
		s  = 'SIDL-interface %s\n' % (self.classname or '?')
		if self.fromfile :
			s += ' | from %s\n' % self.fromfile
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		if self.supername :
			s += ' | super %s\n' % self.supername
		for uf in self.usingfiles :
			s += ' | using %s\n' % uf
		for f in self.functions :
			s += ' | func %s %s::%s()\n' % (f.rtype,f.declname or '',f.name)
		for k,v in self.attributes.items():
			s += ' | attribute %s=%s\n' % (k,v)
		if self.loc.package :
			s += ' | package %s\n' % self.loc.package
		return s

	def AddFunction ( self, f=None ) :
		if isinstance(f,SIDL_Function) :
			if f.code :
				# append to section code instead !
				flines = f.GetCppLines()
				ftext  = ''.join(flines)
				fcode  = SIDL_RawText( text=ftext, loc=f.loc, env=f.env, attributes=f.attributes )
				self.AddSection( 'code', fcode )
			else :
				mang = f.GetMangled()
				if not mang in self.fcache :
					f.declname = f.declname or self.classbase
					self.functions.append( f )
					self.fcache[mang] = f

	def AddForward ( self, uri ) :
		if not uri in self.forwards :
			self.forwards.append( uri )

	def GetOutpath ( self ) :
		if not self.fromfile :
			p = self.env.SubstVar( 'iclassoutpath' )
			return fs.Path(p)

	def GetInpath ( self ) :
		if self.fromfile :
			return self.fromfile

	def GetDependencies ( self ) :
		deps = SIDL_CodeObject.GetDependencies( self )
		if self.super:
			extend1( deps, self.super.GetDependencies() )
		if self.fromfile :
			append1( deps, self.fromfile )
		else :
			opath = self.GetOutpath()
			if opath :
				append1( deps, opath )
		return deps

	def CollectSubTypes ( self ) :
		sub = []
		for p in self.proxies :
			sub.append( p.classname )
		for t in self.subtypes :
			assert isinstance(t,SIDL_Typedef)
			sub.append( t.identifier )
			sub.extend( t.CollectSubTypes() )
		return sub




class SIDL_Abstract ( SIDL_CodeObject ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }
		usingfiles		: [ str ]
		sections		: { [SIDL_RawText] }
		subtypes		: [ SIDL_Typedef ]

		classname		: str (including scope)
		classbase		: str (excluding scope)
		classpath		: str (the scope only)
		supername		: str (arbitrary supername)
		concerns		: [ SIDL_Interface ]
		bridges			: [ (concernid,interfaceid,funcname,value) ]
							with value of type SIDL_RawText
		states			: [ {uri,attributes} ]
	'''

	def __init__ ( self, cname=None, loc=None, env=None, supername=None, concerns=None, attributes=None ) :
		if loc.package:		uri = '%s.%s' % (loc.package,cname)
		else :				uri = cname
		SIDL_CodeObject.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes, sections=['proper','code','stub','builder'] )
		self.classname  = cname
		self.classbase  = cname.split('::')[-1]
		self.classpath  = cname.split('::')[:-1]
		self.supername  = supername
		self.concerns = concerns or []
		self.bridges  = []
		self.states = []
		self.env.SetVar( 'mclassname', self.classname )

	def __str__ ( self ) :
		s  = 'SIDL-abstract %s\n' % (self.classname or '?')
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		if self.supername:
			s += ' | super %s\n' % self.supername
		for uf in self.usingfiles :
			s += ' | using %s\n' % uf
		for c in self.concerns :
			s += ' | concern %s\n' % c.classname
		for b in self.bridges:
			s += ' | bridge for concern <%s>, repl <%s> with <%s>\n' % (b[0],b[1],b[2])
		for t in self.attributes:
			s += ' | attribute %s=%s\n' % (t[0],t[1])
		if self.loc.package :
			s += ' | package %s\n' % self.loc.package
		return s

	def AddBridge ( self, pattern, raw, loc ) :
		assert isinstance(pattern,str)
		assert isinstance(raw,SIDL_RawText)
		pl = pattern.split('.')
		if   len(pl)==1 :	b = ('*','*',pl[0],raw)
		elif len(pl)==2 :	b = ('*',pl[0],pl[1],raw)
		elif len(pl)==3 :	b = (pl[0],pl[1],pl[2],raw)
		else :				error.SIDL_Error( "Invalid bridge pattern. Please use the form [[concernid.]interfaceid.]funcname !", loc )
		self.bridges.append( b )

	def Concerns ( self, classname ) :
		for c in self.concerns :
			if c.classname==classname :
				return True

	def IsStateMachine ( self ) :
		return self.Concerns('IStateMachine')

	def IsSwitched ( self ) :
		return self.Concerns('ISwitch')

	def GetState ( self, uri ) :
		for s in self.states :
			if s['uri']==uri :
				return s

	def AddState ( self, uri, attributes=None, loc=None ) :
		if not self.IsStateMachine() :
			error.SIDL_Error( 'Invalid state definition. The implementation must concern the IStateMachine !', loc )
		attributes = attributes or {}
		# check the parents
		chain = uri.split('.')
		for i in range(1,len(chain)) :
			c = '.'.join(chain[:i])
			if not self.GetState(c) :
				error.SIDL_Error( 'Undefined parent state <%s> !'%c, loc )
		# check attributes
		for k,v in attributes.items() :
			if k=='start' :
				for s in self.states:
					for ak,av in s['attributes'].items() :
						if ak=='start' :
							error.SIDL_Error( 'The starting state <%s> is already defined !'%s['uri'], loc )
		# checked! append it
		self.states.append( {'uri':uri, 'attributes':attributes} )

	def GetSkelOutpath ( self ) :
		p = self.env.SubstVar( 'mclassoutpath' )
		return fs.Path(p)

	def GetStubOutpath ( self ) :
		p = self.env.SubstVar( 'sclassoutpath' )
		return fs.Path(p)

	def GetOutpath ( self ) :
		return self.GetSkelOutpath()

	def GetDependencies ( self ) :
		deps = SIDL_CodeObject.GetDependencies( self )
		for c in self.concerns:
			extend1( deps, c.GetDependencies() )
		return deps

	


		
class SIDL_Implementation ( SIDL_Abstract ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }
		classname		: str (including scope)
		classbase		: str (excluding scope)
		classpath		: str (the scope only)
		supername		: str
		usingfiles		: [ str ]
		sections		: { [SIDL_RawText] }
		subtypes		: [ SIDL_Typedef ]
		concerns		: [ SIDL_Interface ]
		bridges			: [ (concernid,interfaceid,funcname,value) ]

		qualifier		: str ('static', ...)
		abstract		: SIDL_Abstract
	'''

	def __init__ ( self, cname=None, loc=None, env=None, qualifier=None, supername=None, concerns=None, abstract=None, attributes=None ) :
		SIDL_Abstract.__init__( self, cname=cname, loc=loc, env=env, supername=supername, concerns=concerns, attributes=attributes )
		self.qualifier = qualifier
		self.abstract = abstract

	def __str__ ( self ) :
		q  = "'%s' "%self.qualifier.upper() if self.qualifier else ''
		s  = 'SIDL-implementation %s%s\n' % (q,self.classname or '?')
		if self.loc.filename :
			s += ' | file [%s]\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line [%s]\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		if self.supername:
			s += ' | super %s\n' % self.supername
		if self.abstract:
			s += ' | abstract %s\n' % self.abstract.classname
		for uf in self.usingfiles :
			s += ' | using %s\n' % uf
		for c in self.concerns:
			s += ' | concern %s\n' % c.classname
		for b in self.bridges:
			s += ' | bridge for concern <%s>, repl <%s> with <%s>\n' % (b[0],b[1],b[2])
		for k,v in self.attributes.items():
			s += ' | attribute %s=%s\n' % (str(k),str(v))
		if self.loc.package :
			s += ' | package %s\n' % self.loc.package
		return s



class SIDL_Test ( SIDL_CodeObject ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }
		usingfiles		: [ str ]
		sections		: { [SIDL_RawText] }
		subtypes		: [ SIDL_Typedef ]

		(no specific class member)
	'''

	def __init__ ( self, uri=None, loc=None, env=None, attributes=None ) :
		sections = ['proper','code','onrun','oninit','onshut','unit']
		SIDL_CodeObject.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes, sections=sections )
		self.env.SetVar( 'testname', uri.replace('.','_') )

	def __str__ ( self ) :
		s  = 'SIDL-test %s\n' % (self.uri or '?')
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		if self.loc.package :
			s += ' | package %s\n' % self.loc.package
		return s

	def GetOutpath ( self ) :
		p = self.env.SubstVar( 'testoutpath' )
		return fs.Path(p)

	def GetName ( self ) :
		tname = self.GetAttribute('name') or self.uri.replace('.','_')
		return tname




class SIDL_Group ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		folders			: [ (name,[files],[options]) ]
	'''

	def __init__ ( self, uri=None, folders=None, loc=None, env=None, attributes=None ) :
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.folders = folders or []

	def __str__ ( self ) :
		s  = 'SIDL-group %s\n' % (self.uri or '?')
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		for f in self.folders:
			s += ' | folder "%s"\n' % f[0]
		return s

	def AddFiles ( self, foldername=None, files=None, options=None ) :
		fo = ( foldername, files or [], options or [] )
		self.folders.append( fo )

	def AddEmptyFolder ( self, foldername ) :
		fo = ( foldername, [], [] )
		self.folders.append( fo )

	def CollectFolders ( self, basename=None ) :
		if basename is None :
			basename = self.GetAttribute('folder')
		co = []
		for f in self.folders :
			if basename and f[0]:	n = '%s.%s' % (basename,f[0])
			elif basename:			n = '%s' % basename
			elif f[0]:				n = '%s' % f[0]
			else:					n = ''
			co.append( (n,f[1],f[2]) )
		return co




class SIDL_Project ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		groups			: [ uri ]
	'''

	def __init__ ( self, uri=None, groups=None, loc=None, env=None, attributes=None ) :
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.groups = groups or []
		self.env.SetVar( 'projobj', self )
		self.env.SetVar( 'projname', self.uri.replace('.','_') )

	def __str__ ( self ) :
		s  = 'SIDL-project %s\n' % (self.uri or '?')
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		for g in self.groups:
			s += ' | group "%s"\n' % g
		return s

	def AddGroup ( self, uri, content=None ) :
		if uri :
			groups = []
			if content :
				assert isinstance(content,SIDL_Content)
				content_groups = content.groups.keys()
				for u in atmake.ToList(uri) :
					g = fnmatch.filter(content_groups,u)
					if not g :
						error.SIDL_Error( r'Group [%s] is not defined !'%u, self.loc )
					groups.extend( g )
			else :
				groups = atmake.ToList(uri)
			for g in groups :
				if not g in self.groups :
					self.groups.append( g )

	def GetOutpath ( self, defext=None ) :
		p = self.env.SubstVar( 'projoutpath' )
		e = os.path.splitext(p)[1]
		if not e and defext :
			if not defext.startswith('.') and not p.endswith('.') :
				p += '.'
			p = p + defext
		return fs.Path(p)




class SIDL_Solution ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		projects		: [ uri ]
		dependencies	: { uri : [uri] }
	'''

	def __init__ ( self, uri=None, projects=None, dependencies=None, loc=None, env=None, attributes=None ) :
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.projects = projects or []
		self.dependencies = dependencies or {}
		self.env.SetVar( 'projobj', self )
		self.env.SetVar( 'projname', self.uri.replace('.','_') )

	def __str__ ( self ) :
		s  = 'SIDL-solution %s\n' % (self.uri or '?')
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		for g in self.projects:
			deps = self.dependencies.get(g,[])
			if deps :	s += ' | project "%s" : %s\n' % (g,', '.join(deps))
			else :		s += ' | project "%s"\n' % (g)
		return s

	def AddProject ( self, uri, deps=None, content=None ) :
		if uri :
			if content :
				assert isinstance(content,SIDL_Content)
				check_uris = [uri] + atmake.ToList(deps)
				for u in check_uris :
					if not u in content.projects :
						error.SIDL_Error( r'Project [%s] is not defined !'%u, self.loc )
			self.projects.append( uri )
			self.dependencies[uri] = atmake.ToList(deps)

	def GetOutpath ( self, defext=None ) :
		p = self.env.SubstVar( 'projoutpath' )
		e = os.path.splitext(p)[1]
		if not e and defext :
			if not defext.startswith('.') :
				defext = '.%s'%defext
			p = p + defext
		return fs.Path(p)


		
		
		
	
	
class SIDL_BuildEnv ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location

		config			: { filename:str, params:dict }
		arguments		: SIDL_RawText
	'''

	def __init__ ( self, uri=None, loc=None, env=None, config=None, attributes=None ) :
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.config = config
		self.arguments = SIDL_RawText()

	def __str__ ( self ) :
		s  = 'SIDL-buildenv %s\n' % (self.uri or '?')
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		return s

	

class SIDL_BuildStep ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		inputs			: [ str ]
		outputs			: [ str ]
		qualifier		: 'UNARY' | 'MULTIPLE'
		actions			: SIDL_RawText
	'''

	def __init__ ( self, uri=None, loc=None, env=None, inputs=None, outputs=None, attributes=None ) :
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.inputs = inputs
		self.outputs = outputs
		self.qualifier = 'UNARY'
		self.actions = SIDL_RawText()

	def __str__ ( self ) :
		s  = 'SIDL-buildstep %s\n' % (self.uri or '?')
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		return s

	
	
class SIDL_BuildSetup ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		nodes			: [ SIDL_BuildStep|SIDL_BuildEnv ]
	'''
	
	def __init__ ( self, uri=None, loc=None, env=None, attributes=None ) :
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.nodes = []
	
	def __str__ ( self ) :
		s  = 'SIDL-buildsetup %s\n' % (self.uri or '?')
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		return s
	
	def AddNode( self, mri, content ) :
		nodes = []
		assert isinstance(content, SIDL_Content)
		content_builds = dict(list(content.buildenvs.items()) + list(content.buildsteps.items()))
		content_keys = list(content.buildenvs.keys())
		content_keys.extend( list(content.buildsteps.keys()) )
		for u in atmake.ToList(mri) :
			n = fnmatch.filter(content_keys, u)
			if not n :
				error.SIDL_Error( r'Build step [%s] is not defined !'%u, self.loc )
			else :
				for id in n :
					nodes.extend( [content_builds[id]] )
		for n in nodes :
			if not n in self.nodes :
				self.nodes.append( n )
			


			
			
			
class SIDL_Build ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }
		
		target			: uri
		tree			: uri
		solution		: uri
	'''
	
	def __init__ ( self, loc=None, env=None, target=None, tree=None, solution=None, attributes=None ) :
		SIDL_Object.__init__( self, loc=loc, env=env, attributes=attributes )
		self.target = target
		self.tree = tree
		self.solution = solution
		
	
	def __str__ ( self ) :
		s  = 'SIDL-build\n'
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		return s		




class SIDL_Module ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		name			: str
		scope			: [ mri ]
	'''

	def __init__ ( self, name=None, scope=None, loc=None, env=None, attributes=None ) :
		SIDL_Object.__init__( self, uri=name, loc=loc, env=env, attributes=attributes )
		self.name  = name
		self.scope = scope or []

	def __str__ ( self ) :
		s  = 'SIDL-module %s\n' % (self.name or '?')
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		for o in self.scope:
			s += " | scopes '%s'\n" % o
		return s

	def AddScope ( self, mri ) :
		if mri :
			if not mri in self.scope :
				self.scope.append( mri )

	def MatchScope ( self, scope ) :
		if scope is None :
			return True
		elif isinstance(scope,SIDL_Object) :
			return self.MatchScope( scope.loc.Scope() )
		elif isinstance(scope,SIDL_Location) :
			return self.MatchScope( scope.Scope() )
		elif isinstance(scope,str) :
			if not self.scope :
				# no specific scope matchs everything !
				return True
			for s in self.scope :
				if fnmatch.fnmatch(scope,s) :
					return True




class SIDL_RecordItem :
	'''
		what			: str				'const'   'var'   'rec'
		id				: str                  x        x       x
		value			: str                  x        x
		type			: str                           x       x
		align			: int                           x       x
		width			: str                           x         
		size			: str                           x       
		given			: str                           x       x
		loc				: SIDL_Location
	'''

	def __init__ ( self, what=None, id=None, align=None, value=None, type=None, dim=None, given=None, loc=None ) :
		if dim:					d = dim.split(',')
		else:					d = ['1','1']
		if len(d)==1:			d.append('1')
		if len(d)>2:			error.SIDL_Error( 'Incorrect dimension of degree %d !'%len(d), loc )
		if align and align<0:	error.SIDL_Error( 'Incorrect alignement of %d !'%align, loc )
		if align and align>8:	error.SIDL_Error( 'The maximum number of bits to be zeroed is limited to 8 !', loc )
		self.what		= what
		self.id			= id
		self.type		= type
		self.value		= value
		self.align		= align
		self.width		= d[0]
		self.size		= d[1]
		self.given		= given
		self.loc		= loc

	def sign ( self ) :
		# some keys do not matter in the signature
		s = [ self.what, self.type, self.width, self.size, self.value, self.align, self.given ]
		return str(s).replace(' ','').replace("'",'').replace('None','')




class SIDL_Record ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		members			: [ SIDL_RecordItem ]
	'''

	def __init__ ( self, uri=None, loc=None, env=None, attributes=None ) :
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.members = []

	def __str__ ( self ) :
		s  = 'SIDL-record %s\n' % (self.uri or '?')
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		if self.members :
			allids = self.CollectMemberIds()
			s += ' | members %s\n' % ', '.join(allids)
		return s

	def C_uri ( self ) :
		return self.uri.replace('.','_')

	def CollectMemberIds ( self ) :
		return [x for x in [m.id for m in self.members] if x]

	def FindMember ( self, id ) :
		for m in self.members :
			if m.id==id :
				return m

	def CanEvaluate ( self, exp ) :
		# checks if an expression could be evaluated by const or var members specified previously in the record.
		# member's id are replaced with the value 0
		e = str( exp )
		for m in self.members :
			if m.id and (m.what=='var' or m.what=='const') :
				e = e.replace( m.id, '0' )
		try:
			# see http://lybniz2.sourceforge.net/safeeval.html
			eval( e, {"__builtins__":None}, None )
			return True
		except (NameError,SyntaxError):
			return False

	def AddConstant ( self, m ) :
		assert m.loc
		assert m.id
		assert atmake.IsDefined(m.value)
		if self.FindMember(m.id) :
			error.SIDL_Error( "Invalid redefinition of the constant id '%s' !"%m.id, m.loc )
		self.members.append( m )

	def AddVariable ( self, m ) :
		assert m.loc
		assert m.id
		assert m.type
		assert m.size
		assert m.width
		assert (m.align is None) or (m.align >= 0)
		# check id
		if self.FindMember(m.id) :
			error.SIDL_Error( "Unauthorised redefinition of the variable id '%s' !"%m.id, m.loc )
		# check given condition could be evaluated
		if m.given and not self.CanEvaluate(m.given) :
			error.SIDL_Error( "Given conditional expression '%s' can't be evaluated !"%m.given, m.loc )
		# check width could be evaluated
		if not self.CanEvaluate(m.width):
			error.SIDL_Error( "Width expression '%s' can't be evaluated !"%m.width, m.loc )
		# check size could be evaluated
		if not self.CanEvaluate(m.size):
			error.SIDL_Error( "Cardinality expression '%s' can't be evaluated !"%m.size, m.loc )
		self.members.append( m )




class SIDL_Tag ( SIDL_Record ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }
		members			: [ SIDL_RecordItem ]

		code			: int
	'''

	def __init__ ( self, uri=None, loc=None, env=None, attributes=None ) :
		SIDL_Record.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.code = None

	def __str__ ( self ) :
		s  = 'SIDL-tag %s\n' % (self.uri or '?')
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		if self.code :
			s += ' | code %d\n' % self.code
		if self.members :
			allids = self.CollectMemberIds()
			s += ' | members %s\n' % ', '.join(allids)
		return s

	def AddRecord ( self, m, content ) :
		assert isinstance(content,SIDL_Content)
		assert m.loc
		assert m.id
		assert m.type
		if self.code is None :
			error.SIDL_Error( "A numerical tag-id must be defined first with the .code directive !", m.loc )
		if self.FindMember(m.id) :
			error.SIDL_Error( "Unauthorised redefinition of the member id '%s' !"%m.id, m.loc )
		rec = content.records.get( m.type )
		if not rec :
			error.SIDL_Error( "Referenced record type '%s' must be declared first !"%m.type, m.loc )
		# records are byte-aligned
		if not m.align:
			m.align = 0
		self.members.append( m )

	def SetTagCode ( self, code, content ) :
		if not isinstance(code,int) or code<=0 :
			error.SIDL_Error( "Tag-code must be greater than 0 !", m.loc )
		self.code = code




class SIDL_Map ( SIDL_Object ) :
	'''
		uri				: str
		env				: SIDL_Env
		loc				: SIDL_Location
		blockin			: SIDL_Location
		blockout		: SIDL_Location
		attributes		: { id:value }

		tags			: [ str ]
	'''

	def __init__ ( self, uri=None, loc=None, env=None, attributes=None ) :
		SIDL_Object.__init__( self, uri=uri, loc=loc, env=env, attributes=attributes )
		self.tags  = []
		self.codes = {}

	def __str__ ( self ) :
		s  = 'SIDL-map %s\n' % (self.uri or '?')
		if self.loc.filename :
			s += ' | file <%s>\n' % self.loc.filename
		if self.loc.lineno :
			s += ' | line <%s>\n' % self.loc.lineno
		if self.blockin and self.blockout :
			s += ' | block %d:%d\n' % (self.blockin.lineno,self.blockout.lineno)
		if self.tags :
			s += ' | tags %s\n' % ', '.join(self.tags)
		return s

	def C_uri ( self ) :
		return self.uri.replace('.','_')

	def AddTag ( self, uri, loc, content ) :
		assert isinstance(content,SIDL_Content)
		tlist = []
		tall  = content.tags.keys()
		for u in atmake.ToList(uri) :
			t = fnmatch.filter(tall,u)
			if not t :
				error.SIDL_Error( r"Tag '%s' is not defined !"%u, loc )
			tlist.extend( t )
		for tname in tlist :
			if not tname in self.tags :
				tobj  = content.tags[ tname ]
				tcode = tobj.code
				tags  = [content.tags[t] for t in self.tags]
				for t in tags :
					if t.code == tcode :
						error.SIDL_Error( r"Tag '%s' (id:%d) conflicts with previously defined tag '%s' (id:%d) !"%(tname,tcode,t.uri,t.code), loc )
				self.tags.append( tname )




class SIDL_Outputlines :
	'''
		lines		: [ str ]
	'''

	def __init__ ( self, lines=None ) :
		self.lines = lines or []

	def IsValid ( self ) :
		if self.lines :
			return True

	def Append ( self, lines, indent=None, **kwargs ) :
		if isinstance(lines,SIDL_Outputlines):	li = lines.ToLines()
		else:									li = atmake.ToList(lines)
		if not indent is None :
			li = [indent+l+'\n' for l in types.UnindentLines(li)]
		for k,v in kwargs.items() :
			kk,vv = '{%s}'%k, str(v)
			li = [l.replace(kk,vv) for l in li]
		self.lines.extend( li )

	def AppendFile ( self, infile, inprefix='' ) :
		raw_lines = open(infile,'r').readlines()
		pfx_lines = [inprefix+l for l in raw_lines]
		self.lines.extend( pfx_lines )

	def Newline ( self, count ) :
		if len(self.lines)>0 :
			last_lines = self.lines[-count*2:]
			last_bytes = ''.join(last_lines)[-(count+1):]
			last_nl    = '\n'*(count+1)
			while not last_bytes.endswith(last_nl) :
				self.lines.append( '\n' )
				last_bytes += '\n'

	def MainSeparator ( self, title ) :
		t = '//==== %s ' % title
		s = t.ljust(100,'=')
		self.Append( s+'\n\n' )

	def SubSeparator ( self, title ) :
		t = '//---- %s ' % title
		s = t.ljust(100,'-')
		self.Append( s+'\n\n' )

	def PostprocessedLines ( self ) :
		slines = ''.join(self.lines).splitlines()
		plines  = [ '%s\n' % l.replace('@lineno@',str(i)) for i,l in enumerate(slines) ]
		return plines

	def ToLines ( self, indent=None ) :
		indent = indent or ''
		return [ indent+l for l in self.PostprocessedLines() ]




class SIDL_Outputnode ( SIDL_Outputlines ) :
	'''
		lines		: [ str ]

		tag			: str
		attributes	: { id:value }
		subnodes	: [ SIDL_Outputnode ]
		multiline	: boolean
	'''

	def __init__ ( self, tag ) :
		SIDL_Outputlines.__init__( self )
		self.tag = tag
		self.attributes = {}
		self.subnodes = []
		self.multiline = False

	def Define ( self, attrname, attrvalue=1 ) :
		assert atmake.IsString(attrname)
		self.attributes[ attrname ] = attrvalue
		return self

	def AddNode ( self, intag ) :
		n = SIDL_Outputnode( intag )
		self.subnodes.append( n )
		return n

	def ToLines ( self, indent=None ) :
		def xml_attr_value( v ) :
			if atmake.IsNumber(v) :	return str(v)
			else :				return '"%s"'%str(v)
		# attributes
		attrs = ''
		if self.attributes:
			attrs = ' %s' % ' '.join(['%s=%s'%(an,xml_attr_value(av)) for an,av in self.attributes.items()])
		# content
		content = []
		if self.subnodes :
			for n in self.subnodes :
				content.extend( n.ToLines() )
		elif self.lines :
			content = self.lines
		content_lines = ''.join(content).splitlines()
		r = []
		if self.multiline or len(content_lines)>1 or self.subnodes :
			r.append( '<%s%s>\n'%(self.tag,attrs) )
			r.extend( ['\t%s\n'%l for l in content_lines] )
			r.append( '</%s>\n'%self.tag )
		elif len(content_lines)==1 :
			r.append( '<%s%s>%s</%s>\n'%(self.tag,attrs,content_lines[0],self.tag) )
		else :
			r.append( '<%s%s />\n'%(self.tag,attrs) )
		if indent:	return [indent+l for l in r]
		else:		return r




class SIDL_Outputfile ( SIDL_Outputlines ) :
	'''
		label		: str
		path		: str
		source		: SIDL_Object
		cleanable	: boolean
		forcewrite	: boolean
		readonly	: boolean
		dummy		: boolean
	'''

	def __init__ ( self, path=None, lines=None, source=None, label=None, cleanable=None, forcewrite=None, readonly=None, dummy=None ) :
		SIDL_Outputlines.__init__( self, lines=lines )
		self.label		= label
		self.path		= path
		self.source		= source
		self.cleanable	= True if cleanable is None else cleanable
		self.forcewrite	= forcewrite
		self.readonly	= True if readonly is None else readonly
		self.dummy		= dummy

	def IsDummy ( self ) :
		if self.dummy:	return True
		else:			return False

	def Basename ( self ) :
		return os.path.basename( self.path )

	def BasenameAsCLabel ( self ) :
		bn = self.Basename()
		for c in ['.',',','-','_',' ','[',']','(',')'] :
			bn = bn.replace(c,'_')
			bn = bn.replace('__',' ')
		return bn

	def CanBeWritten ( self ) :
		opath = self.path
		if opath and fs.IsAbsPath(opath) :
			if fs.IsPathExists(opath):
				if not os.path.isfile(opath):
					return
				if os.path.getsize(opath)>4:
					return
			return True

	def WillBeWritten ( self, lines=None ) :
		wlines = lines or self.PostprocessedLines()
		if self.path and wlines :
			writeit = True
			if fs.IsFilePath(self.path) and not self.forcewrite :
				file_md5  = sign.GetFileContentSign( self.path )
				lines_md5 = sign.GetLinesSign( wlines )
				if file_md5==lines_md5 :
					writeit = False
			return writeit

	def Writeback ( self, content, verbose ) :
		assert isinstance(content,SIDL_Content)
		if self.path and not self.dummy :
			written = False
			if self.cleanable or self.CanBeWritten() :
				wlines = self.PostprocessedLines()
				if self.WillBeWritten(lines=wlines) :
					# print "writing file %s ..." % self.path
					try:
						fs.CreateFile( self.path, rec=True )
						open(self.path,'w').writelines(wlines)
						if self.readonly :
							fs.SetFileReadOnly( self.path )
						written = True
					except:
						pass
			# always touch shelf informations
			content.ShelfOutfile( filename=self.path, source=self.source, cleanable=self.cleanable )
			# verbose ?
			if written and verbose:
				if   self.label:	o_label = ' %s:' % self.label
				elif self.source:	o_label = " %s '%s':" % (self.source.__class__.__name__[5:],self.source.uri)
				else:				o_label = ''
				print("sidl:%s exports [%s]." % (o_label,self.path))
			return written




class SIDL_Content :
	'''
		env					: SIDL_Env
		shelf				: shelve dictionnary
		shelffile			: shelve filename
		sidlfiles			: [ (str,int) ] as (pathname,level)
		comments			: [ SIDL_RawText ]
		docs				: [ SIDL_RawText ]
		books				: [ SIDL_RawText ]
		actions				: { SIDL_RawText }
		interfaces			: { id : SIDL_Interface }
		abstracts			: { id : SIDL_Abstract }
		implementations		: { id : SIDL_Implementation }
		uids				: { id : SIDL_Uid }
		stringids			: { id : SIDL_StringId }
		stringenums			: { id : SIDL_StringEnum }
		forcerefs			: [ str ]
		groups				: { id : SIDL_Group }
		projects			: { id : SIDL_Project }
		solutions			: { id : SIDL_Solution }
		tests				: { id : SIDL_Test }
		modules				: { str : [SIDL_Module] }
		records				: { id : SIDL_Record }
		tags				: { id : SIDL_Tag }
		maps				: { id : SIDL_Map }
		buildenvs			: { id : SIDL_BuildEnv }
		buildsteps			: { id : SIDL_BuildSteps }
		buildsetups			: { id : SIDL_BuildSetup }
		builds				: [ SIDL_Build ]
		regions				: [ (str-label/None,loc) ]		# if r[0] is None => close region tag
		switchtemplates		: { (SIDL_Function,prefix) }
	'''

	def __init__ ( self, env, shelffile=None ) :
		self.env				= env
		self.shelf				= None
		self.shelffile			= None
		self.comments			= []
		self.docs				= []
		self.books				= []
		self.uids				= {}
		self.stringids			= {}
		self.stringenums		= {}
		self.groups				= {}
		self.projects			= {}
		self.solutions			= {}
		self.actions			= {}
		self.tests				= {}
		self.interfaces			= {}
		self.abstracts			= {}
		self.implementations	= {}
		self.switchtemplates	= {}
		self.forcerefs			= []
		self.modules			= {}
		self.regions			= []
		self.records			= {}
		self.tags				= {}
		self.buildenvs			= {}
		self.buildsteps			= {}
		self.buildsetups			= {}
		self.builds				= []
		self.maps				= {}
		if shelffile :
			self.shelf = shelve.open( shelffile, writeback=True )
			if not self.HasShelf() :
				error.SIDL_Error( r'Shelve file [%s] has failed !'%shelffile )
			else :
				self.shelffile = shelffile

	def __str__ ( self ) :
		s =  ''
		for o in list(self.interfaces.values())+list(self.abstracts.values())+list(self.implementations.values()) :
			s += str(o)
			s += '\n'
		for o in list(self.tests.values())+list(self.records.values())+list(self.tags.values())+list(self.maps.values()) :
			s += str(o)
			s += '\n'
		for o in list(self.groups.values())+list(self.projects.values())+list(self.solutions.values()) :
			s += str(o)
			s += '\n'
		for o in list(self.buildenvs.values())+list(self.buildsteps.values())+list(self.buildsetups.values()) :
			s += str(o)
			s += '\n'
		for o in list(self.uids.values())+list(self.stringids.values())+list(self.stringenums.values()) :
			s += str(o)
			s += '\n'
		for ml in self.modules.values() :
			for mo in ml :
				s += str(mo)
				s += '\n'
		return s

	def GetPlatform ( self ) :
		return self.env.SubstVar('__PLATFORM__')

	def IsVerbose ( self ) :
		if self.env.SubstVar('__VERBOSE__'):	return True
		else:									return False

	def GetAllObjects ( self ) :
		r  = self.comments + self.docs + self.books
		r += list(self.groups.values()) + list(self.projects.values()) + list(self.solutions.values()) + list(self.actions.values())
		r += list(self.records.values()) + list(self.tags.values()) + list(self.maps.values())
		r += list(self.interfaces.values()) + list(self.abstracts.values()) + list(self.implementations.values())
		r += list(self.uids.values()) + list(self.stringids.values()) + list(self.stringenums.values()) + list(self.tests.values()) + list(self.switchtemplates.values())
		return r

	def FindByURI ( self, uri ) :
		allobjects = self.GetAllObjects()
		res = []
		full_uri = ('.' in uri)
		for o in allobjects :
			try:
				if full_uri :
					if o.uri==uri :
						res.append( o )
				else :
					base_uri = o.uri.split('.')[-1]
					if base_uri==uri :
						res.append( o )
			except :
				pass
		return res


	#-- interface & abstract & implementations & ...

	def CreateInterface( self, cname, loc=None, env=None, fromfile=None, supername=None, attributes=None ) :
		if cname in self.interfaces :
			error.SIDL_Error( "Interface '%s' is already defined here [%s] !"%(cname,self.interfaces[cname].loc.Here()), loc )
		croot = env.SubstVar('iclassroot')
		if fromfile or cname==croot :
			super = None
		else :
			supername = supername or croot
			super = self.interfaces.get( supername, None )
			if not super :
				error.SIDL_Error( "Super interface '%s' is not defined !"%supername, loc )
		i = SIDL_Interface( cname, loc=loc, env=env, fromfile=fromfile, super=super, attributes=attributes )
		self.interfaces[ cname ] = i
		return i

	def CreateAbstract( self, cname, loc=None, env=None, supername=None, connames=None, attributes=None ) :
		if cname in self.abstracts :
			error.SIDL_Error( "Abstract '%s' is already defined here [%s] !"%(cname,self.abstracts[cname].loc.Here()), loc )
		concerns=[]
		if connames :
			for cn in connames :
				c = self.interfaces.get( cn, None )
				if not c :
					error.SIDL_Error( "Concern interface '%s' is not defined !"%cn, loc )
				concerns.append( c )
		a = SIDL_Abstract( cname, loc=loc, env=env, supername=supername, concerns=concerns, attributes=attributes )
		self.abstracts[ cname ] = a
		return a

	def CreateImplementation( self, cname, loc=None, env=None, qualifier=None, supername=None, abstractname=None, connames=None, attributes=None ) :
		if cname in self.implementations :
			error.SIDL_Error( "Implementation '%s' is already defined here [%s] !"%(cname,self.implementations[cname].loc.Here()), loc )
		abstract = None
		if abstractname :
			abstract = self.abstracts.get( abstractname, None )
			if not abstract :
				error.SIDL_Error( "Abstract '%s' is not defined !"%abstractname, loc )
		concerns=[]
		if connames :
			for cn in connames :
				c = self.interfaces.get( cn, None )
				if not c :
					error.SIDL_Error( "Concern interface '%s' is not defined !"%cn, loc )
				concerns.append( c )
		i = SIDL_Implementation( cname, loc=loc, env=env, qualifier=qualifier, supername=supername, abstract=abstract, concerns=concerns, attributes=attributes )
		self.implementations[ cname ] = i
		return i


	#-- uid/alias & stringids & stringenums

	def CreateUID ( self, id, loc=None, env=None, attributes=None, alias=None ) :
		assert( id )
		if alias :
			# already defined ?
			if id in self.uids :
				ref = self.uids[id]
				assert isinstance(ref,SIDL_Uid)
				nullalias = self.SubstModuleAttribute('gencode','nullalias',ref)
				if ref.alias and ref.alias != nullalias :
					where = ref.loc.Here()
					error.SIDL_Error( "Alias '%s' is already defined here: %s !"%(id,where), loc )
			if (not alias in self.uids) and (not alias in self.implementations) :
				error.SIDL_Error( "Alias reference '%s' is not defined !"%alias, loc )
		uid = SIDL_Uid( id=id, loc=loc, env=env, attributes=attributes, alias=alias )
		self.uids[id] = uid
		return uid
	
	def CreateStringId ( self, name, string, private=None, loc=None, env=None, attributes=None ) :
		assert( name )
		assert( string )
		if name in self.stringids :
			si = self.stringids[name]
			if si.string==string :
				si.private = private
				return si
			else :
				where = self.stringids[name].loc.Here()
				error.SIDL_Error( "String-id '%s' is already defined here: %s !"%(name,where), loc )
		else :
			si = SIDL_StringId( name=name, string=string, private=private, loc=loc, env=env, attributes=attributes )
			self.stringids[name] = si
			return si

	def CreateStringEnum ( self, name, string=None, extends=None, concerns=None, loc=None, env=None, attributes=None ) :
		assert( name )
		if not extends and not concerns :
			error.SIDL_Error( "String-enum '%s' must concern one or more string-ids !"%name, loc )
		if name in self.stringenums :
			where = self.stringenums[name].loc.Here()
			error.SIDL_Error( "String-enum '%s' is already defined here: %s !"%(name,where), loc )
		members = []
		# registration of extensions
		for e in atmake.ToList(extends) :
			if not e in self.stringenums :
				error.SIDL_Error( "String-enum '%s' is not defined but referenced !"%e, loc )
			for m in self.stringenums[e].members :
				if not m in members :
					members.append(m)
		# registration of concerns
		for m in atmake.ToList(concerns) :
			if not m in self.stringids :
				self.CreateStringId( name=m, string=m, private=True, loc=loc, env=env )
			if not m in members :
				members.append(m)
		# self registration of string-enum
		if not name in self.stringids :
			self.CreateStringId( name=name, string=(string or name), private=True, loc=loc, env=env )
		se = SIDL_StringEnum( name=name, members=members, loc=loc, env=env, attributes=attributes )
		self.stringenums[name] = se
		return se


	#-- buildenv / buildstep / buildsetup
		
	def CreateBuildEnv ( self, uri, loc=None, env=None, config=None, attributes=None ) :
		assert( uri )
		if uri in self.buildenvs :
			error.SIDL_Error( "BuildEnv '%s' is already defined here [%s] !"%(uri,self.buildenvs[uri].loc.Here()), loc )
		if uri in self.buildsteps :
			error.SIDL_Error( "Uri '%s' is already defined here [%s] !"%(uri,self.buildsteps[uri].loc.Here()), loc )
		be = SIDL_BuildEnv( uri=uri, loc=loc, env=env, config=config, attributes=attributes )
		self.buildenvs[ uri ] = be
		return be
		
	def CreateBuildStep ( self, uri, loc=None, env=None, inputs=None, outputs=None, qualifier=None, attributes=None ) :
		assert( uri )
		if uri in self.buildsteps :
			error.SIDL_Error( "BuildStep '%s' is already defined here [%s] !"%(uri,self.buildsteps[uri].loc.Here()), loc )
		if uri in self.buildenvs :
			error.SIDL_Error( "Uri '%s' is already defined here [%s] !"%(uri,self.buildenvs[uri].loc.Here()), loc )
		bs = SIDL_BuildStep( uri=uri, loc=loc, env=env, inputs=inputs, outputs=outputs, attributes=attributes )
		bs.qualifier = qualifier
		self.buildsteps[ uri ] = bs
		return bs
		
	def CreateBuildSetup ( self, uri, loc=None, env=None, attributes=None ) :
		assert( uri )
		if uri in self.buildsetups :
			error.SIDL_Error( "BuildSetup '%s' is already defined here [%s] !"%(uri,self.buildsetups[uri].loc.Here()), loc )
		bt = SIDL_BuildSetup( uri=uri, loc=loc, env=env, attributes=attributes )
		self.buildsetups[ uri ] = bt
		return bt
	
	def CreateBuild ( self, loc=None, env=None, target=None, tree=None, solution=None, attributes=None ) :
		assert(target)
		assert(tree)
		if tree not in self.buildsetups :
			error.SIDL_Error( "Buildsetup '%s' is not declared !"%(tree), loc )
		b = SIDL_Build( loc=loc, env=env, target=target, solution=solution, tree=tree, attributes=attributes )
		self.builds.append( b )
		return b


	#-- switchtemplate

	def AddSwitchTemplate ( self, uri, func ) :
		if uri and func :
			self.switchtemplates[uri] = func


	#-- test

	def CreateTest( self, uri, loc=None, env=None, attributes=None ) :
		if uri in self.tests :
			error.SIDL_Error( "Test '%s' is already defined here [%s] !"%(uri,self.tests[uri].loc.Here()), loc )
		t = SIDL_Test( uri=uri, loc=loc, env=env, attributes=attributes )
		self.tests[ uri ] = t
		return t


	#-- group / project / solution

	def CreateGroup( self, uri=None, folders=None, loc=None, env=None, attributes=None ) :
		if uri in self.groups :
			error.SIDL_Error( "Group '%s' is already defined here [%s] !"%(uri,self.groups[uri].loc.Here()), loc )
		g = SIDL_Group( uri=uri, folders=folders, loc=loc, env=env, attributes=attributes )
		self.groups[ uri ] = g
		return g

	def CreateProject( self, uri=None, groups=None, loc=None, env=None, attributes=None ) :
		if uri in self.projects :
			error.SIDL_Error( "Project '%s' is already defined here [%s] !"%(uri,self.projects[uri].loc.Here()), loc )
		p = SIDL_Project( uri=uri, groups=groups, loc=loc, env=env, attributes=attributes )
		self.projects[ uri ] = p
		return p

	def CreateSolution( self, uri=None, projects=None, dependencies=None, loc=None, env=None, attributes=None ) :
		if uri in self.solutions :
			error.SIDL_Error( "Solution '%s' is already defined here [%s] !"%(uri,self.solutions[uri].loc.Here()), loc )
		s = SIDL_Solution( uri=uri, projects=projects, dependencies=dependencies, loc=loc, env=env, attributes=attributes )
		self.solutions[ uri ] = s
		return s


	#-- record / tag / map

	def CreateRecord( self, uri=None, loc=None, env=None, attributes=None ) :
		if uri in self.records :
			error.SIDL_Error( "Record '%s' is already defined here [%s] !"%(uri,self.records[uri].loc.Here()), loc )
		r = SIDL_Record( uri=uri, loc=loc, env=env, attributes=attributes )
		self.records[uri] = r
		return r

	def CreateTag( self, uri=None, loc=None, env=None, attributes=None ) :
		if uri in self.tags :
			error.SIDL_Error( "Tag '%s' is already defined here [%s] !"%(uri,self.tags[uri].loc.Here()), loc )
		t = SIDL_Tag( uri=uri, loc=loc, env=env, attributes=attributes )
		self.tags[uri] = t
		return t

	def CreateMap( self, uri=None, loc=None, env=None, attributes=None ) :
		if uri in self.maps :
			error.SIDL_Error( "Map '%s' is already defined here [%s] !"%(uri,self.maps[uri].loc.Here()), loc )
		m = SIDL_Map( uri=uri, loc=loc, env=env, attributes=attributes )
		self.maps[uri] = m
		return m


	#-- comment / doc / book / action
	
	def AddComment ( self, uri=None, attributes=None, raw=None ) :
		if raw :
			if uri and self.FindByURI(uri) :
				error.SIDL_Error( "URI '%s' is already in use !"%uri )
			raw.uri = uri
			raw.SetAttributes( attributes )
			self.comments.append( raw )
	
	def AddDoc ( self, uri=None, attributes=None, raw=None ) :
		if raw :
			if uri and self.FindByURI(uri) :
				error.SIDL_Error( "URI '%s' is already in use !"%uri )
			raw.uri = uri
			raw.SetAttributes( attributes )
			self.docs.append( raw )
	
	def AddBook ( self, uri=None, attributes=None, raw=None ) :
		if raw :
			if uri and self.FindByURI(uri) :
				error.SIDL_Error( "URI '%s' is already in use !"%uri )
			raw.uri = uri
			raw.SetAttributes( attributes )
			self.books.append( raw )

	def AddAction ( self, uri=None, attributes=None, raw=None ) :
		if raw :
			if uri and self.FindByURI(uri) :
				error.SIDL_Error( "URI '%s' is already in use !"%uri )
			raw.uri = uri
			raw.SetAttributes( attributes )
			self.actions[uri] = raw


	#-- region

	def OpenRegion( self, label=None, loc=None, env=None ) :
		self.regions.append( (label or '', loc) )

	def CloseRegion( self, loc=None, env=None ) :
		self.regions.append( (None, loc) )


	# -- module

	def CreateModule ( self, name, loc=None, env=None, attributes=None ) :
		m = SIDL_Module( name=name, loc=loc, env=env, attributes=attributes )
		r = self.modules.setdefault( name, [] )
		r.append( m )
		return m

	def SelectModuleDefinitions ( self, name, withattribute=None, withscope=None ) :
		defs = self.modules.get( name, [] )
		if withattribute :
			defs = [x for x in defs if x.HasAttribute(withattribute)]
		if withscope :
			defs = [x for x in defs if x.MatchScope(withscope)]
		return defs

	def GetModuleAttribute ( self, name, attrname, obj ) :
		# look in the object attributes
		if obj and isinstance(obj,SIDL_Object) and obj.HasAttribute(attrname) :
			return obj.GetAttribute(attrname)
		# look in a named module definition
		defs = self.SelectModuleDefinitions( name, attrname, withscope=obj )
		if defs :
			# retrieve the last value of the attribute !
			defs.reverse()
			for d in defs :
				if d.HasAttribute(attrname) :
					return d.GetAttribute(attrname)

	def SubstModuleAttribute ( self, name, attrname, obj ) :
		attrvalue = self.GetModuleAttribute( name, attrname, obj )
		if attrvalue :
			return obj.env.Subst( attrvalue )


	# -- shelf

	def ShelfFile ( self ) :
		return self.shelffile


	def HasShelf ( self ) :
		return atmake.IsDefined(self.shelf)


	def ShelfAccess ( self, uri, defvalue=None ) :
		if self.HasShelf() :
			u = uri.split('.')[:-1]
			k = uri.split('.')[-1]
			d = self.shelf
			for p in u :
				if p not in d: d[p]={}
				d = d[p]
			if k not in d :
				d[k]=defvalue
			return d[k]


	def ShelfSync ( self ) :
		if self.HasShelf() :
			self.shelf.sync()


	def ShelfClose ( self ) :
		if self.HasShelf() :
			self.shelf.close()
			self.shelf = None
			self.shelffile = None


	def ShelfOutfile ( self, filename, source=None, cleanable=True ) :
		def valid_name(inname,defname) :
			if inname:	return inname.replace(' ','_')
			else:		return defname
		ofiles = self.ShelfAccess( 'outfiles', {} )
		if atmake.IsDefined(ofiles) :
			oinfo = {}
			oinfo['path'] = fs.Path(filename)
			oinfo['cleanable'] = cleanable
			# outputs locations
			loc, locin, locout = [None]*3
			if isinstance(source,SIDL_Object) and source.blockin and source.blockout :
				loc, locin, locout = source.loc, source.blockin, source.blockout
			elif isinstance(source,SIDL_Object) :
				loc, locin, locout = source.loc, source.loc, source.loc
			elif isinstance(source,SIDL_Location) :
				loc, locin, locout = source, source, source
			if loc and locin and locout :
				oinfo['sidlfile'] = '%d %d %s' % (loc.lineno, locout.lineno, fs.Path(loc.filename))
			# outputs object informations
			if isinstance(source,SIDL_Object) :
				if 1:					oinfo['uri'] = str(source.uri)
				if 1:					oinfo['sidltype'] = source.__class__.__name__[5:]
			if isinstance(source,SIDL_CodeObject) :
				if source.loc.package:	oinfo['package']   = source.loc.package
				for k,v in source.sections.items() :
					if v:				oinfo['section_%s'%str(k)] = ' '.join([ '%s %d'%(valid_name(r.GetAttribute('name'),'noname'),r.loc.lineno) for r in v ])
			if isinstance(source,SIDL_Interface) :
				if 1:					oinfo['outpath']   = source.GetOutpath()
				if source.classname:	oinfo['classname'] = source.classname
				if source.supername:	oinfo['supername'] = source.supername
				if source.functions:	oinfo['functions'] = ' '.join([ '%s %d'%(f.name,f.loc.lineno) for f in source.functions ])
			if isinstance(source,SIDL_Abstract) :
				if source.classname:	oinfo['classname'] = source.classname
				if source.supername:	oinfo['supername'] = source.supername
				if source.concerns:		oinfo['concerns']  = ' '.join([ c.classname for c in source.concerns ])
				stub = source.GetStubOutpath()
				if stub:				oinfo['stub'] = fs.Path(stub)
			if isinstance(source,SIDL_Implementation) :
				if 1:					oinfo['outpath']   = source.GetOutpath()
				if source.abstract:		oinfo['abstract']  = source.abstract.classname
			# stores in shelf
			ofiles[ oinfo['path'] ] = oinfo


	def CollectProjectFiles ( self, proj_uri ) :
		files = {}
		p = self.projects.get( proj_uri, None )
		if p :
			for g_uri in p.groups :
				g = self.groups.get( g_uri, None )
				if g :
					for fo in g.folders :
						for f in fo[1] :
							files[ fs.Path(f) ] = 1
		return list(files.keys())


	def Bin2H( self, tofile=None, fromfile=None, loc=None, attributes=None ) :
		arrayname = None
		tofile = tofile or '%s.h' % fromfile
		if attributes and ('rename' in attributes) :
			rename = attributes['rename']
			p1, p2 = os.path.split( fromfile )
			tofile = os.path.join( p1, rename )
		if attributes and ('tofile' in attributes) :
			tofile = attributes['tofile']
		if attributes and ('array' in attributes) :
			arrayname = attributes['array']
		done = bin2h.UpdateConvert( tofile, fromfile, arrayname=arrayname )
		if done :
			self.ShelfOutfile( filename=tofile, source=loc )
			return fs.Path(tofile)


	def Shell( self, action, args=None, loc=None ) :
		def _arg(n):
			for a in args:
				if a[0]==n:
					return a[1]
		def _check_args(req,opt):
			al = [ a[0] for a in args ]
			rl = req.split()
			ol = opt.split()
			for r in rl :
				if not r in al:
					error.SIDL_Error( "The argument '%s' is required for shell action '%s' !"%(r,action), loc=loc )
			for a in al :
				if not (a in rl) and not (a in ol) :
					error.SIDL_Error( "Invalid argument '%s' for shell action '%s' !"%(a,action), loc=loc )
			return True

		if action=='cpfile':
			_check_args('ofile ifile', '')
			fs.CpFile( _arg('ifile'), _arg('ofile') )
		elif action=='rmfile':
			_check_args('file', '')
			fs.RmFile( _arg('file') )
		elif action=='rmfiles':
			_check_args('path', '')
			fs.RmFiles( _arg('path') )
		elif action=='cpfiles':
			_check_args('ipath opath', '')
			files = glob.glob( _arg('ipath') )
			for inf in files :
				of = '%s/%s' % (_arg('opath'),os.path.basename(inf))
				fs.CpFile( inf, of )
		elif action=='mkdir':
			_check_args('path', '')
			fs.CreateDir( _arg('path'), rec=True )



