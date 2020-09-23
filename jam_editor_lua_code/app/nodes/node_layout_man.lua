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

requestID = 1
viewLayout = nil
windowType = { }
nodeUri = 'jam.layout-man'
viewUri = 'jam.layout-man.hud'
currentVersion = 1

function backup( name )
	if node.handle.parent.tableBackup.nodes.layout_man.layouts[name] then
		local table = node.handle.facade:callMessage( {subject='private-backup-layout',target=viewUri,content={name} } )
		if table ~= nil then
			node.handle.parent.tableBackup.nodes.layout_man.layouts[name] = table
		end
		--affect table to the layout entry
		return true
	else
		return false
	end
end

function getWindowManager( windowTypeName )
	for manager, t in pairs(windowType) do
		for p, o in pairs( t ) do
			if o == windowTypeName then
				 return manager
			end
		end
	end
	return nil
end

function renameLayout( oldName, newName )
	local reply = node.handle.facade:callMessage( {subject='private-rename-layout',target=viewUri,content={oldName, newName} } )
	if reply == true then
		node.handle.parent.tableBackup.nodes.layout_man.layouts[newName] = node.handle.parent.tableBackup.nodes.layout_man.layouts[oldName]
		node.handle.parent.tableBackup.nodes.layout_man.layouts[oldName] = nil
		return true
	else
		return false
	end
end


function listLayout( )
	local list = {}
	for p,o in pairs(node.handle.parent.tableBackup.nodes.layout_man.layouts) do
		list[#list+1] = p
	end
	return list
end


function deleteLayout( name )
	local deleted = node.handle.facade:callMessage( {subject='private-delete-layout', target=viewUri,content={ name } } )
	if deleted == true then
		if node.handle.parent.tableBackup.nodes.layout_man.layouts[name] then	
			node.handle.parent.tableBackup.nodes.layout_man.layouts[name] = nil
			return true
		else
			return false
		end		
	else
		return false
	end
end


function addLayout( name )
	if node.handle.parent.tableBackup.nodes.layout_man.layouts[name] == nil then
		local layout = {}
		layout['displays'] = {{}}
		layout['displays'][1]['location'] = {0,0,800,600}
		layout['displays'][1]['main-display'] = 'true'
		layout['windows'] = {{}}
		layout['windows'][1]['display'] = 1
		layout['windows'][1]['location'] = {0,0,800,600}
		layout['windows'][1]['uri-manager'] = ''
		layout['windows'][1]['settings'] = {}
		node.handle.parent.tableBackup.nodes.layout_man.layouts[name] = layout
		node.handle.parent.tableBackup.nodes.layout_man.currentlayout= name
		return true
	else
		return false
	end
end

function addDefaultLayout( name )
	addLayout( name )
	requestID = requestID + 1
	node.handle.parent.tableBackup.boot = {}
	node.handle.parent.tableBackup.boot.version = currentVersion
	node.handle.facade:postMessage( {subject='private-add-layout',target=viewUri,content={name} } )
	node.handle.facade:postMessage( {subject='private-show-layout',target=viewUri,content={ name } } )
	node.handle.facade:postMessage( {subject='private-add-display',target=viewUri,content={ {0, 0, 800, 600, 1, 1}} } )
	node.handle.facade:postMessage( {subject='private-add-window',target=viewUri,content={ {0, 0, 800, 600, 1, 'request'..requestID, 'jam.asset-viewer', 'viewer'} } } )
	node.handle.facade:postMessage( {subject='create-window',target='jam.asset-viewer',content={'viewer','request'..requestID,{}} } )
end


function presentLayout( name )

	if jam.profile.console_only == true then
		return true
	end

	if node.handle.parent.tableBackup.boot and node.handle.parent.tableBackup.boot.version == currentVersion  then
		-- Check Layout and load it only if isn't already loaded
		local existing = node.handle.facade:callMessage( {subject='private-is-layout',target=viewUri,content={name} } )

		if existing == false then

			if jam.profile.asset_viewer_only then
				addDefaultLayout( name )
				node.handle.facade:postMessage( {subject='private-desactive-tile', target=viewUri,content={} } )
				return true
			end

			if node.handle.parent.tableBackup.nodes.layout_man.layouts[name] then
				-- Add Layout
				node.handle.facade:postMessage( {subject='private-add-layout',target=viewUri,content={name} } )
				-- Show Layout
				node.handle.facade:postMessage( {subject='private-show-layout',target=viewUri,content={ name } } )
				-- Add Windows
				for p,o in pairs(node.handle.parent.tableBackup.nodes.layout_man.layouts[name].displays) do
					node.handle.facade:postMessage( {subject='private-add-display',target=viewUri,content={ {o.location[1], o.location[2], o.location[3], o.location[4], p, o['main-display']}} } )
				end
				-- Add and Assign Slots
				for p,o in pairs(node.handle.parent.tableBackup.nodes.layout_man.layouts[name].windows) do
					requestID = requestID + 1
					node.handle.facade:postMessage( {subject='private-add-window',target=viewUri,content={ {o.location[1], o.location[2], o.location[3], o.location[4], o.display, 'request'..requestID, o['uri-manager'], o['window-type']} } } )
					node.handle.facade:postMessage( {subject='create-window',target=o['uri-manager'],content={o['window-type'],'request'..requestID,o['settings']} } )
				end
				return true
			end

			if name == 'default' then
				addDefaultLayout( name )
				return true
			end

		else
			-- Show Layout it already exist
			node.handle.facade:postMessage( {subject='private-show-layout',target=viewUri,content={ name } } )
		end
	else
		addDefaultLayout( name )
		return true
	end
	return true
end


function onInitialize( inh )

	--Intialize Handle
	inh.loadNode = loadNode

	--Creation of the manager
	viewLayout = new 'MFlLayoutManager'

	--Add observer
	inh.facade:addObserver( onMessage, nodeUri )
	inh.facade:addObserver( viewLayout, viewUri )

	--Init Images
	local rlist = node.handle:getResource( '*', 'corner.png' )
	local corner = rlist.flimage
	local rlist3 = node.handle:getResource( '*', 'jam_title.png' )
	local title = rlist3.flimage
	local rlist5 = node.handle:getResource( '*', 'close.png' )
	local closebutton = rlist5.flimage
	local rlist6 = node.handle:getResource( '*', 'minimize.png' )
	local minbutton = rlist6.flimage

	node.handle.facade:postMessage { subject='private-image-init', target=viewUri,content={minbutton,closebutton,title,corner} }

end


function onCreate ( inh )

end


function onDelete ( inh )
	node.handle.facade:removeObserver( viewUri )
	viewLayout = nil
	collectgarbage( )
end


function onMessage ( fa, uri, m )

	-- New Message 

	if m.subject == 'present-layout' then
		fa:postMessage( {subject='collect-window-managers',target='jam.*',content={ } } )
		if m.content[1] then
			local presented = presentLayout( m.content[1] )
			fa:replyMessage( presented )
		else
			fa:replyMessage( false )
		end
	end
	if m.subject == 'list-layout' then
		local list = listLayout( )
		fa:replyMessage( list )
	end
	if m.subject == 'add-layout' then
		if m.content[1]then	
			local added = addLayout( m.content[1] )
			fa:replyMessage( added )
		else
			fa:replyMessage( false )
		end
	end
	if m.subject == 'rename-layout' then
		if m.content[1] and m.content[2] then
			local renamed = renameLayout( m.content[1], m.content[2] )
			fa:replyMessage( renamed )
		else
			fa:replyMessage( false )
		end
	end
	if m.subject == 'inspect-layout' then
		if m.content[1] then
			local arrayOut = fa:callMessage( {subject='private-inspect-layout',target=viewUri,content={ m.content[1] } } )
			fa:replyMessage( arrayOut )
		else
			fa:replyMessage( {} )
		end
	end
	if m.subject == 'delete-layout' then
		if m.content[1] then
			local deleted = deleteLayout( m.content[1] )
			fa:replyMessage( deleted )
		end
	end
	if m.subject == 'modify-layout' then
		if m.content[1] and m.content[2] and m.content[3] then
			fa:postMessage( {subject='create-window',target=m.content[3],content={m.content[2],m.content[1],{}} } )
		end
	end
	if m.subject == 'backup-layout' then
		if m.content[1] then
			local backuped = backup( m.content[1] )
			fa:replyMessage( backuped )
		end
	end
	if m.subject == 'show-overlay' then
		if m.content[1] and m.content[2] then
			fa:callMessage( {subject='private-show-overlay',target=viewUri,content={ m.content[1], m.content[2], m.source } } )
		end
	end
	if m.subject == 'hide-overlay' then
		if m.content[1] then
			fa:callMessage( {subject='private-hide-overlay',target=viewUri,content={ m.content[1] } } )
		end
	end
	if m.subject == 'private-hide-display' then
		local reply = fa:callMessage( {subject='private-have-to-close', target=viewUri, content={} } )
		if reply == true then
			-- start backup
			if not jam.profile.asset_viewer_only then
				fa:callMessage( {subject='backup-session', target='jam.session-man', content={} } )
			end
			-- delete window
			fa:removeObserver( viewUri )
			viewLayout = nil
			collectgarbage( )
			node.handle.terminate()
		end
	end
	if m.subject == 'private-modify-layout' then
		local manager = getWindowManager( m.content[2] )
		fa:postMessage( {subject='delete-window', target=m.content[3], content={m.content[1]}} )
		if manager then
			fa:postMessage( {subject='private-update-window', target=viewUri, content={manager,m.content[2],m.content[1] } } )
			fa:postMessage( {subject='create-window', target=manager, content={m.content[2],m.content[1],{}} } )
		end
	end
	if m.subject == 'private-duplicate-window' then
		if m.content[1] then
			requestID = requestID + 1
			fa:postMessage( {subject='private-add-window',target=viewUri,content={ {m.content[1].x, m.content[1].y, m.content[1].w, m.content[1].h, m.content[1].idxDisplay, 'request'..requestID, m.content[1].uriManager, m.content[1].windowType} } } )
			fa:postMessage( {subject='duplicate-window',target=m.content[1].uriManager,content={m.content[1].windowType,'request'..requestID,{m.content[1].uriView}} } )
		end
	end

	-- Reply

	if m.subject == 'reply-create-window' then
		if m.content[1] and m.content[2] then
			fa:postMessage( {subject='private-assign-window',target=viewUri,content={ m.content[1], m.content[2] } } )
		end
	end
	if m.subject == 'reply-duplicate-window' then
		if m.content[1] and m.content[2] then
			fa:postMessage( {subject='private-assign-window',target=viewUri,content={ m.content[1], m.content[2] } } )
		end
	end
	if m.subject == 'reply-collect-window-managers' then
		if m.content[1] then
			windowType[m.content[1][1]] = m.content[1][2]
			fa:postMessage( {subject='private-update-windowtype', target=viewUri, content={m.content[1][2]} } )
		end
	end
end


function onUndead ( )

end



