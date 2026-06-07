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
-- This utility was designed to assist with Bone Switch Node setup.
-----------------------------------------------------------------------------
macroscript SwitchNodeUtil category:"Gamebryo Tools" 
	buttonText:"Switch Node" toolTip:"Switch Node" Icon:#("Gamebryo", 6)
( 
    global SwitchNodeUtility
	local r_dialogue = undefined
    local SwitchNodeUtility = undefined
    include "Gamebryo/SwitchNodeHelpers.ms"

    function CleanUp =
    (
        try(callbacks.removeScripts #filePostOpen id:#SwitchNodeUtilPostOpen); catch()
	    try(callbacks.removeScripts #filePreOpen id:#SwitchNodeUtilPreOpen); catch()
        try(callbacks.removeScripts #systemPreReset id:#SwitchNodeUtilPreReset); catch()
	    try(SwitchNodeUtility = undefined); catch()
	    try(closerolloutfloater r_dialogue); catch()    
    )

    on execute do
    (
	    -----------------------------------------------------------------------------
	    -- On SwitchNode Utils open, do this
	    -----------------------------------------------------------------------------
	    -- close the oldrollout floater if it exists
	    CleanUp()
        
        -- put up new rollout floater and add rollout to it
        r_dialogue = newrolloutfloater "Switch Node Utility" 220 650

	    addRollout NiSwitchNodeRollout r_dialogue
	    SwitchNodeUtility = NiSwitchNodeRollout
	    
	    callbacks.addScript #filePostOpen "try(SwitchNodeUtility.RefreshSwitchNodeGroups());catch()" id:#SwitchNodeUtilPostOpen
        callbacks.addScript #systemPreReset "CleanUp()" id:#SwitchNodeUtilPreReset
		    
	    -- end of Macro Script; rollout takes over
    )
)
