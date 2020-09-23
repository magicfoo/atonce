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
require 'profiler'
require 'weakref'
require 'datablock'




function mtc_get_name ( )
	return 'World'
end



function mtc_realize ( )
	--
end



function onInitialize( inh )
	--inh.node.mtcGetName = mtc_get_name
	inh.node.mtcRealize = mtc_realize
end








function generate_collection_from_sua ( in_suafile )

	-- parse sua
	local sua = su.io.loadStream( in_suafile )
	if not sua then
		return
	end

	suadirectory = su.io.getDirname( in_suafile )


	local coll = type.list()

	local defines = type.list()
	local ids = {}


	local ort_type = {
		['undef']		= 0,
		['constant']	= 1,
		['cycle']		= 2,
		['oscillate']	= 3,
		['linear']		= 4,
		['rel-cycle']	= 5
	}

	function ort_rec ( inort )
		local o1,o2 = unpack( su.string.split(inort) )
		local t1,t2 = ort_type[o1], ort_type[o2]
		assert( t1 and t2 )
		return { t1, t2 }
	end


	function timerange_rec ( inrange, intimes )
		inrange[1] = min( inrange[1], intimes[1] )
		inrange[2] = min( inrange[2], intimes[#intimes] )
		return inrange
	end


	function define ( obj, name )
		assert( obj )
		local id = defines:size() + 1
		local def = tag 'define'
		def.id = id
		def.object = obj
		ids[obj] = id
		defines:push_back( def )
		return def
	end


	function objectdef ( obj )
		local id = ids[obj]
		if id then return defines[id] end
	end


	function objectid ( obj )
		return ids[obj] or 0
	end


	function find_by_node ( n, olist )
		for k,v in iter(olist) do
			if v.node==n then
				return v
			end
		end
	end


	function import_fx ( fxObj )
	end

	function import_bitmap ( authBmp )
		w = authBmp:width()
		h = authBmp:height()

		local bmp = tag 'bitmap'
		bmp.width	= w
		bmp.height	= h
		bmp.wpad	= 0
		bmp.hpad	= 0
		bmp.astatus	= 0
		coll:push_back( bmp )

		
		bGenGl = true
		bGenDx = true

		if bGenGl then
				
			local gl_fmt = {
				GL_RGB		 = 0x1907,
				GL_RGBA		 = 0x1908,
				GL_LUMINANCE = 0x1909
			}
			local gl_type = {
				GL_UNSIGNED_BYTE = 0x1401
			}

			local genData = authBmp:generateGlMip( gl_fmt['GL_RGBA'], gl_type['GL_UNSIGNED_BYTE'] );
			assert( genData )

			if genData then
				local mipGl = tag 'bitmap.glmip'
				mipGl.format= gl_fmt['GL_RGBA']
				mipGl.type	= gl_type['GL_UNSIGNED_BYTE']
				mipGl.size	= size( genData )
				mipGl.data	= genData

				coll:push_back( mipGl )
			end
		end
		if bGenDx then
			local dx_fmt = {
				
				D3DFMT_R8G8B8	= 20,
				D3DFMT_A8R8G8B8	= 21,
			}

			local genData = authBmp:generateDx9Mip( dx_fmt['D3DFMT_A8R8G8B8'] );
			assert( genData )

			if genData then
				local mipDx9 = tag 'bitmap.dxmip'
				mipDx9.format	= dx_fmt['D3DFMT_A8R8G8B8']
				mipDx9.size		= size( genData )
				mipDx9.data		= genData

				coll:push_back( mipDx9 )
			end
		end
	end



	function import_fileref ( infref )
		-- try texture or fx
		
		vPath = su.io.getVirtualPath( infref )			-- try absolute
		if vPath == nil then
			vPath = suadirectory .. '/' .. infref		-- try relative
		end
		if not su.io.isFile( vPath ) then
			baseName = su.io.getBasename( infref )
			vPath = suadirectory .. '/' .. baseName		-- try sua nearby
		end

		local authBmp = new 'MAuthBitmap'

		if authBmp:loadFromFile( vPath ) then
			
			local def = define( infref )
			coll:push_back( def )
			import_bitmap( authBmp )
		else
		--	authFx = nil
		--	if authFx:loadFromFile()
		end
	end


	function import_anim ( inanim )
		local def = define(inanim)
		local anim = tag 'anim'
		anim.bbox = { center={0,0,0}, rotation={0,0,0,1}, length={1,1,1} }
		anim.timerange = {}
		coll:push_back( def )
		coll:push_back( anim )

		function generate_klin_anim ( t, ctrl, srate )
			local keys = ctrl.key
			local n = #keys.time
			local v = #keys.value
			local s = ctrl['k-size']
			assert( v+n==n*s )
			t.samplingrate = srate or 0
			t.ort = ort_rec(ctrl.ort)
			t.keycnt = n
			t.keys = type.array(n*s)
			local k=1
			for i=1,n do
				t.keys:push_back( keys.time[i] )
				for j=1,s-1 do
					t.keys:push_back( keys.value[k] )
					k=k+1
				end
			end
		end

		-- loc controler
		if inanim.loc then
			local ctrl = inanim.loc
			local t = tag 'anim.klin.loc'
			generate_klin_anim( t, ctrl, inanim['sampling-rate'] )
			coll:push_back( t )
			anim.timerange = timerange_rec( anim.timerange, ctrl.key.time )
		end

		-- rot controler
		if inanim.rot then
			local ctrl = inanim.rot
			local t = tag 'anim.klin.rot'
			generate_klin_anim( t, ctrl, inanim['sampling-rate'] )
			coll:push_back( t )
			anim.timerange = timerange_rec( anim.timerange, ctrl.key.time )
		end

		-- scl controler
		if inanim.scl then
			local ctrl = inanim.scl
			local t = tag 'anim.klin.scl'
			generate_klin_anim( t, ctrl, inanim['sampling-rate'] )
			coll:push_back( t )
			anim.timerange = timerange_rec( anim.timerange, ctrl.key.time )
		end
	end


	function import_trimesh ( ingeom )

		inmesh = ingeom.mesh

		local locations
		locations = type.v3list(inmesh['vtx-location'])
		locations:swizzle_xzy()

		local uv0, uv1, uv2
		if inmesh['face-vtx-tex']			then uv0 = type.v2list(inmesh['face-vtx-tex']) end
		if inmesh['face-vtx-tex-channel-2']	then uv1 = type.v2list(inmesh['face-vtx-tex-channel-2']) end
		if inmesh['face-vtx-tex-channel-3']	then uv2 = type.v2list(inmesh['face-vtx-tex-channel-3']) end

		local normals
		if inmesh['face-vtx-normal'] then
			normals = type.v3list(inmesh['face-vtx-normal'])
			normals:swizzle_xzy()
		end

		local facecnt = #inmesh['face-vtx-location'] / 3
		assert( #inmesh['face-mat'] == facecnt )

		function collect_mat ( )
			local ml = {}
			for fi=1,facecnt do
				local mi = inmesh['face-mat'][fi]
				local m = ml[mi] or {}
				m[#m+1] = fi
				ml[mi] = m
			end
			return ml
		end

		function get_face( i )
			local j = (i-1)*3
			local f = { }
			for i=1,3 do
				local li = inmesh['face-vtx-location'][j+i] + 1
				f[i] = { li=li, l=locations:get_xyz(li) }
				if uv0 then f[i].uv0 = uv0:get_xyz(j+i) end
				if uv1 then f[i].uv1 = uv1:get_xyz(j+i) end
				if uv2 then f[i].uv2 = uv2:get_xyz(j+i) end
	--			if normals then f[i].n = normals:get_xyz(j+i) end
			end
			return f
		end

		local def = define(ingeom)
		local mesh = tag 'mesh'
		mesh.bbox = locations:bbox()
		mesh.skeletonid = -1
		coll:push_back( def )
		coll:push_back( mesh )


		local vf = 2^1											-- VF_LOC3
		local stride = 3*4
		
		if uv0     then vf=vf+2^2; stride=stride+2*4 end		-- VF_UV0
		if uv1     then vf=vf+2^3; stride=stride+2*4 end		-- VF_UV1
		if uv2     then vf=vf+2^4; stride=stride+2*4 end		-- VF_UV2
																-- VF_COL
	--	if normals then vf=vf+2^6; stride=stride+3*4 end		-- VF_NRM
																-- VF_TAN
																-- VF_BIN

		

		local mats	= collect_mat()
		local ibpos = 0
		local vbpos = 0

		local vboidx = 0

		for mati,matf in iter(mats) do

			local m1 = profiler.startmeasure( 'generating surface' )

			-- collect texids
			texids = {}

			cmat = nil
			if ingeom.material.type == 'multi/sub-object' then
				matSub = ingeom.material['sub']
				assert( matSub )
				cmIdx = matSub[mati+1] + 1
				if cmIdx ~= 0 then
					cmat = sua.material[cmIdx]
				end
			else
				assert( mati == 0 )
				cmat = ingeom.material
			end

			if cmat and cmat.effectparams then
															-- specific jam fx for now -> waiting IEffectDocument
				texNameA = { 'DiffuseTex', 'LightTex' }		-- later : get this array through IEffectDocument

				for _,texName in iter(texNameA) do
					tf = cmat.effectparams[texName]
					if tf then
						tid = objectid( tf )
						if tid ~= 0 then
							texids[#texids+1] = tid
						end
					end
				end
			end


			-- vbo for surface
			local vbo = tag 'mesh.vbo'
			vbo.vf			= vf
			vbo.stride		= stride
			vbo.primtype	= 2													-- PRIM_TRIANGLE
			vbo.dynamic		= 0
			vbo.count		= (#matf)*3

			vbo.data		= {}
			vbo.data.size	= vbo.count*vbo.stride
			vbo.data.bytes	= su.util.newBlob( vbo.data.size )
		
			vbo.kick		= {}
			vbo.kick.size	= vbo.count
			vbo.kick.bits	= {}

			vb_curs = su.util.newCursor( vbo.data.bytes )
			coll:push_back( vbo )


			-- surf
			local surf = tag 'mesh.surface'
			surf.vboidx = vboidx
			surf.effectid = 0
			surf.techniqueidx = 0
			surf.shdparams = string_rec('a')
			surf.texids = { count=#texids, ids=texids }
			coll:push_back( surf )

			vboidx = vboidx+1

			-- update vb & ib
			for k,v in iter(matf) do
				local f = get_face( v )
				for i=1,3 do
					-- swap one triangle on two for stripping

					local ii = i
					if k%2 == 1 then
						if (i-1)%3 == 1 then	ii = i+1 end
						if (i-1)%3 == 2 then	ii = i-1 end
					end
					vb_curs:writef32( unpack(f[ii].l) )
					if uv0 then vb_curs:writef32( unpack(f[ii].uv0) ) end
					if uv1 then vb_curs:writef32( unpack(f[ii].uv1) ) end
					if uv2 then vb_curs:writef32( unpack(f[ii].uv2) ) end
		--			if normals then vb_curs:writef32( unpack(f[ii].n) ) end
				end
				
				vbo.kick.bits[#vbo.kick.bits+1] = 0
				vbo.kick.bits[#vbo.kick.bits+1] = 0
				vbo.kick.bits[#vbo.kick.bits+1] = 1
			end

			profiler.getmeasure(m1)
		end

		return def
	end


	function import_node ( inl, innode )
		local n = tag 'litem.node'
		n.parentidx = innode.parent or -1
		n.name = string_rec( innode.name )
		n.note = string_rec( innode.note )
		n.hidden = innode.hidden or false
		n.trs = matrix_rec( innode.location, innode.rotation, innode.scale )
		n.trsanimid = objectid(innode.anim)

		local t = innode.type or 'node'

		if t=='geom' then
			local g = find_by_node(innode,sua.geometry)
			if g then
				n.tagid = 'rsc.collection.litem.geometry'
				n.meshid = objectid(g)
				n.offset = matrix_rec( g['offset-location'], g['offset-rotation'], g['offset-scale'] )
				n.color = color_rec( g.color )
			end

		elseif t=='camera' then
			local c = find_by_node(innode,sua.camera)
			if c then
				n.tagid = 'rsc.collection.litem.camera'
				n.fov = c.fov
				n.fovanimid = 0
				n.targetdist = c['target-dist'] or 0
				n.envrange = c['env-range'] or {0,0}
			end

		elseif t=='light' then
			local l = find_by_node(innode,sua.light)
			if l then
				n.tagid = 'rsc.collection.litem.light'
			end

		elseif t=='helper' then
			local h = find_by_node(innode,sua.helper)
			if h then
				n.tagid = 'rsc.collection.litem.helper'
				n.width = 0
				n.height = 0
				n.depth = 0
			end

		elseif t=='bone' then
			n.tagid = 'rsc.collection.litem.bone'
			n.length = 0
		end

		inl:push_back( n )
	end


	function push_back_list ( inl, inname )
		if not inl:empty() then
			local li = tag 'list'
			li.name = string_rec( inname )
			li.count = inl:size()
			coll:push_back( li )
			--
			for _,v in pairs(inl) do coll:push_back(v) end
		end
	end


	local dpyl = type.list()
	local expl = type.list()
	
	for _,v in iter(sua.fileref) do import_fileref(v) end
--	for _,v in iter(sua.anim) do import_anim(v) end
	for _,v in iter(sua.geometry) do import_trimesh(v) end
	for _,v in iter(sua.node) do import_node(dpyl,v) end

	push_back_list( dpyl, 'display' )
	push_back_list( expl, 'export' )

	-- end tag
	coll:push_back( tag	'rsc.common.end' )
	return coll
end




function generate_pak_from_sua ( in_pakfile, in_suafile )

--	test_model()

	local m1 = profiler.startmeasure( 'generating collection' )
	local coll = generate_collection_from_sua( in_suafile )
	profiler.getmeasure( m1 )
	assert( coll )

--	printTable( coll )

	local m2 = profiler.startmeasure( 'binarizing collection' )
	local map, ctxt = su.util.binarizeMap( coll )
	if map==nil then
		printTable( ctxt, "Binarize error" )
		return
	end
	profiler.getmeasure( m2 )

	local pak = su.util.openPak( in_pakfile , true )
	assert( pak )

	pak:startUpdating()
	pak:updateContent( 1, 0x1b7d71d1, map )
	pak:endUpdating()

	return in_pakfile
end





