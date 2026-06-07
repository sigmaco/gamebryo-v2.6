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

PrecacheGeometryCA = attributes "PrecacheGeometrySettings" version:3 attribID:#(0x47ba7c95, 0xa68a3bca)
( 

	parameters NiPSys rollout:params 
	( 
		Consistency		type:#integer default:1		animatable:false	ui:rdoConsistency
		
		KeepPositions	type:#boolean default:false animatable:false    ui:chkKeepPositions
		KeepNormals		type:#boolean default:false animatable:false    ui:chkKeepNormals
		KeepVertColors	type:#boolean default:false animatable:false    ui:chkKeepVertColors
		KeepUVCoords	type:#boolean default:false animatable:false    ui:chkKeepUVCoords
		KeepBoneIndices	type:#boolean default:false animatable:false    ui:chkKeepBoneIndices
		KeepBoneData	type:#boolean default:false animatable:false    ui:chkKeepBoneData
	) 

	rollout params "Gamebryo PreCache Params" 
	( 
		label	lblConsistency "Geometry Data Consistency:" align:#left
		radiobuttons rdoConsistency labels:#( "Default","Static (Unchanging)", "Mutable (Seldomly Changes)", "Volatile (Always Changes)") default:1 align:#left
		
		label    lblKeep			"Data to Preserve:" align:#left		offset:[0,10]
		checkbox chkKeepPositions	"Keep Positions"	align:#left
		checkbox chkKeepNormals		"Keep Normals"		align:#left
		checkbox chkKeepVertColors	"Keep Vertex Colors" align:#left
		checkbox chkKeepUVCoords	"Keep UV Coords"	align:#left
		checkbox chkKeepBoneIndices	"Keep Indices" align:#left
		checkbox chkKeepBoneData	"Keep Bone Data"	align:#left
	) 
	

) 


	