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

-- print( 'tunit-hook-database' )


-- Create an Open DB
db = new 'MAssetDatabase'
dbpath = '/project/tunits_hook.sqlite'
if not db:opendb( dbpath ) then
	assert( db:createdb( dbpath ) )
	assert( db:opendb( dbpath ) )
end

-- Creation of the first table
local createLine = 'CREATE TABLE "TABLE_HOOK" ("ID" INTEGER PRIMARY KEY NOT NULL UNIQUE, "name" CHAR NOT NULL, "idx" INTEGER )'
db:execute( createLine )

-- Insert new lines in the table test
createLine = 'INSERT INTO "TABLE_HOOK" ("ID", "name", "idx" ) VALUES (1,"lapin",58)'
db:execute( createLine )
createLine = 'INSERT INTO "TABLE_HOOK" ("ID", "name", "idx" ) VALUES (2,"tortue",135)'
db:execute( createLine )
createLine = 'INSERT INTO "TABLE_HOOK" ("ID", "name", "idx" ) VALUES (3,"vache",8)'
db:execute( createLine )

-- Recover modification of hook
local hookt = db:collectHookTable( )
assert( hookt )


-- Recover second time to check there is nothing
hookt = db:collectHookTable( )
assert( hookt )


-- Creation of the first table
local createLine = 'CREATE TABLE "TABLE_HOOK2" ("ID" INTEGER PRIMARY KEY NOT NULL UNIQUE, "name" CHAR NOT NULL, "idx" INTEGER )'
db:execute( createLine )

-- Insert new lines in the table test
createLine = 'INSERT INTO "TABLE_HOOK2" ("ID", "name", "idx" ) VALUES (1,"lapin",58)'
db:execute( createLine )


-- Recover second time to check there is nothing
hookt = db:collectHookTable( )
assert( hookt )	

-- Insert new lines in the table test
createLine = 'INSERT INTO "TABLE_HOOK" ("ID", "name", "idx" ) VALUES (4,"lapint",58)'
db:execute( createLine )

-- Insert new lines in the table test
createLine = 'INSERT INTO "TABLE_HOOK2" ("ID", "name", "idx" ) VALUES (4,"lapint",58)'
db:execute( createLine )

-- Recover second time to check there is nothing
hookt = db:collectHookTable( )
assert( hookt )	

-- Close and delete file
assert( db:closedb( dbpath ) )
su.io.removeFile( dbpath )


