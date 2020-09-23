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
require 'stream'

nodeUri				= 'jam.dam'

SOURCE				= "SOURCE_FILE"
PAK					= "PAK"
PAK_ENTRY 			= "PAK_ENTRY"
TUNITS				= "TUNITS"
MTC					= "MTC"
MTO					= "MTO"

mto = {}
uriInspector = nil

inspector = 'jam.asset-inspector'

function h_nil ( innode )
	--
end


-- Test Table

function createTestTable( )
	local createLine = 'CREATE TABLE "'..TUNITS..'" ("ID" INTEGER PRIMARY KEY NOT NULL UNIQUE, "name" CHAR NOT NULL, "idx" INTEGER )'
	node.handle.db.executeSQL( createLine )
	createLine = 'INSERT INTO "'..TUNITS..'" ("ID", "name", "idx" ) VALUES (1,"lapin",58)'
	node.handle.db.executeSQL( createLine )
	createLine = 'INSERT INTO "'..TUNITS..'" ("ID", "name", "idx" ) VALUES (2,"tortue",135)'
	node.handle.db.executeSQL( createLine )
	createLine = 'INSERT INTO "'..TUNITS..'" ("ID", "name", "idx" ) VALUES (3,"vache",8)'
	node.handle.db.executeSQL( createLine )
end

-- Selection


function selectAllSourceFile( )
	local createLine = 'SELECT * FROM "'..SOURCE..'" WHERE "used" = 1'
	local table = node.handle.db.executeSQL( createLine )
	return table
end

function selectAllPak( )
	return node.handle.db.selectAll( PAK )
end

function selectPak( inID )
	local createLine = 'SELECT "path" FROM "'..PAK..'" WHERE "ID"='..inID
	local table = node.handle.db.executeSQL( createLine )
	return table.path[1]
end

function selectSourceFile( inID )
	local createLine = 'SELECT "path" FROM "'..SOURCE..'" WHERE "ID"='..inID..' AND "used" = 1'
	local table = node.handle.db.executeSQL( createLine )	
	return table.path[1]
end

function selectSourceFileId( )
	local createLine = 'SELECT "ID", "used" FROM "'..SOURCE..'" WHERE "used">1'
	local table = node.handle.db.executeSQL( createLine )
	local q = 'UPDATE "'..SOURCE..'" SET "used"=1 WHERE "used">1'
	node.handle.db.executeSQL( q )
	return table
end

function selectAllMto( )
	return node.handle.db.selectAll( MTO )
end

function selectMto( inID )
	local createLine = 'SELECT * FROM "'..MTO..'" WHERE "ID"='..inID..' AND "used" = 1'
	local table = node.handle.db.executeSQL( createLine )
	return table
end

function selectMtoAttributes( inID )
	local createLine = 'SELECT "attributes" FROM "'..MTO..'" WHERE "ID"='..inID
	local table = node.handle.db.executeSQL( createLine )
	return table.attributes[1]
end

function selectMtc( inID )
	local createLine = 'SELECT "name" FROM "'..MTC..'" WHERE "ID"='..inID
	local table = node.handle.db.executeSQL( createLine )	
	return table.name[1]
end

function selectAllMtc( )
	return node.handle.db.selectAll( MTC )
end

function selectMtcByName( name )
	local createLine = 'SELECT "ID" FROM "'..MTC..'" WHERE "name"="'..name..'"'
	local table = node.handle.db.executeSQL( createLine )
	return table.ID[1]
end

function selectPakEntries( inIdPak )
	local createLine = 'SELECT * FROM "'..PAK_ENTRY..'" WHERE "pakID"="'..inIdPak..'"'
	local table = node.handle.db.executeSQL( createLine )
	return table
end

-- Check

function checkSourceFile( )
	local createLine = 'CREATE TABLE "'..SOURCE..'" ("ID" INTEGER PRIMARY KEY  NOT NULL  UNIQUE , "path" CHAR NOT NULL, "used" INTEGER NOT NULL  DEFAULT 1)'
	node.handle.db.createTable( SOURCE, createLine )
end

function checkPak( )
	local createLine = 'CREATE TABLE "'..PAK..'" ("ID" INTEGER PRIMARY KEY  NOT NULL  UNIQUE , "path" CHAR NOT NULL, "dirty" INTEGER NOT NULL DEFAULT 1, "used" INTEGER NOT NULL  DEFAULT 1)'
	node.handle.db.createTable( PAK, createLine )
	local createEntries = 'CREATE TABLE "'..PAK_ENTRY..'" ("ID" INTEGER PRIMARY KEY  NOT NULL  UNIQUE , "pakID" INTEGER, "resourceID" INTEGER, "used" INTEGER NOT NULL  DEFAULT 1)'
	node.handle.db.createTable( PAK_ENTRY, createEntries )
end

function checkMtc( )
	local createLine = 'CREATE TABLE "'..MTC..'" ("ID" INTEGER PRIMARY KEY NOT NULL UNIQUE, "name" CHAR NOT NULL )'
	node.handle.db.createTable( MTC, createLine )
end

function checkMto( )
	local createLine = 'CREATE TABLE "'..MTO..'" ("ID" INTEGER PRIMARY KEY NOT NULL UNIQUE, "attributes" CHAR NOT NULL, "mtc" CHAR NOT NULL, "source-content-md5" CHAR NOT NULL, "dirty" INTEGER NOT NULL DEFAULT 1, "used" INTEGER NOT NULL  DEFAULT 1)'
	node.handle.db.createTable( MTO, createLine )
end

-- Drop table

function dropSourceFile( )
	return node.handle.db.dropTable( SOURCE )
end

function dropMtc( )
	return node.handle.db.dropTable( MTC )
end

function dropPak( )
	return node.handle.db.dropTable( PAK )
end

function dropPakEntry( )
	return node.handle.db.dropTable( PAK_ENTRY )
end

function dropMto( )
	return node.handle.db.dropTable( MTO )
end

-- Empty table

function cleanSourceFile( )
	return node.handle.db.cleanTable( SOURCE )
end

function cleanPak( )
	return node.handle.db.cleanTable( PAK )
end

function cleanPakEntry( )
	return node.handle.db.cleanTable( PAK_ENTRY )
end

function cleanMtc( )
	return node.handle.db.cleanTable( MTC )
end

function cleanMto( )
	return node.handle.db.cleanTable( MTO )
end

-- Add

function addPak( inPath )
	local ID
	local usedTab = node.handle.db.executeSQL( 'SELECT "ID" FROM "'..PAK..'" WHERE "used"= 0 ' )
	if usedTab.ID then
		ID = usedTab.ID[1]
		local q = 'UPDATE "'..PAK..'" SET "path"="'..inPath..'", "used"="1" WHERE "ID"="'..ID..'"'
		node.handle.db.executeSQL( q )
	else
		local checkQuery = 'SELECT ID FROM "'..PAK..'" WHERE path="'..inPath..'" ORDER BY "ID" DESC'
		local addQuery = 'INSERT INTO "'..PAK..'" ("path") VALUES("'..inPath..'")'
		node.handle.db.executeSQL( addQuery )
		local table = node.handle.db.executeSQL( checkQuery )
		ID = table.ID[1]
		end
	return ID
end

function addPakEntry( pakID, resourceID )
	local ID
	local usedTab = node.handle.db.executeSQL( 'SELECT "ID" FROM "'..PAK_ENTRY..'" WHERE "used"= 0 ' )
	if usedTab.ID then
		ID = usedTab.ID[1]
		local q = 'UPDATE "'..PAK_ENTRY..'" SET "pakID"="'..pakID..'", "used"="1", "resourceID"="'..resourceID..'" WHERE "ID"="'..ID..'"'
		node.handle.db.executeSQL( q )
	else
		local checkQuery = 'SELECT ID FROM "'..PAK_ENTRY..'" WHERE pakID="' ..pakID..'" AND  resourceID="'..resourceID..'"'
		local addQuery = 'INSERT INTO "'..PAK_ENTRY..'" ("pakID","resourceID") VALUES("'..pakID..'","'..resourceID..'")'
		node.handle.db.executeSQL( addQuery )
		local table = node.handle.db.executeSQL( checkQuery )
		ID = table.ID[1]
		end
	return ID
end

function addMtoDb( attributes, mtc )
	local ID
	local usedTab = node.handle.db.executeSQL( 'SELECT "ID" FROM "'..MTO..'" WHERE "used"= 0 ' )
	if usedTab.ID then
		ID = usedTab.ID[1]
		local q = 'UPDATE "'..MTO..'" SET "attributes"="'..attributes..'", "mtc"="'..mtc..'" , "used"="1" WHERE "ID"="'..ID..'"'
		node.handle.db.executeSQL( q )
	else
		local checkQuery = 'SELECT "ID" FROM "'..MTO..'"WHERE "attributes"="'..attributes..'" AND "mtc"="'..mtc..'" ORDER BY "ID" DESC'
		local addQuery = 'INSERT INTO "'..MTO..'" ("attributes","mtc") VALUES ("'..attributes..'","'..mtc..'")'
		node.handle.db.executeSQL( addQuery )
		local table = node.handle.db.executeSQL( checkQuery )
		ID = table.ID[1]
	end
	return ID
end

function addSourceFile( inPath, intempId )
	local ID
	local usedTab = node.handle.db.executeSQL( 'SELECT "ID" FROM "'..SOURCE..'" WHERE "used"= 0 ' )
	if usedTab.ID then
		ID = usedTab.ID[1]
		local q = 'UPDATE "'..SOURCE..'" SET "path"="'..inPath..'", "used"="'..intempId..'" WHERE "ID"="'..ID..'"'
		node.handle.db.executeSQL( q )
	else
		local checkQuery = 'SELECT "ID" FROM "'..SOURCE..'" WHERE "path"="'..inPath..'" ORDER BY "ID" DESC'
		local addQuery = 'INSERT INTO "'..SOURCE..'" ("path","used") VALUES("'..inPath..'","'..intempId..'")'
		node.handle.db.executeSQL( addQuery )
		local table = node.handle.db.executeSQL( checkQuery )
		ID = table.ID[1]
	end
end

function addMtc( inName )
	local checkQuery = 'SELECT ID FROM "'..MTC..'" WHERE name="'..inName..'"'
	local addQuery = 'INSERT INTO "'..MTC..'" ("name") VALUES("'..inName..'")'
	return node.handle.db.addInTable( checkQuery, addQuery )
end

-- Remove

function removeSourceFile( id )
	local query = 'UPDATE "'..SOURCE..'" SET "used"= 0 WHERE "ID"="'..id..'"'
	node.handle.db.executeSQL( query )
	return true
end

function removeMtoDb( id )
	local query = 'UPDATE "'..MTO..'" SET "used"= 0 WHERE "ID"="'..id..'"'
	node.handle.db.executeSQL( query )
	return true
end

function removePak( id )
	local query = 'UPDATE "'..PAK..'"SET "used"= 0 WHERE "ID"="'..id..'"'
	node.handle.db.executeSQL( query )
	return true
end

-- Count

function countSourceFile( )
	return node.handle.db.countAll( SOURCE )
end

function countPak( )
	return node.handle.db.countAll( PAK )
end

function countPakEntry( )
	return node.handle.db.countAll( PAK_ENTRY )
end

function countMtc( )
	return node.handle.db.countAll( MTC )
end

function countMto( )
	return node.handle.db.countAll( MTO )
end


-- Update

function updatePak( inPath, inId )
	local updateQuery = 'UPDATE "'..PAK..'" SET "path"="'..inPath..'" WHERE "ID"='..inId
	node.handle.db.executeSQL( updateQuery )
	return true
end

function updateMto( inAttributes, inId )
	local updateQuery = 'UPDATE "'..MTO..'" SET "attributes"="'..inAttributes..'" WHERE "ID"='..inId
	node.handle.db.executeSQL( updateQuery )
	return true
end

-- node functions

function addMto( inMtc, inId ) 
	local ID
	local ud
	local temp
	local ud_h
	if inMtc == 'User-Data' then
		ud_h = node.handle.root:findChildRec( 'mtc_userdata' )
	end
	ud = ud_h.node
	temp = ud.mtc_realize()
	if inId == -1 then
		ID = addMtoDb( ' ', inMtc )
		local attr = temp:getattributes( )
		local attrs_s = stream.presentLua( attr )
		updateMto( attrs_s, ID )
	else
		ID = inId
		local attrs = selectMtoAttributes( ID )
		local attrs_t = stream.parseLua( attrs )
		temp:setattributes( attrs_t )
	end
	mto[ID] = temp

	return ID
end

function setValueMto( inId, inValue )
	node.handle.facade:postMessage( {subject='private-set-value', target=inspector,content={inValue} } )
	mto[inId].db.query:setvalue( inValue )
	local attrs = mto[inId]:getattributes( )
	local attrs_s = stream.presentLua( attrs )
	updateMto( attrs_s, inId )
end

function getValueMto( inId )
	local attr = mto[inId].db.query:getvalue(  )
	return attr
end

function inspectMto( inId )
	local tableBlocks = mto[inId]:getblocks() 	
	node.handle.facade:postMessage( {subject='build-network', target='*.*', content={tableBlocks} } )
	local obj = new 'MFlUserDataInspector'
	uriInspector = node.handle.facade:addObserver( obj, inspector )
	return uriInspector
end

function removeMto( inId )
	if inId then
		--removeMtoDb( m.content[1] )
	end
end

function buildPak( inId )
	local vnet
	local in_pakfile = selectPak( inId )
	local pak = su.util.openPak( in_pakfile , true )
	assert( pak )

	pak:startUpdating()

	if inId then
		local table = selectPakEntries( inId )
		for k,v in pairs(table['resourceID']) do
			if mto[v] then
				vnet = mto[v]:generate()
				mapid, mapname = su.util.selectMap(vnet)
				local map, ctxt = su.util.binarizeMap( vnet )
				if map==nil then
					return
				end
				pak:updateContent( v, mapid, map )
			end	
		end
	end

	pak:endUpdating()
end

function onInitialize( inh )
	inh.moCreate = h_nil
	inh.moDelete = h_nil
	inh.moGenerate = h_nil
	inh.moDepend = h_nil
	inh.moInspect = h_nil
	inh.moEdit = h_nil
	inh.moLabel = h_nil


	-- Pak function
	inh.db.buildPak = buildPak

	-- Mto access
	inh.db.addMto = addMto
	inh.db.inspectMto = inspectMto
	inh.db.setValueMto = setValueMto
	inh.db.getValueMto = getValueMto
	inh.db.removeMto = removeMto
	inh.db.checkMto = checkMto
	inh.db.updateMto = updateMto

	-- selection
	inh.db.selectAllSourceFile = selectAllSourceFile
	inh.db.selectAllPak = selectAllPak
	inh.db.selectPak = selectPak
	inh.db.selectSourceFile = selectSourceFile
	inh.db.selectSourceFileId = selectSourceFileId
	inh.db.selectMtc = selectMtc
	inh.db.selectAllMtc = selectAllMtc
	inh.db.selectMtcByName = selectMtcByName

	-- check
	inh.db.checkSourceFile = checkSourceFile
	inh.db.checkPak = checkPak
	inh.db.checkMtc = checkMtc	

	-- drop
	inh.db.dropSourceFile = dropSourceFile
	inh.db.dropPak = dropPak
	inh.db.dropPakEntry = dropPakEntry
	inh.db.dropMtc = dropMtc

	-- clean
	inh.db.cleanSourceFile = cleanSourceFile
	inh.db.cleanPak = cleanPak
	inh.db.cleanPakEntry = cleanPakEntry
	inh.db.cleanMtc = cleanMtc

	-- add
	inh.db.addPak = addPak
	inh.db.addPakEntry = addPakEntry
	inh.db.addSourceFile = addSourceFile
	inh.db.addMtc = addMtc

	-- remove
	inh.db.removeSourceFile = removeSourceFile
	inh.db.removePak = removePak

	-- count 
	inh.db.countSourceFile = countSourceFile
	inh.db.countPak = countPak
	inh.db.countPakEntry = countPakEntry
	inh.db.countMtc = countMtc

	-- Update
	inh.db.updatePak = updatePak

end

local function collectMtc ( nh )
	if isfunction(nh.node.mtcGetName) then
		addMtc( nh.node.mtcGetName() )
	end
	for _,c in pairs( nh.children ) do
		collectMtc( c )
	end
end

function onCreate ( inh )
	inh.facade:addObserver( onMessage, nodeUri )
	createTestTable( )
	checkMtc( )
	cleanMtc( )
	collectMtc( node.handle )
end

function onMessage ( fa, uri, m )
	if m.subject=='generate-pak-from-sua' then
		node.handle.children.mtc_collection.generatePakFromSUA( m.content[1] , m.content[2] )
	end
end

