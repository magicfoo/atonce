
import os, sys, uuid
import atmake
import atmake.tools.fs as fs
from .components import *




# var = andsysroot,
#  cf get_subst_conf

# per target :
# -Wl,-soname,$(LIB_FULL_NAME)

android_base = {
	'config' : {
		# Compile Part
		'Defines':		[ 'SU_HW_ANDROID', 'ANDROID', '__ARM_ARCH_5__', '__ARM_ARCH_5T__', '__ARM_ARCH_5E__', '__ARM_ARCH_5TE__' ],		# CFLAGS -D
		'IncludeDir' :	[ '{andsysroot}/usr/include' ],			# CFLAGS -I
		'CFLAGS_m' :	[ 'arch=armv5te', 'tune=xscale', 'soft-float', 'thumb' ],
		'CFLAGS_f' :	[ 'no-exceptions', 'pic', 'function-sections', 'unwind-tables', 'stack-protector', 'omit-frame-pointer', 'no-strict-aliasing', 'inline-limit=64' ],
		'CFLAGS' :		'-Wno-psabi -Wno-multichar -Wa,--noexecstack -Os -O0 -g',
		
		'CFLAGS_cpp' :	'-fno-rtti -fpermissive',
		
		# Link Part
		'LinkLib' :		[ 'c', 'm', 'stdc++', 'supc++', 'log', 'GLESv2' ],		# LFLAGS -l
		'LFLAGS' :		'-Wl,-rpath-link={andsysroot}/usr/lib -Wl,--no-undefined -Wl,-z,noexecstack -shared --sysroot={andsysroot}'
	}
}

target_config = {
	'Debug' : {
		# Compile Part
		'Defines':		[ 'SU_DEBUG', 'DEBUG' ],					# CFLAGS -D
		'IncludeDir' :	[],											# CFLAGS -I
		'CFLAGS_m' :	[],
		'CFLAGS_f' :	[],
		'CFLAGS' :		'',
		
		# Link Part
		'LinkLib' :		[],											# LFLAGS -l
		'LFLAGS' :		'',
		
		# genmake util
		'TargetExt' :	'_DBG'
	},
	'Release' : {
		# Compile Part
		'Defines':		[ 'SU_RELEASE', 'SU_NDEBUG', 'NDEBUG' ],	# CFLAGS -D
		'IncludeDir' :	[],											# CFLAGS -I
		'CFLAGS_m' :	[],
		'CFLAGS_f' :	[],
		'CFLAGS' :		'',
		
		# Link Part
		'LinkLib' :		[],											# LFLAGS -l
		'LFLAGS' :		'',
		
		# genmake util
		'TargetExt' :	'_RLS'
	}
}

g_compile_file_ext = [ 'cpp', 'c' ]




def get_subst_conf	( inVal, inAndsysRoot ) :
	return inVal.replace( '{andsysroot}', inAndsysRoot );
def get_projname		(	inPObj	) :
	return inPObj.uri.replace( '.', '_' )
def get_projname_lite	(	inPObj	) :
	return get_projname( inPObj ).replace( 'lib', 'L' ).replace( 'program', 'P' ).replace( '_', '' )

def get_vname_objlst	(	inPObj	) :
	return 'OBJ_%s' % get_projname_lite( inPObj )
def get_vname_objdir	(	inPObj	) :
	return 'OBJDIR_%s' % get_projname_lite( inPObj )
def get_vname_objdirmk	(	inPObj	) :
	return 'OBJDIRMK_%s' % get_projname_lite( inPObj )
def get_vname_cflags	(	inPObj	) :
	return 'CFLAGS_%s' % get_projname_lite( inPObj )
def get_vname_lflags	(	inPObj	) :
	return 'LFLAGS_%s' % get_projname_lite( inPObj )
def get_vname_target	(	inPObj	) :
	return 'TARGET_%s' % get_projname_lite( inPObj )


def get_vval_objdir		(	inPObj, inConfName	) :
	return '%s_tmp_android/%s' % ( get_projname( inPObj ), inConfName )
def get_vval_objdirmk	(	inPObj, inConfName	) :
	return '%s_tmp_android\\%s' % ( get_projname( inPObj ), inConfName )
def get_vval_objdirbase	(	inPObj	) :
	return '%s_tmp_android' % ( get_projname( inPObj ) )

	
	
def build_cflags_lflags( inConfig ) :
	cflags = ''
	lflags = ''
	for var, val in inConfig.items() :
		if var == 'Defines' :
			for v in val :
				cflags += '-D%s ' %( v )
		elif var == 'IncludeDir' :
			for v in val :
				cflags += '-I%s ' %( v )
		elif var == 'CFLAGS_m' :
			for v in val :
				cflags += '-m%s ' %( v )
		elif var == 'CFLAGS_f' :
			for v in val :
				cflags += '-f%s ' %( v )
		elif var == 'CFLAGS' :
			cflags += val + ' '
		elif var == 'LinkLib' :
			for v in val :
				lflags += '-l%s ' %( v )
		elif var == 'LFLAGS' :
			lflags += val + ' '
	return cflags, lflags


	
	
	
	
def get_targetdir		(	inPObj				) :
	t = inPObj.GetAttribute('target')
	if not t:	t = inPObj.GetOutpath()
	else:		t = inPObj.env.Subst( t )
	return os.path.dirname( t )

def get_targetbasename	(	inPObj, inSuffix	) :
	t = inPObj.GetAttribute('target')
	if not t:	t = inPObj.GetOutpath()
	else:		t = inPObj.env.Subst( t )
	tp = '%s%s' % ( t, inSuffix )
	return os.path.basename( tp )

def appendList( l1, l2 ) :
	l2 = [l+'\n' for l in ''.join(l2).splitlines()]
	for l in l2 :
		l1.append( l )
	return l1


def Generate ( sidl=None, solutionname=None, verbose=None, precompiled=None ) :
	
	def find_solution ( in_name ) :
		return sidl.solutions.get( in_name, None )
		
	def find_project ( in_name ) :
		return sidl.projects.get( in_name, None )
		
	def find_group ( in_name ) :
		return sidl.groups.get( in_name, None )
	
	def create_rec ( root, uri ) :
		if not uri:		return root
		u = uri[0]
		if not u:		return root
		if u not in root :
			root[u] = {}
		return create_rec( root[u], uri[1:] )

	def get_filters( inProjObj ) :
		filters = {}
		for gname in inProjObj.groups :
			g = find_group( gname )
			if g :
				folders = g.CollectFolders(  )
				for f in folders :
					fl = f[0].split('.')
					fo = create_rec( filters, fl )
					if 'files' not in fo:
						fo['files'] = []
					fo['files'].append( (f[1],f[2]) )
					fo['UUID'] = f[0]
		return filters
	
	
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
		for a in atmake.ToList(attrnames) :
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
			
	
	def get_file_rec( outFileA, inKey, inValue, inFileExtFilter ) :
		if atmake.IsDict( inValue ) :
			for k,v in inValue.items() :
				sublines = get_file_rec( outFileA, k, v, inFileExtFilter )
		elif atmake.IsList( inValue ) and inKey=='files' :
			for files,options in inValue :
				for f in files :
					excluded = ('excludedfrombuild' in atmake.ToList(options))
					fileExt = atmake.GetFileExt( f )
					if not excluded and fileExt in inFileExtFilter:
						outFileA.append( f )
						
	
	def get_file_lst(	inProjObj, inFileExtFilter	) :
		fileA = []
		filters = get_filters( inProjObj )
		for k,v in filters.items() :
			get_file_rec( fileA, k, v, inFileExtFilter )
		return fileA;
	
	
	
	def gen_header_makefile( outLines, inSolObj ) :
	
		ndkpath = ( sidl.env.Subst( '$subuild' ) + '\\android_ndk' ).replace( '/', '\\' )
		sysroot = ndkpath + '\\sysroot'
		
		gp = ( ndkpath + '\\bin\\' )
		gccpath = ( gp + 'arm-linux-androideabi-gcc' )
		gpppath = ( gp + 'arm-linux-androideabi-g++' )
		arpath  = ( gp + 'arm-linux-androideabi-ar' )
		
		
		# build globals variables
		
		base_conf = android_base[ 'config' ]
		
		cflags, lflags = build_cflags_lflags( base_conf )
		cflags = get_subst_conf( cflags, sysroot )
		lflags = get_subst_conf( lflags, sysroot )
		cflags_cpp = base_conf[ 'CFLAGS_cpp' ]
		
		outLines.Append( '# Makefile for android platform' )
		outLines.Newline( 1 )
		
		outLines.Append( 'GCC    = %s\n' % ( gccpath ) )
		outLines.Append( 'GPP    = %s\n' % ( gpppath ) )
		outLines.Append( 'AR     = %s\n' % ( arpath ) )
		outLines.Newline( 1 )
		
		outLines.Append( 'CFLAGS = %s\n' % ( cflags ) )
		outLines.Append( 'LFLAGS = %s\n' % ( lflags ) )
		outLines.Append( 'CFLAGS_cpp = %s\n' % ( cflags_cpp ) )
		outLines.Newline( 2 )
	
	
	def gen_configuration_android( outLines, inProjObj, inConfName, inConfList ) :
		outLines.Append( '!IF DEFINED( %s )\n' % inConfName.upper() )
		
		vname_objdir = get_vname_objdir( inProjObj )
		vname_objdirmk = get_vname_objdirmk( inProjObj )
		outLines.Append( "%s = %s\n" %( vname_objdir, get_vval_objdir( inProjObj, inConfName ) ) )
		outLines.Append( "%s = %s\n" %( vname_objdirmk, get_vval_objdirmk( inProjObj, inConfName ) ) )
		
		
		
		vval_cflags, vval_lflags = build_cflags_lflags( inConfList )
		vname_cflags = get_vname_cflags( inProjObj );
		vname_lflags = get_vname_lflags( inProjObj );
		
		
		cpppath			= collect_project_attributes( inProjObj, 'cpppath',		inConfName )	# ok
		cppdefines		= collect_project_attributes( inProjObj, 'cppdefines',	inConfName )	# ok
		cppoptions		= collect_project_attributes( inProjObj, 'cppoptions',	inConfName )	# not impl
		linkpath		= collect_project_attributes( inProjObj, 'linkpath',	inConfName )	# ok
		linklibs		= collect_project_attributes( inProjObj, 'linklibs',	inConfName )	# ok
		linkoptions		= collect_project_attributes( inProjObj, 'linkoptions',	inConfName )	# not impl
		liboptions	    = collect_project_attributes( inProjObj, 'liboptions',	inConfName )	# not impl
		rcculture		= collect_project_attributes( inProjObj, 'culture',		inConfName )	# not impl
		
		
		if cppoptions and len( cppoptions[0] ) :
			print('warning genproj android : cppoption is ignored : %s' % cppoptions)	# not impl
		if linkoptions and len( linkoptions[0] ):
			print('warning genproj android : linkoptions is ignored : %s' % linkoptions)	# not impl
		if liboptions and len( liboptions[0] ) :
			print('warning genproj android : liboptions is ignored : %s' % liboptions)	# not impl
		if rcculture and len( rcculture[0] ) :
			print('warning genproj android : rcculture is ignored : %s' % rcculture)		# not impl
		
		
		
		for p in cpppath :
			if len( p ) :
				vval_cflags += "-I%s " % p
		for d in cppdefines :
			if len( d ) :
				vval_cflags += "-D%s " % d
			vval_cflags = vval_cflags.replace( '"', '\\"' )
		for p in linkpath :
			if len( p ) :
				vval_lflags += "-Wl,-rpath-link=%s " % p
		for l in linklibs :
			if len( l ) :
				vval_lflags += "-l%s " % l
		
		
		outLines.Append( "%s = %s\n" % ( vname_cflags, vval_cflags ) )
		outLines.Append( "%s = %s\n" % ( vname_lflags, vval_lflags ) )
		
		
		tdir	= get_targetdir( inProjObj ) + '/'
		tbname	= get_targetbasename( inProjObj, inConfList['TargetExt'] )
		
		
		projBuild = inProjObj.GetAttribute( 'build' ) or 'program'
		text  = {'program':'so', 'archive':'a', 'dll':'dll'}[projBuild]
		if text == 'so' :
			tdir += ( 'and%s/libs/armeabi/' % tbname )
			tbname = 'sugarProg'
		
		tfname	= 'lib%s.%s' % ( tbname, text )
		
		vname_target = get_vname_target( inProjObj )
	#	vname_targetfile = get_vname_targetfile( inProjObj )
		outLines.Append( "%s = %s\n" % ( vname_target, tdir + tfname ) )
	#	outLines.Append( "%s = %s\n" % ( vname_targetfile, tfname ) )
		
		outLines.Append( '!ENDIF\n' )
	
	
	def gen_configurations_android ( outLines, inProjObj, inSolPath ) :
		outLines.Append( "#\n# Generate for %s\n#\n\n" %( get_projname( inProjObj ) ) )
		
		outLines.Append( '# Generate configurations\n' )
		
		for confName, confLst in target_config.items() :
			gen_configuration_android( outLines, inProjObj, confName, confLst )
		outLines.Newline( 2 )
	
	
	
	def gen_objlist_and_genericrules( outLines, inProjObj, inSolPath ) :
		vname_objdir = get_vname_objdir( inProjObj )
		vname_objlst = get_vname_objlst( inProjObj )
		outLines.Append( "%s = " % ( vname_objlst ) )
		
		fileA = get_file_lst( inProjObj, g_compile_file_ext )
		
		
		dirA_c = []
		dirA_cpp = []
		for f in fileA :
			d, f = os.path.split( f )
			f, e = os.path.splitext( f )
			
			d = fs.FileToFile( d, inSolPath ).replace( '/', '\\' ) + '\\'
			
			if e == '.c'	and not d in dirA_c :
				dirA_c.append( d )
			if e == '.cpp'	and not d in dirA_cpp :
				dirA_cpp.append( d )
			
			outLines.Append( "\\\n\t$(%s)/%s.o " % ( vname_objdir, f ) )
		
		outLines.Newline( 2 )
		
		
		vname_cflags = get_vname_cflags( inProjObj );
		vname_lflags = get_vname_lflags( inProjObj );
		
		outLines.Append( "# Generate generic rules for c\n" )
		for d in dirA_c :
			outLines.Append( '{%s}.c{$(%s)}.o:\n' %( d, vname_objdir ) )
			outLines.Append( '\t@-echo $<\n' )
			outLines.Append( '\t@-$(GCC) -o $@ -c $< $(CFLAGS) $(%s)\n' %( vname_cflags ) )
		
		outLines.Append( "# Generate generic rules for cpp\n" )
		for d in dirA_cpp :
			outLines.Append( '{%s}.cpp{$(%s)}.o:\n' %( d, vname_objdir ) )
			outLines.Append( '\t@-echo $<\n' )
			outLines.Append( '\t@-$(GPP) -o $@ -c $< $(CFLAGS) $(CFLAGS_cpp) $(%s)\n' %( vname_cflags ) )
		
		
		outLines.Newline( 2 )
	
	
	
	def gen_rules( outLines, inProjObj, inProjName, inSolObj ) :
		
		outLines.Append( '# Generate rules\n' )
		
		vname_target = get_vname_target( inProjObj )
		vname_objdir = get_vname_objdir( inProjObj )
		vname_objlst = get_vname_objlst( inProjObj )
		vname_lflags = get_vname_lflags( inProjObj )
		vname_objdirmk = get_vname_objdirmk( inProjObj )
		
		# gen create_dir
		
		vname_mkdirrule = 'mktmpdir_%s' % get_projname( inProjObj )
		
		outLines.Append( '%s :\n' % vname_mkdirrule )
		outLines.Append( '\t@-echo Check directory $(%s)...\n' % vname_objdirmk )
		outLines.Append( '\t@-if not exist $(%s)\\*.* mkdir $(%s)\n\n' % ( vname_objdirmk, vname_objdirmk ) )
		
		
		projBuild = inProjObj.GetAttribute('build')  or 'program'
		
		if projBuild == 'program' :
			# Create SharedLib
			# get dep
			
			lstDep = ''
			addRules = ''
			
			pdeps = inSolObj.dependencies.get( inProjName, [] )
			for pURIdep in pdeps :
				linkPObj = find_project( pURIdep )
				lstDep += '$(%s) $(%s) ' %( get_vname_target( linkPObj ), get_vname_lflags( linkPObj ) )
				addRules += '%s ' %( get_projname( linkPObj ) )
			
			outLines.Append( '%s : %s $(%s) %s\n' % ( get_projname( inProjObj ), vname_mkdirrule, vname_objlst, addRules ) )
			outLines.Append( '\t@-echo link $(%s)...\n' % vname_target )
			outLines.Append( '\t@-$(GPP) -o $(%s) $(%s) $(LFLAGS) $(%s) %s\n\n' % ( vname_target, vname_objlst, vname_lflags, lstDep ) )
			
		elif projBuild == 'archive' :
			# Create StaticLib
			outLines.Append( '%s : %s $(%s)\n' % ( get_projname( inProjObj ), vname_mkdirrule, vname_objlst ) )
			outLines.Append( '\t@-echo archive $(%s)...\n' % vname_target )
			outLines.Append( '\t@-$(AR) crs $(%s) $(%s)\\*.o\n\n' % ( vname_target, vname_objdir ) )
			
		else :
			print('error not supported project type %s for %s' % ( projBuild, get_projname( inProjObj ) ))
		
	def gen_common_rules ( outLines, inSolObj ) :
		lstprg = ''
		cleanact = ''
		
		for pname in inSolObj.projects :
			projObj = find_project( pname )
			
			cleanact += '\n\terase .\\$(%s)\\*.o'	% ( get_vname_objdirmk( projObj ) )
			cleanact += '\n\trmdir .\\$(%s)'		% ( get_vname_objdirmk( projObj ) )
			cleanact += '\n\trmdir .\\%s'			% ( get_vval_objdirbase( projObj ) )
			
			projBuild = projObj.GetAttribute( 'build' )  or 'program'
			if projBuild == 'program' :
				lstprg += '%s ' % ( get_projname( projObj ) )
		
		if len( lstprg ) == 0 :
			for pname in inSolObj.projects :
				projObj = find_project( pname )
				lstprg += '%s ' % ( get_projname( projObj ) )
		
		outLines.Append( '\n\nall : %s\n\n' %( lstprg ) )
		# copy in javaproj/libs/armeabi
		
		outLines.Append( '\nclean :%s\n\n' %( cleanact ) )
		
		
		
	def gen_solution_android ( output, inSolObj ) :
		solPath	= inSolObj.GetOutpath()
		solDir	= os.path.dirname( solPath ) + '/'
		solName	= os.path.basename( solPath )
		
		makeFileName = 'makefile_' + sidl.GetPlatform()
		makeFilePath = solDir + makeFileName
		# using makefile : nmake /F makeFilePath
		
		
		outFile = SIDL_Outputfile(	path = makeFilePath,
									source = inSolObj,
									label = "Makefile Android solution '%s'" % inSolObj.uri,
									readonly = False	)

		output.append( outFile )
		
		
		makeLine = SIDL_Outputlines()
		gen_header_makefile( makeLine, inSolObj )
		
		for pname in inSolObj.projects :
			projObj = find_project( pname )
			gen_configurations_android( makeLine, projObj, solPath )
			gen_objlist_and_genericrules( makeLine, projObj, solPath )
		
		for pname in inSolObj.projects :
			projObj = find_project( pname )
			gen_rules( makeLine, projObj, pname, inSolObj )
		
		
		gen_common_rules( makeLine, inSolObj )
		
		outFile.Append( makeLine )
		
		
		
		
		# register directories to clean up
		sidl.ShelfOutfile( makeFilePath )
	
	
	def gen_solution_msvisual ( output, solobj ) :

		outsol = SIDL_Outputfile(	path	= solobj.GetOutpath(defext='msvc.makefile'),
									source	= solobj,
									label	= "Visual Makefile solution '%s'" % solobj.uri,
									readonly= False	)
		output.append( outsol )

		outsol.Append( '# AtMade\n' )

		# register directories to clean up
		sidl.ShelfOutfile( '%s/tmp' % os.path.dirname(outsol.path) )
	
	
	
	if sidl is None:
		return
	
	platform = sidl.GetPlatform()
	
	outputs = []
	
	if solutionname:
		# find the solution to generate
		s = find_solution(solutionname)
		if not s:	print("sidl: solution '%s' is not defined !" % (solutionname))
		solutions = [ s ]
	else :
		solutions = sidl.solutions.values()
	
	# generate all solutions
	for s in solutions :
		if platform=='WIN32':			gen_solution_msvisual( outputs, s )
		elif platform == 'ANDROID' :	gen_solution_android( outputs, s )

	# writeback all outputs
	for o in [x for x in outputs if x] :
		o.Writeback( sidl, verbose )
	