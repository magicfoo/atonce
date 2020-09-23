
import os, sys, uuid
import atmake
import atmake.tools.fs as fs
from .components import *



iph_fwVersion 	= 4.3
iph_fwMin		= 4.0



custum_buildrules = {
	'*.lua':		['atmake tool nohistory bin2h ifile=$INPUT_FILE_DIR/$INPUT_FILE_BASE.lua ofile=$INPUT_FILE_DIR/$INPUT_FILE_BASE.lua.h arrayname=1', '$INPUT_FILE_DIR/$INPUT_FILE_BASE.lua.h', "bin2h'ing file $(InputFileName) ..." ],
}





def get_pbxproject_config ( inConfName ) :
	tplt = {
		'ARCHS': 											'\"$(ARCHS_STANDARD_32_BIT)\"',
		'GCC_C_LANGUAGE_STANDARD':							'c99',
		'GCC_VERSION':										 '\"\"',
		'IPHONEOS_DEPLOYMENT_TARGET':						'%s' % iph_fwMin,
		
		'PREBINDING':										'NO',
		
	#	'\"CODE_SIGN_IDENTITY[sdk=iphoneos*]\"':			'\"iPhone Developer\"',
	#	'SDKROOT':											'iphonesimulator%s' % iph_fwVersion,
	}
	
	if inConfName == 'Simulator' :
		tplt['\"CODE_SIGN_IDENTITY[sdk=iphoneos*]\"']	= '\"iPhone Developer\"'
		tplt['SDKROOT']									= 'iphonesimulator%s' % iph_fwVersion
		tplt['ONLY_ACTIVE_ARCH']						= 'YES'
		
	elif inConfName == 'Device' :
		tplt['\"CODE_SIGN_IDENTITY[sdk=iphoneos*]\"']	= '\"iPhone Developer\"'
		tplt['SDKROOT']									= 'iphoneos%s' % iph_fwVersion
		tplt['ONLY_ACTIVE_ARCH']						= 'YES'
		
	elif inConfName == 'Distribution' :
		tplt['\"CODE_SIGN_IDENTITY[sdk=iphoneos*]\"']	= '\"iPhone Distribution\"'
		tplt['SDKROOT']									= 'iphoneos%s' % iph_fwVersion
		tplt['ONLY_ACTIVE_ARCH']						= 'NO'
		
	else :
		print( 'get_pbxproject_config warning : can\'t find PBXProject config for %s' % inConfName )
	
	return tplt


native_target_config = {
	'TARGETED_DEVICE_FAMILY':							'\"1,2\"',								# Iphone IPad	'GCC_INPUT_FILETYPE':								'sourcecode.cpp.objcpp',				# Obj c++	'GCC_PFE_FILE_C_DIALECTS':							'\"c++ objective-c++ c objective-c\"',		'GCC_WARN_ABOUT_RETURN_TYPE': 						'YES',	'GCC_WARN_UNUSED_VARIABLE': 						'YES',	'GCC_WARN_ABOUT_INVALID_OFFSETOF_MACRO':			'NO',	'GCC_ENABLE_CPP_EXCEPTIONS': 						'NO',	'GCC_ENABLE_CPP_RTTI': 								'NO',	'GCC_ENABLE_OBJC_EXCEPTIONS': 						'NO',	'GCC_OBJC_CALL_CXX_CDTORS':							'NO',	'GCC_PREPROCESSOR_DEFINITIONS_NOT_USED_IN_PRECOMPS':'\"\"',	'GCC_PRECOMPILE_PREFIX_HEADER':						'NO',		'OTHER_CFLAGS':										'\"-fvisibility=hidden\"',	'DEAD_CODE_STRIPPING':								'YES',	'STRIP_STYLE':										'all',	'LD_OPENMP_FLAGS':									'\"\"',	'STRINGS_FILE_OUTPUT_ENCODING':						'\"UTF-8\"',	'SCAN_ALL_SOURCE_FILES_FOR_INCLUDES':				'YES',	'PRECOMPS_INCLUDE_HEADERS_FROM_BUILT_PRODUCTS_DIR':	'NO',	'ALWAYS_SEARCH_USER_PATHS': 						'NO'
}


xcode_iphone_plist_ext = {
	'Simulator' : '_SIM',
	'Device' : '_DEV',
	'Distribution' : '_DST'
}

xcode_iphone_config = {
	'Simulator' : {
		
		'Debug' : {
			'SDKROOT':											'iphonesimulator%s' % iph_fwVersion,
						'GCC_PREPROCESSOR_DEFINITIONS':						[ 'SU_DEBUG', '_DEBUG', 'SU_IOS_SIM', 'SU_IOS_C_DIALECTS_OBJCPP' ],			'FRAMEWORK_SEARCH_PATHS':							[ '$(inherited)', '\\"$(SDKROOT)$(SYSTEM_LIBRARY_DIR)/PrivateFrameworks\\"'],			'LIBRARY_SEARCH_PATHS':								[],			'TargetExtention':									'_SIM_DBG',			
			'GCC_GENERATE_DEBUGGING_SYMBOLS':					'YES',			'GCC_OPTIMIZATION_LEVEL':							'0',			'COPY_PHASE_STRIP': 								'NO',
						'GCC_DYNAMIC_NO_PIC':								'NO'
		},
		
		'Release' : {
			'SDKROOT':											'iphonesimulator%s' % iph_fwVersion,
						'GCC_PREPROCESSOR_DEFINITIONS':						[ 'SU_RELEASE', '_NDEBUG', 'SU_NDEBUG', 'SU_IOS_SIM', 'SU_IOS_C_DIALECTS_OBJCPP' ],			'FRAMEWORK_SEARCH_PATHS':							[ '$(inherited)', '\\"$(SDKROOT)$(SYSTEM_LIBRARY_DIR)/PrivateFrameworks\\"'],			'LIBRARY_SEARCH_PATHS':								[],			'TargetExtention':									'_SIM_RLS',			
			'GCC_GENERATE_DEBUGGING_SYMBOLS':					'NO',			'GCC_OPTIMIZATION_LEVEL':							'3',			'COPY_PHASE_STRIP': 								'YES',
						'GCC_ENABLE_SSE3_EXTENSIONS':						'YES',			'GCC_ENABLE_SUPPLEMENTAL_SSE3_INSTRUCTIONS':		'YES'
		}
	},
	'Device' : {
		
		'Debug' : {
			'SDKROOT':											'iphoneos%s' % iph_fwVersion,
			
			'GCC_PREPROCESSOR_DEFINITIONS':						[ 'SU_DEBUG', '_DEBUG', 'SU_IOS_DEV', 'SU_IOS_C_DIALECTS_OBJCPP' ],
			'FRAMEWORK_SEARCH_PATHS':							[ '$(inherited)', '\\"$(SDKROOT)$(SYSTEM_LIBRARY_DIR)/PrivateFrameworks\\"'],			'LIBRARY_SEARCH_PATHS':								[],			'TargetExtention':									'_DBG',
			
			'GCC_GENERATE_DEBUGGING_SYMBOLS':					'YES',			'GCC_OPTIMIZATION_LEVEL':							'0',			'COPY_PHASE_STRIP': 								'NO',
						'GCC_DYNAMIC_NO_PIC':								'NO'
		},
		
		'Release' : {
			'SDKROOT':											'iphoneos%s' % iph_fwVersion,
			
			'GCC_PREPROCESSOR_DEFINITIONS':						[ 'SU_RELEASE', '_NDEBUG', 'SU_NDEBUG', 'SU_IOS_DEV', 'SU_IOS_C_DIALECTS_OBJCPP' ],			'FRAMEWORK_SEARCH_PATHS':							[ '$(inherited)', '\\"$(SDKROOT)$(SYSTEM_LIBRARY_DIR)/PrivateFrameworks\\"'],			'LIBRARY_SEARCH_PATHS':								[],
			'TargetExtention':									'_RLS',			
			'GCC_GENERATE_DEBUGGING_SYMBOLS':					'NO',			'GCC_OPTIMIZATION_LEVEL':							'3',			'COPY_PHASE_STRIP': 								'YES',
						'GCC_ENABLE_SSE3_EXTENSIONS':						'YES',			'GCC_ENABLE_SUPPLEMENTAL_SSE3_INSTRUCTIONS':		'YES'
		}
		
	},
	
	'Distribution' : {
		'SDKROOT':											'iphoneos%s' % iph_fwVersion,
		
		'GCC_PREPROCESSOR_DEFINITIONS':						[ 'SU_IOS_DISTRIB', 'SU_MASTER', '_NDEBUG', 'SU_NDEBUG', 'SU_IOS_DEV', 'SU_IOS_C_DIALECTS_OBJCPP' ],		'FRAMEWORK_SEARCH_PATHS':							[ '$(inherited)', '\\"$(SDKROOT)$(SYSTEM_LIBRARY_DIR)/PrivateFrameworks\\"'],		'LIBRARY_SEARCH_PATHS':								[],
		'TargetExtention':									'_DST',		
		'GCC_GENERATE_DEBUGGING_SYMBOLS':					'NO',		'GCC_OPTIMIZATION_LEVEL':							'3',		'COPY_PHASE_STRIP': 								'YES',
				'GCC_ENABLE_SSE3_EXTENSIONS':						'YES',		'GCC_ENABLE_SUPPLEMENTAL_SSE3_INSTRUCTIONS':		'YES'
		
	},
}



xcode_darwin_config = {
	
	'Debug' : {
		'PBXProject' : {
			'ARCHS': 											'\"$(ARCHS_STANDARD_32_BIT)\"',
			'GCC_C_LANGUAGE_STANDARD':							'c99',
			'GCC_VERSION':										 '\"\"',
			'ONLY_ACTIVE_ARCH':									'YES',
			'PREBINDING':										'NO',
		},
		'PBXNativeTarget' : {
			'GCC_INPUT_FILETYPE':								'sourcecode.cpp.objcpp',				# Obj c++
			'GCC_PFE_FILE_C_DIALECTS':							'\"c++ objective-c++ c objective-c\"',

			'GCC_WARN_ABOUT_RETURN_TYPE': 						'YES',
			'GCC_WARN_UNUSED_VARIABLE': 						'YES',
			'GCC_WARN_ABOUT_INVALID_OFFSETOF_MACRO':			'NO',
			'GCC_ENABLE_CPP_EXCEPTIONS': 						'NO',
			'GCC_ENABLE_CPP_RTTI': 								'NO',
			'GCC_ENABLE_OBJC_EXCEPTIONS': 						'NO',
			
			'GCC_OBJC_CALL_CXX_CDTORS':							'NO',
			'GCC_PREPROCESSOR_DEFINITIONS_NOT_USED_IN_PRECOMPS':'\"\"',
			'GCC_PRECOMPILE_PREFIX_HEADER':						'NO',
			
			'OTHER_CFLAGS':										'\"-fvisibility=hidden\"',
			'DEAD_CODE_STRIPPING':								'YES',
			'STRIP_STYLE':										'all',
			'LD_OPENMP_FLAGS':									'\"\"',
			'STRINGS_FILE_OUTPUT_ENCODING':						'\"UTF-8\"',
			'SCAN_ALL_SOURCE_FILES_FOR_INCLUDES':				'YES',
			'PRECOMPS_INCLUDE_HEADERS_FROM_BUILT_PRODUCTS_DIR':	'NO',
			'ALWAYS_SEARCH_USER_PATHS': 						'NO',
			
			'STANDARD_C_PLUS_PLUS_LIBRARY_TYPE':				'dynamic',
			
			
			# HW
			'SDKROOT': 											'macosx10.6',
			
			# Different from Release
			'GCC_GENERATE_DEBUGGING_SYMBOLS':					'YES',
			'COPY_PHASE_STRIP': 								'YES',
			'GCC_DYNAMIC_NO_PIC':								'NO',
			'GCC_OPTIMIZATION_LEVEL':							'0',
			'GCC_PREPROCESSOR_DEFINITIONS':						[ 'SU_DEBUG', '_DEBUG', 'SU_OSX_C_DIALECTS_OBJCPP' ],
			'TargetExtention':									'_DARWIN_DBG',
			'LIBRARY_SEARCH_PATHS':								[],
			
			'OTHER_LDFLAGS':									'-liconv',						# just for archive
		},
	},
	
	'Release' : {
		'PBXProject' : {
			'ARCHS': 											'\"$(ARCHS_STANDARD_32_BIT)\"',
			'GCC_C_LANGUAGE_STANDARD':							'c99',
			'GCC_VERSION':										 '\"\"',
			'ONLY_ACTIVE_ARCH':									'YES',
			'PREBINDING':										'NO',
		},
		'PBXNativeTarget' : {
			'GCC_INPUT_FILETYPE':								'sourcecode.cpp.objcpp',				# Obj c++
			'GCC_PFE_FILE_C_DIALECTS':							'\"c++ objective-c++ c objective-c\"',

			'GCC_WARN_ABOUT_RETURN_TYPE': 						'YES',
			'GCC_WARN_UNUSED_VARIABLE': 						'YES',
			'GCC_WARN_ABOUT_INVALID_OFFSETOF_MACRO':			'NO',
			'GCC_ENABLE_CPP_EXCEPTIONS': 						'NO',
			'GCC_ENABLE_CPP_RTTI': 								'NO',
			'GCC_ENABLE_OBJC_EXCEPTIONS': 						'NO',
			
			'GCC_OBJC_CALL_CXX_CDTORS':							'NO',
			'GCC_PREPROCESSOR_DEFINITIONS_NOT_USED_IN_PRECOMPS':'\"\"',
			'GCC_PRECOMPILE_PREFIX_HEADER':						'NO',
			
			'OTHER_CFLAGS':										'\"-fvisibility=hidden\"',
			'DEAD_CODE_STRIPPING':								'YES',
			'STRIP_STYLE':										'all',
			'LD_OPENMP_FLAGS':									'\"\"',
			'STRINGS_FILE_OUTPUT_ENCODING':						'\"UTF-8\"',
			'SCAN_ALL_SOURCE_FILES_FOR_INCLUDES':				'YES',
			'PRECOMPS_INCLUDE_HEADERS_FROM_BUILT_PRODUCTS_DIR':	'NO',
			'ALWAYS_SEARCH_USER_PATHS': 						'NO',
			
			'STANDARD_C_PLUS_PLUS_LIBRARY_TYPE':				'dynamic',
			
			
			# HW
			'SDKROOT': 											'macosx10.6',
			
			# Different from Debug
			'GCC_GENERATE_DEBUGGING_SYMBOLS':					'NO',
			'COPY_PHASE_STRIP': 								'NO',
			'GCC_OPTIMIZATION_LEVEL':							'3',
			'GCC_PREPROCESSOR_DEFINITIONS':						[ 'SU_RELEASE', '_NDEBUG', 'SU_NDEBUG', 'SU_OSX_C_DIALECTS_OBJCPP' ],
			'TargetExtention':									'_DARWIN_RLS',
			'GCC_ENABLE_SSE3_EXTENSIONS':						'YES',
			'GCC_ENABLE_SUPPLEMENTAL_SSE3_INSTRUCTIONS':		'YES',
			
			'LIBRARY_SEARCH_PATHS':								[],
		},
	},
}



# 'SDKROOT': 'macosx10.6',

xcode_product_define = {
	'archive' : {
		'outfileext' :		'.a',
		'outfiletype' :		'archive.ar',
		'type' :			'com.apple.product-type.library.static',
	},
	'dll' : {
		'outfileext' :		'.dylib',
		'outfiletype' :		'compiled.mach-o.dylib',
		'type' :			'com.apple.product-type.library.dynamic',
	},
	'program' : {
		'outfileext' :		'.app',
		'outfiletype' :		'wrapper.application',
		'type' :			'com.apple.product-type.application',
	}
}


xcode_default_iphone_frameworks = {
#	Framework Name :				Path relative to SDKROOT
	
	'CoreGraphics.framework' : 			'System/Library/Frameworks/',
	'UIKit.framework' :					'System/Library/Frameworks/',
	'OpenGLES.framework' :				'System/Library/Frameworks/',
	'AudioToolbox.framework' :			'System/Library/Frameworks/',
	'Foundation.framework' :			'System/Library/Frameworks/',
	'QuartzCore.framework' :			'System/Library/Frameworks/',
	'AVFoundation.framework' :			'System/Library/Frameworks/',
	
	'CoreVideo.framework' :				'System/Library/Frameworks/',
	'CoreMedia.framework' :				'System/Library/Frameworks/',
	'SystemConfiguration.framework' :	'System/Library/Frameworks/',
	'Security.framework' :				'System/Library/Frameworks/',
}

xcode_default_osx_frameworks = {
	'AppKit.framework' :			'System/Library/Frameworks/',
	'OpenGL.framework' :			'System/Library/Frameworks/',
	'Foundation.framework' :		'System/Library/Frameworks/',
	'QuartzCore.framework' :		'System/Library/Frameworks/',
	'AudioToolbox.framework' :		'System/Library/Frameworks/',
}

xcode_default_frameworks = xcode_default_osx_frameworks


def generate_ios_plist_lines ( inkeys ) :

	prop_keys = {
		'CFBundleDevelopmentRegion' : 		'English',
		'CFBundleDisplayName' :				'${PRODUCT_NAME}',
		'CFBundleExecutable' :				'${EXECUTABLE_NAME}',
	#	'CFBundleIconFile' :				'Icon.png',
		'CFBundleIdentifier' :				'com.yourcompany.${PRODUCT_NAME:identifier}',
		'CFBundleInfoDictionaryVersion' :	'6.0',
		'CFBundleName' :					'${PRODUCT_NAME}',
		'CFBundlePackageType' :				'APPL',
		'CFBundleSignature' :				'????',
		'CFBundleVersion' :					'1.0',
		'LSRequiresIPhoneOS' :				True,
	}

	for k,v in inkeys.items() :
		prop_keys[k] = v

	lines = []
	lines.append( '<?xml version="1.0" encoding="UTF-8"?>\n' )
	lines.append( '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">\n' )
	lines.append( '<plist version="1.0">\n' )
	lines.append( '<dict>\n' )

	for k,v in prop_keys.items() :
		lines.append( '\t<key>%s</key>\n' % k )
		if v is True :
			lines.append( '\t<true/>\n' )
		elif v is False :
			lines.append( '\t<false/>\n' )
		elif atmake.IsInteger(v) :
			lines.append( '\t<integer>%s</integer>\n' % str(v) )
		elif atmake.IsString(v) :
			if '|' in v :
				array = [a.strip() for a in v.split('|')]
				lines.append( '\t<array>\n' )
				for a in array :
					la = a.lower()
					if la=='true' :
						lines.append( '\t\t<true/>\n' )
					elif la=='false' :
						lines.append( '\t\t<false/>\n' )
					elif len(a)>0 :
						lines.append( '\t\t<string>%s</string>\n' % a )	
				lines.append( '\t</array>\n' )
			else :
				lines.append( '\t<string>%s</string>\n' % v )

	lines.append( '</dict>\n' )
	lines.append( '</plist>\n' )
	return lines


def generate_project_ios_plist_lines ( inproj, inconfig ) :

	def_conf = ['plist_all_','plist_']
	my_conf  = ['plist_%s_'%inconfig.lower()]
	all_conf = ['plist_%s_'%c for c in 'simulator device distribution'.split()]

	def _startswith( s, l ) :
		for a in l:
			if s.startswith(a) : return s[len(a):]
		return None

	defined_keys = {}

	# setup default values of the form plist_ID or plist_all_ID
	for k,v in inproj.attributes.items() :
		if not _startswith(k,all_conf) :
			a = _startswith(k,def_conf)
			if a:
				defined_keys[a] = v

	# setup plist_CONFIG_ID only
	for k,v in inproj.attributes.items() :
		a = _startswith(k,my_conf)
		if a:
			defined_keys[a] = v

	#print( inconfig, defined_keys )

	return generate_ios_plist_lines( defined_keys )









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



ext_file_ref	 		= [ 'cpp', 'c', 'm', 'mm', 'framework', 'xib', 'h', 'hpp', 'pch', 'plist', 'sidl', 'lua' ];
ext_build_file			= [ 'cpp', 'c', 'm', 'mm', 'framework', 'xib', 'lua' ];
ext_sources_build_phase = [ 'cpp', 'c', 'm', 'mm', 'lua' ];




def xcode_uuid ( label, section=None ) :
	uuidLabel='%s:%s' %(label, section or ' ');
	return str( uuid.uuid3(uuid.NAMESPACE_URL,uuidLabel) ).upper().replace('-','')[0:24];

def xcode_buildfile_uuid ( projName, pathFile ) :
	return xcode_uuid( projName + pathFile, 'buildFile' )

def xcode_fileref_uuid ( label ) :
	return xcode_uuid( label, 'fileRef' )

def xcode_group_uuid ( label ) :
	return xcode_uuid( label, 'group' )

def xcode_frameworkBuildPhase_uuid 	( projName ) :
	return xcode_uuid( projName, 'frameworkBuildPhase' )

def xcode_shellScriptPreBuildPhase_uuid( projName ) :
	return xcode_uuid( projName, 'shellScriptPreBuildPhase' )

def xcode_shellScriptPostBuildPhase_uuid( projName ) :
	return xcode_uuid( projName, 'shellScriptPostBuildPhase' )
	
def xcode_sourceBuildPhase_uuid 	( projName ) :
	return xcode_uuid( projName, 'sourceBuildPhase' )
	
def xcode_resourceBuildPhase_uuid 	( projName ) :
	return xcode_uuid( projName, 'resourceBuildPhase' )
	
def xcode_nativeTarget_uuid 		( projName ) :
	return xcode_uuid( projName, 'nativeTarget' )
	
def xcode_configList_uuid 			( projName ) :
	return xcode_uuid( projName, 'configList' )

def xcode_buildConfiguration_uuid 	( projName, confName ) :
	return xcode_uuid( projName + confName, 'buildConfiguration' )

def xcode_targetDep_uuid			( inProjName, inDepProjName ) :
	return xcode_uuid( inProjName + inDepProjName, 'PBXTargetDependencyUUID' )

def xcode_containerItemProxy_uuid	( inProjName, inDepProjName ) :
	return xcode_uuid( inProjName + inDepProjName, 'PBXContainerItemProxyUUID' )
	
def xcode_buildrules_uuid	( inProjName, inPattern ) :
	return xcode_uuid( inProjName + inPattern, 'PBXBuildRulesUUID' )


def appendList( l1, l2 ) :
	l2 = [l+'\n' for l in ''.join(l2).splitlines()]
	for l in l2 :
		l1.append( l )
	return l1



g_bfA = []

def RegBuildFile( inFilePath, inProjName, inIndent ) :
	bfuuid = xcode_buildfile_uuid( inProjName, inFilePath )
#	print ( inFilePath  + ' ' + bfuuid )
#	if bfuuid in g_bfA :
#		print ( 'FAILED' )
#		return ''

	g_bfA.append( bfuuid )
	
	fruuid = xcode_fileref_uuid	( inFilePath )
	
#	print ( inFilePath  + ' ' + bfuuid )
	return '\t'*inIndent + '%s /* %s */ = {isa = PBXBuildFile; fileRef = %s; };\n' % ( bfuuid, os.path.basename( inFilePath ), fruuid )



	
	
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
	
	
	def collect_project_attribute ( projobj, attrname ) :
		e_li = sidl.env.GetVarlist( attrname )
		p_li = projobj.GetAttributeList( attrname )
		g_li = []
		for gname in projobj.groups :
			g = find_group( gname )
			if g :
				g_li.extend( g.GetAttributeList(attrname) )
		return e_li + p_li + g_li
	
	
	
	
	def find_conf_groups ( projobj, confname ) :
	
		confTypeName = ''
		if confname.count( 'Debug' ) != 0 :
			confTypeName = 'debug'
		elif confname.count( 'Release' ) != 0 :
			confTypeName = 'release'
						
		G = []
		for gname in projobj.groups :
			g = find_group( gname )
			if g :
				if not g.HasAttribute('configuration') or g.AttributeEquals('configuration',confTypeName) :
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
		  
		
		

	
	def genBuildFile_Unit ( inSolpath, inProjName, inKey, inValue ) :
		lines = []
		if atmake.IsDict( inValue ) :
			for k,v in inValue.items() :
				sublines = genBuildFile_Unit( inSolpath, inProjName, k, v )
				appendList( lines, sublines )
		elif atmake.IsList( inValue ) and inKey=='files' :
			for files,options in inValue :
				for f in files :
					fileExt = atmake.GetFileExt( f )
					if fileExt in ext_build_file :
						relPath = fs.FileToFile( f, inSolpath ).replace( '\\', '/' )
						cli = RegBuildFile( relPath, inProjName, 2 )
						lines.append( cli )
		
		return lines
	
	
	
	def genBuildFile ( inSolObj ) :
		lines = []
		lines.append( '\n/* Begin PBXBuildFile section */\n' )
		
		solPath = inSolObj.GetOutpath( defext='xcodeproj' )
		
		for pname in inSolObj.projects :
			projObj = find_project( pname )
			if projObj :
				lines.append( '\t/* PBXBuildFile for %s */\n' % pname )
				
				projName  = projObj.uri.replace( '.', '_' )
				projBuild = projObj.GetAttribute('build')  or 'program'
				
				if projBuild != 'archive' :
					# Add dependency in PBXBuildFile
					pdeps = inSolObj.dependencies.get( pname, [] )
					for pURIdep in pdeps :
						lt_Name		= pURIdep.replace( '.', '_' ) + '.a'
						lines.append( RegBuildFile( lt_Name, projName, 2 ) )
						
						# Addlinklib from dep
						linkPObj = find_project( pURIdep )
						linkPName = linkPObj.uri.replace( '.','_' )
						linklibs = collect_project_attribute( linkPObj, 'linklibs' )
						for v in linklibs :
							lines.append( RegBuildFile( v, projName, 2 ) )
					
					# Add default frameworks
					for k, v in xcode_default_frameworks.items() :
						lines.append( RegBuildFile( k, projName, 2 ) )
				
				filters = get_filters( projObj )
				for k,v in filters.items() :
					li = genBuildFile_Unit( solPath, projName, k, v )
					appendList( lines, li )
				
		lines.append( '/* End PBXBuildFile section */\n' )
		return lines
	
	
	
	def genFileRef_Unit ( inProjPath, inKey, inValue ) :
		lines = []
		if atmake.IsDict(inValue) :
			for k,v in inValue.items() :
				sublines = genFileRef_Unit( inProjPath, k, v )
				appendList( lines, sublines )
		elif atmake.IsList(inValue) and inKey=='files' :
			for files,options in inValue :
				for f in files :
					fileExt = atmake.GetFileExt(f);
				#	if fileExt in ext_file_ref :
					relativePath = fs.FileToFile( f, inProjPath ).replace( '\\', '/' );
					baseName = os.path.basename( f );
					
					fileEncoding = '';
					if fileExt=='m' or fileExt=='cpp' or fileExt=='h' or fileExt=='pch' or fileExt=='plist' :
						fileEncoding = 'fileEncoding = 4;'
					
					refID 		= xcode_fileref_uuid( relativePath )
					isa 		= 'isa = PBXFileReference;'
					name 		= 'name = \"%s\";' 		% baseName
					path 		= 'path = \"%s\";' 			% relativePath
					sourceTree 	= 'sourceTree = SOURCE_ROOT;'
					
					lines.append( '\t'*2 + '%s /* %s */ = {  %s %s %s %s %s };\n' % ( refID, baseName, isa, fileEncoding, name, path, sourceTree ) );
		return lines;
	
	
	
	
	def genFileRef( inSolObj ) :
		lines = []
		lines.append( '\n/* Begin PBXFileReference section */\n' )

		
		solPath = inSolObj.GetOutpath( defext='xcodeproj' )
		
		for pname in inSolObj.projects :
			pobj = find_project( pname )
			if pobj :
				lines.append( '\t/* PBXFileReference for %s */\n' % pname )
				filters = get_filters( pobj )
				
				projBuild = pobj.GetAttribute('build')  or 'program';				
				
				# Add Products
				prod_Name	  = pobj.uri.replace( '.','_' )
				prod_Name	 += xcode_product_define[projBuild]['outfileext']
				prod_FileType = xcode_product_define[projBuild]['outfiletype']
				prod_UUID_FR  = xcode_fileref_uuid( prod_Name )
				
				lines.append( '\t'*2 + '%s /* %s */ = {isa = PBXFileReference; explicitFileType = %s; includeInIndex = 0; path = %s; sourceTree = BUILT_PRODUCTS_DIR;};\n'% ( prod_UUID_FR, prod_Name, prod_FileType, prod_Name ));
				
				
				if projBuild != 'archive' :
					# Add default frameworks
					for k, v in xcode_default_frameworks.items() :
						fw_Name	  	= k;
						fw_Path   	= v + k;
						fw_UUID_FR	= xcode_fileref_uuid( fw_Name );
						
						lines.append( '\t'*2 + '%s /* %s */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = %s; path = %s; sourceTree = SDKROOT; };\n' % ( fw_UUID_FR, fw_Name, fw_Name, fw_Path ) );
				
				
				linklibs = collect_project_attribute( pobj, 'linklibs' )
				for v in linklibs :
					if v :
						lib_Name	= v
						lib_Path	= v
						lib_UUID_FR	= xcode_fileref_uuid( lib_Name )
						lines.append( '\t'*2 + '%s /* %s */ = {isa = PBXFileReference; lastKnownFileType = archive.ar; path = %s; sourceTree = \"<absolute>\"; };\n' % ( lib_UUID_FR, lib_Name, lib_Path ) );
						
				
				
				for k,v in filters.items() :
					appendList( lines, genFileRef_Unit( solPath, k, v ) );
		
		lines.append( '/* End PBXFileReference section */\n' );
		return lines;
	
	
	

	def genGroupUnit( inGroupURI, inGroupName, inChildLst ) :
		lines = []
		lines.append( '\t'*1 + 	'%s /* %s */ = {\n'					% ( xcode_group_uuid( inGroupURI ), inGroupURI )	)
		lines.append( '\t'*2 + 		'isa = PBXGroup;\n'																	)
		lines.append( '\t'*2 + 		'children = (\n'																	)
		for child in inChildLst :
			lines.append( '\t'*3 + 		'%s\n'						% child												)
		lines.append( '\t'*2 + 		');\n'																				)
		lines.append( '\t'*2 + 		'name = \"%s\";\n'				% inGroupName 										)
		lines.append( '\t'*2 + 		'sourceTree = \"<group>\";\n'														)
		lines.append( '\t'*1 +	'};\n'																					)
		return lines

	
	
	def genUUIDFilters_U ( inKey, inValue, inLastURI ) :
		if atmake.IsDict( inValue ) :
			if inLastURI == '' :
				inLastURI = '%s' % inKey;
			else :
				inLastURI = '%s.%s' % ( inLastURI, inKey );

			inValue['UUID'] = inLastURI;
			
			for k1,v1 in inValue.items() :
				genUUIDFilters_U( k1, v1, inLastURI );
		
	def genUUIDFilters ( inProjObj, inFilters ) :
		projName = inProjObj.uri.replace( '.', '_' );
		for k,v in inFilters.items() :
			genUUIDFilters_U( k, v, projName );
		
		return inFilters;
	
	
	def genGroup_Rec ( solPath, inKey, inValue ) :
		lines = []
		
		if atmake.IsDict( inValue ) :
			groupURI 	= 0
			groupName 	= inKey
			childLst 	= []
			
			for k,v in inValue.items() :
				if k == 'UUID' :
					groupURI = v
				
				if atmake.IsDict(v) :										# Child is a filter
					gpURI = 0
					for k1,v1 in v.items() :
						if k1=='UUID' :
							gpURI = v1
					childLst.append( '%s /* %s */,' % ( xcode_group_uuid( gpURI ), gpURI ) )

				elif atmake.IsList(v) and k=='files' :						# Child is a fileList
					for files,options in v :
						for f in files :
							rPath	 = fs.FileToFile( f, solPath ).replace( '\\', '/' )
							refUUID	 = xcode_fileref_uuid( rPath )
							baseName = os.path.basename( f )
							childLst.append( '%s /* %s */,' % ( refUUID, baseName ) )

			group_lines = genGroupUnit( groupURI, groupName, childLst )
			appendList( lines, group_lines )
			
			for k,v in inValue.items() :
				appendList( lines, genGroup_Rec( solPath, k, v ) )
		
		return lines


	def genMainGroupProj( inSolPath, filters, inProjObj ) :				# Gen MainGroup for a project		
		childLst_MainGroup = []
		
		for k,v in filters.items() :
			if atmake.IsDict(v) :
				UUIChild = 0
				ValChild = 0
				for k1,v1 in v.items() :
					if k1=='UUID' :
						UUIChild = xcode_group_uuid( v1 )
						ValChild = v1
				childLst_MainGroup.append( '%s /* %s */,' % ( UUIChild, ValChild ) )

			elif atmake.IsList(v) and k=='files' :
				for f in v :
					rPath	 = fs.FileToFile( f, inSolPath ).replace( '\\', '/' )
					refUUID  = xcode_fileref_uuid( rPath )
					baseName = os.path.basename( f )
					childLst_MainGroup.append( '%s /* %s */,' % ( refUUID, baseName ) )
		
		projName = inProjObj.uri.replace( '.','_' )
		mainGroupName = '%s_MAINGROUP' % projName
		
		lines = []
		appendList( lines, genGroupUnit( mainGroupName, projName, childLst_MainGroup ) )
		
		return lines
	
	
	def genXCodeGroupAdditionnal( inSolObj ) :					# Gen MainGroup and Products
		childLst_Products  = []
		childLst_MainGroup = []
		childLst_Fw = []
		
		fRefA = []
		for pname in inSolObj.projects :
			projObj	  = find_project( pname )
			projName  = projObj.uri.replace( '.','_' )
			projBuild = projObj.GetAttribute('build')  or 'program'
			
			mainGroupName = '%s_MAINGROUP' % projName
			productName	  = projName + xcode_product_define[projBuild]['outfileext']
			
			childLst_MainGroup.append( '%s /* %s */,' 	% ( xcode_group_uuid( mainGroupName ), mainGroupName ) )
			childLst_Products.append ( '%s /* %s */,' 	% ( xcode_fileref_uuid( productName ), productName ) )
			
			linklibs = collect_project_attribute( projObj, 'linklibs' )
			for v in linklibs :
				if v :
					lib_Name	= v
					lib_UUID_FR	= xcode_fileref_uuid( lib_Name )
					if not lib_UUID_FR in fRefA :
						fRefA.append( lib_UUID_FR )
						childLst_Fw.append ( '%s /* %s */,' 	% ( lib_UUID_FR, lib_Name ) )
		
		# Add default frameworks
		for k, v in xcode_default_frameworks.items() :
			fw_Name	   = k
			fw_UUID_FR = xcode_fileref_uuid( fw_Name )
			childLst_Fw.append ( '%s /* %s */,' 	% ( fw_UUID_FR, fw_Name ) )
		
		childLst_MainGroup.append( '%s /* %s */,' 	% ( xcode_group_uuid( 'Products' ), 'Products' ) )
		childLst_MainGroup.append( '%s /* %s */,' 	% ( xcode_group_uuid( 'Frameworks' ), 'Frameworks' ) )
		
		lines = []
		appendList( lines, genGroupUnit( 'MAINGROUP', 'MAINGROUP', childLst_MainGroup ) )
		appendList( lines, genGroupUnit( 'Products',  'Products',  childLst_Products  ) )
		appendList( lines, genGroupUnit( 'Frameworks',  'Frameworks',  childLst_Fw  ) )
		return lines
		
	
	
	def genGroup( inSolObj ) :
		lines = []
		lines.append( '\n/* Begin PBXGroup section */\n' )
		
		solPath = inSolObj.GetOutpath( defext='xcodeproj' )
					
		appendList( lines, genXCodeGroupAdditionnal( inSolObj ) )
		
		
		for pname in inSolObj.projects :
			pobj = find_project( pname )
			
			filters = get_filters( pobj )
			filters = genUUIDFilters( pobj, filters )
			appendList( lines, genMainGroupProj( solPath, filters, pobj ) )
				
			for k,v in filters.items() :
				groupLines = genGroup_Rec( solPath, k, v )
				appendList( lines, groupLines )
		
		
		lines.append( '/* End PBXGroup section */\n' )
		return lines
	
	
	
	
	def genContainerItemProxy( inSolObj ) :
		lines = []
		
		solPath = inSolObj.GetOutpath() + '_' + sidl.GetPlatform() + '.xcodeproj'
		solUUID = xcode_uuid( solPath )
		
		for pname in inSolObj.projects :
			pobj = find_project( pname )
			projName = pobj.uri.replace( '.','_' )
			
			pdeps = inSolObj.dependencies.get( pname, [] )
			for pURIdep in pdeps :
				depProj_Name = pURIdep.replace( '.', '_' )
				depProj_NativeTargUUID	  = xcode_nativeTarget_uuid ( depProj_Name )
				PBXContainerItemProxyUUID = xcode_containerItemProxy_uuid	( projName, depProj_Name )
				
				lines.append( '\t'*2 + 	'%s /* PBXContainerItemProxy */ = {\n'				% PBXContainerItemProxyUUID					)
				lines.append( '\t'*3 + 		'isa = PBXContainerItemProxy;\n' 															)
				lines.append( '\t'*3 + 		'containerPortal = %s /* Project object */;\n'	% solUUID									)
				lines.append( '\t'*3 + 		'proxyType = 1;\n'																			)
				lines.append( '\t'*3 + 		'remoteGlobalIDString = %s /* %s */;\n'			% ( depProj_NativeTargUUID, depProj_Name )	)
				lines.append( '\t'*3 + 		'remoteInfo = \"%s\";\n'						% depProj_Name								)
				lines.append( '\t'*2 + 	'};\n'																							)
		
		return lines
	
	
	def genCustumBuildRules( inSolObj ) :
		lines = []
		
		for pname in inSolObj.projects :
			pobj = find_project( pname )
			projName = pobj.uri.replace( '.','_' )
			
			for pattern, v in custum_buildrules.items() :

				lines.append( '\n/* Begin PBXBuildRule section */\n' )
				
				br_UUID = xcode_buildrules_uuid( projName, pattern )
				
				
				lines.append( '\t'*2 + 	'%s /* PBXBuildRule */ = {\n'								% br_UUID	)
				lines.append( '\t'*3 + 		'isa = PBXBuildRule;\n' 											)
				lines.append( '\t'*3 + 		'compilerSpec = com.apple.compilers.proxy.script;\n' 				)
				lines.append( '\t'*3 + 		'filePatterns = \"%s\";\n' 								% pattern	)
				lines.append( '\t'*3 + 		'fileType = pattern.proxy;\n' 										)
				lines.append( '\t'*3 + 		'isEditable = 1;\n' 												)
				lines.append( '\t'*3 + 		'outputFiles = (\n' 												)
				lines.append( '\t'*4 + 			'\"%s\"\n' 											% v[1]		)
				lines.append( '\t'*3 + 		');\n' 																)
				
				lines.append( '\t'*3 + 		'script = \"%s\";\n' 									% v[0]		)
				
				
				lines.append( '\t'*2 + 	'};\n'																	)
		
		return lines
		
	def genTargDependency( inSolObj ) :
		lines = []
		
		countDep = 0
		for pname in inSolObj.projects :
			pobj = find_project( pname )
			projName = pobj.uri.replace( '.','_' )
			
			pdeps = inSolObj.dependencies.get( pname, [] )
			for pURIdep in pdeps :
				if countDep == 0 :
					lines.append( '\n/* Begin PBXTargetDependency section */\n' )
					
				countDep += 1;
				
				depProj_Name = pURIdep.replace( '.', '_' )
				depProj_NativeTargUUID	  = xcode_nativeTarget_uuid			( depProj_Name )
				PBXTargetDependencyUUID	  = xcode_targetDep_uuid			( projName, depProj_Name )
				PBXContainerItemProxyUUID = xcode_containerItemProxy_uuid	( projName, depProj_Name )
				
				
				lines.append( '\t'*2 + 	'%s /* %s */ = {\n' 				% ( PBXTargetDependencyUUID, 'PBXTargetDependency' ) 	)
				lines.append( '\t'*3 + 		'isa = PBXTargetDependency;\n' 															)
				lines.append( '\t'*3 + 		'target = %s /* %s */;\n' 		% ( depProj_NativeTargUUID, depProj_Name ) 				)
				lines.append( '\t'*3 + 		'targetProxy = %s /* %s */;\n' 	% ( PBXContainerItemProxyUUID, depProj_Name ) 			)
				lines.append( '\t'*2 + 	'};\n' 																						)
			
		if countDep > 0 :
			lines.append( '/* End PBXTargetDependency section */\n' )
			appendList( lines, genContainerItemProxy( inSolObj ) )
		
		return lines
	
	
	
	# generate a pre/post action

	def gen_build_action ( output, inProjObj, stage ) :
		# stage is 'prebuild' or 'postbuild'
		build = inProjObj.attributes.get( stage )
		if build :
			act, args = build
			actobj = sidl.actions.get( act )
			if actobj.file :
				return prebuild_action.file, {}
			else :
				pypath = inProjObj.GetOutpath( defext='py' )
				pylines = [ actobj.text ]
				pylines.append( build_action_launcher )
				
				
				outScriptFile = SIDL_Outputfile(	path = pypath,
													source = inProjObj,
													label = "sidl: XCode %s script '%s' exported [%s]." % (stage,inProjObj.uri,pypath),
													lines = pylines,
													readonly = False	)
													
				output.append( outScriptFile )

				return pypath, args
	
	
	def genNativeTarget( inSolObj ) :
		lines = []
		lines.append( '\n/* Begin PBXNativeTarget section */\n' )
		
		for pname in inSolObj.projects :
			projObj = find_project( pname )
			projName = projObj.uri.replace( '.','_' )
			
			projbuild = projObj.GetAttribute('build') or 'program'
			isLib = ( projbuild == 'archive' );
			
			preBuild  = projObj.attributes.get( 'prebuild' )
			postBuild = projObj.attributes.get( 'postbuild' )
			
			pdeps = inSolObj.dependencies.get( pname, [] )
			
			productName = projName
			productName += xcode_product_define[projbuild]["outfileext"]
			productType  = xcode_product_define[projbuild]["type"]
			
			
			lines.append		( '\t'*2 + 	'%s /* %s */ = {\n' 							% ( xcode_nativeTarget_uuid( projName ), projName ) 			)
			lines.append		( '\t'*3 + 		'isa = PBXNativeTarget;\n'																					)
			lines.append		( '\t'*3 + 		'buildConfigurationList = %s;\n'			% xcode_configList_uuid( projName )								)
			lines.append		( '\t'*3 + 		'buildPhases = (\n' 																						)
			if preBuild :
				lines.append	( '\t'*4 +			'%s	/* PreBuild ShellScript */,\n'		% xcode_shellScriptPreBuildPhase_uuid ( projName )				)
			lines.append		( '\t'*4 + 			'%s /* Resources */,\n' 				% xcode_resourceBuildPhase_uuid( projName )						)
			lines.append		( '\t'*4 + 			'%s /* Sources */,\n' 					% xcode_sourceBuildPhase_uuid( projName )						)
			if not isLib :
				lines.append	( '\t'*4 + 			'%s /* Frameworks */,\n' 				% xcode_frameworkBuildPhase_uuid( projName )					)
			if postBuild :
				lines.append	( '\t'*4 +			'%s	/* PostBuild ShellScript */,\n'		% xcode_shellScriptPostBuildPhase_uuid( projName ) 				)
			lines.append		( '\t'*3 + 		');\n' 																										)
			lines.append		( '\t'*3 + 		'buildRules = (\n' 																							)
			for pat, v in custum_buildrules.items() :
				lines.append	( '\t'*4 + 			'%s /* PBXBuildRule */,\n				'% xcode_buildrules_uuid( projName, pat )						)
			lines.append		( '\t'*3 + 		');\n' 																										)
			lines.append		( '\t'*3 + 		'dependencies = (\n' 																						)
			for pURIdep in pdeps :
				lines.append	( '\t'*4 +			'%s /* PBXTargetDependency */,\n' 		% xcode_targetDep_uuid( projName, pURIdep.replace( '.', '_' ) )	)
			lines.append		( '\t'*3 + 		');\n' )
			lines.append		( '\t'*3 + 		'name = \"%s\";\n'							% projName 														)
			lines.append		( '\t'*3 + 		'productName = \"%s\";\n'					% projName 														)
			lines.append		( '\t'*3 + 		'productReference = %s /* %s */;\n'			% ( xcode_fileref_uuid( productName ), productName )			)
			lines.append		( '\t'*3 + 		'productType = \"%s\";\n'					% ( productType )												)
			lines.append		( '\t'*2 +	'};\n'																											)
		
		lines.append( '/* End PBXNativeTarget section */\n' )
		return lines
	
	
	
	def genProject( inSolObj ) :
		lines = []
		
		solPath = inSolObj.GetOutpath() + '_' + sidl.GetPlatform() + '.xcodeproj'
		solUUID = xcode_uuid( solPath )
		
		solName = inSolObj.uri.replace( '.','_' )
		
		
		lines.append( '\n/* Begin PBXProject section */\n' )
		
		lines.append( '\t'*2 + 	'%s /* Project object */ = {\n'				% solUUID											)
		lines.append( '\t'*3 + 		'isa = PBXProject;\n' 																		)
		lines.append( '\t'*3 + 		'buildConfigurationList = %s;\n'		% xcode_configList_uuid( 'PBXProject' )				)		#Link with XCConfigurationList_PBXProject
		lines.append( '\t'*3 + 		'compatibilityVersion = "Xcode 3.1";\n'														)
		lines.append( '\t'*3 + 		'hasScannedForEncodings = 1;\n'																)
		lines.append( '\t'*3 + 		'mainGroup = %s /* MAINGROUP */;\n'		% xcode_group_uuid( 'MAINGROUP' )					)
		lines.append( '\t'*3 + 		'projectDirPath = "";\n'																	)
		lines.append( '\t'*3 + 		'projectRoot = "";\n'																		)
		lines.append( '\t'*3 + 		'targets = (\n'																				)
		for pname in inSolObj.projects :
			projName = find_project( pname ).uri.replace( '.','_' );
			lines.append( '\t'*4 + 		'%s /* PBXNativeTarget %s */,\n '	% ( xcode_nativeTarget_uuid( projName ), projName )	)		# Link with PBXNativeTarget ID
		lines.append( '\t'*3 + 		');\n '																						)
		lines.append( '\t'*2 + 	'};\n '																							)
		
		
		lines.append( '/* End PBXProject section */\n' )
		return lines
	
	
	
	
	'''
		PBX_xxxxx_BuildPhases : 
			- Framework	->	PBXFrameworksBuildPhase
			- Resources ->	PBXResourcesBuildPhase
			- Sources	->	PBXSourcesBuildPhase
	'''
	
	def genFwBuildPhase( inSolObj ) :
		lines = []
		lines.append( '\n/* Begin PBXFrameworksBuildPhase section */\n' )
		
		for pname in inSolObj.projects :
			projObj	  = find_project( pname )
			projBuild = projObj.GetAttribute('build')  or 'program'
			
			if projBuild != 'archive' :
				projName = projObj.uri.replace( '.','_' )
				projDeps = inSolObj.dependencies.get( pname, [] )
				frameworksUUID = xcode_frameworkBuildPhase_uuid( projName )
				
				lines.append( '\t'*2 + 	'%s /* Frameworks for %s */ = {\n' 		% ( frameworksUUID, projName )		)
				lines.append( '\t'*3 + 		'isa = PBXFrameworksBuildPhase;\n' 										)
				lines.append( '\t'*3 + 		'buildActionMask = 2147483647;\n' 										)
				lines.append( '\t'*3 + 		'files = (\n' 															)
				
				fRefA = []
				# Target dependency
				for pURIdep in projDeps :
					dt_Name    = pURIdep.replace( '.', '_' ) + '.a';
					dt_UUID_BF = xcode_buildfile_uuid( projName, dt_Name );
					lines.append( '\t'*4 + 		'%s /* %s */,\n' 				% ( dt_UUID_BF, dt_Name )			)
					
					linkPObj = find_project( pURIdep )
					linkPName = linkPObj.uri.replace( '.','_' )
					linklibs = collect_project_attribute( linkPObj, 'linklibs' )
					for v in linklibs :
						if v :
							lib_Name	= v;
							lib_UUID_BF = xcode_buildfile_uuid( projName, lib_Name );
							if not lib_UUID_BF in fRefA :
								fRefA.append(lib_UUID_BF)
								lines.append( '\t'*4 +		'%s /* %s in Frameworks */,\n' 	% ( lib_UUID_BF, projName ) )
				
				# Add default frameworks
				for k, v in xcode_default_frameworks.items() :
					fw_Name	   = k;
					fw_UUID_BF = xcode_buildfile_uuid( projName, fw_Name );
					lines.append( '\t'*4 +		'%s /* %s in Frameworks */,\n' 	% ( fw_UUID_BF, fw_Name ) 			)
			
				lines.append( '\t'*3 + 		');\n' 																	)
				lines.append( '\t'*3 + 		'runOnlyForDeploymentPostprocessing = 0;\n' 							)
				lines.append( '\t'*2 + 	'};\n' 																		)
		
		lines.append( '/* End PBXFrameworksBuildPhase section */\n' );
		return lines;
	
	
	def genShellScriptBuildPhase_Unit( inShellScriptUUID, inProjName, inShellPathInterpretor, inShellScriptLine ) :
		lines = []
		
		lines.append( '\t'*2 + 	'%s /* ShellScript for %s*/ = {\n' 				% ( inShellScriptUUID, inProjName )	)
		lines.append( '\t'*3 + 		'isa = PBXShellScriptBuildPhase;\n' 											)
		lines.append( '\t'*3 + 		'buildActionMask = 2147483647;\n' 												)
		lines.append( '\t'*3 + 		'files = (\n' 																	)
		lines.append( '\t'*3 + 		');\n' 																			)
		lines.append( '\t'*3 + 		'inputPaths = (\n' 																)
		lines.append( '\t'*3 + 		');\n' 																			)
		lines.append( '\t'*3 + 		'outputPaths = (\n' 															)
		lines.append( '\t'*3 + 		');\n' 																			)
		lines.append( '\t'*3 + 		'runOnlyForDeploymentPostprocessing = 0;\n' 									)
		lines.append( '\t'*3 + 		'shellPath = %s\n' 							% inShellPathInterpretor			)
		lines.append( '\t'*3 + 		'shellScript = "%s";\n' 					% inShellScriptLine					)
		lines.append( '\t'*2 + 	'};\n' 																				)
		
		return lines
	
	
	def genShellScriptBuildPhase( output, inSolObj ) :
		lines = []
		lines.append( '\n/* Begin PBXShellScriptBuildPhase section */\n' )
		
		
		solPath = inSolObj.GetOutpath( defext='xcodeproj' );
		for pname in inSolObj.projects :
			projObj		= find_project( pname )
			projName	= projObj.uri.replace( '.','_' );
			
			pypath 		= 'atpython'
			varProj 	= 'project=\\\"$TARGET_BUILD_DIR\\\"'
			varTarg 	= 'target=\\\"$TARGET_BUILD_DIR/$FULL_PRODUCT_NAME\\\"'
			shellPath	= "/bin/sh;"
			
			prebuild = gen_build_action( output, projObj, 'prebuild' )
			if prebuild :
				actpath, actargs		= prebuild
				relpath					= '\\\"%s\\\"' % fs.FileToFile( actpath, solPath )
				alist					= [ '%s=\\\"%s\\\"'%(str(n),str(v)) for n,v in actargs.items() ] if actargs else ''
				callfunc				= 'call=\\\"prebuild\\\"'
				
				shellScriptLine			= '%s -u %s %s %s %s %s' % ( pypath, relpath, callfunc, varProj, varTarg, ' '.join(alist) )
				shellScriptUUID			= xcode_shellScriptPreBuildPhase_uuid( projName )
				
				appendList( lines, genShellScriptBuildPhase_Unit( shellScriptUUID, projName, shellPath, shellScriptLine ) )
			
			
			postbuild = gen_build_action( output, projObj, 'postbuild' )
			if postbuild :
				actpath, actargs		= postbuild
				relpath					= '\\\"%s\\\"' % fs.FileToFile( actpath, solPath )
				alist					= [ '%s=\\\"%s\\\"'%(str(n),str(v)) for n,v in actargs.items() ] if actargs else ''
				callfunc				= 'call=\\\"postbuild\\\"'
				
				shellScriptLine			= '%s -u %s %s %s %s %s' % ( pypath, relpath, callfunc, varProj, varTarg, ' '.join(alist) )
				shellScriptUUID			= xcode_shellScriptPostBuildPhase_uuid( projName )
				
				appendList( lines, genShellScriptBuildPhase_Unit( shellScriptUUID, projName, shellPath, shellScriptLine ) )

		lines.append( '/* End PBXShellScriptBuildPhase section */\n' )
		return lines
	
	def genResBuildPhase( inSolObj ) :
		lines = []
		lines.append( '\n/* Begin PBXResourcesBuildPhase section */\n' )
		
		for pname in inSolObj.projects :
			projObj = find_project( pname )
			if projObj :
				projName = projObj.uri.replace( '.','_' );
				resourceUUID = xcode_resourceBuildPhase_uuid( projName );
				
				lines.append( '\t'*2 + 	'%s /* Resources for %s */ = {\n' 				% ( resourceUUID, projName )	)
				lines.append( '\t'*3 + 		'isa = PBXResourcesBuildPhase;\n'											)
				lines.append( '\t'*3 + 		'buildActionMask = 2147483647;\n'											)
				lines.append( '\t'*3 + 		'files = (\n'																)
				# include .xib...
				lines.append( '\t'*3 + 		');\n' 																		)
				lines.append( '\t'*3 + 		'runOnlyForDeploymentPostprocessing = 0;\n'									)
				lines.append( '\t'*2 + 	'};\n'																			)
		
		lines.append( '/* End PBXResourcesBuildPhase section */\n' )
		return lines
	
	def genSrcBuildPhase_Unit ( inSolPath, inProjName, fname, fvalue ) :
		lines = []
		if atmake.IsDict(fvalue) :
			for k,v in fvalue.items() :
				appendList( lines, genSrcBuildPhase_Unit( inSolPath, inProjName, k, v ) );
		elif atmake.IsList(fvalue) and fname=='files' :
			for files,options in fvalue :
				for f in files :
					excluded = ('excludedfrombuild' in atmake.ToList(options))
					if not excluded :
						fileExt = atmake.GetFileExt(f)
						if fileExt in ext_sources_build_phase :
							rPath = fs.FileToFile( f, inSolPath ).replace( '\\', '/' );
							lines.append( '\t'*4 + '%s /* %s in Sources */,\n' 		% ( xcode_buildfile_uuid( inProjName, rPath ), os.path.basename( f ) ) );
		return lines
	
	def genSrcBuildPhase( inSolObj ) :
		lines = [];
		lines.append( '\n/* Begin PBXSourcesBuildPhase section */\n' )
		
		solPath = inSolObj.GetOutpath( defext='xcodeproj' );
					
		for pname in inSolObj.projects :
			projObj = find_project( pname )
			projName = projObj.uri.replace( '.','_' )
			sourceUUID = xcode_sourceBuildPhase_uuid( projName )
	
			lines.append( '\t'*2 + 	'%s /* Sources for %s */ = {\n' 			% ( sourceUUID, projName ) 	)
			lines.append( '\t'*3 + 		'isa = PBXSourcesBuildPhase;\n' 									)
			lines.append( '\t'*3 + 		'buildActionMask = 2147483647;\n' 									)
			lines.append( '\t'*3 + 		'files = (\n' 														)
			
			filters = get_filters( projObj );
			for k,v in filters.items() :
				appendList( lines, genSrcBuildPhase_Unit( solPath, projName, k, v ) );
			
			lines.append( '\t'*3 +		');\n' 																)
			lines.append( '\t'*3 +		'runOnlyForDeploymentPostprocessing = 0;\n' 						)
			lines.append( '\t'*2 +	'};\n' 																	)
		
		lines.append( '/* End PBXSourcesBuildPhase section */\n' )
		return lines
	
	
	def genXCBuildConfigPBXProject_Unit( inConfName, inConfType ) :
		lines = []
		UUID = xcode_buildConfiguration_uuid( 'PBXProject', inConfName )

		lines.append( '\t'*2 + '%s /* PBXProject %s */ = {\n' %( UUID, inConfName ) );
		lines.append( '\t'*3 + 'isa = XCBuildConfiguration;\n' );
		lines.append( '\t'*3 + 'buildSettings = {\n' );
		
		
		pbxProjCConf = get_pbxproject_config( inConfType )
		
		for var,val in pbxProjCConf.items() :

			if atmake.IsList( val ) :
				lines.append( '\t'*4 + '%s = (\n' % var );
				for v in val :
					lines.append( '\t'*5 + '\"%s\",\n' % v );
				lines.append( '\t'*4 + ');\n' );
				
			else :
				lines.append( '\t'*4 + '%s = %s;\n' % ( var, val ) );
		
		lines.append( '\t'*3 + 		'};\n' );
		lines.append( '\t'*3 + 		'name = \"%s\";\n' % inConfName );
		lines.append( '\t'*2 + 	'};\n' );
		return lines
	
	
	def genXCBuildConfigPBXProject_All( inSolObj ) :
	
		pfm_name = sidl.GetPlatform()
		pfm_configs = {'IOS':xcode_iphone_config, 'DARWIN':xcode_darwin_config}[ pfm_name ]
		
		
		lines = []
		for confType, confEntry in pfm_configs.items() :
		
			if confType == 'Distribution' :
				appendList( lines, genXCBuildConfigPBXProject_Unit( 'Distribution', confType ) )
			else :
				for confMode, confList in confEntry.items() :
					confName = '%s - %s' % ( confType, confMode );
					appendList( lines, genXCBuildConfigPBXProject_Unit( confName, confType ) )
			
		return lines;
	
	
	def genXCBuildConfigPBXNativeTarget_Unit( inConfName, inConfType, inConfList, inProjName, inProjObj, inSolObj ) :
		lines = []
		UUID = xcode_buildConfiguration_uuid( inProjName, inConfName )
	
		lines.append( '\t'*2 + '%s /* PBXNativeTarget %s */ = {\n' %( UUID, inConfName ) );
		lines.append( '\t'*3 + 'isa = XCBuildConfiguration;\n' );
		lines.append( '\t'*3 + 'buildSettings = {\n' );
		
		
		fConfLst = {}
		for k,v in native_target_config.items() :
			fConfLst[k] = v
		for k,v in inConfList.items() :
			fConfLst[k] = v
		
		
		ldFlags = None
		for var,val in fConfLst.items() :
			
			if inProjObj and var == 'GCC_PREPROCESSOR_DEFINITIONS' :
				val = val + collect_project_attributes( inProjObj, 'cppdefines', inConfName );
			
			if inProjObj and var == 'LIBRARY_SEARCH_PATHS' :
				val = val + collect_project_attributes( inProjObj, 'linkpath', inConfName );
				
				projBuild = inProjObj.GetAttribute('build')  or 'program'
				if projBuild != 'archive' :
					# From dep
					pdeps = inSolObj.dependencies.get( inProjName.replace( '_', '.' ), [] )
					for pURIdep in pdeps :
						linkPObj = find_project( pURIdep )
						val.append( os.path.dirname( linkPObj.GetAttribute( 'target' ) ))				# add target dir
						val = val + collect_project_attributes( linkPObj, 'linkpath', inConfName )		# add dependency target dir
			
			if atmake.IsList( val ) :
				lines.append( '\t'*4 + '%s = (\n' % var );
				for v in val :
					if inProjObj and var == 'GCC_PREPROCESSOR_DEFINITIONS' :
						v = v.replace( '"', '\\\\\\"' )
					lines.append( '\t'*5 + '\"%s\",\n' % v )
					
				lines.append( '\t'*4 + ');\n' )
			
			else :
				if var == 'OTHER_LDFLAGS' :
					ldFlags = val
				elif var == 'TargetExtention' :
					targetExt = val
				else :
					lines.append( '\t'*4 + '%s = %s;\n' % ( var, val ) )
		
		if inProjObj :																					# Add automatic specific options to PBXNativeTarget
			# Add HEADER_SEARCH_PATHS
			cpppaths = collect_project_attributes( inProjObj, 'cpppath', inConfName )

			lines.append( '\t'*4 + 'HEADER_SEARCH_PATHS = (\n' )
			for cpppath in cpppaths :
				if cpppath :
					lines.append( '\t'*5 + '\"%s\",\n' % cpppath )
			lines.append( '\t'*4 + ');\n' )

			# Add PRODUCT_NAME
			productName = inProjName # + targetExt;
			lines.append( '\t'*4 + 'PRODUCT_NAME = %s;\n' 		% productName )
			
			# Add PList file
			projbuild = inProjObj.GetAttribute('build')  or 'program';
			isLib = ( projbuild=='archive' );
			if not isLib :
				infoPlistName = '\"%s%s.plist\"' %  ( inProjName, xcode_iphone_plist_ext[inConfType] )
				lines.append( '\t'*4 + 'INFOPLIST_FILE = %s;\n' 	% infoPlistName )
				
				if ldFlags != None :
					lines.append( '\t'*4 + 'OTHER_LDFLAGS = \"%s\";\n' 	% ldFlags )
			
			
			target		= inProjObj.GetAttribute( 'target' )
			targetPath	= '%s' % ( os.path.dirname( target ) )
			lines.append( '\t'*4 + 'SYMROOT = %s;\n' % targetPath )
			lines.append( '\t'*4 + 'CONFIGURATION_BUILD_DIR = %s;\n' % targetPath )
			lines.append( '\t'*4 + 'OBJROOT = %s;\n' % targetPath )
			
		
		lines.append( '\t'*3 + 		'};\n' );
		lines.append( '\t'*3 + 		'name = \"%s\";\n' % inConfName );
		lines.append( '\t'*2 + 	'};\n' );
		return lines
	
	
	def genXCBuildConfigPBXNativeTarget_All( inProjName, inProjObj, inSolObj ) :

		pfm_name = sidl.GetPlatform()
		pfm_configs = {'IOS':xcode_iphone_config, 'DARWIN':xcode_darwin_config}[ pfm_name ]
		
		
		lines = []
		for confType, confEntry in pfm_configs.items() :
		
			if confType == 'Distribution' :
				appendList( lines, genXCBuildConfigPBXNativeTarget_Unit( 'Distribution', confType, confEntry, inProjName, inProjObj, inSolObj ) )
				
			else :
				for confMode, confList in confEntry.items() :
					confName = '%s - %s' % ( confType, confMode );
					appendList( lines, genXCBuildConfigPBXNativeTarget_Unit( confName, confType, confList, inProjName, inProjObj, inSolObj ) )
			
		return lines;

	def genXCBuildConfig( inSolObj ) :
		lines = []
		lines.append( '\n/* Begin XCBuildConfiguration section */\n' )
		
		
		
		appendList( lines, genXCBuildConfigPBXProject_All( inSolObj ) )
	#	appendList( lines, genXCBuildConfig_Unit( 'PBXProject', 'PBXProject', None, inSolObj ) )
		
		
		for pname in inSolObj.projects :
			pobj = find_project( pname )
			projName = pobj.uri.replace( '.', '_' )
			
			appendList( lines, genXCBuildConfigPBXNativeTarget_All( projName, pobj, inSolObj ) )
		
		lines.append( '/* End XCBuildConfiguration section */\n' )
		return lines
	
	


	def genXCConfigListNatTarg_Unit( inProjName ) :
		lines = [];
		
		lines.append( '\t'*2 + '%s /* Build configuration list for PBXNativeTarget %s */ = {\n' % ( xcode_configList_uuid( inProjName ), inProjName ) );
		lines.append( '\t'*3 + 		'isa = XCConfigurationList;\n' );
		lines.append( '\t'*3 + 		'buildConfigurations = (\n' );
	
	
		pfm_name = sidl.GetPlatform()
		pfm_configs = {'IOS':xcode_iphone_config, 'DARWIN':xcode_darwin_config}[ pfm_name ]
		

		for confType, confEntry in pfm_configs.items() :
		
			if confType == 'Distribution' :
				confName = 'Distribution'
				confUUID = xcode_buildConfiguration_uuid( inProjName, confName );
				lines.append( '\t'*4 + '%s /* %s */,\n' %( confUUID, confName ) );
				defaultConfName = confName;
				
			else :
				for confMode, confList in confEntry.items() :
					confName = '%s - %s' % ( confType, confMode );
					confUUID = xcode_buildConfiguration_uuid( inProjName, confName );
					lines.append( '\t'*4 + '%s /* %s */,\n' %( confUUID, confName ) );
					defaultConfName = confName;
		
		lines.append( '\t'*3 + 		');\n' );
		lines.append( '\t'*3 + 		'defaultConfigurationIsVisible = 0;\n' );
		lines.append( '\t'*3 + 		'defaultConfigurationName = \"%s\";\n' % defaultConfName );
		lines.append( '\t'*2 + 	'};\n' );
			
		return lines;
	
	
	def genXCConfigList( inSolObj ) :
		lines = [];
		lines.append( '\n/* Begin XCConfigurationList section */\n' );
		
		appendList( lines, genXCConfigListNatTarg_Unit( 'PBXProject' ) );
		
		# PBXNativeTarget ConfigList
		for pname in inSolObj.projects :
			projName = find_project( pname ).uri.replace( '.', '_' );
			appendList( lines, genXCConfigListNatTarg_Unit( projName ) );
		
		return lines;
	
	
	
	
	
	def genXCodeProj( output, inSolObj ) :
		lines = []
		appendList( lines, genBuildFile				( inSolObj ) )
		appendList( lines, genFileRef				( inSolObj ) )
		appendList( lines, genGroup					( inSolObj ) )
		appendList( lines, genCustumBuildRules		( inSolObj ) )
		appendList( lines, genTargDependency		( inSolObj ) )
		appendList( lines, genNativeTarget			( inSolObj ) )
		appendList( lines, genProject				( inSolObj ) )
		appendList( lines, genFwBuildPhase			( inSolObj ) )
		appendList( lines, genShellScriptBuildPhase	( output, inSolObj ) )
		appendList( lines, genResBuildPhase			( inSolObj ) )
		appendList( lines, genSrcBuildPhase			( inSolObj ) )
		appendList( lines, genXCBuildConfig			( inSolObj ) )
		appendList( lines, genXCConfigList			( inSolObj ) )
		return lines
	
	
	
	def gen_solution ( output, inSolObj ) :
	
		solPath = inSolObj.GetOutpath() + '_' + sidl.GetPlatform() + '.xcodeproj'
		solDir = os.path.dirname( solPath )
		
		solUUID = xcode_uuid( solPath )
		
		solLines = []
		solLines.append( 		'// !$*UTF8*$!\n'												)
		solLines.append( 		'{\n'															)
		solLines.append( '\t' +		'archiveVersion = 1;\n'										)
		solLines.append( '\t' +		'classes = {\n' 											)
		solLines.append( '\t' +		'};\n' 														)
		solLines.append( '\t' +		'objectVersion = 45;\n' 									)
		solLines.append( '\t' +		'objects = {\n\n' 											)
		
		appendList( solLines, genXCodeProj( output, inSolObj )  )
		
		solLines.append( '\t' +		'};\n' 														)
		solLines.append( '\t' +		'rootObject = %s /* Project object */;\n'	% ( solUUID )	)
		solLines.append( 		'}\n' 															)
		
		
		
		
		outsol = SIDL_Outputfile(	path = '%s/project.pbxproj' % solPath,
									source = inSolObj,
									label = "XCode solution '%s'" % inSolObj.uri,
									lines = solLines,
									readonly = False	)

		output.append( outsol )
		
		
		# plist could be an user file ... it could be hand-written
		for pname in inSolObj.projects :
			projObj  = find_project( pname )
			projName = projObj.uri.replace( '.', '_' );
		
			projbuild = projObj.GetAttribute('build') or 'program'
			isLib = ( projbuild=='archive' )
			if not isLib :
				pfm_configs = {'IOS':xcode_iphone_config, 'DARWIN':xcode_darwin_config}[ sidl.GetPlatform() ]
				for confType, confEntry in pfm_configs.items() :
				
					oPList = SIDL_Outputfile(	path = '%s/%s%s.plist' % (solDir, projName, xcode_iphone_plist_ext[confType] ),
												source = projObj,
												label = "Plist projName '%s' for config %s" % ( projName, confType ),
												lines = generate_project_ios_plist_lines(projObj,confType),
												readonly = False,
												cleanable = False	)
					output.append( oPList )

		# register directories to clean up
		sidl.ShelfOutfile( solPath )
		sidl.ShelfOutfile( '%s/%s_build' % ( solDir, inSolObj.uri.replace( '.', '_' ) ) )
	
	
	if sidl is None:
		return

	platform = sidl.GetPlatform()
		
	if platform == 'DARWIN' :
		xcode_default_frameworks = xcode_default_osx_frameworks
	elif platform == 'IOS' :
		xcode_default_frameworks = xcode_default_iphone_frameworks
	else : 
		return

	outputs = []
	
	if solutionname:
		# find the solution to generate
		s = find_solution( solutionname )
		if s :		gen_solution( outputs, s )
		else :		print("sidl: solution '%s' is not defined !" % (solutionname))
	else :
		# generate all solutions
		for s in sidl.solutions.values() :
			gen_solution( outputs, s )

	# writeback all outputs
	for o in [x for x in outputs if x] :
		o.Writeback(sidl,verbose)
	


