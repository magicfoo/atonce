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

dbman_h = app.root:findChildRec( 'db_man' )
assert( dbman_h )
dbman = dbman_h.node
assert( dbman )



-- get the list of dbobserver blocks

dbo = dbman.dbobserver_blocks
assert( dbo )
assert( counttable(dbo)==0 )



-- check the weak table

b1 = datablock.block( 'dbobserver' )
b2 = datablock.block( 'dbobserver' )
assert( b1 )
assert( b2 )
assert( counttable(dbo)==2 )
assert( dbo[b1] )
assert( dbo[b2] )

b2 = nil
collectgarbage()
assert( counttable(dbo)==1 )
assert( dbo[b1] ~= nil )
assert( dbo[b2] == nil )



-- check the notification (manually, with a direct call to the dbman.notify_dbobserver())

assert( isempty(b1.updated:getvalue()) )

-- single
b1.observed:setvalue( {'T1'} )
v1 = b1.updated:getvalue()
assert( isempty(v1) )

dbman.notify_dbobserver( {'T1'} )

v1 = b1.updated:getvalue()
assert( #v1==1 )
assert( v1[1]=='T1' )

v1 = b1.updated:getvalue()
assert( #v1==1 )
assert( v1[1]=='T1' )

-- multiple
b1.observed:setvalue( {'T1','T2'} )
dbman.notify_dbobserver( {'T3'} )
v1 = b1.updated:getvalue()
assert( isempty(v1) )

dbman.notify_dbobserver( {'T2'} )
v1 = b1.updated:getvalue()
assert( #v1==1 )
assert( v1[1]=='T2' )

dbman.notify_dbobserver( {'T1','T2'} )
v1 = b1.updated:getvalue()
assert( #v1==2 )
assert( v1[1]=='T1' )
assert( v1[2]=='T2' )





