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


views = { }
nodeWindowType = { 'viewer' }
nodeUri = 'jam.asset-viewer'
currentDrag = {}



function backup( uriView )
	local tabSettings = {}
	tabSettings = node.handle.facade:callMessage( {subject='private-get-settings',target=uriView,content={} } )
	return tabSettings
end


function onInitialize( inh )
	inh.facade:addObserver( onMessage, nodeUri )
end


function onCreate( inh )

end

function OnDrag( from, what, id )
	local rlist = node.handle:getResource( '*', 'loading.png' )
	local img = rlist.flimage
	node.handle.facade:postMessage( {subject='show-overlay', target='jam.layout-man', source=nodeUri, content={ img, from } } )
	currentDrag.file = what
	currentDrag.uri = from
end

function onMessage ( fa, uri, m )

	if m.subject == 'private-on-drag' then
		OnDrag( m.source, m.content[1], 0 )

	elseif m.subject == 'overlay-shown' then
		fileDroped = resolvepath( currentDrag.file )
		binPath = nil
		if fileDroped then
			if su.io.hasExtension(fileDroped,'sua') then
				binPath = fileDroped .. '.pak'
				fa:callMessage( {subject='generate-pak-from-sua',target='jam.dam',content={ binPath, fileDroped} } )
			elseif su.io.hasExtension(fileDroped,'pak') then
				binPath = fileDroped
			end
		end
		if binPath then
			fa:callMessage( {subject='show-pak',target= currentDrag.uri ,content={ binPath } } )
		end
		fa:postMessage( {subject='hide-overlay', target='jam.layout-man', content={ currentDrag.uri } } )

	elseif m.subject == 'create-window' then
		if m.content[2] and m.content[1] then
			local uri = createView( m.content[1], m.content[3] )
			local requestID = m.content[2]
			fa:replyMessage( uri ,requestID )
		end

	elseif m.subject == 'duplicate-window' then
		if m.content[2] and m.content[1] and m.content[3] then
			local uri = createView( m.content[1], m.content[3] )
			local uriViewToCopy = m.content[3][1]
			if views[uriViewToCopy] then
				local settings = fa:callMessage( { subject='private-get-settings', target=uriViewToCopy, content={} } )	
				fa:callMessage( { subject='private-set-settings', target=uri, content={ settings } } )
			end
			local requestID = m.content[2]
			fa:replyMessage( uri ,requestID )
		end

	elseif m.subject == 'delete-window' then
		if m.content[1] then
			if( views[m.content[1]] ) then
				fa:removeObserver( m.content[1] )
				views[m.content[1]] = nil
				collectgarbage( )
			end
		end

	elseif m.subject == 'collect-window-managers' then
		fa:replyMessage( { nodeUri, nodeWindowType } )

	elseif m.subject == 'backup-window' then
		fa:replyMessage( backup( m.content[1] ) )

	elseif m.subject == 'on-drag-release' then
		if m.content[1] and m.content[2] and m.content[3] and m.content[4] then
			local x = m.content[1]
			local y = m.content[2]
			local string = m.content[3]
			local id = m.content[4]
			local under
			local uriView = nil

			-- Search for a view under
			for k,v in pairs(views) do
				under = fa:callMessage({ subject='private-is-under', target=k, content={x, y} } )
				if under == true then
					uriView = k
				end
			end

			-- Check if destination is available
			if uriView then
				OnDrag( uriView, string, id )
			end
		end
	end
end


function createView( inWindowType, inSettings )
	if inWindowType == 'viewer' then
		local av = new 'MAssetViewer'
		local uri = node.handle.facade:addObserver( av, nodeUri..'.%d' )
		assert( views[uri] == nil )	
		views[uri] = av
		if inSettings then
			node.handle.facade:callMessage( { subject='private-set-settings', target=uri, content={ inSettings } } )
		end
		local rlist = node.handle:getResource( '*', 'cursor.png' )
		local rlist2 = node.handle:getResource( '*', 'loop.png' )
		local rlist3 = node.handle:getResource( '*', 'pause.png' )
		local rlist4 = node.handle:getResource( '*', 'play.png' )
		local rlist5 = node.handle:getResource( '*', 'timeline.png' )
		node.handle.facade:callMessage( { subject='private-image-init', target = uri, content = { rlist.flimage, rlist2.flimage, rlist3.flimage, rlist4.flimage, rlist5.flimage } } )
		return uri
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




