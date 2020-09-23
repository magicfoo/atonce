
import sys, os, getopt, imp, inspect, types, shutil, pprint, glob, copy
from . import tools
from .tools import spawn
import collections




def iif( cond, truevalue, falsevalue ) :
	# return truevalue if cond else falsevalue
	if cond:	return truevalue
	else :	return falsevalue


def Message( msg='' ) :
	if msg :
		sys.stderr.write( "ATMake message: %s\n" % msg )


def Warning( raison ) :
	Assert( not IsEmpty(raison) )
	sys.stderr.write( "ATMake warning: %s\n" % raison )


def Error( raison, code=1 ) :
	Assert( not IsEmpty(raison) )
	sys.stderr.write( "ATMake error: %s\n" % raison )
	if code != 0 :
		sys.stderr.write( '\n' )
		sys.stderr.write( '\n' )
		raise AssertionError
	else :
		sys.exit( code )


def Abort( raison, code=0 ) :
	Assert( not IsEmpty(raison) )
	sys.stderr.write( "ATMake aborted: %s\n" % raison )
	sys.exit( code )


def Assert ( condition, raison='', code=0 ) :
	if not condition :
		if IsEmpty(raison) :	raison = '?'
		sys.stderr.write( "ATMake assert failed: %s\n" % raison )
		sys.stderr.write( '\n' )
		sys.stderr.write( '\n' )
		raise AssertionError


def BasePath() :
	return os.path.dirname(__file__)

def RelPath( rpath ) :
	full_path = os.path.normpath( BasePath() + os.sep + rpath )
	return full_path

def IsString( o ) :
	return isinstance(o,str)

def IsNumber( o ) :
	return isinstance(o,int) or isinstance(o,float)
def IsInteger( o ) :
	return isinstance(o,int)

def IsSpace( o ) :
	return IsString(o) and o.isspace()

def IsEmpty( o ) :
	if o is None :
		return True
	if IsList(o) or IsString(o) or IsTuple(o) or IsDict(o) :
		if len(o)==0 :	return True
		if IsSpace(o) :	return True
	return False

def ToString( o ) :
	if IsString(o) :	return o
	else:				return str(o)

def IsList( o ) :
	return isinstance( o, list )

def IsTuple( o ) :
	return isinstance( o, tuple )

def IsDict( o ) :
	return isinstance( o, dict )

def IsMany( o ) :
	return IsList(o) or IsTuple(o)

def ToList( o ) :
	if IsNone(o):	return []
	if IsList(o):	return o
	if IsTuple(o):	return [ i for i in o ]
	else:			return [ o ]

def ToDict( *o ) :
	# build { i:o[i] }
	if len(o)==0 :
		return {}
	if len(o)==1 and IsDict(o):
		return o
	if len(o)==1 and (IsList(o) or IsTuple(o)) :
		o = o[0]
	return dict( list(zip(list(range(0,len(o))),o)) )

def ToKeyDict( *o ) :
	# build { o[i]:i }
	if len(o)==0 :
		return {}
	if len(o)==1 and IsDict(o):
		return o
	if len(o)==1 and (IsList(o) or IsTuple(o)) :
		o = o[0]
	return dict( list(zip(o,list(range(0,len(o))))) )

def IsFunction( o ) :
	return isinstance( o, types.FunctionType )

def IsMethod( o ) :
	return isinstance( o, types.MethodType )

def IsCallable( o ) :
	return isinstance( o, collections.Callable)

def IsNone( o ) :
	return (o is None)

def IsDef( o ) :
	return not IsNone(o)

def IsDefined( o ) :
	return not o is None

def IsClassof( o, classobj ) :
	return o and str(o.__class__) == str(classobj)

def IsA( o, classobj ) :
	return o and isinstance(o,classobj)



def Call( cmdline, check=True, cwd=None, env=None ) :
	if env is None :
		env = os.environ
	scode, xcode, oo, ee = tools.spawn.Spawn( cmdline, cwd=cwd, env=env )
	if check and not scode :
		Abort( 'Command not found [%s] !' % cmdline )
	if check and xcode!=0 :
		Abort( 'CL has failed [%s]=%s !' % (cmdline,str(xcode)) )
	if oo and len(oo) :
		sys.stdout.writelines( oo )
	if ee and len(ee) :
		sys.stdout.writelines( ee )


def GetFileExt( p ) :
	if not p :
		return p
	f, e = os.path.splitext( p )
	if not e or len(e)==1 :
		return None
	return e[1:].lower()

def HasFileExt( p, e ) :
	if not p or not e :
		return False
	return GetFileExt(p) == e

def ToFileExt( p, e ) :
	if not p or not e :
		return None
	f, ce = os.path.splitext( p )
	if e and len(e)>0 :
		return '%s.%s' % (f, e)
	else :
		return f

def SplitMatchedExt( files, exts ) :
	match_s = []
	remain_s = []
	for s in ToList(files) :
		matched = False
		for e in ToList(exts) :
			if HasFileExt(s,e) :
				matched = True
				break
		if matched :	match_s.append( s )
		else :			remain_s.append( s )
	return ( remain_s, match_s )


def CpFile( src, dst ) :
	try :
		shutil.copyfile( src, dst )
		return True
	except :
		return False


def RmFile( path ) :
	try :
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
			shutil.rmtree( f, True )
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


def GetFuncArgs( func ) :
	try:
		args = inspect.getargspec( func )[0]
		defs = inspect.getargspec( func )[3]
	except AttributeError :
		return None
	if defs :	defi = len(defs) - len(args)
	else :		defi = 0
	argi = 0
	arglist = []
	for arg in args :
		if defs and defi>=0 :
			arglist.append( [arg,defs[defi]] )
		else :
			arglist.append( [arg] )
		argi += 1
		defi += 1
	return arglist


def HelpModFuncDecl( mod, funcname ) :
	args = GetFuncArgs( getattr(mod,funcname) )
	pp = pprint.PrettyPrinter( indent=4 )
	_f = f = "%s( " % str(funcname)
	for arg in args :
		if len(arg) == 1 :	f += arg[0]
		else:				f += '%s=%s' % ( arg[0], pp.pformat(arg[1]) )
		_f = f
		f += ', '
	f = _f + ' )'
	return f


def HelpModFuncDoc( mod, funcname ) :
	try:
		fdoc = inspect.getdoc( getattr(mod,funcname) )
		if not fdoc.strip().startswith('hidden') :
			return fdoc
	except AttributeError :
		pass


def HelpMod( mod ) :
	print("actions for module '%s' :" % ( mod.__name__.split('.')[-1] ))
	mod_decl = dir( mod )
	for decl in mod_decl :
		decl_attr = getattr( mod, decl )
		if IsFunction( decl_attr ) :
			fdecl = HelpModFuncDecl( mod, decl )
			fdoc  = HelpModFuncDoc( mod, decl )
			if fdecl and fdoc :
				print('\t- ', fdecl)
				print('\t\t' + fdoc.replace( '\n', '\n\t\t' ))
				print()


def ListExternalProjects ( ) :
	projdirs = []
	basedir = RelPath( '../..' )
	for projbase in glob.glob(basedir+'/*') :
		if os.path.isdir(projbase) and not projbase.startswith('.') :
			projdir = projbase+'/project'
			if os.path.isdir(projdir) :
				if glob.glob(projdir+'/*.py') :
					projdirs.append( projdir )
	return projdirs


def ImportProject ( dirpath ) :
	if os.path.isdir(dirpath) :
		if not dirpath in sys.path :
			sys.path.append( os.path.normpath(dirpath) )


def ImportExternalProjects ( ) :
	projdirs = ListExternalProjects()
	for p in projdirs :
		ImportProject( p )

def ListFrames ( ) :
	frames = {}
	for frame in glob.glob( RelPath('/frameworks/*.py') ) :
		frame_name = os.path.basename(frame)[:-3]
		if os.path.isfile(frame) and frame_name!='__init__' :
			frames[ frame_name ] = frame
	return frames
	
def LoadFrame ( framename ) :
	frames = ListFrames()
	if not framename in frames :
		Abort( "No framework named '%s' !" % framename )
#	try:
	return imp.load_source( framename, frames[framename] )
#	except:
#		Abort( "Module '%s' failed to load !" % framename )
	
	
def ListMods ( ) :
	mods = {}
	# local modules, nested under the atmake directory
	for mod in glob.glob( RelPath('*.py') ) :
		mod_name = os.path.basename(mod)[:-3]
		if os.path.isfile(mod) and mod_name!='__init__' :
			mods[ mod_name ] = mod
	# project modules, nested in external projects themselves
	projdirs = ListExternalProjects()
	for p in projdirs :
		for mod in glob.glob(p+'/*.py') :
			mod_name = os.path.basename(mod)[:-3]
			if os.path.isfile(mod) and mod_name!='__init__' :
				mods[ mod_name ] = mod
	return mods


def LoadMod( modname ) :
	mods = ListMods()
	if not modname in mods :
		Abort( "No module named '%s' !" % modname )
#	try:
	return imp.load_source( modname, mods[modname] )
#	except:
#		Abort( "Module '%s' failed to load !" % modname )




def Make( modname, action, args ) :

	def get_args( ) :
		res = {}
		for a in args :
			if IsString(a) :
				aa = a.split('=')+[None]
				n = aa[0]
				v = aa[1]
				if not IsString(n):								n = None
				elif n.startswith('--'):						n = n[2:]
				elif n.startswith('-'):							n = n[1:]
				if not IsString(v):								v = None
				elif v.isspace():								v = None
				elif v.lower()=='none':							v = None
				elif v.lower()=='true':							v = True
				elif v.lower()=='false':						v = False
				elif v.lower()=='yes':							v = True
				elif v.lower()=='no':							v = False
				elif v.isdigit():								v = int(v)
				elif v.startswith("'") and v.endswith("'"):		v = v[1:-1]
				elif v.startswith('"') and v.endswith('"'):		v = v[1:-1]
				if n:											res[n] = v
		return res

	# format args
	args = get_args()

	mod = LoadMod( modname )

	# module probing ?
	if action=='probe' or action=='help' :
		HelpMod( mod )
		return 0

	# do action
	try:
		action_func = getattr( mod, action )
	except AttributeError :
		HelpMod( mod )
		Abort( "No action named '%s' in module '%s' !" % (action,modname) )

	# action probing ?
	if len(args)==1 and ('help' in args or 'probe' in args) :
		fdecl = HelpModFuncDecl( mod, action )
		fdoc  = HelpModFuncDoc( mod, action )
		if fdecl and fdoc :
			print('%s' % fdecl)
			print('\t%s' % fdoc.replace('\n','\n\t'))
			print()
		else :
			print("No help text to display.")
		return 0

	# action's expected arguments
	action_args = GetFuncArgs( action_func )
	if not action_args :
		return action_func()
	else :
		# check arguments and build options
		global options
		options = copy.deepcopy( args )
		values = []
		for a in action_args :
			if a[0] in options :
				del options[ a[0] ]
			v = args.get( a[0], None )
			if v is None :
				if len(a)==2 :
					v = a[1]
				else :
					Abort( "Argument '%s' is missing for action '%s' !" % (a[0],HelpModFuncDecl(mod,action)) )
			values.append( v )
		return action_func( *values )




# options, build by the Make function

options = {}

def GetOptions ( ) :
	return options

def GetOption ( optname, defvalue ) :
	return options.get( optname, defvalue )

def HasOption ( optname ) :
	return (optname in options)



