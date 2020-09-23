
import os, sys
import tempfile
import atmake
import atmake.tools.cpptoxml as cpptoxml
from pygccxml import declarations
from .components import *
from .error import *
from .parser import *




def SIDL_Load( sidlfile=None, shelffile=None, env=None ) :


	def extend1 ( toappend=None, target=None ) :
		if target is None :		target = []
		for l in atmake.ToList(toappend) :
			if not l in target :
				target.append( l )
		return target


	def collect_fromfiles ( obj, res=None ) :
		if obj is None:		return res or []
		if res is None:		res = []
		if atmake.IsClassof(obj,SIDL_Interface) :
			res = collect_fromfiles( obj.super, res )
			res = extend1( obj.GetInpath(), res )
		elif atmake.IsClassof(obj,SIDL_Abstract) :
			for c in obj.concerns :
				res = collect_fromfiles( c, res )
		elif atmake.IsClassof(obj,SIDL_Implementation) :
			res = collect_fromfiles( obj.abstract, res )
			for c in obj.concerns :
				res = collect_fromfiles( c, res )
		elif atmake.IsClassof(obj,SIDL_Content) :
			for o in list(obj.interfaces.values()) + list(obj.abstracts.values()) + list(obj.implementations.values()) :
				res = collect_fromfiles( o, res )
		return res


	def parse_cpp ( sidl=None ) :
		cpppath       = sidl.env.GetVarlist('cpppath')
		gccxmldefines = sidl.env.GetVarlist('gccxmldefines')
		gccxmlfiles   = sidl.env.GetVarlist('gccxmlfiles')
		fromfiles     = collect_fromfiles( sidl )
		gccfiles      = atmake.ToList(gccxmlfiles) + atmake.ToList(fromfiles)
		cpplines = []
		for f in gccfiles :
			if fs.IsAbsPath(f) and fs.IsFilePath(f) :
				cpplines.append( '#include "%s"\n'%f )
			else :
				cpplines.append( '#include <%s>\n'%f )
		xmlfile = tempfile.NamedTemporaryFile(prefix='.xml').name
		cppfile = tempfile.NamedTemporaryFile(prefix='.cpp').name
		open(cppfile,'w').writelines( cpplines )
		ok  = cpptoxml.ParseCpp( infile=cppfile, outfile=xmlfile, incdirs=cpppath, defines=gccxmldefines )
		cpp = cpptoxml.ParseXml(xmlfile) if ok else None
		atmake.RmFile( cppfile )
		atmake.RmFile( xmlfile )
		return cpp


	def cpp_build_opttypes( cpp ) :
		opttypes = {}
		tdefs = declarations.find_all_declarations( cpp, type=declarations.typedef_t )
		for t in tdefs :
			if len(t.decl_string) < len(t.type.decl_string) :
				opttypes[t.type] = declarations.dummy_type_t(t.decl_string)
		return opttypes


	def import_intf ( sidl=None, cpp=None, opttypes=None, intf=None ) :

		def cpp_node ( cname ) :
			try:
				scopes = cname.split( '::' )
				ns = declarations.get_global_namespace( cpp )
				for s in scopes[:-1] :
					ns = ns.namespace( s )
				return ns.class_( scopes[-1] )
			except :
				print("Interface name '%s' is not found !" % cname)
				return None

		def cpp_opt_type ( tnode ) :
			opt_decl_string = tnode.decl_string
			try :
				p = None
				n = tnode
				while n :
					opt = opttypes.get( n, None )
					if opt and p :
						# change the base and rebuild the tnode super parent ...
						p.base = opt
						opt_decl_string = tnode.build_decl_string()
						break
					if opt :
						opt_decl_string = opt.decl_string
						break
					p = n
					n = p.base
			except AttributeError:
				pass
			opttypes[ tnode ] = declarations.dummy_type_t( opt_decl_string )
			return opt_decl_string

		def cpp_mth ( node ) :
				loc			= SIDL_Location( f=node.location.file_name, l=node.location.line )
				f_name		= str(node.name)
				f_rtype		= cpp_opt_type(node.return_type)
				f_args		= [ (cpp_opt_type(a.type),a.name,a.default_value) for a in node.arguments ]
				f_declname	= str(node.parent.name)
				f_vpure		= (node.virtuality == declarations.VIRTUALITY_TYPES.PURE_VIRTUAL)
				f_public	= (node.access_type == declarations.ACCESS_TYPES.PUBLIC)
				f_static	= node.has_static
				f_const		= node.has_const
				f_mangled	= node.mangled
				if f_public and f_vpure and not f_static :
					# only accepts pure and public methods
					f = SIDL_Function( name=f_name, rtype=f_rtype, declname=f_declname, isconst=f_const, hasellipsis=node.has_ellipsis, loc=loc )
					for a in f_args :
						f.AppendArg( type=a[0], name=a[1], defvalue=a[2] )
					return f

		node = cpp_node( intf.classname )
		if node :
			intf.fromcpp = node
			if node.bases :		intf.supername = [ n.related_class for n in node.bases ][0].name
			else :				intf.supername = None
			classes = [ b.related_class for b in node.recursive_bases ] + [ node ]
			funcs = []
			for c in classes:
				funcs.extend( c.member_functions(allow_empty=True) )
			for fnode in funcs :
				f = cpp_mth( fnode )
				intf.AddFunction( f )
			return


	#----------------

	sidl = SIDL_ParseFile( sidlfile=sidlfile, shelffile=shelffile, env=env )
	cpp = parse_cpp( sidl )
	if not cpp :
		SIDL_Error( 'CPP parsing error. Halted !' )
		return
	# init optimized types
	opttypes = cpp_build_opttypes( cpp )
	# importing interfaces from cpp
	for intf in sidl.interfaces.values() :
		if intf.fromfile :
			import_intf( sidl, cpp, opttypes, intf )
	return sidl, cpp




