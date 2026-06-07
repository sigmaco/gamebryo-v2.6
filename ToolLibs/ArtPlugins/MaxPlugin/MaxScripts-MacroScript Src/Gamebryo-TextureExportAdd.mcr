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

macroscript GamebryoTextureExportToggle category:"Gamebryo Tools" 
	buttonText:"Toggle Texture Export Attributes" 
	toolTip:"Add/Remove Texture Export Attributes" Icon:#("Gamebryo", 27)
(
	include "Gamebryo/NiShaderHelpers.ms"
	include "Gamebryo/TextureExportCustAttribs.ms"
	include "Gamebryo/MipMapCustAttribs.ms"

	on execute do
	(
		currentTexMap = GetActiveBitmapTexture()			

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

			if iFoundIndexTexExport > 0 do
			(
				strContents = "Do you wish to remove the Texture Export Custom Attributes from '"
				strContents = strContents + (currentTexMap.name as string)
				strContents = strContents + "'?"
				bRemove = queryBox strContents title:"Remove Texture Export Cust Attribs?" beep:false 

				if bRemove do
				(
					custAttributes.delete currentTexMap iFoundIndexTexExport
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
			
			if iFoundIndexMipMap > 0 do
			(
				strContents = "Do you wish to remove the Mip Map Custom Attributes from '"
				strContents = strContents + (currentTexMap.name as string)
				strContents = strContents + "'?"
				bRemove = queryBox strContents title:"Remove Mip Map Export Cust Attribs?" beep:false 

				if bRemove do
				(
					custAttributes.delete currentTexMap iFoundIndexMipMap
				)
			)
			if iFoundIndexMipMap <= 0 do
			(
				custAttributes.add currentTexMap MipMapPyramidCA
			)
		)
			
		if currentTexMap == undefined do
		(
			messageBox "The material editor does not current have a selected texture map!" title:"Invalid Texture Map"
		)
	)
)