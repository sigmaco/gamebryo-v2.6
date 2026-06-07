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
macroscript LODUtil category:"Gamebryo Tools" 
	buttonText:"Level-Of-Detail" toolTip:"Level-Of-Detail" Icon:#("Gamebryo", 6)
( 
	global LODUtility
    local r_dialogue = undefined
	include "Gamebryo/LODHelpers.ms"

    function CleanUp =
    (
        try(callbacks.removeScripts id:#LODUtilityCallbacks); catch()
	    try(LODUtility = undefined); catch()
	    try(closerolloutfloater r_dialogue); catch()
    )

    on execute do
    (
	    -----------------------------------------------------------------------------
	    -- On LOD Utils open, do this
	    -----------------------------------------------------------------------------
	    -- close the oldrollout floater if it exists
	    CleanUp()
        
        -- put up new rollout floater and add rollout to it
        r_dialogue = newrolloutfloater "Level-Of-Detail Utility" 220 650

	    addRollout NiLODRollout r_dialogue
	    LODUtility = NiLODRollout
	    
	    callbacks.addScript #filePreOpen "try(LODUtility.ViewMatchTimer.active = false);catch()" id:#LODUtilityCallbacks
	    callbacks.addScript #filePostOpen "try(LODUtility.ResetUtility(); LODUtility.PopulateSetName()); catch(print \"Error in LODUtility FilePostLoad callback.\")" id:#LODUtilityCallbacks
		callbacks.addScript #nodePostDelete "try(LODUtility.UpdateUI(); LODUtility.RefreshLODGroups rootNode); catch()" id:#LODUtilityCallbacks
		callbacks.addScript #nodeRenamed "try(LODUtility.UpdateUI(); LODUtility.RefreshLODGroups rootNode); catch();" id:#LODUtilityCallbacks
		callbacks.addScript #systemPostReset "try(LODUtility.ResetUtility());catch()" id:#LODUtilityCallbacks
		callbacks.addScript #systemPostNew "try(LODUtility.ResetUtility());catch()" id:#LODUtilityCallbacks
				    
	    -- end of Macro Script; rollout takes over
    )
)
