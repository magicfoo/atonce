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


require 'type'



function h_stdout ( t )
	print( 'jam: '..t )
end



function h_app_terminate ( innode )
	app.terminate()
end



function h_app_state ( innode )
	return app.state
end



function h_app_turn ( innode )
	return app.turn
end



function h_app_time ( innode )
	return app.time
end



function h_find_child ( innode, inchild )
	if innode then
		if istable(inchild) then
			-- by value
			for k,v in pairs(innode.children) do
				if v==inchild then
					return k
				end
			end
		else
			-- by name
			return innode.children[ inchild ]
		end
	end
end



function h_has_child ( innode, inchild )
	local c = h_find_child( innode, inchild )
	return (c~=nil)
end



function h_create_child ( innode, inskelname, inname )
	return app.createNode( innode, inskelname, inname )
end



function h_delete_child ( innode, inchild )
	local c = h_find_child( innode, inchild )
	if c then
		app.deleteNode( c )
	end
end



function h_find_child_rec ( innode, inchild )
	local c = h_find_child( innode, inchild )
	if c then return c end

	if not c then
		for k,v in pairs(innode.children) do
			c = h_find_child_rec( v, inchild )
			if c then return c end
		end
	end
end



function h_apply ( innode, ... )
	local function apply_rec ( inh, ... )
		if inh and inh.node and isfunction(inh.node.onApply) then
			local r = inh.node.onApply(...)
			if r then return r end
		end
		for k,v in pairs(inh.children) do
			local r = apply_rec(v,...)
			if r then return r end
		end
	end
	return apply_rec( innode, ... )
end



function h_skeldef ( innode, inskelname )
	return app.skeleton[ inskelname ]
end



function h_skelname ( innode, inskelvalue )
	if inskelvalue then
		for k,v in pairs(app.skeleton) do
			if v==inskelvalue then
				return k
			end
		end
	end
end



function h_dump ( innode, incfg )
	local cfg = incfg or {}
	if innode and innode.node and isfunction(innode.node.onDump) then
		innode.node.onDump( innode, cfg )
	end
	return cfg
end



function h_load ( innode, incfg )
	if innode and innode.node and isfunction(innode.node.onLoad) then
		innode.node.onLoad( innode, incfg )
	end
end



function h_node_treepath ( innode )
	local function path_rec ( inh )
		if inh.parent then
			return path_rec(inh.parent)..'.'..inh.name
		else
			return inh.name
		end
	end
	return path_rec( innode )
end



function h_select_rsc ( innode, inset, inname )
	assert( isstring(inset) )
	assert( isstring(inname) )

	local function select_values ( t, pattern )
		local v = t[ pattern ]
		if v then
			return {v}
		else
			local r = {}
			for k,v in pairs(t) do
				if su.string.match(k,pattern) then
					r[#r+1] = v
				end
			end
			return r
		end
	end

	local r = {}
	local s1 = select_values( app.resources, inset )
	for k1,v1 in pairs(s1) do
		local s2 = select_values( v1, inname )
		for k2,v2 in pairs(s2) do
			r[#r+1] = v2
		end
	end
	return r
end



function h_get_rsc ( innode, inset, inname )
	local r = h_select_rsc( innode, inset, inname )
	if #r > 0 then
		return r[1]
	end
end



function h_load_rsc ( innode, infilename )
	return app.loadRscFile( infilename )
end



function h_tree_info ( innode )
	return app.treeInfo( innode )
end



function h_args ( innode )
	return app.args
end


function h_session( innode )
	return app.session
end


function h_sessionfile( innode )
	return app.sessionfile
end


function h_directory( innode )
	return app.directory
end




local timers = type.list()


function h_timer_reload ( innode, inkey, indelay )
	if isstring(inkey) and isnumber(indelay) and indelay>0 then

		local to = su.util.getTime() + indelay

		for k,v in pairs(timers) do
			if v.node==innode and v.key==inkey then
				v.to = to
				return
			end
		end

		-- append it
		timers.push_back { node=innode, key=inkey, to=to }
	end
end


function h_timer_cancel ( innode, inkey )
	if isstring(inkey) then
		for k,v in pairs(timers) do
			if v.node==innode and v.key==inkey then
				timers[k] = nil
			end
		end
	end
end


function h_timer_sync ( innode )
	h_timer_reload( innode, 'sync', 1 )
end


function timer_update ( fa )
	local t = su.util.getTime()

	for k,v in pairs(timers) do
		if v.to <= t then
			local cb = v.node.onTimeout
			if isfunction(cb) then
				cb( v.key )
			end
			timers[k] = nil
		end
	end
end




function onInitialize( inh )

	assert( node.handle == inh )

	inh.root			= inh
	inh.facade			= app.facade
	inh.stdout			= h_stdout
	inh.findChild		= h_find_child
	inh.findChildRec	= h_find_child_rec
	inh.hasChild		= h_has_child
	inh.createChild		= h_create_child
	inh.deleteChild		= h_delete_child
	inh.apply			= h_apply
	inh.skeldef			= h_skeldef
	inh.skelname		= h_skelname
	inh.dump			= h_dump
	inh.load			= h_load
	inh.treepath		= h_node_treepath
	inh.terminate		= h_app_terminate
	inh.state			= h_app_state
	inh.turn			= h_app_turn
	inh.time			= h_app_time
	inh.selectResource	= h_select_rsc
	inh.getResource		= h_get_rsc
	inh.loadResource	= h_load_rsc
	inh.args			= h_args
	inh.directory		= h_directory
	inh.session			= h_session
	inh.sessionfile		= h_sessionfile
	inh.timerReload		= h_timer_reload
	inh.timerCancel		= h_timer_cancel
	inh.timerSync		= h_timer_sync

	-- preload all resource files
	local rsc_files = su.io.listDirectory( '/app/resources', '*.zip' )
	for i,f in ipairs(rsc_files) do
		local rset = app.loadRscArchive( '/app/resources/'..f )
		local rname = su.string.mid(f,1,-5)
		app.resources[rname] = rset
	end

end



function onCreate ( inh )

	--printTable( app.treeInfo(inh) )

end



function onMessage ( fa, uri, m )

	if m.subject == 'app-wakeup' then
		timer_update( fa )
	end

end


