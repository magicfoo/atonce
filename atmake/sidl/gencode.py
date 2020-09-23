

import os, sys, binascii, fnmatch
import tempfile
import atmake
import atmake.tools.algo as algo
from atmake.tools.types import *
from atmake.tools import fs
from .components import *
from .error import *
from .genuid import *



IINTERFACE_BRIDGES = [	'GetBuilder',
						'Retain',
						'CountRef',
						'Release',
						'CountInterface',
						'EnumInterface',
						'GetInterface'	]

# default bridges
#
# symbols are :
# 	$return		: 
#	$rtype		: 
#	$fname		: 
#	$fparams	: 
#	$iclassname	: 
#	$mclassname	: 
#	$sclassname	: 
#	$intf		: 
#	$skel		: 
#	$stub		: 
#                       | concern		| class				| function			| text
SKEL_DEF_BRIDGES  = [ ( '*' ,			'*',				'*',				r'$return $stub.$fname($fparams)' )																	]
SKEL_DEF_BRIDGES += [ ( '*',			'IInterface',		mth,				r'$return $skel.$fname($fparams)' ) for mth in IINTERFACE_BRIDGES									]
SKEL_DEF_BRIDGES += [ ( '*',			'IInterface',		'GetBase',			r'$return &$skel' )																					]
SKEL_DEF_BRIDGES += [ ( '*',			'IInterface',		'GetBuilder',		r'$return ${mclassname}_builder()' )																]
SKEL_DEF_BRIDGES += [ ( '*',			'ISwitch',			'CountSwitch',		r'$return ${mclassname}_count_switch()' )															]
SKEL_DEF_BRIDGES += [ ( '*',			'ISwitch',			'EnumSwitch',		r'$return ${mclassname}_enum_switch(p0,p1)' )														]
SKEL_DEF_BRIDGES += [ ( '*',			'ISwitch',			'FindSwitch',		r'$return ${mclassname}_find_switch(p0)' )															]
SKEL_DEF_BRIDGES += [ ( '*',			'ISwitch',			'CallSwitch',		r'va_list vl; va_start(vl,p1); $return ${mclassname}_call_switch_stack(m,p0,p1,vl); va_end(vl)' )	]
SKEL_DEF_BRIDGES += [ ( '*',			'ISwitch',			'CallSwitchMem',	r'$return ${mclassname}_call_switch_mem(m,p0,p1,p2,p3)' )											]





def SIDL_GenCode ( sidl=None, header=None, warnings=None, verbose=None, generate_cpp_line_directive=1 ) :

	def extend1 ( toappend=None, target=None ) :
		if target is None :
			target = []
		for l in atmake.ToList(toappend) :
			if not l in target :
				target.append( l )
		return target


	def collect_using_files ( obj, res=None ) :
		if obj is None:		return res or []
		if res is None:		res = []
		if atmake.IsClassof(obj,SIDL_Interface) :
			res = collect_using_files( obj.super, res )
			res = extend1( obj.usingfiles, res )
		elif atmake.IsClassof(obj,SIDL_Abstract) :
			res = extend1( obj.usingfiles, res )
			for c in obj.concerns :
				res = collect_using_files( c, res )
		elif atmake.IsClassof(obj,SIDL_Implementation) :
			res = collect_using_files( obj.abstract, res )
			res = extend1( obj.usingfiles, res )
			for c in obj.concerns :
				res = collect_using_files( c, res )
		elif atmake.IsA(obj,SIDL_CodeObject) :
			res = extend1( obj.usingfiles, res )
		return res


	def collect_dep_files ( obj, res=None ) :
		if obj is None:		return res or []
		if res is None:		res = []
		if atmake.IsClassof(obj,SIDL_Interface) :
			res = collect_dep_files( obj.super, res )
			res = extend1( obj.usingfiles, res )
			res = extend1( obj.GetInpath(), res )
			res = extend1( obj.GetOutpath(), res )
		elif atmake.IsClassof(obj,SIDL_Abstract) :
			res = extend1( obj.usingfiles, res )
			for c in obj.concerns :
				res = collect_dep_files( c, res )
		elif atmake.IsClassof(obj,SIDL_Implementation) :
			res = collect_dep_files( obj.abstract, res )
			res = extend1( obj.usingfiles, res )
			for c in obj.concerns :
				res = collect_dep_files( c, res )
		elif atmake.IsA(obj,SIDL_CodeObject) :
			res = extend1( obj.usingfiles, res )
		return res


	def collect_concerns ( obj, res=None ) :
		if obj is None:		return res or []
		if not res:			res = []
		if atmake.IsClassof(obj,SIDL_Abstract) :
			res = extend1( obj.concerns, res )
		elif atmake.IsClassof(obj,SIDL_Implementation) :
			res = collect_concerns( obj.abstract, res )
			res = extend1( obj.concerns, res )
		return res


	def collect_section ( obj, secname, res=None ) :
		if obj is None:		return res or []
		if not res:			res = []
		if atmake.IsClassof(obj,SIDL_Implementation) :
			res = collect_section( obj.abstract, secname, res )
			res = extend1( obj.sections[secname], res )
		elif atmake.IsA(obj,SIDL_CodeObject) :
			res = extend1( obj.sections[secname], res )
		return res


	def collect_attributes ( obj, res=None ) :
		if obj is None:		return res or {}
		if not res:			res = {}
		if atmake.IsClassof(obj,SIDL_Implementation) :
			res = collect_attributes( obj.abstract, res )
			res.update( obj.attributes )
		elif atmake.IsA(obj,SIDL_CodeObject) :
			res.update( obj.attributes )
		return res


	def collect_bridges ( obj, res=None ) :
		if obj is None:		return res or []
		if not res:			res = list( SKEL_DEF_BRIDGES )
		if atmake.IsClassof(obj,SIDL_Abstract) :
			res = extend1( obj.bridges, res )
		elif atmake.IsClassof(obj,SIDL_Implementation) :
			res = collect_bridges( obj.abstract, res )
			res = extend1( obj.bridges, res )
		return res


	def collect_funcs ( concern, funcs=None, fcache=None ) :
		# collect all functions from bases
		if funcs  is None:		funcs  = []
		if fcache is None:		fcache = {}
		if concern.super :
			collect_funcs( concern.super, funcs, fcache )
		for f in concern.functions :
			if (not f.qualifier) or (f.qualifier=='virtual') :
				mang = f.GetMangled()
				if mang not in fcache :
					funcs.append( f )
					fcache[mang] = f
		return funcs


	def collect_switches ( impl, concerns ) :
		res = []
		for c in concerns :
			for f in collect_funcs(c) :
				if f.switched :
					res.append( (f,c) )
		return res


	def gettype_func_arg ( intype ) :
		t = intype.replace(' ','')
		if t in ['void']:									return 'v'
		if t in ['bool']:									return 'b'
		if t in ['char','uchar','byte','int8','uint8']:		return 'c'
		if t in ['short','ushort','int16','uint16']:		return 'h'
		if t in ['int','uint','int32','uint32']:			return 'w'
		if t in ['long','ulong','int64','uint64']:			return 'l'
		if t in ['float']:									return 'f'
		if t in ['pcstr','pstr','char*','const char*']:		return 's'
		if t.endswith('*'):									return 'p'


	def asany_func_arg ( intype ) :
		t = gettype_func_arg(intype)
		if t==None:		return None
		if t=='v':		return None
		if t=='p':		return 'asPointer'
		if t=='b':		return 'asBool'
		if t=='c':		return 'asInt'
		if t=='h':		return 'asInt'
		if t=='w':		return 'asInt'
		if t=='l':		return 'asInt'
		if t=='f':		return 'asFloat'
		if t=='s':		return 'asString'


	def generate_inc_file ( obj, path ) :
		if len(path)>2 and path[0]=='"' and path[-1]=='"' :
			return path
		if len(path)>2 and path[0]=='<' and path[-1]=='>' :
			return path
		objpath = obj.GetOutpath()
		if fs.IsAbsPath(path) :
			return '"%s"' % fs.FileToFile( path, objpath )
		abspath = fs.ToAbsPath( path )
		if fs.IsFilePath(abspath) :
			return '"%s"' % fs.FileToFile( abspath, objpath )
		return '<%s>' % path
		

	def generate_code ( rw, obj, prefix='' ) :
		outpath = obj.GetOutpath()
		lines = []
		if atmake.IsString(rw.text) :
			rw_lineno   = rw.loc.lineno
			rw_filepath = fs.Path(rw.loc.filename)
			ulines = types.UnindentLines( rw.text.splitlines() )
			# remove heading empty lines
			while ulines and len(ulines[0].strip())==0 :
				del ulines[0]; rw_lineno += 1
			# remove ending empty lines
			while ulines and len(ulines[-1].strip())==0 :
				del ulines[-1]
			if ulines :
				if generate_cpp_line_directive :
					lines.append( '%s#line %d "%s"\n' % (prefix, rw_lineno, rw_filepath) )
				for l in ulines :
					lines.append( '%s%s\n' % (prefix,l) )
				if generate_cpp_line_directive :
					lines.append( '%s#line @lineno@ "%s"\n' % (prefix,fs.Path(outpath)) )
		elif atmake.IsString(rw.file) :
			ifile = generate_inc_file( obj, rw.file )
			lines.append( '%s#include %s\n' % (prefix, ifile) )
		return lines

		
	def generate_section ( section, sectname, obj, prefix='' ) :
		outpath = obj.GetOutpath()
		allfromfile = True
		for s in atmake.ToList(section) :
			if not s.file:
				allfromfile = False
		lines = []
		if section and allfromfile :
			lines.extend( ['#undef  SIDL_%s\n'%sn.upper() for sn in obj.sections.keys()] )
			lines.append( '#define SIDL_%s\n'%sectname.upper() )
		c = 0
		for s in atmake.ToList(section) :
			lines.extend( generate_code(rw=s,obj=obj,prefix=prefix) )
			lines.append( '\n' )
			c+=1
		if c>0 :
			del lines[-1]
		return lines

		
	def generate_subtype( intf, subt, prefix=None ) :

		def _text ( r ) :
			if isinstance(r,SIDL_RawText):		return r.GetText()
			else:								return r

		def _enum_code ( ) :
			lines = []
			if subt.identifier:		lines.append( 'enum %s\n{\n' % subt.identifier )
			else:					lines.append( 'enum\n{\n' )
			le = 0;
			for dv in atmake.ToList(subt.declarations) :
				if atmake.IsTuple(dv):		v0,v1 = _text(dv[0]), _text(dv[1])
				else:				v0,v1 = _text(dv), None
				le = max(len(v0),le)
			i = 0
			for dv in atmake.ToList(subt.declarations) :
				if atmake.IsTuple(dv):		v0,v1 = _text(dv[0]), _text(dv[1])
				else:				v0,v1 = _text(dv), None
				if v1 and v1.isdigit():
					i = int(v1)
					lines.append( '\t%s = (%d),\n'%(v0.ljust(le),i) )
					i+=1
				elif v1 :
					lines.append( '\t%s = (%s),\n'%(v0.ljust(le),v1) )
					i = None
				elif not i is None :
					lines.append( '\t%s = (%d),\n'%(v0.ljust(le),i) )
					i+=1
				else :
					lines.append( '\t%s,\n'%v0.ljust(le) )
			lines.append( '};\n' )
			return lines

		def _compound_code ( cpp_specifier ) :
			inner_lines = []
			inner_funcs = []
			inner_type = None

			# check for a getbase() function for root delegate
			if subt.specifier=='delegate' and not subt.super :
				if not 'getbase' in [f.name for f in subt.Filter(SIDL_Function)] :
					inner_funcs.append( SIDL_Function(name='getbase',rtype='void*',qualifier='virtual') )

			for dv in atmake.ToList(subt.declarations) :
				if isinstance(dv,SIDL_Function) :
					inner_funcs.append( dv )
				elif isinstance(dv,SIDL_RawText) :
					if inner_type and inner_type!='raw':	inner_lines.append( '\n' )
					inner_lines.extend( ['%s\n'%l for l in _text(dv).splitlines()] )
					inner_type = 'raw'
				elif atmake.IsString(dv) and dv=='@classid' :
					if inner_lines:		inner_lines.append( '\n' )
					inner_lines.extend( 'static const classid ClassId = %s;\n' % genuid_hex(subt.identifier) )
					inner_type = dv
				elif atmake.IsString(dv) and dv=='@getclassid' :
					if inner_lines:		inner_lines.append( '\n' )
					getcid = SIDL_Function(rtype='classid',name='GetClassId')
					inner_funcs.append( getcid )
					if subt.super:	getcid.SetCode( 'return ClassId;' )
					else:			getcid.DefaultQualifier('virtual')
					inner_type = dv
				elif atmake.IsString(dv) and dv=='@cdtor' :
					ctor = SIDL_Function(rtype='/**/',name='%s'%subt.identifier,code='/**/')
					dtor = SIDL_Function(rtype='/**/',name='~%s'%subt.identifier,qualifier='virtual',code='/**/')
					inner_funcs.append( ctor )
					inner_funcs.append( dtor )
					inner_type = dv
				elif atmake.IsTuple(dv) and len(dv)==2 :
					vt, vid = _text(dv[0]), _text(dv[1])
					if inner_type and inner_type!='var':	inner_lines.append( '\n' )
					if '%%' in vt:	inner_lines.append( '%s;\n' % vt.replace('%%',vid) )
					else:			inner_lines.append( '%s %s;\n' % (vt,vid) )
					inner_type = 'var'
					# to manage an array as a raw type, use the form: (char %%[16]) shaderprofile
					# as a result, it will be substituted by: char shaderprofile[16]
				elif isinstance(dv,SIDL_Typedef) :
					if inner_lines:		inner_lines.append( '\n' )
					inner_lines.extend( generate_subtype(intf,dv) )
					inner_type = 'typedef'

			if inner_funcs:
				if inner_lines:		inner_lines.append( '\n' )
				inner_lines.extend( SIDL_Function_GetCppLines(functions=inner_funcs,aligncode=True) )

			lines = []
			inherits = ' : public %s '%subt.super if subt.super else ''
			lines.append( '%s %s%s\n{\n' % (cpp_specifier,(subt.identifier or ''),inherits) )
			lines.extend( ['\t%s\n'%l for l in ''.join(inner_lines).splitlines()] )
			lines.append( '};\n' )
			return lines

		def _type_code ( ) :
			# to manage a C function pointer, use the form: @type Func=(void %%(int a))
			assert atmake.IsList(subt.declarations)
			if len(subt.declarations):	tval = _text( subt.declarations[0] )
			else:						tval = None
			if not atmake.IsString(tval) :
				return 'struct %s;\n' % subt.identifier
			elif '%%' in tval:
				return 'typedef %s;' % tval.replace('%%',subt.identifier)
			else:
				return 'typedef %s %s;' % (tval,subt.identifier)

		lines = []
		if   subt.specifier=='enum':		lines = _enum_code()
		elif subt.specifier=='type':		lines = _type_code()
		elif subt.specifier=='union':		lines = _compound_code('union')
		elif subt.specifier=='struct':		lines = _compound_code('struct')
		elif subt.specifier=='delegate':	lines = _compound_code('struct')
		if lines and prefix:
			lines = ['%s%s\n'%(prefix,l) for l in ''.join(lines).splitlines() ]
		return lines


	def generate_define ( sym, value, prefix='' ) :
		lines = []
		sym_noargs = sym.split('(')[0]
		lines.append( '%s#ifndef %s\n' % (prefix,sym_noargs) )
		lines.append( '%s#define %s %s\n' % (prefix,sym,value) )
		lines.append( '%s#endif\n\n' % prefix )
		return lines


	def generate_itf_bridge_code ( impl, intf, f, bridges ) :
		# bridges are [ (concernid,interfaceid,funcname,value) ]
		#	with value is SIDL_RawText
		code = None
		for b in bridges :
			concern, cname, fname, raw = b
			if fnmatch.fnmatch(intf.classbase,concern) :
				if fnmatch.fnmatch(f.declname,cname) :
					if fnmatch.fnmatch(f.name,fname) :
						code = raw
		assert( code )

		if isinstance(code,SIDL_RawText) :
			code_text = code.GetText()
			code_env  = code.env
		else :
			code_text = code
			code_env  = impl.env

		args = []
		for a in f.args :
			args.append( (a[0],'p%d'%len(args)) )

		g = {}
		g['return']		= '' if f.rtype=='void' else 'return'
		g['rtype']		= f.rtype
		g['fname']		= f.name
		g['fparams']	= ','.join([a[1] for a in args])
		g['iclassname'] = intf.classname
		g['mclassname'] = impl.classname
		g['sclassname']	= 'SIDL_STUB_CLASSNAME'
		g['intf']		= '%s_i' % intf.classname
		g['skel']		= '(*m)'
		g['stub']		= '(m->stub)'

		def _subst_code ( code_text ) :
			_t = code_env.Subst( code_text, moreglobals=g )
			_t = _t.strip()
			while ' '*2 in _t :
				_t = _t.replace( ' '*2, ' '*1 )
			if not _t.endswith(';') :
				_t += ';'
			return _t

		code_using_m     = _subst_code( '$mclassname* m = SU_DELEGATE_BASEPTR($mclassname,$intf,this); %s' % code_text )
		code_not_using_m = _subst_code( code_text )
		code_check_m     = code_not_using_m.replace(' ','')
		for m in '*m m* m-> m+ (m, ,m) ,m,'.split() :
			if m in code_check_m :
				return code_using_m

		return code_not_using_m


	def generate_itf_bridge_call ( impl, intf, f, bridges, fmt1, fmt2 ) :
		code = generate_itf_bridge_code( impl, intf, f, bridges )
		args = [ '%s p%d'%(a[0],i) for i,a in enumerate(f.args) ]
		if f.hasellipsis:	args.append( '...' )
		call  = '%s  %s ( ' % ( f.rtype.ljust(fmt1), f.name.ljust(fmt2) )
		call += ','.join(args)
		call += ' ) { %s }' % code
		return call




	def generate_delegate_lines ( impl ) :

		def _d_funcs ( itf, d ) :
			fl = d.Filter( SIDL_Function )
			if d.super :
				s = itf.FindSubType( d.super )
				if s:	fl = _d_funcs(itf,s) + fl
			return fl

		def _d_types ( itf, t ) :
			for sep in ['*','&','(',')','[',']'] :
				a = t.split(sep)
				if len(a)==2 :
					return _d_types(itf,a[0])+sep+_d_types(itf,a[1])
			for s in itf.subtypes :
				if t==s.identifier :
					return '%s::%s' % (itf.classname,s.identifier)
			return t

		lines = []

		for itf in impl.concerns :
			dl = [x for x in itf.subtypes if x.specifier=='delegate']
			for d in dl :
				fl = _d_funcs(itf,d)
				cn = '%s_%s' % (itf.classname,d.identifier)
				lines.append( '#define %s_delegate(REF,DLG) \\\n' % cn )
				lines.append( '\t\tstruct %s_delegate_t : public su::%s::%s { \\\n' % (cn,itf.classname,d.identifier) )
				if not 'getbase' in [f.name for f in fl] :
					lines.append( '\t\t\tvoid* getbase() { return SU_DELEGATE_BASEPTR(REF,DLG,this); } \\\n' )
				for f in fl :
					args1 = [ '%s p%d'%(_d_types(itf,a[0]),i) for i,a in enumerate(f.args) ]
					args2 = [ 'p%d'%i for i,a in enumerate(f.args) ]
					if f.hasellipsis:	args1.append( '...' )
					ret   = '' if f.rtype=='void' else 'return '
					rtype = _d_types(itf,f.rtype)
					call  = '%s %s(%s) { ' % (rtype,f.name,','.join(args1))
					call += '%s((REF*)getbase())->%s(%s); ' % (ret,f.name,','.join(args2))
					call += '} '
					lines.append( '\t\t\t%s \\\n' % call )
					
				lines.append( '\t\t} DLG;' )
				lines.append( '\n'*2 )

		return lines




	def get_stub_file ( obj ) :
		if obj :
			proper = collect_section( obj, 'proper' )
			stub   = collect_section( obj, 'stub' )
			if not proper and not stub :
				return obj.GetStubOutpath()
			if stub and len(stub)==1 and stub[0].file and fs.IsAbsPath(stub[0].file) :
				return stub[0].file




	def generate_stub_lines ( obj ) :
		bridges  = collect_bridges( obj )
		concerns = collect_concerns( obj )

		lines = []
		lines.append( header )
		lines.append( '\n'*2 )
		lines.append( '\nusing namespace su;\n' )
		lines.append( '\n'*2 )
		lines.append( '\n\nnamespace\n{\n\n' )

		# -- FSM states

		if obj.IsStateMachine() :
			for i,s in enumerate(obj.states) :
				lines.append( '\t//---- state %s -- \n\n' % s['uri'] )
				lines.append( '\tSIDL_IMPL_STATE( %s )\n\t{\n' % s['uri'].replace('.','_') )
				lines.append( '\t};\n\n\n' )
			lines.append( '\n'*4 )

		# -- stub

		if atmake.IsClassof(obj,SIDL_Abstract) :
			lines.append( '\tstruct SIDL_STUB_SUPERNAME\n\t{\n\n' )
			lines.append( '\t\tbool Init ( Pool* p, AnyList* o )\n\t\t{\n' )
			lines.append( '\t\t\t// Your code here.\n' )
			lines.append( '\t\t\treturn true;\n' )
			lines.append( '\t\t}\n\n\n' )
			lines.append( '\t\tvoid Shut ( )\n\t\t{\n' )
			lines.append( '\t\t\t// Your code here.\n' )
			lines.append( '\t\t}\n\n\n' )
		elif obj.abstract :
			lines.append( '\tstruct SIDL_STUB_CLASSNAME : public SIDL_STUB_SUPERNAME\n\t{\n\n' )
			lines.append( '\t\ttypedef SIDL_STUB_SUPERNAME abstract;\n\n' )
			lines.append( '\t\tbool Init ( Pool* p, AnyList* o )\n\t\t{\n' )
			lines.append( '\t\t\t// Your code here.\n' )
			lines.append( '\t\t\treturn abstract::Init(p,o);\n' )
			lines.append( '\t\t}\n\n\n' )
			lines.append( '\t\tvoid Shut ( )\n\t\t{\n' )
			lines.append( '\t\t\t// Your code here.\n' )
			lines.append( '\t\t\tabstract::Shut();\n' )
			lines.append( '\t\t}\n\n\n' )
		else :
			lines.append( '\tstruct SIDL_STUB_CLASSNAME\n\t{\n\n' )
			lines.append( '\t\tbool Init ( Pool* p, AnyList* o )\n\t\t{\n' )
			lines.append( '\t\t\t// Your code here.\n' )
			lines.append( '\t\t\treturn true;\n' )
			lines.append( '\t\t}\n\n\n' )
			lines.append( '\t\tvoid Shut ( )\n\t\t{\n' )
			lines.append( '\t\t\t// Your code here.\n' )
			lines.append( '\t\t}\n\n\n' )
		fcache = {}
		for c in concerns :
			for f in collect_funcs(c) :
				code = generate_itf_bridge_code( obj, c, f, bridges )
				mang = f.GetMangled()
				if not mang in fcache :
					fcache[mang] = 1
					# is this function redirected to the stub code ?
					if 'stub' in code :
						if c.classname=='IStateMachine' :
							fcode = 'SIDL_IMPL_AUTOMATON( %s(%s) );' % (f.name.lower(),','.join([a[1] for a in f.args]) )
							if f.rtype!='void': fcode = 'return '+fcode
							flines = SIDL_Function_WithCode( f, fcode, indent='\t\t' )
						else:
							flines = SIDL_Function_YourCodeHere( f, indent='\t\t' )
						lines.extend( flines )
						lines.append( '\n'*2 )
		lines.append( '\t};\n\n\n}\n\n\n' )
		return lines


		

	def generate_impl_lines ( impl ) :

		def state_id(u) :
			for i,s in enumerate(impl.states) :
				if s['uri']==u :
					return i

		def state_attr(s,n) :
			for ak,av in s['attributes'].items() :
				if ak==n:
					return av

		def state_start(s) :
			if state_attr(s,'start'):	return True
			else:						return False

		def state_next_id(s,i) :
			n = state_attr(s,'next')
			x = len(impl.states) - 1
			if n:
				ni = state_id(n)
				if not ni :
					SIDL_Error( 'Undefined next state <%s> referenced in state <%s> !'%(n,s['uri']), impl.loc )
				return ni
			elif i<x:
				return (i+1)%(x+1)
			else:
				return -1

		def state_super_id(s) :
			p = '.'.join(s['uri'].split('.')[:-1])
			if p :
				pi = state_id(p)
				if not pi :
					SIDL_Error( 'Undefined super state <%s> referenced in state <%s> !'%(p,s['uri']), impl.loc )
				return pi
			else :
				return -1

		output = []

		outmain = SIDL_Outputfile( path=impl.GetOutpath(), source=impl )
		output.append( outmain )

		proper			= collect_section( impl, 'proper' )
		code			= collect_section( impl, 'code' )
		stub			= collect_section( impl, 'stub' )
		builder			= collect_section( impl, 'builder' )
		delegate		= generate_delegate_lines( impl )
		depfiles		= collect_dep_files( impl )
		concerns		= collect_concerns( impl )
		switches		= collect_switches( impl, concerns )
		attributes		= collect_attributes( impl )
		bridges			= collect_bridges( impl )
		stubfile		= get_stub_file( impl )
		absstubfile		= get_stub_file( impl.abstract )


		# complete headers
		if impl.IsStateMachine():		depfiles = ['<sufw/logic/su_logic.h>'] + depfiles
		if impl.MatchScope('su.fw.*'):	depfiles = ['<sufw/precompiled/supch_core.h>'] + depfiles
		else:							depfiles = ['<sufw/su_core.h>'] + depfiles
		if 1:							depfiles.append( '<stdarg.h>' )


		# default stub if not stub and not proper
		if not proper and not stub :
			stub = []
			if absstubfile:		stub.append( SIDL_RawText(file=absstubfile) )
			if 1:				stub.append( SIDL_RawText(file=stubfile) )


		#
		# abstract & implementation stub
	
		if absstubfile :
			# stub is an user file, intended to be hand-written
			outabsstub = SIDL_Outputfile( path=absstubfile, source=impl.abstract, cleanable=False, readonly=False )
			outabsstub.Append( generate_stub_lines(impl.abstract) )
			output.append( outabsstub )

		if stubfile :
			# stub is an user file, intended to be hand-written
			outstub = SIDL_Outputfile( path=stubfile, source=impl, cleanable=False, readonly=False )
			outstub.Append( generate_stub_lines(impl) )
			output.append( outstub )


		#
		# -- header

		outmain.Append( header )
		outmain.Append( '\n'*2 )
		outmain.Append( warnings )
		outmain.Append( '\n'*3 )


		#
		# -- full set of headers

		if len(depfiles) :
			outmain.MainSeparator( 'includes' )
			for f in depfiles :
				ifile = generate_inc_file( impl, f )
				outmain.Append( '#include %s\n' % ifile )
		outmain.Newline( 4 )


		#
		# -- some defines ...

		if 1 :
			outmain.MainSeparator( 'defines' )
			packg_uri       = impl.loc.package
			stubclassname   = impl.classname+'_Stub'
			stubsupername   = impl.abstract.classname+'_Stub' if impl.abstract else None
			revno           = attributes.get( 'revision', None )
			if 1:						outmain.Append( generate_define('SIDL_IMPL_CLASSNAME',impl.classname) )
			if impl.supername :			outmain.Append( generate_define('SIDL_IMPL_SUPERNAME',impl.supername) )
			if 1:						outmain.Append( generate_define('SIDL_IMPL_MUID','%s'%genuid_hex(impl.classbase)) )
			if impl.qualifier :			outmain.Append( generate_define('SIDL_IMPL_QUALIFIER',str(impl.qualifier) ) )
			if impl.qualifier :			outmain.Append( generate_define('SIDL_IMPL_IS_%s'%str(impl.qualifier).upper(),'') )
			if impl.IsSwitched() :		outmain.Append( generate_define('SIDL_IMPL_SWITCHED','') )
			if impl.IsStateMachine() :	outmain.Append( generate_define('SIDL_IMPL_FSM','') )
			if impl.IsStateMachine() :	outmain.Append( generate_define('SIDL_IMPL_STATE(NAME)','struct %s_state_##NAME##_t : public su::fsm::State'%impl.classname) )
			if impl.IsStateMachine() :	outmain.Append( generate_define('SIDL_IMPL_AUTOMATON(CALL)','((%s_automaton(this))->CALL)'%impl.classname) )
			if atmake.IsDefined(revno):	outmain.Append( generate_define('SIDL_IMPL_REVISION','SIDL_IMPL_ATTR_REVISION') )
			else :						outmain.Append( generate_define('SIDL_IMPL_REVISION','0') )
			if packg_uri :				outmain.Append( generate_define('SIDL_IMPL_PACKAGE','"%s"'%packg_uri) )
			if len(attributes):			outmain.Append( generate_define('SIDL_IMPL_ATTR_V','sidl_attributes') )
			else:						outmain.Append( generate_define('SIDL_IMPL_ATTR_V','NULL') )
			if 1:						outmain.Append( generate_define('SIDL_IMPL_ATTR_C',str(len(attributes))) )
			if 1:						outmain.Append( generate_define('SIDL_STUB_CLASSNAME',stubclassname) )
			if stubsupername :			outmain.Append( generate_define('SIDL_STUB_SUPERNAME',stubsupername) )
			if 1:						outmain.Append( generate_define('SIDL_IMPL_INTERFACE(I)','((I*)%s_get_interface(%s_stub_to_base(this),I::ClassId))'%(impl.classname,impl.classname)) )
			if len(attributes) :
				cpp_attributes = []
				cpp_attributes.append( 'namespace\n{\n' )
				cpp_attributes.append( '\tconst char* sidl_attributes[]=\n\t{\n' )
				for ak,av in attributes.items() :
					asym = 'SIDL_IMPL_ATTR_%s' % ak.upper()
					outmain.Append( generate_define(asym,atmake.ToString(av)) )
					cpp_attributes.append( '\t\t"%s", "%s",\n' % (ak,str(av)) )
				cpp_attributes.append( '\t};\n}\n' )
				outmain.Newline( 2 )
				outmain.Append( cpp_attributes )
			outmain.Newline( 2 )


		#
		# -- subtype's delegate definitions

		if len(delegate) :
			outmain.MainSeparator( 'delegates' )
			outmain.Append( delegate )
			outmain.Newline( 4 )


		#
		# -- declarations

		if 1 :
			outmain.MainSeparator( 'declarations' )
			outmain.Append( 'namespace {\n\n' )
			# %m_create_object()
			outmain.Append( 'su::IInterface* %s_create_object( su::Pool*, su::Pool*, su::AnyList* );\n' % impl.classname )
			# %m_base_to_stub
			outmain.Append( 'void* %s_base_to_stub ( void* );\n' % impl.classbase )
			# %m_stub_to_base
			outmain.Append( 'void* %s_stub_to_base ( void* );\n' % impl.classbase )
			# %m_builder
			outmain.Append( 'su::addin::Builder* %s_builder ( );\n' % impl.classbase )
			# %m_count_interface
			outmain.Append( 'uint %s_count_interface ( );\n' % impl.classbase )
			# %m_enum_interface
			outmain.Append( 'bool %s_enum_interface ( uint, su::classid*, pcstr* );\n' % impl.classbase )
			# %m_first_interface
			outmain.Append( 'su::IInterface* %s_first_interface ( void* );\n' % impl.classbase )
			# %m_get_interface
			outmain.Append( 'su::IInterface* %s_get_interface ( void*, su::classid );\n' % impl.classbase )
			# switch declarations
			if impl.IsSwitched():
				# %m_count_switch()
				outmain.Append( 'uint %s_count_switch ( );\n' % impl.classbase )
				# %m_enum_switch
				outmain.Append( 'bool %s_enum_switch ( uint, su::ISwitch::Desc& );\n' % impl.classbase )
				# %m_find_switch
				outmain.Append( 'int %s_find_switch ( pcstr );\n' % impl.classbase )
				# %m_call_switch
				outmain.Append( 'bool %s_call_switch_stack ( void*, uint, void*, va_list );\n' % impl.classbase )
				outmain.Append( 'bool %s_call_switch_mem ( void*, uint, su::Any*, int, su::Any* );\n' % impl.classbase )
			# FMS declarations
			if impl.IsStateMachine() :
				outmain.Append( 'enum {\n' )
				for i,s in enumerate(impl.states) :
					if i==0:
						outmain.Append( '\t%s_state_count = %s,\n' % (impl.classbase,len(impl.states)) )
					prefx = '%s_state_%s' % (impl.classbase, s['uri'].replace('.','_'))
					outmain.Append( '\t%s = %s,\n' % (prefx,i) )
					outmain.Append( '\t%s_next = %s,\n' % (prefx,state_next_id(s,i)) )
					outmain.Append( '\t%s_super = %s,\n' % (prefx,state_super_id(s)) )
					for ak,av in s['attributes'].items() :
						if ak=='goto' :
							outmain.Append( '\t%s_%s = %s,\n' % (prefx,av[0],state_id(av[1])) )
				outmain.Append( '};\n' )
				# %m_automaton
				outmain.Append( 'su::fsm::Automaton* %s_automaton ( void* ); // from stub\n' % impl.classbase )
			outmain.Append( '\n}\n' )
			outmain.Append( '\n'*2 )


		#
		# -- proper part

		if len(proper) :
			outmain.MainSeparator( 'proper area' )
			outmain.Append( generate_section(proper,'proper',obj=impl) )
			outmain.Append( '\n'*4 )


		#
		# -- stub part

		if len(stub) :
			outmain.MainSeparator( 'stub area' )
			outmain.Append( generate_section(stub,'stub',obj=impl) )
			outmain.Append( '\n'*4 )


		#
		# -- implementation . start

		outmain.MainSeparator( 'skeleton' )
		outmain.Append( 'namespace {\n' )
		outmain.Append( '\n'*2 )
		impl_supername = ' : public %s\n' % impl.supername if impl.supername else ''
		outmain.Append( 'struct %s%s\n' % (impl.classbase,impl_supername) )
		outmain.Append( '{\n\n' )


		#
		# -- implementation . bridges

		outmain.SubSeparator( "bridges" )
		for c in concerns :
			outmain.Append( 'struct %s_t : public su::%s {\n' % (c.classbase,c.classbase) )
			c_funcs = collect_funcs(c)
			c_rtype_len = 0
			c_fname_len = 0
			for f in c_funcs :
				c_rtype_len = max( c_rtype_len, len(f.rtype) )
				c_fname_len = max( c_fname_len, len(f.name) )
			curclass = None
			for f in c_funcs :
				if f.declname != curclass :
					curclass = f.declname
					outmain.Append( '\t// from %s\n' % (curclass or '?') )
				b = generate_itf_bridge_call( impl, c, f, bridges, c_rtype_len, c_fname_len )
				outmain.Append( '\t%s\n'%b )
			outmain.Append( '} %s_i;\n' % c.classbase )
			outmain.Append( '\n'*2 )


		#
		# -- implementation . stub

		outmain.SubSeparator( 'stub nested object' )
		if impl.IsStateMachine() :
			outmain.Append( '\nstruct Automaton_t : su::fsm::Automaton {\n\n' )
			for i,s in enumerate(impl.states) :
				outmain.Append( '\t%s_state_%s_t s%d;\n' % (impl.classname,s['uri'].replace('.','_'),i) )
			# ctor
			outmain.Append( '\n\tAutomaton_t ( )\n\t{\n' )
			outmain.Append( '\t\tinit(NULL);\n' )
			outmain.Append( '\t}\n\n' )
			# dtor
			outmain.Append( '\tvirtual ~Automaton_t ( )\n\t{\n' )
			outmain.Append( '\t\tshut();\n' )
			outmain.Append( '\t}\n\n' )
			# countstate
			outmain.Append( '\tint countstate ( )\n\t{\n' )
			outmain.Append( '\t\treturn %d;\n' % len(impl.states) )
			outmain.Append( '\t}\n\n' )
			# enumstate
			outmain.Append( '\tsu::fsm::StateDesc enumstate ( int id )\n\t{\n' )
			outmain.Append( '\t\tSU_ASSERT(id<%d);\n' % len(impl.states) )
			outmain.Append( '\t\tsu::fsm::StateDesc d;\n' )
			for i,s in enumerate(impl.states) :
				if i==0 :	outmain.Append( '\t\tif' )
				else:		outmain.Append( '\t\telse if' )
				outmain.Append( '(id==%s) { d.obj=&s%s; d.name="%s"; d.superid=%s; d.nextid=%s; d.startpoint=%s; }\n' % (i,i,s['uri'],state_super_id(s),state_next_id(s,i),str(state_start(s)).lower()) )
			outmain.Append( '\t\treturn d;\n' )
			outmain.Append( '\t}\n\n' )
			#
			outmain.Append( '} automaton;\n\n' )
			outmain.Append( '\n'*2 )
		outmain.Append( '%s stub;\n' % stubclassname )
		outmain.Append( '\n'*2 )


		#
		# -- implementation . code

		outmain.SubSeparator( 'skeleton management code area' )
		if len(code) :
			# user code here
			outmain.Append( generate_section(code,'code',obj=impl) )
		else:
			# default refcount
			outmain.Append( 'int refcount;\n' )
			if not impl.qualifier=='static' :
				outmain.Append( 'Pool* nestp;\n' )
			outmain.Append( '\n'*2 )
			# default Retain()
			outmain.Append( 'void Retain ( )\n' )
			outmain.Append( '{\n' )
			outmain.Append( '\trefcount++;\n' )
			outmain.Append( '}\n' )
			outmain.Append( '\n'*2 )
			# default CountRef()
			outmain.Append( 'uint CountRef ( )\n' )
			outmain.Append( '{\n' )
			outmain.Append( '\treturn refcount;\n' )
			outmain.Append( '}\n' )
			outmain.Append( '\n'*2 )
			# default Release()
			outmain.Append( 'void Release ( )\n' )
			outmain.Append( '{\n' )
			outmain.Append( '\tif( refcount==1 )\n' )
			outmain.Append( '\t\tDestroy( this );\n' )
			outmain.Append( '\telse\n' )
			outmain.Append( '\t\trefcount--;\n' )
			outmain.Append( '}\n' )
			outmain.Append( '\n'*2 )
			# default CountInterface()
			outmain.Append( 'uint CountInterface ( )\n{\n\treturn %s_count_interface();\n}\n' % impl.classbase )
			outmain.Append( '\n'*2 )
			# default EnumInterface()
			outmain.Append( 'bool EnumInterface ( uint i, su::classid* cid, pcstr* cname )\n{\n\treturn %s_enum_interface(i,cid,cname);\n}\n' % impl.classname )
			outmain.Append( '\n'*2 )
			# default GetInterface()
			outmain.Append( 'su::IInterface* GetInterface ( classid cid )\n' )
			outmain.Append( '{\n' )
			for c in concerns :
				outmain.Append( '\tSU_IMPLEMENTS_MEMBER( %s_i, cid );\n' % c.classbase )
			outmain.Append( '\treturn NULL;\n' )
			outmain.Append( '}\n' )
			outmain.Append( '\n'*2 )
			# default Create()
			outmain.Append( 'static %s* Create( Pool* np, Pool* dp, AnyList* o )\n{\n' % impl.classbase )
			if impl.qualifier=='static' :
				outmain.Append( '\tstatic %s m;\n' % impl.classbase )
				outmain.Append( '\tstatic bool yet = false;\n' )
				outmain.Append( '\tif( !yet )\n\t{\n\t\tm.refcount = 0;\n\t\tyet = true;\n\t}\n' )
				outmain.Append( '\tif( m.refcount==0 )\n\t{\n' )
				outmain.Append( '\t\tif( !m.stub.Init(dp,o) )\n' )
				outmain.Append( '\t\t\treturn NULL;\n' )
				outmain.Append( '\t}\n' )
				outmain.Append( '\tm.refcount += 1;\n' )
				outmain.Append( '\treturn &m;\n' )
			else :
				outmain.Append( '\tif( !np )\treturn NULL;\n' )
				outmain.Append( '\t%s* m = np->New<%s>();\n' % (impl.classbase,impl.classbase) )
				outmain.Append( '\tif( !m )\treturn NULL;\n' )
				outmain.Append( '\tm->refcount = 1;\n' )
				outmain.Append( '\tm->nestp = np;\n' )
				outmain.Append( '\tif( !m->stub.Init(dp,o) )\n\t{\n' )
				outmain.Append( '\t\tnp->Delete( m );\n' )
				outmain.Append( '\t\treturn NULL;\n' )
				outmain.Append( '\t}\n' )
				outmain.Append( '\treturn m;\n' )
			outmain.Append( '}\n' )
			outmain.Append( '\n'*2 )
			# default Destroy()
			outmain.Append( 'static void Destroy( %s* m )\n{\n' % impl.classbase )
			outmain.Append( '\tm->stub.Shut();\n' )
			outmain.Append( '\tm->refcount=0;\n' )
			if not impl.qualifier=='static' :
				outmain.Append( '\tm->nestp->Delete( m );\n' )
			outmain.Append( '}\n' )


		#
		# -- implementation . end

		outmain.Append( '\n};\n' )
		outmain.Append( '\n' )
		outmain.Append( '}\n' )
		outmain.Append( '\n'*2 )


		#
		# -- builder

		outmain.Newline( 4 )
		outmain.MainSeparator( 'builder' )
		outmain.Newline( 2 )
		if builder :
			outmain.Append( generate_section(builder,'builder',obj=impl) )
		else :
			outmain.Append( 'SU_BEGIN_BUILDER( %s )\n\n' % impl.classbase )
			# GetMUID()
			outmain.Append( 'su::classid GetMUID ( ) { return SIDL_IMPL_MUID; }\n' )
			# GetMClassname()
			outmain.Append( 'pcstr GetMClassname ( ) { return "%s"; }\n' % impl.classbase )
			# GetRevision()
			outmain.Append( 'uint32 GetRevision ( ) { return SIDL_IMPL_REVISION; }\n' )
			# GetPackage()
			outmain.Append( 'pcstr GetPackage ( ) { return SIDL_IMPL_PACKAGE; }\n' )
			# CountInterface()
			outmain.Append( 'uint CountInterface ( ) { return %s_count_interface(); }\n' % impl.classbase )
			# EnumInterface()
			outmain.Append( 'bool EnumInterface ( uint i, su::classid* cid, pcstr* cname ) { return %s_enum_interface(i,cid,cname); }\n' % impl.classname )
			# FirstInterface()
			outmain.Append( 'su::IInterface* FirstInterface ( void* base ) { return %s_first_interface(base); }\n' % impl.classname )
			# GetInterface()
			outmain.Append( 'su::IInterface* GetInterface ( void* base, su::classid cid ) { return %s_get_interface(base,cid); }\n' % impl.classname )
			# CreateObject()
			outmain.Append( 'su::IInterface* CreateObject( su::Pool* nestp, su::Pool* datap, su::AnyList* opts ) { return %s_create_object(nestp,datap,opts); }\n' % impl.classname )
			# BaseToStub()
			outmain.Append( 'void* BaseToStub ( void* base ) { return %s_base_to_stub(base); }\n' % impl.classname )
			# StubToBase()
			outmain.Append( 'void* StubToBase ( void* base ) { return %s_stub_to_base(base); }\n' % impl.classname )
			# CountSwitch()
			if impl.IsSwitched():
				outmain.Append( 'uint CountSwitch ( ) { return %s_count_switch(); }\n' % impl.classbase )
			else:
				outmain.Append( 'uint CountSwitch ( ) { return 0; }\n' )
			# EnumSwitch()
			if impl.IsSwitched():
				outmain.Append( 'bool EnumSwitch ( uint i, su::ISwitch::Desc& desc ) { return %s_enum_switch(i,desc); }\n' % impl.classname )
			else:
				outmain.Append( 'bool EnumSwitch ( uint, su::ISwitch::Desc& ) { return false; }\n' )
			# close builder
			outmain.Append( '\nSU_END_BUILDER( %s )\n\n' % impl.classbase )


		#
		# -- definitions

		outmain.Newline( 4 )
		outmain.MainSeparator( 'definitions' )
		outmain.Newline( 2 )
		outmain.Append( 'namespace {\n\n\n' )
		# %m_create_object()
		outmain.Append( 'su::IInterface* %s_create_object( su::Pool* nestp, su::Pool* datap, su::AnyList* opts )\n{\n' % impl.classname )
		if concerns :
			outmain.Append( '\t%s* m = %s::Create( nestp, datap, opts );\n' % (impl.classname,impl.classname) )
			outmain.Append( '\tSU_REQUIRE_OR_NULL( m );\n' )
			outmain.Append( '\treturn &m->%s_i;\n' % concerns[0].classname )
		else :
			outmain.Append( '\treturn NULL;\n' )
		outmain.Append( '}\n' )
		outmain.Append( '\n'*2 )
		# %m_base_to_stub()
		outmain.Append( 'void* %s_base_to_stub ( void* base )\n{\n' % impl.classname )
		outmain.Append( '\tSU_REQUIRE_OR_NULL( base );\n' )
		outmain.Append( '\treturn &(((%s*)base)->stub);\n' % impl.classname )
		outmain.Append( '}\n' )
		outmain.Append( '\n'*2 )
		# %m_stub_to_base()
		outmain.Append( 'void* %s_stub_to_base ( void* stubp )\n{\n' % impl.classname)
		outmain.Append( '\tSU_REQUIRE_OR_NULL( stubp );\n' )
		outmain.Append( '\treturn SU_DELEGATE_BASEPTR(%s,stub,stubp);\n' % impl.classname )
		outmain.Append( '}\n' )
		outmain.Append( '\n'*2 )
		# %m_builder
		outmain.Append( 'su::addin::Builder* %s_builder ( )\n{\n\treturn &_builder_%s;\n}\n' % (impl.classbase,impl.classbase) )
		outmain.Append( '\n'*2 )
		# %m_count_interface
		outmain.Append( 'uint %s_count_interface ( )\n{\n\treturn %d;\n}\n' % (impl.classbase,len(concerns)) )
		outmain.Append( '\n'*2 )
		# %m_enum_interface
		outmain.Append( 'bool %s_enum_interface ( uint i, su::classid* cid, pcstr* cname )\n{\n' % (impl.classbase) )
		if concerns :
			cids   = [ 'su::%s::ClassId'%c.classbase for c in concerns ]
			cnames = [ '"%s"'%c.classbase              for c in concerns ]
			outmain.Append( '\tstatic const su::classid cids[] = { %s };\n' % (', '.join(cids)) )
			outmain.Append( '\tstatic const pcstr cnames[] = { %s };\n' % (', '.join(cnames)) )
			outmain.Append( '\tSU_REQUIRE_OR_FALSE( i<%d );\n' % len(concerns) )
			outmain.Append( '\tif( cid )   *cid   = cids[i];\n' )
			outmain.Append( '\tif( cname ) *cname = cnames[i];\n' )
			outmain.Append( '\treturn true;\n' )
		else :
			outmain.Append( '\treturn false;\n' )
		outmain.Append( '}\n' )
		outmain.Append( '\n'*2 )
		# %m_first_interface()
		outmain.Append( 'su::IInterface* %s_first_interface ( void* base )\n{\n' % impl.classname )
		outmain.Append( '\tSU_REQUIRE_OR_NULL( base );\n' )
		outmain.Append( '\treturn &(((%s*)base)->%s_i);\n' % (impl.classname,concerns[0].classname) )
		outmain.Append( '}\n' )
		outmain.Append( '\n'*2 )
		# %m_get_interface()
		outmain.Append( 'su::IInterface* %s_get_interface ( void* base, su::classid cid )\n{\n' % impl.classname )
		outmain.Append( '\tSU_REQUIRE_OR_NULL( base );\n' )
		outmain.Append( '\treturn ((%s*)base)->GetInterface(cid);\n' % impl.classname )
		outmain.Append( '}\n' )
		outmain.Append( '\n'*2 )
		# switch definitions
		if impl.IsSwitched():
			# %m_count_switch()
			outmain.Append( 'uint %s_count_switch ( )\n{\n\treturn %d;\n}\n' % (impl.classbase,len(switches)) )
			outmain.Append( '\n'*2 )
			# %m_enum_switch
			outmain.Append( 'bool %s_enum_switch ( uint i, su::ISwitch::Desc& desc )\n{\n' % impl.classbase )
			if switches :
				outmain.Append( '\tstatic su::ISwitch::Desc swdesc[] = {\n' )
				for s in switches :
					# s is (func,concern)
					sf = s[0].NoScope()
					types = [sf.rtype]+[a[0] for a in sf.args]
					etypes = [gettype_func_arg(t) for t in types]
					if None in etypes:	etypes = None
					sw_concern = '"%s"' % s[1].classname
					sw_name    = '"%s"' % (sf.switchname or sf.name)
					sw_func    = '"%s"' % sf.name
					sw_sign    = '"%s"' % ','.join(types).replace(' ','')
					sw_types   = ('"%s"' % ''.join(etypes)) if etypes else 'NULL'
					sw_mark    = ('"%s"' % sf.mark) if sf.mark else 'NULL'
					outmain.Append( '\t\t{ %s, %s, %s, %s, %s, %s },\n' % (sw_name,sw_func,sw_sign,sw_types,sw_concern,sw_mark) )
				outmain.Append( '\t\t};\n' )
				outmain.Append( '\tSU_REQUIRE_OR_FALSE( i<%s );\n' % len(switches) )
				outmain.Append( '\tdesc = swdesc[i];\n' )
				outmain.Append( '\treturn true;\n' )
			else :
				outmain.Append( '\treturn false;\n' )
			outmain.Append( '}\n' )
			outmain.Append( '\n'*2 )
			# %m_find_switch
			outmain.Append( 'int %s_find_switch ( pcstr inname=NULL )\n{\n' % impl.classbase )
			if switches :
				outmain.Append( '\tSU_REQUIRE_OR_NEG( inname && inname[0] );\n' )
				outmain.Append( '\tfor( uint i=0 ; i<%d ; i++ ) {\n' % len(switches) )
				outmain.Append( '\t\tsu::ISwitch::Desc d; %s_enum_switch(i,d);\n' % impl.classbase )
				outmain.Append( '\t\tif( util::Strequals(inname,d.name,false) )\n' )
				outmain.Append( '\t\t\treturn i;\n' )
				outmain.Append( '\t\t}\n' )
			outmain.Append( '\treturn -1;\n}\n' )
			outmain.Append( '\n'*2 )
			# %m_call_switch_stack
			outmain.Append( 'bool %s_call_switch_stack ( void* base, uint i, void* r, va_list args )\n{\n' % impl.classbase )
			if switches :
				outmain.Append( '\tSU_REQUIRE_OR_FALSE( base );\n' )
				outmain.Append( '\tSU_REQUIRE_OR_FALSE( i<%s );\n' % len(switches) )
				for si,s in enumerate(switches) :
					f,c = s
					rtype  = f.rtype if f.rtype!='void' else None
					params = [ 'p%d'%i for i,a in enumerate(f.args) ]
					outmain.Append( '\tif( i==%d ) {\n' % si )
					if rtype :
						outmain.Append( '\t\tSU_ASSERT( r );\n' )
						callret = '*((%s*)r) = ' % rtype
					else :
						outmain.Append( '\t\tSU_ASSERT( r == NULL );\n' )
						callret = ''
					outmain.Append([ '\t\t%s p%d = va_arg(args,%s);\n'%(a[0],i,a[0]) for i,a in enumerate(f.args) ])
					outmain.Append( '\t\t%s((%s*)base)->%s_i.%s(%s);\n' %(callret,impl.classname,s[1].classname,s[0].name,','.join(params)) )
					outmain.Append( '\t\treturn true;\n\t}\n' )
			outmain.Append( '\treturn false;\n}\n' )
			outmain.Append( '\n'*2 )
			# %m_call_switch_mem
			outmain.Append( 'bool %s_call_switch_mem ( void* base, uint i, Any* r, int argc, Any* argv )\n{\n' % impl.classbase )
			if switches :
				outmain.Append( '\tSU_REQUIRE_OR_FALSE( base );\n' )
				outmain.Append( '\tSU_REQUIRE_OR_FALSE( i<%s );\n' % len(switches) )
				for si,s in enumerate(switches) :
					f,c = s
					rtype  = f.rtype if f.rtype!='void' else None
					params = [ 'p%d'%i for i,a in enumerate(f.args) ]
					outmain.Append( '\tif( i==%d ) {\n' % si )
					if f.args:	outmain.Append( '\t\tSU_ASSERT( argc==%d && argv )\n' % len(f.args) )
					else:		outmain.Append( '\t\tSU_ASSERT( argc==0 && argv==NULL )\n' )
					if rtype :
						outmain.Append( '\t\tSU_ASSERT( r )\n' )
						callret1 = 'r->set( '
						callret2 = ' )'
					else :
						outmain.Append( '\t\tSU_ASSERT( r == NULL )\n' )
						callret1 = ''
						callret2 = ''
					outmain.Append([ '\t\t%s p%d = (%s)argv[%d].%s();\n'%(a[0],i,a[0],i,asany_func_arg(a[0])) for i,a in enumerate(f.args) ])
					outmain.Append( '\t\t%s((%s*)base)->%s_i.%s(%s)%s;\n' %(callret1,impl.classname,s[1].classname,s[0].name,','.join(params),callret2) )
					outmain.Append( '\t\treturn true;\n\t}\n' )
			outmain.Append( '\treturn false;\n}\n' )
			outmain.Append( '\n'*2 )
		# FSM definitions
		if impl.IsStateMachine() :
			# %m_automaton
			outmain.Append( 'su::fsm::Automaton* %s_automaton ( void* stub )\n{\n' % impl.classbase )
			outmain.Append( '\tSU_REQUIRE_OR_NULL(stub);\n' )
			outmain.Append( '\treturn &((%s*)%s_stub_to_base(stub))->automaton ;\n}\n' % (impl.classbase,impl.classbase) )
			outmain.Append( '\n'*2 )
		# -
		outmain.Append( '}\n' )


		#
		# -- footer

		outmain.Append( '\n'*2 )
		outmain.Append( warnings )
		outmain.Append( '\n'*2 )
		
		return output




	def generate_intf_lines ( intf ) :

		output = []

		outmain = SIDL_Outputfile( path=intf.GetOutpath(), source=intf )
		output.append( outmain )

		proper   = collect_section( intf, 'proper' )
		code     = collect_section( intf, 'code' )
		depfiles = extend1( intf.usingfiles )

		#
		# -- header

		outmain.Append( header )
		outmain.Append( '\n'*2 )
		outmain.Append( warnings )
		outmain.Append( '\n'*3 )

		#
		# -- include-once symbols

		outmain.Append( '#ifndef _su_%s_h_\n' % intf.classbase.lower() )
		outmain.Append( '#define _su_%s_h_\n' % intf.classbase.lower() )
		outmain.Append( '\n'*4 )

		#
		# -- forwards

		if intf.forwards :
			c_class = atmake.ToList(sidl.SubstModuleAttribute('gencode','c_class',intf))
			c_class = ';'.join(c_class).split(';')
			for w in intf.forwards :
				scope  = w.split('.')
				t      = 'class' if scope[-1] in c_class else 'struct'
				if len(scope)==2 and scope[0]=='su' :
					l = 'su_type( %s )' % scope[-1]
				else :
					l = ''.join([ 'namespace %s { '%s for s in scope[:-1] ])
					l += '%s %s;' % (t,scope[-1])
					l += ' }' * (len(scope)-1)
				outmain.Append( l+'\n' )
			outmain.Append( '\n'*2 )

		#
		# -- includes

		if len(depfiles) :
			for f in depfiles :
				ifile = generate_inc_file( intf, f )
				outmain.Append( '#include %s\n' % ifile )
			outmain.Append( '\n'*4 )

		#
		# -- proper part

		if len(proper) :
			outmain.Append( generate_section(proper,'proper',obj=intf) )
			outmain.Append( '\n'*4 )

		#
		# -- open definition

		outmain.Append( 'namespace su\n{\n' )
		outmain.Append( '\n'*2 )
		if intf.supername :		outmain.Append( '\tstruct %s : public %s\n' % (intf.classbase,intf.supername) )
		else :					outmain.Append( '\tstruct %s\n' % (intf.classbase) )
		outmain.Append( '\t{\n' )

		#
		# -- definition . SU_DECL_INTERFACE fixed part

		outmain.Append( '\t\tSU_DECL_INTERFACE( %s, %s )\n\n\n' % (intf.supername,genuid_hex(intf.classbase)) )

		#
		# -- definition . code part

		if len(code) :
			outmain.Append( generate_section(code,'code',obj=intf,prefix='\t\t') )
			outmain.Newline( 2 )

		#
		# -- definition . subtypes

		if len(intf.subtypes) :
			# forward declarations list of the subtypes
			outmain.Newline( 2 )
			for subt in intf.subtypes :
				if subt.specifier=='union':		outmain.Append( '\t\tunion %s;\n' % subt.identifier )
				if subt.specifier=='struct':	outmain.Append( '\t\tstruct %s;\n' % subt.identifier )
				if subt.specifier=='delegate':	outmain.Append( '\t\tstruct %s;\n' % subt.identifier )
			# outputs subtypes themselves
			for subt in intf.subtypes :
				outmain.Newline( 2 )
				outmain.Append( generate_subtype(intf,subt,prefix='\t\t') )
			outmain.Newline( 2 )


		#
		# -- definition . methods

		if intf.functions :
			no_code_functions = [x for x in intf.functions if (x.code is None)]
			outmain.Append( SIDL_Function_GetCppLines(no_code_functions, indent='\t\t', sep='\n') )

		#
		# -- close definition

		outmain.Append( '\t};\n\n\n}\n' )
		outmain.Append( '\n'*4 )

		#
		# -- include-once symbols

		outmain.Append( '#endif // _su_%s_h_\n' % intf.classbase.lower() )

		#
		# -- footer

		outmain.Append( '\n'*2 )
		outmain.Append( warnings )
		outmain.Append( '\n'*2 )

		return output




	def generate_test_lines ( test ) :
		
		output = []

		outmain = SIDL_Outputfile( path=test.GetOutpath(), source=test, label="test '%s'"%test.GetName() )
		output.append( outmain )

		proper			= collect_section( test, 'proper' )
		onrun			= collect_section( test, 'onrun'  )
		oninit			= collect_section( test, 'oninit' )
		onshut			= collect_section( test, 'onshut' )
		unit			= collect_section( test, 'unit'   )
		code			= collect_section( test, 'code'   )
		depfiles		= ['"supch_fwtests.h"'] + collect_dep_files( test )

		# -- header

		outmain.Append( header )
		outmain.Append( '\n'*2 )
		outmain.Append( warnings )
		outmain.Append( '\n'*3 )

		#
		# -- includes

		if len(depfiles) :
			for f in depfiles :
				ifile = generate_inc_file( test, f )
				outmain.Append( '#include %s\n' % ifile )
			outmain.Append( 'using namespace su;\n' )
			outmain.Append( '\n'*4 )

		#
		# -- proper part

		if len(proper) :
			outmain.Append( generate_section(proper,'proper',obj=test) )
			outmain.Append( '\n'*4 )

		#
		# -- test suite declaration

		if onrun or oninit or onshut or unit or code :
			outmain.Append( 'TestSuite( %s )\n{\n' % test.GetName() )
			outmain.Append( '\n'*1 )

			if len(code) :
				# -- explicit code part
				outmain.Append( generate_section(code,'code',obj=test,prefix='\t') )
			else :
				# -- onrun, oninit,onshut part
				onfunc = { 'TestRun':onrun, 'TestInit':oninit, 'TestShut':onshut }
				for k,v in onfunc.items() :
					if len(v) :
						outmain.Append( '\t%s( )\n\t{\n' % k )
						outmain.Append( generate_section(v,k,obj=test,prefix='\t\t') )
						outmain.Append( '\t}\n' )
						outmain.Append( '\n'*1 )

				# units
				for ui,u in enumerate(unit) :
					tu_name = u.GetAttribute('name') or 'tunit%04d'%ui
					outmain.Append( '\tTestUnit( %s )\n\t{\n' % tu_name.replace(' ','_').replace('-','_') )
					outmain.Append( generate_section(u,'unit',obj=test,prefix='\t\t') )
					outmain.Append( '\t}\n' )
					outmain.Append( '\n'*1 )

			outmain.Append( '};\n' )
			outmain.Append( '\n'*2 )
			outmain.Append( 'ExportSuite( %s )\n' % test.GetName() )
			outmain.Append( '\n'*2 )

		#
		# -- footer

		outmain.Append( '\n'*2 )
		outmain.Append( warnings )
		outmain.Append( '\n'*2 )

		return output




	def generate_test_collection_lines ( ) :

		output = []

		for modef in sidl.SelectModuleDefinitions('gencode','tests') :

			outpath   = modef.SubstAttribute( 'tests' )
			runfilter = modef.SubstAttribute( 'testsrunfilter', '*' )
			assert( outpath )

			outmain = SIDL_Outputfile( path=outpath, label='tests collection' )
			output.append( outmain )

			tl = []
			for k in sorted(sidl.tests.keys()) :
				t = sidl.tests[k]
				if modef.MatchScope(t) :
					units = collect_section( t, 'unit' )
					if len(units) :
						tl.append( k.replace('.','_') )

			# header
			outmain.Append( header )
			outmain.Newline( 2 )
			outmain.Append( warnings )
			outmain.Newline( 2 )
			outmain.Append( '#ifndef _%s_\n' % outmain.BasenameAsCLabel() )
			outmain.Append( '#define _%s_\n' % outmain.BasenameAsCLabel() )
			outmain.Newline( 3 )

			# declarations
			for t in tl :
				outmain.Append( 'ImportSuite( %s )\n' % t )
			outmain.Append( '\n'*2 )

			outmain.Append( 'namespace\n{\n')
			outmain.Append( '\n'*1 )
			outmain.Append( '\tvoid RunSuites ( )\n\t{\n' )
			for t in tl :
				if runfilter:	co = '' if algo.mfilters(t,runfilter) else '// '
				else:			co = ''
				outmain.Append( '\t\t%sRunSuite( %s )\n' % (co,t) )
			outmain.Append( '\t}\n\n')
			outmain.Append( '}\n')

			# footer
			outmain.Newline( 4 )
			outmain.Append( '#endif // _%s_\n' % outmain.BasenameAsCLabel() )
			outmain.Newline( 2 )
			outmain.Append( warnings )
			outmain.Newline( 2 )

		return output

		


	def generate_uid_collection_lines ( uids ) :

		output = []

		for modef in sidl.SelectModuleDefinitions('gencode','uids') :

			modns	= modef.SubstAttribute( 'namespace' ) or 'su'
			outpath = modef.SubstAttribute( 'uids' )
			assert( outpath )

			outmain = SIDL_Outputfile( path=outpath, label='uids collection' )
			output.append( outmain )

			# header
			outmain.Append( header )
			outmain.Newline( 2 )
			outmain.Append( warnings )
			outmain.Newline( 2 )
			outmain.Append( '#ifndef _%s_\n' % outmain.BasenameAsCLabel() )
			outmain.Append( '#define _%s_\n' % outmain.BasenameAsCLabel() )
			outmain.Newline( 3 )

			outmain.Append( 'namespace %s { namespace uid\n{\n' % modns )
			outmain.Newline( 1 )

			main_lines  = []
			alias_lines = []

			for k in sorted(uids.keys()) :
				obj = uids[k]
				if obj.MatchScope(modef) :
					if isinstance(obj,SIDL_Uid) and obj.alias :
						alias_lines.append( '\tconst su::classid %s = %s;\n' % (k,obj.alias) )
					else :
						main_lines.append( '\tconst su::classid %s = %s;\n' % (k,genuid_hex(k)) )

			outmain.Newline( 1 )
			outmain.Append( main_lines )

			outmain.Newline( 1 )
			outmain.Append( alias_lines )

			outmain.Newline( 1 )
			outmain.Append( '} }\n')

			# footer
			outmain.Newline( 4 )
			outmain.Append( '#endif // _%s_\n' % outmain.BasenameAsCLabel() )
			outmain.Newline( 2 )
			outmain.Append( warnings )
			outmain.Newline( 2 )

		return output




	def generate_force_references_lines ( ) :

		output = []

		builders = {}
		for b in sidl.implementations.values() :
			builders[b.classbase] = b

		for modef in sidl.SelectModuleDefinitions('gencode','forceref') :

			outpath = modef.SubstAttribute( 'forceref' )
			assert( outpath )

			outmain = SIDL_Outputfile( path=outpath, label='force references' )
			output.append( outmain )

			blist = []

			for k in sorted(builders.keys()) :
				obj = builders[k]
				if obj.MatchScope(modef) and (not k in blist) :
					blist.append( k )

			# header
			outmain.Append( header )
			outmain.Newline( 2 )
			outmain.Append( warnings )
			outmain.Newline( 2 )
			outmain.Append( '#ifndef _%s_\n' % outmain.BasenameAsCLabel() )
			outmain.Append( '#define _%s_\n' % outmain.BasenameAsCLabel() )
			outmain.Newline( 3 )

			# declarations
			for b in blist :
				outmain.Append( 'void su_forceref_builder_%s();\n' % b )
			outmain.Append( '\n'*2 )

			outmain.Append( 'namespace\n{\n\n' )
			outmain.Append( '\tstruct su_forceref_t\n\t{\n' )
			outmain.Append( '\t\tsu_forceref_t ( )\n\t\t{\n' )
			for b in blist :
				outmain.Append( '\t\t\tsu_forceref_builder_%s();\n' % b )
			outmain.Append( '\t\t}\n\t} su_forceref_object;\n\n}' )

			# footer
			outmain.Newline( 4 )
			outmain.Append( '#endif // _%s_\n' % outmain.BasenameAsCLabel() )
			outmain.Newline( 2 )
			outmain.Append( warnings )
			outmain.Newline( 2 )

		return output

		



		
	def generate_settings_lines ( ) :

		output = []

		for modef in sidl.SelectModuleDefinitions('gencode','settings') :

			modns	= modef.SubstAttribute( 'namespace' )
			outpath = modef.SubstAttribute( 'settings' )
			assert( outpath )

			MODNS = modns.upper()

			outmain = SIDL_Outputfile( path=outpath, label='settings file' )
			output.append( outmain )

			# header
			outmain.Append( header )
			outmain.Newline( 2 )
			outmain.Append( warnings )
			outmain.Newline( 3 )

			outmain.Append( generate_define('%s_BUILD_REVISION'%MODNS,sidl.env.SubstVar('__REVISION__') or 0) )
			outmain.Append( generate_define('%s_BUILD_REVISION_S'%MODNS,'"%s"'%sidl.env.SubstVar('__REVISION__') or 0) )

			outmain.Append( generate_define('%s_BUILD_USERNAME'%MODNS,sidl.env.SubstVar('username')) )
			outmain.Append( generate_define('%s_BUILD_USERNAME_S'%MODNS,'"%s"'%sidl.env.SubstVar('username')) )

			outmain.Append( generate_define('%s_BUILD_WSNAME'%MODNS,sidl.env.SubstVar('wsname')) )
			outmain.Append( generate_define('%s_BUILD_WSNAME_S'%MODNS,'"%s"'%sidl.env.SubstVar('wsname')) )

			outmain.Append( generate_define('%s_BUILD_HOSTNAME'%MODNS,sidl.env.SubstVar('hostname')) )
			outmain.Append( generate_define('%s_BUILD_HOSTNAME_S'%MODNS,'"%s"'%sidl.env.SubstVar('hostname')) )

			platform = sidl.env.SubstVar('__PLATFORM__')

			outmain.Append( generate_define('%s_BUILD_PLATFORM'%MODNS,platform) )
			outmain.Append( generate_define('%s_BUILD_PLATFORM_S'%MODNS,'"%s"'%platform) )

			outmain.Append( '#if defined(SU_PLATFORM_CONFIG) || defined(SU_PLATFORM_SYSTEM) || defined(SU_PLATFORM_STARTUP)\n' )
			outmain.Append( '#if defined(SU_GENERIC) || defined(SU_HW_GENERIC)\n' )
			outmain.Append( '#include "su_generic.h"\n' )
			outmain.Append( '#else\n' )
			outmain.Append( '#include <sufw-%s/su_platform_%s.h>\n' % (platform.lower(),platform.lower()) )
			outmain.Append( '#endif\n' )
			outmain.Append( '#endif\n' )

			# footer
			outmain.Newline( 4 )
			outmain.Append( warnings )
			outmain.Newline( 2 )

		return output




	def generate_mapping_lines ( ) :

		def build_sign ( o ) :
			if isinstance(o,SIDL_RecordItem) :
				# for record item, takes the record type signature instead !
				if o.what=='rec':	return build_sign( sidl.records[o.type] )
				else:				return o.sign()
			elif isinstance(o,SIDL_Record) :
				return ''.join([ build_sign(m) for m in o.members ])
			elif isinstance(o,SIDL_Map) :
				return ''.join([ build_sign(sidl.tags[t]) for t in o.tags ])

		def max_none( a, b ) :
			if a is None:	return b
			if b is None:	return a
			return max(a,b)

		def get_align ( o ) :
			a = None
			if isinstance(o,SIDL_Record) :
				for m in o.members :
					a = max_none(a,m.align)
			elif isinstance(o,SIDL_Tag) :
				for m in o.members :
					if   m.what=='rec':	a = max_none( a, get_align(sidl.records[o.type]) )
					elif m.what=='var':	a = max_none( a, m.align )
			elif isinstance(o,SIDL_Map) :
				for t in o.tags :
					a = max_none( a, get_align(sidl.tags[t]) )
			return a

		def collect_requirements ( o ) :
			# returns [ (rec,obj,id) ]
			if isinstance(o,SIDL_RecordItem) and (o.what=='rec') :
				return collect_requirements(sidl.records[o.type])
			elif isinstance(o,SIDL_Record) :
				lreq,lid = [],[]
				for m in o.members :
					# requirements from the given condition and the dimensions
					s = ''.join( atmake.ToList(m.given) + atmake.ToList(m.width) + atmake.ToList(m.size) )
					for mm in o.members :
						if (mm.what=='var') and (mm.value==None) and (mm.id in s) and (mm.id not in lid) :
							id = mm.id
							lreq.append( (None,mm,id) )
							lid.append( id )
					if m.what=='rec':
						# collect requirements of the referenced record itself
						sub = collect_requirements( m )
						for s in sub :
							id = '%s_%s'%(m.id,s[1].id)
							if not id in lid :
								lreq.append( (m,s[1],id) )
								lid.append( id )
				return lreq

		def subst_var_name ( s, members, pattern ) :
			for m in members:
				if m.what=='var':
					p = s.split(m.id,1)
					if len(p)==2 :
						p0 = subst_var_name( p[0], members, pattern )
						p1 = subst_var_name( p[1], members, pattern )
						return p0 + pattern%m.id + p1
			return s

		def var_ctype ( o ) :
			assert isinstance(o,SIDL_RecordItem)
			ctype = { 'ui':'uint', 'si':'int', 'fp':'float', 'fx':'float' }[ o.type ]
			return ctype


		output = []
		
		for modef in sidl.SelectModuleDefinitions('gencode','maps') :

			modns   = modef.SubstAttribute('namespace')
			outpath = modef.SubstAttribute('maps')
			outbase = os.path.splitext( outpath )[0]
			assert( outpath )
			assert( outbase )

			# GENERATE OBJECT CODE

			objcode = []
			maplist = []

			for r in list(sidl.records.values())+list(sidl.tags.values())+list(sidl.maps.values()) :

				if not modef.MatchScope(r) :
					continue

				uri = r.C_uri()

				pubcode = SIDL_Outputlines()
				privcode = SIDL_Outputlines()
				declcode = SIDL_Outputlines()
				objcode.append({ "uri":uri, "declcode":declcode, "pubcode":pubcode, "privcode":privcode })

				if atmake.IsClassof(r,SIDL_Record) or atmake.IsClassof(r,SIDL_Tag) :

					# RECORD or TAG

					req			  = collect_requirements( r )
					reqid		  = [ri for rr,ro,ri in req]
					const_members = [x for x in r.members if x.what=='const']
					var_members   = [x for x in r.members if x.what!='const']
					
					# create private string id
	
					sidl.CreateStringId( 'map_'+uri, string=r.uri, private=True, loc=r.loc )

					# declarations

					if 1:
						declcode.Append( '\t// -- declaration of %s\n' % uri )
						declcode.Newline(1)
						declcode.Append( '\tstruct %s%s\n\t{\n' % (uri,' : public base_tag' if atmake.IsClassof(r,SIDL_Tag) else '') )
						# -- var members
						if var_members :
							for m in var_members :
								if m.what=='rec':	declcode.Append( '\t\t%s %s;\n' % (m.type.replace('.','_'), m.id) )
								else:				declcode.Append( '\t\t%s_attr %s;\n' % (m.type,m.id) )
							declcode.Newline(1)
						# -- const members
						if const_members :
							for m in const_members :
								declcode.Append( '\t\tstatic const uint %s = (%s);\n' % (m.id,m.value) )
						# -- ALIGN const member
						declcode.Append( '\t\tstatic const uint ALIGN = (%d);\n' % (get_align(r) or 0) )
						if atmake.IsClassof(r,SIDL_Tag):
							# -- tag decl (members, requirements, ctor)
							declcode.Append( '\t\tstatic const uint CODE = (%d);\n' % r.code )
							declcode.Newline(1)
							if req:
								declcode.Append( '\t\tstruct requirements\n\t\t{\n' )
								declcode.Append( '\t\t\trequirements() { util::Uninitialized(*this); }\n' )
								for rr,ro,ri in req:
									declcode.Append( '\t\t\t%s %s;\n'%(var_ctype(ro),ri) )
								declcode.Append( '\t\t};' )
								declcode.Newline(1)
								declcode.Append( '\t\tint create ( byte* p, requirements& r, bool forcelong=false, bool checkun=true );\n' )
							else :
								declcode.Newline(1)
								declcode.Append( '\t\tint create ( byte* p, bool forcelong=false );\n' )
							declcode.Append( '\t\tint load ( byte* p );\n' )
						else:
							# -- record decl
							declcode.Newline(1)
							if req:		args = ', %s' % ', '.join(['%s in_%s'%(var_ctype(ro),ri) for rr,ro,ri in req])
							else:		args = ''
							declcode.Append( '\t\tint create ( byte* p%s );\n' % args )
							declcode.Append( '\t\tint load ( byte* p );\n' )
						declcode.Append( '\t};\n' )

					# tag create()

					if atmake.IsClassof(r,SIDL_Tag) :
						# -- signature
						if req:
							pubcode.Append( 'int map::%s::create ( byte* p, requirements& r, bool forcelong, bool checkun )\n' % uri )
							pubcode.Append( '{\n' )
							# -- check requirements
							pubcode.Append( '\tif( checkun ) {\n' )
							for rr,ro,ri in req:
								pubcode.Append( '\t\tSU_ASSERT( !util::CheckUninitialized(r.%s) );\n' % ri )
							pubcode.Append( '\t}\n' )
						else :
							pubcode.Append( 'int map::%s::create ( byte* p, bool forcelong )\n' % uri )
							pubcode.Append( '{\n' )
						# -- setup
						pubcode.Append( '\tutil::Zero(*this);\n' )
						pubcode.Append( '\tint s=base_tag::create(p,forcelong); code.set(CODE);\n' )
						for i,m in enumerate(var_members) :
							pubcode.Append( '\t' )
							if m.given :
								cond = subst_var_name( m.given, r.members, 'r.%s' )
								pubcode.Append( 'if(%s){ '%cond )
							if m.what=='var' :
								if not m.align is None:		pubcode.Append( 's+=ub_align(p,s,%d); ' % m.align )
								dim = subst_var_name( '%s,%s'%(m.width,m.size), r.members, 'r.%s' )
								pubcode.Append( 's+=%s.setup(p,s,%s); ' % (m.id,dim) )
								if   (m.id in reqid) :		pubcode.Append( '%s.set(r.%s); %s.ro=true;' % (m.id,m.id,m.id) )
								elif (m.value != None) :	pubcode.Append( '%s.set(%s); ' % (m.id,m.value) )
								elif (m.id=='slen') :		pubcode.Append( '%s.set(forcelong?0x3f:0); ' % (m.id) )
							elif m.what=='rec' :
								if not m.align is None:		pubcode.Append( 's+=ub_align(p,s,%d); ' % m.align )
								else:						pubcode.Append( 's+=ub_align(p,s,0); ' )
								mreq = [x for x in req if x[2].startswith('%s_'%m.id)]
								if mreq:	args = ',%s' % ','.join(['r.%s'%ri for rr,ro,ri in mreq])
								else:		args = ''
								pubcode.Append( 's+=%s.create(p+(s>>3)%s); ' % (m.id,args) )
							if m.given :
								pubcode.Append( '}' )
							pubcode.Append( '\n' )
						pubcode.Append( '\tif(forcelong)   { SU_ASSERT(s>=48); llen.set((s-48+7)>>3); return s; }\n' )
						pubcode.Append( '\telse if(s>=520) { return create(p%s,true%s); }\n' % ((',r',',checkun') if req else ('','')) )
						pubcode.Append( '\telse            { SU_ASSERT(s>=16); slen.set((s-16+7)>>3); return s; }\n' )
						pubcode.Append( '}\n' )

					# record create()

					if atmake.IsClassof(r,SIDL_Record) :
						pubcode.Append( 'int %s::map::%s::create ( byte* p%s )\n' % (modns,uri,args) )
						pubcode.Append( '{\n' )
						# -- setup
						pubcode.Append( '\tutil::Zero(*this);\n' )
						pubcode.Append( '\tint s=0;\n' )
						for i,m in enumerate(var_members) :
							pubcode.Append( '\t' )
							if m.given :
								cond = subst_var_name( m.given, r.members, 'in_%s' )
								pubcode.Append( 'if(%s){ '%cond )
							if m.what=='var' :
								if not m.align is None:		pubcode.Append( 's+=ub_align(p,s,%d); ' % m.align )
								dim = subst_var_name( '%s,%s'%(m.width,m.size), r.members, 'in_%s' )
								pubcode.Append( 's+=%s.setup(p,s,%s); ' % (m.id,dim) )
								if   (m.id in reqid) :		pubcode.Append( '%s.set(in_%s); %s.ro=true; ' % (m.id,m.id,m.id) )
								elif (m.value != None) :	pubcode.Append( '%s.set(%s); ' % (m.id,m.value) )
							if m.given :
								pubcode.Append( '}' )
							pubcode.Append( '\n' )
						pubcode.Append( '\treturn s;\n' )
						pubcode.Append( '}\n' )

					# record/tag load()

					if 1 :
						pubcode.Newline( 2 )
						pubcode.Append( 'int map::%s::load ( byte* p )\n' % uri )
						pubcode.Append( '{\n' )
						pubcode.Append( '\tutil::Zero(*this);\n' )
						if atmake.IsClassof(r,SIDL_Tag):	pubcode.Append( '\tint s=base_tag::load(p);\n' )
						else:						pubcode.Append( '\tint s=0;\n' )
						for i,m in enumerate(var_members) :
							pubcode.Append( '\t' )
							if m.given :
								cond = subst_var_name( m.given, r.members, '%s.get()' )
								pubcode.Append( 'if(%s){ '%cond )
							if m.what=='var' :
								if not m.align is None:		pubcode.Append( 's+=ub_align(p,s,%d); ' % m.align )
								dim = subst_var_name( '%s,%s'%(m.width,m.size), r.members, '%s.get()' )
								pubcode.Append( 's+=%s.setup(p,s,%s); ' % (m.id,dim) )
							elif m.what=='rec' :
								if not m.align is None:		pubcode.Append( 's+=ub_align(p,s,%d); ' % m.align )
								else:						pubcode.Append( 's+=ub_align(p,s,0); ' )
								pubcode.Append( 's+=%s.load(p+(s>>3)); ' % m.id )
							if m.given :
								pubcode.Append( '}' )
							pubcode.Append( '\n' )
						pubcode.Append( '\treturn s;\n' )
						pubcode.Append( '}\n' )

					# record/tag writecontent()

					if atmake.IsClassof(r,SIDL_Tag) :
						privcode.Append( '\tvoid writecontent_tag_%s ( map::base_map& m, IDatatree* dtt, map::context& ctxt )\n\t{\n' % uri )
						privcode.Append( '\t\tSU_REQUIRE_OR_RETURN( ctxt.errcode==0 );\n' )
						privcode.Append( '\t\tStrcopy( ctxt.tagname, SU_VERBOSE_STR("%s") );\n' % uri )
						privcode.Append( '\t\tmap::%s t;\n' % uri )
						req = collect_requirements( r )
						if req:
							privcode.Append( '\t\tmap::%s::requirements req;\n' % uri )
							for rr,ro,ri in req:
								rpath = '%s.%s'%(rr.id,ro.id) if rr else ri
								privcode.Append( '\t\treq.%s = get_tag_req(dtt,"%s",ctxt);\n' % (ri,rpath) )
						privcode.Append( '\t\tm.tag_add(t%s);\n' % (',req' if req else '') )
						for m in r.members :
							if m.what=='var':		privcode.Append( '\t\twritecontent_var(t.%s,dtt,ctxt,"%s");\n' % (m.id,m.id) )
							elif m.what=='rec':		privcode.Append( '\t\twritecontent_rec_%s(t.%s,dtt,ctxt,"%s");\n' % (m.type.replace('.','_'),m.id,m.id) )
						privcode.Append( '\t\tif( ctxt.errcode==0 ) ctxt.tagname[0]=0;\n' )
						privcode.Append( '\t}\n' )
					elif atmake.IsClassof(r,SIDL_Record) :
						privcode.Append( '\tvoid writecontent_rec_%s ( map::%s& o, IDatatree* dtt, map::context& ctxt, pcstr sub=NULL )\n\t{\n' % (uri,uri) )
						privcode.Append( '\t\tSU_REQUIRE_OR_RETURN( ctxt.errcode==0 );\n' )
						privcode.Append( '\t\tStrcopy( ctxt.recname, SU_VERBOSE_STR("%s") );\n' % r.uri )
						privcode.Append( '\t\tif(sub) dtt->push(-1,sub);\n' )
						for m in r.members :
							if m.what=='var':		privcode.Append( '\t\twritecontent_var(o.%s,dtt,ctxt,SU_VERBOSE_STR("%s"));\n' % (m.id,m.id) )
						privcode.Append( '\t\tif(sub) dtt->pop(1);\n' )
						privcode.Append( '\t\tif( ctxt.errcode==0 ) ctxt.recname[0]=0;\n' )
						privcode.Append( '\t}\n' )

				elif atmake.IsClassof(r,SIDL_Map) :

					# MAP

					maplist.append( r )
					sign = build_sign( r )

					# build string enum
					
					mm = [ 'map_'+t.replace('.','_') for t in r.tags ]
					sidl.CreateStringEnum( 'map_'+uri, string=r.uri, concerns=mm, loc=r.loc )

					# declaration

					declcode.Append( '\t// -- declaration of %s\n' % uri )
					declcode.Newline( 1 )
					declcode.Append( '\tstruct %s : public base_map\n\t{\n' % uri )
					declcode.Append( '\t\tstatic const uint32 CSIGN = %s;\n' % genuid_hex(uri) )
					declcode.Append( '\t\tstatic const uint32 FSIGN = %s;\n' % genuid_hex(sign) )
					declcode.Append( '\t\tstatic const uint   ALIGN = (%d);\n' % (get_align(r) or 0) )
					declcode.Append( '\t\t\n' )
					declcode.Append( '\t\tint create ( byte* p );\n' )
					declcode.Append( '\t\tint load ( byte* p );\n' )
					declcode.Append( '\t\tbool writecontent( IDatatree*, context& );\n' )
					declcode.Append( '\t\tstatic bool matchcontent( IDatatree* );\n' )
					declcode.Append( '\t};\n' )

					# create()

					pubcode.Append( 'int map::%s::create ( byte* p )\n{\n' % uri )
					pubcode.Append( '\tint s=base_map::create(p,ALIGN);\n' )
					pubcode.Append( '\tcsign.set(CSIGN);\n' )
					pubcode.Append( '\tfsign.set(FSIGN);\n' )
					pubcode.Append( '\tflen.set((s+7)>>3);\n' )
					pubcode.Append( '\treturn s;\n' )
					pubcode.Append( '}\n' )

					# load()

					pubcode.Newline( 2 )
					pubcode.Append( 'int map::%s::load ( byte* p )\n{\n' % uri )
					pubcode.Append( '\tint n = base_map::load(p,ALIGN);\n' )
					pubcode.Append( '\tif( !n ) return 0;\n' )
					pubcode.Append( '\tif( fsign.get() != FSIGN ) return 0;\n' )
					pubcode.Append( '\tif( csign.get() != CSIGN ) return 0;\n' )
					pubcode.Append( '\treturn n;\n' )
					pubcode.Append( '}\n' )

					# writecontent()

					pubcode.Newline( 2 )
					pubcode.Append( 'bool map::%s::writecontent( IDatatree* dtt, context& ctxt )\n{\n' % uri )
					pubcode.Append( '\tutil::Zero(ctxt);\n' )
					pubcode.Append( '\tSU_REQUIRE_OR_FALSE( dtt );\n' )
					pubcode.Append( '\tSU_REQUIRE_OR_FALSE( dtt->issequence(-1) );\n' )
					pubcode.Newline( 1 )
					pubcode.Append( '\tutil::Strcopy( ctxt.mapname, SU_VERBOSE_STR("%s") );\n' % r.uri )
					pubcode.Newline( 1 )
					pubcode.Append( '\tint n = dtt->size(-1);\n' )
					pubcode.Append( '\tfor( int i=0 ; i<n && ctxt.errcode==0 ; i++ )\n' )
					pubcode.Append( '\t{\n' )
					pubcode.Append( '\t\tint top = dtt->gettop();\n' )
					pubcode.Newline( 1 )
					pubcode.Append( '\t\tStringId tsid = push_tag(dtt,i,ctxt);\n' )
					pubcode.Append( '\t\tif( !tsid )	break;\n' )
					pubcode.Newline( 1 )
					for i,t_name in enumerate(r.tags) :
						t = sidl.tags[t_name]
						t_uri = t.uri.replace('.','_')
						if i==0 :	if_test = 'if'
						else:		if_test = 'else if'
						pubcode.Append( '\t\t%s( tsid == sid::map_%s::map_%s )\n' % (if_test,uri,t_uri) )
						pubcode.Append( '\t\t\twritecontent_tag_%s(*this,dtt,ctxt);\n' % t_uri )
					pubcode.Newline( 1 )
					pubcode.Append( '\t\tdtt->settop( top );\n' )
					pubcode.Append( '\t}\n' )
					pubcode.Append( '\treturn (ctxt.errcode==0);\n' )
					pubcode.Append( '}\n' )

					# matchcontent()

					pubcode.Newline( 2 )
					pubcode.Append( 'bool map::%s::matchcontent( IDatatree* dtt )\n{\n' % uri )
					pubcode.Append( """
						SU_REQUIRE_OR_FALSE( dtt );
						SU_REQUIRE_OR_FALSE( dtt->issequence(-1) );
						int top=dtt->gettop(), n=dtt->size(-1), i;
						for( i=0 ; i<n ; i++ )
						{
							dtt->push(-1,i);
							if( !dtt->ismapping(-1) )		break;
							dtt->push(-1,"tagid");
							pcstr tagid = dtt->asstring(-1);
							if( !sid::{senum}::HAS(tagid) )	break;
							dtt->pop(2);
						}
						dtt->settop(top);
						return (n>0) && (i==n);
						""", indent="\t", senum='map_'+uri )
					pubcode.Append( '}\n' )


			# GENERATE BINARIZER CODE

			bincode = SIDL_Outputlines()
			if 1 :
				bincode.Append( '// binarizer' )
				bincode.Newline( 1 )
				# map::select
				bincode.Append( 'StringId map::select ( su::IDatatree* dtt )\n{\n' )
				for m in maplist :
					uri = m.C_uri()
					bincode.Append( '\tif( %s::matchcontent(dtt) )\t\treturn sid::map_%s::TYPE;\n' % (uri,uri) )
				bincode.Append( '\treturn sid::null;\n' )
				bincode.Append( '}' )
				bincode.Newline( 2 )
				# map::binarize
				bincode.Append( 'uint map::binarize ( IDatatree* dtt, byte* p, context& ctxt )\n{\n' )
				bincode.Append( '\tSU_REQUIRE_OR_0( dtt );\n' )
				bincode.Append( '\tSU_REQUIRE_OR_0( dtt->issequence(-1) );\n' )
				bincode.Append( '\tSU_REQUIRE_OR_0( p );\n' )
				bincode.Newline( 1 )
				bincode.Append( '\tutil::Zero(ctxt);\n' )
				bincode.Newline( 1 )
				bincode.Append( '\tStringId mapid = select(dtt);\n' )
				for i,m in enumerate(maplist) :
					uri = m.C_uri()
					cond = 'if' if i==0 else 'else if'
					bincode.Append( '\t%s( mapid==sid::map_%s::TYPE )\n\t{\n' % (cond,uri) )
					bincode.Append( '\t\t%s map; map.create(p);\n' % uri )
					bincode.Append( '\t\tif( map.writecontent(dtt,ctxt) )	return map.bsize();\n' )
					bincode.Append( '\t}\n' )
				bincode.Append( '\telse\n\t{\n' )
				bincode.Append( '\t\tStrcopy( ctxt.errmsg, "Invalid map !" );\n' )
				bincode.Append( '\t\tctxt.errcode = map::context::EC_INVALID_MAP;\n' )
				bincode.Append( '\t}\n\n' )
				bincode.Append( '\treturn 0;\n' )

				bincode.Append( '}\n' )
				bincode.Newline( 2 )
				# map::binarize
				bincode.Append( 'uint map::binarize ( IDatatree* dtt, Pool* p, byte*& outaddr, context& ctxt )\n{\n' )
				bincode.Append( '\tSU_REQUIRE_OR_0( p );\n' )
				bincode.Append( '\tSU_REQUIRE_OR_0( globals::binarizemaxbs );\n' )
				bincode.Append( '\tbyte* baddr = (byte*) p->Alloc(globals::binarizemaxbs);\n' )
				bincode.Append( '\tSU_REQUIRE_OR_FALSE( baddr );\n' )
				bincode.Append( '\tutil::Zero(baddr,globals::binarizemaxbs);\n' )
				bincode.Append( '\tbyte* qaddr = Round16(baddr);\n' )
				bincode.Newline( 1 )
				bincode.Append( '\tuint bs = binarize(dtt,qaddr,ctxt);\n' )
				bincode.Append( '\tSU_ASSERT( bs < globals::binarizemaxbs );\n' )
				bincode.Newline( 1 )
				bincode.Append( '\tif(bs) outaddr = (byte*) p->DumpMemory(qaddr,bs);\n' )
				bincode.Append( '\tp->Free(baddr);\n' )
				bincode.Newline( 1 )
				bincode.Append( '\treturn (outaddr && bs) ? bs : 0;\n' )
				bincode.Append( '}\n' )
				bincode.Newline( 2 )


			# GENERATE REVERSE CODE

			revcode = SIDL_Outputlines()
			if 1 :
				revcode.Append( '// reverter' )
				revcode.Newline( 1 )
				# map::revert
				revcode.Append( 'StringId map::revert ( uint32 csign )\n{\n' )
				for m in maplist :
					uri = m.C_uri()
					revcode.Append( '\tif( %s::CSIGN==csign )\t\treturn sid::map_%s::TYPE;\n' % (uri,uri) )
				revcode.Append( '\treturn sid::null;\n' )
				revcode.Append( '}' )
				revcode.Newline( 2 )
				# map::revert
				revcode.Append( 'StringId map::revert ( StringId mapid, uint code )\n{\n' )
				for i,m in enumerate(maplist) :
					uri = m.C_uri()
					cond = 'if' if i==0 else 'else if'
					revcode.Append( '\t%s( sid::map_%s::TYPE==mapid )\n\t{\n' % (cond,uri) )
					for i,t_name in enumerate(m.tags) :
						t = sidl.tags[t_name]
						t_uri = t.uri.replace('.','_')
						if i==0 :	if_test = 'if'
						else:		if_test = 'else if'
						revcode.Append( '\t\t%s( map::%s::CODE==code )\t\treturn sid::map_%s::map_%s;\n' % (if_test,t_uri,uri,t_uri) )
					revcode.Append( '\t}\n' )
				revcode.Append( '\treturn sid::null;\n' )
				revcode.Append( '}' )
				revcode.Newline( 2 )
				# map::revert
				revcode.Append( 'bool map::info( StringId mapid, uint& csign, uint& fsign, uint& align )\n{\n' )
				for m in maplist :
					uri = m.C_uri()
					revcode.Append( '\tif( sid::map_%s::TYPE==mapid ) { csign=%s::CSIGN; fsign=%s::FSIGN; align=%s::ALIGN; return true; };\n' % (uri,uri,uri,uri) )
				revcode.Append( '\treturn false;\n' )
				revcode.Append( '}' )
				revcode.Newline( 2 )


			# GENERATE FILES

			# generate chh file
			
			if 1:
				outchh = SIDL_Outputfile( path='%s.h'%outbase, label='mapping file hdr' )
				output.append( outchh )

				# -- chh header
				outchh.Append( header )
				outchh.Newline( 2 )
				outchh.Append( warnings )
				outchh.Newline( 2 )
				outchh.Append( '#ifndef _%s_\n' % outchh.BasenameAsCLabel() )
				outchh.Append( '#define _%s_\n' % outchh.BasenameAsCLabel() )
				outchh.Newline( 3 )
				outchh.Append( '#include <sufw/su_core.h>\n' )
				outchh.Append( '#include <sufw/asset/su_idatatree.h>\n' )
				outchh.Newline( 3 )
				outchh.Append( 'namespace %s { namespace map\n{\n' % modns )

				# -- chh code
				for o in objcode :
					outchh.Newline( 2 )
					outchh.Append( o["declcode"] )
				
				# -- chh binarizer
				outchh.Newline( 2 )
				outchh.Append( '\tStringId select   ( IDatatree*                                            );\n')
				outchh.Append( '\tuint     binarize ( IDatatree*, byte*, context&                           );\n')
				outchh.Append( '\tuint     binarize ( IDatatree*, Pool*, byte*&, context&                   );\n')

				# -- chh reverter
				outchh.Newline( 1 )
				outchh.Append( '\tStringId revert   ( uint32   mapcsign                                     );\n' )
				outchh.Append( '\tStringId revert   ( StringId mapid, uint tagcode                          );\n' )
				outchh.Append( '\tbool     info     ( StringId mapid, uint& csign, uint& fsign, uint& align );\n' )

				# -- chh footer
				outchh.Newline( 2 )
				outchh.Append( '} }\n')
				outchh.Newline( 4 )
				outchh.Append( '#endif // _%s_\n' % outchh.BasenameAsCLabel() )
				outchh.Newline( 2 )
				outchh.Append( warnings )
				outchh.Newline( 2 )

			# generate cpp file

			if 1:
				outcpp = SIDL_Outputfile( path='%s.cpp'%outbase, label='mapping file cpp' )
				output.append( outcpp )

				# -- cpp header
				outcpp.Append( header )
				outcpp.Newline( 2 )
				outcpp.Append( warnings )
				outcpp.Newline( 3 )
				outcpp.Append( '#include "%s"\n' % outchh.Basename() )
				outcpp.Append( 'using namespace su;\n' )
				outcpp.Append( 'using namespace su::util;\n' )
				outcpp.Newline( 2 )
	
				# -- cpp base code
				if modns=='su':
					outcpp.AppendFile( '%s/gencode_mapping.cpp'%os.path.dirname(__file__) )

				# -- cpp private code
				outcpp.Append( 'namespace\n{\n' )
				outcpp.Newline( 2 )
				for o in objcode :
					outcpp.Newline( 2 )
					outcpp.Append( o["privcode"] )
				outcpp.Newline( 2 )
				outcpp.Append( '}\n' )

				# -- cpp public code
				for o in objcode :
					outcpp.Newline( 2 )
					outcpp.Append( '// %s\'s code\n' % o['uri'] )
					outcpp.Newline( 1 )
					outcpp.Append( o["pubcode"] )

				# -- cpp binarizer code
				outcpp.Newline( 2 )
				outcpp.Append( bincode )

				# -- cpp reverter code
				outcpp.Newline( 2 )
				outcpp.Append( revcode )

				# -- cpp footer
				outcpp.Newline( 3 )
				outcpp.Append( warnings )
				outcpp.Newline( 2 )

		return output




	def generate_stringids_lines ( ) :

		output = []
		
		for modef in sidl.SelectModuleDefinitions('gencode','stringids') :

			modns   = modef.SubstAttribute('namespace')
			outpath = modef.SubstAttribute('stringids')
			outbase = os.path.splitext( outpath )[0]
			assert( outpath )
			assert( outbase )

			chh = SIDL_Outputlines()
			cpp = SIDL_Outputlines()

			# generate private content

			ohash = {}
			for o in sidl.stringids.values() :
				if modef.MatchScope(o):
					h = genuid_hex( o.string )
					ohash[h] = o.string

			ohash_id  = {}
			ohash_str = []
			for i,h in enumerate(sorted(ohash.keys())) :
				ohash_str.append( (h,ohash[h]) )
				ohash_id[h] = i

			oid = {}
			for o in sidl.stringids.values() :
				if modef.MatchScope(o) :
					h = genuid_hex( o.string )
					oid[o.name] = ohash_id[h]+1

			# -- string-id's hashing
			cpp.Append( '#define LAST_ID\t%d\n' % len(ohash) )	# 1..N! (0 is reserved)
			cpp.Newline(1)
			cpp.Append( 'namespace\n{\n' )
			cpp.Newline(1)
			cpp.Append( '\tstruct sid_def { uint32 hash; pcstr str; };\n' )
			cpp.Newline(1)
			cpp.Append( '\tconst sid_def sid_list[] = {\n' )
			cpp.Append( '\t\t{ 0, NULL },\n' )
			for i,s in enumerate(ohash_str) :
				cpp.Append( '\t\t{ %s, "%s" }, // -- %d\n' % (s[0],s[1],i+1) )
			cpp.Append( '\t};\n' )

			# -- string-enum's array
			cpp.Newline(1)
			for o in sidl.stringenums.values() :
				if modef.MatchScope(o) :
					ids = []
					for m in o.members :
						id = str(oid[m])
						if id in ids :
							SIDL_Error( "String-id associated to '%s' is already defined in the enumeration '%s' !"%(m,o.name), o.loc )
						ids.append( id )
					cpp.Append( '\tconst short %s_ids[] = { %s };\n' % (o.name,', '.join(ids)) )
			cpp.Newline(1)

			# -- enum's functions
			cpp.Append( """
				StringId enum_find( pcstr a, const short ids[], int n )
				{
					uint32 h = util::hmac_crc32_w(a);
					for( short i=0 ; i<n ; i++ )
						if( sid_list[ids[i]].hash==h )
							return ids[i];
					return 0;
				}

				StringId enum_getnext( StringId a, const short ids[], int n )
				{
					for( short i=1 ; i<n ; i++ )
						if( ids[i-1]==a )
							return ids[i];
					return 0;
				}

				bool enum_has( StringId a, const short ids[], int n )
				{
					for( short i=0 ; i<n ; i++ )
						if( ids[i]==a )
							return true;
					return false;
				}
			""", indent='\t' )
			cpp.Newline(1)
			cpp.Append( '}\n' )
			cpp.Newline(2)


			# generate base classes string-id & string-enum

			chh.Newline(1)
			chh.Append( 'namespace %s { namespace sid\n{\n' % modns )
			chh.Newline(1)
			# --
			cpp.Append( 'int   su_sid_count() { return LAST_ID+1; }\n' )
			cpp.Append( 'void* su_sid_list()  { return (sid_def*)sid_list;  }\n' )
			cpp.Newline(2)


			# generate public string-ids

			def sort_by_name(d) :
				return sorted( d.values(), key=lambda x:x.name )


			cpp.Newline(2)
			for o in sort_by_name(sidl.stringids) :
				if modef.MatchScope(o) :
					if not o.private :
						chh.Append( '\tconst StringId %s = %d;\n' % (o.name,oid[o.name]) )


			cpp.Newline(2)
			for i,o in enumerate(sort_by_name(sidl.stringenums)) :
				if modef.MatchScope(o) :
					# -- chh
					chh.Newline(2)
					chh.Append( '\t// -- %s\n\n' % o.name )
					chh.Append( '\tstruct %s : public StringEnum\n\t{\n' % o.name )
					chh.Append( '\t\t\t\t\t\t\t\t%s ( ) { v=FIRST; }\n' % o.name )
					chh.Append( '\t\t\t\t\t\t\t\t%s ( const %s& inref ) { v=inref.v; }\n' % (o.name,o.name) )
					chh.Append( '\t\t\t\t\t\t\t\t%s ( StringId inref ) { v=inref; SU_ASSERT(has(v)); }\n' % o.name )
					chh.Append( '\t\t\t\t\t\t\t\t%s ( pcstr inref ) { v=find(inref); SU_ASSERT(has(v)); }\n' % o.name )
					chh.Append( '\t\t%s&%soperator = ( StringId inref ) { v=inref; SU_ASSERT(has(v)); return *this; };\n' % (o.name,' '*(23-len(o.name))) )
					chh.Append( '\t\tbool\t\t\t\t\toperator == ( const %s& inref ) const { return (inref.v==v); }\n' % o.name )
					chh.Append( '\t\t\n' )
					for m in o.members :
						chh.Append( '\t\tstatic const StringId\t%s = %d;\n' % (m,oid[m]) )
					chh.Append( '\t\t\n' )
					chh.Append( '\t\tstatic const uint\t\tCOUNT = %d;\n' % len(o.members) )
					chh.Append( '\t\tstatic const StringId\tFIRST = %s;\n' % oid[o.members[0]] )
					chh.Append( '\t\tstatic const StringId\tLAST = %s;\n' % oid[o.members[-1]] )
					chh.Append( '\t\tstatic const StringId\tTYPE = %s;\n' % oid[o.name] )
					chh.Append( '\t\tstatic StringId\t\t\tNEXT( StringId );\n' )
					chh.Append( '\t\tstatic StringId\t\t\tFIND( pcstr );\n' )
					chh.Append( '\t\tstatic bool\t\t\t\tHAS( StringId );\n' )
					chh.Append( '\t\tstatic bool\t\t\t\tHAS( pcstr inref ) { return FIND(inref)!=sid::null; }\n' )
					chh.Append( '\t\t\n' )
					chh.Append( '\t\tvirtual StringId\t\ttype ( ) const { return TYPE; }\n' )
					chh.Append( '\t\tvirtual StringId\t\tfirst ( ) const { return FIRST; }\n' )
					chh.Append( '\t\tvirtual StringId\t\tlast ( ) const { return LAST; }\n' )
					chh.Append( '\t\tvirtual StringId\t\tnext ( StringId inref ) const { return NEXT(inref); }\n' )
					chh.Append( '\t\tvirtual StringId\t\tfind ( pcstr inref ) const { return FIND(inref); }\n' )
					chh.Append( '\t\tvirtual bool\t\t\thas ( StringId inref ) const { return HAS(inref); }\n' )
					chh.Append( '\t\tvirtual bool\t\t\thas ( pcstr inref ) const { return HAS(inref); }\n' )
					chh.Append( '\t\t\n' )
					chh.Append( '\t\tvirtual StringId\t\tvalue ( ) const { return v; }\n' )
					chh.Append( '\tprotected:\n' )
					chh.Append( '\t\tStringId v;\n' )
					chh.Append( '\t};\n' )
					# -- cpp
					cpp.Newline( 2 )
					cpp.Append( '// -- %s\n' % o.name )
					cpp.Append( 'StringId %s::sid::%s::NEXT( StringId inref ) { return enum_getnext(inref,%s_ids,sizeof(%s_ids)/sizeof(short)); }\n' % (modns,o.name,o.name,o.name) )
					cpp.Append( 'bool %s::sid::%s::HAS( StringId inref ) { return enum_has(inref,%s_ids,sizeof(%s_ids)/sizeof(short)); }\n' % (modns,o.name,o.name,o.name) )
					cpp.Append( 'StringId %s::sid::%s::FIND( pcstr inref ) { return enum_find(inref,%s_ids,sizeof(%s_ids)/sizeof(short)); }\n' % (modns,o.name,o.name,o.name) )


			chh.Newline( 2 )
			chh.Append( '} }\n')

			# GENERATE CHH FILE

			outchh = SIDL_Outputfile( path='%s.h'%outbase, label='stringid file chh' )
			output.append( outchh )
			outchh.Append( header )
			outchh.Newline( 2 )
			outchh.Append( warnings )
			outchh.Newline( 2 )
			outchh.Append( '#ifndef _%s_\n' % outchh.BasenameAsCLabel() )
			outchh.Append( '#define _%s_\n' % outchh.BasenameAsCLabel() )
			outchh.Newline( 4 )
			outchh.Append( chh )
			outchh.Newline( 4 )
			outchh.Append( '#endif // _%s_\n' % outchh.BasenameAsCLabel() )
			outchh.Newline( 2 )
			outchh.Append( warnings )
			outchh.Newline( 2 )

			# GENERATE CPP FILE

			outcpp = SIDL_Outputfile( path='%s.cpp'%outbase, label='stringid file cpp' )
			output.append( outcpp )
			outcpp.Append( header )
			outcpp.Newline( 2 )
			outcpp.Append( warnings )
			outcpp.Newline( 2 )
			outcpp.Append( '#include <sufw/su_core.h>\n' )
			outcpp.Append( '#include "%s"\n' % outchh.Basename() )
			outcpp.Append( 'using namespace su;\n' )
			outcpp.Append( 'using namespace %s::sid;\n' % modns )
			outcpp.Newline( 2 )
			outcpp.Append( cpp )
			outcpp.Newline( 2 )
			outcpp.Append( warnings )
			outcpp.Newline( 2 )

		return output

		
		
		


	outputs = []


	# export interfaces
	for obj in sidl.interfaces.values() :
		outputs.extend( generate_intf_lines(obj) )

	# export implementations
	for obj in sidl.implementations.values() :
		outputs.extend( generate_impl_lines(obj) )

	# export tests
	for obj in sidl.tests.values() :
		outputs.extend( generate_test_lines(obj) )

	# export maps
	outputs.extend( generate_mapping_lines() )

	# export uid collection
	uids = {}
	for u in sidl.uids.values() :				uids[u.id] = u
	for u in sidl.interfaces.values() :			uids[u.classbase] = u
	for u in sidl.implementations.values() :	uids[u.classbase] = u
	outputs.extend( generate_uid_collection_lines(uids) )

	# export force references
	outputs.extend( generate_force_references_lines() )

	# export test collection
	outputs.extend( generate_test_collection_lines() )

	# export stringsids
	outputs.extend( generate_stringids_lines() )

	# export settings file
	outputs.extend( generate_settings_lines() )

	# writeback all outputs
	for o in [x for x in outputs if x] :
		o.Writeback(sidl,verbose)


