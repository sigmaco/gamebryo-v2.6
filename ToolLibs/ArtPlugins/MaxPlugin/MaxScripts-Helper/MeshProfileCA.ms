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

MeshProfileCA = attributes "MeshProfileSettings" version:3 attribID:#(0x2be29156, 0x33c1f730)
( 
    
	parameters NiMeshProfileParams rollout:MeshProfileRollout 
	( 
		MeshProfileName	type:#string	default:"Default"
	) 
    
	rollout MeshProfileRollout "Mesh Profile" 
	( 		
	    dropdownlist comboMeshProfile "Mesh profile" items:#()
	    on comboMeshProfile selected newIndex do
		(
			-- change the MeshProfile string to equal the selection text
			MeshProfileName = comboMeshProfile.selected
		)
		on attachedToNode ownerNode do
		(
			-- initialize the MeshProfile to a valid name			
			profilelist = NiGetMeshProfileList()
			if profilelist.count > 0 then
				MeshProfileName = profilelist[1]
			else
				MeshProfileName = "Default"
		)
		on MeshProfileRollout open do
		(
		    profilelist = NiGetMeshProfileList()	
			-- create the drop down list array
			comboMeshProfile.items = profilelist
			
			-- set the initial index to the previous technique
			found = false
			for i = 1 to profilelist.count do
			(
				if profilelist[i] == MeshProfileName then
				(
					comboMeshProfile.selection = i
					found = true;
					break;
				)
			)
			
			-- issue warning when previous technique is not found
			if found == false do
			(
				-- if the technique was set to some value, issue a warning
				if MeshProfile != "" then
				(
					messageString = "The previously used mesh profile "
					messageString += MeshProfileName + " was not found in the mesh profile list. \n"

					messageBox messageString
				)
				
				if techArray.count > 0 then
				(
					MeshProfileName = profilelist[1]
				)
				else
				(
					MeshProfileName = "Default"
				)
			)
		)
	) 	
) 


	