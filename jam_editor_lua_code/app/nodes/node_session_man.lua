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

tableBackup = nil
sessionChooser = nil
sessionFile = nil

function getTableBackup( )
	return tableBackup
end

function onInitialize( inh )
	
	sessionChooser = new 'MFlSessionChooser'
	inh.tableBackup = getTableBackup

	inh.facade:addObserver( onMessage, 'jam.session-man' )
	inh.facade:addObserver( sessionChooser, 'jam.session-chooser' )	
end


function onCreate ( inh )	
	local tableSession = node.handle.root.session()
	node.handle.facade:postMessage( {subject='open-session',target='jam.session-man',content={ tableSession } } )	
end


function onMessage ( fa, uri, m )

	if m.subject == 'open-session' then
		if m.content[1] then						
			tableBackup = m.content[1]
			if tableBackup then
				node.handle.tableBackup = tableBackup					
				if tableBackup.nodes then				
					--If layout-man isn't empty present the current layout
					fa:postMessage( {subject='present-layout',target='jam.layout-man',content={ tableBackup.nodes.layout_man.currentlayout } } )					
				else
					--If first open init layoutmanager with empty array
					tableBackup.nodes={}
					tableBackup.nodes.layout_man={}
					tableBackup.nodes.layout_man.layouts={}
					tableBackup.nodes.layout_man.currentlayout= nil
					fa:postMessage( {subject='present-layout',target='jam.layout-man',content={ 'default' } } )					
				end
			end
		end
	end
	if m.subject == 'select-session' then
		fa:postMessage( {subject='private-select-session', target='jam.session-chooser', content={} } )				
	end
	if m.subject == 'backup-session'  then	
		local backuped
		for p,o in pairs(tableBackup.nodes.layout_man.layouts) do
			backuped = fa:callMessage( {subject='backup-layout',target='jam.layout-man',content={p} } )				
		end		
		fa:replyMessage( backuped )				
		local ssfile = node.handle.root.sessionfile()
		su.io.dumpStream( ssfile, tableBackup )		
	end

	--Reply
	if m.subject == 'reply-backup-layout' then

	end

end


function onUndead ( )

end





