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

#include "MEntityPropertyDescriptor.h"
#include "MCollectionPropertyEditor.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace System::Collections;
using namespace System::Drawing::Design;
using namespace System::Diagnostics;

//---------------------------------------------------------------------------
MEntityPropertyDescriptor::MEntityPropertyDescriptor(
    MPropertyContainer* pmPropertyContainer, String* strPropertyName, 
    unsigned int uiPropertyIndex, Type* pmEditorType, Type* pmConverterType,
    String* strName, Attribute* amAttributes[]) 
    : PropertyDescriptor(strName, amAttributes), 
    m_pmContainer(pmPropertyContainer), m_strPropertyName(strPropertyName),
    m_uiPropertyIndex(uiPropertyIndex), m_pmEditorType(pmEditorType),
    m_pmConverterType(pmConverterType), m_uiNonNullElementCount(0)
{
    MAssert(m_pmContainer != NULL, "MEntity::EntityPropertyDescriptor Error: "
        "Null entity provided to constructor!");
    MAssert(!String::IsNullOrEmpty(m_strPropertyName),
        "MEntity::EntityPropertyDescriptor Error: Null or empty property "
        "name provided to constructor!");
    MAssert(m_pmContainer->HasProperty(m_strPropertyName),
        "MEntity::EntityPropertyDescriptor Error: Specified property does "
        "not exist on entity!");
    MAssert(m_pmContainer->GetPropertyType(m_strPropertyName) != NULL,
        "MEntity::EntityPropertyDescriptor Error: Property type not found "
        "for specified property!");

    unsigned int uiElementCount = m_pmContainer->GetElementCount(
        m_strPropertyName);
    MAssert(m_uiPropertyIndex == INVALID_PROPERTY_INDEX ||
        m_uiPropertyIndex < uiElementCount,
        "MEntity::EntityPropertyDescriptor Error: Property index must be "
        "smaller than element count!");

    if (m_uiPropertyIndex == INVALID_PROPERTY_INDEX)
    {
        m_uiNonNullElementCount = 0;
        for (unsigned int ui = 0; ui < uiElementCount; ui++)
        {
            if (m_pmContainer->GetPropertyData(m_strPropertyName, ui) != NULL)
            {
                m_uiNonNullElementCount++;
            }
        }
    }
}
//---------------------------------------------------------------------------
MPropertyContainer* MEntityPropertyDescriptor::get_PropertyContainer()
{
    return m_pmContainer;
}
//---------------------------------------------------------------------------
String* MEntityPropertyDescriptor::get_PropertyName()
{
    return m_strPropertyName;
}
//---------------------------------------------------------------------------
unsigned int MEntityPropertyDescriptor::get_PropertyIndex()
{
    return m_uiPropertyIndex;
}
//---------------------------------------------------------------------------
Type* MEntityPropertyDescriptor::get_EditorType()
{
    return m_pmEditorType;
}
//---------------------------------------------------------------------------
Type* MEntityPropertyDescriptor::get_ConverterType()
{
    return m_pmConverterType;
}
//---------------------------------------------------------------------------
Type* MEntityPropertyDescriptor::get_ComponentType()
{
    return m_pmContainer->GetType();
}
//---------------------------------------------------------------------------
bool MEntityPropertyDescriptor::get_IsReadOnly()
{
    return (m_pmContainer->IsPropertyReadOnly(m_strPropertyName) ||
        !m_pmContainer->Writable);
}
//---------------------------------------------------------------------------
Type* MEntityPropertyDescriptor::get_PropertyType()
{
    return m_pmContainer->GetPropertyType(m_strPropertyName)->ManagedType;
}
//---------------------------------------------------------------------------
bool MEntityPropertyDescriptor::CanResetValue(Object*)
{
    return m_pmContainer->IsPropertyUnique(m_strPropertyName);
}
//---------------------------------------------------------------------------
Object* MEntityPropertyDescriptor::GetValue(Object*)
{
    if (m_uiPropertyIndex == INVALID_PROPERTY_INDEX)
    {
        return String::Format("[Expand to see {0} sub-properties]",
            __box(m_uiNonNullElementCount));
    }
    else
    {
        return m_pmContainer->GetPropertyData(m_strPropertyName,
            m_uiPropertyIndex);
    }
}
//---------------------------------------------------------------------------
void MEntityPropertyDescriptor::ResetValue(Object*)
{
    m_pmContainer->ResetProperty(m_strPropertyName);
}
//---------------------------------------------------------------------------
void MEntityPropertyDescriptor::SetValue(Object*,
    Object* pmValue)
{
    if (m_uiPropertyIndex != INVALID_PROPERTY_INDEX)
    {
        m_pmContainer->SetPropertyData(m_strPropertyName, pmValue,
            m_uiPropertyIndex, true);
    }
}
//---------------------------------------------------------------------------
bool MEntityPropertyDescriptor::ShouldSerializeValue(
    Object* pmComponent)
{
    return CanResetValue(pmComponent);
}
//---------------------------------------------------------------------------
Object* MEntityPropertyDescriptor::GetEditor(Type* pmEditorBaseType)
{
    if (m_amEditorTypes != NULL)
    {
        for (int i = 0; i < m_iEditorCount; i++)
        {
            if (m_amEditorTypes[i] == pmEditorBaseType)
            {
                return m_amEditors[i];
            }
        }
    }

    Object* pmEditor = NULL;
    if (m_pmEditorType != NULL && pmEditorBaseType == __typeof(UITypeEditor))
    {
        pmEditor = CreateInstance(m_pmEditorType);
    }
    if (pmEditor == NULL)
    {
        if (m_uiPropertyIndex == INVALID_PROPERTY_INDEX)
        {
            Type* objectType = __typeof(System::Collections::ArrayList);
            pmEditor = new MCollectionPropertyEditor(objectType,
                this->PropertyType);
        }
        else
        {
            pmEditor = TypeDescriptor::GetEditor(this->PropertyType,
                pmEditorBaseType);
        }
    }
    if (m_amEditorTypes == NULL)
    {
        m_amEditorTypes = new Type*[5];
        m_amEditors = new Object*[5];
    }
    if (m_iEditorCount >= m_amEditorTypes->Length)
    {
        Type* amEditorTypes[] = new Type*[m_amEditorTypes->Length * 2];
        Array::Copy(m_amEditorTypes, amEditorTypes, m_amEditorTypes->Length);
        m_amEditorTypes = amEditorTypes;

        Object* amEditors[] = new Object*[m_amEditors->Length * 2];
        Array::Copy(m_amEditors, amEditors, m_amEditors->Length);
        m_amEditors = amEditors;
    }
    m_amEditorTypes[m_iEditorCount] = pmEditorBaseType;
    m_amEditors[m_iEditorCount++] = pmEditor;

    return pmEditor;
}
//---------------------------------------------------------------------------
TypeConverter* MEntityPropertyDescriptor::get_Converter()
{
    if (m_pmConverter == NULL)
    {
        if (m_pmConverterType != NULL &&
            __typeof(TypeConverter)->IsAssignableFrom(m_pmConverterType))
        {
            m_pmConverter = dynamic_cast<TypeConverter*>(CreateInstance(
                m_pmConverterType));
        }
        if (m_pmConverter == NULL)
        {
            m_pmConverter = TypeDescriptor::GetConverter(this->PropertyType);
        }
    }

    return m_pmConverter;
}
//---------------------------------------------------------------------------
