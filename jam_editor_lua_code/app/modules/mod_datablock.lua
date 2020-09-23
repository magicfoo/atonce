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
require 'stream'




--[[


	properties
	{
		blocks			: { block:1 }
		attributes		: {}

		setdirty		( )

		set				( attrkey:*, attrvalue:* )
		get				( attrkey:* )

		attach			( block )
		detach			( block )
	}



	definition
	{
		regname			: string
		compute			: function												-- return the new value
		iplugs			: { string:plug }
		oplugs			: { string:plug }
		properties		: list<properties>

		plug			( name:string )

		input			( name:string )
		output			( name:string )

		affect			( inplug-name:string, outplug-name:string )				-- support call-chaining, i.e. affect (i,o) (i,o) (i,o) ...

		onCreate		( block )
	}



	inplug
	{
		block			: block
		name			: string
		data			: *					-- data XOR source !
		source			: outplug			-- data XOR source !

		getsource		( )
		isconnected		( )
		issimple		( )					-- one data or connected to a block
		iscomplex		( )					-- connected to a junction
		setdirty		( )

		setdata			( )
		getdata			( )

		setvalue		( )
		getvalue		( )
	}



	outplug
	{
		block			: block
		name			: string
		dirty			: boolean
		value			: *
		targets			: list<inplug>

		isconnected		( )
		isdirty			( )
		setdirty		( )

		getvalue		( )
		update			( )
	}



	block
	{
		def				: definition
		properties		: list<properties>
		[plug1]			: plug
		[plug2]			: plug
		[...]			: plug

		setdirty		( )
		isdirty			( name:string )
		alldirty		( [names] )
		onedirty		( [names] )
		property		( key:* )
	}



	junction
	{
		[in1]			: inplug
		[in2]			: inplug
		[...]			: inplug
		[out]			: outplug

		setdirty		( )
	}



	network
	{
		[block1]		: block
		[block2]		: block
		[...]			: block

		[props1]		: properties
		[props2]		: properties
		[...]			: properties

		setdirty		( )

		register		( b:block [,name:*] ) : name
		unregister		( name:* )
		find			( b:block )

		getblocks		( ) : list<block>
		getinputs		( ) : list<plug>

		getproperties	( ) : list<properties>
		findproperties	( ) : list<properties>

		getattributes	( ) : {}
		setattributes	( {} )
	}



	properties			( )

	definition			( compute:function [,regname:string] [,properties, properties, properties] )
	getdefinition		( regname:string )

	junction			( )
	switch				( )

	block				( def:definition [,properties, properties, properties] )
	block				( def:string     [,properties, properties, properties] )

	network				( )

	connect				( start:outplug, end:inplug )					-- support call-chaining: connect (op,ip) (op,ip) (op,ip) ...
	disconnect			( end:inplug )									-- support call-chaining: disconnect (ip) (ip) (ip) ... ]

	typeof				( obj )											-- returns 'properties' or 'definition' or 'block' or 'network' or 'inplug' or 'outplug' or 'junction'
	istype				( obj, type:string )							-- returns true/false

	serialize			( network ) : {}								-- TODO !
	compose				( serialization:{} ) : network


--]]







--
-- block properties


local propx = { __type='properties' }
local propmt = { __index=propx }


function propx:setdirty ( )
	for k,_ in pairs(self.blocks) do
		k:setdirty()
	end
end


function propx:set ( _attrkey, _attrvalue )
	self.attributes[_attrkey] = _attrvalue
end


function propx:get ( _attrkey )
	return self.attributes[_attrkey]
end


function propx:attach ( _bk )
	self.blocks[_bk] = 1
end


function propx:detach ( _bk )
	self.blocks[_bk] = nil
end


function properties ( )
	local p = {}
	p.blocks = {}
	p.attributes = {}
	setmetatable( p, propmt )
	return p
end






--
-- block definition


local defx = { __type='definition' }
local defmt = { __index=defx }
local reg_definitions = {}
local reserved_plugnames = { ['def']=1, ['properties']=1 }
local dummy_def = nil


function defx:plug ( _name )
	if self.iplugs[_name] then return 0 end
	if self.oplugs[_name] then return 1 end
end


function defx:input ( _name )
	assert( isstring(_name) )
	assert( su.string.isIdent(_name) )
	assert( not reserved_plugnames[_name] )
	assert( self.oplugs[_name] == nil )

	local p = self.iplugs[_name]
	if not p then	
		p = type.list()
		self.iplugs[_name] = p
	end
	return p
end


function defx:output ( _name )
	assert( isstring(_name) )
	assert( su.string.isIdent(_name) )
	assert( not reserved_plugnames[_name] )
	assert( self.iplugs[_name] == nil )

	local p = self.oplugs[_name]
	if not p then	
		p = type.list()
		self.oplugs[_name] = p
	end
	return p
end


function defx:affect ( _inname, _outname )
	-- internel-input => internal-output

	local function affect_chain( _i, _o )
		local ip = defx.input(self,_i)
		local op = defx.output(self,_o)
		assert( not ip:find(_o) )
		assert( not op:find(_i) )
		ip:push_back( _o )
		op:push_back( _i )
		return affect_chain
	end

	-- call chaining !
	return affect_chain( _inname, _outname )
end


function getdefinition ( _regname )
	assert( isstring(_regname) )
	return reg_definitions[_regname]
end


function definition ( ... )
	local args = pack(...)

	local computefunc
	local regname
	local proplist = type.list()

	for _,v in pairs(args) do
		if isfunction(v) then
			computefunc = v
		elseif istable(v) then
			proplist:push_back(v)
		elseif isstring(v) then
			regname = v
		end
	end

	-- compute function is of the form:
	--		compute ( _outplug )
	assert( isfunction(computefunc) )

	-- registration must be unique !
	regname = regname or (#reg_definitions+1)
	assert( reg_definitions[regname] == nil )

	local d = {}
	d.compute = computefunc
	d.regname = regname
	d.iplugs = {}
	d.oplugs = {}
	d.properties = proplist
	setmetatable( d, defmt )

	reg_definitions[regname] = d
	return d
end


if dummy_def==nil then
	dummy_def = definition( function() end, 'dummy' )
	dummy_def:affect ('i1','o1')
	dummy_def:affect ('i1','o2')
	dummy_def:affect ('i2','o1')
	dummy_def:affect ('i2','o2')
	dummy_def:affect ('i3','o3')
end




--
-- input plug


local ipx = { __type='inplug' }
local ipmt = { __index=ipx }


function ipx:getsource ( )
	return self.source
end


function ipx:isconnected ( )
	return (self.source ~= nil)
end


function ipx:issimple ( )
	return not self.source or self.source.block.__type=='block'
end


function ipx:iscomplex ( )
	return self.source and self.source.block.__type=='junction'
end


function ipx:setdirty ( )
	-- propagated to each affected outputs
	local bk = self.block
	if bk.def then
		local inl = bk.def.iplugs[self.name]
		for _,o in pairs(inl) do
			bk[o]:setdirty()
		end 
	else
		assert( bk.__type == 'junction' )
		bk:setdirty()
	end
end


function ipx:setdata ( _data )
	if self.source then
		-- an input-plug can't be both connected and holding some data !
		-- must be disconnected first !
		assert( _data==nil )
	else
		self.data = _data
	end
end


function ipx:getdata ( )
	if self.source then
		return nil
	else
		return self.data
	end
end


function ipx:setvalue ( _data )
	if self.data ~= _data then
		self:setdata( _data )
		self:setdirty()
	end
end


function ipx:getvalue ( )
	if self.source then
		return self.source:getvalue()
	else	
		return self.data
	end
end


local function new_inplug ( _bk, _name )
	local p = { block=_bk, name=_name, data=nil, source=nil }
	setmetatable( p, ipmt )
	return p
end





--
-- output plug


local opx = { __type='outplug' }
local opmt = { __index=opx }


function opx:isconnected ( )
	return not self.target:empty()
end


function opx:isdirty ( )
	return self.dirty
end


function opx:setdirty ( )
	if not self.dirty then
		self.dirty = true
		for _,c in pairs(self.targets) do
			c:setdirty()
		end
	end
end


function opx:update ( )
	if self.dirty then
		local bk = self.block

		if bk.def then
			-- for each sourced inputs
			local outl = bk.def.oplugs[self.name]
			for _,v in pairs(outl) do
				local inp = bk[v]
				-- for each source
				if inp.source then
					inp.source:update()
				end 
			end
			self.value = bk.def.compute( self )
			self.dirty = false

		else
			assert( bk.__type == 'junction' )
			-- for each dynamic inputs
			for _,inp in pairs(bk) do
				if inp.source then
					inp.source:update()
				end 
			end
			self.value = bk:compute()
			self.dirty = false
		end
	end
end


function opx:getvalue ( )
	self:update()
	return self.value
end


local function new_outplug ( _bk, _name )
	local p = { block=_bk, name=_name, dirty=true, value=nil, targets=type.list() }
	setmetatable( p, opmt )
	return p
end





--
-- block unit


local bkx = { __type='block' }
local bkmt = { __index=bkx }


function bkx:setdirty ( )
	for _,p in pairs(self) do
		if p.__type=='outplug' then
			p:setdirty()
		end
	end
end


function bkx:isdirty ( _name )
	assert( isstring(_name) )
	assert( self[_name] )
	assert( self[_name].__type=='outplug' )
	return self[_name].dirty
end


function bkx:alldirty ( _names )
	local names = totable(_names)
	local atleast_one = false
	for _,n in pairs(names) do
		assert( self[n] )
		assert( self[n].__type=='outplug' )
		atleast_one = true
		if not self[n].dirty then return false end
	end
	return atleast_one
end


function bkx:onedirty ( _names )
	local names = totable(_names)
	for _,n in pairs(names) do
		assert( self[n] )
		assert( self[n].__type=='outplug' )
		if self[n].dirty then return true end
	end
	return false
end


function bkx:property ( _key )
	-- user-defined properties
	for _,v in pairs(self.properties) do
		local p = v:get(_key)
		if p~=nil then return p end
	end

	-- inherited definition properties
	for _,v in pairs(self.def.properties) do
		local p = v:get(_key)
		if p~=nil then return p end
	end
end


function block ( ... )
	local args = pack(...)

	local def
	local proplist=type.list()
	for _,v in pairs(args) do
		if isstring(v) then
			if not def then
				def = getdefinition( v )
				assert( def )
			end
		elseif istable(v) then
			if v.__type=='definition' then
				def = v
			else
				proplist:push_back(v)
			end
		end
	end
	assert( def )

	local b = {}
	b.def = def
	b.properties = proplist

	-- create input plugs
	for k,_ in iter(def.iplugs) do
		assert( not b[k] )
		b[k] = new_inplug(b,k)
	end

	-- create output plugs
	for k,_ in iter(def.oplugs) do
		assert( not b[k] )
		b[k] = new_outplug(b,k)
	end

	-- attach definition's properties
	for _,v in iter(def.properties) do
		v:attach(b)
	end

	-- attach user-defined's properties
	for _,v in iter(b.properties) do
		v:attach(b)
	end

	setmetatable( b, bkmt )

	if isfunction(def.onCreate) then
		def:onCreate(b)
	end

	return b
end




--
-- junction unit


local jux = { __type='junction' }
local jumt = { __index=jux }


function jux:setdirty ( )
	self.out:setdirty()
end


function jux:compute ( )
	local o = {}
	for _,p in pairs(self) do
		if p.__type=='inplug' then
			o[#o+1] = p:getvalue()
		end
	end
	return o
end


function jux:free_input ( )
	local k=1
	for _,p in pairs(self) do
		if p.__type=='inplug' then
			if not p.source then
				return p
			end
			k=k+1
		end
	end

	local n = 'in'..k
	assert( self[n]==nil )

	self[n] = new_inplug(self,n)
	return self[n]
end


function junction ( )
	local j = {}

	-- create single output
	j.out = new_outplug(j,'out')

	setmetatable( j, jumt )
	return j
end




--
-- network of blocks


local netx = { __type='network' }
local netmt = { __index=netx }


function netx:register ( _item, _name )
	local regname
	if _name then
		assert( isstring(_name) )
		assert( su.string.isIdent(_name) )
		regname = _name
	else
		regname = #self+1
	end
	assert( self[regname] == nil )
	assert( not self:find(_item) )
	self[regname] = _item
	return regname
end


function netx:unregister ( _regname )
	assert( self[_regname] )
	self[_regname] = nil
end


function netx:find ( _item )
	for k,v in pairs(self) do
		if v==_item then
			return k
		end
	end
end


function netx:setdirty ( )
	for _,b in pairs(self) do
		if b.__type=='block' then
			b:setdirty()
		end
	end
end


function netx:getblocks	( )
	local t = {}
	for _,b in pairs(self) do
		if istype(b,'block') then t[b]=1 end
	end	
	return type.keys(t)
end


function netx:getinputs ( )
	local blocks = self:getblocks()

	local inputs = type.list()
	for _,b in pairs(blocks) do
		for _,p in pairs(b) do
			if p.__type=='inplug' then
				inputs:push_back(p)
			end
		end
	end

	return inputs
end


function netx:findproperties ( )
	-- find properties deeply in blocks themselves !
	local blocks = self:getblocks()

	local t = {}
	for _,b in pairs(blocks) do
		for _,p in iter(b.properties) do
			t[p] = 1
		end
	end

	return type.keys(t)
end


function netx:getproperties ( )
	-- get properties in network by keys
	local t = {}
	for _,v in pairs(self) do
		if v.__type=='properties' then t[v]=1 end
	end
	return type.keys(t)
end


function netx:getattributes	( )
	local attrs = {}

	for k,v in pairs(self) do
		if istype(v,'properties') then
			-- collect props attributes
			attrs[k] = v.attributes

		elseif istype(v,'block') then
			-- collect input-plug's data
			for _,p in pairs(v) do
				if p.__type=='inplug' then
					if p.source==nil and p.data then
						if not attrs[k] then attrs[k]={} end
						attrs[k][p.name] = p.data
					end
				end
			end
		end
	end
	return attrs
end


function netx:setattributes	( _attributes )

	for k,v in pairs(self) do
		if istype(v,'properties') then
			v.attributes = _attributes[k] or {}
		elseif istype(v,'block') then
			for _,p in pairs(v) do
				if p.__type=='inplug' then
					if p.source==nil then
						p.data = _attributes[k] and _attributes[k][p.name]
					end
				end
			end
		end
	end
	
end


function network ( )
	local n = {}
	setmetatable( n, netmt )
	return n
end





function typeof ( obj )
	return obj and istable(obj) and obj.__type
end


function istype ( obj, type )
	return typeof(obj) == type
end






function connect ( _outp, _inp )
	-- start:output => end:input
	assert( _outp.__type=='outplug' )
	assert( _outp.targets:find(_inp) == nil )
	assert( _inp.__type=='inplug' )
	assert( _inp.data == nil )

	if _inp.source==nil then
		-- not already connected
		_outp.targets:push_back( _inp )
		_inp.source = _outp
		_inp:setdirty()
		return connect

	elseif _inp.source.block.__type=='junction' then
		-- connected with a junction !
		-- => connect output to junction's free-input
		return connect (_outp, _inp.source.block:free_input())

	else
		-- connected with a block
		-- => insert a junction
		local o = _inp.source
		local j = junction()
		disconnect( _inp )
		return connect (j.out, _inp) (o, j:free_input()) (_outp, j:free_input())
	end
end



function disconnect ( _inp )
	local outp = _inp.source
	if outp then
		local i = outp.targets:find(_inp)
		assert( i )
		outp.targets:erase( i )
		_inp.source = nil
	end

	-- call-chaining !
	return disconnect
end







--[[
	-- some values
	v cpt 5
	v files { 'f1.txt', 'f2.txt' }

	-- a propertie object
	p props { platform='psp2', weld=false }

	# blocks
	b b1 dummy
	b b2 dummy props
	b b3 dummy
	b b4 dummy

	# connections
	> (b3.i1 b1.o2) (b3.i2 b1.o2) (b3.i3 b2.o1)
	> (b4.i1 b3.o1) (b4.i2 b3.o1) (b4.i3 b3.o2)

	// assignements
	= b1.i1 7
	= b1.i2 cpt
--]]



function serialize ( _network )
	--
end



function compose ( _serialization )
	if not _serialization or not isstring(_serialization) then
		return
	end

	local function _split( _l, _sep )
		local l = su.string.strip(_l)
		if not l then return end
		if su.string.startsWith(l,'--') then return end
		if su.string.startsWith(l,'#') then return end
		if su.string.startsWith(l,'//') then return end
		return su.string.split(l,_sep)
	end

	local function _value( _decl )
		local args = duptable(_decl); args[1]=nil; args[2]=nil
		local decl = su.string.join(args)
		return stream.parseLua( decl, {} )
	end

	local function _plug ( _decl, _net )
		local d = su.string.split(_decl,'.')
		if #d~=2 then print('invalid plug declaration '.._decl..' !'); return; end
		local b = _net[ d[1] ]
		if not b or not istype(b,'block') then print('invalid block '..d[1]..' !'); return; end
		local p = b[ d[2] ]
		if not p then print('invalid plug '..d[2]..' !'); return; end
		return p
	end


	local net = network()
	local lines = su.string.splitlines(_serialization)

	for _,l in pairs(lines) do
		local decl = _split(l)
		if decl then
			assert( istable(decl) )
			assert( #decl >= 3 )

			-- value declaration
			--		v toto 5
			--		v toto {...}
			if decl[1]=='v' then
				assert( #decl >= 3 )
				local var = decl[2]
				local val = _value(decl)
				assert( su.string.isIdent(var) )
				assert( net[var]==nil )
				net:register( val, var )

			-- properties declaration
			--		p props {}
			elseif decl[1]=='p' then
				assert( #decl >= 3 )
				local var = decl[2]
				local val = _value(decl)
				assert( su.string.isIdent(var) )
				assert( istable(val) )
				local p = properties()
				for k,v in pairs(val) do p:set(k,v) end
				assert( net[var]==nil )
				net:register( p, var )

			-- blocks declaration
			--		b name def [props]
			elseif decl[1]=='b' then
				assert( #decl >= 3 )
				local var = decl[2]
				local def = decl[3]
				assert( su.string.isIdent(var) )
				assert( su.string.isIdent(def) )
				local props = {}
				for i=4,#decl do
					local p = decl[i]
					assert( typeof(net[p])=='properties' )
					props[i-3] = net[p]
				end
				local b = block( def, unpack(props) )
				assert( net[var]==nil )
				net:register( b, var )

			-- connections declaration
			--		> (b1.o2 b3.i1) ...
			--		> b1.o2 b3.i1 ...
			elseif decl[1]=='>' then
				assert( #decl >= 3 )
				decl = _split(l,' \t()')
				for k=2,#decl,2 do
					local pstart = _plug(decl[k+0],net)
					local pend   = _plug(decl[k+1],net)
					assert( istype(pstart,'outplug') )
					assert( istype(pend,'inplug') )
					connect( pstart, pend )
				end

			-- assignement declaration
			--		= b1.i1 7
			--		= b2.i3 files
			elseif decl[1]=='=' then
				assert( #decl >= 3 )
				-- get plug
				local var = decl[2]
				local p = _plug(decl[2],net)
				assert( istype(p,'inplug') )
				-- get value
				local val = decl[3]
				if su.string.isIdent(val) then
					val = net[val]
					assert( val ~= nil )
				else
					val = _value(decl)
				end
				p:setvalue( val )
			end
		end
	end

	return net
end






