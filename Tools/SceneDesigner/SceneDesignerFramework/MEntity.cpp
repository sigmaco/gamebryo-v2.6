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

#include "MEntity.h"
#include "MEntityCloneHelper.h"
#include "MEventManager.h"
#include "MEntityFactory.h"
#include "MComponentFactory.h"
#include "MUtility.h"
#include "MAddRemoveComponentCommand.h"
#include "MRenameEntityCommand.h"
#include "MChangeHiddenStateCommand.h"
#include "MChangeFrozenStateCommand.h"
#include "MChangeEntityTagsCommand.h"
#include "ServiceProvider.h"
#include "MEntityPropertyDescriptor.h"
#include "MExpandablePropertyConverter.h"
#include "MFramework.h"
#include "MLayerManager.h"
#include "IEntityPathService.h"
#include "IPropertyTypeService.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace System::Drawing;

static const char* gs_pcSceneRootPointerName = "Scene Root Pointer";

//---------------------------------------------------------------------------
MEntity::MEntity(NiEntityInterface* pkEntity) : m_pkEntity(pkEntity)
{
    MInitInterfaceReference(m_pkEntity);
}
//---------------------------------------------------------------------------
void MEntity::_SDMInit()
{
    m_pkPropertyNames = NiNew NiTObjectSet<NiFixedString>(64);
}
//---------------------------------------------------------------------------
void MEntity::_SDMShutdown()
{
    NiDelete m_pkPropertyNames;
}
//---------------------------------------------------------------------------
NiEntityPropertyInterface* MEntity::get_PropertyInterface()
{
    return m_pkEntity;
}
//---------------------------------------------------------------------------
void MEntity::Do_Dispose(bool bDisposing)
{
    __super::Do_Dispose(bDisposing);

    if (bDisposing)
    {
        const unsigned int uiArrayCount = m_pkEntity->GetComponentCount();
        for (unsigned int ui = 0; ui < uiArrayCount; ui++)
        {
            MComponentFactory::Instance->Remove(m_pkEntity->GetComponentAt(
                ui));
        }

        MDisposeInterfaceReference(m_pkEntity);
    }
}
//---------------------------------------------------------------------------
String* MEntity::ToString()
{
    MVerifyValidInstance;

    return this->Name;
}
//---------------------------------------------------------------------------
String* MEntity::get_Name()
{
    MVerifyValidInstance;

    return m_pkEntity->GetName();
}
//---------------------------------------------------------------------------
void MEntity::set_Name(String* strName)
{
    MVerifyValidInstance;

    this->Dirty = true;

    if (!strName->Equals(this->Name))
    {
        // Check for unique names here.
        if (MFramework::Instance->LayerManager->FindEntityByName(strName))
        {
            throw new ArgumentException(String::Format("An entity "
                "already exists in the scene with the name \"{0}\"; new name "
                "cannot be set.", strName));
        }

        const char* pcName = MStringToCharPointer(strName);
        CommandService->ExecuteCommand(new MRenameEntityCommand(
            NiNew NiRenameEntityCommand(m_pkEntity, pcName)), true);
        MFreeCharPointer(pcName);
    }
}
//---------------------------------------------------------------------------
Guid MEntity::get_TemplateID()
{
    MVerifyValidInstance;

    return MUtility::IDToGuid(m_pkEntity->GetTemplateID());
}
//---------------------------------------------------------------------------
void MEntity::set_TemplateID(Guid mGuid)
{
    MVerifyValidInstance;

    this->Dirty = true;

    NiUniqueID kUniqueID;
    MUtility::GuidToID(mGuid, kUniqueID);
    m_pkEntity->SetTemplateID(kUniqueID);
}
//---------------------------------------------------------------------------
MEntity* MEntity::CreateGeneralEntity(String* strName)
{
    const char* pcName = MStringToCharPointer(strName);
    NiUniqueID kTemplateID;
    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
    NiGeneralEntity* pkEntity = NiNew NiGeneralEntity(pcName, kTemplateID);
    MFreeCharPointer(pcName);

    return MEntityFactory::Instance->Get(pkEntity);
}
//---------------------------------------------------------------------------
MEntity* MEntity::Clone(String* strNewName, bool bInheritProperties)[]
{
    MVerifyValidInstance;

    MEntityCloneHelper* cloner = new MEntityCloneHelper;
    cloner->Clone(this, strNewName, bInheritProperties);
    cloner->Fixup(true);
    MEntity* amEntities[] = cloner->GetEntities();
    cloner->Dispose();

    return amEntities;
}
//---------------------------------------------------------------------------
MEntity* MEntity::get_MasterEntity()
{
    MVerifyValidInstance;

    NiEntityInterface* pkMasterEntity = m_pkEntity->GetMasterEntity();
    if (pkMasterEntity)
    {
        return MEntityFactory::Instance->Get(pkMasterEntity);
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MEntity::set_MasterEntity(MEntity* pmEntity)
{
    MVerifyValidInstance;

    // we don't set the dirty flag here because this should only happen if 
    // it's already dirty and changing it here causes problems when converting 
    // a read-only file.
    
    MAssert(pmEntity != this, "Master Entity set to itself");

    if (pmEntity)
    {
        m_pkEntity->ReplaceMasterEntity(pmEntity->m_pkEntity);
    }
    else
    {
        m_pkEntity->ReplaceMasterEntity(NULL);
    }
}
//---------------------------------------------------------------------------
MEntity* MEntity::get_RootMasterEntity()
{
    MVerifyValidInstance;

    MEntity* pmMaster = MasterEntity;
    while (pmMaster != NULL && pmMaster->MasterEntity != NULL)
    {
        pmMaster = pmMaster->MasterEntity;
    }

    return pmMaster;
}
//---------------------------------------------------------------------------
void MEntity::MakeEntityUnique()
{
    MVerifyValidInstance;

    this->Dirty = true;

    String* astrPropertyNames[] = GetPropertyNames();

    CommandService->BeginUndoFrame(String::Format("Make \"{0}\" entity "
        "unique", this->Name));
    for (int i = 0; i < astrPropertyNames->Length; i++)
    {
        MakePropertyUnique(astrPropertyNames[i]);
    }
    CommandService->EndUndoFrame(true);
}
//---------------------------------------------------------------------------
MEntity* MEntity::get_PrefabRoot()
{
    MVerifyValidInstance;

    NiEntityInterface* pkPrefabRoot = m_pkEntity->GetPrefabRoot();
    if (pkPrefabRoot)
    {
        return MEntityFactory::Instance->Get(pkPrefabRoot);
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MEntity::set_PrefabRoot(MEntity* pmPrefabRoot)
{
    MVerifyValidInstance;

    MAssert(pmPrefabRoot != this, "Prefab Root set to itself");

    if (pmPrefabRoot)
    {
        m_pkEntity->SetPrefabRoot(pmPrefabRoot->m_pkEntity);
    }
    else
    {
        m_pkEntity->SetPrefabRoot(NULL);
    }
}
//---------------------------------------------------------------------------
bool MEntity::get_IsPrefabRoot()
{
    return HasProperty("Prefab Path");
}
//---------------------------------------------------------------------------
MEntity* MEntity::GetPrefabEntities()[]
{
    MVerifyValidInstance;

    unsigned int uiCount = 0;
    m_pkEntity->GetElementCount("Prefab Entities", uiCount);

    List<MEntity*>* pmPrefabEntities = new List<MEntity*>(uiCount);
    for (unsigned int ui = 0; ui < uiCount; ++ui)
    {
        NiEntityInterface* pkPrefabEntity = NULL;
        m_pkEntity->GetPropertyData(NiPrefabComponent::PROP_PREFAB_ENTITIES(),
            pkPrefabEntity, ui);
        if (pkPrefabEntity)
        {
            pmPrefabEntities->Add(MEntityFactory::Instance->Get(
                pkPrefabEntity));
        }
    }

    return pmPrefabEntities->ToArray();
}
//---------------------------------------------------------------------------
bool MEntity::get_SupportsComponents()
{
    MVerifyValidInstance;

    return NIBOOL_IS_TRUE(m_pkEntity->SupportsComponents());
}
//---------------------------------------------------------------------------
unsigned int MEntity::get_ComponentCount()
{
    MVerifyValidInstance;

    return m_pkEntity->GetComponentCount();
}
//---------------------------------------------------------------------------
MComponent* MEntity::GetComponents()[]
{
    MVerifyValidInstance;

    unsigned int uiComponentCount = m_pkEntity->GetComponentCount();
    MComponent* amComponents[] = new MComponent*[uiComponentCount];
    for (unsigned int ui = 0; ui < uiComponentCount; ui++)
    {
        amComponents[ui] = MComponentFactory::Instance->Get(
            m_pkEntity->GetComponentAt(ui));
    }

    return amComponents;
}
//---------------------------------------------------------------------------
MComponent* MEntity::GetComponentByTemplateID(Guid mTemplateID)
{
    MVerifyValidInstance;

    NiUniqueID kTemplateID;
    MUtility::GuidToID(mTemplateID, kTemplateID);
    return MComponentFactory::Instance->Get(
        m_pkEntity->GetComponentByTemplateID(kTemplateID));
}
//---------------------------------------------------------------------------
bool MEntity::CanAddComponent(MComponent* pmComponent)
{
    MVerifyValidInstance;

    MAssert(pmComponent != NULL, "Null component provided to function!");

    return NIBOOL_IS_TRUE(pmComponent->GetNiEntityComponentInterface()
        ->CanAttachToEntity(m_pkEntity));
}
//---------------------------------------------------------------------------
bool MEntity::CanRemoveComponent(MComponent* pmComponent)
{
    MVerifyValidInstance;

    MAssert(pmComponent != NULL, "Null component provided to function!");

    return NIBOOL_IS_TRUE(pmComponent->GetNiEntityComponentInterface()
        ->CanDetachFromEntity(m_pkEntity));

}
//---------------------------------------------------------------------------
void MEntity::AddComponent(MComponent* pmComponent,
    bool bPerformErrorChecking, bool bUndoable)
{
    MVerifyValidInstance;

    this->Dirty = true;

    MAssert(pmComponent != NULL, "Null component provided to function!");

    CommandService->ExecuteCommand(new MAddRemoveComponentCommand(
        NiNew NiAddRemoveComponentCommand(m_pkEntity, 
        pmComponent->GetNiEntityComponentInterface(), true,
        bPerformErrorChecking)), bUndoable);

}
//---------------------------------------------------------------------------
void MEntity::RemoveComponent(MComponent* pmComponent,
    bool bPerformErrorChecking, bool bUndoable)
{
    MVerifyValidInstance;

    this->Dirty = true;

    MAssert(pmComponent != NULL, "Null component provided to function!");

    CommandService->ExecuteCommand(new MAddRemoveComponentCommand(
        NiNew NiAddRemoveComponentCommand(m_pkEntity, 
        pmComponent->GetNiEntityComponentInterface(), false,
        bPerformErrorChecking)), bUndoable);

}
//---------------------------------------------------------------------------
String* MEntity::GetPropertyNames()[]
{
    MVerifyValidInstance;

    NIASSERT(m_pkPropertyNames->GetSize() == 0);
    m_pkEntity->GetPropertyNames(*m_pkPropertyNames);

    String* astrPropertyNames[] = new String*[m_pkPropertyNames->GetSize()];
    for (unsigned int ui = 0; ui < m_pkPropertyNames->GetSize(); ui++)
    {
        astrPropertyNames[ui] = m_pkPropertyNames->GetAt(ui);
    }

    m_pkPropertyNames->RemoveAll();

    return astrPropertyNames;
}
//---------------------------------------------------------------------------
NiAVObject* MEntity::GetSceneRootPointer(unsigned int uiIndex)
{
    MVerifyValidInstance;

    if (uiIndex >= GetSceneRootPointerCount())
    {
        return NULL;
    }

    NiObject* pkSceneRootPointer = NULL;
    NiAVObject* pkSceneRoot = NULL;
    if (m_pkEntity->GetPropertyData(gs_pcSceneRootPointerName,
        pkSceneRootPointer, uiIndex))
    {
        pkSceneRoot = NiDynamicCast(NiAVObject, pkSceneRootPointer);
    }

    return pkSceneRoot;
}
//---------------------------------------------------------------------------
unsigned int MEntity::GetSceneRootPointerCount()
{
    MVerifyValidInstance;

    unsigned int uiCount;
    if (!m_pkEntity->GetElementCount(gs_pcSceneRootPointerName, uiCount))
    {
        uiCount = 0;
    }

    return uiCount;
}
//---------------------------------------------------------------------------
bool MEntity::get_Hidden()
{
    MVerifyValidInstance;

    if (m_pmLayer != NULL && !m_pmLayer->Visible)
    {
        return true;
    }

    if (PrefabRoot != NULL && PrefabRoot->Hidden)
    {
        return true;
    }

    return LocalHidden;
}
//---------------------------------------------------------------------------
bool MEntity::get_LocalHidden()
{
    return NIBOOL_IS_TRUE(m_pkEntity->GetHidden());
}
//---------------------------------------------------------------------------
void MEntity::set_Hidden(bool bHidden)
{
    MVerifyValidInstance;

    SetHidden(bHidden, true);
}
//---------------------------------------------------------------------------
void MEntity::SetHidden(bool bHidden, bool bUndoable)
{
    MVerifyValidInstance;

    this->Dirty = true;

    if (this->Hidden != bHidden)
    {
        CommandService->ExecuteCommand(new MChangeHiddenStateCommand(
            NiNew NiChangeHiddenStateCommand(m_pkEntity, bHidden)),
            bUndoable);
    }
}
//---------------------------------------------------------------------------
bool MEntity::get_Frozen()
{
    MVerifyValidInstance;

    if (m_pmLayer != NULL && m_pmLayer->Locked)
    {
        return true;
    }

    if (PrefabRoot != NULL && PrefabRoot->Frozen)
    {
        return true;
    }
    
    return LocalFrozen;
}
//---------------------------------------------------------------------------
bool MEntity::get_LocalFrozen()
{
    return m_bFrozen;
}
//---------------------------------------------------------------------------
void MEntity::set_Frozen(bool bFrozen)
{
    MVerifyValidInstance;

    SetFrozen(bFrozen, true);
}
//---------------------------------------------------------------------------
void MEntity::SetFrozen(bool bFrozen, bool bUndoable)
{
    MVerifyValidInstance;

    this->Dirty = true;

    if (m_bFrozen != bFrozen)
    {
        CommandService->ExecuteCommand(new MChangeFrozenStateCommand(this,
            bFrozen), bUndoable);
    }
}
//---------------------------------------------------------------------------
String* MEntity::get_Tags()
{
    MVerifyValidInstance;

    return m_pkEntity->GetTags();
}
//---------------------------------------------------------------------------
void MEntity::set_Tags(String* strTags)
{
    MVerifyValidInstance;

    this->Dirty = true;

    const char* pcTags = MStringToCharPointer(strTags);
    CommandService->ExecuteCommand(new MChangeEntityTagsCommand(NiNew
        NiChangeEntityTagsCommand(m_pkEntity,
        NiChangeEntityTagsCommand::TAGOP_SET, pcTags)), true);
    MFreeCharPointer(pcTags);
}
//---------------------------------------------------------------------------
String* MEntity::get_TagsArray()[]
{
    String* astrTags[] = Tags->Split(TagDelimiterString->ToCharArray());
    List<String*>* pmTags = new List<String*>(astrTags->Length);
    for (int i = 0; i < astrTags->Length; ++i)
    {
        String* strTag = astrTags[i];
        if (!String::IsNullOrEmpty(strTag))
        {
            pmTags->Add(strTag);
        }
    }

    return pmTags->ToArray();
}
//---------------------------------------------------------------------------
void MEntity::AddTag(String* strTag)
{
    MVerifyValidInstance;

    this->Dirty = true;

    const char* pcTag = MStringToCharPointer(strTag);
    CommandService->ExecuteCommand(new MChangeEntityTagsCommand(NiNew
        NiChangeEntityTagsCommand(m_pkEntity,
        NiChangeEntityTagsCommand::TAGOP_ADD, pcTag)), true);
    MFreeCharPointer(pcTag);
}
//---------------------------------------------------------------------------
void MEntity::RemoveTag(String* strTag)
{
    MVerifyValidInstance;

    this->Dirty = true;

    const char* pcTag = MStringToCharPointer(strTag);
    CommandService->ExecuteCommand(new MChangeEntityTagsCommand(NiNew
        NiChangeEntityTagsCommand(m_pkEntity,
        NiChangeEntityTagsCommand::TAGOP_REMOVE, pcTag)), true);
    MFreeCharPointer(pcTag);
}
//---------------------------------------------------------------------------
bool MEntity::ContainsTag(String* strTag)
{
    MVerifyValidInstance;

    const char* pcTag = MStringToCharPointer(strTag);
    bool bContainsTag = NIBOOL_IS_TRUE(m_pkEntity->ContainsTag(pcTag));
    MFreeCharPointer(pcTag);

    return bContainsTag;
}
//---------------------------------------------------------------------------
char MEntity::get_TagDelimiter()
{
    return NiEntityInterface::GetTagDelimiter();
}
//---------------------------------------------------------------------------
String* MEntity::get_TagDelimiterString()
{
    return new String(TagDelimiter, 1);
}
//---------------------------------------------------------------------------
bool MEntity::get_Writable()
{
    return __super::get_Writable();
}
//---------------------------------------------------------------------------
void MEntity::set_Writable(bool bWritable)
{
    __super::set_Writable(bWritable);

    MComponent* amComponents[] = GetComponents();
    for (int i = 0; i < amComponents->Length; ++i)
    {
        amComponents[i]->Writable = bWritable;
    }
}
//---------------------------------------------------------------------------
NiEntityInterface* MEntity::GetNiEntityInterface()
{
    MVerifyValidInstance;

    return m_pkEntity;
}
//---------------------------------------------------------------------------
void MEntity::Update(float fTime, NiExternalAssetManager* pkAssetManager)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkAssetManager->SetErrorHandler(spErrors);

    // Update the entity.
    m_pkEntity->Update(NULL, fTime, spErrors, pkAssetManager);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
}
//---------------------------------------------------------------------------
IMessageService* MEntity::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
PropertyDescriptorCollection* MEntity::GetProperties()
{
    MVerifyValidInstance;

    return GetProperties(new Attribute*[0]);
}
//---------------------------------------------------------------------------
PropertyDescriptorCollection* MEntity::GetProperties(
    Attribute* amAttributes[])
{
    MVerifyValidInstance;

    ArrayList* pmPropertyDescs = new ArrayList();

    String* strGeneralCategory = "General";

    // Name.
    Attribute* amEntityAttributes[] = new Attribute*[3];
    amEntityAttributes[0] = new CategoryAttribute(strGeneralCategory);
    amEntityAttributes[1] = new DescriptionAttribute(
        "The name of the entity.");
    amEntityAttributes[2] = new MergablePropertyAttribute(false);
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::Name, "Name", amEntityAttributes));

    // Entity template.
    amEntityAttributes = new Attribute*[2];
    amEntityAttributes[0] = new CategoryAttribute(strGeneralCategory);
    amEntityAttributes[1] = new DescriptionAttribute("The entity template "
        "from which this entity inherits its properties.");
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::MasterEntity, "Entity Template",
        amEntityAttributes));

    // Layer.
    amEntityAttributes = new Attribute*[2];
    amEntityAttributes[0] = new CategoryAttribute(strGeneralCategory);
    amEntityAttributes[1] = new DescriptionAttribute("The layer to which this "
        "entity belongs.");
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::Layer, "Layer", amEntityAttributes));

    // ID.
    amEntityAttributes = new Attribute*[3];
    amEntityAttributes[0] = new CategoryAttribute(strGeneralCategory);
    amEntityAttributes[1] = new DescriptionAttribute("The unique ID for this "
        "entity.");
    amEntityAttributes[2] = new MergablePropertyAttribute(false);
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::ID, "ID", amEntityAttributes));

    // Hidden.
    amEntityAttributes = new Attribute*[2];
    amEntityAttributes[0] = new CategoryAttribute(strGeneralCategory);
    amEntityAttributes[1] = new DescriptionAttribute("Indicates whether or "
        "not the entity is hidden. Hidden entities are not displayed in the "
        "tool.");
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::Hidden, "Hidden", amEntityAttributes));

    // Frozen.
    amEntityAttributes = new Attribute*[2];
    amEntityAttributes[0] = new CategoryAttribute(strGeneralCategory);
    amEntityAttributes[1] = new DescriptionAttribute("Indicates whether or "
        "not the entity is frozen. Frozen entities cannot be edited in the "
        "tool.");
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::Frozen, "Frozen", amEntityAttributes));

    // Tags.
    amEntityAttributes = new Attribute*[2];
    amEntityAttributes[0] = new CategoryAttribute(strGeneralCategory);
    amEntityAttributes[1] = new DescriptionAttribute("A string containing one "
        "or more delimited text tags used for searching.");
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::Tags, "Tags", amEntityAttributes));

    PropertyDescriptorCollection* pmBaseProperties =
        __super::GetProperties(amAttributes);

    pmPropertyDescs->AddRange(pmBaseProperties);

    PropertyDescriptor* amPropertyDescArray[] =
        dynamic_cast<PropertyDescriptor*[]>(pmPropertyDescs->ToArray(
        __typeof(PropertyDescriptor)));
    return new PropertyDescriptorCollection(amPropertyDescArray);
}
//---------------------------------------------------------------------------
// EntityDescriptor implementation.
//---------------------------------------------------------------------------
MEntity::EntityDescriptor::EntityDescriptor(MEntity* pmEntity,
    ValueType eValueType, String* strName, Attribute* amAttributes[]) :
    PropertyDescriptor(strName, amAttributes), m_pmEntity(pmEntity),
    m_eValueType(eValueType)
{
    MAssert(m_pmEntity != NULL, "MEntity::EntityDescriptor Error: "
        "Null entity provided to constructor.");

    IPropertyTypeService* pmPropertyTypeService = MGetService(
        IPropertyTypeService);
    MAssert(pmPropertyTypeService != NULL, "IPropertyTypeService not found!");
    Emergent::Gamebryo::SceneDesigner::PluginAPI::PropertyType* pmPropertyType
        = pmPropertyTypeService->LookupType("Entity Tags");
    if (pmPropertyType != NULL)
    {
        m_pmTagsEditorType = pmPropertyType->Editor;
        m_pmTagsTypeConverter = pmPropertyType->TypeConverter;
    }
}
//---------------------------------------------------------------------------
MEntity* MEntity::EntityDescriptor::get_Entity()
{
    return m_pmEntity;
}
//---------------------------------------------------------------------------
Type* MEntity::EntityDescriptor::get_ComponentType()
{
    return m_pmEntity->GetType();
}
//---------------------------------------------------------------------------
bool MEntity::EntityDescriptor::get_IsReadOnly()
{
    switch (m_eValueType)
    {
        case ValueType::Name:
        case ValueType::Hidden:
        case ValueType::Frozen:
            if (m_pmEntity->Writable &&
                MFramework::Instance->Scene->GetEntityByID(m_pmEntity->ID))
            {
                return false;
            }
            else
            {
                return true;
            }
        case ValueType::Tags:
            return !m_pmEntity->Writable;
        default:
            return true;
    }
}
//---------------------------------------------------------------------------
Type* MEntity::EntityDescriptor::get_PropertyType()
{
    switch (m_eValueType)
    {
        case ValueType::Name:
            return m_pmEntity->Name->GetType();
        case ValueType::MasterEntity:
            return __typeof(String);
        case ValueType::Layer:
            return __typeof(String);
        case ValueType::ID:
            return __typeof(String);
        case ValueType::Hidden:
            return __box(m_pmEntity->Hidden)->GetType();
        case ValueType::Frozen:
            return __box(m_pmEntity->Frozen)->GetType();
        case ValueType::Tags:
            return __typeof(String);
        default:
            return NULL;
    }
}
//---------------------------------------------------------------------------
bool MEntity::EntityDescriptor::CanResetValue(Object*)
{
    return false;
}
//---------------------------------------------------------------------------
Object* MEntity::EntityDescriptor::GetValue(Object*)
{
    switch (m_eValueType)
    {
        case ValueType::Name:
            return m_pmEntity->Name;
        case ValueType::MasterEntity:
            if (m_pmEntity->MasterEntity != NULL)
            {
                IEntityPathService* pmPathService = 
                    MGetService(IEntityPathService);
                String* strFullPath = 
                    pmPathService->FindFullPath(m_pmEntity->MasterEntity);

                return strFullPath;
            }
            else
            {
                return NULL;
            }
        case ValueType::Layer:
            if (m_pmEntity->Layer != NULL)
            {
                return String::Format("{0} [{1}]",
                    m_pmEntity->Layer->ShortName, m_pmEntity->Layer->Name);
            }
            else
            {
                return NULL;
            }
        case ValueType::ID:
            return new String(m_pmEntity->GetNiEntityInterface()->GetID()
                .ToString());
        case ValueType::Hidden:
            return __box(m_pmEntity->Hidden);
        case ValueType::Frozen:
            return __box(m_pmEntity->Frozen);
        case ValueType::Tags:
            return m_pmEntity->Tags;
        default:
            return NULL;
    }
}
//---------------------------------------------------------------------------
void MEntity::EntityDescriptor::ResetValue(Object*)
{
}
//---------------------------------------------------------------------------
void MEntity::EntityDescriptor::SetValue(Object*, Object* pmValue)
{
    switch (m_eValueType)
    {
        case ValueType::Name:
            m_pmEntity->Name = dynamic_cast<String*>(pmValue);
            break;
        case ValueType::Hidden:
            m_pmEntity->Hidden = *dynamic_cast<__box bool*>(pmValue);
            break;
        case ValueType::Frozen:
            m_pmEntity->Frozen = *dynamic_cast<__box bool*>(pmValue);
            break;
        case ValueType::Tags:
            m_pmEntity->Tags = dynamic_cast<String*>(pmValue);
            break;
        default:
            break;
    }
}
//---------------------------------------------------------------------------
bool MEntity::EntityDescriptor::ShouldSerializeValue(Object*)
{
    return true;
}
//---------------------------------------------------------------------------
Object* MEntity::EntityDescriptor::GetEditor(Type* pmEditorBaseType)
{
    Object* pmEditor = NULL;
    if (m_eValueType == ValueType::Tags && m_pmTagsEditorType != NULL)
    {
        pmEditor = CreateInstance(m_pmTagsEditorType);
    }
    if (pmEditor == NULL)
    {
        pmEditor = __super::GetEditor(pmEditorBaseType);
    }

    return pmEditor;
}
//---------------------------------------------------------------------------
TypeConverter* MEntity::EntityDescriptor::get_Converter()
{
    TypeConverter* pmTypeConverter = NULL;
    if (m_eValueType == ValueType::Tags)
    {
        pmTypeConverter = dynamic_cast<TypeConverter*>(
            CreateInstance(m_pmTagsTypeConverter));
    }
    else
    {
        pmTypeConverter = __super::Converter;
    }

    return pmTypeConverter;
}
//---------------------------------------------------------------------------
