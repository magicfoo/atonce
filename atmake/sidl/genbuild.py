
import os, sys, os.path, copy, errno
import atmake
import atmake.tools.algo as algo
from atmake.tools.types import *
from atmake.tools import fs
from atmake.tools import builder
from atmake.sidl import subst_build
from .components import *
from .error import *
from .genuid import *


buildmodes 		= 	{ 	'exe'	: 'program',
						'elf'	: 'program',
						'so'	: 'program',
						'irx'	: 'program',
						'lib'	: 'archive',
						'dll'	: 'archive',
						'a'		: 'archive'
						}


def SIDL_GenBuild ( sidl=None, verbose=None ) :

	def collect_targets(sidl, turi, suri=None):
		assert(sidl)
		assert(turi)
		ret = []
		# if compile project from solution
		if suri :
			# grab project dependencies
			for deps in sidl.solutions[suri].dependencies.values() :
				for dep in deps :
					ret.append( sidl.projects[dep] )
			# grab project
			if turi in sidl.solutions[suri].projects :
				ret.append( sidl.projects[turi] )
			return ret
			
		# grab project or solution projects
		try :
			return [sidl.projects[turi]]
		except KeyError :
			for puri in sidl.solutions[turi].projects :
				ret.append(sidl.projects[puri])
			return ret
	
	def collect_buildsetup(sidl, uri) :
		assert(sidl)
		assert(uri)
		b = sidl.buildsetups[uri]
		return b
	
	def collect_envs(sidl, nodes, ctxt=None) :
		assert(sidl)
		rkeys = []
		for n in nodes :
			# load new build environnement variables
			if isinstance(n, SIDL_BuildEnv) :
				newenv, upkeys = load_configure(sidl, ctxt, verbose, n)
				rkeys.extend( upkeys )
				#add newenv to context env
				for k,v in newenv.items() :
					if k in list(ctxt['env'].keys()) :
						try :
							ctxt['env'][k] += v
						except :
							ctxt['env'][k] = v
					else : 
						ctxt['env'][k] = v
		return ( ctxt['env'], rkeys )

	def load_configure(sidl, ctxt=None, verbose=None, buildenv=None):
		assert(sidl)
		assert(buildenv)
		ret = {}
		keys = []
		# load build environnement variables from extern file
		if buildenv.config:
			filename = buildenv.config["filename"]
			try :
				withs = buildenv.config["params"]
			except KeyError :
				withs = None
			mod = imp.load_source("", filename)
			try :
				func_conf = getattr(mod, "configure")
				ret = func_conf( sidl, ctxt, verbose, withs )
			except AttributeError:
				SIDL_Error("No function configure in %s"%(filename), buildenv.loc)
		# load build environnement variables from sidl
		if buildenv.arguments :
			vars = dict([ s.strip().split(None,1) for s in buildenv.arguments.text.splitlines() if len(s) > 0])
			ret.update( vars )
			keys.extend( list(vars.keys()) )
		return (ret, keys)
		
	def collect_builders(sidl, nodes) :
		assert(sidl)
		assert(nodes)
		ret = []
		for n in nodes :
			# create builders from buildsetup
			if isinstance(n, SIDL_BuildStep) :
				if n.inputs :
					ins  = ' '.join(n.inputs)
				else :
					ins  = ''
				if n.outputs :
					outs = ' '.join(n.outputs)
				else :
					outs = ''
				acts = list(n.actions.GetText().splitlines())
				if n.qualifier == "multiple":
					b = builder.Builder(outs, ins, acts, name=n.uri, one2one=False)
				else :
					b = builder.Builder(outs, ins, acts, name=n.uri, one2one=True)
				ret.append(b)
		return ret
	
	def CollectFiles( sidl, node ) :
		assert(sidl)
		assert(node)
		matched = []
		for g in node.groups :
			# foreach groups collect files
			folders = sidl.groups[g].folders
			if len(folders) > 0 :
				for folder in folders :
					if 'excludedfrombuild' not in folder[2] and 'excluded' not in folder[0] :
						matched.extend(folder[1])
		return matched
	
	def FilterExts ( sources, extlist ) :
		assert(sources)
		matched = []
		if extlist :
			for e in extlist :
				if "*" not in e :
					match = fnmatch.filter([os.path.normpath(f) for f in sources], "*."+e)
				else :
					match = fnmatch.filter([os.path.normpath(f) for f in sources], e)
				matched.extend(match)
		return matched
		
	def Process ( ctxt, builder, actions, exec_actions=True ) :
		opt_silent_mode		= ctxt['env']['OPT_SILENT_MODE']
		opt_show_depends	= ctxt['env']['OPT_SHOW_DEPENDS']
		opt_show_tree		= ctxt['env']['OPT_SHOW_TREE']
		opt_show_actions	= ctxt['env']['OPT_SHOW_ACTIONS']
		opt_exec_actions	= ctxt['env']['OPT_EXEC_ACTIONS'] and exec_actions
		opt_show_outputs	= ctxt['env']['OPT_SHOW_OUTPUTS']
		opt_en_act_cache	= ctxt['env']['OPT_EN_ACTION_CACHE']
		opt_show_act_cache	= ctxt['env']['OPT_SHOW_ACTION_CACHE']
		msg_stdout			= ctxt['env']['BUILD_STDOUT']
		msg_stderr			= ctxt['env']['BUILD_STDERR']
		atmake.Assert( atmake.IsCallable(msg_stdout) )
		atmake.Assert( atmake.IsCallable(msg_stderr) )

		# display action tree ?
		if opt_show_tree or opt_show_depends :
			for b in ctxt['builders'] :
				print(b.name)
		
		action_cpt = 0
		
		
		# load cache
		signs = sign.BuildSignCache( ctxt )
		assert( signs )

		# check depends
		
		depends = []
		target = ctxt['TARGET']+'.'+builder.GetTargetExt()
		if signs and opt_en_act_cache :
			# depends = ctxt.CollectDepends( bf.sources )
			# get build state
			bstt = signs.IsUptodate( target , None, actions )
			if opt_show_act_cache :
				print( 'Action cache for target <%s>' % target )
				if   bstt == sign.BuildState.UP_TO_DATE :			print( '  => up to date.' )
				elif bstt == sign.BuildState.NO_CACHE_ENTRY :		print( '  => Not in cache !' )
				elif bstt == sign.BuildState.ACTIONS_CHANGED :		print( '  => Actions have changed !' )
				elif bstt == sign.BuildState.TARGET_FILE_CHANGED :	print( '  => The target file has changed !' )
				elif bstt == sign.BuildState.TARGET_FILE_DELETED :	print( '  => The target file is not found !' )
				elif bstt == sign.BuildState.DEPENDS_CHANGED :		print( '  => Depends have changed !' )
				elif bstt == sign.BuildState.DEPEND_FILE_CHANGED :	print( '  => A depend file has changed !' )
				elif bstt == sign.BuildState.DEPEND_FILE_DELETED :	print( '  => A depend file is not found !' )
			if bstt != sign.BuildState.UP_TO_DATE :
				# exec actions
				for a in actions :
					action_cpt += 1
					if opt_show_actions and (not opt_silent_mode) :
						print('>>>> %s' % str(a))
					if opt_exec_actions :
						res = Exec( action=a, env=ctxt['env'] )
						if res :
							rcode, xcode, oo, ee = res
							if not rcode :
								atmake.Abort( 'Command not found [%s] !' % a, 1 )
							if opt_show_outputs and ((xcode != 0) or (ee) or (not opt_silent_mode)) :
								msg_stdout( lines=oo, ctxt=ctxt )
								msg_stderr( lines=ee, ctxt=ctxt )
							if xcode != 0 :
								atmake.Abort( 'Command exit code: %s !' % str(xcode), xcode )
		
		# update the signatures build cache
		if signs and opt_en_act_cache :
			signs.SetUptodate( ctxt['TARGET'], None, actions )
		
		return action_cpt
		
	def Exec ( action, cwd=None, env=None ) :
		"""
			returns ( exec_success, exit_code, stdout lines, stderr lines )
		"""
		assert(action)
		assert(env)
		
		if 'SPAWN' not in env :
			return None
		spawner = env['SPAWN']

		if 'SPAWN_ENV' in env :		_env = env['SPAWN_ENV']
		elif env :					_env = env

		if 'buildroot' in env :		_cwd = env['buildroot']
		elif cwd :					_cwd = cwd

		res = spawner( action=action, cwd=_cwd, env=_env )
		return res
	
	def Build(ctxt, p, b, src=None, index=None):
		ctxt['SOURCES'] = src
		ret = None
		# collect target
		if b.IsOne2One(): 
			ctxt['TARGET'] = GetTargetOne2One(src=src, env=ctxt['env'], project=p.uri, index=index)
			if src :
				ret = ctxt['TARGET']+'.'+b.GetTargetExt()
			else :
				ret = ctxt['TARGET']
		else :
			ctxt['TARGET'] = GetTargetMany2One(env=ctxt['env'], project=p)
			ret = ctxt['TARGET']+'.'+b.GetTargetExt()
		# substitute environnement variables in builder action
		acts = b.SubstActions(ctxt)
		# launch actions
		Process(ctxt, b, acts, True)
		# return builded target
		return ret
	
	def collect_sources(sidl, ctxt, project, bs) :
		ret = ''
		index = 1
		# collect sources
		srcs = CollectFiles(sidl, project)
		srcs.extend(ctxt['SOURCES'])
		for b in bs :
			# check if were building archive or program
			if b.GetTargetExt() in buildmodes.keys():
				if project.attributes.get("build") != buildmodes.get(b.GetTargetExt()) :
					continue
			# filter sources and build it !
			csrcs = FilterExts( srcs, b.GetSourceExt() )
			if len(csrcs) > 0 :
				for src in csrcs :
					if b.IsOne2One() :
						ret = Build(ctxt, project, b, [src], index)
						srcs.append( ret )
						index += 1
				if not b.IsOne2One() :
					ret = Build(ctxt, project, b, csrcs, None)
					srcs.append( ret )
			# if source extension not defined
			if len(b.GetSourceExt()) == 0 :
				ret = Build(ctxt, project, b, None, None)
		return ret
	
	def GetTargetOne2One( src=None, env=None, project=None, index=None ) :
		assert(env)
		# for unary builder, build all in buildtmp folder
		tmptarget = env['buildroot']+'/buildtmp/'+project
		tmpbuild = tmptarget
		if not os.path.exists( tmpbuild ):
			os.makedirs( tmpbuild )
		if src :
			root, ext = os.path.splitext(src[0])
			target = root
			tmptarget = tmpbuild+'/'+str(index)
		
		return tmptarget
	
	def GetTargetMany2One( env=None, project=None ) :
		assert(env)
		assert(project)
		# for multiple builder, build target in $projoutpath folder
		tmptarget = None
		
		root = project.GetOutpath()
		tmptarget = root
		env['TOPTARGET'] = tmptarget
		
		return tmptarget
	
	# subst new env vars
	def format_env(ctxt, newkeys):
		for k in newkeys :
			ctxt['env'][k] = subst_build.Subst( ctxt, ctxt['env'].get(k) ) 
		return ctxt['env']
	
	def check_libext(x) :
		if not '.lib' in x :
			return x+'.lib'
		else :
			return x
	
	# additional dependencies (INCLUDES, LIBRARIES, ...)
	def collect_additionals(sidl, p, mode, ctxt) :
		
		# grab project attributes
		if 'INCLUDE' in ctxt['env'] :
			ctxt['env']['INCLUDE']		+= [e.strip() for e in p.attributes.get('cpppaths', [])]
		if 'LIB' in ctxt['env'] :
			ctxt['env']['LIB']			+= [e.strip() for e in p.attributes.get('linkpaths', [])]
		if 'LIBS' in ctxt['env'] :
			ctxt['env']['LIBS'] 			+= list( map( check_libext , p.attributes.get('linklibs', [])) )
		if 'CPPDEFINES' in ctxt['env'] :
			ctxt['env']['CPPDEFINES'] 	 	+= ' '+' '.join( p.attributes.get('cppdefines', '').split(';') )
	
		# grab project groups
		for guid in p.groups :
			g = sidl.groups[guid]
			# for release configuration grab additionals
			if "release" in mode or "rls" in mode :
				if not g.HasAttribute('configuration') or g.AttributeEquals('configuration',"release",cs=False) :
					cpppaths = g.GetAttributeList("cpppath")
					linkpaths = g.GetAttributeList("linkpath")
					cppdefines = g.GetAttributeList("cppdefines")
					linklibs = g.GetAttributeList("linklibs")
			# for debug configuration grab additionals
			elif "debug" in mode or "dbg" in mode :
				if g.AttributeEquals('configuration',"debug",cs=False) :
					cpppaths = g.GetAttributeList("cpppath")
					linkpaths = g.GetAttributeList("linkpath")
					cppdefines = g.GetAttributeList("cppdefines")
					linklibs = g.GetAttributeList("linklibs")
			# put additionals in env for subst
			if 'INCLUDE' in ctxt['env'] :
				ctxt['env']['INCLUDE']		+= [e.strip() for e in cpppaths]
			if 'LIB' in ctxt['env'] :
				ctxt['env']['LIB']			+= [e.strip() for e in linkpaths]
			if 'LIBS' in ctxt['env'] :
				ctxt['env']['LIBS'] 			+= list( map( check_libext, linklibs) )
			if 'CPPDEFINES' in ctxt['env'] :
				ctxt['env']['CPPDEFINES']		+= ' '.join(cppdefines)+' '
		return ctxt['env']

				
	# ----------------------------------------------------------------------------- #
	
	depends = []
	bs = sidl.builds
	# for each build tag
	for b in bs :
		ps = collect_targets(sidl, b.target, b.solution)
		# init
		ctxt = {}
		ctxt['SOURCES'] = depends
		ctxt['SOURCE'] = []
		ctxt['TARGET'] = []
		for p in ps :
			ctxt['builders'] = []
			# new build environnement
			ctxt['env'] = copy.deepcopy(p.env.globals)
			# collect setups
			bt = collect_buildsetup(sidl, b.tree)
			# collect environnement variables
			ctxt['env'], newkeys = collect_envs(sidl, bt.nodes, ctxt)
			# collect dependencies
			ctxt['env']	= collect_additionals(sidl, p, bt.uri.lower(), ctxt)
			# subst all that stuff
			ctxt['env'] = format_env(ctxt, newkeys)
			# make builders
			ctxt['builders'] = collect_builders(sidl, bt.nodes)
			# launch building tree
			t = collect_sources( sidl, ctxt, p, ctxt['builders'] )
			# append target to source for dependencies
			depends.append ( t.replace('/', '\\') )
		
	



