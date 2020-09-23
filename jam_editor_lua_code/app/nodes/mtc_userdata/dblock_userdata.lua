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





--
--
--                  ---------
--                  |       |
--          files   >---\   |
--                  |    ---> map
--          content >---/   |
--                  |       |
--                  ---------
--
--
-- files	: list of filenames referencing content stored on disk
-- content	: content in memory to binarize to the map
-- map		: lua map
--


require 'type'
require 'datablock'






local function _compute ( _oplug )
	local bk = _oplug.block
	assert( _oplug == bk.map )

	local m = type.list()

	local content = bk.content:getvalue()
	if content then
		for _,v in pairs(totable(content)) do
			if isbinary(v) then
				local ud = { tagid='rsc.userdata.content' }
				ud.label = { len=0 }
				ud.sourcefile = { len=0 }
				ud.data = { size=size(v), bytes=v }
				m:push_back( ud )
			end
		end
	end

	local files = bk.files:getvalue()
	if files then
		for _,f in pairs(totable(files)) do
			if su.io.isFile(f) then
				local bytes = su.io.loadBinary(f)
				if isbinary(bytes) then
					local ud = { tagid='rsc.userdata.content' }
					local label = su.io.getBasename(f)
					local file = su.io.getNormalized(f)
					ud.label = { len=#label, text=label }
					ud.sourcefile = { len=#file, text=file }
					ud.data = { size=size(bytes), bytes=bytes }
					m:push_back( ud )
				end
			end
		end
	end

	return m
end




function register_udata ( )
	local def = datablock.definition( _compute, 'userdata' )
	def:affect ('files','map') ('content','map')
end





