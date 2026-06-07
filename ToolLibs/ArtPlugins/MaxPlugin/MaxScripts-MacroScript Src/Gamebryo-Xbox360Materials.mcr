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

macroscript XenonMaterials category:"Gamebryo Tools" 
	buttonText:"Xbox 360 Materials" toolTip:"Xbox 360 Materials" Icon:#("Gamebryo", 14)
(
	for matl in sceneMaterials do
	(
		if ( isKindOf matl Gamebryo_XPlatform_Material) then
		(
			matl.viewportMtlIndex = 1
		)
		else if (isKindOf matl MultiMaterial) then
		(
			for subMtl in matl.materialList do
			(
				if (isKindOf subMtl Gamebryo_XPlatform_Material) then
				(
					subMtl.viewportMtlIndex = 1
				)
			)
		)
	)
)