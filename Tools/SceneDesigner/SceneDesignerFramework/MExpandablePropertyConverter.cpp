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
#include "SceneDesignerFrameworkPCH.h"

#include "MExpandablePropertyConverter.h"
#include "MEntityPropertyDescriptor.h"
#include "ServiceProvider.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace System::Collections;

//---------------------------------------------------------------------------
bool MExpandablePropertyConverter::GetPropertiesSupported(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
PropertyDescriptorCollection* MExpandablePropertyConverter::GetProperties(
    ITypeDescriptorContext* pmContext, Object* pmValue, Attribute* amFilter[])
{
    MEntityPropertyDescriptor* pmEntityDescriptor =
        dynamic_cast<MEntityPropertyDescriptor*>(
        pmContext->PropertyDescriptor);
    if (pmEntityDescriptor == NULL)
    {
        return TypeConverter::GetProperties(pmContext, pmValue, amFilter);
    }

    NiEntityPropertyInterface* pkPropertyInterface =
        pmEntityDescriptor->PropertyContainer->PropertyInterface;
    
    const char* pcPropertyName = MStringToCharPointer(
        pmEntityDescriptor->PropertyName);
    NiFixedString kPropertyName = pcPropertyName;
    MFreeCharPointer(pcPropertyName);

    ArrayList* pmPropertyDescs = new ArrayList();

    NiFixedString kDisplayName;
    bool bSuccess = NIBOOL_IS_TRUE(pkPropertyInterface->GetDisplayName(
        kPropertyName, kDisplayName));
    MAssert(bSuccess, "Property name not found!");
    MAssert(kDisplayName.Exists(), "Property must be displayable to be used "
        "with MExpandablePropertyConverter!");

    ArrayList* pmPropAttributes = new ArrayList();
    NiDefaultErrorHandlerPtr spErrors;

    NiFixedString kCategory;
    bSuccess = NIBOOL_IS_TRUE(pkPropertyInterface->GetCategory(kPropertyName,
        kCategory));
    MAssert(bSuccess, "Property name not found!");
    if (kCategory.Exists())
    {
        pmPropAttributes->Add(new CategoryAttribute(kCategory));
    }
    else
    {
        if (!spErrors)
        {
            spErrors = NiNew NiDefaultErrorHandler(2);
        }
        spErrors->ReportError("No category specified for property.",
            "The property will be added to a general category.",
            pkPropertyInterface->GetName(), kPropertyName);
    }

    NiFixedString kDescription;
    bSuccess = NIBOOL_IS_TRUE(pkPropertyInterface->GetDescription(
        kPropertyName, kDescription));
    MAssert(bSuccess, "Property name not found!");
    if (kDescription.Exists())
    {
        pmPropAttributes->Add(new DescriptionAttribute(kDescription));
    }
    else
    {
        if (!spErrors)
        {
            spErrors = NiNew NiDefaultErrorHandler(1);
        }
        spErrors->ReportError("No description specified for property.",
            "No description will be displayed for this property.",
            pkPropertyInterface->GetName(), kPropertyName);
    }

    NiFixedString kSemanticType;
    bSuccess = NIBOOL_IS_TRUE(
        pkPropertyInterface->GetSemanticType(kPropertyName, kSemanticType));
    MAssert(bSuccess, "Property name not found!");
    MAssert(kSemanticType.Exists(), "Property must have a semantic type to "
        "be used with MExpandablePropertyConverter!");
    PropertyType* pmPropertyType = PropertyTypeService->LookupType(
        kSemanticType);
    MAssert(pmPropertyType != NULL, "Property semantic type must be found to "
        "be used with MExpandablePropertyConverter!");

    if (spErrors)
    {
        MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
            spErrors);
    }

    unsigned int uiCount;
    bSuccess = NIBOOL_IS_TRUE(
        pkPropertyInterface->GetElementCount(kPropertyName, uiCount));
    MAssert(bSuccess, "Property name not found!");
    unsigned int uiDisplayIndex = 0;
    for (unsigned int uiIndex = 0; uiIndex < uiCount; uiIndex++)
    {
        if (pmEntityDescriptor->PropertyContainer->GetPropertyData(
            kPropertyName, uiIndex) != NULL)
        {
            String* strDisplayName = String::Format("{0} [{1:D02}]",
                new String(kDisplayName), __box(uiDisplayIndex++));
            pmPropertyDescs->Add(new MEntityPropertyDescriptor(
                pmEntityDescriptor->PropertyContainer, kPropertyName, uiIndex,
                pmPropertyType->Editor, pmPropertyType->TypeConverter,
                strDisplayName, dynamic_cast<Attribute*[]>(
                pmPropAttributes->ToArray(__typeof(Attribute)))));
        }

    }

    PropertyDescriptor* amPropertyDescArray[] =
        dynamic_cast<PropertyDescriptor*[]>(pmPropertyDescs->ToArray(
        __typeof(PropertyDescriptor)));
    return new PropertyDescriptorCollection(amPropertyDescArray);
}
//---------------------------------------------------------------------------
IPropertyTypeService* MExpandablePropertyConverter::get_PropertyTypeService()
{
    if (ms_pmPropertyTypeService == NULL)
    {
        ms_pmPropertyTypeService = MGetService(IPropertyTypeService);
        MAssert(ms_pmPropertyTypeService != NULL, "Property type service "
            "not found!");
    }
    return ms_pmPropertyTypeService;
}
//---------------------------------------------------------------------------
IMessageService* MExpandablePropertyConverter::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
