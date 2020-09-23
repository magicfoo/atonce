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







function tag ( name )
	assert( isstring(name) )
	if su.string.startsWith(name,'rsc.') then
		return { tagid = name }
	else
		return { tagid = 'rsc.collection.'..name }
	end
end








-- SHARED RECORD  ------------------------------------------------------------------------------------------------------------

rec = {}


function rec.string ( intext )
	if isstring(intext) then
		return { len=#intext, text=intext }
	else
		return { len=0 }
	end
end


function rec.matrix ( in_t, in_r, in_s )
	local t = su.math.newVec3( in_t or {0,0,0} )
	local r = su.math.newQuat( in_r or {0,0,0,1} )
	local s = su.math.newVec3( in_s or {1,1,1} )
	r:w( -r:w() )	-- y/z swizzling
	t = t:xzy()
	r = r:xzyw()
	s = s:xzy()
	local tr = su.math.buildTR( t, r, s )
	return { coeff = tr:asTable() }
end


function rec.color ( incolor )
	local c = {}
	local co = incolor or {1,0,1,1}	-- default is mangenta
	for i=1,4 do c[i]=(co[i] or 1)*255 end
	return c
end









-- COLLECTION ------------------------------------------------------------------------------------------------------------

coll = {}


coll.ort_type = {
	['undef']		= 0,
	['constant']	= 1,
	['cycle']		= 2,
	['oscillate']	= 3,
	['linear']		= 4,
	['rel-cycle']	= 5
}


function coll.ort ( inort )
	local o1,o2 = unpack( su.string.split(inort) )
	local t1,t2 = coll.ort_type[o1], ort_type[o2]
	assert( t1 and t2 )
	return { t1, t2 }
end




