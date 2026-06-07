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

using namespace System::ComponentModel;
using namespace System::Globalization;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MEulerQuaternionConverter : public TypeConverter
    {
    public:
        virtual PropertyDescriptorCollection* GetProperties(
            ITypeDescriptorContext* pmContext, Object* pmValue,
            Attribute* amFilter[]);
        virtual bool GetPropertiesSupported(
            ITypeDescriptorContext* pmContext);

        virtual bool CanConvertTo(ITypeDescriptorContext* pmContext,
            Type* pmDestinationType);
        virtual Object* ConvertTo(ITypeDescriptorContext* pmContext,
            CultureInfo* pmCulture, Object* pmValue, Type* pmDestinationType);
        virtual bool CanConvertFrom(ITypeDescriptorContext* pmContext,
            Type* pmType);
        virtual Object* ConvertFrom(ITypeDescriptorContext* pmContext,
            CultureInfo* pmInfo, Object* pmValue);

    private:
        __gc class ElementDescriptor : public PropertyDescriptor
        {
        public:
            ElementDescriptor(MQuaternion* pmQuaternion,
                unsigned int uiElement,
                PropertyDescriptor* pmParentDescriptor, String* strName,
                Attribute* amAttributes[]);

            // PropertyDescriptor overrides.
            __property Type* get_ComponentType();
            __property bool get_IsReadOnly();
            __property Type* get_PropertyType();
            bool CanResetValue(Object* pmComponent);
            Object* GetValue(Object* pmComponent);
            void ResetValue(Object* pmComponent);
            void SetValue(Object* pmComponent, Object* pmValue);
            bool ShouldSerializeValue(Object* pmComponent);

        private:
            __property Object* get_Element();
            __property void set_Element(Object* pmValue);

            MQuaternion* m_pmQuaternion;
            unsigned int m_uiElement;
            PropertyDescriptor* m_pmParentDescriptor;
        };
    };
}}}}
