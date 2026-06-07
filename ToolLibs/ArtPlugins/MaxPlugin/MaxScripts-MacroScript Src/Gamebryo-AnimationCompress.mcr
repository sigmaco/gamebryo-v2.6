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

macroScript AnimCompressUtility category:"Gamebryo Tools" toolTip:"Animation Compression Utility" icon:#("Gamebryo", 8)
(
-- --------------------GLOBAL VARIABLES--------------------------
Global AnimCompressRollout
local AnimCompressFloater
local SequenceNamesFromIndex
local RootsFromIndex

local CurrentActor = undefined
local CurrentSequence = undefined
local CurrentObjects = #()
local ActorArray = #()
local ObjectArray = #()
local CurrentSliderValue = 100

local SEQUENCE_ONLY = 1
local OBJECT_ONLY = 2
local OBJECT_AND_SEQUENCE = 3

-- --------------------GLOBAL FUNCTIONS--------------------------

include "Gamebryo\AnimationHelpers.ms"

-----------------------------------------------------------------------------
-- 
-----------------------------------------------------------------------------
function GetGlobalRatioKey =
(
	Key = undefined
	if ActorArray != undefined and CurrentActor != undefined and 
		CurrentActor > 0 and CurrentActor <= ActorArray.count then
	(
		Notes = GetActorNoteTrack ActorArray[CurrentActor]
		if Notes != undefined then
		(
			Index = undefined
			keyCount = NiGetNoteKeyCount Notes
			for i = 1 to keyCount do
			(
				ratio = GetSequenceData Notes.keys[i] "-GlobalRatio"
				if ratio != undefined then
				(
					Index = i
				)
			)
			if Index == undefined then
			(
				Key = FindBlankKey Notes 0
				if Key == undefined then
					Key = addNewNoteKey Notes.keys 0
			)
			else -- if there was an existing global setting
			(
				Key = Notes.keys[Index]
			)
		)
	)
	return Key
)

-----------------------------------------------------------------------------
-- 
-----------------------------------------------------------------------------
function GetObjectRatioKey Object = 
(
	Key = undefined
	if Object != undefined then
	(
		if hasNoteTracks Object then
		(
			-- get the first available note track
			Notes = getNoteTrack Object 1
			if Notes != undefined then
			(
				Index = getNoteKeyIndex Notes 0
				if Index != undefined then
					Key = Notes.keys[Index]
			)
		)
	)
	return Key
)

-----------------------------------------------------------------------------
-- 
-----------------------------------------------------------------------------
function SetObjectRatioKey Object =
(
	Key = undefined
	if Object != undefined then
	(
		if hasNoteTracks Object then
		(
			-- get the first available note track
			Notes = getNoteTrack Object 1
			if Notes != undefined then
			(
				Index = getNoteKeyIndex Notes 0
				if Index == undefined then
				(
					Key = FindBlankKey Notes 0
					if Key == undefined then
						Key = addNewNoteKey Notes.keys 0
				)
				else -- if there was an existing global setting
				(
					Key = Notes.keys[Index]
				)
			)
		)
		else -- create a new note track
		(
			Notes = noteTrack "AnimCompressTrack"
			addNoteTrack Object Notes
			Key = addNewNoteKey Notes.keys 0
		)
	)
	return Key
)

-----------------------------------------------------------------------------
-- 
-----------------------------------------------------------------------------
function GetSequenceRatioKey sequenceName =
(
	-- get the note track of the layer root of the
	-- currently selected sequence
	index = 1
	bFoundSequence = false
	for sequence in SequenceNamesFromIndex do
	(
		if sequence == sequenceName then 
		(
			bFoundSequence = true
			exit
		)
		index += 1
	)
	
	if bFoundSequence then
	(
		Object = RootsFromIndex[index]
		for i = 1 to numNoteTracks Object do
		(
			Notes = getNoteTrack Object i
			-- iterate through this object's keys
			for key in Notes.keys do
			(
				-- search the key for the name
				stringIndex = findString key.value ("-name " + sequenceName)
				if stringIndex != undefined then
				(
					endIndex = stringIndex + sequenceName.count + 5
					if key.value[endIndex + 1] == " " then
					(
						-- we've found it, return this key
						return key
					)
					else if endIndex == key.value.count then
					(
						-- it may be the last string in the note
						return key
					)
				)
			)
		)
	)
	
	return undefined
)

-----------------------------------------------------------------------------
-- 
-----------------------------------------------------------------------------
function GetAllSequencesUp Object = 
(
	-- iterate through all roots above that object and append their lists
	-- Fill out SequenceNamesFromIndex so we don't have to parse the string array to
	-- 	get the correct sequence name when writing to user-defined props.
	ReturnList = #()
	SequenceNamesFromIndex = #()
	RootsFromIndex = #()
	RootArray = #()
	GetLayerRootsUp Object RootArray
	for Root in RootArray do
	(
		-- find all sequences
		SequenceNameArray = GetSequenceList Root
		for Sequence in SequenceNameArray do
		(
			append SequenceNamesFromIndex Sequence
			append RootsFromIndex Root
			TempString = Sequence + " - (" + Root.name + ")"
			append ReturnList TempString
		)
	)
	return ReturnList
)

-----------------------------------------------------------------------------
-- 
-----------------------------------------------------------------------------
function GetAllSequencesDown Object = 
(
	-- iterate through all roots below that object and append their lists
	-- Fill out SequenceNamesFromIndex so we don't have to parse the string array to
	-- 	get the correct sequence name when writing to user-defined props.
	ReturnList = #()
	SequenceNamesFromIndex = #()
	RootsFromIndex = #()
	RootArray = #()
	GetLayerRootsDown Object RootArray
	for Root in RootArray do
	(
		-- find all sequences
		SequenceNameArray = GetSequenceList Root
		for Sequence in SequenceNameArray do
		(
			append SequenceNamesFromIndex Sequence
			append RootsFromIndex Root
			key = GetSequenceRatioKey Sequence
			dontCompress = GetSequenceData key "-SeqDontCompress"
			value = GetSequenceData key "-SeqRatio"
			if value != undefined or dontCompress == "true" then
			(
				TempString = Sequence + " - (" + Root.name + ") *"
			)
			else
			(
				TempString = Sequence + " - (" + Root.name + ")"
			)
			append ReturnList TempString
		)
	)
	return ReturnList
)

-----------------------------------------------------------------------------
-- 
-----------------------------------------------------------------------------
function SetSequenceOverrides Object SequenceIndex dataType NewValue =
(
	if Object != undefined then
	(
		key = GetObjectRatioKey Object
		if key != undefined then
		(
			SequenceName = SequenceNamesFromIndex[SequenceIndex]
			dataName = "-" + SequenceName + "Override"
			bOverridden = GetSequenceData key dataName
			if bOverridden != "true" then
			(
				-- find out if there is an object override already
				Ratio = GetSequenceData key "-ObjRatio"
				DontCompress = GetSequenceData key "-ObjDontCompress"
				if Ratio != undefined or DontCompress == "true" then
				(
					-- we've found an overriden object, add objseq key
					dataName = "-" + SequenceName + dataType
					SetSequenceData key dataName NewValue
				)
			)
		)
	)
	for Child in Object.children do
	(
		SetSequenceOverrides Child SequenceIndex dataType NewValue
	)
)

-----------------------------------------------------------------------------
-- 
-----------------------------------------------------------------------------
function RemoveSequenceOverrides Object SequenceIndex =
(
	-- this method is given a sequence name, then removes all objseq 
	-- overrides in the scene that use that sequence
	if Object != undefined then
	(
		key = GetObjectRatioKey Object
		if key != undefined then
		(
			SequenceName = SequenceNamesFromIndex[SequenceIndex]
			dataName = "-" + SequenceName + "Override"
			bOverridden = GetSequenceData key dataName
			if bOverridden != "true" then
			(
				DeleteSequenceData key dataName
				dataName = "-" + SequenceName + "Ratio"
				DeleteSequenceData key dataName
				dataName = "-" + SequenceName + "DontCompress"
				DeleteSequenceData key dataName
				dataName = "-" + SequenceName + "CompressFloats"
				DeleteSequenceData key dataName
			)
		)
	)
	for Child in Object.children do
	(
		RemoveSequenceOverrides Child SequenceIndex
	)
)

-----------------------------------------------------------------------------
-- 
-----------------------------------------------------------------------------
function DeleteSequenceOverrides key =
(
	-- this method is given an object key and removes all objseq overrides
	for SequenceName in SequenceNamesFromIndex do
	(
		dataName = "-" + SequenceName + "Override"
		bOverridden = GetSequenceData key dataName
		if bOverridden != "true" then
		(
			DeleteSequenceData key dataName
			dataName = "-" + SequenceName + "Ratio"
			DeleteSequenceData key dataName
			dataName = "-" + SequenceName + "DontCompress"
			DeleteSequenceData key dataName
			dataName = "-" + SequenceName + "CompressFloats"
			DeleteSequenceData key dataName
		)
	)
)

-----------------------------------------------------------------------------
-- 
-----------------------------------------------------------------------------
function GetMostCommonMember varArray  =
(
    if varArray.count == 0 then return #(undefined, 0)
    
    sort varArray
    bestValue = varArray[1]
    bestCount = 1
    
    curValue = varArray[1]
    curCount = 1

    for i = 2 to varArray.count do
    (
        -- if we are already on the best value, just keep adding
        -- to the current counts for both
        if varArray[i] != curValue then
        (
            curCount = 0
            curValue = varArray[i]
        )
        
        if curValue == bestValue then 
        (
            bestCount = bestCount + 1
            curCount = curCount + 1
        )
        else
        (
            curCount = curCount + 1
            if curCount > bestCount then
            (
                bestCount = curCount
                bestValue = curValue
            )
        )
    )

    return #(bestValue, bestCount)
)
-- --------------------UI DEFINITION-----------------------------

rollout AnimCompressRollout "Compression Utility" width:200 height:675
(
	groupBox GlobalGroup "Global Compression" pos:[8,8] width:184 height:192
		checkbox GlobalDontCompressCheck "Don't use compression" pos:[16, 32] width:168 height:16
		dropDownList ActorRootDropdown "Actor Root" pos:[16,56] width:168 height:40
		slider GlobalSlider "Actor Compression Ratio" pos:[16,104] width:168 height:44 range:[1,100,100] type:#integer
		label GlobalSizeLabel "Size" pos:[24,152] width:24 height:16
		label GlobalQualityLabel "Quality" pos:[144,152] width:40 height:16
		checkbox GlobalCompressFloatCheck "Use compact keys" pos:[16,176] width:168 height:16 checked:true
	groupBox SelectionGroup "Selection Control" pos:[8,208] width:184 height:256
		radioButtons SelectionRadio "" pos:[16,232] width:133 height:48 labels:#("Sequence Only", "Object Only", "Object and Sequence") default:1 columns:1
		dropDownList SequenceDropdown "Sequence" pos:[16,288] width:168 height:40
		multilistBox ObjectList "Object" pos:[16,328] width:168 height:8
	groupBox OverrideGroup "Compression Override" pos:[8,472] width:184 height:224	
		label CurrentObjectLabel "Object:" pos:[16,496] width:168 height:16
		checkbox OverrideCheck "Override global for this object" pos:[16,520] width:168 height:16
		checkbox OverrideNoCompressCheck "Do not compress this object" pos:[16,544] width:168 height:16
		slider OverrideSlider "Compression Ratio" pos:[16,568] width:168 height:44 range:[1,100,100] type:#integer
		label OverrideSizeLabel "Size" pos:[24,616] width:24 height:16
		label OverrideQualityLabel "Quality" pos:[144,616] width:40 height:16
		checkbox OverrideCompressFloatCheck "Use compact keys" pos:[16,640] width:168 height:16 checked:true
		button MultipleSelectionResetBtn "Values differ. Force all to match" pos:[16,664] width:168 height:16 visible:false

-- --------------------MEMBER FUNCTIONS--------------------------

    local GlobalKey = undefined
    local GlobalDontCompress = undefined
    local GlobalRatio = undefined
    local GlobalCompressFloats = undefined
    
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	function RefreshGlobalVars =
    (
        GlobalKey = GetGlobalRatioKey()
		GlobalDontCompress = ""
		GlobalRatio = undefined
		GlobalCompressFloats = undefined
		if GlobalKey != undefined then
		(
			GlobalRatio = GetSequenceData GlobalKey "-GlobalRatio"
			if GlobalRatio != undefined then
			(
				GlobalSlider.value = (GlobalRatio as integer)
			)
			GlobalCompressFloats = GetSequenceData GlobalKey "-GlobalCompressFloats"
			if GlobalCompressFloats != "false" then
				GlobalCompressFloatCheck.checked = true
			else
				GlobalCompressFloatCheck.checked = false
			GlobalDontCompress = GetSequenceData GlobalKey "-GlobalDontCompress"
			if GlobalDontCompress == "true" then
				GlobalDontCompressCheck.checked = true
			else
				GlobalDontCompressCheck.checked = false
		)
    )
    
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
    function RefreshActorRootUI =
    (
        StringArray = #()
		for Actor in ActorArray do
		(
			append StringArray Actor.name
		)
		ActorRootDropdown.items = StringArray
		ActorRootDropdown.enabled = true
		CurrentActor = ActorRootDropdown.selection
    )
    
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
    function SetOptionalUI enable =
    (
        GlobalSlider.enabled = enable
		GlobalSizeLabel.enabled = enable
		GlobalQualityLabel.enabled = enable
		GlobalCompressFloatCheck.enabled = enable
		SelectionRadio.enabled = enable
		SequenceDropdown.enabled = enable
		ObjectList.enabled = enable
		CurrentObjectLabel.enabled = enable
		OverrideNoCompressCheck.enabled = enable
		OverrideCheck.enabled = enable
		OverrideSlider.enabled = enable
		OverrideSizeLabel.enabled = enable
		OverrideQualityLabel.enabled = enable
		OverrideCompressFloatCheck.enabled = enable
     	MultipleSelectionResetBtn.visible = enable
    )
    
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
    function SetGlobalUI enable =
    (
        GlobalDontCompressCheck.enabled = enable
		GlobalSlider.enabled = enable
		GlobalSizeLabel.enabled = enable
		GlobalQualityLabel.enabled = enable
		GlobalCompressFloatCheck.enabled = enable
    )
   
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
    function SetOverrideUI enabled ratio compressFloats dontCompress  =
    (
        OverrideCheck.enabled = true
        
        if enabled == true then
        (
            MultipleSelectionResetBtn.visible = false
            
		    if dontCompress == "true" then
		    (
			    OverrideCheck.checked = true
			    OverrideNoCompressCheck.enabled = true
			    OverrideNoCompressCheck.checked = true
			    OverrideSlider.enabled = false
			    OverrideSlider.value = GlobalSlider.value
			    OverrideSizeLabel.enabled = false
			    OverrideQualityLabel.enabled = false
			    OverrideCompressFloatCheck.enabled = false
			    OverrideCompressFloatCheck.checked = GlobalCompressFloatCheck.checked
		    )
		    else
		    (
			    if ratio != undefined then
			    (
			        OverrideCheck.checked = true
				    OverrideNoCompressCheck.enabled = true
				    OverrideNoCompressCheck.checked = false
				    OverrideSlider.value = (ratio as integer)
				    OverrideSlider.enabled = true
				    OverrideSizeLabel.enabled = true
				    OverrideQualityLabel.enabled = true
				    OverrideCompressFloatCheck.enabled = true
				    if compressFloats != "false" then
					    OverrideCompressFloatCheck.checked = true
				    else
					    OverrideCompressFloatCheck.checked = false
			    )
			    else
			    (
				    OverrideNoCompressCheck.enabled = false
				    OverrideNoCompressCheck.checked = false
				    OverrideCheck.checked = false
				    OverrideSlider.value = GlobalSlider.value
				    OverrideSlider.enabled = false
				    OverrideSizeLabel.enabled = false
				    OverrideQualityLabel.enabled = false
				    OverrideCompressFloatCheck.enabled = false
				    OverrideCompressFloatCheck.checked = GlobalCompressFloatCheck.checked
			    )
		    )
		)
		else
		(
		    OverrideCheck.checked = false
		    MultipleSelectionResetBtn.visible = false
		    CurrentObjectLabel.enabled = false
			OverrideNoCompressCheck.enabled = false
			OverrideCheck.enabled = false
			OverrideSlider.enabled = false
			OverrideSizeLabel.enabled = false
			OverrideQualityLabel.enabled = false
			OverrideCompressFloatCheck.enabled = false
		)
    )
     
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
    function RefreshSequenceOverrideUI = 
    (
        local selected = false
        
        -- fill out the sequence list
		if CurrentActor != undefined then
			SequenceDropdown.items = GetAllSequencesDown ActorArray[CurrentActor]
		ObjectList.enabled = false
		CurrentSequence = SequenceDropdown.selection
		
		OverrideNoCompressCheck.caption = "Don't compress this seq."
		OverrideCheck.caption = "Override global for this seq."
		
		ratio = undefined
		compressFloats = undefined
		dontCompress = undefined
		if CurrentSequence != undefined and CurrentSequence > 0 then
		(
			CurrentObjectLabel.caption = "Sequence: " + SequenceNamesFromIndex[CurrentSequence]
		    CurrentObjectLabel.enabled = true
			OverrideNoCompressCheck.enabled = false
				
			-- find the sequence key
			key = GetSequenceRatioKey SequenceNamesFromIndex[CurrentSequence]
			if key != undefined then
			(
				ratio = GetSequenceData key "-SeqRatio"
				compressFloats = GetSequenceData key "-SeqCompressFloats"
				dontCompress = GetSequenceData key "-SeqDontCompress"
			)
		
			SetOverrideUI true ratio compressFloats dontCompress
		)
		else -- if nothing is selected
		(
			SetOverrideUI false ratio compressFloats dontCompress
		)
    )
    
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
    function RefreshObjectOverrideUI =
    (
        local selected = false
		
        SequenceDropdown.enabled = false
		ObjectArray = #()
		GetAllChildren ActorArray[CurrentActor] ObjectArray
		StringArray = #()
		for Object in ObjectArray do
		(
			key = GetObjectRatioKey Object
			dontCompress = GetSequenceData key "-ObjDontCompress"
			ratio = GetSequenceData key "-ObjRatio"
			if dontCompress == "true" or ratio != undefined then
			(
				append StringArray (object.name + " *")
			)
			else
			(
				append StringArray Object.name
			)
			-- clean out blank tracks
			if hasNoteTracks Object then
			(
				Notes = getNoteTrack Object 1
				if Notes != undefined then
				(
					if Notes.keys.count == 1 and Notes.keys[1].value == "" then
					(
						deleteNoteTrack Object Notes
					)
				)
			)
		)
		ObjectList.items = StringArray
		CurrentObjects = ObjectList.selection as array
		
		OverrideNoCompressCheck.caption = "Don't compress this object"
		OverrideCheck.caption = "Override global for this object"
		ratio = undefined
		compressFloats = undefined
		dontCompress = undefined

		if CurrentObjects != undefined  and CurrentObjects.count > 0 then
		(
		    if CurrentObjects.count == 1 then
		    (
			    CurrentObjectLabel.caption = "Object: " + ObjectArray[CurrentObjects[1]].name
			)
			else
			(
			    CurrentObjectLabel.caption = "Object: " + "Mutiple Selected"
			)
			
		    CurrentObjectLabel.enabled = true
			OverrideNoCompressCheck.enabled = false
			
			bMultiSelectButNotSame = false
				
			ratioArray = #()
			compressFloatsArray = #()
			dontCompressArray = #()
			bNoKeyFound = false
			iArrayEntries = 0
			for i in CurrentObjects do
			(
				if ObjectArray[i] != undefined then
				(
					key = GetObjectRatioKey ObjectArray[i]
					if key != undefined then
					(
						newratio = GetSequenceData key "-ObjRatio"
						newcompressFloats = GetSequenceData key "-ObjCompressFloats"
						newdontCompress = GetSequenceData key "-ObjDontCompress"
						
						if newratio != undefined then 
						(
							append ratioArray newratio
						)
						
						if newcompressFloats != undefined then
						(
							append compressFloatsArray newcompressFloats
						)
						
						if newdontCompress != undefined then
						(
							append dontCompressArray newdontCompress
						)
						iArrayEntries = iArrayEntries + 1
					)
					if key == undefined then
					(
						bNoKeyFound = true
					)
				)
			)
			
			if bNoKeyFound == true and iArrayEntries != 0 then 
			(
				bMultiSelectButNotSame = true
			)
			
			
			if iArrayEntries > 0 do
			(
    			ratioValue = GetMostCommonMember ratioArray
    			ratio = ratioValue[1]
    			compressFloatsValue = GetMostCommonMember compressFloatsArray
    			compressFloats = compressFloatsValue[1]
    			dontCompressValue = GetMostCommonMember dontCompressArray
    			dontCompress = dontCompressValue[1]
    			
    			if ratioValue[2] != ratioArray.count or compressFloatsValue[2] != ratioArray.count or dontCompressValue[2] != dontCompressArray.count then
    			(
    				bMultiSelectButNotSame = true
    			)
			)
					
			SetOverrideUI true ratio compressFloats dontCompress
			
			if bMultiSelectButNotSame == true then
			(
				MultipleSelectionResetBtn.visible = true
				OverrideNoCompressCheck.enabled = false
				OverrideCheck.enabled = false
				OverrideSlider.enabled = false
				OverrideSizeLabel.enabled = false
				OverrideQualityLabel.enabled = false
				OverrideCompressFloatCheck.enabled = false
			)
			
		)
		else -- if nothing is selected
		(
			SetOverrideUI false ratio compressFloats dontCompress
		)
    )
    
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
    function RefreshObjectAndSequenceOverrideUI =
    (
        local selected = false
		
        SequenceDropdown.items = GetAllSequencesDown ActorArray[CurrentActor]
		CurrentSequence = SequenceDropdown.selection
		if CurrentSequence != undefined and CurrentSequence > 0 then
		(
			ObjectArray = #()
			GetAllChildren RootsFromIndex[CurrentSequence] ObjectArray
			StringArray = #()
			for Object in ObjectArray do
			(
				key = GetObjectRatioKey Object
				dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "DontCompress"
				dontCompress = GetSequenceData key dataName
				dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Ratio"
				ratio = GetSequenceData key dataName
				if dontCompress == "true" or ratio != undefined then
					append StringArray (object.name + " *")
				else
					append StringArray Object.name

			)
			ObjectList.items = StringArray
			CurrentObjects = ObjectList.selection as array
			
			OverrideNoCompressCheck.caption = "Don't compress this combo"
			OverrideCheck.caption = "Override global for this combo"
			
			if CurrentObjects != undefined  and CurrentObjects.count == 1 then
			(
				CurrentObjectLabel.caption = "(" + ObjectArray[CurrentObjects[1]].name + ", " + SequenceNamesFromIndex[CurrentSequence] + ")"
				selected = true
			)
			if CurrentObjects != undefined  and CurrentObjects.count > 1 then
			(
				CurrentObjectLabel.caption ="(" + "Mutiple Selected" + ", " + SequenceNamesFromIndex[CurrentSequence] + ")"
				selected = true
			)
		)
		
		if selected == true then
		(
			CurrentObjectLabel.enabled = true
			OverrideNoCompressCheck.enabled = false
			ratio = undefined
			compressFloats = undefined
			dontCompress = undefined
			comboOverride = undefined
			bMultiSelectButNotSame = false
				
            ratioArray = #()
			compressFloatsArray = #()
			dontCompressArray = #()
			comboOverrideArray = #()
			bNoKeyFound = false
			iArrayEntries = 0
			for i in CurrentObjects do
			(
				if ObjectArray[i] != undefined then
				(
					key = GetObjectRatioKey ObjectArray[i]
					if key != undefined then
					(
						dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Ratio"
						newratio = GetSequenceData key dataName
						dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "CompressFloats"
						newcompressFloats = GetSequenceData key dataName
						dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "DontCompress"
						newdontCompress = GetSequenceData key dataName
						dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Override"
						newcomboOverride = GetSequenceData key dataName
						
						if newratio != undefined then
							append ratioArray newratio
						
						if newcompressFloats != undefined then
							append compressFloatsArray newcompressFloats
						
						if newdontCompress != undefined then
							append dontCompressArray newdontCompress
						
						if newcomboOverride != undefined then
							append comboOverrideArray newcomboOverride
						
						iArrayEntries = iArrayEntries + 1
					)
					if key == undefined then
					(
						bNoKeyFound = true
					)
				)
			)
			
			if bNoKeyFound == true and iArrayEntries != 0 then 
			(
				bMultiSelectButNotSame = true
			)
			
			if iArrayEntries > 0 do
			(
    			ratioValue = GetMostCommonMember ratioArray
    			ratio = ratioValue[1]
    			compressFloatsValue = GetMostCommonMember compressFloatsArray
    			compressFloats = compressFloatsValue[1]
    			dontCompressValue = GetMostCommonMember dontCompressArray
    			dontCompress = dontCompressValue[1]
    			comboOverrideValue = GetMostCommonMember comboOverrideArray
    			comboOverride = comboOverrideValue[1]
    			
    			if ratioValue[2] != ratioArray.count or compressFloatsValue[2] != ratioArray.count or 
    			   dontCompressValue[2] != dontCompressArray.count or 
    			   comboOverrideValue[2] != comboOverrideArray.count then
    			(
    				bMultiSelectButNotSame = true
    			)
			)
						
			if comboOverride == "true" then
			(
				SetOverrideUI true ratio compressFloats dontCompress
				
				if bMultiSelectButNotSame == true then
			    (
				    MultipleSelectionResetBtn.visible = true
				    OverrideNoCompressCheck.enabled = false
				    OverrideCheck.enabled = false
				    OverrideSlider.enabled = false
				    OverrideSizeLabel.enabled = false
				    OverrideQualityLabel.enabled = false
				    OverrideCompressFloatCheck.enabled = false
			    )
			    if bMultiSelectButNotSame == false then
			    (
				    MultipleSelectionResetBtn.visible = false
			    )
			)
			else -- if comboOverride != "true"
			(
				OverrideNoCompressCheck.enabled = false
				OverrideCheck.enabled = true
				OverrideCheck.checked = false
				OverrideSlider.enabled = false
				OverrideSizeLabel.enabled = false
				OverrideQualityLabel.enabled = false
				OverrideCompressFloatCheck.enabled = false
				OverrideCompressFloatCheck.checked = GlobalCompressFloatCheck.checked	
				if ratio != undefined then
				(
					OverrideSlider.value = (ratio as integer)
				)
				else
				(
					-- get the object's ratio
					for i in CurrentObjects do
				    (
					    if ObjectArray[i] != undefined then
					    (
							key = GetObjectRatioKey ObjectArray[i]
							if key != undefined then
							(
								ratio = GetSequenceData key "-ObjRatio"
							)
							if ratio == undefined then
							(
								key = GetSequenceRatioKey SequenceNamesFromIndex[CurrentSequence]
								if key != undefined then
									ratio = GetSequenceData key "-SeqRatio"
							)
							if ratio == undefined then
								OverrideSlider.value = GlobalSlider.value
							else
								OverrideSlider.value = (ratio as integer)
						)
					)
				)
			)
			
			if bMultiSelectButNotSame == true then
			(
				MultipleSelectionResetBtn.visible = true
				OverrideNoCompressCheck.enabled = false
				OverrideCheck.enabled = false
				OverrideSlider.enabled = false
				OverrideSizeLabel.enabled = false
				OverrideQualityLabel.enabled = false
				OverrideCompressFloatCheck.enabled = false
			)
			if bMultiSelectButNotSame == false then
			(
				MultipleSelectionResetBtn.visible = false
			)
		)
		else -- if nothing is selected
		(
			SetOverrideUI false ratio compressFloats dontCompress
		)
    )
    
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
    function RefreshOverrideUI =
    (
    	local selected = false
		
		if SelectionRadio.state == SEQUENCE_ONLY then
		(
			RefreshSequenceOverrideUI()
    	)
		else if SelectionRadio.state == OBJECT_ONLY then
		(
			RefreshObjectOverrideUI()
		)
		else if SelectionRadio.state == OBJECT_AND_SEQUENCE then
		(
			RefreshObjectAndSequenceOverrideUI()
		)
		
    )
    
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	function RefreshUI =
	(
		ActorArray = #()
		GetActorRoots rootNode ActorArray
		
		RefreshGlobalVars()
		RefreshActorRootUI()
		
		if GlobalDontCompress == "true" then
		(
			SetOptionalUI false
			return true
		)
		
		if CurrentActor == undefined or CurrentActor < 1 then
		(
			GlobalDontCompressCheck.enabled = false
			SetOptionalUI false
			return true
		)
		
		SetGlobalUI true
		SelectionRadio.enabled = true
		SequenceDropdown.enabled = true
		ObjectList.enabled = true
		
		RefreshOverrideUI()
	)

-- --------------------EVENT HANDLERS----------------------------

	-----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
    on GlobalDontCompressCheck changed NewValue do
	(
		GlobalKey = GetGlobalRatioKey()
		if GlobalKey != undefined then
		(
			SetSequenceData GlobalKey "-GlobalRatio" ((GlobalSlider.value as integer) as string)
			SetSequenceData GlobalKey "-GlobalDontCompress" NewValue
			SetSequenceData GlobalKey "-GlobalCompressFloats" GlobalCompressFloatCheck.checked
		)
		RefreshUI()
	)

    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on ActorRootDropdown selected NewActor do
	(
		RefreshUI()
	)
	
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on GlobalSlider changed NewValue do
	(
		-- set the global parameter for compression ratio
		-- this is stored in the note track of the actor root
		GlobalKey = GetGlobalRatioKey()
		if GlobalKey != undefined then
		(
			SetSequenceData GlobalKey "-GlobalRatio" (NewValue as string)
			SetSequenceData GlobalKey "-GlobalDontCompress" GlobalDontCompressCheck.checked
			SetSequenceData GlobalKey "-GlobalCompressFloats" GlobalCompressFloatCheck.checked
			if OverrideSlider.enabled == false then
				OverrideSlider.value = NewValue
		)
		--RefreshUI()
	)
	
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on GlobalSlider buttonup do
	(
		RefreshUI()
	)
		
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on GlobalCompressFloatCheck changed NewValue do
	(
		-- set the global parameter for float compression
		GlobalKey = GetGlobalRatioKey()
		if GlobalKey != undefined then
		(
			SetSequenceData GlobalKey "-GlobalRatio" ((GlobalSlider.value as integer) as string)
			SetSequenceData GlobalKey "-GlobalDontCompress" GlobalDontCompressCheck.checked
			SetSequenceData GlobalKey "-GlobalCompressFloats" (NewValue as string)
		)
		RefreshUI()
	)
	
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on SelectionRadio changed newState do
	(
		RefreshUI()
	)
	
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on SequenceDropdown selected NewValue do
	(
		RefreshUI()
	)
	
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on ObjectList selectionEnd do
	(
		RefreshUI()
	)
	
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on OverrideNoCompressCheck changed NewValue do
	(
		setWaitCursor()
		-- set this flag on either the obect or the seq
		if SelectionRadio.state == SEQUENCE_ONLY then
		(
			-- find the sequence key
			key = GetSequenceRatioKey SequenceNamesFromIndex[CurrentSequence]
			if key != undefined then
			(
				SetSequenceData key "-SeqDontCompress" NewValue
				SetSequenceOverrides RootsFromIndex[CurrentSequence] CurrentSequence "DontCompress" NewValue
			)
		)
		else if SelectionRadio.state == OBJECT_ONLY then
		(
			-- set the override for the object's key
			for i in CurrentObjects do
			(
				if ObjectArray[i] != undefined then
				(
				    key = SetObjectRatioKey ObjectArray[i]
				    if key != undefined then
				    (
					    SetSequenceData key "-ObjDontCompress" NewValue
					    DeleteSequenceOverrides key
				    )
			    )
			)
		)
		else if SelectionRadio.state == OBJECT_AND_SEQUENCE then
		(
		    for i in CurrentObjects do
			(
			    -- set a sequence flag in the object's key
			    if ObjectArray[i] != undefined then
			    (
				    key = SetObjectRatioKey ObjectArray[i]
				    if key != undefined then
				    (
					    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "DontCompress"
					    SetSequenceData key dataName NewValue
					    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Override"
					    if NewValue == true then
						    SetSequenceData key dataName NewValue
					    else
						    SetSequenceData key dataName OverrideCheck.checked
				    )
			    )
			)
		)
		RefreshUI()
		setArrowCursor()
	)
	
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on OverrideCheck changed NewValue do
	(
		setWaitCursor()
		-- if we are removing the override, delete the param from the object/sequence
		if NewValue == false then
		(
			if SelectionRadio.state == SEQUENCE_ONLY then
			(
				-- delete from the sequence key
				key = GetSequenceRatioKey SequenceNamesFromIndex[CurrentSequence]
				if key != undefined then
				(
					DeleteSequenceData key "-SeqRatio"
					DeleteSequenceData key "-SeqCompressFloats"
					DeleteSequenceData key "-SeqDontCompress"
					RemoveSequenceOverrides rootNode CurrentSequence
				)
			)
			else if SelectionRadio.state == OBJECT_ONLY then
			(
				-- delete flag for the object's key
				for i in CurrentObjects do
			    (
				    if ObjectArray[i] != undefined then
				    (
					    key = SetObjectRatioKey ObjectArray[i]
					    if key != undefined then
					    (
						    DeleteSequenceData key "-ObjRatio"
						    DeleteSequenceData key "-ObjCompressFloats"
						    DeleteSequenceData key "-ObjDontCompress"
						    DeleteSequenceOverrides key
					    )
				    )
				)
			)
			else if SelectionRadio.state == OBJECT_AND_SEQUENCE then
			(
				-- delete the sequence flag in the object's key
				for i in CurrentObjects do
			    (
				    if ObjectArray[i] != undefined then
				    (
					    key = SetObjectRatioKey ObjectArray[i]
					    if key != undefined then
					    (
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Ratio"
						    DeleteSequenceData key dataName
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "CompressFloats"
						    DeleteSequenceData key dataName
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "DontCompress"
						    DeleteSequenceData key dataName
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Override"
						    DeleteSequenceData key dataName
					    )
				    )
				)
			)
		)
		else -- if it is being checked, create a key with the global value stored
		(
			if SelectionRadio.state == SEQUENCE_ONLY then
			(
				key = GetSequenceRatioKey SequenceNamesFromIndex[CurrentSequence]
				if key != undefined then
				(
					SetSequenceData key "-SeqRatio" (GlobalSlider.value as integer)
					SetSequenceData key "-SeqCompressFloats" true
					SetSequenceData key "-SeqDontCompress" false
					SetSequenceOverrides RootsFromIndex[CurrentSequence] CurrentSequence "Ratio" (GlobalSlider.value as integer)
					SetSequenceOverrides RootsFromIndex[CurrentSequence] CurrentSequence "CompressFloats" true
					SetSequenceOverrides RootsFromIndex[CurrentSequence] CurrentSequence "DontCompress" false
				)
			)
			else if SelectionRadio.state == OBJECT_ONLY then
			(
			    for i in CurrentObjects do
			    (
				    if ObjectArray[i] != undefined then
				    (
					    key = SetObjectRatioKey ObjectArray[i]
					    if key != undefined then
					    (
						    SetSequenceData key "-ObjRatio" (GlobalSlider.value as integer)
						    SetSequenceData key "-ObjCompressFloats" true
						    SetSequenceData key "-ObjDontCompress" false
						    DeleteSequenceOverrides key
					    )
				    )
				)
			)
			else if SelectionRadio.state == OBJECT_AND_SEQUENCE then
			(
				-- delete the sequence flag in the object's key
				for i in CurrentObjects do
			    (
				    if ObjectArray[i] != undefined then
				    (
					    key = SetObjectRatioKey ObjectArray[i]
					    if key != undefined then
					    (
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Ratio"
						    SetSequenceData key dataName (GlobalSlider.value as integer)
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "CompressFloats"
						    SetSequenceData key dataName true
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "DontCompress"
						    SetSequenceData key dataName false
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Override"
						    SetSequenceData key dataName true
					    )
				    )
				)
			)
		)
		RefreshUI()
		setArrowCursor()
	)
	
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on OverrideSlider changed NewValue do
	(
		CurrentSliderValue = NewValue
	)
	
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on OverrideSlider buttonup do
	(
		setWaitCursor()
		--depending on whether we are working on the object or the sequence, set the note track keys
		if SelectionRadio.state == SEQUENCE_ONLY then
		(
			-- find the sequence key
			key = GetSequenceRatioKey SequenceNamesFromIndex[CurrentSequence]
			if key != undefined then
			(
				SetSequenceData key "-SeqRatio" CurrentSliderValue
				SetSequenceOverrides RootsFromIndex[CurrentSequence] CurrentSequence "Ratio" CurrentSliderValue
			)
		)
		else if SelectionRadio.state == OBJECT_ONLY then
		(
			-- set the override for the object's key
			for i in CurrentObjects do
			(
			    if ObjectArray[i] != undefined then
			    (
				    key = SetObjectRatioKey ObjectArray[i]
				    if key != undefined then
				    (
					    SetSequenceData key "-ObjRatio" CurrentSliderValue
					    DeleteSequenceOverrides key
				    )
			    )
			)
		)
		else if SelectionRadio.state == OBJECT_AND_SEQUENCE then
		(
			-- set a sequence flag in the object's key
			for i in CurrentObjects do
			(
			    if ObjectArray[i] != undefined then
			    (
				    key = SetObjectRatioKey ObjectArray[i]
				    if key != undefined then
				    (
					    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Ratio"
					    SetSequenceData key dataName CurrentSliderValue
					    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Override"
					    SetSequenceData key dataName true
				    )
			    )
			)
		)
		RefreshUI()
		SetArrowCursor()
	)
	
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on OverrideCompressFloatCheck changed NewValue do
	(
		setWaitCursor()
		--depending on whether we are working on the object or the sequence, set the note track keys
		if SelectionRadio.state == SEQUENCE_ONLY then
		(
			-- find the sequence key
			key = GetSequenceRatioKey SequenceNamesFromIndex[CurrentSequence]
			if key != undefined then
			(
				SetSequenceData key "-SeqCompressFloats" NewValue
				SetSequenceOverrides RootsFromIndex[CurrentSequence] CurrentSequence "CompressFloats" NewValue
			)
		)
		else if SelectionRadio.state == OBJECT_ONLY then
		(
			-- set the override for the object's key
			for i in CurrentObjects do
			(
			    if ObjectArray[i] != undefined then
			    (
				    key = SetObjectRatioKey ObjectArray[i]
				    if key != undefined then
				    (
					    SetSequenceData key "-ObjCompressFloats" NewValue
					    DeleteSequenceOverrides key
				    )
			    )
			)
		)
		else if SelectionRadio.state == OBJECT_AND_SEQUENCE then
		(
			-- set a sequence flag in the object's key
			for i in CurrentObjects do
			(
			    if ObjectArray[i] != undefined then
			    (
				    key = SetObjectRatioKey ObjectArray[i]
				    if key != undefined then
				    (
					    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "CompressFloats"
					    SetSequenceData key dataName NewValue
					    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Override"
					    SetSequenceData key dataName true
				    )
			    )
			)
		)
		RefreshUI()
		setArrowCursor()
	)
	
    -----------------------------------------------------------------------------
	-- 
	-----------------------------------------------------------------------------
	on MultipleSelectionResetBtn pressed do
	(
		setWaitCursor()
	    if SelectionRadio.state == SEQUENCE_ONLY then
		(
			print "ERROR! Should not reach this!"
		)
		else if SelectionRadio.state == OBJECT_ONLY then
		(
			-- set the override for the object's key
			for i in CurrentObjects do
			(
			    if ObjectArray[i] != undefined then
			    (
				    key = SetObjectRatioKey ObjectArray[i]
				    if key != undefined then
				    (
					    if OverrideCheck.checked == false then
					    (
					        DeleteSequenceData key "-ObjRatio"
						    DeleteSequenceData key "-ObjCompressFloats"
						    DeleteSequenceData key "-ObjDontCompress"
						    DeleteSequenceOverrides key
						)
						else
						(
						    SetSequenceData key "-ObjCompressFloats" OverrideCompressFloatCheck.checked
					        SetSequenceData key "-ObjRatio" OverrideSlider.value
					        SetSequenceData key "-ObjDontCompress" OverrideNoCompressCheck.checked
					        DeleteSequenceOverrides key
						)
				    )
			    )
			)
		)
		else if SelectionRadio.state == OBJECT_AND_SEQUENCE then
		(
			-- set a sequence flag in the object's key
			for i in CurrentObjects do
			(
			    if ObjectArray[i] != undefined then
			    (
				    key = SetObjectRatioKey ObjectArray[i]
				    if key != undefined then
					(
    					if OverrideCheck.checked == false then
    					(
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Ratio"
						    DeleteSequenceData key dataName
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "CompressFloats"
						    DeleteSequenceData key dataName
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "DontCompress"
						    DeleteSequenceData key dataName
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Override"
						    DeleteSequenceData key dataName
						)
						else
						(
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Ratio"
						    SetSequenceData key dataName OverrideSlider.value
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "CompressFloats"
						    SetSequenceData key dataName OverrideCompressFloatCheck.checked
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "DontCompress"
						    SetSequenceData key dataName OverrideNoCompressCheck.checked
						    dataName = "-" + SequenceNamesFromIndex[CurrentSequence] + "Override"
						    SetSequenceData key dataName true
					    )
					)
			    )
			)
		)
		RefreshUI()
		setArrowCursor()
	)

)
-- --------------------END ROLLOUT-------------------------------

-- --------------------INITIALIZATION----------------------------

on execute do
(
	try(closeRolloutFloater AnimCompressFloater);catch()
	try (callbacks.removeScripts id:#AnimCompressUtilityCallbacks); catch()
	AnimCompressFloater = newRolloutFloater "Compression Utility" 216 730
	AddRollout AnimCompressRollout AnimCompressFloater
	AnimCompressRollout.RefreshUI()
	AnimCompressRollout.RefreshUI()
	callbacks.addScript #filePostOpen "try (AnimCompressRollout.RefreshUI()); catch(print (\"error in callback \"))" id:#AnimCompressUtilityCallbacks persistent:false
	callbacks.addScript #SystemPostReset "try (AnimCompressRollout.RefreshUI()); catch(print (\"error in callback \"))" id:#AnimCompressUtilityCallbacks persistent:false
	callbacks.addScript #SystemPostNew "try (AnimCompressRollout.RefreshUI()); catch(print (\"error in callback \"))" id:#AnimCompressUtilityCallbacks persistent:false
)

on AnimCompressRollout close do
(
	callbacks.removescripts id:#AnimCompressUtilityCallbacks
)

)
-- --------------------END MACROSCRIPT---------------------------


