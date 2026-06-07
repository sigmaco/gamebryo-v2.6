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
macroscript GamebryoBoneLODHelper category:"Gamebryo Tools" 
	buttonText:"Bone LOD Helper" toolTip:"Bone LOD Helper" Icon:#("Gamebryo", 7)
( 
	global BoneLODUtility
	global SkinLODUtility
    local BoneLODUtility = undefined
    local r_dialogue = undefined
    include "Gamebryo/BoneLODHelper.ms"

	function CleanUp =
    (
        try(closerolloutfloater	r_dialogue);catch()
        try(callbacks.removeScripts #filePostOpen id:#BoneSkinLODUtilPostOpen);catch()
        try(callbacks.removeScripts #systemPreReset id:#BoneLODUtilPreReset);catch()
        
        BoneLODUtility = undefined
    )
    
    on execute do
    (
        -----------------------------------------------------------------------------
	    -- On Bone LOD Utils open, do this
	    -----------------------------------------------------------------------------
		    BoneLODUtility = undefined
        -- close the oldrollout floater if it exists
	    CleanUp()

        -- put up new rollout floater and add rollout to it
        r_dialogue = newrolloutfloater "Bone LOD Utility" 220 600
        addRollout NiBoneLODRollout r_dialogue
        BoneLODUtility = NiBoneLODRollout
	    callbacks.addScript #filePostOpen "try(BoneLODUtility.RefreshAll()\nBoneLODUtility.RefreshAllSkins()\nBoneLODUtility.UpdateUI()\nBoneLODUtility.UpdateSkinUI()\n);catch()" id:#BoneSkinLODUtilPostOpen
		callbacks.addScript #systemPreReset "CleanUp()" id:#BoneLODUtilPreReset
		
	    -- end of Macro Script; rollout takes over
    )
)
