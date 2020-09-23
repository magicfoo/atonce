"""
	VUPP Tool (python internal builder)

	AtOnce Technologies
	(c)2002-2005 Gerald Gainant

	The goal of this tool is to preprocess a mapped registers .vu file
	in order to generate a standard .vsm file (.vu -> .vsm).


	Additional directives are :
	---------------------------

	.vcl
		The file will be passed to vcl.exe

	.name codename
		name of the vu code
		This directive generates labels and global exportation

	.regname name [reg0 [reg1]]
		associates the variable named name to the registers reg0 & reg1
		reg0 is the current register of 'name'
		reg1 is the back register of 'name', accessible as 'name@'

	.regswap
		swap all the current and back register previoulsy associated.

	.regreset
		restore all initial current & back register association


	Special VCL directives are :
	----------------------------

	.vcl_optim_loop
		Enable loop optimization. The -L command line switch is used.

	.vcl_optim_loop_rw
		Enable loop memory optimization. The -S command line switch is used.

	.vcl_no_reduce_code
		Disable code reduction. The -C command line switch is used.

	.vcl_fast_timeout
		Turn loop optimization to 1 second. The -t1 command line switch is used.

	.vcl_semicolon_is_newline
		Enables the convertion of ';' to a newline '\n'.
		Very usefull in CPP define macro as in the following example :
			#define MatrixIdentity( MatRes )									\
				addw.x	MatRes##0, vf00;		mfir.yzw	MatRes##0, vi00;	\
				addw.y	MatRes##1, vf00;		mfir.xzw	MatRes##0, vi00;	\
				addw.z	MatRes##2, vf00;		mfir.xyw	MatRes##0, vi00;	\
				max		MatRes##3, vf00

	.vcl_loop_copy
		Copy the previous loop-block of instructions from the last label.
		This is very usefull to duplicate a loop on a new variable set,
		generate mode code that VCL can interleave to reduce loop size.
		The following code is an example :
			loop:
				--LoopCS 3,3
				lq			xyzk@(ptr)
				mulax		acc,   M0, xyzk@
				madday		acc,   M1, xyzk@
				maddaz		acc,   M2, xyzk@
				maddw.xyz	pxyq@, M3, vf00
				sqi			pxyq@(ptr++)
				.vcl_loop_copy
				ibne		ptr, ptr_end, loop
		Will output :
			loop:
				--LoopCS 3,3
				lq			xyzk_c0(ptr)
				mulax		acc,   M0, xyzk_c0
				madday		acc,   M1, xyzk_c0
				maddaz		acc,   M2, xyzk_c0
				maddw.xyz	pxyq_c0,M3, vf00
				sqi			pxyq_c0,(ptr++)
				lq			xyzk_c1(ptr)
				mulax		acc,   M0, xyzk_c1
				madday		acc,   M1, xyzk_c1
				maddaz		acc,   M2, xyzk_c1
				maddw.xyz	pxyq_c1,M3, vf00
				sqi			pxyq_c1,(ptr++)
				ibne		ptr, ptr_end, loop

		The symbol '@' can be used to mark variable that will be renamed ('_c%d' prefix).
		'@'    = '_c0' prefix
		'@@'   = '_c1' prefix
		'@@@'  = '_c2' prefix
		'@@@@' = '_c3' prefix
"""


import os, sys
from atmake import *
from . import token
from . import spawn



def generate_vcl ( vu_lines ) :
	vcl_opts = []
	opt_optim_loop = False
	opt_optim_loop_rw = False
	opt_reduce_code = True
	opt_semicolon_is_newline = False		# usefull for define () add; mul; ...
	opt_fast_timeout = False

	loop_code_started = False
	loop_code_idx = 0
	loop_code = []

	vcl_lines = [ '\t.vu\n' ]
	for l in vu_lines :
		if l.find('.vcl_optim_loop') >= 0 :
			opt_optim_loop = True

		elif l.find('.vcl_optim_loop_rw') >= 0 :
			opt_optim_loop_rw = True

		elif l.find('.vcl_no_reduce_code') >= 0 :
			opt_reduce_code = False

		elif l.find('.vcl_fast_timeout') >= 0 :
			opt_fast_timeout = True

		elif l.find('.vcl_semicolon_is_newline') >= 0 :
			opt_semicolon_is_newline = True

		elif l.find('.vcl_loop_copy') >= 0 :
			loop_code_idx = loop_code_idx + 1
			for lcl in loop_code :
				lcl = lcl.replace( '@@@@', '_c%d'%(loop_code_idx+3) )		# '@' -> '_?'
				lcl = lcl.replace( '@@@',  '_c%d'%(loop_code_idx+2) )		# '@' -> '_?'
				lcl = lcl.replace( '@@',   '_c%d'%(loop_code_idx+1) )		# '@' -> '_?'
				lcl = lcl.replace( '@',    '_c%d'%(loop_code_idx+0) )		# '@' -> '_?'
				vcl_lines.append( lcl )
			loop_code_started = False

		elif l.find('.vcl') < 0 :
			# ';' -> '\n' ?
			if opt_semicolon_is_newline :
				l = l.replace( ';', '\n\t' )
			# start loop block ?
			if l.find(':') >= 0 :
				loop_code_started = True
				loop_code_idx = 0
				loop_code = []
			elif loop_code_started and l.find('--') < 0 :
				loop_code.append( l )
				l = l.replace( '@@@@', '_c%d'%(loop_code_idx+3) )			# '@' -> '_?'
				l = l.replace( '@@@',  '_c%d'%(loop_code_idx+2) )			# '@' -> '_?'
				l = l.replace( '@@',   '_c%d'%(loop_code_idx+1) )			# '@' -> '_?'
				l = l.replace( '@',    '_c%d'%(loop_code_idx+0) )			# '@' -> '_?'
			vcl_lines.append( l )

	if opt_optim_loop :		vcl_opts += [ 'optim_loop' ]
	if opt_optim_loop_rw :		vcl_opts += [ 'optim_loop_rw' ]
	if opt_reduce_code :		vcl_opts += [ 'reduce_code' ]
	if opt_fast_timeout :		vcl_opts += [ 'fast_timeout' ]
	return ( vcl_lines, vcl_opts )






def generate_vsm ( vu_lines ) :

	class Context :
		""" vupp context holding registers name
		"""
		def __init__( self ) :
			self.name	      = None
			self.reg_mapping  = dict()
			self.reg_mapstate = dict()
			self.reg_vi_names = dict()
			self.reg_vf_names = dict()
			# init reg_names
			for i in range(0,16) :
				self.reg_vi_names[ 'vi%02d'%i ] = []
			for i in range(0,32) :
				self.reg_vf_names[ 'vf%02d'%i ] = []

		def is_vi_reg( self, reg ) :
			return len(reg) >= 3 and reg.lower()[0:2] == 'vi'

		def is_vf_reg( self, reg ) :
			return len(reg) >= 3 and reg.lower()[0:2] == 'vf'

		def is_reg( self, reg ) :
			return self.is_vi_reg(reg) or self.is_vf_reg(reg)

		def norm_reg( self, reg ) :
			reg = reg.lower()
			if not self.is_vi_reg(reg) and not self.is_vf_reg(reg) :
				return None
			if len(reg) != len('VXII') :
				reg = reg[0:2]+'0'+reg[-1]
			return reg

		def add_regname( self, name, reg0, reg1=None ) :
			if not reg1 :
				reg1 = reg0
			reg0 = self.norm_reg(reg0)
			reg1 = self.norm_reg(reg1)
			self.reg_mapping[name]  = (reg0,reg1)
			self.reg_mapstate[name] = (reg0,reg1)
			if self.is_vi_reg(reg0) :
				try :				self.reg_vi_names[reg0].index(name)
				except ValueError :	self.reg_vi_names[reg0] += [name]
			else :
				try :				self.reg_vf_names[reg0].index(name)
				except ValueError :	self.reg_vf_names[reg0] += [name]
			if self.is_vi_reg(reg1) :
				try :				self.reg_vi_names[reg1].index(name)
				except ValueError :	self.reg_vi_names[reg1] += [name]
			else :
				try :				self.reg_vf_names[reg1].index(name)
				except ValueError :	self.reg_vf_names[reg1] += [name]

		def reset_mapping( self ) :
			self.reg_mapstate = dict( self.reg_mapping )

		def swap_mapping( self ) :
			names = self.reg_mapstate.keys()
			for n in names :
				reg0, reg1 = self.reg_mapstate[n]
				self.reg_mapstate[n] = (reg1,reg0)

		def find_reg_mapping( self, name, backreg=False ) :
			try :
				if backreg :		return self.reg_mapstate[name][1]
				else :		return self.reg_mapstate[name][0]
			except KeyError :
				return None


	def _sort( l ) :
		l.sort()
		return l


	def _sortedKeys( d ) :
		keys = d.keys()
		keys.sort()
		return keys


	def _process_line( line, lineno, ctxt ) :
		tokens  = token.tokenize( line )
		if len(tokens) == 0 :
			return [line]

		def _findseq( seqline ) :
			seqtoks = token.tokenize( seqline )
			if len(seqtoks) > len(tokens) : return -1
			try : pos = tokens.index(seqtoks[0])
			except ValueError : return -1
			if seqtoks == tokens[pos:pos+len(seqtoks)] : return pos
			else : return -1

		# .name vucodename ?

		seqi = _findseq('.name')
		if seqi >= 0 :
			ctxt.name = tokens[seqi+2]
			return [	'\t.vu\n',
					'\t.global %s_CodeStart\n' % ctxt.name,
					'\t.global %s_CodeEnd\n' % ctxt.name,
					'\t.align 4\n',
					'%s_CodeStart:\n' % ctxt.name ]

		# .regname name reg0 [reg1] ?

		seqi = _findseq('.regname')
		if seqi >= 0 :
			name = tokens[seqi+2]
			reg0 = tokens[seqi+3]
			reg1 = reg0
			if len(tokens) > seqi+4 :
				reg1 = tokens[seqi+4]
			ctxt.add_regname( name, reg0, reg1 )
			return []

		# .regswap ?

		seqi = _findseq('.regswap')
		if seqi >= 0 :
			ctxt.swap_mapping()
			return []

		# .regreset ?

		seqi = _findseq('.regreset')
		if seqi >= 0 :
			ctxt.reset_mapping()
			return []

		# substitute regname with reg

		linemep = token.mepize( line, tokens )
		toki = 0
		last_is_reg = False
		pp_line = linemep[0]
		while toki < len(tokens) :
			tok = tokens[toki]
			if tok == '[' and last_is_reg :
				# (previous reg)[xyzw]
				while True :
					toki += 1
					tok = tokens[toki]
					if tok == ']' : break
					pp_line += tok
			else :
				# vxxx[@] ?
				backmap = (toki+1 < len(tokens)) and (tokens[toki+1] == '@')
				reg = ctxt.find_reg_mapping( tok, backmap )
				if reg :
					pp_line += reg
					last_is_reg = True
					if backmap : toki += 1
				else :
					pp_line += tok
					last_is_reg = ctxt.is_reg( tok )
			pp_line += linemep[toki+1]
			toki += 1
		return [ pp_line ]

	# process source
	vsm_lines = []
	vsm_lines += [ '#\n# AtOnce Technologies - VPU preprocessor (c)2002-2007 Gerald Gainant\n#\n\n' ]

	ctxt = Context()
	for i in range(len(vu_lines)) :
		vsm_lines += _process_line( vu_lines[i], i, ctxt )

	# end of .vu section ?
	if ctxt.name :
		vsm_lines += [ '\t.align 4\n', '%s_CodeEnd:\n\n' % ctxt.name ]

	# output mapping informations
	vsm_lines += '## Mapping informations :\n'
	vsm_lines += '#\n## By name :\n'
	nameKeys = _sortedKeys( ctxt.reg_mapping )
	nameLen  = 0
	for k in nameKeys :
		if len(k) > nameLen : nameLen = len(k)
	for k in nameKeys :
		namePad = ' '*(nameLen-len(k))
		reg0 = ctxt.reg_mapping[k][0]
		reg1 = ctxt.reg_mapping[k][1]
		if reg0 == reg1 :		vsm_lines += '# %s : %s\n' % (k+namePad, reg0)
		else :			vsm_lines += '# %s : %s %s@\n' % (k+namePad, reg0, reg1)
	vsm_lines += '#\n## By register :\n'
	for k in _sortedKeys(ctxt.reg_vi_names) :
		vsm_lines += '# %s [#%2d] : ' % (k, len(ctxt.reg_vi_names[k]))
		for n in _sort(ctxt.reg_vi_names[k]) :
			vsm_lines += '%s ' % n
		vsm_lines += '\n'
	vsm_lines += '\n'
	for k in _sortedKeys(ctxt.reg_vf_names) :
		vsm_lines += '# %s [#%2d] : ' % (k, len(ctxt.reg_vf_names[k]))
		for n in _sort(ctxt.reg_vf_names[k]) :
			vsm_lines += '%s ' % n
		vsm_lines += '\n'
	vsm_lines += '\n\n\n'

	return vsm_lines




def ProcessVUFile( target, source ) :

	# source line
	try:
		vu_lines = open( source, 'r' ).readlines()
	except IOError as msg:
		return ( True, 1, None, "Can't open source file : %s !\n" % source )

	# redirect to vcl ?
	to_vcl = False
	for l in vu_lines :
		if l.find('.vcl') > 0 :
			to_vcl = True
			break

	if to_vcl :
		# .vu => .vcl => .vsm
		vcl_lines, vcl_opts = generate_vcl( vu_lines )
		vcl = os.path.splitext(source)[0]+'.vcl'

		try :
			open(vcl, 'w').writelines( vcl_lines )
		except IOError as msg:
			return ( True, 1, None, "Can't write vcl tmp file : %s !\n" % vcl )

		# call vcl
		cl = r'vcl '
		cl += ' -L ' if 'optim_loop' not in vcl_opts else ''
		cl += ' -S ' if 'optim_loop_rw' in vcl_opts else ''
		cl += ' -C ' if 'reduce_code' not in vcl_opts else ''
		cl += ' -t1 ' if 'fast_timeout' in vcl_opts else ''
		cl += '{-o%s} ' % target
		cl += ' {%s} ' % vcl
		return spawn.Spawn( cl )

	else :
		# .vu => .vsm
		vsm_lines = generate_vsm( vu_lines )

		try:
			open(target,'w').writelines( vsm_lines )
		except IOError as msg:
			return ( True, 1, None, "Can't write vsm target file : %s !\n" % target )

		return ( True, 0, None, None )



