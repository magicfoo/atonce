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



root			=	{ }
session_man		=	{ parent=root, file='node_session_man.lua' }

layout_man		=	{ parent=session_man, file='node_layout_man.lua' }
asset_viewer	=	{ parent=session_man, file='node_asset_viewer.lua' }
console			=	{ parent=session_man, file='node_console.lua' }
form_designer	=	{ parent=session_man, file='node_form_designer.lua' }
db_man			=	{ parent=session_man, file='dbman/node_dbman.lua' }
network			=	{ parent=session_man, file='node_network.lua' }

asset_man		=	{ parent=db_man, file='node_asset_man.lua' }

asset_browser	=	{ parent=asset_man, file='node_asset_browser.lua' }
mtc_collection	=	{ parent=asset_man, file='mtc_collection/node_mtc_collection.lua' }
mtc_world		=	{ parent=asset_man, file='mtc_world/node_mtc_world.lua' }
mtc_userdata	=	{ parent=asset_man, file='mtc_userdata/node_mtc_userdata.lua' }




