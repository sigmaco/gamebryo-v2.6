// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Precompiled Header
#include "StdPluginsCppPCH.h"

#include "MKfmFilenameEditor.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
String* MKfmFilenameEditor::GetDialogTitle()
{
    return "Choose KFM File";
}
//---------------------------------------------------------------------------
String* MKfmFilenameEditor::GetDialogFilter()
{
    return "KFM Files (*.kfm)|*.kfm";
}
//---------------------------------------------------------------------------
