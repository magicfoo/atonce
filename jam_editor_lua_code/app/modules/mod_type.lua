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
-- list

local lix = {}
local limt  = { __index=lix }

function lix:clear ( )
	for k,_ in pairs(self) do
		l[k] = nil
	end
end

function lix:size ( )
	return #self
end

function lix:resize ( n, default_v )
	local p = #self
	local def = default_v or 0
	if p<n then
		for i=p+1,n do
			self[i]=def
		end
	else
		for i=n+1,p do
			self[i]=nil
		end
	end
end

function lix:checkintegrity ( )
	local n = #self
	for k,v in pairs(self) do
		if not isinteger(k) or k<1 or k>n then return false end
	end
	return true
end

function lix:extend ( ll )
	local t = totable(ll)
	for k,v in pairs(t) do
		self[ #self+1 ] = v
	end
end

function lix:extendkeys ( ll )
	local t = totable(ll)
	for k,v in pairs(t) do
		self[ #self+1 ] = k
	end
end

function lix:append ( v )
	self[ #self+1 ] = v
end

function lix:push_back ( v )
	self[ #self+1 ] = v
end

function lix:pop_back ( )
	local n = #self
	if n>0 then
		local v = self[n]
		self[n] = nil
		return v
	end
end

function lix:insert ( i, v )
	local n = #self
	if i>=1 and i<=n then
		for j=n,i,-1 do
			self[j+1] = self[j]
		end
		self[i] = v
	end
end

function lix:erase ( i )
	local n = #self
	if i>=1 and i<=n then
		for j=i+1,n do
			self[j-1] = self[j]
		end
		self[n] = nil
	end
end

function lix:empty ( )
	return #self == 0
end

function lix:front ( )
	if #self>0 then
		return self[1]
	end
end

function lix:back ( )
	return self[#self]
end

function lix:find ( v )
	for i=1,#self do
		if self[i]==v then return i end
	end
end

function lix:min ( )
	local n = #self
	if n>0 then
		local mn = self[1]
		for i=2,n do
			if self[i]<mn then mn=self[i] end
		end
		return mn
	end
end

function lix:max ( )
	local n = #self
	if n>0 then
		local mx = self[1]
		for i=2,n do
			if self[i]>mx then mx=self[i] end
		end
		return mx
	end
end


function list ( )
	local l = {}
	setmetatable( l, limt )
	return l
end


function array ( n )
	local l = su.util.newArray( n )
	setmetatable( l, limt )
	return l
end


function keys ( t )
	local l = list()
	l:extendkeys( t )
	return l
end








--
-- v3list


local function v3list_ctor ( )

	local impl = {}
	local mt = { __index = impl }

	function impl.checkintegrity ( l )
		local n = #l
		if (n%3)~=0 then return false end
		for k,v in pairs(l) do
			if not isinteger(k) or k<1 or k>n then return false end
		end
		return true
	end

	function impl.clear ( l )
		for k,_ in pairs(l) do
			l[k] = nil
		end
	end

	function impl.size ( l )
		return #l/3
	end

	function impl.load( l, src )
		impl.clear( l )
		local n = #src/3
		for i=1,n do
			local j = (i-1)*3
			l[j+1] = src[j+1]
			l[j+2] = src[j+2]
			l[j+3] = src[j+3]
		end
	end

	function impl.swizzle_xzy ( l )
		local n = #l/3
		for i=1,n do
			local j=(i-1)*3
			local y=l[j+2]
			local z=l[j+3]
			l[j+2] = z
			l[j+3] = y
		end
	end

	function impl.set_xyz ( l, i, v )
		if v and #v==3 and i>=1 and (i*3)<=#l then
			local j=(i-1)*3
			l[j+1] = v[1]
			l[j+2] = v[2]
			l[j+3] = v[3]
		end
	end

	function impl.get_xyz ( l, i )
		if i>=1 and (i*3)<=#l then
			local j=(i-1)*3
			return { l[j+1], l[j+2], l[j+3] }
		end
	end

	function impl.min ( l )
		local n = #l/3
		if n>=1 then
			local m = { l[1], l[2], l[3] }
			for i=2,n do
				local j = (i-1)*3
				if l[j+1]<m[1] then m[1]=l[j+1] end
				if l[j+2]<m[2] then m[2]=l[j+2] end
				if l[j+3]<m[3] then m[3]=l[j+3] end
			end
			return m
		end
	end

	function impl.max ( l )
		local n = #l/3
		if n>=1 then
			local m = { l[1], l[2], l[3] }
			for i=2,n do
				local j = (i-1)*3
				if l[j+1]>m[1] then m[1]=l[j+1] end
				if l[j+2]>m[2] then m[2]=l[j+2] end
				if l[j+3]>m[3] then m[3]=l[j+3] end
			end
			return m
		end
	end

	function impl.bbox ( l )
		local mn = impl.min(l)
		local mx = impl.max(l)
		local c = { (mn[1]+mx[1])*0.5, (mn[2]+mx[2])*0.5, (mn[3]+mx[3])*0.5 }
		local r = { 0,0,0,1 }
		local l = { mx[1]-mn[1], mx[2]-mn[2], mx[3]-mn[3] }
		return { center=c, rotation=r, length=l }
	end

	return function ( src )
		local l = {}
		setmetatable( l, mt )
		if src then
			l:load( src )
		end
		return l
	end
end

v3list = v3list_ctor()



function v3min ( r, a, b )
	if a[1]<b[1] then r[1]=a[1] else r[1]=b[1] end
	if a[2]<b[2] then r[2]=a[2] else r[2]=b[2] end
	if a[3]<b[3] then r[3]=a[3]	else r[3]=b[3] end
	return r
end


function v3max ( r, a, b )
	if a[1]>b[1] then r[1]=a[1] else r[1]=b[1] end
	if a[2]>b[2] then r[2]=a[2] else r[2]=b[2] end
	if a[3]>b[3] then r[3]=a[3]	else r[3]=b[3] end
	return r
end


function v3copy ( r, a )
	r[1]=a[1]
	r[2]=a[2]
	r[3]=a[3]
	return r
end





--
-- v2list


local function v2list_ctor ( )

	local impl = {}
	local mt = { __index = impl }

	function impl.checkintegrity ( l )
		local n = #l
		if (n%2)~=0 then return false end
		for k,v in pairs(l) do
			if not isinteger(k) or k<1 or k>n then return false end
		end
		return true
	end

	function impl.clear ( l )
		for k,_ in pairs(l) do
			l[k] = nil
		end
	end

	function impl.size ( l )
		return #l/2
	end

	function impl.load( l, src )
		impl.clear( l )
		local n = #src/2
		for i=1,n do
			local j = (i-1)*2
			l[j+1] = src[j+1]
			l[j+2] = src[j+2]
		end
	end

	function impl.set_xyz ( l, i, v )
		assert( v )
		if i>=1 and (i*2)<=#l then
			local j=(i-1)*2
			l[j+1] = v[1]
			l[j+2] = v[2]
		end
	end

	function impl.get_xyz ( l, i )
		if i>=1 and (i*2)<=#l then
			local j=(i-1)*2
			return { l[j+1], l[j+2] }
		end
	end

	function impl.min ( l )
		local n = #l/2
		if n>=1 then
			local m = { l[1], l[2], l[3] }
			for i=2,n do
				local j = (i-1)*2
				if l[j+1]<m[1] then m[1]=l[j+1] end
				if l[j+2]<m[2] then m[2]=l[j+2] end
			end
			return m
		end
	end

	function impl.max ( l )
		local n = #l/2
		if n>=1 then
			local m = { l[1], l[2], l[3] }
			for i=2,n do
				local j = (i-1)*2
				if l[j+1]>m[1] then m[1]=l[j+1] end
				if l[j+2]>m[2] then m[2]=l[j+2] end
			end
			return m
		end
	end

	function impl.bbox ( l )
		local mn = impl.min(l)
		local mx = impl.max(l)
		local c = { (mn[1]+mx[1])*0.5, (mn[2]+mx[2])*0.5 }
		local l = { mx[1]-mn[1], mx[2]-mn[2] }
		return { center=c, rotation=0, length=l }
	end

	return function ( src )
		local l = {}
		setmetatable( l, mt )
		if src then
			l:load( src )
		end
		return l
	end
end

v2list = v2list_ctor()






