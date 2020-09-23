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
--                       ---------
--                       |       |
--             observed  >------->  updated
--                       |       |
--                       ---------
--
--
-- table_observed: is an array of strings, corresponding to a list of table names to observe against any modification
-- table_updated: outputed list of table names wich have changed
--





require 'datablock'
require 'weakref'


dbobserver_blocks = weakref.ktable()




local function _oncreate ( _def, _bk )
	assert( _bk )
	dbobserver_blocks[ _bk ] = 1
	_bk.hooked = {}
end



local function _compute ( _oplug )
	local bk = _oplug.block
	assert( _oplug == bk.updated )

	local olist = bk.observed:getvalue()
	local res = {}

	if istable(olist) and istable(bk.hooked) then
		local k=1
		for _,v in pairs(olist) do
			if bk.hooked[v] then
				res[k] = v
				k = k+1
			end
		end
	end

	bk.hooked = nil
	return res
end



function register_dbobserver ( )
	local def = datablock.definition( _compute, 'dbobserver' )
	def.onCreate = _oncreate
	def:affect ('observed','updated')
end



function notify_dbobserver ( updtated_tables )

	local function update_obs ( _b )
		_b.hooked = _b.hooked or {}
		for _,t in pairs(updtated_tables) do
			_b.hooked[t] = 1
		end
		_b.updated:setdirty()
	end

	if istable(updtated_tables) then
		for b,_ in pairs(dbobserver_blocks) do
			update_obs( b )
		end
	end
end



