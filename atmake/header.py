


import sys, os
import atmake
import atmake.tools.header


def processFiles( template, searchpath, extlist=['.h','.c','.cpp','.cxx','.vupp','.vsmpp','.vcl','.vsa','.psa'] ) :
	"""	Update file's header recursively.
		<template>    The template header lines
		<searchpath>  The root search path
		<extlist>     List of file extensions to match
	"""
	# read header file
	header_lines = open(template,'r').readlines()
	if header_lines == None :
	    atmake.Error( "Template header file <%s> is not found !\n" % template )
	for ext in extList :
		atmake.tools.header.processFiles( searchpath, ext, header_lines )


