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

#pragma once

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI
{
    public __gc class PropertyType
    {
    public:
        PropertyType(String* strName, String* strPrimitiveType,
            Type* pmManagedType);
        PropertyType(String* strName, String* strPrimitiveType,
            Type* pmManagedType, Type* pmEditor, Type* pmTypeConverter);
        PropertyType(String* strName, String* strPrimitiveType,
            Type* pmManagedType, Type* pmEditor, Type* pmTypeConverter,
            Type* pmCollectionEditor);

        __property String* get_Name();
        __property void set_Name(String* strName);

        __property String* get_PrimitiveType();
        __property void set_PrimitiveType(String* strPrimitiveType);

        __property Type* get_ManagedType();
        __property void set_ManagedType(Type* pmManagedType);

        __property Type* get_Editor();
        __property void set_Editor(Type* pmEditor);

        __property Type* get_TypeConverter();
        __property void set_TypeConverter(Type* pmTypeConverter);

        __property Type* get_CollectionEditor();
        __property void set_CollectionEditor(Type* pmCollectionEditor);

    private:
        String* m_strName;
        String* m_strPrimitiveType;
        Type* m_pmManagedType;
        Type* m_pmEditor;
        Type* m_pmTypeConverter;
        Type* m_pmCollectionEditor;
    };
}}}}
