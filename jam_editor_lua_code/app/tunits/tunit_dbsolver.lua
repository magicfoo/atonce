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


local b1
local v1


b1 = datablock.block( 'dbsolver' )
assert( b1 )



b1.query:setvalue( 'TUNITS.2.NAME' )
v1 = b1.result:getvalue()
assert( v1.name[1] == 'tortue' )



b1.query:setvalue( 'TUNITS.1.idx' )
v1 = b1.result:getvalue()
assert( v1.idx[1] == 58 )



b1.query:setvalue( 'SELECT * from TUNITS' )
v1 = b1.result:getvalue()
assert( counttable(v1)==3 )
assert( istable(v1.ID) )
assert( istable(v1.name) )
assert( istable(v1.idx) )
assert( v1.name[1] == 'lapin' )
assert( v1.name[2] == 'tortue' )
assert( v1.name[3] == 'vache' )
assert( v1.idx[1] == 58 )
assert( v1.idx[2] == 135 )
assert( v1.idx[3] == 8 )


