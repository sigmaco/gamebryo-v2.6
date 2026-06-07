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


macroscript GamebryoTextureExportAddAll category:"Gamebryo Tools" 
	buttonText:"Add Texture Export Attributes To All Materials" 
	toolTip:"Add Texture Export Attributes To All Materials" Icon:#("Gamebryo", 23)
(
	include "Gamebryo/NiShaderHelpers.ms"
	include "Gamebryo/TextureExportCustAttribs.ms"
	include "Gamebryo/MipMapCustAttribs.ms"

	on execute do
	(
		for mtl in sceneMaterials do
		(
			allTexMaps = #()
			allTexMaps = GetAllTexMapsFromMaterial mtl allTexMaps

			for currentTexMap in allTexMaps do
			(
				if currentTexMap != undefined do
				(	
					iCACount = custAttributes.count currentTexMap
					iFoundIndexTexExport = -1
					iFoundIndexMipMap = -1

					for i = 1 to iCACount do
					(
						TempCA = custAttributes.getdef currentTexMap i
						if TempCA != undefined and TempCA.name == TextureExportCA.name do
						(
							iFoundIndexTexExport = i
						)
					) 

					if iFoundIndexTexExport <= 0 do
					(
						custAttributes.add currentTexMap TextureExportCA						
					)

					iCACount = custAttributes.count currentTexMap
					for i = 1 to iCACount do
					(
						TempCA = custAttributes.getdef currentTexMap i
						if TempCA != undefined and TempCA.name == MipMapPyramidCA.name do
						(
							iFoundIndexMipMap = i
						)
					) 
					
					if iFoundIndexMipMap <= 0 do
					(
						custAttributes.add currentTexMap MipMapPyramidCA
					)
				)
			)				
		)
	)
)