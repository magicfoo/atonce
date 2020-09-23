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



include 'dblock_dbobserver.lua'
include 'dblock_dbsolver.lua'


db = nil
nodeUri = 'jam.db-man'



function selectAll( inTable )
	local createLine = 'SELECT * FROM "'..inTable..'"'
	local table = node.handle.db.executeSQL( createLine )
	return table
end

function createTable( inTable, inCreationLine )
	local signature = node.handle.db.getSignature( inTable )
	if not node.handle.db.isTable( inTable ) then
		node.handle.db.executeSQL( inCreationLine )
	else
		if not signature then
			node.handle.db.executeSQL( inCreationLine )
		else
			if not signature == inCreationLine then
				node.handle.db.executeSQL( inCreationLine )
			end
		end
	end
end

function dropTable( inTable )
	local createLine = 'DROP TABLE "main"."'..inTable..'"'
	node.handle.db.executeSQL( createLine )
	return true
end

function cleanTable( inTable )
	local createLine = 'DELETE FROM "main"."'..inTable..'"'
	node.handle.db.executeSQL( createLine )
	return true
end

function addInTable( checkQuery, addQuery )
	local table = node.handle.db.executeSQL( checkQuery )
	if not table.ID then
		node.handle.db.executeSQL( addQuery )
	end
	table = node.handle.db.executeSQL( checkQuery )
	return table.ID[1]
end

function countAll( inTable )
	if not node.handle.db.isTable( inTable ) then
		return 0
	end
	local query = 'SELECT COUNT(ID) FROM "'..inTable..'"'
	local table = node.handle.db.executeSQL( query )
	return table['COUNT(ID)'][1]
end

function openDB( inPath )
	if inPath then
		if not db:opendb( inPath ) then
			assert( db:createdb( inPath ) )
			assert( db:opendb( inPath ) )
		end
		return true
	end
end

function closeDB( )
	return db:closedb( )
end

function executeSQL( inQuery )
	if inQuery then	
		return  db:execute( inQuery )
	end
end

function isTable( inName )
	if inName then
		local query = 'SELECT name FROM sqlite_master WHERE type="table" AND name="'..inName..'"'
		local table = executeSQL( query )
		return table and table.name
	end
end

function beginTransaction( )
	executeSQL( 'BEGIN TRANSACTION' )
end

function commitTransaction( )
	executeSQL( 'COMMIT TRANSACTION' )
end

function getSignature( inTableName )
	local checkLine = 'SELECT sql FROM sqlite_master WHERE type="table" AND name ="'..inTableName..'"'
	local table = executeSQL( checkLine )
	if table and table.sql then
		return table.sql[1]
	end
end


function hook( )
	return db:collectHookTable( )
end


function onInitialize( inh )
	db = new 'MAssetDatabase'
	assert( db )

	inh.facade:addObserver( onMessage, nodeUri )

	inh.db = {}
	inh.db.openDB = openDB
	inh.db.closeDB = closeDB
	inh.db.executeSQL = executeSQL
	inh.db.getSignature = getSignature
	inh.db.beginTransaction = beginTransaction
	inh.db.commitTransaction = commitTransaction
	inh.db.hook = hook
	inh.db.isTable = isTable
	inh.db.selectAll = selectAll
	inh.db.createTable = createTable
	inh.db.dropTable = dropTable
	inh.db.cleanTable = cleanTable
	inh.db.addInTable = addInTable
	inh.db.countAll = countAll

	register_dbsolver()
	register_dbobserver()
end

function onCreate ( inh )
	node.handle.db.openDB( inh.session().dbman.dbfile )
end


function onMessage ( fa, uri, m )
	
	if m.subject == 'open-db' then
		if m.content[1] then
			openDB( m.content[1] )
		end
	end

	if m.subject == 'close-db' then
		closeDB( )
	end

	if m.subject == 'execute-sql' then
		if m.content[1] then
			return executeSQL( m.content[1] )
		end
	end

	if m.subject == 'flush' then

	end

end


function onDelete ( inh )
	node.handle.db.closeDB( )
end



