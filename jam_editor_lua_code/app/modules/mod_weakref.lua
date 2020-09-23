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




local vtmt = { __mode='v' }
local ktmt = { __mode='k' }


function vtable ( )
	local t = {}
	setmetatable( t, vtmt )
	return t
end


function ktable ( )
	local t = {}
	setmetatable( t, ktmt )
	return t
end







local gtx = { }
local gtmt = { __mode='k', __index=gtx }


function gtx:garbage ( )
	collectgarbage()
end


function gtx:assert_garbaged ( )
	collectgarbage()
	assert( #self == 0 )
end


function table ( ... )
	local t = {}
	setmetatable( t, gtmt )

	local args = pack(...)
	for k,v in pairs(args) do
		t[#t+1] = v
	end

	return t
end




