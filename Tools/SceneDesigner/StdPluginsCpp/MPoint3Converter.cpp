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

#include "MPoint3Converter.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace System::Collections;

//---------------------------------------------------------------------------
PropertyDescriptorCollection* MPoint3Converter::GetProperties(
    ITypeDescriptorContext* pmContext, Object* pmValue, Attribute* amFilter[])
{
    if (pmValue->GetType() == __typeof(MPoint3) &&
        pmContext->PropertyDescriptor != NULL)
    {
        MPoint3* pmPoint3 = static_cast<MPoint3*>(pmValue);

        MEntityPropertyDescriptor* pmEntityDescriptor =
            dynamic_cast<MEntityPropertyDescriptor*>(
            pmContext->PropertyDescriptor);

        ArrayList* pmPropertyDescs = new ArrayList();
        pmPropertyDescs->Add(new ElementDescriptor(pmPoint3, 0,
            pmEntityDescriptor, "x", amFilter));
        pmPropertyDescs->Add(new ElementDescriptor(pmPoint3, 1,
            pmEntityDescriptor, "y", amFilter));
        pmPropertyDescs->Add(new ElementDescriptor(pmPoint3, 2,
            pmEntityDescriptor, "z", amFilter));

        PropertyDescriptor* amPropertyDescArray[] =
            dynamic_cast<PropertyDescriptor*[]>(pmPropertyDescs->ToArray(
            __typeof(PropertyDescriptor)));
        return new PropertyDescriptorCollection(amPropertyDescArray);
    }

    return TypeDescriptor::GetProperties(pmValue, amFilter);
}
//---------------------------------------------------------------------------
bool MPoint3Converter::GetPropertiesSupported(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
bool MPoint3Converter::CanConvertTo(ITypeDescriptorContext* pmContext,
    Type* pmDestinationType)
{
    if (pmDestinationType == __typeof(MPoint3))
    {
        return true;
    }

    return TypeConverter::CanConvertTo(pmContext,
        pmDestinationType);
}
//---------------------------------------------------------------------------
Object* MPoint3Converter::ConvertTo(ITypeDescriptorContext* pmContext,
    CultureInfo* pmCulture, Object* pmValue, Type* pmDestinationType)
{
    if (pmDestinationType == __typeof(String) &&
        pmValue->GetType() == __typeof(MPoint3))
    {
        MPoint3* pmPoint3 = static_cast<MPoint3*>(pmValue);

        // Build the string as (x, y, z).
        return String::Format("({0}, {1}, {2})", __box(pmPoint3->X),
            __box(pmPoint3->Y), __box(pmPoint3->Z));
    }

    return TypeConverter::ConvertTo(pmContext, pmCulture, pmValue,
        pmDestinationType);
}
//---------------------------------------------------------------------------
bool MPoint3Converter::CanConvertFrom(ITypeDescriptorContext* pmContext,
    Type* pmType)
{
    if (pmType == __typeof(String))
    {
        return true;
    }

    return TypeConverter::CanConvertFrom(pmContext, pmType);
}
//---------------------------------------------------------------------------
Object* MPoint3Converter::ConvertFrom(ITypeDescriptorContext* pmContext,
    CultureInfo* pmInfo, Object* pmValue)
{
    if (pmValue->GetType() == __typeof(String))
    {
        try
        {
            String* strValue = static_cast<String*>(pmValue);

            // Parse the format (x, y, z).
            int iOpenParen = strValue->IndexOf('(');
            int iComma1 = strValue->IndexOf(',', iOpenParen + 1);
            int iComma2 = strValue->IndexOf(',', iComma1 + 1);
            int iCloseParen = strValue->IndexOf(')', iComma2 + 1);
            if (iOpenParen != -1 && iComma1 != -1 && iComma2 != -1 &&
                iCloseParen != -1)
            {
                MPoint3* pmPoint3 = new MPoint3;

                // Get x.
                String* strX = strValue->Substring(iOpenParen + 1,
                    iComma1 - (iOpenParen + 1));
                pmPoint3->X = Single::Parse(strX);

                // Get y.
                String* strY = strValue->Substring(iComma1 + 1,
                    iComma2 - (iComma1 + 1));
                pmPoint3->Y = Single::Parse(strY);

                // Get z.
                String* strZ = strValue->Substring(iComma2 + 1,
                    iCloseParen - (iComma2 + 1));
                pmPoint3->Z = Single::Parse(strZ);

                return pmPoint3;
            }
        }
        catch(...)
        {
            throw new ArgumentException(String::Format("Cannot convert '{0}' "
                "to type MPoint3", pmValue));
        }
    }

    return TypeConverter::ConvertFrom(pmContext, pmInfo, pmValue);
}
//---------------------------------------------------------------------------
// ElementDescriptor implementation.
//---------------------------------------------------------------------------
MPoint3Converter::ElementDescriptor::ElementDescriptor(MPoint3* pmPoint3,
    unsigned int uiElement, PropertyDescriptor* pmParentDescriptor,
    String* strName, Attribute* amAttributes[]) : PropertyDescriptor(strName,
    amAttributes), m_pmPoint3(pmPoint3), m_uiElement(uiElement),
    m_pmParentDescriptor(pmParentDescriptor)
{
    MAssert(m_pmPoint3 != NULL, "MPoint3Converter::ElementDescriptor "
        "Error: Null MPoint3 provided to constructor.");
    MAssert(m_uiElement < 3, "MPoint3Converter::ElementDescriptor Error: "
        "Element ID out of range.");
}
//---------------------------------------------------------------------------
Type* MPoint3Converter::ElementDescriptor::get_ComponentType()
{
    return m_pmPoint3->GetType();
}
//---------------------------------------------------------------------------
bool MPoint3Converter::ElementDescriptor::get_IsReadOnly()
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
Type* MPoint3Converter::ElementDescriptor::get_PropertyType()
{
    return this->Element->GetType();
}
//---------------------------------------------------------------------------
bool MPoint3Converter::ElementDescriptor::CanResetValue(
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
Object* MPoint3Converter::ElementDescriptor::GetValue(Object*)
{
    return this->Element;
}
//---------------------------------------------------------------------------
void MPoint3Converter::ElementDescriptor::ResetValue(Object* pmComponent)
{
    if (m_pmParentDescriptor != NULL)
    {
        m_pmParentDescriptor->ResetValue(pmComponent);
    }
}
//---------------------------------------------------------------------------
void MPoint3Converter::ElementDescriptor::SetValue(Object* pmComponent,
    Object* pmValue)
{
    this->Element = pmValue;
    if (m_pmParentDescriptor != NULL)
    {
        m_pmParentDescriptor->SetValue(pmComponent, m_pmPoint3);
    }
}
//---------------------------------------------------------------------------
bool MPoint3Converter::ElementDescriptor::ShouldSerializeValue(
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
Object* MPoint3Converter::ElementDescriptor::get_Element()
{
    switch (m_uiElement)
    {
        case 0:
            return __box(m_pmPoint3->X);
        case 1:
            return __box(m_pmPoint3->Y);
        case 2:
            return __box(m_pmPoint3->Z);
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MPoint3Converter::ElementDescriptor::set_Element(Object* pmValue)
{
    __box float* pfValue = dynamic_cast<__box float*>(pmValue);
    MAssert(pfValue != NULL, "Element type is not correct; cannot set "
        "value.");

    switch (m_uiElement)
    {
        case 0:
            m_pmPoint3->X = *pfValue;
            break;
        case 1:
            m_pmPoint3->Y = *pfValue;
            break;
        case 2:
            m_pmPoint3->Z = *pfValue;
            break;
    }
}
//---------------------------------------------------------------------------
