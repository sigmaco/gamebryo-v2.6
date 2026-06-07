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

#include "MSequenceIDConverter.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace System::Collections;

//---------------------------------------------------------------------------
bool MSequenceIDConverter::GetStandardValuesSupported(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
bool MSequenceIDConverter::GetStandardValuesExclusive(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
TypeConverter::StandardValuesCollection*
    MSequenceIDConverter::GetStandardValues(ITypeDescriptorContext* pmContext)
{
    if (pmContext != NULL && pmContext->PropertyDescriptor != NULL &&
       pmContext->PropertyDescriptor->GetType() ==
            __typeof(MEntityPropertyDescriptor))
    {
        MEntityPropertyDescriptor* pmDescriptor = static_cast<
            MEntityPropertyDescriptor*>(pmContext->PropertyDescriptor);
        MPropertyContainer* pmEntity = pmDescriptor->PropertyContainer;

        if (!pmEntity->HasProperty(ms_strSequenceIDsName))
        {
            return TypeConverter::GetStandardValues(pmContext);
        }

        ArrayList* pmValues = new ArrayList();

        unsigned int uiCount = pmEntity->GetElementCount(
            ms_strSequenceIDsName);

        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            pmValues->Add(pmEntity->GetPropertyData(ms_strSequenceIDsName,
                ui));
        }

        return new StandardValuesCollection(pmValues);
    }

    return TypeConverter::GetStandardValues(pmContext);
}
//---------------------------------------------------------------------------
bool MSequenceIDConverter::CanConvertTo(ITypeDescriptorContext* pmContext,
    Type* pmDestinationType)
{
    if (pmContext != NULL && pmContext->PropertyDescriptor != NULL &&
        pmContext->PropertyDescriptor->GetType() ==
            __typeof(MEntityPropertyDescriptor) &&
        pmDestinationType == __typeof(unsigned int))
    {
        return true;
    }

    return TypeConverter::CanConvertTo(pmContext,
        pmDestinationType);
}
//---------------------------------------------------------------------------
Object* MSequenceIDConverter::ConvertTo(ITypeDescriptorContext* pmContext,
    CultureInfo* pmCulture, Object* pmValue, Type* pmDestinationType)
{
    if (pmContext != NULL && pmContext->PropertyDescriptor != NULL &&
        pmContext->PropertyDescriptor->GetType() ==
            __typeof(MEntityPropertyDescriptor) &&
        pmDestinationType == __typeof(String) &&
        pmValue->GetType() == __typeof(unsigned int))
    {
        MEntityPropertyDescriptor* pmDescriptor = static_cast<
            MEntityPropertyDescriptor*>(pmContext->PropertyDescriptor);
        MPropertyContainer* pmPropertyContainer = 
            pmDescriptor->PropertyContainer;

        unsigned int uiSequenceID = *static_cast<__box unsigned int*>(
            pmValue);

        if (uiSequenceID == NiActorManager::INVALID_SEQUENCE_ID)
        {
            // The sequence ID is invalid. Display an empty string.
            return String::Empty;
        }

        if (!pmPropertyContainer->HasProperty(ms_strSequenceIDsName) ||
            !pmPropertyContainer->HasProperty(ms_strSequenceNamesName))
        {
            return TypeConverter::ConvertTo(pmContext, pmCulture, pmValue,
                pmDestinationType);
        }

        unsigned int uiCount = pmPropertyContainer->GetElementCount(
            ms_strSequenceIDsName);
        MAssert(uiCount == pmPropertyContainer->GetElementCount(
            ms_strSequenceNamesName), "Element counts differ!");

        if (uiCount == 0)
        {
            // This can happen before the actor component has resolved its
            // NiActorManager pointer. In this case, just display the
            // sequence ID with no name.
            return String::Format("[{0}]", uiSequenceID.ToString());
        }

        unsigned int uiIndex;
        for (uiIndex = 0; uiIndex < uiCount; uiIndex++)
        {
            unsigned int uiTempSeqID = *dynamic_cast<__box unsigned int*>(
                pmPropertyContainer->GetPropertyData(ms_strSequenceIDsName,
                uiIndex));
            if (uiTempSeqID == uiSequenceID)
            {
                break;
            }
        }
        MAssert(uiIndex < uiCount, "Sequence ID not found in properties!");

        // Build the string as "[SequenceID] SequenceName".
        return String::Format("[{0}] {1}", uiSequenceID.ToString(),
            pmPropertyContainer->GetPropertyData(ms_strSequenceNamesName,
            uiIndex));
    }

    return TypeConverter::ConvertTo(pmContext, pmCulture, pmValue,
        pmDestinationType);
}
//---------------------------------------------------------------------------
bool MSequenceIDConverter::CanConvertFrom(ITypeDescriptorContext* pmContext,
    Type* pmType)
{
    if (pmContext != NULL && pmContext->PropertyDescriptor != NULL &&
        pmContext->PropertyDescriptor->GetType() ==
            __typeof(MEntityPropertyDescriptor) &&
        pmType == __typeof(String))
    {
        return true;
    }

    return TypeConverter::CanConvertFrom(pmContext, pmType);
}
//---------------------------------------------------------------------------
Object* MSequenceIDConverter::ConvertFrom(ITypeDescriptorContext* pmContext,
    CultureInfo* pmInfo, Object* pmValue)
{
    if (pmContext != NULL && pmContext->PropertyDescriptor != NULL &&
        pmContext->PropertyDescriptor->GetType() ==
            __typeof(MEntityPropertyDescriptor) &&
        pmValue->GetType() == __typeof(String))
    {
        MEntityPropertyDescriptor* pmDescriptor = static_cast<
            MEntityPropertyDescriptor*>(pmContext->PropertyDescriptor);
        MPropertyContainer* pmPropertyContainer = 
            pmDescriptor->PropertyContainer;

        String* strValue = static_cast<String*>(pmValue);

        if (!pmPropertyContainer->HasProperty(ms_strSequenceIDsName) ||
            !pmPropertyContainer->HasProperty(ms_strSequenceNamesName))
        {
            return TypeConverter::ConvertFrom(pmContext, pmInfo, pmValue);
        }

        // Parse the format "[SequenceID] SequenceName".
        int iOpenBracket = strValue->IndexOf('[');
        int iCloseBracket = strValue->IndexOf(']', iOpenBracket + 1);
        MAssert(iOpenBracket != -1 && iCloseBracket != -1, "Invalid format!");

        String* strSequenceID = strValue->Substring(iOpenBracket + 1,
            iCloseBracket - (iOpenBracket + 1));
        try
        {
            unsigned int uiSequenceID = UInt32::Parse(strSequenceID);
            return __box(uiSequenceID);
        }
        catch(...)
        {
            throw new ArgumentException(String::Format("Cannot convert '{0}' "
                "to a sequence ID", pmValue));
        }
    }

    return TypeConverter::ConvertFrom(pmContext, pmInfo, pmValue);
}
//---------------------------------------------------------------------------
