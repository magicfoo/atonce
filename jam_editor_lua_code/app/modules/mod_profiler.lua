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




function startmeasure ( inname )
	local t0 = su.util.getTime()
	return { name=inname or '?', time=t0 }
end



function getmeasure ( inm )
	local t0 = inm.time
	local t1 = su.util.getTime()
	local dt = t1-t0
	if dt<1 then
		local ti = tointeger(dt*1000)
		print( 'measure <' .. inm.name .. '> takes ' .. ti .. ' ms' )
	else
		local ti = tointeger(dt)
		local tf = tointeger(dt*10) - ti*10
		print( 'measure <' .. inm.name .. '> takes ' .. ti .. '.' .. tf .. ' s' )
	end
	return { name=inm.name, time=t1 }
end



