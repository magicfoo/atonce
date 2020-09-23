""" ATMake_

Utils base class

"""


import os, subprocess, re
import atmake



_re_cmdargs = [	re.compile( r'{([^}]*)}' ),		# {arg}
				re.compile( r'"([^"]*)"' ),		# "arg"
				re.compile( r"'([^']*)'" )	]	# 'arg'


def cmdline_to_list ( cmdl ) :
	# not a string ?
	if not atmake.IsString(cmdl) :
		return []

	# empty ?
	if not cmdl :
		return []

	# only whitespace ?
	if cmdl.isspace() :
		return []

	# {arg} or "arg" or 'arg'
	for _re in _re_cmdargs :
		so = _re.search( cmdl )
		if so :
			arg1 = so.string[:so.start()]
			arg2 = so.group( 1 )
			arg3 = so.string[so.end():]
			return cmdline_to_list(arg1) + [arg2] + cmdline_to_list(arg3)

	# whitespace
	asep = cmdl.split()
	args = []
	for a in asep :
		args.append( a.strip() )
	return args



def Spawn ( cmdline, cwd=None, env=None, showcl=None ) :
	"""
		returns :
			( exec_success, exit_code, stdout lines, stderr lines )
	"""
	def __readlines( ilines ) :
		olines = []
		ol = ''
		for il in ilines :
			if il==ord('\r') :
				continue
			if il==ord('\n') :
				olines.append( ol+'\r\n' )
				ol = ''
			else :
				ol += chr(il)
		return olines

	if atmake.IsString(cmdline) :
		cmdlist = cmdline_to_list( cmdline )
	elif atmake.IsList(cmdline) :
		cmdlist = cmdline
	else :
		atmake.Error( "Command-line must be a string or a list of strings !" )

	if showcl :
		print(cmdlist)

	# disable cl.exe direct redirection to visual IDE output window !
	if env and 'VS_UNICODE_OUTPUT' in env :
		env['VS_UNICODE_OUTPUT'] = ''

	oo = ee = ''
	try :
		p = subprocess.Popen(	cmdlist,
								stdout = subprocess.PIPE,
								stderr = subprocess.PIPE,
								cwd = cwd,
								env = env)
		oo, ee = p.communicate()
		oo = __readlines( oo )
		ee = __readlines( ee )
		retcode = p.returncode
		return ( True, retcode, oo, ee )
	except OSError as e :
		print(oo)
		print(ee)
		return ( False, 0, None, None )





