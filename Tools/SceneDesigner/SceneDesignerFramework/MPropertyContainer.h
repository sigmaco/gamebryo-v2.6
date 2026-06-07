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

#include "MDisposable.h"
#include "IPropertyTypeService.h"
#include "ICommandService.h"
#include "IMessageService.h"

using namespace System::ComponentModel;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI
    ::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MLayer;

    public __abstract __gc class MPropertyContainer : public MDisposable,
        public ICustomTypeDescriptor
    {
    public:
        MPropertyContainer();

        __property String* get_Name();
        __property bool get_Dirty();
        __property void set_Dirty(bool bDirty);
        __property virtual NiEntityPropertyInterface* 
            get_PropertyInterface() = 0;

        bool HasProperty(String* strPropertyName);
        bool IsCollection(String* strPropertyName);
        unsigned int GetElementCount(String* strPropertyName);
        void SetElementCount(String* strPropertyName, unsigned int uiCount);
        bool IsPropertyReadOnly(String* strPropertyName);
        PropertyType* GetPropertyType(String* strPropertyName);
        bool IsPropertyUnique(String* strPropertyName);
        bool IsPropertyInheritable(String* strPropertyName);
        String* GetPropertyDisplayName(String* strPropertyName);
        bool IsExternalAssetPath(String* strPropertyName,
            unsigned int uiIndex);

        Object* GetPropertyData(String* strPropertyName);
        Object* GetPropertyData(String* strPropertyName,
            unsigned int uiIndex);
        void SetPropertyData(String* strPropertyName, Object* pmData,
            bool bUndoable);
        void SetPropertyData(String* strPropertyName, Object* pmData,
            unsigned int uiIndex, bool bUndoable);

        bool CanResetProperty(String* strPropertyName);
        void ResetProperty(String* strPropertyName);
        void MakePropertyUnique(String* strPropertyName);

        __property virtual bool get_Writable();
        __property virtual void set_Writable(bool bWritable);

        __property Guid get_ID();

        __property String* get_SourceFilename();

        __property MLayer* get_Layer();
    protected public:
        __property void set_Layer(MLayer* pmLayer);

    protected:
        __property static IPropertyTypeService* get_PropertyTypeService();
        static IPropertyTypeService* ms_pmPropertyTypeService;

        __property static ICommandService* get_CommandService();
        static ICommandService* ms_pmCommandService;

        __property static IMessageService* get_MessageService();
        static IMessageService* ms_pmMessageService;

        MLayer* m_pmLayer;
        bool m_bDirtyBit;
        bool m_bWritable;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // ICustomTypeDescriptor overrides.
    public:
        AttributeCollection* GetAttributes();
        String* GetClassName();
        String* GetComponentName();
        TypeConverter* GetConverter();
        EventDescriptor* GetDefaultEvent();
        PropertyDescriptor* GetDefaultProperty();
        Object* GetEditor(Type* pmEditorBaseType);
        EventDescriptorCollection* GetEvents();
        EventDescriptorCollection* GetEvents(Attribute* amAttributes[]);
        PropertyDescriptorCollection* GetProperties();
        PropertyDescriptorCollection* GetProperties(
            Attribute* amAttributes[]);
        Object* GetPropertyOwner(PropertyDescriptor* pmPropertyDescriptor);
    };

}}}}

