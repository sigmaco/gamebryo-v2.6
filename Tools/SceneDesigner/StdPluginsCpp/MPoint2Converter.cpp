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

#include "MPoint2Converter.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace System::Collections;

//---------------------------------------------------------------------------
PropertyDescriptorCollection* MPoint2Converter::GetProperties(
    ITypeDescriptorContext* pmContext, Object* pmValue, Attribute* amFilter[])
{
    if (pmValue->GetType() == __typeof(MPoint2) &&
        pmContext->PropertyDescriptor != NULL)
    {
        MPoint2* pmPoint2 = static_cast<MPoint2*>(pmValue);

        MEntityPropertyDescriptor* pmEntityDescriptor =
            dynamic_cast<MEntityPropertyDescriptor*>(
            pmContext->PropertyDescriptor);

        ArrayList* pmPropertyDescs = new ArrayList();
        pmPropertyDescs->Add(new ElementDescriptor(pmPoint2, 0,
            pmEntityDescriptor, "x", amFilter));
        pmPropertyDescs->Add(new ElementDescriptor(pmPoint2, 1,
            pmEntityDescriptor, "y", amFilter));

        PropertyDescriptor* amPropertyDescArray[] =
            dynamic_cast<PropertyDescriptor*[]>(pmPropertyDescs->ToArray(
            __typeof(PropertyDescriptor)));
        return new PropertyDescriptorCollection(amPropertyDescArray);
    }

    return TypeDescriptor::GetProperties(pmValue, amFilter);
}
//---------------------------------------------------------------------------
bool MPoint2Converter::GetPropertiesSupported(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
bool MPoint2Converter::CanConvertTo(ITypeDescriptorContext* pmContext,
    Type* pmDestinationType)
{
    if (pmDestinationType == __typeof(MPoint2))
    {
        return true;
    }

    return TypeConverter::CanConvertTo(pmContext,
        pmDestinationType);
}
//---------------------------------------------------------------------------
Object* MPoint2Converter::ConvertTo(ITypeDescriptorContext* pmContext,
    CultureInfo* pmCulture, Object* pmValue, Type* pmDestinationType)
{
    if (pmDestinationType == __typeof(String) &&
        pmValue->GetType() == __typeof(MPoint2))
    {
        MPoint2* pmPoint2 = static_cast<MPoint2*>(pmValue);

        // Build the string as (x, y).
        return String::Format("({0}, {1})", __box(pmPoint2->X),
            __box(pmPoint2->Y));
    }

    return TypeConverter::ConvertTo(pmContext, pmCulture, pmValue,
        pmDestinationType);
}
//---------------------------------------------------------------------------
bool MPoint2Converter::CanConvertFrom(ITypeDescriptorContext* pmContext,
    Type* pmType)
{
    if (pmType == __typeof(String))
    {
        return true;
    }

    return TypeConverter::CanConvertFrom(pmContext, pmType);
}
//---------------------------------------------------------------------------
Object* MPoint2Converter::ConvertFrom(ITypeDescriptorContext* pmContext,
    CultureInfo* pmInfo, Object* pmValue)
{
    if (pmValue->GetType() == __typeof(String))
    {
        try
        {
            String* strValue = static_cast<String*>(pmValue);

            // Parse the format (x, y).
            int iOpenParen = strValue->IndexOf('(');
            int iComma = strValue->IndexOf(',', iOpenParen + 1);
            int iCloseParen = strValue->IndexOf(')', iComma + 1);
            if (iOpenParen != -1 && iComma != -1 && iCloseParen != -1)
            {
                MPoint2* pmPoint2 = new MPoint2;

                // Get x.
                String* strX = strValue->Substring(iOpenParen + 1,
                    iComma - (iOpenParen + 1));
                pmPoint2->X = Single::Parse(strX);

                // Get y.
                String* strY = strValue->Substring(iComma + 1,
                    iCloseParen - (iComma + 1));
                pmPoint2->Y = Single::Parse(strY);

                return pmPoint2;
            }
        }
        catch(...)
        {
            throw new ArgumentException(String::Format("Cannot convert '{0}' "
                "to type MPoint2", pmValue));
        }
    }

    return TypeConverter::ConvertFrom(pmContext, pmInfo, pmValue);
}
//---------------------------------------------------------------------------
// ElementDescriptor implementation.
//---------------------------------------------------------------------------
MPoint2Converter::ElementDescriptor::ElementDescriptor(MPoint2* pmPoint2,
    unsigned int uiElement, PropertyDescriptor* pmParentDescriptor,
    String* strName, Attribute* amAttributes[]) : PropertyDescriptor(strName,
    amAttributes), m_pmPoint2(pmPoint2), m_uiElement(uiElement),
    m_pmParentDescriptor(pmParentDescriptor)
{
    MAssert(m_pmPoint2 != NULL, "MPoint2Converter::ElementDescriptor "
        "Error: Null MPoint2 provided to constructor.");
    MAssert(m_uiElement < 3, "MPoint2Converter::ElementDescriptor Error: "
        "Element ID out of range.");
}
//---------------------------------------------------------------------------
Type* MPoint2Converter::ElementDescriptor::get_ComponentType()
{
    return m_pmPoint2->GetType();
}
//---------------------------------------------------------------------------
bool MPoint2Converter::ElementDescriptor::get_IsReadOnly()
{
    if (m_pmParentDescriptor != NULL)
    {
        return m_pmParentDescriptor->IsReadOnly;
    }
    else
    {
        return true;
    }
}
//---------------------------------------------------------------------------
Type* MPoint2Converter::ElementDescriptor::get_PropertyType()
{
    return this->Element->GetType();
}
//---------------------------------------------------------------------------
bool MPoint2Converter::ElementDescriptor::CanResetValue(
    Object* pmComponent)
{
    if (m_pmParentDescriptor != NULL)
    {
        return m_pmParentDescriptor->CanResetValue(pmComponent);
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
Object* MPoint2Converter::ElementDescriptor::GetValue(Object*)
{
    return this->Element;
}
//---------------------------------------------------------------------------
void MPoint2Converter::ElementDescriptor::ResetValue(Object* pmComponent)
{
    if (m_pmParentDescriptor != NULL)
    {
        m_pmParentDescriptor->ResetValue(pmComponent);
    }
}
//---------------------------------------------------------------------------
void MPoint2Converter::ElementDescriptor::SetValue(Object* pmComponent,
    Object* pmValue)
{
    this->Element = pmValue;
    if (m_pmParentDescriptor != NULL)
    {
        m_pmParentDescriptor->SetValue(pmComponent, m_pmPoint2);
    }
}
//---------------------------------------------------------------------------
bool MPoint2Converter::ElementDescriptor::ShouldSerializeValue(
    Object* pmComponent)
{
    if (m_pmParentDescriptor != NULL)
    {
        return m_pmParentDescriptor->ShouldSerializeValue(pmComponent);
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
Object* MPoint2Converter::ElementDescriptor::get_Element()
{
    switch (m_uiElement)
    {
        case 0:
            return __box(m_pmPoint2->X);
        case 1:
            return __box(m_pmPoint2->Y);
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MPoint2Converter::ElementDescriptor::set_Element(Object* pmValue)
{
    __box float* pfValue = dynamic_cast<__box float*>(pmValue);
    MAssert(pfValue != NULL, "Element type is not correct; cannot set "
        "value.");

    switch (m_uiElement)
    {
        case 0:
            m_pmPoint2->X = *pfValue;
            break;
        case 1:
            m_pmPoint2->Y = *pfValue;
            break;
    }
}
//---------------------------------------------------------------------------
