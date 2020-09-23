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

include 'dblock_userdata.lua'


local usdata_network =
[[
	p gen_settings { platform='generic' }

	b db dbsolver
	b udata	userdata gen_settings

	> (db.result udata.files)
]]



function udata_network_generate ( net )
	assert( net.udata )
	local platform = net.udata:property('platform')
	return net.udata.map:getvalue()
end




function mtc_get_name ( )
	return 'User-Data'
end


function mtc_realize ( )
	local n = datablock.compose( usdata_network )
	n.generate = udata_network_generate
	return n
end


function onInitialize( inh )
	inh.node.mtcGetName = mtc_get_name
	inh.node.mtcRealize = mtc_realize

	register_udata()
end




