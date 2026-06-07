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

MultiNifExportCA = attributes "MultiNifExportSettings" version:2 attribID:#(0x99977ad7, 0x72555b55)
( 

	parameters main rollout:params 
	( 
		MultiNifExportUseGlobalSettings 	type:#boolean	default:true animatable:false
		MultiNifExportFileName 				type:#string default:"" animatable:false
		MultiNifExportUseObjectName 	type:#boolean	default:true animatable:false
		MultiNifExportDeleteAfterExport 	type:#boolean	default:true animatable:false
		MultiNifExportRepairEmitterWithoutParticles type:#boolean default:true animatable:false
		MultiNifExportTranslateType 		type:#integer 	default:2 animatable:false
		MultiNifExportRotateType 			type:#integer 	default:0 animatable:false
		MultiNifExportScaleType 			type:#integer 	default:0 animatable:false
	) 

	rollout params "MultiNifExport Parameters" 
	( 
		checkbox UseGlobalSettingsCheck "Use Global Settings"
		label	 DividerLbl	"- OR -" align:#center offset:[0,10]
		checkbox UseObjectNameCheck "Use Object Name" offset:[0,10]
		label	 ExportFileNameLbl "Export Filename:" align:#left
		edittext ExportFileNameTxt ""
		checkbox DeleteAfterExportCheck "Delete After Export" offset:[0,10] 
		checkbox RepairEmitterCheck "Repair Emitter w/o Particles"
		
		dropdownlist TranslateType_dd "Translate Type" items:#("World Translation","Local Translation","No Translation") offset:[0,10] selection:3
		on TranslateType_dd selected i do MultiNifExportTranslateType = i-1 

		dropdownlist RotateType_dd "Rotate Type" items:#("World Rotation","Local Rotation","No Rotation") 
		on RotateType_dd selected i do MultiNifExportRotateType = i-1

		dropdownlist ScaleType_dd "Scale Type" items:#("World Scale","Local Scale","No Scale") 
		on ScaleType_dd selected i do MultiNifExportScaleType = i-1

		function UpdateForObjectName state =
		(
			if state == true do
			(
				ExportFileNameLbl.enabled = false
				ExportFileNameTxt.enabled = false
			)
			if state == false do
			(
				ExportFileNameLbl.enabled = true
				ExportFileNameTxt.enabled = true
			)
		)

		function UpdateForGlobals state=
		(
			if state == true do
			(
				UseObjectNameCheck.enabled = false
				ExportFileNameLbl.enabled = false
				ExportFileNameTxt.enabled = false
				DeleteAfterExportCheck.enabled = false
				RepairEmitterCheck.enabled = false
				TranslateType_dd.enabled = false
				RotateType_dd.enabled = false
				ScaleType_dd.enabled = false
			)

			if state == false do
			(
				UseObjectNameCheck.enabled = true
				UpdateForObjectName(MultiNifExportUseObjectName)
				DeleteAfterExportCheck.enabled = true
				RepairEmitterCheck.enabled = true
				TranslateType_dd.enabled = true
				RotateType_dd.enabled = true
				ScaleType_dd.enabled = true
			)
		)

		function UpdateUI = 
		(
			ScaleType_dd.selection = MultiNifExportScaleType + 1
			RotateType_dd.selection = MultiNifExportRotateType + 1
			TranslateType_dd.selection = MultiNifExportTranslateType + 1
			UpdateForGlobals(MultiNifExportUseGlobalSettings)	
			ExportFileNameTxt.text = MultiNifExportFileName
			UseGlobalSettingsCheck.checked = MultiNifExportUseGlobalSettings
			UseObjectNameCheck.checked = MultiNifExportUseObjectName
			DeleteAfterExportCheck.checked = MultiNifExportDeleteAfterExport
			RepairEmitterCheck.checked = MultiNifExportRepairEmitterWithoutParticles
		)
		
		on params open do 
		(
			UpdateUI()
		)

		on UseObjectNameCheck changed state do
		(
			MultiNifExportUseObjectName = state
			UpdateForObjectName(state)
		)
		
		on ExportFileNameTxt changed newValue do
		(
			MultiNifExportFileName = newValue
		)

		on UseGlobalSettingsCheck changed state do
		(
			MultiNifExportUseGlobalSettings = state
			UpdateForGlobals(state)
		)

		on DeleteAfterExportCheck changed newValue do
		(
			MultiNifExportDeleteAfterExport = newValue
		)
		
		on RepairEmitterCheck changed newValue do
		(
			MultiNifExportRepairEmitterWithoutParticles = newValue
		)

	) 
	

)
