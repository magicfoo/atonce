
import sys, os, inspect, re, glob
import fnmatch
import ply.lex as lex
import ply.yacc as yacc
from ply.lex import TOKEN, LexError
import atmake
import atmake.tools.fs as fs
import atmake.tools.algo as algo
from .genuid import *
from .components import *
from .error import *

PLY_DEBUG = 0		# 1 to generate parser.out






class PLY_Lexer:
	'''
		module
		lexer
		input_stack, stack of lexers
	'''

	def __init__ ( self, module=None, debug=0, optimize=0, lextab=None ) :
		self.module			= module
		self.lexer			= lex.lex( module=self.module, debug=debug, optimize=optimize, lextab=lextab )
		self.input_stack	= []

	def findinput ( self, file=None ) :
		level = 0
		for l in self.input_stack :
			if file and l.file==file :
				return ( level, l )
			level += 1

	def pushinput ( self, text=None, file=None, env=None ) :
		if not text :
			return
		env = env or {}
		# prevent cycling include !
		if self.findinput(file=file) :
			return False
		# create new lexer for this input
		l = self.lexer.clone()
		l.input( text )
		l.file = file
		l.env = {}
		# inherits from parent lexer's env
		p = self.curinput()
		if p and p.env :
			l.env.update( p.env )
		if env :
			l.env.update( env )
		l.env['file'] = l.file
		self.input_stack.append( l )
		return True

	def input ( self, text=None ) :
		self.pushinput( text=text )

	def curinput ( self ) :
		try:		return self.input_stack[-1]
		except:		return None

	def exitcur ( self ) :
		if len(self.input_stack)>0 :
			self.input_stack.pop()

	def exitall ( self ) :
		self.input_stack = []

	@property
	def lineno ( self ) :
		l = self.curinput()
		if l:	return l.lineno
		else:	return 1

	@property
	def lexpos ( self ) :
		l = self.curinput()
		if l:	return l.lexpos
		else:	return 0

	@property
	def file ( self ) :
		l = self.curinput()
		if l:	return l.file
		else:	return None

	@property
	def env ( self ) :
		l = self.curinput()
		if l:	return l.env
		else:	return None

	def token ( self ) :
		while True :
			l = self.curinput()
			if not l:	return
			tok = l.token()
			if tok:	return tok
			else:	self.input_stack.pop()






class PLY_ManagedParser:
	'''
		mark
		lexer
		parser
	'''

	def __init__( self ):
		self.mark		= None
		self.lexer		= None
		self.parser		= None

	def manage ( self ) :

		def fwrapper ( mth, doc=None ) :
			def f( p ) :
				return mth( p, self.mark )
			f.__name__ = mth.__name__
			f.__doc__  = doc or mth.__doc__
			return f

		def mwrapper ( i ) :
			# generate		def mi( self, p ) :
			#					'mi: '
			#					...
			def f ( p ) :
				self.mark = i
				p[0]      = i
			f.__name__ = 'p_wrapper_m%d'%i
			f.__doc__  = 'm%d :'%i
			return f

		if self.parser :
			return

		rule_re = re.compile('^\s*(.+empty|.+m(?P<mi>\d+))?\s*$')
		mis = {}

		# check for all parsing methods 'p_xxx'
		for dname in dir(self) :
			dvalue = getattr( self, dname )
			if atmake.IsMethod(dvalue) and dname.startswith('p_') :
				# check managed p_ method with signature like (self,p,m)
				dargs = inspect.getargspec(dvalue)[0]
				if dargs==['self','p','m'] :
					# append ' empty' symbol to all rules not ending with a symbol 'mi'
					# this prevent mark to be undefined (i.e. empty symbol clear the mark !)
					erules = []
					for l in dvalue.__doc__.splitlines() :
						r = l.rstrip()
						m = rule_re.match(r)
						if m :
							mi = m.group('mi')
							if mi :
								mis[ int(mi) ] = 1
						else :
							r += ' empty'
						erules.append( r )
					nrules = '\n'.join(erules)
					setattr( self, dname, fwrapper(dvalue,nrules) )

		# auto generate p_mi rules ...
		for i in mis.keys() :
			mi = mwrapper( i )
			setattr( self, mi.__name__, mi )

		if PLY_DEBUG:
			self.lexer  = PLY_Lexer( module=self, debug=1, optimize=0, lextab=None )
			self.parser = yacc.yacc( module=self, debug=1, write_tables=0, picklefile=None )
		else:
			self.lexer  = PLY_Lexer( module=self, debug=0, optimize=1, lextab=None )
			self.parser = yacc.yacc( module=self, debug=0, optimize=1, write_tables=0, picklefile=None, errorlog=yacc.NullLogger() )

	def argloc ( self, p=None, argno=None ) :
		if p and argno :
			fline	= p.lineno(argno)
			fname	= p.file(argno)
			package	= self.lexer.env.get('package')
			return SIDL_Location( f=fname, l=fline, package=package )
		else :
			return SIDL_Location()

	def p_empty ( self, p ) :
		'empty : '
		self.mark = None
		p[0] = None

	def p_error( self, t ) :
		if type(t)==lex.LexToken :
			print("sidl: %s[%s]: Syntax error at '%s' !" % (t.lexer.file, t.lexer.lineno, t.value))
		else :
				print("sidl: Syntax error at EOF")






class SIDL_Parser( PLY_ManagedParser ) :
	'''
		mark
		lexer
		parser

		env_stack	: [ SIDL_Env ]
		content		: SIDL_Content
	'''


	states = (
		('raw',		'exclusive'),
		('skip',	'exclusive'),
	)

	controls = {
		# directly managed by the lexer !
		'@include'			:	'INCLUDE',
		'@if'				:	'IF',
		'@ifnot'			:	'IFNOT',
		'@else'				:	'ELSE',
		'@elif'				:	'ELIF',
		'@elifnot'			:	'ELIFNOT',
		'@endif'			:	'ENDIF',
		'@exit'				:	'EXIT',
		'@eof'				:	'EOF',
	}

	commands = {
		'@nop'				:	'NOP',
		'@set'				:	'SET',
		'@unset'			:	'UNSET',
		'@append'			:	'APPEND',
		'@prepend'			:	'PREPEND',
		'@push'				:	'PUSH',
		'@pop'				:	'POP',
		'@save'				:	'SAVE',
		'@restore'			:	'RESTORE',
		'@outpath'			:	'OUTPATH',
		'@inpath'			:	'INPATH',
		'@iopath'			:	'IOPATH',
		'@package'			:	'PACKAGE',
		'@interface'		:	'INTERFACE',
		'@abstract'			:	'ABSTRACT',
		'@implementation'	:	'IMPLEMENTATION',
		'@test'				:	'TEST',
		'@using'			:	'USING',
		'@forward'			:	'FORWARD',
		'@print'			:	'PRINT',
		'@subst'			:	'SUBST',
		'@eval'				:	'EVAL',
		'@pragma'			:	'PRAGMA',
		'@exec'				:	'EXEC',
		'@comment'			:	'COMMENT',
		'@doc'				:	'DOC',
		'@book'				:	'BOOK',
		'@action'			:	'ACTION',
		'@group'			:	'GROUP',
		'@project'			:	'PROJECT',
		'@program'			:	'PROGRAM',
		'@archive'			:	'ARCHIVE',
		'@dll'				:	'DLL',
		'@solution'			:	'SOLUTION',
		'@file'				:	'FILE',
		'@dir'				:	'DIR',
		'@module'			:	'MODULE',
		'@region'			:	'REGION',
		'@endregion'		:	'ENDREGION',
		'@bin2h'			:	'BIN2H',
		'@shell'			:	'SHELL',
		'@record'			:	'RECORD',
		'@tag'				:	'TAG',
		'@map'				:	'MAP',
		'@uid'				:	'UID',
		'@stringid'			:	'STRINGID',
		'@stringenum'		:	'STRINGENUM',
		'@folder'			:	'FOLDER',
		'@section'			:	'SECTION',
		'@for'				:	'FOR',
		'@func'				:	'FUNC',
		'@switch'			:	'SWITCH',
		'@enum'				:	'ENUM',
		'@struct'			:	'STRUCT',
		'@union'			:	'UNION',
		'@classid'			:	'CLASSID',
		'@getclassid'		:	'GETCLASSID',
		'@cdtor'			:	'CDTOR',
		'@type'				:	'TYPE',
		'@delegate'			:	'DELEGATE',
		'@buildenv'			:	'BUILDENV',
		'@buildstep'		:	'BUILDSTEP',
		'@buildsetup'		:	'BUILDSETUP',
		'@build'			:	'BUILD'
	}

	directives = {
		'.const'			:	'D_CONST',
		'.ui'				:	'D_UI',
		'.si'				:	'D_SI',
		'.fp'				:	'D_FP',
		'.fx'				:	'D_FX',
		'.code'				:	'D_CODE',
		'.record'			:	'D_RECORD',
		'.state'			:	'D_STATE',
	}

	reserved = {
		'from'				:	'FROM',
		'finalizes'			:	'FINALIZES',
		'extends'			:	'EXTENDS',
		'concerns'			:	'CONCERNS',
		'alias'				:	'ALIAS',
		'as'				:	'AS',
		'defines'			:	'DEFINES',
		'writes'			:	'WRITES',
		'targets'			:	'TARGETS',
		'adds'				:	'ADDS',
		'interns'			:	'INTERNS',
		'excludedfrombuild'	:	'EXCLUDEDFROMBUILD',
		'ifexisting'		:	'IFEXISTING',
		'asarray'			:	'ASARRAY',
		'prebuild'			:	'PREBUILD',
		'postbuild'			:	'POSTBUILD',
		'with'				:	'WITH',
		'static'			:	'STATIC',
		'inline'			:	'INLINE',
		'virtual'			:	'VIRTUAL',
		'template'			:	'TEMPLATE',
		'levels'			:	'LEVELS',
		'requires'			:	'REQUIRES',
		'and'				:	'AND',
		'or'				:	'OR',
		'given'				:	'GIVEN',
		'unary'				:	'UNARY',
		'multiple'			:	'MULTIPLE',
		'outputs'			:	'OUTPUTS',
		'inputs'			:	'INPUTS',
		'using'				:	'USING',
		'targets'			:	'TARGETS'
	}

	tokens = [
		'COLON',
		'LBRACE', 'RBRACE', 'EQ',
		'ICONST', 'FCONST', 'SCONST', 'PCONST', 'BCONST',
		'ID', 'URI', 'MRI',
		'VAR', 'RAW',
		'NOP'
	] + list(commands.values()) + list(directives.values()) + list(reserved.values())


	t_COLON			= r':'
	t_LBRACE		= r'\{'
	t_RBRACE		= r'\}'
	t_EQ			= r'='
#	t_DOT			= r'\.'
#	t_LANGLE		= r'\<'
#	t_RANGLE		= r'\>'
#	t_STAR			= r'\*'
#	t_QUEST			= r'\?'
#	t_SCOPE			= r'::'
#	t_EXCLAM		= r'\!'
#	t_AMPER			= r'&'
#	t_DOLLAR		= r'$'
#	t_SHARP			= r'#'
#	t_LPAREN		= r'\('
#	t_RPAREN		= r'\)'
#	t_LBRACK		= r'\['
#	t_RBRACK		= r'\]'
#	t_COMMA			= r','
#	t_SLASH			= r'\/'
#	t_BSLASH		= r'\\'
#	t_SEMI			= r';'
#	t_PLUS			= r'\+'
#	t_MINUS			= r'-'
#	t_EXP			= r'\*\*'
#	t_ARROW			= r'->'
#	t_ELLIPSIS		= r'\.\.\.'


	start			= 'sidl_body'

	re_id			= r'[A-Za-z_][\w_]*'
	re_uri			= r'%s(\.%s)*' % (re_id,re_id)
	re_sconst1		= r'\"([^\\\n]|(\\.))*?\"'
	re_sconst2		= r'\'([^\\\n]|(\\.))*?\''
	re_pconst		= r'\<([^\\\n]|(\\.))*?\>'
	re_fileconst	= r'%s|%s|%s' % (re_sconst1,re_sconst2,re_pconst)

	def __init__( self ) :
		PLY_ManagedParser.__init__( self )
		self.manage()

	def Env ( self ) :
		return self.env_stack[-1]

	def Pushenv ( self ) :
		env = self.Env()
		self.env_stack.append( env.Clone() )

	def Popenv ( self ) :
		if len(self.env_stack) > 0 :
			self.env_stack.pop()

	def Subst ( self, s=None, loc=None ) :
		env = self.Env()
		return env.Subst( str(s), loc )

	def SubstLines ( self, lines=None, loc=None ) :
		env = self.Env()
		return env.SubstLines( lines, loc )

	def IsVerbose ( self, loc=None ) :
		verbose = self.Subst( '$__VERBOSE__' )
		if (verbose) :	return True
		else :			return False

	def Eval ( self, s=None, loc=None ) :
		env = self.Env()
		s = self.Subst( str(s), loc )
		return env.Eval( s )

	def Exec ( self, code=None ) :
		env = self.Env()
		env.Exec( code )

	def ParserOption ( self, optname, loc ) :
		if optname and loc :
			return self.content.GetModuleAttribute( 'parser', optname, loc )

	def ControlledText( self, text ) :
		# insert @pass commands at some points in the source text
		# to prevent the ahead token extraction inner to the LALR or SLR methods
		if text :
			assert isinstance(text,str)
			text = '%s @nop' % text
			for k in self.controls.keys() :
				text = text.replace( k, '@nop '+k )
		return text

	def PushLexerInput ( self, text=None, path=None, env=None ) :
		if path and not text :
			try:
				path  = fs.ToAbsPath(path)
				lines = open(path,'r').readlines()
				text  = ''.join(lines)
			except:
				return
		if not text is None :
			ctrl_text = self.ControlledText(text)
			self.lexer.pushinput( text=ctrl_text, file=path, env=env )
			return True

	def Extendpath ( self, p, loc=None ) :
		if not p:					return None
		if fs.IsAbsPath(p):			return p
		if p.startswith('<'):		return os.path.join( self.Subst('$inpath'),  p[1:] )
		if p.startswith('in:'):		return os.path.join( self.Subst('$inpath'),  p[3:] )
		if p.startswith('>'):		return os.path.join( self.Subst('$outpath'), p[1:] )
		if p.startswith('out:'):	return os.path.join( self.Subst('$outpath'), p[4:] )
		if loc and loc.filename:	return os.path.join( os.path.dirname(loc.filename), p )
		return p

	def Checkfile ( self, fn, loc=None ) :
		efn = self.Extendpath( fn, loc )
		if fs.IsFilePath(efn) :
			return fs.ToAbsPath(efn)

	def Checkdir ( self, dn, loc=None ) :
		edn = self.Extendpath( dn, loc )
		if fs.IsDirPath(edn) :
			return fs.ToAbsPath(edn)

	def Findfile ( self, fn ) :
		path = self.Env().GetPath()
		for p in path :
			f = os.path.join( p, fn )
			if fs.IsFilePath(f) :
				return fs.ToAbsPath(f)

	def Finddir ( self, dn ) :
		path = self.Env().GetPath()
		for p in path :
			d = os.path.join( p, dn )
			if fs.IsDirPath(d) :
				return fs.ToAbsPath(d)

	def raw_enter ( self, t ) :
		t.lexer.begin( 'raw' )
		self.raw_left  = t.value
		self.raw_right = { '[':']', '(':')' }[ t.value ]
		self.raw_start = t.lexer.lexpos
		self.raw_level = 1
		self.raw_squote = 0		# ' counter
		self.raw_dquote = 0		# " counter
		#print "enter (%s)(%d)" % (t.lexer.file,t.lexer.lineno)

	def raw_leave ( self, t ) :
		raw_text = t.lexer.lexdata[ self.raw_start : t.lexer.lexpos-1 ]
		raw_lines = raw_text.splitlines()
		t.lexer.begin( 'INITIAL' )
		t.lexer.lineno += raw_text.count('\n')
		t.value = '\n'.join( types.UnindentLines(raw_lines) )
		if t.value.startswith('\n'):
			t.value = t.value[1:]
			t.lineno += 1
		if t.value.endswith('\n'):
			t.value = t.value[:-1]
		t.type = 'RAW'
		#print "leave (%s)(%d)" % (t.lexer.file,t.lexer.lineno)
		return t

	def skip_enter ( self, t ) :
		t.lexer.begin( 'skip' )
		self.skip_start = t.lexer.lexpos
		self.skip_level = 1
		return 'skipped'

	def skip_leave ( self, t ) :
		t.lexer.begin( 'INITIAL' )
		skipped_text = t.lexer.lexdata[ self.skip_start : t.lexer.lexpos+1 ]
		t.lexer.lineno += skipped_text.count('\n')

	def skip_check ( self, t, loc ) :
		env = self.Env()
		# remove the command @if[xx]
		cmd, exp = t.value.split(None,1)
		# remove single line comment
		for sl in r'-- # /* //'.split() :
			exp = exp.split(sl)[0].strip()
		# replace defined() evaluator
		while 1:
			i = exp.split('defined(',1)
			if len(i)==1:	break
			j = i[1].split(')',1)
			if len(j)==1:	break
			k = j[0].strip()
			v = env.SubstVar( k, loc )
			if v:	exp = "%s (1) %s"  % (i[0],j[1])
			else:	exp = "%s (0) %s" % (i[0],j[1])
		try:
			#print t.value, exp, eval(exp)
			cond = self.Eval( exp )
		except (NameError, SyntaxError):
			SIDL_Error( "Invalid expression !", loc )
		if 'not' in cmd:	cond = not (cond)
		if not (cond):		return self.skip_enter(t)
		return 'done'

	def include_file ( self, t ) :
		loc = SIDL_Location( f=t.lexer.file, l=t.lexer.lineno )
		# find file to include
		lastc = t.value[-1]
		if lastc in ['"',"'"] :
			f1 = self.Subst( t.value.split(lastc)[-2], loc )
			f2 = self.Checkfile( f1, loc )
		else :
			f1 = self.Subst( t.value.split('<')[1].split('>')[0], loc )
			f2 = self.Findfile( f2 )
		if not f2:
			SIDL_Error( r'File [%s] not found !'%f1, loc )
		return f2

	def tokNOP ( self, t ) :
		t.value = None
		t.type  = 'NOP'
		return t

	def t_iconst ( self, t ) :
		r'\d+([uU]|[lL]|[uU][lL]|[lL][uU])?'
		t.type = 'ICONST'
		t.value = int( t.value )
		return t

	def t_fconst ( self, t ) :
		r'((\d+)(\.\d+)(e(\+|-)?(\d+))? | (\d+)e(\+|-)?(\d+))([lL]|[fF])?'
		t.type = 'FCONST'
		t.value = float( t.value )
		return t

	@TOKEN( '%s|%s' % (re_sconst1,re_sconst2) )
	def t_sconst ( self, t ) :
		t.type  = 'SCONST'
		t.value = t.value[1:-1]
		return t

	@TOKEN( re_pconst )
	def t_pconst ( self, t ) :
		t.type  = 'PCONST'
		t.value = t.value[1:-1]
		return t

	@TOKEN( r'@include\s+(%s)' % (re_fileconst) )
	def t_include ( self, t ) :
		incf = self.include_file( t )
		self.PushLexerInput( path=incf )
		t.lexer.lineno += t.value.count('\n')
		return self.tokNOP(t)

	def t_eof ( self, t ) :
		r'@eof'
		self.lexer.exitcur()
		return self.tokNOP(t)

	def t_exit ( self, t ) :
		r'@exit'
		self.lexer.exitall()
		return self.tokNOP(t)

	@TOKEN( r'((@if|@elif)(not)?\s+[^\n]*) | @else | @endif' )
	def t_control ( self, t ) :
		loc = SIDL_Location( f=t.lexer.file, l=t.lexer.lineno )
		try    :	len(self.skip_stack)
		except :	self.skip_stack = []
		if t.value.startswith('@if'):
			s = self.skip_check(t,loc)
			self.skip_stack.append( s )
		elif t.value.startswith('@elif'):
			if self.skip_stack[-1]=='done':
				self.skip_enter(t)
			else:
				s = self.skip_check(t,loc)
				self.skip_stack[-1] = s
		elif t.value=='@else':
			if self.skip_stack[-1]=='done':
				self.skip_enter(t)
			else:
				self.skip_stack[-1] = 'done'
		elif t.value=='@endif':
			self.skip_stack.pop()
		return self.tokNOP(t)

	@TOKEN( r'((@if|@elif)(not)?\s+[^\n]*) | @else | @endif' )
	def t_skip_control ( self, t ):
		loc = SIDL_Location( f=t.lexer.file, l=t.lexer.lineno )
		if t.value.startswith('@if'):
			self.skip_level += 1
		elif t.value.startswith('@elif'):
			if self.skip_level==1 and not self.skip_stack[-1]=='done':
				self.skip_leave(t)
				s = self.skip_check(t,loc)
				self.skip_stack[-1] = s
		elif t.value=='@else':
			if self.skip_level==1 and not self.skip_stack[-1]=='done':
				self.skip_leave(t)
		elif t.value=='@endif':
			self.skip_level -= 1
			if self.skip_level==0 :
				self.skip_leave( t )
		return self.tokNOP(t)

	def t_skip_pass ( self, t ) :
		r' \t\r'
		pass

	def t_skip_error( self, t ) :
		t.lexer.skip(1)

	def t_id_uri_mri_reserved ( self, t ) :
		r'([A-Za-z_\*\?][\w_\*\?]*)(\.[A-Za-z_\*\?][\w_\*\?]*)*'
		if t.value in ['True','true','Yes','yes','Vrai','vrai','Oui','oui'] :
			t.value = True
			t.type = 'BCONST'
		elif t.value in ['False','false','No','no','Faux','faux','Non','non'] :
			t.value = False
			t.type = 'BCONST'
		elif '*' in t.value :
			t.type = 'MRI'
		elif '?' in t.value :
			t.type = 'MRI'
		elif '.' in t.value :
			t.type = 'URI'
		else :
			t.type = self.reserved.get( t.value, 'ID' )
		return t

	def t_command ( self, t ) :
		r'@[A-Za-z_][\w_]*'
		t.type = self.commands.get( t.value, None )
		if t.type:	return t
		loc = SIDL_Location( f=t.lexer.file, l=t.lexer.lineno )
		SIDL_Error( r"Invalid command '%s' !"%t.value, loc )

	def t_directive ( self, t ) :
		r'\.[A-Za-z_][\w_]*'
		t.type = self.directives.get( t.value, None )
		if t.type:	return t
		loc = SIDL_Location( f=t.lexer.file, l=t.lexer.lineno )
		SIDL_Error( r"Invalid directive '%s' !"%t.value, loc )

	def t_enter_raw ( self, t ) :
		r'[\[\(]'
		return self.raw_enter( t )

	def t_raw_squote ( self, t ) :
		r'\''
		# not already in "" string ?
		if (self.raw_dquote%2)==0 :
			self.raw_squote += 1

	def t_raw_dquote ( self, t ) :
		r'\"'
		# not already in '' string ?
		if (self.raw_squote%2)==0 :
			self.raw_dquote += 1

	def t_raw_up ( self, t ) :
		r'[\[\(]'
		# not already in a string ?
		if (self.raw_dquote%2)==0 and (self.raw_squote%2)==0 :
			if t.value==self.raw_left:
				self.raw_level += 1

	def t_raw_down ( self, t ) :
		r'[\]\)]'
		# not already in a string ?
		if (self.raw_dquote%2)==0 and (self.raw_squote%2)==0 :
			if t.value==self.raw_right :
				self.raw_level -= 1
				if self.raw_level==0 :
					return self.raw_leave( t )

	def t_raw_pass ( self, t ) :
		r' \t\r'
		pass

	def t_raw_error( self, t ) :
		t.lexer.skip(1)

	def t_var ( self, t ) :
		r'\$[A-Za-z_][\w_]*'
		t.type  = 'VAR'
		t.value = t.value[1:]
		return t

	def t_newline ( self, t ) :
		r'\n+'
		t.lexer.lineno += t.value.count('\n')

	def t_pass ( self, t ) :
		r'\s+'
		pass

	def t_error( self, t ):
		print("sidl: Illegal character '%s' (%d)" % ( t.value[0], t.lexer.lineno ))
		t.lexer.skip(1)

	def t_ml_comment( self, t ) :
		r'/\*(.|\n)*?\*/'
		t.lexer.lineno += t.value.count('\n')

	def t_sl_comment( self, t ) :
		r'(//|\#|--)(.)*?\n'
		t.lexer.lineno += 1



		
	def p_sidl ( self, p, m ) :
		'''
		sidl_body			: sidl_body sidl_unit
							| empty

		sidl_unit			: env_unit
							| exec_unit
							| action_unit
							| itf_unit
							| abs_unit
							| impl_unit
							| test_unit
							| group_unit
							| project_unit
							| solution_unit
							| record_unit
							| tag_unit
							| map_unit
							| sid_unit
							| buildenv_unit
							| buildstep_unit
							| buildsetup_unit
							| build_unit
		'''

		pass



	def p_env_unit ( self, p, m ) :
		'''
		env_unit			: sym_unit
							| print_unit
							| doc_unit
							| book_unit
							| comment_unit
							| region_unit
							| module_unit
							| uid_unit
							| switch_template
							| NOP
		'''

		pass




	def p_uid_unit ( self, p, m ) :
		'''
		uid_unit			: UID id attributes               m1
							| UID id ALIAS id attributes      m2
		'''

		env = self.Env()
		loc = self.argloc(p,1)

		if   m==1 :		self.content.CreateUID( p[2], loc=loc, env=env, attributes=p[3] )
		elif m==2 :		self.content.CreateUID( p[2], loc=loc, env=env, attributes=p[3], alias=p[4] )




	def p_sid_unit ( self, p, m ) :
		'''
		sid_unit			: STRINGID   id attributes sid_interns                               m1
							| STRINGENUM id attributes sid_interns sid_extends sid_concerns      m2

		sid_interns			: INTERNS SCONST                                                     m10
							| empty

		sid_extends			: EXTENDS id_list                                                    m10
							| empty

		sid_concerns		: CONCERNS id_list                                                   m10
							| empty
		'''

		env = self.Env()
		loc = self.argloc(p,1)

		if   m==1:		self.content.CreateStringId( p[2], string=(p[4] or p[2]), loc=loc, env=env, attributes=p[3] )
		elif m==2:		self.content.CreateStringEnum( p[2], string=(p[4] or p[2]), extends=p[5], concerns=p[6], loc=loc, env=env, attributes=p[3] )
		elif m==10:		p[0] = p[2]




	def p_module_unit ( self, p, m ) :
		'''
		module_unit			: module_object module_body

		module_object		: MODULE id attributes                    m5

		module_body			: LBRACE module_scope RBRACE              m10
							| empty

		module_scope		: module_scope mri                        m11
							| module_scope env_unit
							| empty
		'''

		env = self.Env()
		loc = self.argloc(p,1)

		if m==5 :
			self.curmod = self.content.CreateModule( name=p[2], loc=loc, env=env, attributes=p[3] )

		elif m==10 :
			self.curmod.SetBlock( self.argloc(p,1), self.argloc(p,3) )

		elif m==11 :
			self.curmod.AddScope( mri=p[2] )




	def p_sym_unit ( self, p, m ) :
		'''
		sym_unit			: SET      id expr         m1
							| UNSET    id              m2
							| APPEND   id expr         m3
							| PREPEND  id expr         m4
							| PUSH                     m5
							| POP                      m6
							| SAVE     id              m7
							| RESTORE  id              m8
							| PRAGMA   SCONST          m9
							| OUTPATH  dir             m10
							| INPATH   dir             m11
							| IOPATH   dir             m12
							| PACKAGE  uri             m13
		'''

		e = self.Env()
		if   m==1:		e.SetVar( p[2], p[3] )
		elif m==2:		e.SetVar( p[2], None )
		elif m==3:		e.AppendVar( p[2], p[3] )
		elif m==4:		e.PrependVar( p[2], p[3] )
		elif m==5:		self.Pushenv()
		elif m==6:		self.Popenv()
		elif m==7:		e.SetVar( p[2], e.Clone() )
		elif m==8:		e.Copy( e.GetVar(p[2]) )
		elif m==9:		e.SetVar( 'pragma',  p[2] )
		elif m==10:		e.SetVar( 'outpath', p[2] )
		elif m==11:		e.SetVar( 'inpath',  p[2] )
		elif m==12:		e.SetVar( 'outpath', p[2] ); e.SetVar( 'inpath', p[2] )
		elif m==13:		self.lexer.env['package'] = p[2]




	def p_region_unit ( self, p, m ) :
		'''
		region_unit			: REGION SCONST      m1
							| ENDREGION          m2
		'''
		
		loc = self.argloc(p,1)
		env = self.Env()
		if   m==1 :		self.content.OpenRegion( p[2], loc=loc, env=env )
		elif m==2 :		self.content.CloseRegion( loc=loc, env=env )



		
	def p_print_unit ( self, p ) :
		'''
		print_unit			: PRINT expr
		'''
		
		print('sidl: ', p[2])



		
	def p_action_unit ( self, p, m ) :
		'''
		action_unit			: BIN2H file_path attributes       m1
		                    | SHELL id shell_params            m2

		shell_params		: shell_params id                  m20
							| shell_params id EQ uri           m21
							| shell_params id EQ expr          m22
							| empty
		'''

		if m==1:
			self.content.Bin2H( fromfile=p[2], loc=self.argloc(p,1), attributes=p[3] )
		elif m==2:
			self.content.Shell( action=p[2], args=p[3], loc=self.argloc(p,1) )
		elif m==20:
			p[0] = p[1] or []
			p[0].append( (p[2],True) )
		elif m==21:
			p[0] = p[1] or []
			p[0].append( (p[2],p[4]) )
		elif m==22:
			loc = self.argloc(p,2)
			env = self.Env()
			p[0] = p[1] or []
			p[0].append( (p[2],env.Subst(p[4],loc=loc)) )



			
	def p_func_unit ( self, p, m ) :
		'''
		func_unit			: func_sign                                                                    m1
							| func_def                                                                     m1
							| switch_func                                                                  m1

		func_membs			: func_sign                                                                    m1
							| func_def                                                                     m1

		func_sign			: FUNC   func_rtype id func_args        func_qualifier func_mark attributes    m10

		func_def			: FUNC   func_rtype id func_args AS raw func_qualifier func_mark attributes    m11

		func_rtype			: raw_text                                                                     m1
							| empty

		func_args			: func_args COLON func_arg                                                     m20
							| empty

		func_arg			: raw id                                                                       m21
							| raw id EQ raw                                                                m22

		func_qualifier		: STATIC                                                                       m1
							| INLINE                                                                       m1
							| VIRTUAL                                                                      m1
							| TEMPLATE                                                                     m1
							| empty

		func_mark			: SCONST                                                                       m1
							| empty

		switch_asname		: AS id                                                                        m2
							| empty

		switch_template		: SWITCH id EQ func_rtype optid func_args attributes                           m30
							| SWITCH id ALIAS id switch_asname                                             m31

		switch_func			: SWITCH func_rtype id switch_asname func_args func_mark attributes            m32
							| SWITCH COLON id   id switch_asname           func_mark attributes            m33
		'''
		
		loc = self.argloc(p,1)
		env = self.Env()
		if m==1 :
			p[0] = p[1]

		elif m==2 :
			p[0] = p[2]

		elif m==10 :
			p[0] = SIDL_Function( p[3], loc=loc, env=env, rtype=p[2], qualifier=p[5], mark=p[6], attributes=p[7] )
			for a in atmake.ToList(p[4]):	p[0].AppendArg( type=a['type'], name=a['name'], defvalue=a['def'] )

		elif m==11 :
			p[0] = SIDL_Function( p[3], loc=loc, env=env, rtype=p[2], qualifier=p[7], mark=p[8], attributes=p[9], code=p[6] )
			for a in atmake.ToList(p[4]):	p[0].AppendArg( type=a['type'], name=a['name'], defvalue=a['def'] )

		elif m==20 :
			p[0] = atmake.ToList(p[1]) + atmake.ToList(p[3])

		elif m==21 or m==22 :
			if m==22:	defvalue = p[4].text
			else:		defvalue = None
			p[0] = { 'type':p[1].text, 'name':p[2], 'def':defvalue }

		elif m==30 :
			swtemplate = SIDL_Function( p[5], loc=loc, env=env, rtype=p[4], attributes=p[7] )
			for a in atmake.ToList(p[6]):	swtemplate.AppendArg( type=a['type'], name=a['name'], defvalue=a['def'] )
			self.content.AddSwitchTemplate( uri=p[2], func=swtemplate )
			sw = copy.deepcopy( swtemplate )

		elif m==31 :
			if not p[4] in self.content.switchtemplates :
				SIDL_Error( r"Switch template '%s' is not defined !"%p[4], loc )
			swtemplate = self.content.switchtemplates[ p[4] ]
			swtemplate = copy.deepcopy( swtemplate )
			swtemplate.uri  = p[5] or swtemplate.uri
			swtemplate.name = swtemplate.uri
			self.content.AddSwitchTemplate( uri=p[2], func=swtemplate )

		elif m==32 :
			p[0] = SIDL_Function( p[4] or p[3], loc=loc, env=env, rtype=p[2], switched=True, switchname=p[3], mark=p[6], attributes=p[7] )
			for a in atmake.ToList(p[5]):	p[0].AppendArg( type=a['type'], name=a['name'], defvalue=a['def'] )

		elif m==33 :
			if not p[3] in self.content.switchtemplates :
				SIDL_Error( r"Switch template '%s' is not defined !"%p[3], loc )
			swtemplate = self.content.switchtemplates[ p[3] ]
			sw = copy.deepcopy( swtemplate )
			sw.switched		  = True
			sw.uri			  = p[5] or ''.join( [x for x in [swtemplate.name,p[4]] if x] )
			sw.name			  = sw.uri
			sw.switchname     = p[4]
			sw.mark			  = p[6]
			sw.attributes.update( p[7] )
			p[0] = sw




	def p_itf_unit ( self, p, m ) :
		'''
		itf_unit			: itf_object itf_body

		itf_object			: INTERFACE id                   attributes     m2
							| INTERFACE id EXTENDS id        attributes     m3
							| INTERFACE id FROM    file_path attributes     m4

		itf_body			: LBRACE itf_body_units RBRACE                  m9
							| empty

		itf_body_units		: itf_body_units env_unit
							| itf_body_units func_unit                      m10
							| itf_body_units section_unit                   m11
							| itf_body_units USING file                     m12
							| itf_body_units FORWARD uri                    m13
							| itf_body_units typedef_unit                   m14
							| empty
		'''
		
		loc = self.argloc(p,1)
		env = self.Env()
		if m==1 :
			p[0] = p[1]

		elif m==2 :
			self.curitf = self.content.CreateInterface( p[2], loc=loc, env=env, attributes=p[3] )

		elif m==3 :
			self.curitf = self.content.CreateInterface( p[2], loc=loc, env=env, supername=p[4], attributes=p[5] )

		elif m==4 :
			self.curitf = self.content.CreateInterface( p[2], loc=loc, env=env, fromfile=p[4], attributes=p[5] )

		elif m==9 :
			self.curitf.SetBlock( self.argloc(p,1), self.argloc(p,3) )

		elif m==10 :
			p[2].DefaultQualifier( 'virtual' )
			self.curitf.AddFunction( p[2] )

		elif m==11 :
			self.curitf.AddSection( p[2]['id'], p[2]['rw'] )

		elif m==12 :
			self.curitf.AddUsingFile( p[3] )

		elif m==13 :
			self.curitf.AddForward( p[3] )

		elif m==14 :
			self.curitf.AddSubType( p[2] )




	def p_abs_unit ( self, p, m ) :
		'''
		abs_unit			: abs_object abs_body

		abs_object			: ABSTRACT id abs_super abs_concerns attributes     m2

		abs_super			: EXTENDS id                                        m10
							| empty

		abs_concerns		: CONCERNS id_list                                  m10
							| empty

		abs_body			: LBRACE abs_body_units RBRACE                      m9
							| empty

		abs_body_units		: abs_body_units env_unit
							| abs_body_units section_unit                       m20
							| abs_body_units USING file                         m21
							| abs_body_units FOR mri WRITES raw_nosubst         m22
							| abs_body_units D_STATE uri state_params           m23
							| empty

		state_params		: state_params id                                   m30
							| state_params id EQ uri                            m31
							| state_params id COLON id EQ uri                   m32
							| empty
		'''

		if m==1 :
			p[0] = p[1]

		elif m==2 :
			self.curabs = self.content.CreateAbstract( p[2], loc=self.argloc(p,1), env=self.Env(), supername=p[3], connames=p[4], attributes=p[5] )

		elif m==9 :
			self.curabs.SetBlock( self.argloc(p,1), self.argloc(p,3) )

		elif m==10 :
			p[0] = p[2]

		elif m==20 :
			self.curabs.AddSection( p[2]['id'], p[2]['rw'] )

		elif m==21 :
			self.curabs.AddUsingFile( p[3] )

		elif m==22 :
			self.curabs.AddBridge( pattern=p[3], raw=p[5], loc=self.argloc(p,2) )

		elif m==23 :
			self.curabs.AddState( uri=p[3], attributes=p[4], loc=self.argloc(p,2) )

		elif m==30 :
			if not p[2] in ['start'] :
				error.SIDL_Error( 'Invalid state attribute <%s> !'%p[2], self.argloc(p,2) )
			attrs = p[1] or {}
			attrs[ p[2] ] = 1
			p[0] = attrs

		elif m==31 :
			if not p[2] in ['next'] :
				error.SIDL_Error( 'Invalid state attribute <%s> !'%p[2], self.argloc(p,2) )
			attrs = p[1] or {}
			attrs[ p[2] ] = p[4]
			p[0] = attrs

		elif m==32 :
			if not p[2] in ['goto'] :
				error.SIDL_Error( 'Invalid state attribute <%s> !'%p[2], self.argloc(p,2) )
			attrs = p[1] or {}
			attrs[ p[2] ] = (p[4],p[6])
			p[0] = attrs



	def p_impl_unit ( self, p, m ) :
		'''
		impl_unit			: impl_object impl_body
							| impl_alias

		impl_object			: IMPLEMENTATION id impl_qualifier impl_super impl_finalizes impl_concerns attributes    m2

		impl_alias			: IMPLEMENTATION id ALIAS id                    m3

		impl_qualifier		: STATIC                                        m5
							| empty

		impl_super			: EXTENDS id                                    m10
							| empty

		impl_finalizes		: FINALIZES id                                  m10
							| empty

		impl_concerns		: CONCERNS id_list                              m10
							| empty

		impl_body			: LBRACE impl_body_units RBRACE                 m9
							| empty

		impl_body_units		: impl_body_units env_unit
							| impl_body_units section_unit                  m20
							| impl_body_units USING file                    m21
							| impl_body_units FOR mri WRITES raw_nosubst    m22
							| impl_body_units D_STATE uri state_params      m23
							| empty
		'''

		env = self.Env()
		loc = self.argloc(p,1)

		if m==1 :
			p[0] = p[1]

		elif m==2 :
			self.curimpl = self.content.CreateImplementation( p[2], loc=loc, env=env, qualifier=p[3], supername=p[4], abstractname=p[5], connames=p[6], attributes=p[7] )

		elif m==3 :
			self.content.CreateUID( p[2], loc=loc, env=env, alias=p[4] )

		elif m==5 :
			p[0] = p[1]

		elif m==9 :
			self.curimpl.SetBlock( self.argloc(p,1), self.argloc(p,3) )

		elif m==10 :
			p[0] = p[2]

		elif m==20 :
			self.curimpl.AddSection( p[2]['id'], p[2]['rw'] )

		elif m==21 :
			self.curimpl.AddUsingFile( p[3] )

		elif m==22 :
			self.curimpl.AddBridge( pattern=p[3], raw=p[5], loc=loc )

		elif m==23 :
			self.curimpl.AddState( uri=p[3], attributes=p[4], loc=self.argloc(p,2) )



			
			
	def p_test_unit ( self, p, m ) :
		'''
		test_unit			: test_object test_body

		test_object			: TEST uri attributes               m1

		test_body			: LBRACE test_units RBRACE          m9
							| empty

		test_units			: test_units env_unit
							| test_units section_unit           m2
							| test_units USING file             m3
							| empty
		'''

		env = self.Env()
		loc = self.argloc(p,1)

		if m==1 :
			self.curtest = self.content.CreateTest( p[2], loc=loc, env=env, attributes=p[3] )

		elif m==2 :
			self.curtest.AddSection( p[2]['id'], p[2]['rw'] )

		elif m==3 :
			self.curtest.AddUsingFile( p[3] )

		elif m==9 :
			self.curtest.SetBlock( self.argloc(p,1), self.argloc(p,3) )




	def p_group_unit ( self, p, m ) :
		'''
		group_unit			: group_object group_body

		group_object		: GROUP uri attributes							m1

		group_body			: LBRACE group_folders RBRACE					m9
							| empty

		group_folders       : group_folders group_folder
							| group_folders env_unit
							| empty

		group_folder		: FOLDER SCONST group_files						m2

		group_files			: group_files ADDS files group_options			m3
							| empty

		group_options		: group_options IFEXISTING						m10
							| group_options EXCLUDEDFROMBUILD				m10
							| empty
		'''

		env = self.Env()
		loc = self.argloc(p,1)

		if m==1 :
			self.curgroup = self.content.CreateGroup( uri=p[2], loc=loc, env=env, attributes=p[3] )

		elif m==2 :
			if p[3]:
				for g in atmake.ToList(p[3]) :
					self.curgroup.AddFiles( foldername=p[2], files=g[0], options=g[1] )
			else :
				self.curgroup.AddEmptyFolder( foldername=p[2] )

		elif m==3 :
			files   = atmake.ToList(p[3])
			options = atmake.ToList(p[4])
			if len(files)==0 :
				if 'ifexisting' in options :
					p[0] = atmake.ToList(p[1])
				elif self.ParserOption('group_enable_empty',loc) :
					p[0] = atmake.ToList(p[1])
				else :
					loc = self.argloc(p,2)
					SIDL_Error( r"No file available and can't be empty !", loc )
			else :
				p[0] = atmake.ToList(p[1]) + [(files,options)]

		elif m==9 :
			self.curgroup.SetBlock( self.argloc(p,1), self.argloc(p,3) )

		elif m==10 :
			p[0] = atmake.ToList(p[1]) + atmake.ToList(p[2])


			
			
	def p_project_unit ( self, p, m ) :
		'''
		project_unit		: project_object project_body

		project_object		: PROJECT uri                                                   attributes    m1
							| PROGRAM uri project_target project_prebuild project_postbuild attributes    m2
							| ARCHIVE uri project_target project_prebuild project_postbuild attributes    m3
							| DLL     uri project_target project_prebuild project_postbuild attributes    m4

		project_target		: TARGETS file_optpath                                                        m5

		project_buildargs	: WITH attribute_list                                                         m5
							| empty

		project_prebuild	: PREBUILD uri project_buildargs                                              m6
							| empty

		project_postbuild	: POSTBUILD uri project_buildargs                                             m6
							| empty

		project_body		: LBRACE project_body_units RBRACE                                            m9
							| empty

		project_body_units	: project_body_units mri                                                      m10
							| project_body_units env_unit
							| mri                                                                         m11
		'''
		
		env = self.Env()
		loc = self.argloc(p,1)

		if m==1 :
			self.curproj = self.content.CreateProject( uri=p[2], loc=loc, env=env, attributes=p[3] )

		elif m==2 or m==3  or m==4 :
			build = { 2:'program', 3:'archive', 4:'dll' }[m]
			if p[4] :
				act, args = p[4]
				if not act in self.content.actions :
					SIDL_Error( 'Prebuild action [%s] is not defined !'%act, loc )
			if p[5] :
				act, args = p[5]
				if not act in self.content.actions :
					SIDL_Error( 'Postbuild action [%s] is not defined !'%act, loc )
			attr = dict( p[6], build=build, target=p[3], prebuild=p[4], postbuild=p[5] )
			self.curproj = self.content.CreateProject( uri=p[2], loc=loc, env=env, attributes=attr )

		elif m==5 :
			p[0] = p[2]

		elif m==6 :
			p[0] = ( p[2], p[3] )

		elif m==9 :
			self.curproj.SetBlock( self.argloc(p,1), self.argloc(p,3) )

		elif m==10 :
			self.curproj.AddGroup( uri=p[2], content=self.content )

		elif m==11 :
			self.curproj.AddGroup( uri=p[1], content=self.content )


			
			
	def p_solution_unit ( self, p, m ) :
		'''
		solution_unit		: solution_object solution_body

		solution_object		: SOLUTION uri attributes                   m1

		solution_body		: LBRACE solution_body_units RBRACE         m9
							| empty

		solution_body_units	: solution_body_units solution_proj
							| solution_body_units env_unit
							| empty

		solution_proj		: uri                                       m2
							| uri REQUIRES solution_proj_deps           m3

		solution_proj_deps	: solution_proj_deps AND uri                m5
							| uri                                       m6
		'''
		
		env = self.Env()
		loc = self.argloc(p,1)
		if   m==1 :		self.cursol = self.content.CreateSolution( uri=p[2], loc=loc, env=env, attributes=p[3] )
		elif m==9 :		self.cursol.SetBlock( self.argloc(p,1), self.argloc(p,3) )
		elif m==2 :		self.cursol.AddProject( uri=p[1], content=self.content )
		elif m==3 :		self.cursol.AddProject( uri=p[1], content=self.content, deps=p[3] )
		elif m==3 :		p[0] = p[1] + [p[3]]
		elif m==6 :		p[0] = [p[1]]


		
		
	def p_section_unit ( self, p, m ) :
		'''
		section_unit		: SECTION id attributes raw_unit          m1
		'''
		
		if m==1 :
			p[0] = { 'id':p[2], 'rw':p[4] }; p[4].SetAttributes( p[3] )


		
		
	def p_record_unit ( self, p, m ) :
		'''
		record_unit			: record_object record_body

		record_object		: RECORD uri attributes                   m3

		record_body			: LBRACE record_members RBRACE            m4
							| empty

		record_members		: record_members record_const             m5
							| record_members record_variable          m6
							| record_members env_unit
							| empty

		record_const		: D_CONST id EQ raw_text                                                 m10
		record_variable		: record_vtype record_vdim record_valign id record_vvalue record_vcond   m11

		record_vtype		: D_UI                                    m20
							| D_SI                                    m20
							| D_FP                                    m20
							| D_FX                                    m20

		record_vdim			: PCONST                                  m1
							| empty

		record_valign		: COLON ICONST                            m2
							| empty

		record_vvalue		: EQ raw_text                             m2
							| empty

		record_vcond		: GIVEN id                                m2
							| GIVEN raw_text                          m2
							| empty
		'''

		env = self.Env()
		loc = self.argloc(p,1)
		if   m==1  :	p[0] = p[1]
		elif m==2  :	p[0] = p[2]
		elif m==3  :	self.currec = self.content.CreateRecord( uri=p[2], loc=loc, env=env, attributes=p[3] )
		elif m==4  :	self.currec.SetBlock( self.argloc(p,1), self.argloc(p,3) )
		elif m==5  :	self.currec.AddConstant( p[2] )
		elif m==6  :	self.currec.AddVariable( p[2] )
		elif m==10 :	p[0] = SIDL_RecordItem( what='const', id=p[2], value=p[4], loc=loc )
		elif m==11 :	p[0] = SIDL_RecordItem( what='var', type=p[1], dim=p[2], align=p[3], id=p[4], value=p[5], given=p[6], loc=loc )
		elif m==20 :	p[0] = p[1][1:]



	def p_tag_unit ( self, p, m ) :
		'''
		tag_unit			: tag_object tag_body

		tag_object			: TAG uri attributes                    m3

		tag_body			: LBRACE tag_members RBRACE             m4
							| empty

		tag_members			: tag_members record_const              m5
							| tag_members record_variable           m6
							| tag_members tag_record                m7
							| tag_members tag_code                  m8
							| tag_members env_unit
							| empty

		tag_record			: D_RECORD uri record_valign id record_vcond    m10

		tag_code			: D_CODE ICONST                         m2
		'''

		env = self.Env()
		loc = self.argloc(p,1)
		if   m==1  :	p[0] = p[1]
		elif m==2  :	p[0] = p[2]
		elif m==3  :	self.curtag = self.content.CreateTag( uri=p[2], loc=loc, env=env, attributes=p[3] )
		elif m==4  :	self.curtag.SetBlock( self.argloc(p,1), self.argloc(p,3) )
		elif m==5  :	self.curtag.AddConstant( p[2] )
		elif m==6  :	self.curtag.AddVariable( p[2] )
		elif m==7  :	self.curtag.AddRecord( p[2], content=self.content )
		elif m==8  :	self.curtag.SetTagCode( p[2], content=self.content )
		elif m==10 :	p[0] = SIDL_RecordItem( what='rec', type=p[2], align=p[3], id=p[4], given=p[5], loc=loc )



		
	def p_map_unit ( self, p, m ) :
		'''
		map_unit			: map_object map_body

		map_object			: MAP uri attributes                      m3

		map_body			: LBRACE map_members RBRACE               m4
							| empty

		map_members			: map_members mri			              m5
							| map_members env_unit
							| mri                                     m6
		'''
		
		env = self.Env()
		loc = self.argloc(p,1)
		if   m==1  :	p[0] = p[1]
		elif m==2  :	p[0] = p[2]
		elif m==3  :	self.curmap = self.content.CreateMap( uri=p[2], loc=loc, env=env, attributes=p[3] )
		elif m==4  :	self.curmap.SetBlock( self.argloc(p,1), self.argloc(p,3) )
		elif m==5  :	self.curmap.AddTag( uri=p[2], loc=loc, content=self.content )
		elif m==6  :	self.curmap.AddTag( uri=p[1], loc=loc, content=self.content )

		
		

	def p_buildenv_unit ( self, p, m ) :
		'''
		buildenv_unit			: buildenv_object	buildenv_rawsubst				m1

		buildenv_object			: BUILDENV uri buildenv_fromfile					m3

		buildenv_fromfile		: FROM file_path									m4
								| FROM file_path WITH attribute_list				m5
								| empty
								
		buildenv_rawsubst		: raw_nosubst										m2
								| empty
		'''
		
		env = self.Env()
		loc = self.argloc(p,1)
		if 	 m==1:	p[1].arguments = p[2]
		if 	 m==2:	p[0] = p[1]
		elif m==2:	p[0] = p[2]
		elif m==3:	p[0] = self.content.CreateBuildEnv( uri=p[2], loc=loc, env=env, config=p[3], attributes=None )
		elif m==4:	p[0] = { 'filename':p[2] }
		elif m==5:	p[0] = { 'filename':p[2], 'params':p[4] }

		
		

	def p_buildstep_unit ( self, p, m ) :
		'''
		buildstep_unit			: buildstep_object raw_nosubst							m5

		buildstep_object		: BUILDSTEP uri buildstep_inputs buildstep_outputs buildstep_qualifier attributes	m6

		buildstep_inputs		: INPUTS  buildstep_reflist							m2
								| empty
		buildstep_outputs		: OUTPUTS buildstep_reflist							m2
								| empty
		buildstep_reflist		: buildstep_reflist buildstep_reference				m3
								| buildstep_reference								m4

		buildstep_reference		: SCONST											m1
								| ID												m1
								| mri												m1

		buildstep_qualifier		: UNARY												m1
								| MULTIPLE											m1
								| empty
		'''
		env = self.Env()
		loc = self.argloc(p,1)
		if 	 m==1:	p[0] = p[1]
		elif m==2:	p[0] = p[2]
		elif m==3:	p[0] = p[1]+[p[2]]
		elif m==4:	p[0] = [p[1]]
		elif m==5:	p[1].actions = p[2]
		elif m==6:	p[0] = self.content.CreateBuildStep( uri=p[2], loc=loc, env=env, inputs=p[3], outputs=p[4], qualifier=p[5], attributes=p[6] )

		
		

	def p_buildsetup_unit ( self, p, m ) :
		'''
		buildsetup_unit			: buildsetup_object buildsetup_body					m1
		
		buildsetup_object		: BUILDSETUP uri	attributes						m5

		buildsetup_body			: LBRACE buildsetup_body_units RBRACE

		buildsetup_body_units	: buildsetup_body_units mri                         m7
								| buildsetup_body_units env_unit
								| mri												m8
		'''
		env = self.Env()
		loc = self.argloc(p,1)		
		if 	 m==1:	p[0] = p[1]
		elif m==5:	self.curbuildsetup = self.content.CreateBuildSetup( uri=p[2], loc=loc, env=env, attributes=p[3] )
		elif m==7:	self.curbuildsetup.AddNode( mri=p[2], content=self.content )
		elif m==8:	self.curbuildsetup.AddNode( mri=p[1], content=self.content )

		
		

	def p_build_unit ( self, p, m ) :
		'''
		build_unit			: BUILD build_targets build_using build_from 	m1
		
		build_targets		: TARGETS uri 									m2
		
		build_from			: FROM uri										m2
							| empty

		build_using			: USING	uri										m2
		'''
		
		env = self.Env()
		loc = self.argloc(p,1)
		if 			m==1:	self.curbuild = self.content.CreateBuild( loc=loc, env=env, target=p[2], tree=p[3], solution=p[4] )
		elif 		m==2:	p[0] = p[2]

		
		

	def p_typedef_unit ( self, p, m ) :
		'''
		typedef_unit		: enum_unit                               m1
							| compound_unit                           m1
							| type_unit                               m1
							| delegate_unit                           m1
							| sh_enum_unit                            m1
							| sh_compound_unit                        m1

		enum_unit			: ENUM id    LBRACE enum_values RBRACE    m11
							| ENUM empty LBRACE enum_values RBRACE    m11

		enum_values			: enum_values id                          m20
							| enum_values id EQ raw                   m22
							| enum_values env_unit                    m1
							| empty

		sh_enum_unit		: ENUM id enum_values                     m10

		compound_unit		: STRUCT id    type_super LBRACE compound_membs RBRACE    m12
							| UNION  id    empty      LBRACE compound_membs RBRACE    m12

		compound_anony_unit	: STRUCT empty type_super LBRACE compound_membs RBRACE    m12
							| UNION  empty empty      LBRACE compound_membs RBRACE    m12

		delegate_unit		: DELEGATE id type_super LBRACE delegate_membs RBRACE    m12

		delegate_membs		: delegate_membs enum_unit                m20
							| delegate_membs type_unit                m20
							| delegate_membs func_sign                m25
							| delegate_membs env_unit                 m1
							| empty

		compound_membs		: compound_membs raw id                   m21
							| compound_membs compound_unit            m20
							| compound_membs compound_anony_unit      m20
							| compound_membs enum_unit                m20
							| compound_membs type_unit                m20
							| compound_membs CLASSID                  m20
							| compound_membs CDTOR                    m20
							| compound_membs func_membs               m20
							| compound_membs env_unit                 m1
							| empty

		sh_compound_unit	: STRUCT id type_super sh_compound_membs    m13
							| UNION  id empty      sh_compound_membs    m13

		sh_compound_membs	: sh_compound_membs raw id                m21
							| sh_compound_membs CLASSID               m20
							| sh_compound_membs env_unit              m1
							| empty

		type_super			: EXTENDS id                              m2
							| empty

		type_unit			: TYPE id                                 m14
							| TYPE id EQ raw                          m11
		'''

		env = self.Env()
		loc = self.argloc(p,1)

		if   m==1  :	p[0] = p[1]
		elif m==2  :	p[0] = p[2]
		elif m==10 :	p[0] = SIDL_Typedef( specifier=p[1][1:], identifier=p[2], declarations=p[3], env=env, loc=loc )
		elif m==11 :	p[0] = SIDL_Typedef( specifier=p[1][1:], identifier=p[2], declarations=p[4], env=env, loc=loc )
		elif m==12 :	p[0] = SIDL_Typedef( specifier=p[1][1:], identifier=p[2], super=p[3], declarations=p[5], env=env, loc=loc )
		elif m==13 :	p[0] = SIDL_Typedef( specifier=p[1][1:], identifier=p[2], super=p[3], declarations=p[4], env=env, loc=loc )
		elif m==14 :	p[0] = SIDL_Typedef( specifier=p[1][1:], identifier=p[2], env=env, loc=loc )
		elif m==20 :	p[0] = atmake.ToList(p[1]) + [ p[2] ]
		elif m==21 :	p[0] = atmake.ToList(p[1]) + [ (p[2],p[3]) ]
		elif m==22 :	p[0] = atmake.ToList(p[1]) + [ (p[2],p[4]) ]
		elif m==25 :
			if p[2].qualifier and p[2].qualifier!='virtual' :
				SIDL_Error( r'Only the virtual qualifier is acceptable inner a delegate object !', self.argloc(p,2) )
			p[2].qualifier = 'virtual'
			p[0] = atmake.ToList(p[1]) + [ p[2] ]




	def p_raw_units ( self, p, m ) :
		'''
		raw_unit			: raw                                m1
							| file_path                          m2

		raw					: RAW                                m3
		raw_nosubst			: RAW                                m4

		raw_text			: raw                                m5
		raw_file			: raw                                m6

		comment_unit		: COMMENT     attributes raw_unit    m10

		doc_unit			: DOC         attributes raw_unit    m11

		book_unit			: BOOK        attributes raw_unit    m12

		action_unit			: ACTION  uri attributes raw_unit    m13

		exec_unit			: EXEC        attributes raw_unit    m14
		'''
		
		env = self.Env()
		loc = self.argloc(p,1)
		if   m==1  :	p[0] = p[1]
		elif m==2  :	p[0] = SIDL_RawText( file=p[1], env=env, loc=loc )
		elif m==3  :	p[0] = SIDL_RawText( text=env.Subst(p[1],loc=loc), env=env, loc=loc )
		elif m==4  :	p[0] = SIDL_RawText( text=p[1], env=env, loc=loc )
		elif m==5  :	p[0] = p[1].text
		elif m==6  :	p[0] = p[1].file
		elif m==10 :	self.content.AddComment( attributes=p[2], raw=p[3] )
		elif m==11 :	self.content.AddDoc( attributes=p[2], raw=p[3] )
		elif m==12 :	self.content.AddBook( attributes=p[2], raw=p[3] )
		elif m==13 :	self.content.AddAction( uri=p[2], attributes=p[3], raw=p[4] )
		elif m==14 :	self.Exec( p[3].GetText() )



		
	def p_file ( self, p, m ) :
		'''
		file				: file_path                    m1
							| file_id_inoutput             m1

		file_path			: file_existpath               m1
							| file_actionpath              m1

		file_actionpath		: file_existpath ASARRAY optid m5

		file_existpath		: PCONST                       m6
							| SCONST                       m7

		file_optpath		: SCONST                       m8

		file_id_input		: uri                          m40
		file_id_output		: uri                          m41
		file_id_inoutput	: uri                          m42
		file_id_dep			: uri                          m43

		dir					: PCONST                       m10
							| SCONST                       m11

		files				: files_glob                   m20
							| file_actionpath	           m20
							| file_id_inoutput             m20

		files_dep			: files_glob                   m20
							| file_actionpath	           m20
							| file_id_dep                  m20

		files_glob			: PCONST                       m30
							| SCONST                       m31
		'''
		
		loc = self.argloc(p,1)

		if m==1:
			p[0] = p[1]

		# file_actionpath (bin2h)
		elif m==5 :
			arrayname = p[3] or 1
			p[0] = self.content.Bin2H( fromfile=p[1], loc=loc, attributes={'array':arrayname} )
			if not p[0] :
				SIDL_Error( r'Bin2H has failed on file [%s] !'%p[1], loc )

		# file_existpath (relative to path)
		elif m==6 :
			f = self.Subst( p[1], loc )
			p[0] = self.Findfile( f )
			if not p[0] :
				if self.ParserOption('file_lazzy',loc) :
					p[0] = ''
				else :
					SIDL_Error( r'File [%s] not found in pathlist !'%f, loc )

		# file_existpath (relative to sidlfile)
		elif m==7 :
			f = self.Subst( p[1], loc )
			p[0] = self.Checkfile( f, loc )
			if not p[0] :
				if self.ParserOption('file_lazzy',loc) :
					p[0] = ''
				else :
					SIDL_Error( r'File [%s] not found !'%f, loc )

		# file_optpath
		elif m==8 :
			f = self.Subst( p[1], loc )
			p[0] = self.Extendpath( f, loc )

		# file_id_input
		elif m==40 :
			res = self.content.FindByURI( p[1] )
			if len(res)==0:		SIDL_Error( r'No reference found for pattern [%s] !'%p[1], loc )
			if len(res)!=1:		SIDL_Error( r'Too many references found for pattern [%s] !'%p[1], loc )
			try:					ipath = res[0].GetInpath()
			except AttributeError:	ipath = None
			if not ipath :
				SIDL_Error( r'No input path defined for the reference [%s] !'%p[1], loc )
			p[0] = ipath

		# file_id_output
		elif m==41 :
			res = self.content.FindByURI( p[1] )
			if len(res)==0:		SIDL_Error( r'No reference found for pattern [%s] !'%p[1], loc )
			if len(res)!=1:		SIDL_Error( r'Too many references found for pattern [%s] !'%p[1], loc )
			try:					opath = res[0].GetOutpath()
			except AttributeError:	opath = None
			if not opath :
				SIDL_Error( r'No output path defined for the reference [%s] !'%p[1], loc )
			p[0] = opath

		# file_id_inoutput
		elif m==42 :
			res = self.content.FindByURI( p[1] )
			if len(res)==0:		SIDL_Error( r'No reference found for pattern [%s] !'%p[1], loc )
			if len(res)!=1:		SIDL_Error( r'Too many references found for pattern [%s] !'%p[1], loc )
			try:					ipath = res[0].GetInpath()
			except AttributeError:	ipath = None
			try:					opath = res[0].GetOutpath()
			except AttributeError:	opath = None
			fpath = ipath or opath
			if not fpath :
				SIDL_Error( r'No path defined for the reference [%s] !'%p[1], loc )
			p[0] = fpath

		# file_id_dep
		elif m==43 :
			res = self.content.FindByURI( p[1] )
			if len(res)==0:		SIDL_Error( r'No reference found for pattern [%s] !'%p[1], loc )
			if len(res)!=1:		SIDL_Error( r'Too many references found for pattern [%s] !'%p[1], loc )
			try:					ideps = res[0].GetDependencies()
			except AttributeError:	ideps = None
			if ideps is None :
				SIDL_Error( r'No dependencies defined for the reference [%s] !'%p[1], loc )
			p[0] = ideps

		# dir (relative to path)
		elif m==10 :
			d    = self.Subst( p[1], loc )
			p[0] = self.Finddir( d )
			if not p[0] :
				if self.ParserOption('file_lazzy',loc) :
					p[0] = ''
				else :
					SIDL_Error( r'Directory [%s] not found in pathlist !'%d, loc )

		# dir (relative to sidlfile)
		elif m==11 :
			d    = self.Subst( p[1], loc )
			p[0] = self.Checkdir( d, loc )
			if not p[0] :
				if self.ParserOption('file_lazzy',loc) :
					p[0] = ''
				else :
					SIDL_Error( r'Directory [%s] not found !'%d, loc )

		# files / files_dep
		elif m==20 :
			p[0] = atmake.ToList( p[1] )

		# files_glob (relative to path)
		elif m==30 :
			d = self.Subst( p[1], loc )
			if ('?' in d) or ('*' in d) :
				base = os.path.basename( d )
				dir1 = os.path.dirname( d ) or '.'
				dir2 = self.Finddir( dir1 )
				if not dir2:
					SIDL_Error( r'Directory [%s] not found !'%dir1, loc )
				globfilter = os.path.join( dir2, base ).replace( '\\', '/' )
				p[0] = [x for x in glob.glob(globfilter) if os.path.isfile(x)]
			else :
				p[0] = self.Findfile( d )

		# files_glob (relative to sidlfile)
		elif m==31 :
			d = self.Subst( p[1], loc )
			d = self.Extendpath( d, loc )
			if ('?' in d) or ('*' in d) :
				base = os.path.basename( d )
				dir1 = os.path.dirname( d ) or '.'
				dir2 = self.Checkdir( dir1, loc )
				if not dir2:
					SIDL_Error( r'Directory [%s] not found !'%dir1, loc )
				globfilter = os.path.join( dir2, base ).replace( '\\', '/' )
				p[0] = [x for x in glob.glob(globfilter) if os.path.isfile(x)]
			else :
				p[0] = self.Checkfile( d, loc )


				
				
	def p_attributes ( self, p, m ) :
		'''
		attributes			: DEFINES attribute_list     m2
							| empty

		attribute_list		: attribute_list attribute   m3
							| attribute                  m1

		attribute			: id EQ expr                 m4
		'''

		env = self.Env()
		if   m==1 :		p[0] = p[1]
		elif m==2 :		p[0] = p[2]
		elif m==3 :
			k,v = list(p[2].items())[0]
			if k in p[1]:	p[1].update( {k: atmake.ToList(p[1][k])+[v]} )
			else:			p[1].update( p[2] )
			p[0] = p[1]
		elif m==4 :
			p[0] = { p[1]: env.Subst(p[3]) }
		else :
			p[0] = {}




	def p_uri ( self, p, m ) :
		'''
		id					: ID                   m1

		uri					: URI                  m1
							| ID                   m1

		mri					: MRI                  m1
							| URI                  m1
							| ID                   m1

		optid				: id                   m1
							| empty

		id_list				: id_list id           m2
							| id                   m3

		uri_list			: uri_list uri         m2
							| uri                  m3

		mri_list			: mri_list mri         m2
							| mri                  m3
		'''
		
		if   m==1  :	p[0] = p[1]
		elif m==2  :	p[0] = p[1] + [ p[2] ]
		elif m==3  :	p[0] = [ p[1] ]

		
		

	def p_expressions ( self, p, m ) :
		'''
		expr				: num_expr        m1
							| bool_expr       m1
							| str_expr        m1
							| eval_expr       m1
							| subst_expr      m1

		str_expr			: SCONST          m1
							| FILE file       m10
							| DIR  dir        m10

		num_expr			: ICONST          m1
							| FCONST          m1

		bool_expr			: BCONST          m1

		eval_expr			: EVAL expr       m30

		subst_expr			: SUBST expr      m40
							| VAR             m41
		'''
		
		loc = self.argloc(p,1)
		env = self.Env()
		if   m==1 :		p[0] = p[1]
		elif m==10:		p[0] = p[2]
		elif m==30:		p[0] = self.Eval( p[2] )
		#subst
		elif m==40:		p[0] = env.Subst( p[2], loc )
		elif m==41:		p[0] = env.GetVar( p[1], loc )


		
		
	def Parse ( self, content ) :
		self.content = content
		self.env_stack = [ content.env ]
		self.parser.parse( lexer=self.lexer, tracking=True )





def SIDL_ParseFile( sidlfile=None, shelffile=None, env=None ) :
	if not env :
		SIDL_Error( 'An SIDL env is required !' )

	if not sidlfile :
		SIDL_Error( 'A SIDL file is required !' )

	if not fs.IsFilePath(sidlfile) :
		SIDL_Error( 'SIDL file [%s] is not found !'%sidlfile )

	c = SIDL_Content( env=env, shelffile=shelffile )
	p = SIDL_Parser()

	if not p.PushLexerInput( path=sidlfile ) :
		SIDL_Error( 'Parsing SIDL file has failed !' )
	p.Parse( content=c )
	return c


