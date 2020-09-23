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

require 'bitmask'
require 'datablock'
require 'stream'

TREE_ENTRY			= "TREE_ENTRY"
VOLUMES				= "VOLUMES"

views = { }

nodeWindowType = { 'browser' }
nodeUri = 'jam.asset-browser'
Uname = 'Uname'
btmCategory = nil
aCategory = {}
inspector = nil

function selectAllTreeEntry( )
	local table = node.handle.db.selectAll( TREE_ENTRY )
	table.filepath={}
	table.opened={}
	table.mtc={}
	local getPath
	local tablePath
	local getOpened
	if not table.category then
		return nil
	end
	for k,v in pairs(table.category) do
		if v == 'SOURCE_FILE' or v == 'PAK' then
			if table.categoryID[k]>0 then
				getPath = 'SELECT path FROM "'..v..'" WHERE ID ='..table.categoryID[k]
				tablePath = node.handle.db.executeSQL( getPath )			
				table.filepath[k] = tablePath.path[1]
			else
				table.filepath[k] = 'virtual'
			end
		else
			table.filepath[k] = 'virtual'
		end
		if v == 'MTO' then
			if table.categoryID[k]>0 then
				getMtc = 'SELECT mtc FROM "'..v..'" WHERE ID ='..table.categoryID[k]
				tableMtc = node.handle.db.executeSQL( getMtc )
				table.mtc[k] = tableMtc.mtc[1]
			else
				table.mtc[k] = 'NONE'
			end
		else
			table.mtc[k] = 'NONE'
		end
	end
	return table
end

function selectMTOId( inName )
	local query = 'SELECT "categoryID" FROM"'..TREE_ENTRY..'" WHERE "category"="MTO" AND "name"="'..inName..'"'
	local table = node.handle.db.executeSQL( query )
	return table
end

function selectPakId( inName )
	local query = 'SELECT "categoryID" FROM"'..TREE_ENTRY..'" WHERE "category"="PAK" AND "name"="'..inName..'"'
	local table = node.handle.db.executeSQL( query )
	return table
end

function selectAllVolumes( )
	return node.handle.db.selectAll( VOLUMES )
end

function selectVolumeId( inName )
	local selectLine = 'SELECT "ID" FROM "'..VOLUMES..'" WHERE "name"="'..inName..'"'
	local table = node.handle.db.executeSQL( selectLine )
	return table
end

function checkVolumes( )
	local createLine = 'CREATE TABLE "'..VOLUMES..'" ("ID" INTEGER PRIMARY KEY NOT NULL UNIQUE, "name" CHAR NOT NULL )'
	node.handle.db.createTable( VOLUMES, createLine )
end

function checkTreeEntry( )
	local createLine = 'CREATE TABLE "'..TREE_ENTRY..'" ("ID" INTEGER PRIMARY KEY  NOT NULL  UNIQUE , "category" CHAR, "categoryID" INTEGER , "path" CHAR NOT NULL, "name" CHAR NOT NULL, "restriction" CHAR NOT NULL, "notes" CHAR NOT NULL )'	
	node.handle.db.createTable( TREE_ENTRY, createLine )
end

function dropVolumes( )
	return node.handle.db.dropTable( VOLUMES )
end

function dropTreeEntry( )
	return node.handle.db.dropTable( TREE_ENTRY )
end

function cleanTreeEntry( )
	return node.handle.db.cleanTable( TREE_ENTRY )
end

function cleanVolumes( )
	return node.handle.db.cleanTable( VOLUMES )
end

function addTreeEntry( category, categoryID, path, name, restriction, notes )

	local checkQuery = 'SELECT * FROM "'..TREE_ENTRY..'" WHERE path="'..path..'"'
	local addQuery = 'INSERT INTO "'..TREE_ENTRY..'" ("category","categoryID","path","name","restriction","notes") VALUES("'..category..'",'..categoryID..',"'..path..'","'..name..'","'..restriction..'","'..notes..'")'
	return node.handle.db.addInTable( checkQuery, addQuery )
end

function addVolume( inName )
	local checkQuery = 'SELECT ID FROM "'..VOLUMES..'" WHERE "name"="'..inName..'"'
	local addQuery = 'INSERT INTO "'..VOLUMES..'" ("name") VALUES ("'..inName..'")'
	return node.handle.db.addInTable( checkQuery, addQuery )
end

function removeVolume( inName )
	local query = 'DELETE FROM "'..VOLUMES..'" WHERE "name"="'..inName..'"'
	node.handle.db.executeSQL( query )
	return true
end


function countTreeEntry( )
	return node.handle.db.countAll( TREE_ENTRY )
end

function backup( uriView )
	cleanTreeEntry()
	local tabSettings = {}
	views[uriView]:saveDatabase( )
	tabSettings = views[uriView]:getSettings()
	local vol = su.io.listMount()
	local table = node.handle.tableBackup()
	table.volumes={}
	for k,v in pairs( vol ) do
		table.volumes[k] = v
	end

	return tabSettings
end

function updateViews( )
	for k,v in pairs(views) do
		views[k]:updateViews( )
	end
end

local function collectVolumes ( )
	local vol = su.io.listMount()
	for k,v in pairs( vol ) do
		addVolume( k )
	end
end

local function isMount( name )
	local vol = su.io.listMount()
	if vol[name] then
		return true
	else
		return false
	end
end

function GetPaKTable( binPath )
	local maps = su.util.listMaps( binPath )
	local table = {}
	local i = 1
	local j = 1
	for _,m in iter(maps) do
		table[j] = {}
		table[j].name = m.mapid
		table[j].tags = {}
		table[j].id	  = m.rid
		tags = su.util.listTags( binPath, m.rid )
		for _,t in pairs(tags) do
			table[j].tags[i] = t.tagid
			i = i+1
		end
		j = j +1
	end

	return table
end

function ComputeDestination( )
	local bmSource = bitmask.create( aCategory )
	bmSource:set( aCategory[1] )
	local bmMTC = bitmask.create( aCategory )
	bmMTC:set( aCategory[3] )
	local bmMTO = bitmask.create( aCategory )
	bmMTO:set( aCategory[4] )
	local bmPak = bitmask.create( aCategory )
	local bmFS = bitmask.create( aCategory )
	bmFS:set( aCategory[1] )
	local table = { bmSource:numerical(), bmMTC:numerical(), bmMTO:numerical(), bmPak:numerical(), bmFS:numerical() }
	return table
end

function ComputeSource( )
	local bmSource = bitmask.create( aCategory )
	bmSource:set( aCategory[1] )
	bmSource:set( aCategory[5] )
	local bmMTC = bitmask.create( aCategory )
	local bmMTO = bitmask.create( aCategory )
	bmMTO:set( aCategory[2] )
	local bmPak = bitmask.create( aCategory )
	bmPak:set( aCategory[3] )
	local bmFS = bitmask.create( aCategory )
	local table = { bmSource:numerical(), bmMTC:numerical(), bmMTO:numerical(), bmPak:numerical(), bmFS:numerical() }
	return table
end

function createView( inWindowType, inSettings )
	if inWindowType == 'browser' then

		-- create the view
		local av = new 'MFlAssetBrowser'
		local uri = node.handle.facade:addObserver( av, nodeUri..'.%d' )
		assert( views[uri] == nil )
		views[uri] = av
		views[uri]:setSettings( inSettings )
		local rlist = node.handle:getResource( '*', 'icons.png' )
		local rlist2 = node.handle:getResource( '*', 'icons_util.png' )
		local rlist4 = node.handle:getResource( '*', 'cursor.png' )
		local rlist5 = node.handle:getResource( '*', 'icon_load.png' )
		node.handle.facade:postMessage( {subject='private-image-init', target=uri,content={rlist5.flimage,rlist4.flimage,rlist.flimage,rlist2.flimage} } )
		views[uri]:loadDatabase( )

		-- get the category from the view
		aCategory = views[uri]:getCategories( )

		-- Set destination and source for the drag
		views[uri]:setDestination( ComputeDestination( ) )
		views[uri]:setSource( ComputeSource( ) )

		return uri
	end
end

function getIdxCategory( inCategory )
	for k, v in pairs(aCategory) do
		if inCategory == v then
			return k
		end
	end
	return -1
end

function enumCategories( )
	return aCategory
end

function extractAndDo( inCommand )
	fields=su.string.split(inCommand," ")
	if fields[1] == 'umount' then
		if su.string.startsWith( fields[2], '/' ) then
			if isMount(fields[2]) then
				removeVolume( fields[2] )
			end
			for k,v in pairs(views) do
				node.handle.facade:postMessage( {subject='private-refresh-volumes', target=k,content={fields[2]} } )
			end
		end
	end
end

function getCategoryDest( inCategory )
	local idx = getIdxCategory( inCategory )
	if idx == -1 then
		return nil
	end

	local table = ComputeDestination( )
	if table then
		return table[idx]
	end

	return nil
end

function getCategorySource( inCategory )
	local idx = getIdxCategory( inCategory )
	if idx == -1 then
		return nil
	end

	local table = ComputeSource( )
	if table then
		return table[idx]
	end

	return nil
end

function cleanInspector( )
	if inspector then
		node.handle.facade:removeObserver( inspector )
		inspector = nil
		collectgarbage( )
	end
end

function onInitialize( inh )
	inh.facade:addObserver( onMessage, nodeUri )
	inh.db.enumCategories = enumCategories
	inh.db.getCategoryDest = getCategoryDest
	inh.db.getCategorySource = getCategorySource
end

function onCreate( inh )

	node.handle.db.checkSourceFile( )
	node.handle.db.checkPak( )
	node.handle.db.checkMto( )

	checkTreeEntry( )
	checkVolumes()

	collectVolumes()
end

function onMessage ( fa, uri, m )

	-- Global message

	if m.subject == 'collect-window-managers' then
		fa:replyMessage( { nodeUri, nodeWindowType } )
	end
	if m.subject == 'delete-window' then
		if m.content[1] then
			if( views[m.content[1]] ) then
				fa:removeObserver( m.content[1] )
				views[m.content[1]] = nil
				collectgarbage( )
			end
		end
	end
	if m.subject == 'create-window' then
		if m.content[2] and m.content[1] and m.content[3] then
			local uri = createView( m.content[1], m.content[3] )
			local requestID = m.content[2]
			fa:replyMessage( uri ,requestID )
		end
	end
	if m.subject == 'duplicate-window' then
		if m.content[2] and m.content[1] and m.content[3] then
			views[m.content[3][1]]:saveDatabase( )
			local uri = createView( m.content[1], m.content[3] )
			local uriViewToCopy = m.content[3][1]
			if views[uriViewToCopy] then
				local settings = views[uriViewToCopy]:getSettings( )
				views[uri]:setSettings( settings )
			end
			local requestID = m.content[2]
			fa:replyMessage( uri ,requestID )
		end
	end
	if m.subject == 'backup-window' then
		if m.content[1] then
			fa:replyMessage( backup( m.content[1] ) )
		end
	end
	if m.subject == 'console-command' then
		if m.content[1] then
			extractAndDo( m.content[1] )
		end
	end

	-- Private Message Utils

	if m.subject == 'private-begin-transaction' then
		node.handle.db.beginTransaction( )
	end
	if m.subject == 'private-commit-transaction' then
		node.handle.db.commitTransaction( )
		for k,v in pairs(views) do
			node.handle.facade:postMessage( {subject='private-refresh-source', target=k,content={} } )
		end
	end

	-- Private Message for source

	if m.subject == 'private-add-source' then
		if m.content[1] and m.content[2] then
			node.handle.db.addSourceFile( m.content[1], m.content[2] )
		end
	end
	if m.subject == 'private-get-source-path' then
		if m.content[1] then
			local result = node.handle.db.selectSourceFile( m.content[1] )
			fa:replyMessage( result )
		end
	end
	if m.subject == 'private-remove-source-file' then
		if m.content[1] then
			node.handle.db.removeSourceFile( m.content[1] )
		end
	end
	if m.subject == 'private-get-all-source-id' then
		local result = node.handle.db.selectSourceFileId( )
		fa:replyMessage( result )
	end

	-- Private Message for tree

	if m.subject == 'private-add-tree-entry' then
		if m.content[1] then
			local category = m.content[1][1]
			local categoryID = m.content[1][2]
			local path = m.content[1][3]
			local name = m.content[1][4]
			local restriction = m.content[1][5]
			local notes = m.content[1][6]
			addTreeEntry( category, categoryID, path, name, restriction, notes )
		end
	end
	if m.subject == 'private-count-tree' then
		local count = countTreeEntry( )
		fa:replyMessage( count )
	end
	if m.subject == 'private-select-tree-entries-and-option' then
		local table = selectAllTreeEntry( )
		fa:replyMessage( table )
	end
	if m.subject == 'private-clean-tree-entry' then
		fa:replyMessage( cleanTreeEntry( ) )
	end
	
	-- Private Message for Volumes

	if m.subject == 'private-select-volume-id' then
		if m.content[1] then
			local table = selectVolumeId( m.content[1] )
			if table.ID then
				fa:replyMessage( table.ID[1] )
			else
				fa:replyMessage( nil )
			end
		end
	end
	if m.subject == 'private-list-dir-fs' then
		if m.content[1] then
			local name = m.content[1]
			local table = su.io.listDirectory( name )
			local returnTab = {}
			if not table then
				return returnTab
			end
			returnTab['mount-path'] = {}
			returnTab['name'] = table
			local i = 0	
			for k,v in pairs(returnTab['name']) do
				returnTab['mount-path'][k] = name..'/'..v
				i = i+1
			end
			fa:replyMessage( returnTab )
		end
	end
	if m.subject == 'private-add-volume' then
		if m.content[1] then
			addVolume( m.content[1] )
		end
	end
	if m.subject == 'private-remove-volume' then
		if m.content[1] then
			removeVolume( m.content[1] )
		end
	end
	if m.subject == 'private-select-volumes' then
		local returnTab = selectAllVolumes( )
		fa:replyMessage( returnTab )
	end
	if m.subject == 'private-is-mount' then
		if m.content[1] then
			local b = isMount( m.content[1] )
			fa:replyMessage( b )
		end
	end

	-- Private message for MTC and MTO

	if m.subject == 'private-select-mtc-entries' then
		local table = node.handle.db.selectAllMtc( )
		fa:replyMessage( table )
	end
	if m.subject == 'private-select-mto-id' then
		if m.content[1] then
			local table = selectMTOId( m.content[1] )
			if table.categoryID then
				fa:replyMessage( table.categoryID[1] )
			else
				fa:replyMessage( nil )
			end
		end
	end
	if m.subject == 'private-add-mto' then
		if m.content[1] and m.content[2]then
			local ID = node.handle.db.addMto( m.content[1], m.content[2] )
			fa:replyMessage( ID )
		end
	end
	if m.subject == 'private-inspect-mto' then
		if m.content[1] then
			inspector = m.content[1]
			fa:replyMessage( node.handle.db.inspectMto( m.content[1] ) )
		end
	end
	if m.subject == 'private-set-value-mto' then
		if m.content[1] and inspector then
			node.handle.db.setValueMto( inspector, m.content[1] )
		end
	end
	if m.subject == 'private-get-value-mto' then
		if inspector then
			fa:replyMessage( node.handle.db.getValueMto( inspector ) )
		end
	end
	if m.subject == 'private-remove-mto' then
		if m.content[1] then
			node.handle.db.removeMto( m.content[1] )
		end
	end

	-- Private message for PAK

	if m.subject == 'private-build-pak' then
		if m.content[1] then
			node.handle.db.buildPak( m.content[1] )
		end
	end
	if m.subject == 'private-generate-pak' then
		if m.content[1] then
			fileDroped = resolvepath( m.content[1] )
			if fileDroped then
				binPath = nil
				if su.io.hasExtension(fileDroped,'sua') then
					binPath = fileDroped .. '.pak'
					fa:callMessage( {subject='generate-pak-from-sua',target='jam.dam',content={ binPath, fileDroped} } )
				elseif su.io.hasExtension(fileDroped,'pak') then
					binPath = fileDroped
				end
				if binPath then
					if su.io.isFile( binPath ) then
						fa:callMessage( {subject='private-add-generated-pak', target=m.source, content={ m.content[1] .. '.pak' } } )
					end
				end
			end
		end
	end
	if m.subject == 'private-list-pak' then
		if m.content[1] then
			local binPath = m.content[1]
			local table = GetPaKTable( binPath )
			fa:replyMessage( table )
		end
	end
	if m.subject == 'private-add-pak' then
		if m.content[1] then
			local rt = node.handle.db.addPak( m.content[1] )
			fa:replyMessage( rt )
		end
	end
	if m.subject == 'private-get-pak-path' then
		if m.content[1] then
			local result = node.handle.db.selectPak( m.content[1] )
			fa:replyMessage( result )
		end
	end
	if m.subject == 'private-remove-pak' then
		if m.content[1] then
			node.handle.db.removePak( m.content[1] )
		end
	end
	if m.subject == 'private-select-pak-id' then
		if m.content[1] then
			local table = selectPakId( m.content[1] )
			if table.ID then
				fa:replyMessage( table.ID[1] )
			else
				fa:replyMessage( nil )
			end
		end
	end
	if m.subject == 'private-update-pak' then
		if m.content[1] and m.content[2] then
			node.handle.db.updatePak( m.content[1], m.content[2] )
		end
	end
	if m.subject == 'private-add-pak-entry' then
		if m.content[1] and m.content[2] then
			local pakid = m.content[2]
			local rscid = m.content[1]
			local table = node.handle.db.addPakEntry( pakid, rscid )
			fa:replyMessage( ID )
		end
	end
end


function onDelete ( inh )
	for k,v in pairs(views) do
		node.handle.facade:removeObserver( k )
		v = nil
	end
	views = {}
	collectgarbage( )
end
