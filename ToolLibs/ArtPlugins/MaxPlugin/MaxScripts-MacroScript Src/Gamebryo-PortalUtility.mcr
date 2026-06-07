macroScript PortalDialog category:"Gamebryo Tools" toolTip:"Portal Utility" icon:#("Gamebryo", 29)
(

-- --------------------GLOBAL VARIABLES--------------------------
Global PortalUtility
Global RefreshUI
Global SelectionChangedCallback
Global FileOpenCallback

local mainFloater

NameRollout
NameFloater
NameType

GroupArray
RoomArray
PortalArray
MeshArray

CurrentGroup = 0
CurrentRoom = 0
CurrentPortal = 0
CurrentMesh = 0
-- --------------------GLOBAL FUNCTIONS--------------------------

function GetGroups = 
(
	GroupArray = $*_NDLRG* as array 
	return GroupArray
)

function CreateGroupList = 
(
	groupList = GetGroups()
	stringList = #()
	for i = 1 to groupList.count do
	(
		append stringList groupList[i].name
	)
	return stringList
)

function GetRooms = 
(
	RoomArray = #()
	if CurrentGroup != 0 then
	(
		for i = 1 to GroupArray[CurrentGroup].children.count do
		(
			if (FindString GroupArray[CurrentGroup].children[i].name "_NDLRM" != undefined) do
			(
				append RoomArray GroupArray[CurrentGroup].children[i]
			)
		)
	)
	return RoomArray
)

function CreateRoomList = 
(
	stringList = #()
	GetRooms()
	if (CurrentGroup != 0) do
	(
		for i = 1 to RoomArray.count do
		(
			append stringList RoomArray[i].name
		)
	)
	return stringList
)

function GetPortals = 
(
	PortalArray = #()
	if CurrentRoom != 0 then
	(
		for i = 1 to RoomArray[CurrentRoom].children.count do
		(
			if (FindString RoomArray[CurrentRoom].children[i].name "_NDLPR" != undefined) then
			(
				append PortalArray RoomArray[CurrentRoom].children[i]
			)
		)
	)
	return PortalArray
)

function CreatePortalList = 
(
	stringList = #()
	GetPortals()
	if CurrentRoom != 0 then
	(
		for i = 1 to PortalArray.count do
		(
			AppendString = ""
			propertyString = getUserProp PortalArray[i] "NDLADJOINER"
			if propertyString != undefined then
				AppendString = " (to " + propertyString + ")"
			if (findString (getUserPropBuffer PortalArray[i]) "NDLTWOWAY") != undefined then
				AppendString += " (two-way)"
			append stringList (PortalArray[i].name + AppendString)
		)
	)
	return stringList
)

function GetMeshes = 
(
	MeshArray = #()
	if CurrentRoom != 0 then
	(
		for i = 1 to RoomArray[CurrentRoom].children.count do
		(
			if (FindString RoomArray[CurrentRoom].children[i].name "_NDLPR" == undefined) then
			(
				append MeshArray RoomArray[CurrentRoom].children[i]
			)
		)
	)
	return MeshArray
)

function CreateMeshList = 
(
	stringList = #()
	GetMeshes()
	if CurrentRoom != 0 then
	(
		for i = 1 to MeshArray.count do
		(
			append stringList MeshArray[i].name
		)
	)
	return stringList
)

-- --------------------UI DEFINITION-----------------------------
rollout PortalUtility "Portal Utility" --width:400 height:600
(
	dropDownList GroupDropdown "Current Group" pos:[16,16] width:232 height:40
	listBox RoomList "Rooms" pos:[16,56] width:232 height:12
	groupBox GroupGroup "Group Controls" pos:[264,16] width:120 height:96
	groupBox RoomGroup "Room Controls" pos:[264,120] width:120 height:112
	button CreateGroupButton "Create" pos:[272,40] width:104 height:16
	button DeleteGroupButton "Delete" pos:[272,64] width:104 height:16
	button RenameGroupButton "Rename" pos:[272,88] width:104 height:16
	button CreateRoomButton "Create" pos:[272,136] width:104 height:16
	button FromSelectionButton "From Selection" pos:[272, 160] width:104 height:16
	button DeleteRoomButton "Delete" pos:[272,184] width:104 height:16
	button RenameRoomButton "Rename" pos:[272,208] width:104 height:16
	dropDownList RoomDropdown "Target Room" pos:[24,515] width:264 height:40
	groupBox PortalGroup "Portal Management" pos:[16,400] width:368 height:184
	listBox PortalList "" pos:[24,416] width:352 height:7
	pickbutton AddPortalButton "Add Portal" pos:[24,560] width:80 height:16
	button RemovePortalButton "Remove Portal" pos:[112,560] width:80 height:16
	button PortalFromSelectionButton "From Selection" pos:[200,560] width:88 height:16
	button RenamePortalButton "Rename" pos:[296,560] width:80 height:16
	checkbox TwoWayCheck "Two-way" pos:[296,536] width:80 height:16
	groupBox MeshGroup "Room Geometry" pos:[16,240] width:368 height:152
	listBox MeshList "" pos:[24,256] width:224 height:9
	pickbutton AttachMeshButton "Add Object" pos:[256,256] width:120 height:16
	button AddSelectionButton "Add Selection" pos:[256,280] width:120 height:16
	button RemoveMeshButton "Remove Object" pos:[256,304] width:120 height:16
	checkbox WallCheck "Wall" pos:[256,328] width:120 height:16
	
-- --------------------MEMBER FUNCTIONS--------------------------

	function RefreshButtons = 
	(
		-- disable everything
		PortalUtility.GroupDropdown.enabled = false
		PortalUtility.DeleteGroupButton.enabled = false
		PortalUtility.RenameGroupButton.enabled = false
		PortalUtility.CreateRoomButton.enabled = false
		PortalUtility.DeleteRoomButton.enabled = false
		PortalUtility.RenameRoomButton.enabled = false
		PortalUtility.RoomDropdown.enabled = false
		PortalUtility.TwoWayCheck.enabled = false
		PortalUtility.AddPortalButton.enabled = false
		PortalUtility.RemovePortalButton.enabled = false
		PortalUtility.RenamePortalButton.enabled = false
		PortalUtility.AttachMeshButton.enabled = false
		PortalUtility.RemoveMeshButton.enabled = false
		PortalUtility.WallCheck.enabled = false
		PortalUtility.PortalFromSelectionButton.enabled = false
	
		if GroupArray.count > 0 then
			PortalUtility.GroupDropdown.enabled = true
			
		if (0 < CurrentGroup) and (CurrentGroup < GroupArray.count + 1) then
		(
			PortalUtility.DeleteGroupButton.enabled = true
			PortalUtility.RenameGroupButton.enabled = true
			PortalUtility.CreateRoomButton.enabled = true
		)
		
		if (0 < CurrentRoom) and (CurrentRoom < RoomArray.count + 1) then
		(
			PortalUtility.DeleteRoomButton.enabled = true
			PortalUtility.RenameRoomButton.enabled = true
			PortalUtility.AddPortalButton.enabled = true
			PortalUtility.AttachMeshButton.enabled = true
		)
		
		if (0 < CurrentPortal) and (CurrentPortal < PortalArray.count + 1) then
		(
			PortalUtility.RemovePortalButton.enabled = true
			PortalUtility.RenamePortalButton.enabled = true
			PortalUtility.RoomDropdown.enabled = true
			PortalUtility.TwoWayCheck.enabled = true
		)
		
		if (0 < CurrentMesh) and (CurrentMesh < MeshArray.count + 1) then
		(
			PortalUtility.RemoveMeshButton.enabled = true
			PortalUtility.WallCheck.enabled = true
		)
	)
	
	function RefreshUI = 
	(
		-- Update GroupDropdown
		PortalUtility.GroupDropdown.items = CreateGroupList()
		if GroupArray.count > 0 and CurrentGroup == 0 then
			CurrentGroup = 1
		if GroupArray.count < CurrentGroup then
			CurrentGroup = 1
		if GroupArray.count == 0 then CurrentGroup = 0
		PortalUtility.GroupDropdown.selection = CurrentGroup
		
		-- Update RoomList
		PortalUtility.RoomList.items = CreateRoomList()
		if RoomArray.count < CurrentRoom then
			CurrentRoom = 1
		if RoomArray.count == 0 then CurrentRoom = 0
		PortalUtility.RoomList.selection = CurrentRoom
		
		-- Update PortalList
		PortalUtility.PortalList.items = CreatePortalList()
		if PortalArray.count < CurrentPortal then
			CurrentPortal = 1
		if PortalArray.count == 0 then CurrentPortal = 0
		PortalUtility.PortalList.selection = CurrentPortal
		-- Update RoomDropdown
		-- Update TwoWayCheck
		PortalUtility.RoomDropDown.items = CreateRoomList()
		if CurrentPortal > 0 and CurrentPortal <= PortalArray.count then
		(
			for i = 1 to RoomArray.count do
			(
				RoomString = "\"" + PortalUtility.RoomDropdown.items[i] + "\""
				if RoomString == getUserProp PortalArray[CurrentPortal] "NDLADJOINER"  then
					PortalUtility.RoomDropdown.selection = i
			)
			if (findString (getUserPropBuffer PortalArray[CurrentPortal]) "NDLTWOWAY" != undefined) then
				PortalUtility.TwoWayCheck.checked = true
			else
				PortalUtility.TwoWayCheck.checked = false
		)
		
		-- Update MeshList
		PortalUtility.MeshList.items = CreateMeshList()
		if MeshArray.count < CurrentMesh then
			CurrentMesh = 1
		if MeshArray.count == 0 then 
		(
			CurrentMesh = 0
			PortalUtility.WallCheck.checked = false
		)
		if CurrentMesh > 0 and CurrentMesh <= MeshArray.count then
		(
			if (FindString MeshArray[CurrentMesh].name "_NDLWL" != undefined) then
			(
				PortalUtility.WallCheck.checked = true
			)
			else
			(
				PortalUtility.WallCheck.checked = false
			)
		)
		PortalUtility.MeshList.selection = CurrentMesh
		
		-- refresh buttons
		PortalUtility.RefreshButtons()
		PortalUtility.SelectionChangedCallback()
	)
	
	function SelectionChangedCallback = 
	(
		PortalUtility.AddSelectionButton.enabled = false
		PortalUtility.PortalFromSelectionButton.enabled = false
		PortalUtility.FromSelectionButton.enabled = false
		
		if (0 < CurrentGroup) and (CurrentGroup <= GroupArray.count) then
		(
			selectedObjects = getCurrentSelection()
			if selectedObjects.count > 0 then
			(
				if (0 < CurrentRoom) and (CurrentRoom <= RoomArray.count) then
				(
					PortalUtility.AddSelectionButton.enabled = true
					PortalUtility.PortalFromSelectionButton.enabled = true
				)
				PortalUtility.FromSelectionButton.enabled = true
			)
		)
	)
	
	function FileOpenCallback = 
	(
		CurrentRoom = CurrentGroup = CurrentMesh = CurrentPortal = 0
		RefreshUI()
	)

-- --------------------EVENT HANDLERS----------------------------
	on GroupDropdown selected selection do
	(
		CurrentGroup = selection
		RefreshUI()
	)
	
	on RoomList selected selection do
	(
		-- display the room's meshes and portals in the listboxes below
		CurrentRoom = selection
		SelectionChangedCallback()
		RefreshUI()
	)
	
	on CreateGroupButton pressed do
	(
		-- pop up an enter name dialog
		NameType = 1
		NameFloater = newRolloutFloater "Create Group" 386 70
		addRollout NameRollout NameFloater 
		setFocus NameRollout.NameText
	)
	
	function CreateGroupCallback NewName = 
	(
		-- manipulate string argument
		nameString = NewName
		if nameString == "" then
			nameString = uniqueName "Group_NDLRG"
		if (nameString.count < 6) or (findString nameString "_NDLRG" == undefined) then
			nameString = nameString + "_NDLRG"
		D = dummy name:nameString
		CurrentGroup = GroupArray.count
		RefreshUI()
	)
	
	on DeleteGroupButton pressed do
	(
		-- if group is selected, delete it
		if (0 < CurrentGroup) and (CurrentGroup <= GroupArray.count) then
			delete GroupArray[CurrentGroup]
		RefreshUI()
	)
	
	on RenameGroupButton pressed do
	(
		-- if group is selected
		if (GroupDropdown.selected != undefined) do
		(	
			-- pop up naming dialog
			NameType = 2
			NameFloater = newRolloutFloater "Rename Group" 386 70
			addRollout NameRollout NameFloater
			NameRollout.NameText.text = GroupDropdown.selected
			setFocus NameRollout.NameText
		)
	)
	
	function RenameGroupCallback NewName = 
	(
		-- check if valid, if has _NDLRG on end if not add it
		nameString = NewName
		if nameString == "" then
			nameString = uniqueName "Group_NDLRG"
		if (nameString.count < 6) or (findString nameString "_NDLRG" == undefined) then
			nameString = nameString + "_NDLRG"
		GroupArray[CurrentGroup].name = nameString
		RefreshUI()
	)
	
	on CreateRoomButton pressed do
	(
		-- pop up an enter name dialog
		NameType = 3
		NameFloater = newRolloutFloater "Create Room" 386 70
		addRollout NameRollout NameFloater
		setFocus NameRollout.NameText
	)
	
	function CreateRoomCallback NewName = 
	(
		nameString = NewName
		if nameString == "" then
			nameString = uniqueName "Room_NDLRM"
		if (nameString.count < 6) or (findString nameString "_NDLRM" == undefined) then
			nameString = nameString + "_NDLRM"
		D = dummy name:nameString
		attachObjects GroupArray[CurrentGroup] D move:false
		D.boxsize = [5, 5, 5]
		CurrentRoom = RoomArray.count
		RefreshUI()
	)
	
	on FromSelectionButton pressed do
	(
		NameType = 6
		NameFloater = newRolloutFloater "Create Room" 386 70
		addRollout NameRollout NameFloater
		setFocus NameRollout.NameText
	)
	
	function FromSelectionCallback NewName =
	(
		if (0 < CurrentGroup) and (CurrentGroup <= GroupArray.count) then
		(
			selectionArray = getCurrentSelection()
			nameString = NewName
			if nameString == "" then
				nameString = uniqueName "Room_NDLRM"
			if (nameString.count < 6) or (findString nameString "_NDLRM" == undefined) then
				nameString = nameString + "_NDLRM"
			D = dummy name:nameString
			attachObjects GroupArray[CurrentGroup] D move:false
			D.boxsize = [5, 5, 5]
			for i = 1 to selectionArray.count do
			(
				if ((findString selectionArray[i].name "_NDLPR" == undefined) and
					(findString selectionArray[i].name "_NDLRM" == undefined) and
					(findString selectionArray[i].name "_NDLRG" == undefined)) then
				(
					if (isGroupMember selectionArray[i]) then
						setGroupMember selectionArray[i] false
					attachObjects D selectionArray[i] move:false
				)
			)
			CurrentRoom = RoomArray.count
			RefreshUI()
		)
	)
	
	on DeleteRoomButton pressed do
	(
		if (0 < CurrentRoom) and (CurrentRoom <= RoomArray.count) then
			delete RoomArray[CurrentRoom]
		RefreshUI()
	)
	
	on RenameRoomButton pressed do
	(
		-- if room is selected pop up enter name dialog
		if (RoomList.selected != undefined) do
		(
			NameType = 4
			NameFloater = newRolloutFloater "Rename Room" 386 70
			addRollout NameRollout NameFloater
			NameRollout.NameText.text = RoomList.selected
			setFocus NameRollout.NameText
		)
	)
	
	function RenameRoomCallback NewName = 
	(
		nameString = NewName
		if nameString == "" then
			nameString = uniqueName "Room_NDLRM"
		if (nameString.count < 6) or (findString nameString "_NDLRM" == undefined) then
			nameString = nameString + "_NDLRM"
		RoomArray[CurrentRoom].name = nameString
		RefreshUI()
	)
	
	on PortalList selected selection do
	(
		-- RefreshUI()
		CurrentPortal = selection
		select PortalArray[CurrentPortal]
		RefreshUI()
	)
	
	on RoomDropdown selected selection do
	(
		-- search user defined properties for adjoiner tag
		if (0 < CurrentPortal) and (CurrentPortal <= PortalArray.count) then
		(
			setUserProp PortalArray[CurrentPortal] "NDLADJOINER" ("\"" + RoomDropdown.selected + "\"")
			string = getUserPropBuffer PortalArray[CurrentPortal]
			i = findString string "NDLADJOINER ="
			if (i != undefined) then
			(
				string2 = replace string i 13 "NDLADJOINER"
				setUserPropBuffer PortalArray[CurrentPortal] string2
			)
		)
		RefreshUI()
	)
	
	on AddPortalButton picked object do
	(
		if (CurrentRoom > 0) and (CurrentRoom <= RoomArray.count) then
		(
			if ((findString object.name "_NDLWL" == undefined) and
				(findString object.name "_NDLRG" == undefined) and 
				(findString object.name "_NDLRM" == undefined)) then
			(
				if (object.name.count < 6) or (findString object.name "_NDLPR" == undefined) then
					object.name += "_NDLPR"
				if (isGroupMember object) then
					setGroupMember object false
				attachObjects RoomArray[CurrentRoom] object move:false
				object.xray = true
				RefreshUI()
				CurrentPortal = PortalList.items.count
				PortalList.selection = PortalList.items.count
				currentUserProp = getUserPropBuffer PortalArray[CurrentPortal]
				if currentUserProp == "" then
				(
					setUserProp PortalArray[CurrentPortal] "NDLADJOINER" ("\"" + RoomDropdown.selected + "\"")
					currentUserProp = getUserPropBuffer PortalArray[CurrentPortal]
					i = findString currentUserProp "NDLADJOINER ="
					if (i != undefined) then
					(
						newUserProp = replace currentUserProp i 13 "NDLADJOINER"
						setUserPropBuffer PortalArray[CurrentPortal] newUserProp
					)
				)
				RefreshUI()
			)
		)
	)
	
	on RemovePortalButton pressed do
	(
		-- unlink the portal from the room
		if (0 < CurrentPortal) and (CurrentPortal <= PortalArray.count) then
		(
			Portal = PortalArray[CurrentPortal]
			RetVal = findString Portal.name "_NDLPR"
			if (RetVal != Undefined) then
			(
				Portal.name = replace Portal.name RetVal 6 ""
			)
			Portal.xray = false
			deleteItem RoomArray[CurrentRoom].children Portal
		)
		RefreshUI()
	)
	
	on PortalFromSelectionButton pressed do
	(
		if (CurrentRoom > 0) and (CurrentRoom <= RoomArray.count) then
		(
			for object in GetCurrentSelection() do
			(
				if ((findString object.name "_NDLWL" == undefined) and
					(findString object.name "_NDLRG" == undefined) and 
					(findString object.name "_NDLRM" == undefined)) then
				(
					if (object.name.count < 6) or (findString object.name "_NDLPR" == undefined) then
						object.name += "_NDLPR"
					if (isGroupMember object) then
						setGroupMember object false
					attachObjects RoomArray[CurrentRoom] object move:false
					object.xray = true
					RefreshUI()
					CurrentPortal = PortalList.items.count
					PortalList.selection = PortalList.items.count
					currentUserProp = getUserPropBuffer PortalArray[CurrentPortal]
					if currentUserProp == "" then
					(
						setUserProp PortalArray[CurrentPortal] "NDLADJOINER" ("\"" + RoomDropdown.selected + "\"")
						currentUserProp = getUserPropBuffer PortalArray[CurrentPortal]
						i = findString currentUserProp "NDLADJOINER ="
						if (i != undefined) then
						(
							newUserProp = replace currentUserProp i 13 "NDLADJOINER"
							setUserPropBuffer PortalArray[CurrentPortal] newUserProp
						)
					)
					RefreshUI()
				)
			)
		)
	)
	
	on RenamePortalButton pressed do
	(	
		-- if portal selected pop up name dialog
		if (PortalList.selected != undefined) do
		(
			NameType = 5
			NameFloater = newRolloutFloater "Rename Portal" 386 70
			addRollout NameRollout NameFloater
			NameRollout.NameText.text = PortalArray[CurrentPortal].Name
			setFocus NameRollout.NameText
		)
	)
	
	function RenamePortalCallback NewName = 
	(
		nameString = NewName
		if nameString == "" then
			nameString = uniqueName "Portal_NDLPR"
		if (nameString.count < 6) or (findString nameString "_NDLPR" == undefined) then
			nameString = nameString + "_NDLPR"
		PortalArray[CurrentPortal].name = nameString
		RefreshUI()
	)
	
	on TwoWayCheck changed checked do
	(
		-- if checked set two way flag in user-defined properties
		if (checked) then
		(
			properties = getUserPropBuffer PortalArray[CurrentPortal]
			i = findString properties "NDLTWOWAY"
			if i == undefined then
			(
				append properties "NDLTWOWAY\r\n"
				setUserPropBuffer PortalArray[CurrentPortal] properties
			)
		)
		else
		(
			properties = getUserPropBuffer PortalArray[CurrentPortal]
			i = findString properties "NDLTWOWAY"
			if i != undefined then
			(
				properties = replace properties i 11 ""
				setUserPropBuffer PortalArray[CurrentPortal] properties
			)
		)
		RefreshUI()
	)
	
	on MeshList selected selection do
	(
		-- RefreshButtons()
		CurrentMesh = selection
		select MeshArray[CurrentMesh]
		RefreshUI()
	)
	
	on AttachMeshButton picked object do
	(
		if (isGroupMember object) then
			setGroupMember object false
		attachObjects RoomArray[CurrentRoom] object move:false
		RefreshUI()
	)
	
	on AddSelectionButton pressed do
	(
		if (0 < CurrentRoom) and (CurrentRoom <= RoomArray.count) then
		(
			selectionArray = getCurrentSelection()
			for i = 1 to selectionArray.count do
			(
				if (isGroupMember selectionArray[i]) then 
					setGroupMember selectionArray[i] false
				attachObjects RoomArray[CurrentRoom] selectionArray[i] move:false
			)
			RefreshUI()
		)
	)
	
	on RemoveMeshButton pressed do
	(
		-- if mesh selected detach that mesh from currently selected room
		if (0 < CurrentMesh) and (CurrentMesh <= MeshArray.count) then
		(
			deleteItem RoomArray[CurrentRoom].children MeshArray[CurrentMesh]
		)
		RefreshUI()
	)
	
	on WallCheck changed checked do
	(
		if (checked) then
		(
			if (MeshArray[CurrentMesh].name.count < 6) or (findString MeshArray[CurrentMesh].name "_NDLWL" == undefined) then
				MeshArray[CurrentMesh].name += "_NDLWL"
			RefreshUI()
		)
		else
		(
			RetVal = findString MeshArray[CurrentMesh].name "_NDLWL"
			if (RetVal != Undefined) then
			(
				MeshArray[CurrentMesh].name = replace MeshArray[CurrentMesh].name RetVal 6 ""
			)
		)
		RefreshUI()
	)
)

-- --------------------NAMING DIALOG-----------------------------

rollout NameRollout "Enter name" width:376 height:40
(
	editText NameText "" pos:[72,8] width:184 height:16
	label lbl1 "Enter Name" pos:[8,12] width:64 height:16
	button NameOKButton "OK" pos:[272,8] width:88 height:24
	
	on NameText entered string do
	(
		NameOKButton.pressed()
	)
	
	on NameOKButton pressed do
	(
		case NameType of
		(
			1: PortalUtility.CreateGroupCallback NameText.text 	-- create group 
			2: PortalUtility.RenameGroupCallback NameText.text	-- rename group
			3: PortalUtility.CreateRoomCallback NameText.text	-- create room
			4: PortalUtility.RenameRoomCallback NameText.text	-- rename room
			5: PortalUtility.RenamePortalCallback NameText.text	-- rename portal
			6: PortalUtility.FromSelectionCallback NameText.text
		)
		closeRolloutFloater NameFloater
	)
)

-- --------------------INITIALIZATION----------------------------

on execute do
(
	try (closeRolloutFloater mainFloater); catch()
	try (callbacks.removeScripts id:#PortalUtilityCallbacks); catch()
	mainFloater = newRolloutFloater "Gamebryo Portal Utility" 410 620
	AddRollout PortalUtility mainFloater
	PortalUtility.RefreshUI()
	callbacks.addScript #filePostOpen "try (PortalUtility.FileOpenCallback()); catch ()" id:#PortalUtilityCallbacks persistent:false
	callbacks.addScript #selectionSetChanged "try (PortalUtility.SelectionChangedCallback()); catch()" id:#PortalUtilityCallbacks persistent:false
	callbacks.addScript #systemPostReset "try (PortalUtility.FileOpenCallback()); catch ()" id:#PortalUtilityCallbacks persistent:false
	callbacks.addScript #systemPostNew "try (PortalUtility.FileOpenCallback()); catch ()" id:#PortalUtilityCallbacks persistent:false
	callbacks.addScript #nodeRenamed "try (PortalUtility.RefreshUI());catch()" id:#PortalUtilityCallbacks persistent:false
	callbacks.addScript #nodeLinked "try (PortalUtility.RefreshUI());catch()" id:#PortalUtilityCallbacks persistent:false
	callbacks.addScript #nodeUnlinked "try (PortalUtility.RefreshUI());catch()" id:#PortalUtilityCallbacks persistent:false
)

on PortalUtility close do
(
	callbacks.removescripts id:#PortalUtilityCallbacks
)

)
-- --------------------END MACROSCRIPT---------------------------