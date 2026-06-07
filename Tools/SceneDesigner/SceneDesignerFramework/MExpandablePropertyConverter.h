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

#include "IPropertyTypeService.h"
#include "IMessageService.h"

using namespace System::ComponentModel;
using namespace System::Globalization;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MExpandablePropertyConverter : public TypeConverter
    {
    public:
        virtual bool GetPropertiesSupported(
            ITypeDescriptorContext* pmContext);
        virtual PropertyDescriptorCollection* GetProperties(
            ITypeDescriptorContext* pmContext, Object* pmValue,
            Attribute* amFilter[]);

    private:
        __property static IPropertyTypeService* get_PropertyTypeService();
        static IPropertyTypeService* ms_pmPropertyTypeService;

        __property static IMessageService* get_MessageService();
        static IMessageService* ms_pmMessageService;
    };
}}}}
