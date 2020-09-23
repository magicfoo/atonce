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



co = su.net.newConnection()
assert( co:getstate(co) == su.net.S_CLOSED )

if false then
	printTable( su.net.decomposeUrl('localhost') )
	printTable( su.net.decomposeUrl('localhost:400') )
	printTable( su.net.decomposeUrl('ftp://www.google.com:32?content.php=1') )
	printTable( su.net.decomposeUrl('user:pass@demo1.atoncetech.com:1973?content.php=1#anch_54') )
	printTable( su.net.decomposeUrl('http://user:pass@demo1.atoncetech.com:1973?content.php=1#anch_54') )
end

co:setopt( su.net.O_URL, 'localhost' )
co:setopt( su.net.O_PORT, 80 )
assert( co:getopt(su.net.O_URL) == 'localhost' )
assert( co:getopt(su.net.O_PORT) == 80 )

assert( co:connect() )

while co:getstate() == su.net.S_CONNECTING do
	print 'connecting ...'
	co:perform()
end

if co:getstate() == su.net.S_CONNECTED then
	print 'connected !'
else
	print( 'failed (' .. co:getstate() .. ')' )
end


error 'stop'


