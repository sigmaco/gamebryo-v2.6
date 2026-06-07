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

#include "MDepthBiasConverter.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
bool MDepthBiasConverter::CanConvertTo(ITypeDescriptorContext* pmContext,
    Type* pmDestinationType)
{
    if (pmContext != NULL &&
        pmContext->PropertyDescriptor != NULL &&
        pmContext->PropertyDescriptor->IsReadOnly &&
        pmDestinationType == __typeof(String))
    {
        return true;
    }

    return __super::CanConvertTo(pmContext, pmDestinationType);
}
//---------------------------------------------------------------------------
Object* MDepthBiasConverter::ConvertTo(ITypeDescriptorContext* pmContext,
    CultureInfo* pmCulture, Object* pmValue, Type* pmDestinationType)
{
    if (pmContext != NULL &&
        pmContext->PropertyDescriptor != NULL &&
        pmContext->PropertyDescriptor->IsReadOnly &&
        pmDestinationType == __typeof(String))
    {
        return new String("[Using Appropriate Default]");
    }

    return __super::ConvertTo(pmContext, pmCulture, pmValue,
        pmDestinationType);
}
//---------------------------------------------------------------------------
