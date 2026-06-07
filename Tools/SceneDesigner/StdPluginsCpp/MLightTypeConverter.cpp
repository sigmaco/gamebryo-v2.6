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

#include "MLightTypeConverter.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace System::Collections;

//---------------------------------------------------------------------------
bool MLightTypeConverter::GetStandardValuesSupported(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
bool MLightTypeConverter::GetStandardValuesExclusive(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
TypeConverter::StandardValuesCollection*
    MLightTypeConverter::GetStandardValues(ITypeDescriptorContext*)
{
    ArrayList* pmValues = new ArrayList();
    pmValues->Add(new String(NiLightComponent::LT_AMBIENT));
    pmValues->Add(new String(NiLightComponent::LT_DIRECTIONAL));
    pmValues->Add(new String(NiLightComponent::LT_POINT));
    pmValues->Add(new String(NiLightComponent::LT_SPOT));

    return new StandardValuesCollection(pmValues);
}
//---------------------------------------------------------------------------
