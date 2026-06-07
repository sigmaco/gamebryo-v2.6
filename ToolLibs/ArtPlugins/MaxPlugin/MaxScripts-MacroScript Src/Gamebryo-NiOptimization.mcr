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
macroscript GamebryoNiOptimizationHelper category:"Gamebryo Tools" 
	buttonText:"NiOptimization Helper" toolTip:"NiOptimization Helper" Icon:#("Gamebryo", 21)
( 
    local r_dialogue = undefined

    include "Gamebryo/NiOptimizationHelpers.ms"

    on execute do
    (
	    -----------------------------------------------------------------------------
	    -- On NiOptimization Utils open, do this
	    -----------------------------------------------------------------------------
		    NiOptimizationUtility = undefined
        -- close the oldrollout floater if it exists
	    try(closerolloutfloater	r_dialogue);catch()
    
        -- put up new rollout floater and add rollout to it
        r_dialogue = newrolloutfloater "NiOptimization Utility" 220 190
        addRollout NiOptimizationPanel r_dialogue
    	    
	    -- end of Macro Script; rollout takes over
    )
)
