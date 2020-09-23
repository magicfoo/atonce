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



local function app_tunits ( filename )
	-- options:
	-- /tunits
	if __debug__ and (jam.profile.enable_tunits or app.args.tunits) then
		local tunits = '/app/tunits/'..filename..'.lua'
		if su.io.isFile(tunits) then
			call( tunits, {app=app} )
		end
	end
end


local function app_exectask ( taskname )
	local taskfile = '/app/tasks/task_'..taskname..'.lua'
	if su.io.isFile(taskfile) then
		local pos = su.string.indexOf( app.cmdline, '/task' )		
		local taskargs = su.string.right( app.cmdline, pos )
		call( taskfile, {app=app,taskargs=taskargs} )	
	end
end





-- setup the projet
-- /project => to set the project directory path

app.projectdir = (app.args.project) or (app.pdir..'/project')

if not su.io.isFullname(app.projectdir) then
	app.projectdir = su.io.getNormalized( app.pdir..'/'..app.projectdir )
end

if not su.io.isDirectory(app.projectdir) then
	error( 'The project must be a valid directory! Exited.' )
end

su.io.mount( '/project', 'MDriverFS', 'root='..su.io.getHostPath(app.projectdir) )
print( 'project <'..app.projectdir..'>' )

if not su.io.isDirectory('/project/jsettings') then
	error( "The project settings directory /project/jsettings not found! Exited." )
end

app.hkey:setValue( 'last project', app.projectdir )




-- setup the session
-- /session => to change the session basename

local sbase = (app.args.session) or (app.uname..','..app.udomain)

app.session = {}
app.sessionfile = '/project/jsettings/'..sbase..'.jsession'
app.sessiondefault = { volumes={ ['/tmp']='root=$tmp' }, dbman={ ['dbfile']='/project/jdb.sqlite' } }

if su.io.isFile(app.sessionfile) then
	app.session = su.io.loadStream( app.sessionfile )
	if not istable(app.session) then
		error( 'Error while loading the session file! Check for indentations in the yaml file <'..app.sessionfile..'>. Exited.' )
	end
else	
	copytable( app.session, app.sessiondefault )
	su.io.dumpStream( app.sessionfile, app.session )
end




-- mount session volumes

for k,v in pairs(app.session.volumes) do
	v = su.string.replace( v, '$app', app.directory )
	v = su.io.getNormalized( v )
	app.stdout( 'mounting volume '..k..' ['..v..']' )
	su.io.mount( k, 'MDriverFS', v )
end




-- launch application's pre-test-units
-- app_tunits( 'pre-tunits' )




-- application definition

app.STATE_INITIALIZING	= 0
app.STATE_STARTING		= 1
app.STATE_RUNNING		= 2
app.STATE_TERMINATING	= 3
app.STATE_EXITING		= 4
app.STATE_SHUTTING		= 5
app.STATE_SHUTTED		= 6

app.state = app.STATE_INITIALIZING

app.undeads = {}

app.skeleton = call( 'skeleton.lua', {app=app} )

app.facade = new 'MLuaAuthFacade'
if app.dbgmaster then
	app.facade:addObserver( app.dbgmaster, 'jam.luadbg' )
end

app.time = su.util.getTime()

app.turn = 1

app.resources = {}

function app.terminate ( )
	if app.state < app.STATE_TERMINATING then
		app.state = app.STATE_TERMINATING
		return true
	end
end



function app.printMessage ( m, uri )
	print( '\nmessage ['.. su.util.sid(m.subject) ..']' )
	print( 'when: '.. app.time ..'s' )
	print( 'uri: '.. (uri or '*') )
	printTable(m)
end



function app.undead ( inObject )
	if istable(inObject) then
		app.undeads[ #app.undeads+1 ] = inObject
	end
end



function app.deleteNode ( innodeH )

	local function delete_rec ( nh )
		if isfunction(nh.node.onDelete) then
			nh.node.onDelete( nh )
		end

		if nh.parent then
			for k,v in pairs(nh.parent.children) do
				if v==innodeH then
					nh.parent.children[k] = nil
					break
				end
			end
		end

		for _,ch in pairs(nh.children) do
			ch.parent = nil
			delete_rec( ch )
		end

		nh.parent = nil
		nh.children = nil

		app.undead( nh.node )
	end

	-- downward call onCreate()
	if innodeH then
		delete_rec( innodeH )
	end
end



function app.createNode ( inparentH, inskelname, inname, inenv )

	local function initialize_rec ( inph, inskelname, inname, inenv )
		assert( inskelname )
		local nskel = su.string.replace( inskelname, ' ', '_' )
		local ndef  = app.skeleton[ nskel ]
		local nfile = (ndef and ndef.file) or ('node_'..nskel..'.lua')
		local npath = '/app/nodes/'..nfile

		local nname = inname
		if not nname then
			local pc = inph and inph.children
			if pc and pc[nskel] then
				nname = #pc + 1
			else
				nname = nskel
			end
		end

		-- the node & its handle
		local n  = inenv or {}
		local nh = {}

		-- setup the node env
		if ndef and istable(ndef.env) then
			for k,v in pairs(ndef.env) do
				n[k] = v
			end
		end
		n.self = n
		n.node = { handle=nh, definition=ndef }

		-- setup the node handle
		nh.self			= nh
		nh.node			= n
		nh.parent		= inph
		nh.name			= nname
		nh.file			= npath
		nh.skelname		= nskel
		nh.children		= {}

		-- attach to parent
		if inph then
			assert( inph.children[nname] == nil )
			inph.children[nname] = nh

			-- inherit some handle's values from parent handle
			for k,v in pairs(inph) do
				if nh[k]==nil then
					nh[k]=v
				end
			end
		end

		-- initialize
		call( npath, n )
		if isfunction(n.onInitialize) then
			n.onInitialize( nh )
		end

		-- create & attach children
		if ndef then
			for k,v in pairs(app.skeleton) do
				if v.parent == ndef then
					initialize_rec( nh, k )
				end
			end
		end

		-- return handle
		return nh
	end


	local function create_rec ( nh )
		-- downward call onCreate()
		if isfunction(nh.node.onCreate) then
			nh.node.onCreate( nh )
		end
		for _,c in pairs(nh.children) do
			create_rec(c)
		end
	end


	local nh
	nh = initialize_rec( inparentH, inskelname, inname, inenv )
	create_rec( nh )

	-- return the node's handle
	return nh
end



function app.loadRscArchive ( infilename )
	local function import_cur_rsc ( rset, zfile )
		local finfo, fname, fdata, fimg
		finfo = zfile:aboutCurrent()
		fname = finfo.filename
		zfile:openCurrent()
		fdata = zfile:readCurrent()
		zfile:closeCurrent()
		fimg = su.fltk.newImage( fdata, fname )
		local rsc = { name=fname, data=fdata, flimage=fimg }
		rset[fname] = rsc
		return rsc
	end

	local rset = {}
	local zfile = su.util.openZip( infilename )
	if zfile and zfile:firstFile() then
		repeat
			import_cur_rsc( rset, zfile )
		until not zfile:nextFile()
	end
	return rset
end



function app.loadRscFile ( infilename )
	local fpath = sif( su.io.isFullname(infilename), infilename, '/app/resources/'..infilename )
	return su.io.loadBinary( fpath )
end



function app.treeInfo ( innode )
	local t = {}
	local function tree_rec ( h, l, indent )
		t[#t+1] = indent..l..': '..h.name
		for k,v in pairs(h.children) do
			tree_rec( v, l+1, indent..'  ' )
		end
	end
	tree_rec( innode, 1, '' )
	return t
end



function app.loop ( )

	local function upd_undead ( )
		local ul = {}
		for k,v in pairs(app.undeads) do
			if isfunction(v.onUndead) then
				if v.onUndead() then
					ul[#ul+1] = v
				end
			end
			app.undeads[k] = nil
		end
		for k,v in pairs(ul) do
			app.undeads[k] = v
		end
	end

	if app.state < app.STATE_SHUTTED then
		su.core.update()
		su.fltk.check()
		app.facade:postMessage({subject='app-wakeup'})
		app.facade:process()
		upd_undead()
	end

	if app.state < app.STATE_SHUTTING then

		-- running or exiting
		local t  = su.util.getTime()
		local dt = t - app.time
		app.time  = t
		app.dtime = dt
		app.turn = app.turn+1

		if app.state ~= app.STATE_RUNNING then
			app.state = app.state+1
		end

	elseif app.state == app.STATE_SHUTTING then

		-- shutting
		if app.root then
			app.deleteNode( app.root )
			app.root = nil
		end

		if #app.undeads == 0 then
			app.state = app.STATE_SHUTTED
		end
	end

	return (app.state < app.STATE_SHUTTED)
end


app.root = app.createNode( nil, 'root', nil, {app=app} )

-- launch application's post test-units
-- app_tunits( 'post-tunits' )


-- execute task or loop in application
-- /task:task-name

if app.args.task then
	app_exectask( app.args.task )
else
	while app.loop() do
	end
end


app = nil



