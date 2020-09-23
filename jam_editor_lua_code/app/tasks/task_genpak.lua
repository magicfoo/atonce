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


require 'profiler'
require 'type'
require 'map'

--/task:genpak /"C:\work\prog\sufw-data\samples\sample" /test.pak
-- options
--	/PVRTC


local logTable = {}
local logIdx = 1
local errfile = ''
local logfile = ''


function log_to_file( inLog, inDoLog )
	print( inLog )
	if inDoLog then
		logTable[logIdx] = inLog 
		logIdx = logIdx + 1
		su.io.dumpStream( errfile, logTable )
	end
end


function assert_or_log( inCondition, inLog )
	if not inCondition then
		log_to_file( 'ERR '..inLog, true )
		assert( nil )
		return nil
	end
	return 1
end



function gen_glmip( inAuthBmp, inCompressed, inCompressBpp )
	local mipGl = nil

	if inCompressed then
		local genData = inAuthBmp:generatePVRTCMip( inCompressBpp );

		if genData then
			mipGl = map.tag 'bitmap.pvrmip'

			if inCompressBpp == 4 then
				mipGl.format = 1
			else
				mipGl.format = 0
			end

			mipGl.size	= size( genData )
			mipGl.data	= genData
		end
	else
		local gl_fmt = {
			GL_RGB		 = 0x1907,
			GL_RGBA		 = 0x1908,
			GL_LUMINANCE = 0x1909
		}
		local gl_type = {
			GL_UNSIGNED_BYTE = 0x1401
		}

		local genData = inAuthBmp:generateGlMip( gl_fmt['GL_RGBA'], gl_type['GL_UNSIGNED_BYTE'] );

		if genData then
			mipGl = map.tag 'bitmap.glmip'
			mipGl.format= gl_fmt['GL_RGBA']
			mipGl.type	= gl_type['GL_UNSIGNED_BYTE']
			mipGl.size	= size( genData )
			mipGl.data	= genData
		end
	end
	return mipGl
end

function gen_dxmip ( inAuthBmp )

	local dx_fmt = {
		D3DFMT_R8G8B8	= 20,
		D3DFMT_A8R8G8B8	= 21,
	}

	local genData = inAuthBmp:generateDx9Mip( dx_fmt['D3DFMT_A8R8G8B8'] );
	assert( genData )

	local mipDx9 = nil
	if genData then
		mipDx9 = map.tag 'bitmap.dxmip'
		mipDx9.format	= dx_fmt['D3DFMT_A8R8G8B8']
		mipDx9.size		= size( genData )
		mipDx9.data		= genData
	end

	return mipDx9
end

function import_bitmap ( inAuthBmp, inColl, inCompress )
	
	local bmp = map.tag 'bitmap'
	bmp.width	= inAuthBmp:width()
	bmp.height	= inAuthBmp:height()
	bmp.wpad	= 0
	bmp.hpad	= 0

	local astatusA = {
		AS_OPAQUE		= 0,
		AS_PUNCHTHROUGH	= 1,
		AS_VARIED		= 2
	}
	if inAuthBmp:hasAlpha() then
		bmp.astatus	= astatusA['AS_VARIED']
	else
		bmp.astatus	= astatusA['AS_OPAQUE']
	end

	inColl:push_back( bmp )

		
	local bGenMipMap	= true
	local bGenDx		= false
	local bGenGl		= true
	local bCompress		= inCompress
	local compressBpp	= 4
	
	
	if bGenMipMap then
		while 1 do
			if bGenGl then	inColl:push_back( gen_glmip( inAuthBmp, bCompress, compressBpp ) )	end
			if bGenDx then	inColl:push_back( gen_dxmip( inAuthBmp ) )							end

			if ( ( inAuthBmp:width() <= 16 ) or ( inAuthBmp:height() <= 16 ) )
			then
				break
			end
			inAuthBmp:resizeByTwo()
		end
	else
		if bGenGl then	inColl:push_back( gen_glmip( inAuthBmp, bCompress, compressBpp ) )	end
		if bGenDx then	inColl:push_back( gen_dxmip( inAuthBmp ) )							end
	end
end





function generateCollectionFromFolder ( inFolder, inCompress )

	local file
	local fbxbool = false
	local objbool = false
	local table = su.io.listDirectory( inFolder )

	for _,v in pairs(table) do
		if su.io.hasExtension(v,'fbx') then
			file = inFolder..'/'..v
			fbxbool = true
		end
	end

	for _,v in pairs(table) do
		if su.io.hasExtension(v,'sua') then
			file = inFolder..'/'..v
		end
	end

	for _,v in pairs(table) do
		if su.io.hasExtension(v,'obj') then
			file = inFolder..'/'..v
			objbool = true
		end
	end

	local sua	
	assert_or_log( file, 'Missing file' )
	print( 'file '..file )


	if fbxbool then
		log_to_file( 'generating fbx', false )
		local fbx = new 'MFbxDocument'
		assert( fbx )
		local err = fbx:openDocument( file )
		assert_or_log( err, 'Fbx file corrupted' )
		sua = fbx:load( )
		assert_or_log( sua, 'Failed to load fbx' )
	elseif objbool then
		log_to_file( 'generating obj', false )
		local objstream = new 'MObjDocument'
		assert( objstream )
		sua = objstream:load( file )
		assert_or_log( sua, 'Failed to load obj' )
	else
		log_to_file( 'generating sua', false )
		sua = su.io.loadStream( file )
		assert_or_log( sua, 'Failed to load sua' )
	end

	--printTable( sua )
	if sua['logs'] then
		for i,v in sua['logs'] do
			log_to_file( 'WARNING '..sua['logs'][i], false )
		end
	end
	
	assert_or_log( sua['error'] == nil, sua['error'] )

	local coll = type.list()

	local defines = type.list()
	local ids = {}


	function timerange_rec ( inrange, intimes )
		inrange[1] = min( inrange[1], intimes[1] )
		inrange[2] = min( inrange[2], intimes[#intimes] )
		return inrange
	end


	function define ( obj, name )
		assert( obj )
		local id = defines:size() + 1
		local def = map.tag 'define'
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


	function import_fileref ( infref )

		baseName = su.io.getBasename( infref )
		vPath = inFolder .. '/' .. baseName

		log_to_file( 'generating fileref ' .. su.io.getBasename( infref ), false )
		local m1 = profiler.startmeasure( 'generating fileref ' .. su.io.getBasename( infref ) )

		local authBmp = new 'MAuthBitmap'
		if authBmp:loadFromFile( vPath ) then

			local def = define( infref )
			coll:push_back( def )
			import_bitmap( authBmp, coll, inCompress )
			
		else
			if not assert_or_log( baseName == 'JamFX.fx', 'File '..baseName..' referenced by fileref is missing' ) then return nil end
		end

		profiler.getmeasure( m1 )

		return true
	end


	function import_trimesh ( ingeom )

		inmesh = ingeom.mesh

		
		local name
		if ingeom.name then 
			name = ingeom.name
		else
			name = ''
		end

		assert_or_log( inmesh, 'Mesh missing in Geometry '..name )

		local locations
		locations = type.v3list(inmesh['vtx-location'])
		locations:swizzle_xzy()

		local uv0, uv1, uv2
		if inmesh['face-vtx-tex']			then uv0 = type.v2list(inmesh['face-vtx-tex']) end
		if inmesh['face-vtx-tex-channel-2']	then uv1 = type.v2list(inmesh['face-vtx-tex-channel-2']) end
		if inmesh['face-vtx-tex-channel-3']	then uv2 = type.v2list(inmesh['face-vtx-tex-channel-3']) end


		if not assert_or_log( uv0, 'UV missing in Mesh '..name ) then return nil end

		local normals
		if inmesh['face-vtx-normal'] then
			normals = type.v3list(inmesh['face-vtx-normal'])
			normals:swizzle_xzy()
		end
		
		assert_or_log( inmesh['face-vtx-location'], 'Face vertex location missing in mesh' )
		assert_or_log( inmesh['face-mat'], 'Face material missing in mesh' )
		local facecnt = #inmesh['face-vtx-location'] / 3
		assert_or_log( #inmesh['face-mat'] == facecnt, 'Face material not equal to nb faces ( Faces must be triangles )' )

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
				if normals then f[i].n = normals:get_xyz(j+i) end
			end
			return f
		end

		local def = define(ingeom)
		local mesh = map.tag 'mesh'
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
		if normals then vf=vf+2^6; stride=stride+3*4 end		-- VF_NRM
																-- VF_TAN
																-- VF_BIN
		

		local mats	= collect_mat()
		local ibpos = 0
		local vbpos = 0

		local vboidx = 0

		for mati,matf in iter(mats) do
		
			log_to_file( 'generating surface', false )
			local m1 = profiler.startmeasure( 'generating surface' )

			-- collect texids
			texids = {}

			cmat = nil
			if ingeom.material then
				if ingeom.material.type == 'multi/sub-object' then
					matSub = ingeom.material['sub']
					assert_or_log( matSub, 'Missing sub material in multi material' )
					cmIdx = matSub[mati+1] + 1
					if cmIdx ~= 0 then
						cmat = sua.material[cmIdx]
					end
				else
					cmat = ingeom.material
				end
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

			local splitA

			if (#matf)*3 >= 65535 then

				local half = (#matf) / 2
				if half*3 >= 65535 then
					print( "warning : too many vertex geometry " .. ingeom.name .. ", surface " .. mati .. "."  )
				end

				splitA = {}
				splitA[1] = {}
				splitA[2] = {}
				for fi=1,half do
					splitA[1][fi] = matf[fi]
					splitA[2][fi] = matf[fi+half]
				end

			else
				splitA = { matf }
			end

			for i,faceA in iter(splitA) do
				-- vbo for surface
				local vbo = map.tag 'mesh.vbo'
				vbo.vf			= vf
				vbo.stride		= stride
				vbo.primtype	= 2													-- PRIM_TRIANGLE
				vbo.dynamic		= 0
				vbo.count		= (#faceA)*3

				vbo.data		= {}
				vbo.data.size	= vbo.count*vbo.stride
				vbo.data.bytes	= su.util.newBlob( vbo.data.size )
		
				vbo.kick		= {}
				vbo.kick.size	= vbo.count
				vbo.kick.bits	= {}

				vb_curs = su.util.newCursor( vbo.data.bytes )
				coll:push_back( vbo )
			
			

				-- surf
				local surf = map.tag 'mesh.surface'
				surf.vboidx = vboidx
				surf.effectid = 0
				surf.techniqueidx = 0
				surf.shdparams = map.rec.string('a')
				surf.texids = { count=#texids, ids=texids }
				coll:push_back( surf )

				vboidx = vboidx+1

				-- update vb & ib
				for k,v in iter(faceA) do
					local f = get_face( v )
					for i=1,3 do
						-- swap one triangle on two for stripping
						local ii = i
						if k%2 == 1 then
							if (i-1)%3 == 1 then	ii = i+1 end
							if (i-1)%3 == 2 then	ii = i-1 end
						end


						assert_or_log( f[ii], 'Face missing in Mesh '..name )
						assert_or_log( f[ii].l, 'Vertex location missing in mesh '..name )
						vb_curs:writef32( unpack(f[ii].l) )
						if uv0 then 
							if not assert_or_log( f[ii].uv0, 'Face vertex texture uv0 missing in Mesh '..name ) then return nil end
							vb_curs:writef32( unpack(f[ii].uv0) ) 
						end
						if uv1 then 
							if not assert_or_log( f[ii].uv1, 'Face vertex texture uv1 missing in Mesh '..name ) then return nil end
							vb_curs:writef32( unpack(f[ii].uv1) ) 
						end
						if uv2 then 
							if not assert_or_log( f[ii].uv2, 'Face vertex texture uv2 missing in Mesh '..name ) then return nil end
							vb_curs:writef32( unpack(f[ii].uv2) ) 
						end
						if normals then vb_curs:writef32( unpack(f[ii].n) ) end
					end
				
					vbo.kick.bits[#vbo.kick.bits+1] = 0
					vbo.kick.bits[#vbo.kick.bits+1] = 0
					vbo.kick.bits[#vbo.kick.bits+1] = 1
				end
			end

			profiler.getmeasure(m1)
		end

		return def
	end


	function import_node ( inl, innode )
		local n = map.tag 'litem.node'
		n.parentidx = innode.parent or -1
		n.name = map.rec.string( innode.name )
		n.note = map.rec.string( innode.note )
		n.hidden = innode.hidden or false
		n.trs = map.rec.matrix( innode.location, innode.rotation, innode.scale )
		n.trsanimid = objectid(innode.anim)

		local t = innode.type or 'node'
		
		if t=='geom' then
			local g = find_by_node(innode,sua.geometry)
			if g then
				n.tagid = 'rsc.collection.litem.geometry'
				n.meshid = objectid(g)
				n.offset = map.rec.matrix( g['offset-location'], g['offset-rotation'], g['offset-scale'] )
				n.color = map.rec.color( g.color )
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

		return true
	end


	function push_back_list ( inl, inname )
		if not inl:empty() then
			local li = map.tag 'list'
			li.name = map.rec.string( inname )
			li.count = inl:size()
			coll:push_back( li )
			--
			for _,v in pairs(inl) do coll:push_back(v) end
		end
	end


	local dpyl = type.list()
	local expl = type.list()
	

	for _,v in iter(sua.fileref) do 
		if not import_fileref(v) then return nil end
	end
	

	for _,v in iter(sua.geometry) do 
		if not import_trimesh(v) then return nil end
	end
	

	for _,v in iter(sua.node) do 
		if not import_node(dpyl,v) then return nil end
	end
	

	push_back_list( dpyl, 'display' )
	push_back_list( expl, 'export' )

	-- end tag
	coll:push_back( map.tag 'rsc.common.end' )
	
	
	assert_or_log( coll, 'Collection generation failed' )
	return coll

end

function loadRscArchive ( infilename )
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


function get_2pow_up( inVal )
	local nv = 1
	while ( inVal > 1 ) do
		inVal = inVal/2
		nv = nv * 2
	end
	return nv
end

function generateTitleBitmap( inBmpPath )
	local coll = type.list()

	baseName = su.io.getBasename( inBmpPath )

	log_to_file( 'generating title ' .. baseName, false )

	local authBmp = new 'MAuthBitmap'
	if authBmp:loadFromFile( inBmpPath ) then
		local def = define( inBmpPath )
		coll:push_back( def )

		local nw = get_2pow_up( authBmp:width() )
		local nh = get_2pow_up( authBmp:height() )
		authBmp:expand( nw, nh )

		import_bitmap( authBmp, coll, false )
	end

	return coll
end

function generatePakFromFolder( inPakfile, inFolder, inCompress )

	log_to_file( 'generating collection', false )
	local m1 = profiler.startmeasure( 'generating collection' )

	local coll = generateCollectionFromFolder( inFolder, inCompress )
	profiler.getmeasure( m1 )
	
	local titlePath = inFolder .. '/title.png'


	local collTitle = nil
	if su.io.isFile( titlePath ) then
		collTitle = generateTitleBitmap( titlePath )
	end


	local m2 = profiler.startmeasure( 'binarizing collection' )
	local map, ctxt = su.util.binarizeMap( coll )
	assert_or_log( map, 'Binarize error' )
	if map==nil then
		printTable( ctxt, "Binarize error" )
		return
	end
	profiler.getmeasure( m2 )

	local pak = su.util.openPak( inPakfile , true )
	assert_or_log( pak, 'Cant open pak '..inPakfile )

	pak:startUpdating()
	pak:updateContent( 1, 0x1b7d71d1, map )
	if collTitle then
		local mapTitle, ctxtTitle = su.util.binarizeMap( collTitle )
		assert_or_log( map, 'Binarize error' )
		if mapTitle==nil then
			printTable( ctxtTitle, "Binarize error" )
		else
			pak:updateContent( 2, 0x1b7d71d1, mapTitle )
		end
		
	end
	pak:endUpdating()

	return inPakfile


end

print( 'task_genpak' )

local splitStr = su.string.split( taskargs, ' ' )


local fileDroped = splitStr[2]
if su.string.startsWith(fileDroped,'/') then
	fileDroped = su.string.right( fileDroped, 2 )
end
fileDroped = su.string.replace( fileDroped, '"', '')

local vpath = su.io.getVirtualPath( fileDroped )
if vpath == nil then
	vpath = fileDroped
end

local filePak = splitStr[3]
if su.string.startsWith(filePak,'/') then
	filePak = su.string.right( filePak, 2 )
end
filePak = su.string.replace( filePak, '"', '')


-- get options
local genPVRTC = false

local cIdxOpt = 4
while 1 do
	local copt = splitStr[cIdxOpt]
	if copt == nil then
		break
	end

	if copt == '/PVRTC' then
		genPVRTC = true
	end

	cIdxOpt = cIdxOpt + 1
end

vpathPak = vpath..'/'..filePak


local baseName = su.io.getBasename( filePak )
errfile = vpath..'/'..filePak.. '.log'
logfile = vpath..'/su_console.log'
print( errfile )
-- su.util.setConsoleFile( logfile )
--log_to_file( vpath )
--log_to_file( vpathPak )
--logTable[1] = vpath
--logTable[2] = vpathPak
--su.io.dumpStream( errfile, logTable )
log_to_file( 'begin...', false )

generatePakFromFolder( vpathPak, vpath, genPVRTC )

log_to_file( '...end ', false )


