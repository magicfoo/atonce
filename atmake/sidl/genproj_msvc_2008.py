
import os, sys, uuid, fnmatch
from atmake import *
import atmake.tools.fs as fs
import atmake.tools.algo as algo
from .components import *




# see http://msdn.microsoft.com/en-us/library/2208a1f2(VS.71).aspx
# for a documented schema.

msvc_configurations = {
	'debug' : {
		'Optimization':						'0',
		'FavorSizeOrSpeed':					'0',
		'BrowseInformation':				'1',
		'BasicRuntimeChecks':				'0',		# 0:Defaults, 3:Both (stack frames & uninitialized vars)
		'RuntimeLibrary':					'3',
		'AdditionalIncludeDirectories':		[],
		'PreprocessorDefinitions':			[],
		'DebugInformationFormat':			'4',		# Program Database for Edit & Continue
		'GenerateDebugInformation':			'true',
		'TargetSuffix':						'_dbg',
		'AdditionalDependencies':			[],
		'AdditionalLibraryDirectories':		[],
	},

	'release' : {
		'Optimization':						'2',
		'FavorSizeOrSpeed':					'1',
		'BrowseInformation':				'0',
		'BasicRuntimeChecks':				'0',
		'RuntimeLibrary':					'2',
		'AdditionalIncludeDirectories':		[],
		'PreprocessorDefinitions':			[],
		'DebugInformationFormat':			'3',		# Program Database (required to enable Minimal Rebuild /Gm)
		'GenerateDebugInformation':			'false',
		'TargetSuffix':						'_rls',
		'AdditionalDependencies':			[],
		'AdditionalLibraryDirectories':		[],
	},
}

msvc_common = {
	'ConfigurationType':					'2',
	'CharacterSet':							'0',			# 0:Not Set, 1:UNICODE (/D _UNICODE), 2:Multi-bytes (/D _MBCS)
	'StringPooling':						'true',
	'MinimalRebuild':						'false',
	'ExceptionHandling':					'0',			# 0:No, 1:Yes (calls dtors for automatic objects during a stack unwind)
	'TreatWChar_tAsBuiltInType':			'true',
	'RuntimeTypeInfo':						'false',
	'UsePrecompiledHeader':					'0',
	'BrowseInformationFile':				'$(IntDir)\\',
	'WarningLevel':							'3',
	'Detect64BitPortabilityProblems':		'false',
	'EnableEnhancedInstructionSet':			'0',			# 0:Not set, 1:SSE, 2:SSE2, 3:SSE3 (x64 only)
	'LinkLibraryDependencies':				'true',
	'CppAdditionalOptions':					[],
	'LinkAdditionalOptions':				[],
	'LibAdditionalOptions':	  				[],
	'LinkIncremental':						'0',			# 0:Default, 1:No, 2:Yes
	'SubSystem':							'0',
	'TargetMachine':						'1',
	'ResourceCompilerCulture':				'1033',			# 1033: Anglais (US),	1036: Francais (France)
	'CompileAs':							'0',			# 0:Default, 1:C, 2:C++
}

#pragma warning( disable : 4018 )		// signed/unsigned mismatch
#pragma warning( disable : 4244 )		// conversion from 'X' to 'Y', possible loss of data
#pragma warning( disable : 4067 )
#pragma warning( disable : 4100 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4201 )
#pragma warning( disable : 4251 )		// 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning( disable : 4275 )
#pragma warning( disable : 4311 )
#pragma warning( disable : 4312 )
#pragma warning( disable : 4503 )		// mangling name exceeds the maximum name lenght
#pragma warning( disable : 4530 )		// C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc



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




def assert_list( d, k ) :
	if not k in d:	d[k]=[]


def assert_dict( d, k ) :
	if not k in d:	d[k]={}


def filter_libs ( libs, prefix=None, suffix=None ) :
	r = []
	for l in libs :
		if l :
			if not l.startswith(prefix or '') :
				l = '%s%s' % (prefix or '', l)
			if not l.endswith(suffix or '') :
				l = '%s%s' % (l, suffix or '')
			if l and (not l in r) :
				r.append( l )
	return r


def msvc_uuid ( label ) :
	return str( uuid.uuid3(uuid.NAMESPACE_URL,label) ).upper()


def msvc_stringvalue ( v ) :
	s = str(v).strip()
	s = s.replace(r'"','&quot;')
	s = s.replace(r'\r','&#x0D;')
	s = s.replace(r'\n','&#x0A;')
	return s



	
	
def Generate ( sidl=None, solutionname=None, verbose=None, precompiled=None ) :

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

	def find_conf_groups ( projobj, confname ) :
		G = []
		for gname in projobj.groups :
			g = find_group( gname )
			if g :
				if not g.HasAttribute('configuration') or g.AttributeEquals('configuration',confname) :
					G.append( g )
		return G

	def collect_project_attributes ( projobj, attrnames, confname ) :
		L = []
		G = find_conf_groups( projobj, confname )
		for a in ToList(attrnames) :
			l = []
			# from global env
			l.extend( sidl.env.GetVarlist(a) )
			# from project direct attributes
			l.extend( projobj.GetAttributeList(a) )
			# from project groups
			for g in G :
				l.extend( g.GetAttributeList(a) )
			# extend the global list
			for ll in l :
				if not ll in L:
					L.append( ll )
		return L

	def conf_options ( options ) :
		copts = {}
		for olist in options :
			conf = 'all'
			opts = []
			for o in olist:
				if o.startswith('configuration'):	conf = o.split(':')[1]
				else:								opts.append( o )
			assert_list( copts, conf )
			copts[conf].extend( opts )
		return copts


	# generate a VCPROJ file entry

	def gen_file ( projpath, filepath, custombuilds=None, options=None ) :
		lines   = []
		relpath = fs.FileToFile( filepath, projpath )
		lines.append( '<File\n\tRelativePath="%s"\n\t>\n' % relpath )

		# sort options by configuration
		coptions = conf_options( options )
		
		for confname in msvc_configurations.keys():
			# excluded from build ?
			if 1:
				excluded = False
				for cname,copts in coptions.items() :
					cname = confname if cname=='all' else cname 
					if cname!=confname:					excluded = True
					if 'excludedfrombuild' in copts:	excluded = True
				if excluded :
					lines.append( '\t<FileConfiguration\n' )
					lines.append( '\t'*2 + 'Name="%s|Win32"\n' % confname )
					lines.append( '\t'*2 + 'ExcludedFromBuild="true"\n' )
					lines.append( '\t'*2 + '>\n' )
					lines.append( '\t</FileConfiguration>\n' )
					continue
			# custom build ?
			if custombuilds:
				for pattern,cb in custombuilds[confname].items() :
					if fnmatch.fnmatch(filepath,pattern) :
						lines.append( '\t<FileConfiguration\n' )
						lines.append( '\t'*2 + 'Name="%s|Win32"\n' % confname )
						lines.append( '\t'*2 + '>\n' )
						lines.append( '\t'*2 + '<Tool\n' )
						lines.append( '\t'*3 + 'Name="VCCustomBuildTool"\n' )
						lines.append( '\t'*3 + 'CommandLine="%s"\n' % (msvc_stringvalue(cb['CommandLine'])) )
						lines.append( '\t'*3 + 'Outputs="%s"\n' % (msvc_stringvalue(cb['Outputs'])) )
						lines.append( '\t'*3 + 'Description="%s"\n' % (msvc_stringvalue(cb['Description'])) )
						lines.append( '\t'*2 + '/>\n' )
						lines.append( '\t</FileConfiguration>\n' )
					
		lines.append( '</File>\n' )
		return lines


	# generate a VCPROJ filter

	def gen_filter ( projpath, fname, fvalue, custombuilds=None ) :
		lines = []
		if fname=='_files' :
			for f in sorted(fvalue.keys()) :
				options = fvalue[f]
				flines = gen_file( projpath, filepath=f, custombuilds=custombuilds, options=options )
				extendlines( lines, flines, level=0 )
		elif IsDict(fvalue) :
			lines.append( '<Filter Name="%s">\n' % fname )
			for k,v in fvalue.items() :
				sublines = gen_filter( projpath, k, v, custombuilds  )
				extendlines( lines, sublines )
			lines.append( '</Filter>\n' )
		return lines


	# generate VCPROJ files

	def gen_files ( projpath, projobj ) :

		def create_rec ( root, uri ) :
			if not uri:				return root
			if IsString(uri):		uri = uri.split('.')
			if not uri[0]:			return root
			if not uri[0] in root:	root[uri[0]] = {}
			return create_rec( root[uri[0]], uri[1:] )

		# create the filters tree
		filters = {}
		filefilter = sidl.SubstModuleAttribute( 'genproj', 'filefilter', obj=projobj )
		for gname in projobj.groups :
			g = find_group( gname )
			if g :
				folders  = g.CollectFolders()
				confname = g.GetAttribute('configuration')
				for f in folders :
					# f is ( filter_path, [files], [options] )
					fo		= create_rec( filters, f[0] )
					files   = [x for x in f[1] if algo.mfilters(x,filefilter,True)]
					options = f[2]
					if confname:
						options.append( 'configuration:%s'%confname )
					assert_dict(fo,'_files')
					for f in files:
						assert_list(fo['_files'],f)
						fo['_files'][f].append( options )

		# collect custom builds
		custombuilds = {}
		for confname in msvc_configurations.keys() :
			custombuilds[ confname ] = {}
			cbs  = collect_project_attributes(projobj,'custombuild',confname)
			cbs += collect_project_attributes(projobj,'vscustombuild',confname)
			for cb in cbs :
				c = cb.split(',')
				while len(c)<4:	c.append( None )
				cbargs = {}
				cbargs['CommandLine']	= c[1] or '?'
				cbargs['Outputs']		= c[2] or '$(InputPath)'
				cbargs['Description']	= c[3] or 'custom building file $(InputFileName) ...'
				pattern = c[0] if ('*' in c[0]) or ('?' in c[0]) else '*.'+c[0]
				custombuilds[confname][ pattern ] = cbargs

		lines = []
		lines.append( '<Files>\n' )
		for k,v in filters.items() :
			filter_lines = gen_filter( projpath=projpath, fname=k, fvalue=v, custombuilds=custombuilds )
			extendlines( lines, filter_lines )
		lines.append( '</Files>\n' )
		return lines


	# generate VCPROJ tool files

	def gen_toolfiles ( projpath=None, projobj=None ) :
		lines = []
		lines.append( '<ToolFiles>\n' )
		lines.append( '</ToolFiles>\n' )
		return lines


	# generate VCPROJ platforms definitions

	def gen_platforms ( projpath=None, projobj=None ) :
		lines = []
		lines.append( '<Platforms>\n' )
		lines.append( '\t<Platform Name="Win32"/>\n' )
		lines.append( '</Platforms>\n' )
		return lines


	# generate a pre/post action

	def gen_build_action ( output, projobj, stage ) :
		# stage is 'prebuild' or 'postbuild'
		build = projobj.attributes.get( stage )
		if build :
			act, args = build
			actobj = sidl.actions.get( act )
			if actobj.file :
				return actobj.file, {}
			else :
				pypath = projobj.GetOutpath( defext='2008.%s.vcevent_%s.py'%(sidl.GetPlatform().lower(),stage) )
				outaction = SIDL_Outputfile( source=projobj, path=pypath, label="MSVC %s script '%s'"%(stage,projobj.uri) )
				outaction.Append( actobj.text )
				outaction.Append( build_action_launcher )
				output.append( outaction )
				return pypath, args

	
	def filter_platform_libs ( libs ) :
		pfm = sidl.GetPlatform();
		if pfm=='WINDOWS':
			return filter_libs(libs,suffix='.lib')
		elif pfm=='PSP2':
			return filter_libs(libs,prefix='-l')


	def get_platform_ext( ) :
		pfm = sidl.GetPlatform();
		if pfm=='WINDOWS':
			return {'program':'exe', 'archive':'lib', 'dll':'dll'}
		elif pfm=='PSP2':
			return {'program':'self', 'archive':'a', 'dll':'prx'}


	# generate a VCPROJ configuration (debug, release, ...)

	def gen_configuration ( output, confname, projobj, projpath ) :

		msvcco = msvc_common
		msvccf = msvc_configurations[confname]

		projbuild = projobj.GetAttribute('build')  or 'program'
		buildtmp  = '%s/msvc2008/$(ProjectName) - $(ConfigurationName)' % (sidl.SubstModuleAttribute('genproj','tmpdir',obj=projobj) or '$(SolutionDir)')
		platformext = get_platform_ext()

		target = projobj.GetAttribute('target')
		if not target:	target = projobj.GetOutpath()
		else:			target = projobj.env.Subst( target )

		targetpath = '%s%s' % (target,msvccf['TargetSuffix'])
		targetdir  = os.path.dirname( targetpath )
		targetbase = os.path.basename( targetpath )
		targetext  = platformext[projbuild]
		targetfile = '%s.%s' % (targetpath,targetext)

		cpppath			= msvccf['AdditionalIncludeDirectories'] + collect_project_attributes(projobj,'cpppath',confname)
		cppdefines		= msvccf['PreprocessorDefinitions']      + collect_project_attributes(projobj,'cppdefines',confname)
		cppoptions		= msvcco['CppAdditionalOptions']		 + collect_project_attributes(projobj,'cppoptions',confname)
		linkpath		= msvccf['AdditionalLibraryDirectories'] + collect_project_attributes(projobj,'linkpath',confname)
		linklibs		= msvccf['AdditionalDependencies']       + collect_project_attributes(projobj,'linklibs',confname)
		linkoptions		= msvcco['LinkAdditionalOptions']		 + collect_project_attributes(projobj,'linkoptions',confname)
		liboptions	    = msvcco['LibAdditionalOptions']	     + collect_project_attributes(projobj,'liboptions',confname)
		rcculture		= collect_project_attributes(projobj,'culture',confname) or msvcco['ResourceCompilerCulture']

		lines = []
		lines.append( '\n\t<Configuration' )
		lines.append( '\n\t\tName="%s|Win32"' % confname )
		lines.append( '\n\t\tOutputDirectory="%s"' % targetdir )
		lines.append( '\n\t\tIntermediateDirectory="%s"' % buildtmp )

		conftype = {'program':1, 'archive':4, 'dll':2}[projbuild]
		lines.append( '\n\tConfigurationType="%d"' % conftype )

		lines.append( '\n\t\tCharacterSet="%s"' % msvcco['CharacterSet'] )
		lines.append( '\n\t>' )

		# prebuilder
		lines.append( '\n\t<Tool Name="VCPreBuildEventTool"' )
		prebuild = gen_build_action( output, projobj, 'prebuild' )
		if prebuild :
			actpath, actargs = prebuild
			relpath = fs.FileToFile( actpath, projpath )
			alist = [ '%s=&quot;%s&quot;'%(str(n),str(v)) for n,v in actargs.items() ] if actargs else ''
			lines.append( '\n\t\tCommandLine="%%ATPYTHON%% -u &quot;%s&quot; call=&quot;prebuild&quot; project=$(InputPath) target=$(TargetPath) %s"' % (relpath,' '.join(alist)) )
		lines.append( '\n\t/>')

		lines.append( '\n\t<Tool Name="VCCustomBuildTool" />')
		lines.append( '\n\t<Tool Name="VCXMLDataGeneratorTool" />')
		lines.append( '\n\t<Tool Name="VCWebServiceProxyGeneratorTool" />')
		lines.append( '\n\t<Tool Name="VCMIDLTool" />')

		# compiler options
		lines.append( '\n\t<Tool Name="VCCLCompilerTool"')
		if 1 :
			lines.append( '\n\t\tOptimization="%s"' % msvccf['Optimization'] )
			lines.append( '\n\t\tFavorSizeOrSpeed="%s"' % msvccf['FavorSizeOrSpeed'] )
			lines.append( '\n\t\tAdditionalOptions="%s"' % ' '.join(cppoptions) )
			lines.append( '\n\t\tAdditionalIncludeDirectories="%s"' % ';'.join(cpppath) )
			lines.append( '\n\t\tPreprocessorDefinitions="%s"' % ';'.join(cppdefines).replace('"','\\&quot;') )
			lines.append( '\n\t\tStringPooling="%s"' % msvcco['StringPooling'])
			lines.append( '\n\t\tMinimalRebuild="%s"' % msvcco['MinimalRebuild'])
			lines.append( '\n\t\tExceptionHandling="%s"' % msvcco['ExceptionHandling'])
			lines.append( '\n\t\tBasicRuntimeChecks="%s"' % msvccf['BasicRuntimeChecks'])
			lines.append( '\n\t\tRuntimeLibrary="%s"' % msvccf['RuntimeLibrary'])
			lines.append( '\n\t\tTreatWChar_tAsBuiltInType="%s"' % msvcco['TreatWChar_tAsBuiltInType'])
			lines.append( '\n\t\tRuntimeTypeInfo="%s"' % msvcco['RuntimeTypeInfo'])
			lines.append( '\n\t\tUsePrecompiledHeader="%s"' % msvcco['UsePrecompiledHeader'])
			lines.append( '\n\t\tBrowseInformation="%s"' % msvccf['BrowseInformation'])
			lines.append( '\n\t\tBrowseInformationFile="%s"' % msvcco['BrowseInformationFile'])
			lines.append( '\n\t\tWarningLevel="%s"' % msvcco['WarningLevel'])
			lines.append( '\n\t\tDetect64BitPortabilityProblems="%s"' % msvcco['Detect64BitPortabilityProblems'])
			lines.append( '\n\t\tEnableEnhancedInstructionSet="%s"' % msvcco['EnableEnhancedInstructionSet'])
			lines.append( '\n\t\tDebugInformationFormat="%s"' % msvccf['DebugInformationFormat'])
			lines.append( '\n\t\tCompileAs="%s"' % msvcco['CompileAs'])
		lines.append( '\n\t/>')

		# archiver options
		lines.append( '\n\t<Tool Name="VCLibrarianTool"')
		lines.append( '\n\t\tOutputFile="%s.%s"' % (targetpath,platformext['archive']) )
		lines.append( '\n\t\tLinkLibraryDependencies="%s"' % msvcco['LinkLibraryDependencies'])
		lines.append( '\n\t\tAdditionalOptions="%s"' % ' '.join(liboptions) )
		lines.append( '\n\t/>')

		# program linker options
		lines.append( '\n\t<Tool Name="VCLinkerTool"')
		if 1:
			deffile = projobj.GetAttribute('deffile')
			lines.append( '\n\t\tOutputFile="%s.%s"' % (targetpath,platformext['program']) )
			lines.append( '\n\t\tLinkLibraryDependencies="%s"' % msvcco['LinkLibraryDependencies'])
			lines.append( '\n\t\tLinkIncremental="%s"' % msvcco['LinkIncremental'])
			lines.append( '\n\t\tAdditionalDependencies="%s"' % ' '.join(filter_platform_libs(linklibs)) )
			lines.append( '\n\t\tAdditionalLibraryDirectories="%s"' % ';'.join(filter_libs(linkpath)) )
			lines.append( '\n\t\tAdditionalOptions="%s"' % ' '.join(linkoptions) )
			lines.append( '\n\t\tModuleDefinitionFile="%s"' % (deffile or '') )
			lines.append( '\n\t\tGenerateDebugInformation="%s"' % msvccf['GenerateDebugInformation'])
			lines.append( '\n\t\tSubSystem="%s"' % msvcco['SubSystem'])
			lines.append( '\n\t\tTargetMachine="%s"' % msvcco['TargetMachine'])
		lines.append( '\n\t/>')

		# resource compiler options
		lines.append( '\n\t<Tool Name="VCResourceCompilerTool"')
		if 1:
			lines.append( '\n\t\tPreprocessorDefinitions="%s"' % ';'.join(cppdefines).replace('"','\\&quot;') )
			lines.append( '\n\t\tCulture="%s"' % ToList(rcculture)[0] )
			lines.append( '\n\t\tAdditionalIncludeDirectories="%s"' % ';'.join(cpppath) )
		lines.append( '\n\t/>')

		# .bsc maker options
		lines.append( '\n\t<Tool Name="VCBscMakeTool"')
		if 1:
			lines.append( '\n\t\tOutputFile="%s.%s"' % (targetpath,'bsc') )
		lines.append( '\n\t/>')
		
		lines.append( '\n\t<Tool Name="VCManagedResourceCompilerTool" />')
		lines.append( '\n\t<Tool Name="VCPreLinkEventTool" />')
		lines.append( '\n\t<Tool Name="VCALinkTool" />')
		lines.append( '\n\t<Tool Name="VCManifestTool" />')
		lines.append( '\n\t<Tool Name="VCXDCMakeTool" />')
		lines.append( '\n\t<Tool Name="VCFxCopTool" />')
		lines.append( '\n\t<Tool Name="VCAppVerifierTool" />')
		lines.append( '\n\t<Tool Name="VCWebDeploymentTool" />')

		# postbuilder
		lines.append( '\n\t<Tool Name="VCPostBuildEventTool"')
		postbuild = gen_build_action( output, projobj, 'postbuild' )
		if postbuild :
			actpath, actargs = postbuild
			relpath = fs.FileToFile( actpath, projpath )
			alist = [ '%s=&quot;%s&quot;'%(str(n),str(v)) for n,v in actargs.items() ] if actargs else ''
			lines.append( '\n\t\tCommandLine="%%ATPYTHON%% -u &quot;%s&quot; call=&quot;postbuild&quot; project=$(InputPath) target=$(TargetPath) %s"' % (relpath,' '.join(alist)) )
		lines.append( '\n\t/>')

		lines.append( '\n\t</Configuration>' )
		return lines


	# generate all VCPROJ configurations

	def gen_configurations ( output, projpath=None, projobj=None ) :
		lines = []
		lines.append( '<Configurations>' )
		for confname in msvc_configurations.keys():
			conflines = gen_configuration( output, confname, projobj, projpath )
			extendlines( lines, conflines, level=0 )
		lines.append( '</Configurations>\n' )
		return lines


	# generate VCPROJ lines

	def gen_project ( output, projpath, projobj ) :
		platforms_lines = gen_platforms(projpath)
		toolfiles_lines = gen_toolfiles(projpath)
		config_lines    = gen_configurations(output,projpath,projobj)
		files_lines     = gen_files(projpath,projobj)
		projuuid		= msvc_uuid( projobj.uri )

		lines = []
		lines.append( '<?xml version="1.0" encoding="Windows-1252"?>\n' )
		lines.append( '<VisualStudioProject\n' )
		lines.append( '\tProjectType="Visual C++"\n' )
		# 2008
		lines.append( '\tVersion="9,00"\n' )
		lines.append( '\tName="%s"\n' % projobj.uri )
		lines.append( '\tProjectGUID="{%s}" \n' % projuuid )
		lines.append( '\t>\n' )
		extendlines( lines, platforms_lines + toolfiles_lines + config_lines + files_lines )
		lines.append( '</VisualStudioProject>\n' )
		return lines


	# generate VCSIDL lines

	def gen_vcsidl ( output, projobj, collectfiles ) :
		outpath = projobj.GetOutpath( defext='2008.%s.vcsidl'%sidl.GetPlatform().lower() )
		outfile = SIDL_Outputfile( path=outpath, source=projobj, label="MSVC sidl-meta '%s'"%projobj.uri )
		output.append( outfile )

		outfile.Append( '<?xml version="1.0" encoding="Windows-1252"?>\n' )
		outfile.Append( '<VisualStudioSIDL>\n' )
		outfile.Append( '<Files>\n' )
		ofiles = sidl.ShelfAccess( 'outfiles', {} )
		for f in collectfiles :
			fpath = fs.Path(f)
			finfo = ofiles.get( fpath, None )
			if finfo:
				outpath = finfo.get('outpath',fpath)
				if outpath==fpath :
					attributes = '\n\t'.join([ '%s="%s"' % (k,str(v)) for k,v in finfo.items() ])
					outfile.Append( '<File\n\t%s\n/>\n' % attributes )
		outfile.Append( '</Files>\n' )
		outfile.Append( '</VisualStudioSIDL>\n' )


	# generate SLN lines

	def gen_solution ( output, solobj ) :

		outsol = SIDL_Outputfile(	path	= solobj.GetOutpath(defext='2008.%s.sln'%sidl.GetPlatform().lower()),
									source	= solobj,
									label	= "MSVC 2008 solution '%s'" % solobj.uri,
									readonly= False	)
		output.append( outsol )

		# MSVC 2008
		outsol.Append( 'Microsoft Visual Studio Solution File, Format Version 10.00\n' )
		outsol.Append( '# Visual Studio 2008\n' )

		for pname in solobj.projects :
			pobj = find_project( pname )
			if pobj :
				# generates project lines
				puuid = msvc_uuid( pobj.uri )
				ppath = pobj.GetOutpath( defext='2008.%s.vcproj'%sidl.GetPlatform().lower() )
				projlines = gen_project( output, ppath, pobj )

				if projlines :
					relpath = fs.FileToFile( ppath, outsol.path )
					outsol.Append( 'Project("{{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}}") = "%s", "%s", "{%s}"\n' % (pobj.uri,relpath,puuid) )
					pdeps = solobj.dependencies.get( pname, [] )
					for pdep in pdeps :
						outsol.Append( '# dep with %s\n' % pdep )
						pdepuuid = msvc_uuid( pdep )
						outsol.Append( 'ProjectSection(ProjectDependencies) = postProject\n' )
						outsol.Append( '\t{%s} = {%s}\n' % (pdepuuid,pdepuuid) )
						outsol.Append( 'EndProjectSection\n' )
					outsol.Append( 'EndProject\n' )

					# outputs the project file
					outproj = SIDL_Outputfile(	path	= ppath,
												source	= pobj,
												label	= "MSVC 2008 project '%s'"%pobj.uri,
												lines	= projlines,
												readonly= False	)
					output.append( outproj )

					# outputs the project's metadata file
					projfiles = sidl.CollectProjectFiles( pname )
					gen_vcsidl( output, pobj, collectfiles=projfiles )

		# register directories to clean up
		sidl.ShelfOutfile( '%s/tmp' % os.path.dirname(outsol.path) )



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
		gen_solution( outputs, s )

	# writeback all outputs
	for o in [x for x in outputs if x] :
		o.Writeback(sidl,verbose)



