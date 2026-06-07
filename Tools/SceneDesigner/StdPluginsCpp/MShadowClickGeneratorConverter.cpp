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

#include "MShadowClickGeneratorConverter.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace System::Collections;

//---------------------------------------------------------------------------
bool MShadowClickGeneratorConverter::GetStandardValuesSupported(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
bool MShadowClickGeneratorConverter::GetStandardValuesExclusive(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
TypeConverter::StandardValuesCollection*
    MShadowClickGeneratorConverter::GetStandardValues(
    ITypeDescriptorContext*)
{
    ArrayList* pmValues = new ArrayList();
    const NiTObjectPtrSet<NiShadowClickGeneratorPtr>& kShadowClickGenerators =
        NiShadowManager::GetShadowClickGenerators();
    for (unsigned int ui = 0; ui < kShadowClickGenerators.GetSize(); ui++)
    {
        pmValues->Add(new String(kShadowClickGenerators.GetAt(ui)->GetName()));
    }

    return new StandardValuesCollection(pmValues);
}
//---------------------------------------------------------------------------
