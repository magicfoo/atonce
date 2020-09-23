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





function presentLua ( _v )
	if _v==nil then
		return
	
	elseif istable(_v) then
		local s = '{'
		local n = 0
		for k,v in pairs(_v) do
			-- present the key
			local sk, sv
			if isstring(k) and su.string.isIdent(k) then
				sk = k
			else
				sk = '['..presentLua(k)..']'
			end
			-- present the value
			sv = presentLua(v)
			if n>0 then s=s..',' end
			s = s .. sk .. '=' .. sv
			n = n + 1
		end
		s = s .. '}'
		return s

	elseif isstring(_v) then
		return "'"..str(_v).."'"

	else
		return str(_v)

	end
end





function parseLua ( _s, _G )
	if isstring(_s) then
		local ss = su.string.strip(_s)
		if ss and #ss>0 then
			local c = su.lang.parseText('stream='..ss)
			if c then
				local fcode = su.lang.loadCode(c)
				if isfunction(fcode) then
					local callenv = _G or getfenv(2)
					local mt = getmetatable(callenv)
					local et = (mt or {}).__index or callenv
					local e = {}
					setmetatable( e, {__index=et} )
					setfenv( fcode, e )
					fcode()
					return e.stream
				end
			end
		end
	end
end






function presentYaml ( _v )
	return su.io.presentStream(_v)
end






function parseYaml ( _s )
	return su.io.parseStream(_v)
end






