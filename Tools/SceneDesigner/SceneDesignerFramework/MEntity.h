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
#include "MPropertyContainer.h"
#include "PropertyType.h"
#include "MComponent.h"
#include "MRenderingContext.h"
#include "IMessageService.h"
#include "IPropertyTypeService.h"
#include "ICommandService.h"

using namespace System::ComponentModel;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MEntity : public MPropertyContainer
    {
    public:
        MEntity(NiEntityInterface* pkEntity);

        virtual String* ToString();

        __property String* get_Name();
        __property void set_Name(String* strName);
        __property Guid get_TemplateID();
        __property void set_TemplateID(Guid mGuid);
        
        __property virtual NiEntityPropertyInterface* get_PropertyInterface();
        static MEntity* CreateGeneralEntity(String* strName);
        // Clone can return more than one entity (in the case of prefabs)
        MEntity* Clone(String* strNewName, bool bInheritProperties)[];
        __property MEntity* get_MasterEntity();
        __property void set_MasterEntity(MEntity* pmEntity);
        __property MEntity* get_RootMasterEntity();
        void MakeEntityUnique();
        __property MEntity* get_PrefabRoot();
        __property void set_PrefabRoot(MEntity* pmPrefabRoot);
        __property bool get_IsPrefabRoot();
        MEntity* GetPrefabEntities()[];

        __property bool get_SupportsComponents();
        __property unsigned int get_ComponentCount();
        MComponent* GetComponents()[];
        MComponent* GetComponentByTemplateID(Guid mTemplateID);
        bool CanAddComponent(MComponent* pmComponent);
        bool CanRemoveComponent(MComponent*pmComponent);
        void AddComponent(MComponent* pmComponent, bool bPerformErrorChecking,
            bool bUndoable);
        void RemoveComponent(MComponent* pmComponent,
            bool bPerformErrorChecking, bool bUndoable);

        String* GetPropertyNames()[];

        NiAVObject* GetSceneRootPointer(unsigned int uiIndex);
        unsigned int GetSceneRootPointerCount();

        __property bool get_Hidden();
        __property void set_Hidden(bool bHidden);
        void SetHidden(bool bHidden, bool bUndoable);
        __property bool get_LocalHidden();

        __property bool get_Frozen();
        __property void set_Frozen(bool bFrozen);
        void SetFrozen(bool bFrozen, bool bUndoable);
        __property bool get_LocalFrozen();

        __property String* get_Tags();
        __property void set_Tags(String* strTags);
        __property String* get_TagsArray()[];
        void AddTag(String* strTag);
        void RemoveTag(String* strTag);
        bool ContainsTag(String* strTag);
        __property static char get_TagDelimiter();
        __property static String* get_TagDelimiterString();

        __property virtual bool get_Writable();
        __property virtual void set_Writable(bool bWritable);

        NiEntityInterface* GetNiEntityInterface();

        void Update(float fTime, NiExternalAssetManager* pkAssetManager);

        // ICustomTypeDescriptor overrides.
        PropertyDescriptorCollection* GetProperties();
        PropertyDescriptorCollection* GetProperties(
            Attribute* amAttributes[]);

        __gc class EntityDescriptor : public PropertyDescriptor
        {
        public:
            __value enum ValueType
            {
                Name,
                MasterEntity,
                Layer,
                ID,
                Hidden,
                Frozen,
                Tags
            };

            EntityDescriptor(MEntity* pmEntity, ValueType eValueType,
                String* strName, Attribute* amAttributes[]);

            __property MEntity* get_Entity();

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
            TypeConverter* get_Converter();

        private:
            MEntity* m_pmEntity;
            ValueType m_eValueType;
            Type* m_pmTagsEditorType;
            Type* m_pmTagsTypeConverter;
        };

    private:
        NiEntityInterface* m_pkEntity;
        static NiTObjectSet<NiFixedString>* m_pkPropertyNames;

        __property static IMessageService* get_MessageService();
        static IMessageService* ms_pmMessageService;

    private public:
        static void _SDMInit();
        static void _SDMShutdown();
        bool m_bFrozen;

    // MDisposable members.
    protected:

        virtual void Do_Dispose(bool bDisposing);
    };
}}}}
