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
#include "TerrainPluginPCH.h"

#include "MTerrainTextureSizeDropDown.h"

using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;
using namespace System::Collections;

//---------------------------------------------------------------------------
bool MTerrainTextureSizeDropDown::GetStandardValuesSupported(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
bool MTerrainTextureSizeDropDown::GetStandardValuesExclusive(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
TypeConverter::StandardValuesCollection*
    MTerrainTextureSizeDropDown::GetStandardValues(
    ITypeDescriptorContext*)
{
    ArrayList* pmValues = new ArrayList();
    pmValues->Add(__box((unsigned int) 2048));
    pmValues->Add(__box((unsigned int) 1024));
    pmValues->Add(__box((unsigned int) 512));
    pmValues->Add(__box((unsigned int) 256));
    pmValues->Add(__box((unsigned int) 128));
    pmValues->Add(__box((unsigned int) 64));
    pmValues->Add(__box((unsigned int) 32));
    pmValues->Add(__box((unsigned int) 16));

    return new StandardValuesCollection(pmValues);
}