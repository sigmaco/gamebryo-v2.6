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

macroscript GamebryoPrecacheGeometryToggle category:"Gamebryo Tools" 
	buttonText:"Toggle Precache Attributes" toolTip:"Add/Remove Precache Attributes" Icon:#("Gamebryo", 16)
(

	for obj in selection do
	(
		include "Gamebryo/PrecacheGeometryCA.ms"
	
		iCACount = custAttributes.count obj
		iFoundIndex = -1
		for i = 1 to iCACount do
		(
			TempCA = custAttributes.getdef obj i
			if TempCA != undefined and TempCA.name == PrecacheGeometryCA.name do
			(
				iFoundIndex = i
			)
		) 

		if iFoundIndex > 0 do
		(
			strContents = "Do you wish to remove the Geometry Precaching Custom Attributes from '"
			strContents = strContents + (obj.name as string)
			strContents = strContents + "'?"
			bRemove = queryBox strContents title:"Remove Geometry Precaching Cust Attribs?" beep:false 

			if bRemove do
			(
				custAttributes.delete obj iFoundIndex
			)
		)
		if iFoundIndex <= 0 do
		(
			custAttributes.add obj PrecacheGeometryCA
		)
	)

)