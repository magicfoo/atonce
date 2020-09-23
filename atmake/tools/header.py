
import os, sys, re


def readLines( fn ):
    try:
        f = open( fn )
    except IOError as msg:
        sys.stderr.write("Can't open file : %s\n" % fn )
        return None
    lines = f.readlines();
    f.close()
    return lines

def writeLines( fn, lines ):
    try:
        f = open( fn, 'w' )
    except IOError as msg:
        sys.stderr.write("Can't write file : %s\n" % fn )
        return
    f.writelines( lines );
    f.close()

def locateLine( lines, pattern ):
    lno = 0
    for l in lines:
        if pattern.match(l) :
            return lno
        lno += 1;
    return -1

def processFile( fn, headerLines ):
    lines = readLines( fn )
    if lines == None :
        return 1
    #sys.stderr.write("Processing [ %s ]\n" % fn )

    pat0 = sre.compile( '^/\*LIC-HDR[\*]*[.]*' )
    pat1 = sre.compile( '^[\*]*LIC-HDR\*/[.]*' )

    # search header start / end
    header0 = locateLine( lines, pat0 )
    header1 = locateLine( lines, pat1 )
    if header0 < header1 and header0 >= 0 :
        lines[header0:header1+1] = headerLines
    else :
        sys.stderr.write("Header missing in [ %s ]\n" % fn )

    writeLines( fn, lines )
    return 1

def processDir( dirname, ext, headerLines ):
    try:
        names = os.listdir( dirname )
    except os.error as msg:
        sys.stderr.write( "Can't list directory: %s\n" % dirname )
        return 1
    files = []
    for name in names :
        fn = os.path.join( dirname, name )
        if os.path.normcase(fn).endswith(ext) or os.path.isdir(fn):
            files.append(fn)
    return processFiles( files, ext, headerLines )

def processFiles( pathList, ext, headerLines ):
    res = None
    for f in pathList:
        if os.path.isdir(f):
            ret = processDir( f, ext, headerLines )
        else :
            ret = processFile( f, headerLines )
        res = res or ret
    return res


