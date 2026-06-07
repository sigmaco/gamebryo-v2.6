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

#include "MShadowTechniqueConverter.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace System::Collections;

//---------------------------------------------------------------------------
bool MShadowTechniqueConverter::GetStandardValuesSupported(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
bool MShadowTechniqueConverter::GetStandardValuesExclusive(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
TypeConverter::StandardValuesCollection*
    MShadowTechniqueConverter::GetStandardValues(
    ITypeDescriptorContext*)
{
    ArrayList* pmValues = new ArrayList();
    for (unsigned short us = 0;
        us < NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES; us++)
    {
        NiShadowTechnique* pkShadowTechnique =
            NiShadowManager::GetKnownShadowTechnique(us);
        if (pkShadowTechnique)
        {
            pmValues->Add(new String(pkShadowTechnique->GetName()));
        }
    }

    return new StandardValuesCollection(pmValues);
}
//---------------------------------------------------------------------------
