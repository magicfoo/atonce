
import os, sys, uuid, fnmatch
from atmake import *
import atmake.tools.fs as fs
import atmake.tools.algo as algo
from .components import *



msvc_common = {
		'CharacterSet':						'NotSet',		# NotSet, UNICODE (/D _UNICODE), Multi-bytes (/D _MBCS)
		'StringPooling':					'true',
		'MinimalRebuild':					'false',
		'ExceptionHandling':				'false',
		'BasicRuntimeChecks':				'Default',
		'TreatWChar_tAsBuiltInType':		'true',
		'RuntimeTypeInfo':					'false',
		'BrowseInformationFile':			'$(IntDir)',
		'WarningLevel':						'Level3',
		'EnableEnhancedInstructionSet':		'NotSet',		# NotSet, SSE, SSE2, SSE3(x64 only)
		'LinkLibraryDependencies':			'true',
		'CppAdditionalOptions':				[],
		'LinkAdditionalOptions':			[],
		'LibAdditionalOptions':	  			[],
		'LinkIncremental':					'Yes',			# Default, No, Yes
		'SubSystem':						'Windows',		# Console, Windows
		'Culture':							'1033',			# 1033: Anglais (US),	1036: Francais (France)
		'GenerateManifest':					'false',
		'ManifestFile':						'',
		'PostBuildEventUseInBuild':			'true',
}


msvc_configurations = {
	'Debug' : {
		'TargetSuffix':						'_dbg',
		'Optimization':						'Disabled',
		'FavorSizeOrSpeed':					'Neither',
		'BrowseInformation':				'true',
		'GenerateDebugInformation':			'true',
		'DebugInformationFormat':			'EditAndContinue',
		'ProgramDataBaseFileName':			'$(TargetDir)$(TargetName).pdb',
		'RuntimeLibrary':					'MultiThreadedDebugDLL',
		'AdditionalIncludeDirectories':		[],
		'PrecompiledHeader':				'NotUsing',			# Use, Create, NotUsing
		'PreprocessorDefinitions':			[],
		'AdditionalDependencies':			[],
		'AdditionalLibraryDirectories':		[],
	},

	'Release' : {
		'TargetSuffix':						'_rls',
		'Optimization':						'MaxSpeed',
		'FavorSizeOrSpeed':					'Speed',
		'BrowseInformation':				'false',
		'GenerateDebugInformation':			'false',
		'DebugInformationFormat':			'ProgramDatabase',
		'ProgramDataBaseFileName':			'$(TargetDir)$(TargetName).pdb',
		'RuntimeLibrary':					'MultiThreadedDLL',
		'AdditionalIncludeDirectories':		[],
		'PrecompiledHeader':				'NotUsing',			# Use, Create, NotUsing
		'PreprocessorDefinitions':			[],
		'AdditionalDependencies':			[],
		'AdditionalLibraryDirectories':		[],
		'AdditionalLibraryDirectories':		[],
	},
}





_re_cfile_inc1 = re.compile( '#include[^"]*"([^"]*)"' )		#   #inlude "..."
_re_cfile_inc2 = re.compile( '#include[^<]*<([^>]*)>' )		#   #inlude <...>

build_action_launcher = """

# ---

import sys, os, inspect

args = {}
for a in sys.argv[1:] :
	if '=' in a:	n,v = a.split('=',1)
	else:			n,v = a,None
	try:			args[n] = int(v)
	except:			args[n] = v

if 'call' in args :
	try:	locals()[ args['call'] ]( args )
	except:	pass

"""


def Generate ( sidl=None, solutionname=None, verbose=None, precompiled=None ) :


	def ncsequals(a,b) :
		if a==b :		return True
		if a is None:	return False
		if b is None:	return False
		return a.lower()==b.lower()

	def assert_list( d, k ) :
		if not k in d:	d[k]=[]

	def assert_dict( d, k ) :
		if not k in d:	d[k]={}

	def find_solution ( in_name ) :
		return sidl.solutions.get( in_name, None )

	def find_project ( in_name ) :
		return sidl.projects.get( in_name, None )

	def find_group ( in_name ) :
		return sidl.groups.get( in_name, None )

	def extendlines ( l1, l2, level=1 ) :
		l2 = [l+'\n' for l in ''.join(l2).splitlines()]
		for l in l2 :
			l1.append( '\t'*level+l )
		return l1

	def msvc_uuid ( label ) :
		return str( uuid.uuid3(uuid.NAMESPACE_URL,label) ).upper()

	def msvc_stringvalue ( v ) :
		v = str(v).strip().replace('"','&quot;')
		v = v.replace( '$(InputPath)', '%(FullPath)' )
		v = v.replace( '$(InputName)', '%(Filename)' )
		v = v.replace( '$(InputFileName)', '%(Filename).%(Extension)' )
		v = v.replace( '$(InputDir)', '%(Directory)' )
		v = v.replace( '$(InputExt)', '%(Extension)' )
		return v

	def collect_source_headers ( infile, insizehint=None ) :
		hfiles = []
		lines = open(infile,'r').readlines( insizehint )
		for l in lines :
			so = _re_cfile_inc1.search(l)
			if so :
				if not so.group(1) in hfiles :
					hfiles.append( so.group(1) )
			else :
				so = _re_cfile_inc2.search(l)
				if so :
					if not so.group(1) in hfiles :
						hfiles.append( so.group(1) )
		return hfiles
		
	def filter_libs ( libs, prefix=None, suffix=None ) :
		r = []
		for l in libs :
			if l :
				if not l.startswith(prefix or ''):	l = '%s%s' % (prefix or '', l)
				if not l.endswith(suffix or ''):	l = '%s%s' % (l, suffix or '')
				if l and (not l in r):				r.append( l )
		return r

	def collect_conf_groups ( inproj, confname ) :
		G = []
		for gname in inproj.groups :
			g = find_group( gname )
			if g and (not g.HasAttribute('configuration') or g.AttributeEquals('configuration',confname,cs=False)) :
				G.append( g )
		return G

	def collect_project_attributes ( inproj, attrnames, confname ) :
		L = []
		G = collect_conf_groups( inproj, confname )
		for a in ToList(attrnames) :
			l = []
			# from global env
			l.extend( sidl.env.GetVarlist(a) )
			# from project direct attributes
			l.extend( inproj.GetAttributeList(a) )
			# from project groups
			for g in G :
				l.extend( g.GetAttributeList(a) )
			# extend the global list
			for ll in l :
				if not ll in L:
					L.append( ll )
		return L

	def conf_options ( options ) :
		# sort options by configuration
		copts = {}
		for olist in options :
			conf = 'all'
			opts = []
			for o in olist:
				if o.startswith('configuration'):	conf = o.split(':')[1].lower()
				else:								opts.append(o)
			assert_list( copts, conf )
			copts[conf].extend( opts )
		return copts

	def builditems_list ( inproj ) :
		fileorder = {}
		filelist = []
		filefilter = sidl.SubstModuleAttribute( 'genproj', 'filefilter', obj=inproj )
		for gname in inproj.groups :
			g = find_group( gname )
			if not g:	continue
			confname = g.GetAttribute( 'configuration' )
			for f in g.folders :	# [ (filter_path,[files],[options]) ]
				ffilter,files,options = f[0],f[1],f[2]
				files = [x for x in files if algo.mfilters(x,filefilter,True)]
				if confname:	options.append( 'configuration:%s'%confname )
				for f in files:
					nf = os.path.normpath(f)
					if not nf in fileorder:
						fileorder[nf] = len(filelist)
						filelist.append( {'file':nf, 'filter':ffilter, 'options':[]} )
					if len(options) :
						filelist[ fileorder[nf] ]['options'].append( options )
		# filter options by configuration name
		for f in filelist :
			f['options'] = conf_options( f['options'] )
		return filelist

	def is_builditem_excluded_from_cfg ( inoptions, incfg ) :
		included_elsewhere = None
		for cname,copts in inoptions.items() :
			if cname=='all' or ncsequals(cname,incfg) :
				# explicitly in this configuration
				if 'excludedfrombuild' in copts:	return True
				else:								return False
			else :
				# implicitly in another configuration
				included_elsewhere = True
		if included_elsewhere:		return True
		else:						return False

	def is_builditem_excluded_from_all_cfgs ( inoptions ) :
		for cfg in msvc_configurations.keys() :
			if not is_builditem_excluded_from_cfg(inoptions,cfg) :
				return False
		return True

	def is_builditem_included_in_all_cfgs ( inoptions ) :
		for cfg in msvc_configurations.keys() :
			if is_builditem_excluded_from_cfg(inoptions,cfg) :
				return False
		return True

	def builditems_custombuilds ( inproj ) :
		# collect custom builds
		custombuilds = {}
		for cfg in msvc_configurations.keys() :
			cbs  = collect_project_attributes(inproj,'custombuild',cfg)
			cbs += collect_project_attributes(inproj,'vscustombuild',cfg)
			for cb in cbs :
				c = cb.split(',')
				while len(c)<4:	c.append( None )
				cbargs = {}
				cbargs['CommandLine']	= c[1] or '?'
				cbargs['Outputs']		= c[2] or '%(FullPath)'
				cbargs['Message']		= c[3] or 'custom building file %(Filename).%(Extension) ...'
				pattern = c[0] if ('*' in c[0]) or ('?' in c[0]) else '*.'+c[0]
				assert_dict( custombuilds, cfg )
				custombuilds[cfg][pattern] = cbargs
		return custombuilds

	def builditem_ext ( inpath ) :
		ext = GetFileExt( inpath )
		if ext:	ext = ext.lower()
		return ext

	def builditem_type ( inpath, incustombuilds ) :
		for cfg,patterns in incustombuilds.items() :
			for pattern in patterns.keys() :
				if fnmatch.fnmatch(inpath,pattern) :
					return 'CustomBuild'
		ext = builditem_ext( inpath )
		if ext in ['h','hpp','hxx','h++']:		return 'ClInclude'
		if ext in ['c','cpp','cxx','c++']:		return 'ClCompile'
		if ext in ['lib']:						return 'Library'
		return 'None'




	# generate a pre/post action

	def gen_build_action ( inproj, stage ) :
		# stage is 'prebuild' or 'postbuild'
		build = inproj.attributes.get( stage )
		if build :
			act, args = build
			actobj = sidl.actions.get( act )
			if actobj.file :
				return actobj.file, {}
			else :
				pypath = inproj.GetOutpath( defext='2010.%s.vcevent_%s.py'%(sidl.GetPlatform().lower(),stage) )
				action = SIDL_Outputfile( source=inproj, path=pypath, label="MSVC %s event script '%s'"%(stage,inproj.uri) )
				action.Append( actobj.text )
				action.Append( build_action_launcher )
				outputs.append( action )
				return pypath, args




	# generate project's user-file for visual studio

	def gen_project_user( outfile, inproj ) :
		outfile.Append( '<?xml version="1.0" encoding="utf-8"?>\n' )
		nproject = SIDL_Outputnode('Project').Define('xmlns','http://schemas.microsoft.com/developer/msbuild/2003') \
											 .Define('ToolsVersion','4.0')

		for cfg in msvc_configurations.keys() :
			cond = "'$(Configuration)|$(Platform)'=='%s|Win32'" % cfg
			g = nproject.AddNode('PropertyGroup').Define('Condition',cond)
			g.AddNode('LocalDebuggerCommand').Append( collect_project_attributes(inproj,'debugcommand',cfg) or '$(TargetPath)' )
			g.AddNode('LocalDebuggerCommandArguments').Append( collect_project_attributes(inproj,'debugargs',cfg) or '' )
			g.AddNode('LocalDebuggerWorkingDirectory').Append( collect_project_attributes(inproj,'debugdir',cfg) or '$(TargetDir)' )
			g.AddNode('LocalDebuggerEnvironment').Append( collect_project_attributes(inproj,'debugenv',cfg) or '' )
			g.AddNode('LocalDebuggerMergeEnvironment').Append( collect_project_attributes(inproj,'debugenvmerge',cfg) or 'true' )
			g.AddNode('DebuggerFlavor').Append('WindowsLocalDebugger')

		outfile.Append( nproject )
		return 1




	# generate project's filters-file for visual studio

	def gen_project_filters( outfile, inproj, builditems ) :

		def register_filter_rec ( f ) :
			if not f:			return
			if not IsList(f):	f = f.split('.')
			register_filter_rec( f[0:-1] )
			s = '\\'.join(f)
			filters[s] = msvc_uuid(s)

		outfile.Append( '<?xml version="1.0" encoding="utf-8"?>\n' )
		nproject = SIDL_Outputnode('Project').Define('xmlns','http://schemas.microsoft.com/developer/msbuild/2003') \
											 .Define('ToolsVersion','4.0')

		g = nproject.AddNode('ItemGroup')

		# generate filters
		filters = {}
		for i in builditems :	# [ {'file':str, 'type':str, 'filter':str, 'options':[]} ]
			register_filter_rec( i['filter'] )
		for f in sorted(filters.keys()) :
			g.AddNode('Filter').Define('Include',f).AddNode('UniqueIdentifier').Append('{%s}'%filters[f])

		# generate items
		for i in builditems :	# [ {'file':str, 'type':str, 'filter':str, 'options':[]} ]
			relpath = fs.FileToFile( i['file'], outfile.path )
			g.AddNode(i['type']).Define('Include',relpath).AddNode('Filter').Append(i['filter'].replace('.','\\'))

		outfile.Append( nproject )
		return 1




	def gen_project_builditem_metadata ( innode, initem, incustombuilds ) :
		type	= initem['type']
		file	= initem['file']
		options = initem['options']
		name	= os.path.basename( file )

		# None type ...
		if type=='None' :
			pass

		# CustomBuild ?
		elif type=='CustomBuild' :
			for cfg in msvc_configurations.keys() :
				for pattern,cb in incustombuilds[cfg].items() :
					if fnmatch.fnmatch(file,pattern) :
						cond = "'$(Configuration)|$(Platform)'=='%s|Win32'" % cfg
						innode.AddNode('Command').Define('Condition',cond).Append( msvc_stringvalue(cb['CommandLine']) )
						innode.AddNode('Outputs').Define('Condition',cond).Append( msvc_stringvalue(cb['Outputs']) )
						innode.AddNode('Message').Define('Condition',cond).Append( msvc_stringvalue(cb['Message']) )

		# Others
		else :

			# create precompiled header ?
			if precompiled and type=='ClCompile' and name.startswith('supch_') :
				hfile = os.path.splitext(file)[0]+'.h'
				if os.path.isfile(hfile) :
					innode.AddNode('PrecompiledHeader').Append('Create')
					innode.AddNode('PrecompiledHeaderFile').Append('%(Dirname)%(Filename).h')
					innode.AddNode('PrecompiledHeaderOutputFile').Append('$(IntDir)%(Filename).pch')
			# use precompiled header ?
			elif precompiled and type=='ClCompile' :
				for h in collect_source_headers(file,1024*8) :
					b = os.path.basename(h)
					if b.startswith('supch_') and b.endswith('.h') :
						innode.AddNode('PrecompiledHeader').Append('Use')
						innode.AddNode('PrecompiledHeaderFile').Append(h)
						innode.AddNode('PrecompiledHeaderOutputFile').Append( '$(IntDir)%s.pch'%os.path.splitext(b)[0] )
						break	# considers only the first one !
			# always included ?
			if not is_builditem_included_in_all_cfgs(options) :
				for cfg in msvc_configurations.keys() :
					cond = "'$(Configuration)|$(Platform)'=='%s|Win32'" % cfg
					if is_builditem_excluded_from_cfg(options,cfg):	excl = 'true'
					else:											excl = 'false'
					innode.AddNode('ExcludedFromBuild').Define('Condition',cond).Append(excl)




	def gen_project_builditems( outfile, inproj, independencies, nitems ) :
		builditems   = builditems_list( inproj )			# [ {'file':str, 'filter':str, 'options':[]} ]
		custombuilds = builditems_custombuilds( inproj )

		for i in builditems :
			type = builditem_type(i['file'],custombuilds)
			if is_builditem_excluded_from_all_cfgs(i['options']) :
				type = 'None'	# never included => see http://msdn.microsoft.com/en-us/library/0c6xyb66.aspx
			i['type'] = type
			relpath = fs.FileToFile(i['file'],outfile.path)
			n = nitems.AddNode(type).Define('Include',relpath)
			gen_project_builditem_metadata(  n, i, custombuilds )

		# Project References
		if independencies :
			for dep in independencies :
				pdep = find_project(dep)
				pdeppath  = pdep.GetOutpath( defext='2010.%s.vcxproj'%sidl.GetPlatform().lower() )
				pdeppathr = fs.FileToFile( pdeppath, outfile.path )
				ref = nitems.AddNode('ProjectReference').Define('Include',pdeppathr)
				ref.AddNode('Project').Append('{%s}'%msvc_uuid(dep))
				ref.AddNode('ReferenceOutputAssembly').Append('false')
				ref.AddNode('LinkLibraryDependencies').Append('true')

		return builditems




	# generate global properties

	def gen_project_global_buildproperties ( innode, inproj, incfg ) :
		innode.AddNode('ProjectGuid').Append( '{%s}'%msvc_uuid(inproj.uri) )
		innode.AddNode('ProjectName').Append( inproj.uri )
		innode.AddNode('Keyword').Append( 'Win32Proj' )



	
	# generate pre-default properties

	def gen_project_pre_buildproperties ( innode, inproj, incfg ) :
		projbuild = inproj.GetAttribute('build')  or 'program'
		conftype = {'program':'Application', 'archive':'StaticLibrary', 'dll':'DynamicLibrary'}[projbuild]
		innode.AddNode('ConfigurationType').Append( conftype )
		innode.AddNode('CharacterSet').Append( msvc_common['CharacterSet'] )




	# generate post-default properties

	def gen_project_post_buildproperties ( innode, inproj, incfg ) :
		msvccf = msvc_configurations[incfg]
		projbuild = inproj.GetAttribute('build')  or 'program'
		projdir   = sidl.SubstModuleAttribute('genproj','tmpdir',obj=inproj) or '$(SolutionDir)'
		buildtmp  = os.path.normpath( '%s/msvc2010/$(ProjectName) - $(Configuration)/'%projdir )

		target = inproj.GetAttribute('target')
		if not target:	target = inproj.GetOutpath()
		else:			target = inproj.env.Subst(target)

		targetpath = os.path.normpath( target+msvccf['TargetSuffix'] )
		targetext  = {'program':'.exe', 'archive':'.lib', 'dll':'.dll'}[projbuild]

		innode.AddNode('_ProjectFileVersion').Append('10.0.30319.1')
		innode.AddNode('LinkIncremental').Append( msvc_common['LinkIncremental'] )
		innode.AddNode('IntDir').Append( buildtmp+'\\' )
		innode.AddNode('OutDir').Append( os.path.dirname(targetpath)+'\\' )
		innode.AddNode('TargetDir').Append( os.path.dirname(targetpath) )
		innode.AddNode('TargetName').Append( os.path.basename(targetpath) )
		innode.AddNode('TargetExt').Append( targetext )
		innode.AddNode('TargetSuffix').Append( msvccf['TargetSuffix'] )
		innode.AddNode('TargetFilename').Append( '$(TargetName)$(TargetExt)' )
		innode.AddNode('TargetPath').Append( targetpath+targetext )
		innode.AddNode('GenerateManifest').Append( msvc_common['GenerateManifest'] )




	# generate build-item's definitions

	def gen_project_builddefinitions ( innode, inproj, incfg ) :
		msvcco = msvc_common
		msvccf = msvc_configurations[incfg]

		AdditionalIncludeDirectories	= msvccf['AdditionalIncludeDirectories'] + collect_project_attributes(inproj,'cpppath',incfg)
		PreprocessorDefinitions			= msvccf['PreprocessorDefinitions']      + collect_project_attributes(inproj,'cppdefines',incfg)
		AdditionalLibraryDirectories	= msvccf['AdditionalLibraryDirectories'] + collect_project_attributes(inproj,'linkpath',incfg)
		AdditionalDependencies			= msvccf['AdditionalDependencies']       + collect_project_attributes(inproj,'linklibs',incfg)
		CppAdditionalOptions			= msvcco['CppAdditionalOptions']         + collect_project_attributes(inproj,'cppoptions',incfg)
		LinkAdditionalOptions			= msvcco['LinkAdditionalOptions']		 + collect_project_attributes(inproj,'linkoptions',incfg)
		LibAdditionalOptions			= msvcco['LibAdditionalOptions']	     + collect_project_attributes(inproj,'liboptions',incfg)
		Culture							= collect_project_attributes(inproj,'culture',incfg) or msvcco['Culture']

		if 1:	# PreBuildEvent
			prebuild = gen_build_action( inproj, 'prebuild' )
			if prebuild :
				actpath, actargs = prebuild
				alist = [ '%s=&quot;%s&quot;'%(str(n),str(v)) for n,v in actargs.items() ] if actargs else []
				acmdl = '$(ATPYTHON) -u "%s" call="prebuild" project=%%(FullPath) target=$(TargetPath) %s' % (actpath,' '.join(alist))
				g = innode.AddNode('PreBuildEvent').AddNode('Command').Append( msvc_stringvalue(acmdl) )

		if 1:	# ClCompile
			g = innode.AddNode('ClCompile')
			g.AddNode('Optimization').Append( msvccf['Optimization'] )
			g.AddNode('FavorSizeOrSpeed').Append( msvccf['FavorSizeOrSpeed'] )
			g.AddNode('AdditionalOptions').Append( ' '.join(CppAdditionalOptions)+' %(AdditionalOptions)' )
			g.AddNode('AdditionalIncludeDirectories').Append( ';'.join(AdditionalIncludeDirectories)+';%(AdditionalIncludeDirectories)' )
			g.AddNode('PreprocessorDefinitions').Append( ';'.join(PreprocessorDefinitions)+';%(PreprocessorDefinitions)' )
			g.AddNode('StringPooling').Append( msvcco['StringPooling'] )
			g.AddNode('MinimalRebuild').Append( msvcco['StringPooling'] )
			g.AddNode('ExceptionHandling').Append( msvcco['ExceptionHandling'] )
			g.AddNode('BasicRuntimeChecks').Append( msvcco['BasicRuntimeChecks'] )
			g.AddNode('RuntimeLibrary').Append( msvccf['RuntimeLibrary'] )
			g.AddNode('TreatWChar_tAsBuiltInType').Append( msvcco['TreatWChar_tAsBuiltInType'] )
			g.AddNode('RuntimeTypeInfo').Append('false')
			g.AddNode('PrecompiledHeader').Append( msvccf['PrecompiledHeader'] )
			g.AddNode('BrowseInformation').Append( msvccf['BrowseInformation'] )
			g.AddNode('BrowseInformationFile').Append( msvcco['BrowseInformationFile'] )
			g.AddNode('WarningLevel').Append( msvcco['WarningLevel'] )
			g.AddNode('EnableEnhancedInstructionSet').Append('NotSet')
			g.AddNode('GenerateDebugInformation').Append( msvccf['GenerateDebugInformation'] )
			g.AddNode('DebugInformationFormat').Append( msvccf['DebugInformationFormat'] )
			g.AddNode('ProgramDataBaseFileName').Append( msvccf['ProgramDataBaseFileName'] )

		if 1 :	# ResourceCompile
			g = innode.AddNode('ResourceCompile')
			g.AddNode('PreprocessorDefinitions').Append( ';'.join(PreprocessorDefinitions)+';%(PreprocessorDefinitions)' )
			g.AddNode('Culture').Append(Culture)
			g.AddNode('AdditionalIncludeDirectories').Append( ';'.join(AdditionalIncludeDirectories)+';%(AdditionalIncludeDirectories)' )

		if 1 :	# Lib
			g = innode.AddNode('Lib')
			g.AddNode('OutputFile').Append( '$(TargetDir)$(TargetName).lib' )
			g.AddNode('AdditionalDependencies').Append( ';'.join(filter_libs(AdditionalDependencies,suffix='.lib'))+';%(AdditionalDependencies)' )
			g.AddNode('AdditionalLibraryDirectories').Append( ';'.join(filter_libs(AdditionalLibraryDirectories))+';%(AdditionalLibraryDirectories)' )
			g.AddNode('AdditionalOptions').Append( ' '.join(LibAdditionalOptions)+' %(AdditionalOptions)' )
			g.AddNode('SubSystem').Append( msvcco['SubSystem'] )

		if 1 :	# Link
			g = innode.AddNode('Link')
			g.AddNode('OutputFile').Append( '$(TargetDir)$(TargetName).exe' )
			g.AddNode('GenerateDebugInformation').Append( msvccf['GenerateDebugInformation'] )
			g.AddNode('AdditionalDependencies').Append( ';'.join(filter_libs(AdditionalDependencies,suffix='.lib'))+';%(AdditionalDependencies)' )
			g.AddNode('AdditionalLibraryDirectories').Append( ';'.join(filter_libs(AdditionalLibraryDirectories))+';%(AdditionalLibraryDirectories)' )
			g.AddNode('AdditionalOptions').Append( ' '.join(LinkAdditionalOptions)+' %(AdditionalOptions)' )
			g.AddNode('SubSystem').Append( msvcco['SubSystem'] )
			g.AddNode('ManifestFile').Append( msvcco['ManifestFile'] )

		if 1 :	# ProjectReference
			g = innode.AddNode('ProjectReference')
			g.AddNode('LinkLibraryDependencies').Append( msvcco['LinkLibraryDependencies'] )

		if 1 :	# Bscmake
			g = innode.AddNode('Bscmake')
			g.AddNode('OutputFile').Append( '$(TargetDir)$(TargetName).bsc' )

		if 1:	# PostBuildEvent
			postbuild = gen_build_action( inproj, 'postbuild' )
			if postbuild :
				actpath, actargs = postbuild
				alist = [ '%s=&quot;%s&quot;'%(str(n),str(v)) for n,v in actargs.items() ] if actargs else []
				acmdl = '$(ATPYTHON) -u "%s" call="postbuild" project=%%(FullPath) target=$(TargetPath) %s' % (actpath,' '.join(alist))
				g = innode.AddNode('PostBuildEvent').AddNode('Command').Append( msvc_stringvalue(acmdl) )




	# generate .VXCPROJ file

	def gen_project ( outfile, inproj, independencies ) :
		outfile.Append( '<?xml version="1.0" encoding="utf-8"?>\n' )

		# Project
		nproject = SIDL_Outputnode('Project')
		nproject.Define('DefaultTargets','Build')
		nproject.Define('ToolsVersion','4.0')
		nproject.Define('xmlns','http://schemas.microsoft.com/developer/msbuild/2003')
		
		# 'ProjectConfigurations'
		n = nproject.AddNode('ItemGroup').Define('Label','ProjectConfigurations')
		for cfg in msvc_configurations.keys() :
			nn = n.AddNode('ProjectConfiguration').Define('Include','%s|Win32'%cfg)
			nn.AddNode('Configuration').Append(cfg)
			nn.AddNode('Platform').Append('Win32')

		# Globals
		for cfg in msvc_configurations.keys() :
			n = nproject.AddNode('PropertyGroup').Define('Condition',"'$(Configuration)|$(Platform)'=='%s|Win32'"%cfg).Define('Label','Globals')
			gen_project_global_buildproperties( n, inproj, cfg )

		# Import base properties
		nproject.AddNode('Import').Define('Project','$(VCTargetsPath)\Microsoft.Cpp.Default.props')

		# Configuration
		for cfg in msvc_configurations.keys() :
			n = nproject.AddNode('PropertyGroup').Define('Condition',"'$(Configuration)|$(Platform)'=='%s|Win32'"%cfg).Define('Label','Configuration')
			gen_project_pre_buildproperties( n, inproj, cfg )

		# Import default properties
		nproject.AddNode('Import').Define('Project','$(VCTargetsPath)\Microsoft.Cpp.props')

		# Local User Property Sheets
		for cfg in msvc_configurations.keys() :
			n = nproject.AddNode('ImportGroup').Define('Condition',"'$(Configuration)|$(Platform)'=='%s|Win32'"%cfg).Define('Label','PropertySheets')
			n.AddNode('Import').Define('Project','$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props') \
							   .Define('Condition',"exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')") \
							   .Define('Label','LocalAppDataPlatform')

		# Project Property Sheets
		for cfg in msvc_configurations.keys() :
			n = nproject.AddNode('PropertyGroup').Define('Condition',"'$(Configuration)|$(Platform)'=='%s|Win32'"%cfg)
			gen_project_post_buildproperties( n, inproj, cfg )

		# Project Items Definitions
		for cfg in msvc_configurations.keys() :
			n = nproject.AddNode('ItemDefinitionGroup').Define('Condition',"'$(Configuration)|$(Platform)'=='%s|Win32'"%cfg)
			gen_project_builddefinitions( n, inproj, cfg )

		# Project Items themselves
		builditems = gen_project_builditems( outfile, inproj, independencies, nproject.AddNode('ItemGroup').Define('Label','Items') )

		# Default targets
		nproject.AddNode('Import').Define('Project','$(VCTargetsPath)\Microsoft.Cpp.targets')

		# Debug targets
		n = nproject.AddNode('Target').Define('Name','CpCompileFiles')
		n.AddNode('Message').Define('Text',"@(ClCompile->'%(Filename)%(Extension)')")
		n = nproject.AddNode('Target').Define('Name','CpIncludeFiles')
		n.AddNode('Message').Define('Text',"@(ClInclude->'%(Filename)%(Extension)')")

		# Extension targets
		nproject.AddNode('ImportGroup').Define('Label','ExtensionTargets')
		  
		outfile.Append( nproject )
		return builditems




	# generate .VCSIDL file

	def gen_project_vcsidl ( outfile, inproj ) :
		outfile.Append( '<?xml version="1.0" encoding="Windows-1252"?>\n' )

		n_vs = SIDL_Outputnode('VisualStudioSIDL')
		n_files = n_vs.AddNode('Files')
		ofiles = sidl.ShelfAccess( 'outfiles', {} )
		collectfiles = sidl.CollectProjectFiles( inproj.uri )
		for f in collectfiles :
			fpath = fs.Path(f)
			finfo = ofiles.get( fpath, None )
			if finfo:
				inprojpath = finfo.get('outpath',fpath)
				if inprojpath==fpath :
					n_file = n_files.AddNode('File')
					for k,v in finfo.items() :
						n_file.Define(k,str(v))

		outfile.Append( n_vs )




	# generate .SLN lines

	def gen_solution ( solobj ) :
		outfile = SIDL_Outputfile( path=solobj.GetOutpath(defext='2010.%s.sln'%sidl.GetPlatform().lower()), source=solobj, label="MSVC 2010 solution '%s'"%solobj.uri, readonly=False )
		outputs.append( outfile )

		# MSVC 2010
		outfile.Append( 'Microsoft Visual Studio Solution File, Format Version 11.00\n' )
		outfile.Append( '# Visual Studio 2010\n' )

		puuid_list = []
		for pname in solobj.projects :
			pobj = find_project( pname )
			if pobj :
				# generates .vcxproj
				pdeps = solobj.dependencies.get( pname, [] )
				puuid = msvc_uuid( pobj.uri )
				puuid_list.append( puuid )
				ppath = pobj.GetOutpath( defext='2010.%s.vcxproj'%sidl.GetPlatform().lower() )
				pfile = SIDL_Outputfile( path=ppath, source=pobj, label="MSVC 2010 project '%s'"%pobj.uri, readonly=False )
				builditems = gen_project( pfile, pobj, pdeps )
				outputs.append( pfile )

				# append project file
				relpath = fs.FileToFile( ppath, outfile.path )
				outfile.Append( 'Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "%s", "%s", "{%s}"\n' % (pobj.uri,relpath,puuid) )
				for pdep in pdeps :
					pdepuuid = msvc_uuid( pdep )
					outfile.Append( 'ProjectSection(ProjectDependencies) = postProject\n' )
					outfile.Append( '\t{%s} = {%s}\n' % (pdepuuid,pdepuuid) )
					outfile.Append( 'EndProjectSection\n' )
				outfile.Append( 'EndProject\n' )

				# generates .vcxproj.filters
				fpath = pobj.GetOutpath( defext='2010.%s.vcxproj.filters'%sidl.GetPlatform().lower() )
				ffile = SIDL_Outputfile( path=fpath, source=pobj, label="MSVC 2010 filters '%s'"%pobj.uri, readonly=False )
				gen_project_filters( ffile, pobj, builditems )
				outputs.append( ffile )

				# generates .vcxproj.user
				upath = pobj.GetOutpath( defext='2010.%s.vcxproj.user'%sidl.GetPlatform().lower() )
				ufile = SIDL_Outputfile( path=upath, source=pobj, label="MSVC 2010 user '%s'"%pobj.uri, readonly=False )
				gen_project_user( ufile, pobj )
				outputs.append( ufile )

				# generate .vcxsidl metadata file
				vcpath = pobj.GetOutpath( defext='2010.%s.vcxsidl'%sidl.GetPlatform().lower() )
				vcfile = SIDL_Outputfile( path=vcpath, source=pobj, label="MSVC 2010 sidl '%s'"%pobj.uri )
				gen_project_vcsidl( vcfile, pobj )
				outputs.append( vcfile )

		# outputs globals
		outfile.Append( 'Global\n' )
		if 1 :
			# GlobalSection(SolutionConfigurationPlatforms) = preSolution
			outfile.Append( '\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n' )
			for cfg in msvc_configurations.keys() :
				outfile.Append( '\t\t%s|Win32 = %s|Win32\n' % (cfg,cfg) )
			outfile.Append( '\tEndGlobalSection\n' )
			# ProjectConfigurationPlatforms
			outfile.Append( '\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n' )
			for puuid in puuid_list :
				for cfg in msvc_configurations.keys() :
					outfile.Append( '\t\t{%s}.%s|Win32.ActiveCfg = %s|Win32\n' % (puuid,cfg,cfg) )
					outfile.Append( '\t\t{%s}.%s|Win32.Build.0 = %s|Win32\n' % (puuid,cfg,cfg) )
			outfile.Append( '\tEndGlobalSection\n' )
			# ProjectConfigurationPlatforms
			outfile.Append( '\tGlobalSection(SolutionProperties) = preSolution\n' )
			outfile.Append( '\t\tHideSolutionNode = FALSE\n' )
			outfile.Append( '\tEndGlobalSection\n' )
		outfile.Append( 'EndGlobal\n' )

		# register directories to clean up
		sidl.ShelfOutfile( '%s/tmp' % os.path.dirname(outfile.path) )


	if sidl is None:
		return

	platform = sidl.GetPlatform()
	if not platform in ['WINDOWS','PSP2'] :
		return

	outputs = []

	if solutionname:
		# find the solution to generate
		s = find_solution(solutionname)
		if not s:	print("sidl: MSVC solution '%s' is not defined !" % (solutionname))
		solutions = [ s ]
	else :
		solutions = sidl.solutions.values()

	# generate all solutions
	for s in solutions :
		gen_solution( s )

	# writeback all outputs
	for o in [x for x in outputs if x] :
		o.Writeback(sidl,verbose)


