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


require 'datablock'



-- get the db-manager handle/node

ud_h = app.root:findChildRec( 'mtc_userdata' )
assert( ud_h )
ud = ud_h.node
assert( ud )


-- test to convert a text file

ipsum_txt = '/project/tunits_ipsum.txt'

b1 = datablock.block( 'userdata' )
b1.files:setvalue( ipsum_txt )
v1 = b1.map:getvalue()
--printTable( v1 )
assert( #v1 == 1 )
assert( v1[1].tagid )
assert( v1[1].tagid == 'rsc.userdata.content' )
assert( v1[1].sourcefile.text == ipsum_txt )
assert( v1[1].data.size == su.io.getSize(ipsum_txt) )
assert( isbinary(v1[1].data.bytes) )


-- test to binarize

mapid, mapname = su.util.selectMap(v1)
assert( mapid and mapid>0 )
assert( mapname == 'rsc.userdata' )

map, ctxt = su.util.binarizeMap(v1)
if not map then
	printTable( ctxt, "Binarize error" )
	assert( map )
end



-- test to realize

net = ud.mtc_realize()
net.db.query:setvalue( ipsum_txt )
vnet = net:generate()
assert( vnet )
mapid, mapname = su.util.selectMap(vnet)
assert( mapname == 'rsc.userdata' )



