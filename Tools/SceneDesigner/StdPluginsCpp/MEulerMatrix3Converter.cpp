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

#include "MEulerMatrix3Converter.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace System::Collections;

//---------------------------------------------------------------------------
PropertyDescriptorCollection* MEulerMatrix3Converter::GetProperties(
    ITypeDescriptorContext* pmContext, Object* pmValue, Attribute* amFilter[])
{
    if (pmValue->GetType() == __typeof(MMatrix3) &&
        pmContext->PropertyDescriptor != NULL)
    {
        MMatrix3* pmMatrix3 = static_cast<MMatrix3*>(pmValue);

        MEntityPropertyDescriptor* pmEntityDescriptor =
            dynamic_cast<MEntityPropertyDescriptor*>(
            pmContext->PropertyDescriptor);

        ArrayList* pmPropertyDescs = new ArrayList();
        pmPropertyDescs->Add(new ElementDescriptor(pmMatrix3, 0,
            pmEntityDescriptor, "X Angle", amFilter));
        pmPropertyDescs->Add(new ElementDescriptor(pmMatrix3, 1,
            pmEntityDescriptor, "Y Angle", amFilter));
        pmPropertyDescs->Add(new ElementDescriptor(pmMatrix3, 2,
            pmEntityDescriptor, "Z Angle", amFilter));

        PropertyDescriptor* amPropertyDescArray[] =
            dynamic_cast<PropertyDescriptor*[]>(pmPropertyDescs->ToArray(
            __typeof(PropertyDescriptor)));
        return new PropertyDescriptorCollection(amPropertyDescArray);
    }

    return TypeDescriptor::GetProperties(pmValue, amFilter);
}
//---------------------------------------------------------------------------
bool MEulerMatrix3Converter::GetPropertiesSupported(
    ITypeDescriptorContext*)
{
    return true;
}
//---------------------------------------------------------------------------
bool MEulerMatrix3Converter::CanConvertTo(
    ITypeDescriptorContext* pmContext, Type* pmDestinationType)
{
    if (pmDestinationType == __typeof(MMatrix3))
    {
        return true;
    }

    return TypeConverter::CanConvertTo(pmContext,
        pmDestinationType);
}
//---------------------------------------------------------------------------
Object* MEulerMatrix3Converter::ConvertTo(
    ITypeDescriptorContext* pmContext, CultureInfo* pmCulture,
    Object* pmValue, Type* pmDestinationType)
{
    if (pmDestinationType == __typeof(String) &&
        pmValue->GetType() == __typeof(MMatrix3))
    {
        MMatrix3* pmMatrix3 = static_cast<MMatrix3*>(pmValue);

        // Build the string as (x, y, z).
        return String::Format("({0}, {1}, {2})",
            __box(MUtility::RadiansToDegrees(pmMatrix3->XAngle)),
            __box(MUtility::RadiansToDegrees(pmMatrix3->YAngle)),
            __box(MUtility::RadiansToDegrees(pmMatrix3->ZAngle)));
    }

    return TypeConverter::ConvertTo(pmContext, pmCulture, pmValue,
        pmDestinationType);
}
//---------------------------------------------------------------------------
bool MEulerMatrix3Converter::CanConvertFrom(
    ITypeDescriptorContext* pmContext, Type* pmType)
{
    if (pmType == __typeof(String))
    {
        return true;
    }

    return TypeConverter::CanConvertFrom(pmContext, pmType);
}
//---------------------------------------------------------------------------
Object* MEulerMatrix3Converter::ConvertFrom(
    ITypeDescriptorContext* pmContext, CultureInfo* pmInfo, Object* pmValue)
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
                // Get x.
                String* strXAngle = strValue->Substring(iOpenParen + 1,
                    iComma1 - (iOpenParen + 1));
                float fXAngle = MUtility::DegreesToRadians(Single::Parse(
                    strXAngle));

                // Get y.
                String* strYAngle = strValue->Substring(iComma1 + 1,
                    iComma2 - (iComma1 + 1));
                float fYAngle = MUtility::DegreesToRadians(Single::Parse(
                    strYAngle));

                // Get z.
                String* strZAngle = strValue->Substring(iComma2 + 1,
                    iCloseParen - (iComma2 + 1));
                float fZAngle = MUtility::DegreesToRadians(Single::Parse(
                    strZAngle));

                MMatrix3* pmMatrix3 = new MMatrix3();
                pmMatrix3->FromEulerAnglesXYZ(fXAngle, fYAngle, fZAngle);
                return pmMatrix3;
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
MEulerMatrix3Converter::ElementDescriptor::ElementDescriptor(
    MMatrix3* pmMatrix3, unsigned int uiElement,
    PropertyDescriptor* pmParentDescriptor, String* strName,
    Attribute* amAttributes[]) : PropertyDescriptor(strName, amAttributes),
    m_pmMatrix3(pmMatrix3), m_uiElement(uiElement),
    m_pmParentDescriptor(pmParentDescriptor)
{
    MAssert(m_pmMatrix3 != NULL,
        "MEulterRotationTypeConverter::ElementDescriptor Error: Null "
        "MMatrix3 provided to constructor.");
    MAssert(m_uiElement < 3,
        "MEulterRotationTypeConverter::ElementDescriptor Error: Element ID "
        "out of range.");
}
//---------------------------------------------------------------------------
Type* MEulerMatrix3Converter::ElementDescriptor::get_ComponentType()
{
    return m_pmMatrix3->GetType();
}
//---------------------------------------------------------------------------
bool MEulerMatrix3Converter::ElementDescriptor::get_IsReadOnly()
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
Type* MEulerMatrix3Converter::ElementDescriptor::get_PropertyType()
{
    return this->Element->GetType();
}
//---------------------------------------------------------------------------
bool MEulerMatrix3Converter::ElementDescriptor::CanResetValue(
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
Object* MEulerMatrix3Converter::ElementDescriptor::GetValue(
    Object*)
{
    return this->Element;
}
//---------------------------------------------------------------------------
void MEulerMatrix3Converter::ElementDescriptor::ResetValue(
    Object* pmComponent)
{
    if (m_pmParentDescriptor != NULL)
    {
        m_pmParentDescriptor->ResetValue(pmComponent);
    }
}
//---------------------------------------------------------------------------
void MEulerMatrix3Converter::ElementDescriptor::SetValue(
    Object* pmComponent, Object* pmValue)
{
    this->Element = pmValue;
    if (m_pmParentDescriptor != NULL)
    {
        m_pmParentDescriptor->SetValue(pmComponent, m_pmMatrix3);
    }
}
//---------------------------------------------------------------------------
bool MEulerMatrix3Converter::ElementDescriptor::ShouldSerializeValue(
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
Object* MEulerMatrix3Converter::ElementDescriptor::get_Element()
{
    switch (m_uiElement)
    {
        case 0:
            return __box(MUtility::RadiansToDegrees(m_pmMatrix3->XAngle));
        case 1:
            return __box(MUtility::RadiansToDegrees(m_pmMatrix3->YAngle));
        case 2:
            return __box(MUtility::RadiansToDegrees(m_pmMatrix3->ZAngle));
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MEulerMatrix3Converter::ElementDescriptor::set_Element(
    Object* pmValue)
{
    __box float* pfValue = dynamic_cast<__box float*>(pmValue);
    MAssert(pfValue != NULL, "Element type is not correct; cannot set "
        "value.");

    switch (m_uiElement)
    {
        case 0:
            m_pmMatrix3->FromEulerAnglesXYZ(
                MUtility::DegreesToRadians(*pfValue), m_pmMatrix3->YAngle,
                m_pmMatrix3->ZAngle);
            break;
        case 1:
            m_pmMatrix3->FromEulerAnglesXYZ(m_pmMatrix3->XAngle,
                MUtility::DegreesToRadians(*pfValue), m_pmMatrix3->ZAngle);
            break;
        case 2:
            m_pmMatrix3->FromEulerAnglesXYZ(m_pmMatrix3->XAngle,
                m_pmMatrix3->YAngle, MUtility::DegreesToRadians(*pfValue));
            break;
    }
}
//---------------------------------------------------------------------------
