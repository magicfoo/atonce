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

lw = new 'MLuaWindows'

atbase_path = lw:getEnv( "ATBASE" )
virtual_atbase_path = su.io.getVirtualPath( atbase_path )
sufw_data_path = virtual_atbase_path .. '/sufw-data/'

assert( su.io.isDirectory(sufw_data_path) )

fbxfile = sufw_data_path .. 'samples/sample/cube.FBX'
suafile = sufw_data_path .. 'samples/sample/acube.sua'


print( fbxfile )
assert( su.io.isFile(fbxfile) )

print 'lancement FbxLoader'
fbxloader = new 'MFbxDocument'
assert( fbxloader:openDocument(fbxfile) )

fbx = fbxloader:load( fbxfile )
printTable( fbx )
su.io.dumpStream( suafile, fbx )
fbx_as_sua = su.io.loadStream( suafile )

	
print( 'NodeNumber'..fbxloader.ET_Node )
nbNodes = fbxloader:count( fbxloader.ET_Node )
print( 'NbNodes : '..nbNodes );
listNodes = fbxloader:list( fbxloader.ET_Node )
print 'Nodes list'
printTable( listNodes )

--for k,v in pairs(listNodes) do
--	node = fbxloader:fetch( fbxloader.ET_Node, v )
--	print( 'node'..v )
--	printTable( node )
--end


nbMeshes = fbxloader:count( fbxloader.ET_Trimesh )
print( 'NbMeshes : '..nbMeshes );
listMeshes = fbxloader:list( fbxloader.ET_Trimesh )
print 'Meshes list'
printTable( listMeshes )

--for k,v in pairs(listMeshes) do
--	mesh = fbxloader:fetch( fbxloader.ET_Trimesh, v )
--	print( 'mesh'..v )
--	printTable( mesh )
--end


nbFilerefs = fbxloader:count( fbxloader.ET_FileRef )
print( 'NbFilerefs : '..nbFilerefs );
listFilerefs = fbxloader:list( fbxloader.ET_FileRef )
print 'Filerefs list'
printTable( listFilerefs )

--for k,v in pairs(listFilerefs) do
--	fileref = fbxloader:fetch( fbxloader.ET_FileRef, v )
--	print( 'fileref'..v )
--	print( fileref )
--end



nbMats = fbxloader:count( fbxloader.ET_Material )
print( 'NbMaterials : '..nbMats );
listMats = fbxloader:list( fbxloader.ET_Material )
print 'Materials list'
printTable( listMats )

--for k,v in pairs(listMats) do
--	material = fbxloader:fetch( fbxloader.ET_Material, v )
--	print( 'material'..v )
--	printTable( material )
--end



nbGeoms = fbxloader:count( fbxloader.ET_Geometry )
print( 'NbGeometries : '..nbGeoms );
listGeoms = fbxloader:list( fbxloader.ET_Geometry )
print 'Geometries list'
printTable( listGeoms )

--for k,v in pairs(listGeoms) do
--	geom = fbxloader:fetch( fbxloader.ET_Geometry, v )
--	print( 'geometry'..v )
--	printTable( geom )
--end


nbHelpers = fbxloader:count( fbxloader.ET_Helper )
print( 'NbHelpers : '..nbHelpers );
listHelpers = fbxloader:list( fbxloader.ET_Helper )
print 'Helpers list'
printTable( listHelpers )

--for k,v in pairs(listHelpers) do
--	helper = fbxloader:fetch( fbxloader.ET_Helper, v )
--	print( 'helper'..v )
--	printTable( helper )
--end


nbCameras = fbxloader:count( fbxloader.ET_Camera )
print( 'NbCameras : '..nbCameras );
listCameras = fbxloader:list( fbxloader.ET_Camera )
print 'Cameras list'
printTable( listCameras )

--for k,v in pairs(listCameras) do
--	camera = fbxloader:fetch( fbxloader.ET_Camera, v )
--	print( 'camera'..v )
--	printTable( camera )
--end


nbLights = fbxloader:count( fbxloader.ET_Light )
print( 'NbLights : '..nbLights );
listLights = fbxloader:list( fbxloader.ET_Light )
print 'Lights list'
printTable( listLights )

--for k,v in pairs(listLights) do
--	light = fbxloader:fetch( fbxloader.ET_Light, v )
--	print( 'light'..v )
--	printTable( light )
--end




