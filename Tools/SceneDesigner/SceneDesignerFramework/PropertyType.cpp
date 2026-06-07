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

#include "PropertyType.h"

using namespace System::ComponentModel::Design;

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

//---------------------------------------------------------------------------
PropertyType::PropertyType(String* strName, String* strPrimitiveType,
    Type* pmManagedType) : m_strName(strName),
    m_strPrimitiveType(strPrimitiveType), m_pmManagedType(pmManagedType)
{
    MAssert(m_strName != NULL, "PropertyType Error: Null name string "
        "provided to constructor.");
    MAssert(m_strPrimitiveType != NULL, "PropertyType Error: Null primitive "
        "type provided to constructor.");
    MAssert(m_pmManagedType != NULL, "PropertyType Error: Null Type "
        "provided to constructor.");
}
//---------------------------------------------------------------------------
PropertyType::PropertyType(String* strName, String* strPrimitiveType,
    Type* pmManagedType, Type* pmEditor, Type* pmTypeConverter) :
    m_strName(strName), m_strPrimitiveType(strPrimitiveType),
    m_pmManagedType(pmManagedType), m_pmEditor(pmEditor),
    m_pmTypeConverter(pmTypeConverter)
{
    MAssert(m_strName != NULL, "PropertyType Error: Null name string "
        "provided to constructor.");
    MAssert(m_strPrimitiveType != NULL, "PropertyType Error: Null primitive "
        "type provided to constructor.");
    MAssert(m_pmManagedType != NULL, "PropertyType Error: Null Type "
        "provided to constructor.");
}
//---------------------------------------------------------------------------
PropertyType::PropertyType(String* strName, String* strPrimitiveType,
    Type* pmManagedType, Type* pmEditor, Type* pmTypeConverter,
    Type* pmCollectionEditor) : m_strName(strName),
    m_strPrimitiveType(strPrimitiveType), m_pmManagedType(pmManagedType),
    m_pmEditor(pmEditor), m_pmTypeConverter(pmTypeConverter),
    m_pmCollectionEditor(pmCollectionEditor)
{
    MAssert(m_strName != NULL, "PropertyType Error: Null name string "
        "provided to constructor.");
    MAssert(m_strPrimitiveType != NULL, "PropertyType Error: Null primitive "
        "type provided to constructor.");
    MAssert(m_pmManagedType != NULL, "PropertyType Error: Null Type "
        "provided to constructor.");
}
//---------------------------------------------------------------------------
String* PropertyType::get_Name()
{
    return m_strName;
}
//---------------------------------------------------------------------------
void PropertyType::set_Name(String* strName)
{
    m_strName = strName;
}
//---------------------------------------------------------------------------
String* PropertyType::get_PrimitiveType()
{
    return m_strPrimitiveType;
}
//---------------------------------------------------------------------------
void PropertyType::set_PrimitiveType(String* strPrimitiveType)
{
    m_strPrimitiveType = strPrimitiveType;
}
//---------------------------------------------------------------------------
Type* PropertyType::get_ManagedType()
{
    return m_pmManagedType;
}
//---------------------------------------------------------------------------
void PropertyType::set_ManagedType(Type* pmManagedType)
{
    m_pmManagedType = pmManagedType;
}
//---------------------------------------------------------------------------
Type* PropertyType::get_Editor()
{
    return m_pmEditor;
}
//---------------------------------------------------------------------------
void PropertyType::set_Editor(Type* pmEditor)
{
    m_pmEditor = pmEditor;
}
//---------------------------------------------------------------------------
Type* PropertyType::get_TypeConverter()
{
    return m_pmTypeConverter;
}
//---------------------------------------------------------------------------
void PropertyType::set_TypeConverter(Type* pmTypeConverter)
{
    m_pmTypeConverter = pmTypeConverter;
}
//---------------------------------------------------------------------------
Type* PropertyType::get_CollectionEditor()
{
    return m_pmCollectionEditor;
    //if (m_pmCollectionEditor != NULL)
    //{
    //    return m_pmCollectionEditor;
    //}
    //else
    //{
    //    return __typeof(
    //        System::ComponentModel::Design::CollectionEditor);//(
    //        //m_pmManagedType); 
    //}
}
//---------------------------------------------------------------------------
void PropertyType::set_CollectionEditor(Type* pmCollectionEditor)
{
    m_pmCollectionEditor = pmCollectionEditor;
}
//---------------------------------------------------------------------------
