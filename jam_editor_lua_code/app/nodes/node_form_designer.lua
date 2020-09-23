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
nodeWindowType = { 'designer' }
nodeUri = 'jam.form-designer'



function backup( uriView )
	return {}
end


function onInitialize( inh )
	inh.facade:addObserver( onMessage, nodeUri )
end


function onCreate( inh )	
	
end

function createView( inWindowType, inSettings )
	if inWindowType == 'designer' then
		local av = new 'MFlDesigner'
		local uri = node.handle.facade:addObserver( av, nodeUri..'.%d' )
		assert( views[uri] == nil )
		views[uri] = av
		return uri
	end
end


function onMessage ( fa, uri, m )

	if m.subject == 'create-window' then
		if m.content[2] and m.content[1] and m.content[3] then
			local uri = createView( m.content[1], m.content[3] )				
			local requestID = m.content[2]
			fa:replyMessage( uri ,requestID )
		end
	end
	if m.subject == 'duplicate-window' then
		if m.content[2] and m.content[1] and m.content[3] then
			local uri = createView( m.content[1], m.content[3] )	
			local uriViewToCopy = m.content[3][1]
			local requestID = m.content[2]
			fa:replyMessage( uri ,requestID )
		end
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
	
	--if m.subject == 'collect-window-managers' then
	--	fa:replyMessage( { nodeUri, nodeWindowType } )
	--end
	
	if m.subject == 'backup-window' then
		fa:replyMessage( backup( m.content[1] ) )
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




