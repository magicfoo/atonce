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


local bmx = { __type='bitmask' }
local bmmt = { __index=bmx }


local function _check_enumeration ( _e )
	assert( istable(_e) )
	local unicity = {}
	local i=1
	for k,v in pairs(_e) do
		assert( isinteger(k) )
		assert( k==i )
		assert( isstring(v) )
		assert( unicity[v]==nil )
		unicity[v] = 1
		i=i+1
	end
end


local function _check_member ( _e, _m )
	assert( isstring(_m) or isinteger(_m) )
	if isstring(_m) then
		for k,v in pairs(_e) do
			if v==_m then
				return k,v		-- int, string
			end
		end
	else
		assert( _m > 0 )
		assert( _m <= #_e )
		assert( _e[_m] )
		return _m, _e[_m]		-- int, string
	end
end





function bmx:member ( _m )
	return _check_member( self.__enum, _m )
end


function bmx:test ( _m )
	local i,s = _check_member( self.__enum, _m )
	assert( s )
	return self[s] ~= nil
end


function bmx:set ( _m )
	local i,s = _check_member( self.__enum, _m )
	assert( s )
	self[s] = i
end


function bmx:clear ( _m )
	local i,s = _check_member( self.__enum, _m )
	assert( s )
	self[s] = nil
end


function bmx:clearall ( _m )
	for k,_ in pairs(self) do
		if k~='__enum' then
			self[k] = nil
		end
	end
end


function bmx:numerical ( )
	local ui = 0
	for k,v in pairs(self) do
		if k~='__enum' then
			ui = ui + 2^(v-1)
		end
	end
	return ui
end


function create ( _enumeration )
	_check_enumeration( _enumeration )
	local m = {}
	m.__enum = _enumeration
	setmetatable( m, bmmt )
	return m
end



