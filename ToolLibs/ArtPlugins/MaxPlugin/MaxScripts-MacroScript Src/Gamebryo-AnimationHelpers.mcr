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
macroscript ActorManagerUtil category:"Gamebryo Tools" 
	buttonText:"Gamebryo Animation Manager" toolTip:"Gamebryo Animation Manager" Icon:#("Gamebryo", 4)
( 
	global ActorManagerUtility
    local r_dialogue = undefined
    include "Gamebryo/AnimationManager.ms"

    function CleanUp =
    (
        -- close the oldrollout floater if it exists
	    try(callbacks.removeScripts #filePostOpen id:#ActorManagerUtilPostOpen);catch()
        try(callbacks.removeScripts #systemPreReset id:#ActorManagerUtilPreReset);catch()
	    try(ActorManagerUtility = undefined);catch()
	    try(closerolloutfloater r_dialogue);catch()
    )

	on execute do
    (
        CleanUp()
        
        -- put up new rollout floater and add rollout to it
        r_dialogue = newrolloutfloater "Animation Manager Utility" 220 700
		addRollout NiActorManagerRollout r_dialogue rolledUp:false
		addRollout NiActorLayerRollout r_dialogue rolledUp:true
		addRollout NiActorSequenceRollout r_dialogue rolledUp:false
		addRollout NiActorAccumulationRollout r_dialogue rolledUp:true
		addRollout NiActorSeqTagsRollout r_dialogue rolledUp:true
		ActorManagerUtility = NiActorManagerRollout
		callbacks.addScript #filePostOpen "try(ActorManagerUtility.RefreshAll());catch()" id:#ActorManagerUtilPostOpen
		callbacks.addScript #systemPreReset "CleanUp()" id:#ActorManagerUtilPreReset
		-- end of Macro Script; rollout takes over
    )
)
