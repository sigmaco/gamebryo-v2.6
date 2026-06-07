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

macroscript GamebryoExportSelected category:"Gamebryo Tools" 
	buttonText:"Gamebryo Export Selected" toolTip:"Gamebryo Export Selected" 
	Icon:#("Gamebryo", 11)
(
    strFilename = GetExportValue "NiFilename"
	GamebryoExportSelected strFilename
)