

import os, sys, zipfile, time, getpass, glob
import atmake
import atmake.utilities as utilities
import atmake.tools.fs as fs
import atmake.tools.svn as svn
import atmake.tools.spawn as spawn
import atmake.tools.cpptools as cpptools
import atmake.tools.wreg as wreg
import atmake.sidl as sidl
import atmake.sidl.subst as subst


ATBASE		= fs.Path( os.path.dirname(__file__)+'/../..' )
SUGAR_DIR	= os.path.split(os.path.dirname(__file__))[0]
SUGAR_SIDL	= os.path.dirname(__file__) + '/fw.sidl'
SUGAR_SHELF	= os.path.dirname(__file__) + '/fw.shelf'



su_header = '/*LIC-HDR********************************************************************\n' \
	+ '**\n' \
	+ '** Copyright (C) 2005-2011 AtOnce Technologies\n' \
	+ '**\n' \
	+ '** This file is part of the Sugar core-system framework.\n' \
	+ '**\n' \
	+ '** This file and the associated product may be used and distributed\n' \
	+ '** under the terms of a current License as defined by AtOnce Technologies\n' \
	+ '** and appearing in the file LICENSE.TXT included in the packaging of this file.\n' \
	+ '**\n' \
	+ '** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE\n' \
	+ '** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\n' \
	+ '**\n' \
	+ '** See http://www.atonce-technologies.com or email info@atonce-technologies.com\n' \
	+ '** for informations about Sugar Framework and AtOnce Technologies others products\n' \
	+ '** License Agreements.\n' \
	+ '**\n' \
	+ '** Contact info@atonce-technologies.com if any conditions of this license\n' \
	+ '** are not clear to you.\n' \
	+ '**\n' \
	+ '*****************************************************************LIC-HDR*/\n'





def build_globals ( verbose=None, platform=None ) :
	""" hidden
	"""

	g = {
		'sufw'				:	SUGAR_DIR,
		'outpath'			: 	SUGAR_DIR,
		'inpath'			:	SUGAR_DIR,
		'__REVISION__'		:	svn.GetRevision(SUGAR_DIR),
		'__PROJECT__'		:	'SUFW',
		'__SUFW__'			:	1,
	}

	return g



def shelf ( verbose=None, platform=None ) :

	"""	Display persistent informations (shelf objects).
	"""

	g = build_globals( verbose=verbose, platform=platform )
	sidl.shelf( sidlfile=SUGAR_SIDL, shelffile=SUGAR_SHELF, verbose=verbose, platform=platform, env=g )



def files (	verbose=None, platform=None ) :

	"""	Display generated files.
	"""

	g = build_globals( verbose=verbose, platform=platform )
	sidl.files( sidlfile=SUGAR_SIDL, shelffile=SUGAR_SHELF, verbose=verbose, platform=platform, env=g )



def clean (	verbose=None, verboseonly=None, platform=None ) :

	"""	Removes all target files.
	"""

	g = build_globals( verbose=verbose, platform=platform )
	sidl.clean( sidlfile=SUGAR_SIDL, shelffile=SUGAR_SHELF, verbose=verbose, verboseonly=verboseonly, platform=platform, env=g )



def dump ( verbose=True, platform=None ) :

	"""	Dumps objects.
		<verbose>   Outputs some informations if True
	"""

	g = build_globals( verbose=verbose, platform=platform )
	sidl.dump( sidlfile=SUGAR_SIDL, shelffile=SUGAR_SHELF, verbose=verbose, platform=platform, env=g )



def gencode ( verbose=None, platform=None, genline=True ) :

	"""	Generates source code for interfaces and implementations.
		<verbose>   Outputs some informations if True
		<genline>   Generate CPP #line directives if True
	"""
	g = build_globals( verbose=verbose, platform=platform )
	sidl.gencode( sidlfile=SUGAR_SIDL, shelffile=SUGAR_SHELF, verbose=verbose, platform=platform, env=g, header=su_header, genline=genline )



def gendoc ( verbose=None, platform=None ) :

	"""	Generates documentation files.
		<verbose>   Outputs some informations if True
	"""

	g = build_globals( verbose=verbose, platform=platform )
	sidl.gendoc( sidlfile=SUGAR_SIDL, shelffile=SUGAR_SHELF, verbose=verbose, platform=platform, env=g )



def genproj	( solutionname=None, verbose=None, precompiled=None, platform=None ) :

	"""	Generates project files from an SIDL file.
		<verbose>       Outputs some informations if True.
		<precompiled>	Default if True.
		<solutionname>  The solution name. This parameter is optionnal. If not defined, all solutions are generated.
	"""

	g = build_globals( verbose=verbose, platform=platform )
	sidl.genproj( solutionname=solutionname, sidlfile=SUGAR_SIDL, shelffile=SUGAR_SHELF, verbose=verbose, precompiled=precompiled, platform=platform, env=g )



def genbuild ( verbose=None, platform=None ) :

	"""	Generates documentation files.
		<verbose>   Outputs some informations if True
	"""

	g = build_globals( verbose=verbose, platform=platform )
	sidl.genbuild( sidlfile=SUGAR_SIDL, shelffile=SUGAR_SHELF, verbose=verbose, platform=platform, env=g )


	


def update ( verbose=None, platform=None ) :

	"""	Clean all and generate all
	"""

	print( "clean" );		clean( verbose=verbose, platform=platform )
	print( "gencode" );		gencode( verbose=verbose, platform=platform )
	print( "genproj" );		genproj( verbose=verbose, platform=platform )

	

	

def infos ( ) :

	"""	Outputs some informations
	"""

	import socket
	pfm = sys.platform
	uname = getpass.getuser()
	wsname = os.getenv('COMPUTERNAME')
	hostname = socket.gethostname() or 'none'
	hostaddr = socket.gethostbyname( hostname )
	revision = svn.GetRevision( SUGAR_DIR )

	print ('user name             : %s [%s]' % (uname.lower(),uname))
	print ('computer name         : %s' % wsname)
	print ('host name             : %s' % hostname)
	print ('host addr             : %s' % hostaddr)
	print ('platform name         : %s [%s]' % (pfm.upper(),pfm))
	print ('sugar dir             : %s' % SUGAR_DIR)
	print ('sugar sidl main file  : %s' % SUGAR_SIDL)
	print ('sugar sidl shelf file : %s' % SUGAR_SHELF)
	print ('sugar svn revision    : %s' % str(revision))

'''	
def package ( codename, level=None ) :

	"""	Generate packages ready to be deployed
		<codename> The codename string used to tag the packages
		<what> Target the set of packages to build
			None|1 : most common packages
			2      : addins packages
			all	   : all packages
	"""

	app7z = utilities.FindApp('7z')
	revision = svn.GetRevision( SUGAR_DIR )

	jam = '%s/suapp/jam' % SUGAR_DIR
	subin = '%s/subin' % SUGAR_DIR
	suapp = '%s/suapp' % SUGAR_DIR
	sudata = '%s/sudata' % SUGAR_DIR
	released = '%s/released' % SUGAR_DIR

	def open_package ( pakname ) :
		tmp = '%s/released/atoncetech_%s' % (SUGAR_DIR,pakname)
		fs.RmTree( tmp )
		fs.CreateDir( tmp, rec=True )
		return tmp

	def close_package ( tmp ) :
		sign = fs.DigestTree( tmp )
		zipf = '%s_[%s][r%s][%s].zip' % (tmp,codename,str(revision),str(sign))
		spawn.Spawn( r'%s a -mx9 -tzip {%s} {%s/atoncetech}' % (app7z,zipf,tmp) )
		fs.RmTree( tmp )

	def assemblies_package ( ) :
		tmp = open_package( 'Assemblies' )
		if tmp :
			# assemblies
			fs.OutputPath( '%s/atoncetech/assemblies' % tmp )
			fs.OutputFiles( '%s/assemblies/*.dll' % subin )
		close_package( tmp )

	def jam_package ( ) :
		tmp = open_package( 'Jam' )
		if tmp :
			# binaries
			fs.OutputPath( '%s/atoncetech/jam' % tmp )
			fs.OutputFiles( '%s/jam.profiles' % subin )
			fs.OutputFile( '%s/jam_rls.exe' % subin, 'jam.exe' )
			fs.OutputFile( '%s/deploy/jam_rls.wsf' % jam , 'jam.wsf' )
			# app
			fs.OutputPath( '%s/atoncetech/jam/app' % tmp )
			fs.OutputFiles( '%s/app/*.lua' % jam )
			fs.OutputFiles( '%s/app/*.text' % jam )
			# app.doc
			fs.OutputPath( '%s/atoncetech/jam/app/doc' % tmp )
			fs.OutputFiles( '%s/app/doc/*.mm' % jam )
			fs.OutputFiles( '%s/app/doc/*.pdf' % jam )
			fs.OutputFiles( '%s/app/doc/*.doc' % jam )
			fs.OutputFiles( '%s/app/doc/*.odt' % jam )
			# app.modules
			fs.OutputPath( '%s/atoncetech/jam/app/modules' % tmp )
			fs.OutputFiles( '%s/app/modules/*.lua' % jam )
			# app.nodes
			fs.OutputPath( '%s/atoncetech/jam/app/nodes' % tmp )
			fs.OutputFiles( '%s/app/nodes/*.lua' % jam )
			# app.resources
			fs.OutputPath( '%s/atoncetech/jam/app/resources' % tmp )
			fs.OutputFiles( '%s/app/resources/*.zip' % jam )
			# app.templates
			fs.OutputPath( '%s/atoncetech/jam/app/templates' % tmp )
			fs.OutputFiles( '%s/app/templates/*.template' % jam )
			# app.users
			fs.OutputPath( '%s/atoncetech/jam/app/users' % tmp )
			# project
			fs.OutputPath( '%s/atoncetech/jam/project' % tmp )
			fs.TouchFile( '%s/atoncetech/jam/project/your_project_files_here' % tmp )
			# data
			fs.OutputPath( '%s/atoncetech/jam/data' % tmp )
			fs.OutputFiles( '%s/data/*.sqlite' % jam )
		close_package( tmp )

	def jam_dbg_package ( ) :
		tmp = open_package( 'Jam_dbg' )
		if tmp :
			# binaries
			fs.OutputPath( '%s/atoncetech/jam' % tmp )
			fs.OutputFiles( '%s/jam_dbg.exe' % subin )
			fs.OutputFiles( '%s/jam_dbg.pdb' % subin )
			fs.OutputFiles( '%s/deploy/jam_dbg.wsf' % jam )
		close_package( tmp )

	def jam_samples_package ( ) :
		def output_sample ( inname ) :
			fs.OutputPath( '%s/atoncetech/jam/samples/%s' % (tmp,inname) )
			for ext in 'png tga pdf fx max sua fbx'.split() :
				fs.OutputFiles( '%s/samples/%s/*.%s' % (sudata,inname,ext) )
		tmp = open_package( 'Jam_samples' )
		if tmp :
			output_sample( 's01_cube' )
			output_sample( 's02_maison' )
			output_sample( 's03_atrium' )
			output_sample( 's04_goblin' )
			output_sample( 's05_boss' )
			output_sample( 's06_moulin' )
		close_package( tmp )

	def addins_package ( ) :
		tmp = open_package( 'Addins' )
		if tmp :
			#---- 3DS MAX
			# doc
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/doc' % tmp )
			fs.OutputFiles( '%s/maxplug/doc/*.pdf' % suapp )
			# sources
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/maxplug' % tmp )
			fs.OutputFiles( '%s/maxplug/*.h' % suapp )
			fs.OutputFiles( '%s/maxplug/*.cpp' % suapp )
			fs.OutputFiles( '%s/maxplug/*.c' % suapp )
			fs.OutputFiles( '%s/maxplug/*.aps' % suapp )
			fs.OutputFiles( '%s/maxplug/*.sln' % suapp )
			fs.OutputFiles( '%s/maxplug/*.rc' % suapp )
			fs.OutputFiles( '%s/maxplug/*.vcxproj' % suapp )
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/Graphx' % tmp )
			fs.OutputFiles( '%s/maxplug/Graphx/*.bmp' % suapp )
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/conf' % tmp )
			fs.OutputFiles( '%s/maxplug/conf/*.*' % suapp )
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/conf/GeoFiles' % tmp )
			fs.OutputFiles( '%s/maxplug/conf/GeoFiles/*.*' % suapp )
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/yaml/include' % tmp )
			fs.OutputFiles( '%s/maxplug/yaml/include/*.*' % suapp )
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/yaml/src' % tmp )
			fs.OutputFiles( '%s/maxplug/yaml/src/*.*' % suapp )
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/yaml/src' % tmp )
			fs.OutputFiles( '%s/maxplug/yaml/src/*.*' % suapp )
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/yaml/win32' % tmp )
			fs.OutputFiles( '%s/maxplug/yaml/win32/*.*' % suapp )
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/yaml/win32/vs2010' % tmp )
			fs.OutputFiles( '%s/maxplug/yaml/win32/vs2010/*.vcxproj' % suapp )
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/yaml/win32/vs2010/lib' % tmp )
			fs.OutputFiles( '%s/maxplug/yaml/win32/vs2010/lib/*.*' % suapp )
			# bin
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/bin' % tmp )
			fs.OutputFiles( '%s/*.dle' % subin )
			# resources
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/resources' % tmp )
			fs.OutputFiles( '%s/maxplug/resources/*.*' % suapp )
			# samples
			fs.OutputPath( '%s/atoncetech/addins/3dsmax/samples/maison' % tmp )
			fs.OutputFiles( '%s/maxplug/maison/*.*' % suapp )

			#---- MS Visual
			# binaries
			fs.OutputPath( '%s/atoncetech/addins/msvisual2010' % tmp )
			fs.OutputFiles( '%s/atmake/sidl/suvisual/2010/bin/Release/suvisual2010*.vsix' % DEVCENTER_DIR )
			fs.OutputFiles( '%s/atmake/sidl/suvisual/2010/bin/Release/suvisual2010*.xml' % DEVCENTER_DIR )
			fs.OutputPath( '%s/atoncetech/addins/msvisual2008' % tmp )
			fs.OutputFiles( '%s/atmake/sidl/suvisual/2008/bin/Release/*.dll' % DEVCENTER_DIR )
			fs.OutputFiles( '%s/atmake/sidl/suvisual/2008/bin/Release/*.xml' % DEVCENTER_DIR )
		close_package( tmp )

	if level==1 or level=='all' or level is None :
		assemblies_package()
		jam_package()
		jam_dbg_package()
		jam_samples_package()

	if level==2 or level=='all' :
		addins_package()

'''

		
		
		

