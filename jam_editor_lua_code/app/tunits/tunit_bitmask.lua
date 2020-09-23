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


require 'bitmask'



enum = { 'EM_CHOICE1', 'EM_CHOICE2', 'EM_CHOICE3', 'EM_CHOICE4' }


bm = bitmask.create( enum )


assert( bm:member(enum[1]) == 1 )
assert( bm:member(enum[2]) == 2 )
assert( bm:member(enum[3]) == 3 )
assert( bm:member(enum[4]) == 4 )
assert( bm:member('EM_CHOICE8') == nil )


assert( not bm:test 'EM_CHOICE1' )
assert( not bm:test 'EM_CHOICE2' )
assert( not bm:test 'EM_CHOICE3' )


bm:set 'EM_CHOICE2'
assert( not bm:test 'EM_CHOICE1' )
assert(     bm:test 'EM_CHOICE2' )
assert( not bm:test 'EM_CHOICE3' )


bm:clear 'EM_CHOICE2'
assert( not bm:test 'EM_CHOICE1' )
assert( not bm:test 'EM_CHOICE2' )
assert( not bm:test 'EM_CHOICE3' )


bm:set 'EM_CHOICE2'
bm:clearall()
assert( not bm:test 'EM_CHOICE1' )
assert( not bm:test 'EM_CHOICE2' )
assert( not bm:test 'EM_CHOICE3' )


bm:clearall()
assert( bm:numerical() == 0 )
bm:set 'EM_CHOICE1'
assert( bm:numerical() == 1 )
bm:set 'EM_CHOICE2'
assert( bm:numerical() == 3 )
bm:set 'EM_CHOICE3'
assert( bm:numerical() == 7 )





