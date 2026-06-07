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
macroscript GamebryoBillboardHelper category:"Gamebryo Tools" 
	buttonText:"Billboard Helper" toolTip:"Billboard Helper" Icon:#("Gamebryo", 5)
( 
    local r_dialogue = undefined
    include "Gamebryo/BillboardHelpers.ms"

    on execute do
    (
	    -----------------------------------------------------------------------------
	    -- On Billboard Utils open, do this
	    -----------------------------------------------------------------------------
		    BillboardUtility = undefined
        -- close the oldrollout floater if it exists
	    try(closerolloutfloater	r_dialogue);catch()
    
        -- put up new rollout floater and add rollout to it
        r_dialogue = newrolloutfloater "Billboard Utility" 220 275
        addRollout BillboardPanel r_dialogue
    	    
	    -- end of Macro Script; rollout takes over
    )
)
