-- EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
--
-- This software is supplied under the terms of a license agreement or
-- nondisclosure agreement with Emergent Game Technologies and may not
-- be copied or disclosed except in accordance with the terms of that
-- agreement.
--
--      Copyright (c) 1996-2008 Emergent Game Technologies.
--      All Rights Reserved.
--
-- Emergent Game Technologies, Chapel Hill, North Carolina 27517
-- http://www.emergent.net
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
-- This utility was designed to assist with Bone Level-of-detail setup.
-----------------------------------------------------------------------------
macroscript GamebryoZBufferHelper category:"Gamebryo Tools" 
	buttonText:"ZBuffer Helper" toolTip:"ZBuffer Helper" Icon:#("Gamebryo", 13)
( 
    local r_dialogue = undefined

    include "Gamebryo/ZBufferHelpers.ms"

    on execute do
    (
	    -----------------------------------------------------------------------------
	    -- On ZBuffer Utils open, do this
	    -----------------------------------------------------------------------------
		    ZBufferUtility = undefined
        -- close the oldrollout floater if it exists
	    try(closerolloutfloater	r_dialogue);catch()
    
        -- put up new rollout floater and add rollout to it
        r_dialogue = newrolloutfloater "ZBuffer Utility" 220 190
        addRollout ZBufferPanel r_dialogue
    	    
	    -- end of Macro Script; rollout takes over
    )
)
