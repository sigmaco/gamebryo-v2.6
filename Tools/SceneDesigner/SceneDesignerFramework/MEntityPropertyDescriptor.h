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

//#include "MEntity.h"
#include "MPropertyContainer.h"

using namespace System::ComponentModel;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MEntityPropertyDescriptor : public PropertyDescriptor
    {
    public:
        static const unsigned int INVALID_PROPERTY_INDEX = (unsigned int) -1;

        MEntityPropertyDescriptor(MPropertyContainer* pmPropertyContainer,
            String* strPropertyName, unsigned int uiPropertyIndex,
            Type* pmEditorType, Type* pmConverterType, String* strName,
            Attribute* amAttributes[]);

        __property MPropertyContainer* get_PropertyContainer();
        __property String* get_PropertyName();
        __property unsigned int get_PropertyIndex();
        __property Type* get_EditorType();
        __property Type* get_ConverterType();

        // PropertyDescriptor overrides.
        __property Type* get_ComponentType();
        __property bool get_IsReadOnly();
        __property Type* get_PropertyType();
        bool CanResetValue(Object* pmComponent);
        Object* GetValue(Object* pmComponent);
        void ResetValue(Object* pmComponent);
        void SetValue(Object* pmComponent, Object* pmValue);
        bool ShouldSerializeValue(Object* pmComponent);
        Object* GetEditor(Type* pmEditorBaseType);
        __property TypeConverter* get_Converter();

    private:
        MPropertyContainer* m_pmContainer;
        String* m_strPropertyName;
        unsigned int m_uiPropertyIndex;
        Type* m_pmEditorType;
        Type* m_pmConverterType;

        unsigned int m_uiNonNullElementCount;

        TypeConverter* m_pmConverter;
        int m_iEditorCount;
        Object* m_amEditors[];
        Type* m_amEditorTypes[];
    };
}}}}
