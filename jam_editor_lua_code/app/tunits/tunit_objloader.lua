--LIC-HDR--------------------------------------------------------------------
--
-- Copyright (C) 2005-2011 AtOnce Technologies
--
-- This file is part of the Sugar core-system framework.
--
-- This file and the associated product may be used and distributed
-- under the terms of a current License as defined by AtOnce Technologies
-- and appearing in the file LICENSE.TXT included in the packaging of this file.
--
-- This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
-- WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
--
-- See http://www.atonce-technologies.com or email info@atonce-technologies.com
-- for informations about Sugar Framework and AtOnce Technologies others products
-- License Agreements.
--
-- Contact info@atonce-technologies.com if any conditions of this license
-- are not clear to you.
--
--------------------------------------------------------------------LIC-HDR--

lw = new 'MLuaWindows'

atbase_path = lw:getEnv( "ATBASE" )
virtual_atbase_path = su.io.getVirtualPath( atbase_path )
sufw_data_path = virtual_atbase_path .. '/sufw-data/'

assert( su.io.isDirectory(sufw_data_path) )

objfile = sufw_data_path .. 'samples/sample/sample_cube.obj'
suafile = sufw_data_path .. 'samples/sample/bcube.sua'
suafile2 = sufw_data_path .. 'samples/s01_cube/sample_cube.sua'


print( objfile )
assert( su.io.isFile(objfile) )

print 'lancement ObjLoader'
objloader = new 'MObjDocument'

objcontent = objloader:load( objfile )

--printTable( objcontent )
su.io.dumpStream( suafile, objcontent )

