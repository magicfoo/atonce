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



require 'datablock'
require 'stream'



--[[


    i1 #-------# o1
       |     / |
    i2 #----/  |
       |  B1   |
    i3 #-------# o2 .............. i1 #-------# o1 ........... i1 #-------# o1
                    \                 |     / |     \             |     / |
                     ............. i2 #----/  |       ........ i2 #----/  |
                                      |  B3   |                   |  B4   |
    i1 #-------# o1 .............. i3 #-------# o2 ........... i3 #-------# o2
       |     / |
    i2 #----/  |
       |  B2   |
    i3 #-------# o2


--]]






--
-- class definition

local function getvalue1 ( inp )
	-- junction support
	local v = inp:getvalue()
	if v then
		assert( not istable(v) or inp:iscomplex() )
		local r = 0
		for _,i in pairs(totable(v)) do r=r+i end
		return r
	end
end

local function compute1 ( _oplug )
	local bk = _oplug.block
	local defvalue = bk:property('defvalue') or 0
	if _oplug==bk.o1 then
		-- o1 = i1+i2
		local i1 = getvalue1(bk.i1) or defvalue
		local i2 = getvalue1(bk.i2) or defvalue
		assert( isinteger(i1) )
		assert( isinteger(i2) )
		return i1 + i2
	elseif _oplug==bk.o2 then
		-- o2 = -i3
		local i3 = getvalue1(bk.i3) or defvalue
		assert( isinteger(i3) )
		return -i3
	end
end

local def1_props = datablock.properties()
def1_props:set('defvalue',0)

local def1 = datablock.definition( compute1, 'tunit', def1_props )
def1:affect ('i1','o1') ('i2','o1') ('i3','o2')



--
-- build the model as a network of blocks

local user_props = datablock.properties()
user_props:set( 'basepath', '/data/project2/meshes' )
user_props:set( 'radius', 6 )
user_props:set( 'nedges', 16 )

local b1 = datablock.block(def1,user_props)
local b2 = datablock.block(def1,user_props)
local b3 = datablock.block(def1)
local b4 = datablock.block(def1)

datablock.connect (b1.o2,b3.i1) (b1.o2,b3.i2) (b2.o1,b3.i3)
datablock.connect (b3.o1,b4.i1) (b3.o1,b4.i2) (b3.o2,b4.i3)



--
-- checks dirty flag propagation

assert(	b1.o1:isdirty() )
assert(	b1.o2:isdirty() )
assert(	b2.o1:isdirty() )
assert(	b2.o2:isdirty() )
assert( b3.o1:isdirty() )
assert( b3.o2:isdirty() )
assert( b4.o1:isdirty() )
assert( b4.o2:isdirty() )

b1.i1:setdirty()
assert( b1.o1:isdirty() )
assert( b1.o2:isdirty() )

b1.i2:setdirty()
assert( b1.o1:isdirty() )
assert( b1.o2:isdirty() )

b1.i3:setdirty()
assert( b1.o2:isdirty() )
assert( b3.o1:isdirty() )
assert( b4.o1:isdirty() )
assert( b3.o2:isdirty() )

b2.i1:setdirty()
assert( b4.o2:isdirty() )



--
-- checks evaluation

b1.i1:setvalue( 1 )
b1.i2:setvalue( 2 )
b1.i3:setvalue( 3 )
assert( b1.o1:getvalue() ==  3 )
assert( b1.o2:getvalue() == -3 )
assert( b3.o1:getvalue() == -6 )
assert( b4.o1:getvalue() == -12 )

b2.i1:setvalue( 5 )
b2.i2:setvalue( -3 )
assert( b2.o1:getvalue() == 2 )
assert( b4.o2:isdirty() )
assert( b4.o2:getvalue() == 2 )
assert( not b4.o2:isdirty() )



--
-- change default-value in properties

b2.i3:setvalue( nil )
assert( b2.o2:isdirty() )
assert( b2.o2:getvalue() == 0 )
assert( not b2.o2:isdirty() )

def1_props:set('defvalue',10)
def1_props:setdirty()

assert( b2.o2:isdirty() )
assert( b2.o2:getvalue() == -10 )

b2.i1:setvalue( nil )
b2.i2:setvalue( nil )
assert( b4.o2:getvalue() == 20 )

def1_props:set('defvalue',-10)
def1_props:setdirty()
assert( b4.o2:getvalue() == -20 )

def1_props:set('lenght',8)
assert( b2:property'lenght' == 8 )



--
-- network

local n1 = datablock.network()
n1:register( b1, 'b1' )
n1:register( b2, 'b2' )
n1:register( b3, 'b3' )
n1:register( b4 )
n1:register( user_props, 'props' )

assert( n1.b1 == b1 )
assert( n1.b2 == b2 )
assert( n1.b3 == b3 )
assert( n1[1] == b4 )

assert( n1:find(b1) == 'b1' )
assert( n1:find(b2) == 'b2' )
assert( n1:find(b3) == 'b3' )
assert( n1:find(b4) == 1 )



--
-- attributes

local attrs
attrs = n1:getattributes()
assert( attrs.b1.i1 == 1 )
assert( attrs.b1.i2 == 2 )
assert( attrs.b1.i3 == 3 )
assert( attrs.props.radius==6 )

b2.i1:setvalue( -4 )
attrs = n1:getattributes()
assert( attrs.b2.i1 == -4 )

local b4_o2 = b4.o2:getvalue()

n1:setattributes( {} )
n1:setdirty()
assert( b2.i1:getvalue() == nil )
assert( b2.i2:getvalue() == nil )
assert( b4.o2:getvalue() ~= b4_o2 )

n1:setattributes( attrs )
n1:setdirty()
assert( b4.o2:getvalue() == b4_o2 )



--
-- stream to/from lua

if 1 then
	local attrs_s = stream.presentLua( attrs )
	assert( isstring(attrs_s) )
	assert( #attrs_s > 10 )

	local attrs_t = stream.parseLua( attrs_s )
	assert( istable(attrs_t) )
	assert( attrs_t.b1.i1 == 1 )
	assert( attrs_t.b1.i2 == 2 )
	assert( attrs_t.b1.i3 == 3 )
	assert( attrs_t.props.radius==6 )
end





--
-- serialize / compose a random network

net_s = [[
	-- value declaration
	v cpt 5
	v files { 'f1.txt', 'f2.txt' }

	-- properties declaration
	p props { platform='psp2', weld=false }

	# declaration of blocks
	b b1 dummy
	b b2 dummy props
	b b3 dummy
	b b4 dummy

	# connection declaration
	> (b1.o2 b3.i1) (b1.o2 b3.i2) (b2.o1 b3.i3)
	>  b3.o1 b4.i1   b3.o1 b4.i2
	>  b3.o2 b4.i3

	// assignement declaration
	= b1.i1 7
	= b1.i2 cpt
	= b2.i3 files
]]

net = datablock.compose( net_s )





--
-- serialize / compose the tunit network

n1_s = [[
	p user_props { basepath='/data/project2/meshes', radius=6, nedges=16 }

	b b1 tunit user_props
	b b2 tunit user_props
	b b3 tunit
	b b4 tunit

	> (b1.o2 b3.i1) (b1.o2 b3.i2) (b2.o1 b3.i3)
	> (b3.o1 b4.i1) (b3.o1 b4.i2) (b3.o2 b4.i3)
]]

n1_attrs = n1:getattributes()

_n1 = datablock.compose( n1_s )
_n1:setattributes( n1_attrs )

assert( _n1.b3.o1:getvalue() == n1.b3.o1:getvalue() )
assert( _n1.b3.o2:getvalue() == n1.b3.o2:getvalue() )




--
-- check junction

local b1 = datablock.block(def1)
local b2 = datablock.block(def1)

datablock.connect (b1.o1,b2.i1) (b1.o2,b2.i1)
assert( b2.i1:isconnected() )
assert( b2.i1:iscomplex() )

b1.i1:setvalue( 1 )
b1.i2:setvalue( 2 )
b1.i3:setvalue( 3 )
b2.i2:setvalue( 4 )
b2.i3:setvalue( 5 )

assert( b1.o1:getvalue() == 3 )
assert( b1.o2:getvalue() == -3 )
assert( b2.o1:getvalue() == 1+2-3+4 )
assert( b2.o2:getvalue() == -5 )

b1.i1:setvalue( 0 )
assert( b2.o1:getvalue() == 0+2-3+4 )


