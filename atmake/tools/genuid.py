

import os, sys, binascii



def genuid( s ) :
	t = s.upper().replace(r'\\','/')
	u = binascii.crc32(t.encode())
	return (u&0xffffffff)


def genuid_hex( s ) :
	return '0x%08xu' % genuid(s)


