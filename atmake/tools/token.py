
import shlex, io



def tokenize( line ) :
	""" Returns a list of token from a string line
	"""
	parser = shlex.shlex( io.StringIO(line) )
	tokens = []
	while True :
		tok = parser.get_token()
		if not tok: break
		tokens += [str(tok)]
	return tokens


def mepize( line, tokens ) :
	""" Returns "mise en page" informations. This is a diff function
		between the token set and the original string line.
	"""

	if len(tokens) == 0 :
		return line

	l = line
	linemep = []
	mep = ''
	toki = 0

	while True :
		if len(l) == 0 :
			# end of line
			linemep += [mep]
			break
		if toki == len(tokens) :
			# no more token
			linemep += [l]
			break
		tok = tokens[toki]
		if l[:len(tok)] == tok :
			# starts with token -> end of mep
			linemep += [mep]
			mep = ''
			toki += 1
			l = l[len(tok):]
		else :
			# append to current mep
			mep += l[0]
			l = l[1:]

	return linemep		

	
