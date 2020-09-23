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




-- su.lang.getKeys
-- { [k1]=v1, [k2]=v2, ... } => { [1]=k1, [2]=k2, ... }
t = su.lang.getKeys {4, 5, 6}
assert( #t == 3 )
assert( t[1] == 1 )
assert( t[2] == 2 )
assert( t[3] == 3 )

t = su.lang.getKeys {a=45, [32]="bonjour"}
assert( #t == 2 )
assert( t[1] == 'a' )
assert( t[2] == 32 )



-- su.lang.getValues
-- { [k1]=v1, [k2]=v2, ... } => { [1]=v1, [2]=v2, ... }
t = su.lang.getValues {a=45, [32]="bonjour"}
--printTable( t )
assert( #t == 2 )
assert( t[1] == 45 )
assert( t[2] == "bonjour" )



-- su.lang.mapValues
-- { [k1]=v1, [k2]=v2, ... } => { [v1]=1, [v2]=1, ... }
t = su.lang.mapValues {a=45, [32]="bonjour"}
--printTable( t )
assert( t[45] )
assert( t.bonjour )



-- su.io.listMount
m = su.io.listMount()
--printTable( m )
assert( m['/app'] )
assert( m['/project'] )



-- su.io.listDirectory
t = su.io.listDirectory( '/project' )
--printTable( projfiles )
v = su.lang.mapValues( t )
assert( v['jsettings'] )
assert( v['.svn'] )
assert( v['tunits_ipsum.txt'] )



-- su.util.listMaps
if su.io.isFile('/project/tunit_sua.pak') then
	maps = su.util.listMaps( '/project/tunit_sua.pak' )
	assert( #maps > 0 )
	assert( maps[1].rid )
	assert( maps[1].tid )
	assert( maps[1].contentbs )
end



-- su.util.listTags
for _,m in iter(maps) do
	tags = su.util.listTags( '/project/tunit_sua.pak', m.rid )
	for _,t in pairs(tags) do
		if t.bsize < 1024*4 then tbs = t.bsize .. 'o'
		elseif t.bsize < 1024*1024 then tbs = tointeger(t.bsize/1024) .. 'Ko'
		else tbs = tointeger(t.bsize/(1024*1024)) .. 'Mo' end
		-- print( t.tagid, tbs )
	end
end



