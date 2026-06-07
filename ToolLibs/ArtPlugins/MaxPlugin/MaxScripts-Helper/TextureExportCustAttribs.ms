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

Global GamebryoNumPlatforms = 6

TextureExportCA = attributes "TextureExportSettings" version:9 attribID:#(0x88da7c45, 0x46542296)
( 
	-- Ordering in the tab: 
	--		1) "Generic"
	--		2) "Xbox 360"
	--		3) "Playstation 3"
	--		4) "DirectX 9"
	--		5) "Wii"	
	--		6) "D3D10"

	-- Image Quality Options in the Tab:
	--		1)"Image Default"
	--		2)"Use Global Settings"
	--		3)"32 bit"
	--		4)"16 bit"
	--		5)"8 bit Palettized"
	--		6)"DXT1"
	--		7)"DXT3"
	--		8)"DXT5"
	
	
	-- Old Filter Mode: "Nearest", "Linear"
	-- Old Mip-Mapping Mode: "Nearest", "Linear", "Off"
	
	local OLD_NEAREST = 1
	local OLD_LINEAR = 2
	local OLD_OFF = 3
	
	-- New Filter Mode:
	-- Nearest, Bilerp, Nearest_MipNearest, Nearest_MipLerp, 
	-- Bilerp_MipNearest, TriLerp, Anisotropic
	
	local FM_NEAREST = 1
	local FM_BILERP = 2
	local FM_NEAREST_MIPNEAREST = 3
	local FM_NEAREST_MIPLERP = 4
	local FM_BILERP_MIPNEAREST = 5
	local FM_TRILERP = 6
	local FM_ANISOTROPIC = 7
			
	parameters main rollout:BitmapUI
	( 
		UseMasterIndexForAll	type:#boolean	animatable:false	default:false
		MasterIndex		type:#integer	animatable:false	default:1		ui:ddlCurrentPlatform
		MaxAnisotropy	type:#intTab	animatable:false	default:1		tabSizeVariable:false  tabSize:6
		FinalImageSize	type:#intTab 	animatable:false	default:1		tabSizeVariable:true  tabSize:6
		FinalImageQuality type:#intTab 	animatable:false	default:2		tabSizeVariable:true  tabSize:6
		FilterMode 		type:#intTab 	animatable:false	default:2		tabSizeVariable:true  tabSize:6
		FilterModeEx 	type:#intTab 	animatable:false	default:2		tabSizeVariable:false  tabSize:6
		MipMapMode 		type:#intTab 	animatable:false	default:2		tabSizeVariable:true  tabSize:6
		FileSettings	type:#intTab  	animatable:false	default:1		tabSizeVariable:true  tabSize:6
	) 

	rollout BitmapUI "Texture Export Parameters" 
	( 	
		dropdownlist	ddlCurrentPlatform			"Selected Platform"			items:#("Generic","Xbox 360", "Playstation 3", "DirectX 9", "Wii", "Direct3D10")	selection:1 across:2
		checkbox		cbUseCurrentPlatformForAll	"Use Current Platform For All" checked:false offset:[4,20]  

		groupBox		grpFilter		"Filtering"				pos:[15,170]	width:143 height:162
		
		radioButtons	rdoFilterModeEx	 "Filter Mode:"			pos:[25,185]	width:60  height:46		labels:#("Nearest", "Bilerp", "Nearest_MipNearest", "Nearest_MipLerp", "Bilerp_MipNearest", "TriLerp", "Anisotropic" ) columns:1
		spinner			spnMaxAnisotropy "Max Anisotropy:"		pos:[29,310]	range:[1,16,1] type:#integer align:#left width:55 enabled:false
		
		groupBox		grpQuality		"Quality"				pos:[171,50]	width:140 height:282
		radioButtons	rdoImageQuality "Final Pixel Quality:"	pos:[178,72]	width:123 height:158	labels:#("Image Default", "Use Global Settings", "32 bit", "16 bit", "8 bit Palettized", "DXT1", "DXT3", "DXT5")
		radioButtons	rdoImageSize	"Final Texture Size"	pos:[178,242]	width:81  height:62		labels:#("Full Size", "Half Size", "Quarter Size") columns:1

		groupBox		grpExportMode	"Export Mode"			pos:[15,50]		width:143 height:110
		radioButtons	rdoFileSettings "Texture Export Settings:" pos:[25,66]	width:117 height:90		labels:#("Global Settings", "Internal To NIF", "External NIF texture", "Original Images", "Original Images VRAM") columns:1 
		
		function UpdateBitmapUI = 
		(
			if MasterIndex < 1 or MasterIndex > GamebryoNumPlatforms then
				MasterIndex = 1
			
			if FileSettings[MasterIndex] >= 4 do		
			(
				rdoImageSize.enabled = false
				rdoImageQuality.enabled = false
			)
			
			if FileSettings[MasterIndex] < 4 do
			(
				rdoImageSize.enabled = true
				rdoImageQuality.enabled = true
			)

			ddlCurrentPlatform.selection = MasterIndex
			cbUseCurrentPlatformForAll.checked = UseMasterIndexForAll
			ddlCurrentPlatform.enabled = not UseMasterIndexForAll

			rdoFilterModeEx.state = FilterModeEx[MasterIndex]
			
			if (FilterModeEx[MasterIndex] == FM_ANISOTROPIC) then
			(
				spnMaxAnisotropy.enabled = true
			)
			else
			(
				spnMaxAnisotropy.enabled = false
			)
			
			spnMaxAnisotropy.value =  MaxAnisotropy[MasterIndex]
			rdoImageQuality.state = FinalImageQuality[MasterIndex]
			rdoImageSize.state = FinalImageSize[MasterIndex]
			rdoFileSettings.state = FileSettings[MasterIndex]
		)

		on BitmapUI reload do
		(
			UpdateBitmapUI()
		)
		
		on BitmapUI open  do
		(
			UpdateBitmapUI()
		)

		on ddlCurrentPlatform selected item do
		(
			MasterIndex = item
			UpdateBitmapUI()
		)

		on cbUseCurrentPlatformForAll changed state do
		(
			UseMasterIndexForAll = state
			MasterIndex = ddlCurrentPlatform.selection
			ddlCurrentPlatform.enabled = not state

			if state == true do
			(
				for i = 1 to GamebryoNumPlatforms do 
				(
					if i != MasterIndex do
					(
						FilterMode[i] = FilterMode[MasterIndex] 
						MipMapMode[i] = MipMapMode[MasterIndex] 
						FinalImageQuality[i] = FinalImageQuality[MasterIndex]
						FinalImageSize[i] = FinalImageSize[MasterIndex]
						FileSettings[i] = FileSettings[MasterIndex]
					)
				)
				UpdateBitmapUI()
			)
		)

		on rdoFilterModeEx changed state do
		(
			for i = 1 to GamebryoNumPlatforms do 
			(
				bAdd = false
				if UseMasterIndexForAll == true then bAdd = true
				if i == MasterIndex then bAdd = true

				if bAdd == true then 
				(
					FilterModeEx[i] = state
					if (state == FM_ANISOTROPIC) then
					(
						spnMaxAnisotropy.enabled = true
					)
					else
					(
						spnMaxAnisotropy.enabled = false					
					)
				)
			)
		)
		
		on spnMaxAnisotropy changed value do
		(
			for i = 1 to GamebryoNumPlatforms do 
			(
				bAdd = false
				if UseMasterIndexForAll == true then bAdd = true
				if i == MasterIndex then bAdd = true
				
				if bAdd == true then 
				(
					MaxAnisotropy[i] = value					
				)
			)			
		)
		
		on rdoImageQuality changed state do
		(
			for i = 1 to GamebryoNumPlatforms do 
			(
				bAdd = false
				if UseMasterIndexForAll == true then bAdd = true
				if i == MasterIndex then bAdd = true

				if bAdd == true then FinalImageQuality[i] = state
			)

			rdoImageQuality.state = state
			
		)
		
		on rdoImageSize changed state do
		(
			for i = 1 to GamebryoNumPlatforms do 
			(
				bAdd = false
				if UseMasterIndexForAll == true then bAdd = true
				if i == MasterIndex then bAdd = true

				if bAdd == true then FinalImageSize[i] = state
			)
		)
		
		on rdoFileSettings changed state do
		(
			oldState = rdoFileSettings.state
			for i = 1 to GamebryoNumPlatforms do 
			(
				bAdd = false
				if UseMasterIndexForAll == true then bAdd = true
				if i == MasterIndex then bAdd = true

				if bAdd == true then FileSettings[i] = state		
			)

			if state >= 4 do
			(
 		 	    for i = 1 to GamebryoNumPlatforms do 
				(
					bAdd = false
					if UseMasterIndexForAll == true then bAdd = true
					if i == MasterIndex then bAdd = true

					if bAdd == true do
					( 	
						FinalImageSize[i] = 1
						FinalImageQuality[i] = 1
					)	
				)
			
			)

			UpdateBitmapUI()
		)
	)
	
	on update do
	(
		if (version < 4) do
		(
			--Compensate for the removal of 4-bit palletized, which was PS2 only
		    if (FinalImageQuality[1] == 6) do
		    (
				FinalImageQuality[1] = 5
		    )
		    if (FinalImageQuality[1] > 6) do
		    (
				FinalImageQuality[1] -= 1
		    )
		    MasterIndex = 1
		    if UseMasterIndexForAll then
		    (
				-- set all the other platform options to the current PC settings
				for i = 2 to GamebryoNumPlatforms do
				(
					FinalImageSize[i] = FinalImageSize[1]
					FinalImageQuality[i] = FinalImageQuality[1]
					FilterMode[i] = FilterMode[1]
					MipMapMode[i] = MipMapMode[1]
					FileSettings[i] = FileSettings[1]
				)
		    )
		    else
		    (
				-- set all other options back to default as they are no longer valid
				for i = 2 to GamebryoNumPlatforms do
				(
					FinalImageSize[i] = 1
					FinalImageQuality[i] = 2
					FilterMode[i] = 2
					MipMapMode[i] = 2
					FileSettings[i] = 1
				)
			)
		)
		if (version < 9) do
		(
			for i = 1 to GamebryoNumPlatforms do
			(				
				if (MipMapMode[i] == OLD_OFF and FilterMode[i] == OLD_NEAREST) then
				(
					-- Nearest
					FilterModeEx[i] = FM_NEAREST 
				)
				else if (MipMapMode[i] == OLD_OFF and FilterMode[i] == OLD_LINEAR) then
				(
					-- Bilerp
					FilterModeEx[i] = FM_BILERP  
				)
				else if (MipMapMode[i] == OLD_NEAREST and FilterMode[i] == OLD_NEAREST) then
				(
					-- Nearest_MipNearest
					FilterModeEx[i] = FM_NEAREST_MIPNEAREST
				)
				else if (MipMapMode[i] == OLD_LINEAR and FilterMode[i] == OLD_NEAREST) then
				(
					-- Nearest_MipLerp
					FilterModeEx[i] = FM_NEAREST_MIPLERP
				)
				else if (MipMapMode[i] == OLD_NEAREST and FilterMode[i] == OLD_LINEAR) then
				(
					-- Bilerp_MipNearest
					FilterModeEx[i] = FM_BILERP_MIPNEAREST
				)
				else if (MipMapMode[i] == OLD_LINEAR and FilterMode[i] == OLD_LINEAR) then
				(
					-- TriLerp
					FilterModeEx[i] = FM_TRILERP
				) -- if
			) -- for	
		) --if
	) -- on update do
) -- TextureExportCA 
