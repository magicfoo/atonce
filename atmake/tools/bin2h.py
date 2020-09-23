"""
	Bin 2 h Tool

	AtOnce Technologies
	(c)2002-2009 Gerald Gainant
"""

import os, re
import atmake
from . import sign



def Convert( tofilename, fromfilename, header=None, arrayname=None ) :
	fromfile = open( fromfilename, 'rb' )
	if fromfile :
		# assert sub directories exists
		tofile_dir = os.path.dirname( tofilename )
		if tofile_dir and not os.path.isdir( tofile_dir ) :
			os.makedirs( tofile_dir )
		tofile = open(tofilename,'w')
		if tofile :
			if header :
				tofile.writelines( header )
			if arrayname :
				if not atmake.IsString(arrayname) :
					ar1 = os.path.basename(fromfilename)
					ar2 = os.path.splitext(ar1)[0]
					arrayname = '%s_data' % ar2.replace('.','_')
				tofile.write( 'namespace { unsigned char %s[] = {\n\n'%arrayname )
			byte_count = 0
			line = ""
			while 1:
				c = fromfile.read( 1 )
				if not c:	break
				if byte_count==16 :
					tofile.write( line+'\n' )
					line =""
					byte_count = 0
				line += '0x%02x,' % ord(c)
				byte_count = byte_count + 1
			tofile.write( line+'\n' )
			if arrayname :
				tofile.write( '}; }\n' )
			tofile.close()
			fromfile.close()
			return True



def UpdateConvert( tofilename, fromfilename, arrayname=None ) :
	src_md5 = sign.GetFileContentSign( fromfilename )
	tag_md5 = "// %s\n\n" % str(src_md5)
	try :
		tag_src = open(tofilename,'r').read(len(tag_md5))
		if tag_src==tag_md5 :
			return True
	except :
		pass
	return Convert( tofilename, fromfilename, header=tag_md5, arrayname=arrayname )



