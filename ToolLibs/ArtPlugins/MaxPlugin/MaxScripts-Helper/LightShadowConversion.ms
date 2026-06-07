
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


DefinedShadowCA = undefined

for obj in objects do
(
	if obj != undefined do
	(
		bConvert = false
		if (isKindOf obj DirectionalLight) then bConvert = true
		if (isKindOf obj FreeSpot) then bConvert = true	
		if (isKindOf obj OmniLight) then bConvert = true
		if (isKindOf obj TargetDirectionalLight) then bConvert = true
		if (isKindOf obj TargetSpot) then bConvert = true

		if bConvert == true do
		(
			if DefinedShadowCA == undefined do
			(
				include "Gamebryo/LightShadowsCA.ms"
	 			DefinedShadowCA = LightShadowsCA
			)
    		
			iCACount = custAttributes.count obj
			bFound = false
			for i = 1 to iCACount do
			(
				TempCA = custAttributes.getdef obj i
				if TempCA != undefined and TempCA.name == DefinedShadowCA .name do
				(
					bFound = true
				)
			) 

			if bFound == false do
			(
				custAttributes.add obj DefinedShadowCA 
			)
		)
	)
)
