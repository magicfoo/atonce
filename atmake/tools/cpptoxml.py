"""
	C++ parser to XML, using gccxml_cc1plus.exe

	AtOnce Technologies
	(c)2002-2008 Gerald Gainant

"""


import os, sys
import atmake
import atmake.utilities
from . import fs
from . import token
from . import spawn
import tempfile
from pygccxml import parser
from pygccxml import declarations
from pygccxml.parser import *




def CheckCpp( infile, incdirs=None, incfiles=None, defines=None, options=None ) :

	# call gccxml_cc1plus.exe
	gccxml = atmake.utilities.FindApp('gccxml_cc1plus')
	cl = r'%s %s ' % (gccxml, options if options else '')

	# -I path
	if incdirs :
		for incdir in atmake.ToList(incdirs) :
			cl += ' -I {%s}' % incdir

	# -include file
	if incfiles :
		for incfile in atmake.ToList(incfiles) :
			cl += ' -include {%s}' % incfile

	# -Dsym
	if defines :
		for sym in atmake.ToList(defines) :
			cl += ' {-D%s}' % sym

	# input cpp source file
	cl += ' {%s}' % infile

	#print cl
	res = spawn.Spawn( cl )

	# ( exec_success, exit_code, stdout lines, stderr lines )
	return res





def ParseCpp( infile, outfile=None, incdirs=None, incfiles=None, defines=None, options=None, startscope=None ) :

	# call gccxml_cc1plus.exe
	gccxml = atmake.utilities.FindApp('gccxml_cc1plus')
	cl = r'%s %s ' % (gccxml, options if options else '-quiet -w')
	# -I path
	if incdirs :
		for incdir in atmake.ToList(incdirs) :
			cl += ' -I {%s}' % incdir

	# -include file
	if incfiles :
		for incfile in atmake.ToList(incfiles) :
			cl += ' -include {%s}' % incfile

	# -Dsym
	if defines :
		for sym in atmake.ToList(defines) :
			cl += ' {-D%s}' % sym

	# -fxml-start=su::CNet for example
	if atmake.IsString(startscope) :
		cl += ' {-fxml-start=%s}' % startscope

	# output XML file
	return_lines = False
	if outfile is None :
		outfile = tempfile.NamedTemporaryFile().name
		return_lines = True
	cl += ' {-fxml=%s}' % outfile

	# input cpp source file
	cl += ' {%s}' % infile

	# print (cl)
	res = spawn.Spawn( cl )
	# print (res)

	if not res :
		return None

	if not res[0] or res[1]!=0 :
		for l in res[3] :
			print(l)
		return None

	if return_lines:
		return  open( outfile, 'r' ).readlines()
	else:
		return True





def ParseXml ( xmlfile=None ) :
	"""
		bypass parser (gccxml call & dcache) ...
		direct call to the __parse_gccxml_created_file( self, gccxml_file )
		
		customizations done on the pygccxml v1.0 :
		
		+ lines 60&61 added in file pygccxml.parser.__init__
		    if not config:
				config = config_t()

		+ line 87 commented in file pygccxml.parser.source_reader.py
			#self.__config.raise_on_wrong_settings()
			
		+ disabled log messages in function __read_version, file pygccxml.parser.scanner.py, line 528, 531, 534, 537
	"""
	if fs.IsFilePath(xmlfile) :
		return parser.parse_xml_file( content=xmlfile )





