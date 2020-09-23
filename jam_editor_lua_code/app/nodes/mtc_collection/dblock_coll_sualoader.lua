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
require 'datablock'





local function find_file ( _path, _reldir )
	local npath = su.io.getNormalized(_path)
	if su.io.isFile(npath) then
		return npath
	end

	-- virtual-path ?
	local vpath = su.io.getVirtualPath(npath)
	if not vpath then
		-- relative-path ?
		assert( isstring(_reldir) )
		vpath = _reldir .. '/' .. npath
	end

	if not su.io.isFile(vpath) then
		-- try sua nearby
		local basename = su.io.getBasename(npath)
		vpath = _reldir .. '/' .. basename		
	end

	if su.io.isFile(vpath) then
		return vpath
	end
end



function find_by_node ( n, olist )
	for k,v in iter(olist) do
		if v.node==n then
			return v
		end
	end
end




function import_anim ( _sua, _anim )
	local e = { type='anim' }
	e.sourceref = _anim
	e.dependencies = {}
	e.object = _anim
	return e
end




function import_node ( _sua, _node )
	local e = { type='node' }
	e.subtype = _node.type or 'node'
	e.sourceref = _node
	e.dependencies = { anim=_node.anim }
	e.object = _node
	e.name = _node.name

	if e.subtype=='geom' then
		local g = find_by_node(_node,_sua.geometry)

	elseif e.subtype=='camera' then
		local c = find_by_node(_node,_sua.camera)

	elseif e.subtype=='light' then
		local l = find_by_node(_node,_sua.light)

	elseif e.subtype=='helper' then
		local h = find_by_node(_node,_sua.helper)
	end

	return e
end



function import_trimesh ( _sua, _mesh )
	local e = { type='node' }
	e.sourceref = _node
	e.dependencies = { }
	e.object = _mesh
	return e
end




local function load_sua ( _file )

	-- parse sua
	local sua = su.io.loadStream( _file )
	if not sua then return end
	suadir = su.io.getDirname( _file )

	-- loaded content
	local cnt = { filerefs=type.list(), elements=type.list(), dependencies={}, cout=type.list() }

	-- import filerefs
	for _,v in iter(sua.fileref) do
		local f = find_file(v,suadir)
		if f then
			cnt.filerefs:push_back(f)
		end
	end

	-- import anim
	for _,v in iter(sua.anim) do
		local e = import_anim(sua,v)
		cnt.elements:push_back( e )
	end

	-- import trimesh
	for _,v in iter(sua.geometry) do
		local e = import_trimesh(sua,v)
		cnt.elements:push_back( e )
	end

	-- import nodes
	for _,v in iter(sua.node) do
		local e = import_node(sua,v)
		cnt.elements:push_back( e )
	end

	return cnt
end




local function compute ( _oplug )
	local bk = _oplug.block

	-- checks if all out-plugs are dirty
	local alldirty = bk:alldirty {'filerefs','elements','dependencies','cout'}

	-- need compute shared outputs ?
	if not bk.shared or alldirty then
		bk.shared = nil
		--
		local suafile = bk.sourcefile:getvalue()
		if isstring(suafile) then
			bk.shared = load_sua( suafile )
		end
	end

	if bk.shared then
		return bk.shared[_oplug.name]
	end
end




function db_coll_sualoader_register ( )
	local def = datablock.definition( compute, 'coll_sualoader' )
	def:affect ('sourcefile','filerefs') ('sourcefile','elements') ('sourcefile','dependencies') ('sourcefile','cout')
end




