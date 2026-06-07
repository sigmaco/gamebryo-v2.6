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

#include "MShadowMapSizeHintConverter.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace System::Collections;

//---------------------------------------------------------------------------
bool MShadowMapSizeHintConverter::GetStandardValuesSupported(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
bool MShadowMapSizeHintConverter::GetStandardValuesExclusive(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
TypeConverter::StandardValuesCollection*
    MShadowMapSizeHintConverter::GetStandardValues(
    ITypeDescriptorContext*)
{
    ArrayList* pmValues = new ArrayList();
    pmValues->Add(__box((unsigned short) 2048));
    pmValues->Add(__box((unsigned short) 1024));
    pmValues->Add(__box((unsigned short) 512));
    pmValues->Add(__box((unsigned short) 256));
    pmValues->Add(__box((unsigned short) 128));
    pmValues->Add(__box((unsigned short) 64));
    pmValues->Add(__box((unsigned short) 32));
    pmValues->Add(__box((unsigned short) 16));
    pmValues->Add(__box((unsigned short) 8));
    pmValues->Add(__box((unsigned short) 4));
    pmValues->Add(__box((unsigned short) 2));
    pmValues->Add(__box((unsigned short) 1));

    return new StandardValuesCollection(pmValues);
}
//---------------------------------------------------------------------------
