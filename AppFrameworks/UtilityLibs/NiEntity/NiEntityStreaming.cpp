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
#include "NiEntityPCH.h"

#include "NiEntityStreaming.h"
#include "NiEntityPropertyInterface.h"
#include "NiPrefabComponent.h"
#include "NiEntityCloneHelper.h"
#include "NiTransformationComponent.h"

#define PT_ENTITYPOINTER    NiEntityPropertyInterface::PT_ENTITYPOINTER
#define PT_NIOBJECTPOINTER  NiEntityPropertyInterface::PT_NIOBJECTPOINTER

//---------------------------------------------------------------------------
// top level Scene list
//---------------------------------------------------------------------------
NiEntityStreaming::NiEntityStreaming() : m_pkErrorHandler(NULL)
{
}
//---------------------------------------------------------------------------
NiEntityStreaming::~NiEntityStreaming()
{
    m_kScenes.RemoveAll();
}
//---------------------------------------------------------------------------
void NiEntityStreaming::InsertScene(NiScene* pkScene)
{
#ifdef NIDEBUG
    NIASSERT(pkScene);

    const unsigned int uiArrayCount = m_kScenes.GetSize();
    for (unsigned int ui = 0; ui < uiArrayCount; ++ui)
    {
        NIASSERT(pkScene != m_kScenes.GetAt(ui));
    }
#endif

    m_kScenes.Add(pkScene);
}
//---------------------------------------------------------------------------
void NiEntityStreaming::RemoveScene(NiScene* pkScene)
{
    const unsigned int uiArrayCount = m_kScenes.GetSize();
    for (unsigned int ui = 0; ui < uiArrayCount; ++ui)
    {
        if (pkScene == m_kScenes.GetAt(ui))
        {
            m_kScenes.OrderedRemoveAt(ui);
            return;
        }
    }
}
//---------------------------------------------------------------------------
void NiEntityStreaming::RemoveAllScenes()
{
    m_kScenes.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int NiEntityStreaming::GetSceneCount() const
{
    return m_kScenes.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NiEntityStreaming::GetEffectiveSceneCount() const
{
    // This function is no longer necessary, since the scenes are stored in
    // an NiTSet, where the effective count is the same as the array count.
    // The function has been left implemented for backwards compatibility.
    return GetSceneCount();
}
//---------------------------------------------------------------------------
NiScene* NiEntityStreaming::GetSceneAt(unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kScenes.GetSize());
    return m_kScenes.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
void NiEntityStreaming::AddLoadedScene(NiScene* pkScene)
{
    NIASSERT(pkScene);

    NIASSERT(!GetLoadedScene(pkScene->GetSourceFilename()));
    m_kLoadedScenes.Add(pkScene);
}
//---------------------------------------------------------------------------
void NiEntityStreaming::RemoveLoadedScene(NiFixedString kSourceFilename)
{
    const unsigned int uiSceneCount = m_kLoadedScenes.GetSize();
    for (unsigned int ui = 0; ui < uiSceneCount; ++ui)
    {
        if (m_kLoadedScenes.GetAt(ui)->GetSourceFilename() == kSourceFilename)
        {
            m_kLoadedScenes.RemoveAt(ui);
            break;
        }
    }
}
//---------------------------------------------------------------------------
NiScene* NiEntityStreaming::GetLoadedScene(
    const NiFixedString& kSourceFilename) const
{
    const unsigned int uiSceneCount = m_kLoadedScenes.GetSize();
    for (unsigned int ui = 0; ui < uiSceneCount; ++ui)
    {
        NiScene* pkLoadedScene = m_kLoadedScenes.GetAt(ui);
        if (pkLoadedScene->GetSourceFilename() == kSourceFilename)
        {
            return pkLoadedScene;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiEntityStreaming::ClearLoadedScenes()
{
    m_kLoadedScenes.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int NiEntityStreaming::GetLoadedSceneCount() const
{
    return m_kLoadedScenes.GetSize();
}
//---------------------------------------------------------------------------
NiScene* NiEntityStreaming::GetLoadedSceneAt(unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kLoadedScenes.GetSize());
    return m_kLoadedScenes.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
void NiEntityStreaming::Reset()
{
    RemoveAllScenes();
    ClearLoadedScenes();
}
//---------------------------------------------------------------------------
void NiEntityStreaming::GatherInformation(
    IDLinkMap& kIDMap,
    FilenameSet& kFilenameSet,
    unsigned int uiSceneIdx)
{
    NIASSERT(uiSceneIdx < m_kScenes.GetSize());

    kIDMap.RemoveLinks();
    kFilenameSet.Clear();

    NiScene* pkScene = GetSceneAt(uiSceneIdx);

    // Grab the old source filename for the scene.
    const NiFixedString kOldFilename = pkScene->GetSourceFilename();

    // Set the source filename for the scene to the current filename.
    pkScene->SetSourceFilename(kFilenameSet.GetCurrentFilename());

    // Process layer filenames.
    unsigned int uiLayerCount = pkScene->GetLayerFilenameCount();
    for (unsigned int uiLayer = 0; uiLayer < uiLayerCount; ++uiLayer)
    {
        kFilenameSet.Add(pkScene->GetLayerFilenameAt(uiLayer), true, true,
            pkScene->GetSourceFilename());
    }

    // Process selection set entities.
    unsigned int uiSelectionSetCount = pkScene->GetSelectionSetCount();
    for (unsigned int uiSet = 0; uiSet < uiSelectionSetCount; uiSet++)
    {
        NiEntitySelectionSet* pkSet = pkScene->GetSelectionSetAt(uiSet);
        NIASSERT(pkSet);

        unsigned int uiEntityCount = pkSet->GetEntityCount();
    
        for (unsigned int uiEntity = 0; uiEntity < uiEntityCount;
            ++uiEntity)
        {
            NiEntityInterface* pkEntity = pkSet->GetEntityAt(uiEntity);
            NIASSERT(pkEntity);

            GatherInformation(kIDMap, kFilenameSet, kOldFilename,
                pkEntity, true);
        }
    }

    // Process scene entities.
    unsigned int uiEntityCount = pkScene->GetEntityCount();
    for (unsigned int uiEntity = 0; uiEntity < uiEntityCount; uiEntity++)
    {
        NiEntityInterface* pkEntity = pkScene->GetEntityAt(uiEntity);

        if (pkEntity == NULL)
        {
            continue;
        }
        
        GatherInformation(kIDMap, kFilenameSet, kOldFilename, pkEntity, true);
    }
}
//---------------------------------------------------------------------------
void NiEntityStreaming::GatherInformation(
    IDLinkMap& kIDMap,
    FilenameSet& kFilenameSet,
    const NiFixedString& kOldFilename,
    NiEntityPropertyInterface* pkPropIntf,
    bool bLayer)
{
    NIASSERT(pkPropIntf);

    // Early out if we have already recorded this ID.
    if (kIDMap.Get(pkPropIntf->GetID()))
    {
        return;
    }

    // Add this ID to the map.
    kIDMap.Add(pkPropIntf, NULL);

    // Add the source filename to the set.
    kFilenameSet.Add(pkPropIntf->GetSourceFilename(), bLayer, false, NULL);

    NiFixedString kInterfaceType = pkPropIntf->GetInterfaceType();
    if (kInterfaceType == NiEntityInterface::IT_ENTITYINTERFACE)
    {
        NiEntityInterface* pkEntity = (NiEntityInterface*) pkPropIntf;

        // Check for master entity.
        NiEntityInterface* pkMasterEntity = pkEntity->GetMasterEntity();
        if (pkMasterEntity)
        {
            GatherInformation(kIDMap, kFilenameSet, kOldFilename,
                pkMasterEntity, false);
        }

        // Check to see if there are any components.
        unsigned int uiComponentCount = pkEntity->GetComponentCount();
        for (unsigned int uiComp = 0; uiComp < uiComponentCount; ++uiComp)
        {
            NiEntityComponentInterface* pkComp = pkEntity->GetComponentAt(
                uiComp);
            if (!pkComp)
            {
                continue;
            }

            GatherInformation(kIDMap, kFilenameSet, kOldFilename, pkComp,
                bLayer);
        }
    }
    else if (kInterfaceType ==
        NiEntityComponentInterface::IT_COMPONENTINTERFACE)
    {
        NiEntityComponentInterface* pkComp = (NiEntityComponentInterface*)
            pkPropIntf;

        // Check for master component.
        NiEntityComponentInterface* pkMasterComp =
            pkComp->GetMasterComponent();
        if (pkMasterComp)
        {
            GatherInformation(kIDMap, kFilenameSet, kOldFilename, pkMasterComp,
                false);
        }
        NiFixedString prefabpath;
        if (pkComp->GetPropertyData("Prefab Path", prefabpath))
        {
            kFilenameSet.Add(prefabpath, false, false, NULL);
        }
    }

    if (bLayer)
    {
        // Check to see if there are any property interface pointers.
        NiTObjectSet<NiFixedString> kPropertyNames;
        pkPropIntf->GetPropertyNames(kPropertyNames);
        unsigned int uiNameCount = kPropertyNames.GetSize();
        for (unsigned int uiProp = 0; uiProp < uiNameCount; ++uiProp)
        {
            const NiFixedString& kPropertyName = kPropertyNames.GetAt(uiProp);

            NiFixedString kPrimitiveType;
            if (!pkPropIntf->GetPrimitiveType(kPropertyName, kPrimitiveType))
            {
                continue;
            }
            if (kPrimitiveType != PT_ENTITYPOINTER)
            {
                continue;
            }

            bool bIsSerializable;
            if (!pkPropIntf->IsPropertySerializable(kPropertyName,
                bIsSerializable) || !bIsSerializable)
            {
                continue;
            }

            unsigned int uiSubEntityCount;
            NIVERIFY(pkPropIntf->GetElementCount(kPropertyName,
                uiSubEntityCount));
            for (unsigned int uiEntity = 0; uiEntity < uiSubEntityCount;
                ++uiEntity)
            {
                NiEntityInterface* pkSubEntity = NULL;
                pkPropIntf->GetPropertyData(kPropertyName, pkSubEntity,
                    uiEntity);
                if (!pkSubEntity)
                {
                    continue;
                }

                GatherInformation(kIDMap, kFilenameSet, kOldFilename,
                    pkSubEntity, true);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiEntityStreaming::ReportError(const NiFixedString& kErrorMessage,
    const NiFixedString& kErrorDescription, const NiFixedString& kEntityName,
    const NiFixedString& kPropertyName)
{
    if (m_pkErrorHandler == NULL)
        return;

    m_pkErrorHandler->ReportError(kErrorMessage, kErrorDescription,
        kEntityName, kPropertyName);
}
//---------------------------------------------------------------------------
void NiEntityStreaming::SetErrorHandler(NiEntityErrorInterface* pkErrorIntf)
{
    m_pkErrorHandler = pkErrorIntf;
}
//---------------------------------------------------------------------------
void NiEntityStreaming::IDLinkMap::RemoveLinks() 
{
    m_kIDMap.RemoveAll();
};
//---------------------------------------------------------------------------
void NiEntityStreaming::IDLinkMap::Add(
    NiEntityPropertyInterface* pkPropIntf,
    NiScene* pkScene)
{
    NIASSERT(pkPropIntf);

    MapItem kItem;
    m_kIDMap.GetAt(&pkPropIntf->GetID(), kItem);
    kItem.m_spPropIntf = pkPropIntf;
    kItem.m_spScene = pkScene;
    m_kIDMap.SetAt(&pkPropIntf->GetID(), kItem);
}
//---------------------------------------------------------------------------
NiEntityPropertyInterface* NiEntityStreaming::IDLinkMap::Get(
    const NiUniqueID& kID)
{
    MapItem kItem;
    if (m_kIDMap.GetAt(&kID, kItem))
    {
        // If a scene exists and the interface is an entity interface,
        // check to make sure that the entity is in the scene. If not, remove
        // the entry from the map for the entity and all its components and
        // return NULL.
        if (kItem.m_spScene && 
            kItem.m_spPropIntf->GetInterfaceType() ==
                NiEntityInterface::IT_ENTITYINTERFACE)
        {
            NiEntityInterface* pkEntity = NiSmartPointerCast(NiEntityInterface,
                kItem.m_spPropIntf);
            if (!kItem.m_spScene->IsEntityInScene(pkEntity))
            {
                const unsigned int uiCompCount = pkEntity->GetComponentCount();
                for (unsigned int uiComp = 0; uiComp < uiCompCount; ++uiComp)
                {
                    NiEntityComponentInterface* pkComp =
                        pkEntity->GetComponentAt(uiComp);
                    m_kIDMap.RemoveAt(&pkComp->GetID());
                }

                m_kIDMap.RemoveAt(&kID);
                return NULL;
            }
        }

        return kItem.m_spPropIntf;
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiEntityStreaming::IDLinkMap::Remove(const NiUniqueID& kID)
{
    m_kIDMap.RemoveAt(&kID);
}
//---------------------------------------------------------------------------
unsigned int NiEntityStreaming::IDLinkMap::GetCount() const
{
    return m_kIDMap.GetCount();
}
//---------------------------------------------------------------------------
NiTMapIterator NiEntityStreaming::IDLinkMap::GetFirstPos() const
{
    return m_kIDMap.GetFirstPos();
}
//---------------------------------------------------------------------------
void NiEntityStreaming::IDLinkMap::GetNext(
    NiTMapIterator& kIter,
    NiUniqueID& kID,
    NiEntityPropertyInterface*& pkPropIntf,
    NiScene*& pkScene) const
{
    const NiUniqueID* pkID;
    MapItem kItem;
    m_kIDMap.GetNext(kIter, pkID, kItem);
    NIASSERT(pkID);
    kID = *pkID;
    NIASSERT(kItem.m_spPropIntf && kItem.m_spPropIntf->GetID() == kID);
    pkPropIntf = kItem.m_spPropIntf;
    pkScene = kItem.m_spScene;
}
//---------------------------------------------------------------------------
NiEntityStreaming::FilenameSet::~FilenameSet()
{
    Clear();
}
//---------------------------------------------------------------------------
void NiEntityStreaming::FilenameSet::Clear()
{
    m_kFilenames.RemoveAll();

    NiTMapIterator kIter = m_kExplicitLayerMap.GetFirstPos();
    while (kIter)
    {
        NiFixedString kSourceFilename;
        NiTObjectSet<NiFixedString>* pkLayerSet = NULL;
        m_kExplicitLayerMap.GetNext(kIter, kSourceFilename, pkLayerSet);

        NiDelete pkLayerSet;
    }
    m_kExplicitLayerMap.RemoveAll();
}
//---------------------------------------------------------------------------
void NiEntityStreaming::FilenameSet::Initialize(
    const NiFixedString& kCurrentFilename)
{
    Clear();
    NIASSERT(kCurrentFilename.Exists());
    m_kCurrentFilename = kCurrentFilename;
}
//---------------------------------------------------------------------------
void NiEntityStreaming::FilenameSet::Add(
    const NiFixedString& kFilename,
    bool bLayer,
    bool bExplicit,
    NiFixedString kSourceFilename)
{
    NIASSERT(m_kCurrentFilename.Exists());
    if (kFilename.Exists())
    {
        if (!kFilename.EqualsNoCase(m_kCurrentFilename))
        {
            unsigned int uiIndex;
            for (uiIndex = 0; uiIndex < m_kFilenames.GetSize(); ++uiIndex)
            {
                if (kFilename.EqualsNoCase(m_kFilenames.GetAt(uiIndex).m_kFilename))
                {
                    break;
                }
            }
            if (uiIndex < m_kFilenames.GetSize())
            {
                if (bLayer)
                {
                    m_kFilenames.GetAt(uiIndex).m_bLayer = true;
                }
                if (bExplicit)
                {
                    m_kFilenames.GetAt(uiIndex).m_bExplicit = true;
                }
            }
            else
            {
                FilenameItem kItem(kFilename, bLayer, bExplicit);
                m_kFilenames.Add(kItem);
            }
        }

        if (bLayer && bExplicit && kSourceFilename.Exists())
        {
            NiTObjectSet<NiFixedString>* pkLayerSet = NULL;
            if (!m_kExplicitLayerMap.GetAt(kSourceFilename, pkLayerSet))
            {
                pkLayerSet = NiNew NiTObjectSet<NiFixedString>();
                m_kExplicitLayerMap.SetAt(kSourceFilename, pkLayerSet);
            }
            NIASSERT(pkLayerSet);

            pkLayerSet->AddUnique(kFilename);
        }
    }
}
//---------------------------------------------------------------------------
const NiFixedString& NiEntityStreaming::FilenameSet::GetCurrentFilename() const
{
    return m_kCurrentFilename;
}
//---------------------------------------------------------------------------
unsigned int NiEntityStreaming::FilenameSet::GetCount() const
{
    return m_kFilenames.GetSize();
}
//---------------------------------------------------------------------------
const NiFixedString& NiEntityStreaming::FilenameSet::GetAt(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kFilenames.GetSize());
    return m_kFilenames.GetAt(uiIndex).m_kFilename;
}
//---------------------------------------------------------------------------
void NiEntityStreaming::FilenameSet::RemoveAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_kFilenames.GetSize());
    m_kFilenames.OrderedRemoveAt(uiIndex);
}
//---------------------------------------------------------------------------
bool NiEntityStreaming::FilenameSet::IsLayer(unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kFilenames.GetSize());
    return m_kFilenames.GetAt(uiIndex).m_bLayer;
}
//---------------------------------------------------------------------------
bool NiEntityStreaming::FilenameSet::IsLayer(const NiFixedString& kFilename)
    const
{
    const unsigned int uiArrayCount = m_kFilenames.GetSize();
    for (unsigned int ui = 0; ui < uiArrayCount; ++ui)
    {
        if (m_kFilenames.GetAt(ui).m_kFilename == kFilename)
        {
            return m_kFilenames.GetAt(ui).m_bLayer;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiEntityStreaming::FilenameSet::IsExplicit(unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kFilenames.GetSize());
    return m_kFilenames.GetAt(uiIndex).m_bExplicit;
}
//---------------------------------------------------------------------------
bool NiEntityStreaming::FilenameSet::IsExplicit(const NiFixedString& kFilename)
    const
{
    const unsigned int uiArrayCount = m_kFilenames.GetSize();
    for (unsigned int ui = 0; ui < uiArrayCount; ++ui)
    {
        if (m_kFilenames.GetAt(ui).m_kFilename == kFilename)
        {
            return m_kFilenames.GetAt(ui).m_bExplicit;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
const NiTObjectSet<NiFixedString>*
    NiEntityStreaming::FilenameSet::GetExplicitLayersForFilename(
    NiFixedString kFilename) const
{
    NiTObjectSet<NiFixedString>* pkLayerSet = NULL;
    if (m_kExplicitLayerMap.GetAt(kFilename, pkLayerSet))
    {
        return pkLayerSet;
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiEntityStreaming::FilenameSet::FilenameItem::FilenameItem() :
    m_kFilename(NULL),
    m_bLayer(false),
    m_bExplicit(false)
{
}
//---------------------------------------------------------------------------
NiEntityStreaming::FilenameSet::FilenameItem::FilenameItem(
    const NiFixedString& kFilename,
    bool bLayer,
    bool bExplicit) :
    m_kFilename(kFilename),
    m_bLayer(bLayer),
    m_bExplicit(bExplicit)
{
}
//---------------------------------------------------------------------------
bool NiEntityStreaming::VerifyEntityComponents(NiEntityInterface* pkEntity)
{
    NiEntityInterface* pkMasterEntity = NULL;
        
    if (pkEntity->GetInterfaceType() == NiEntityInterface::IT_ENTITYINTERFACE)
        pkMasterEntity = pkEntity->GetMasterEntity();

    if (!pkMasterEntity)
    {
        return false;
    }

    bool bRetVal = false;
    unsigned int uiNumMasterComponents = pkMasterEntity->GetComponentCount();
    unsigned int uiNumEntityComponents = pkEntity->GetComponentCount();
    // When we loaded the entity, we removed any components that did not exist
    // on the master entity.  As a result, either the two entities have the
    // same components or the master entity has components that the entity
    // does not have
    for(unsigned int i = 0; i < uiNumMasterComponents; i++)
    {
        NiEntityComponentInterface* pkMasterComponent = 
            pkMasterEntity->GetComponentAt(i);
        NiEntityComponentInterface* pkEntityComponent = NULL;
        for (unsigned int j = 0; j < uiNumEntityComponents; j++)
        {
            pkEntityComponent = pkEntity->GetComponentAt(j);
            // The best way to match components is to check the class name
            // and the component names
            if (pkMasterComponent->GetClassName().Equals(
                    pkEntityComponent->GetClassName()) &&
                pkMasterComponent->GetName().Equals(
                    pkEntityComponent->GetName()))
                break;
            pkEntityComponent = NULL;
        }
        if (!pkEntityComponent)
        {
            char acFilenameErrorMsg[512];
            NiSprintf(acFilenameErrorMsg, 512, "GSA File Error: %s", 
                (const char*)pkEntity->GetSourceFilename());
            ReportError("CONFLICT: Entity missing component that exists "
                        "on master.  Adding component to entity.",
                        acFilenameErrorMsg, pkEntity->GetName(), 
                        pkMasterComponent->GetName());
            pkEntity->AddComponent(pkMasterComponent->Clone(true));
            bRetVal = true;
        }
    }
    return bRetVal;
}
//---------------------------------------------------------------------------
bool NiEntityStreaming::VerifyPrefabEntities(NiScene* pkScene,
    NiEntityInterface* pkPrefabEntity, NiScene* pkPrefabScene, 
    bool& bIsError)
{
    if (!pkPrefabScene || !pkPrefabEntity)
    {
        bIsError = true;
        return false;
    }

    // Assumed that there are no errors
    bIsError = false;

    // Walk the prefab scene's entities to find out which entities are missing
    // in the prefab entity.
    unsigned int uiCount = pkPrefabScene->GetEntityCount();
    bool bChangedEntity = false;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NiEntityInterface* pkEntity = pkPrefabScene->GetEntityAt(ui);
        // We only want entities at the root level (no prefab root) because
        // the rest will be picked up recursively
        if (pkEntity->GetPrefabRoot())
        {
            continue;
        }
        bChangedEntity |= VerifyEntityInPrefab(pkScene, pkPrefabEntity, 
            pkEntity, bIsError);
        if (bIsError)
            break;
    }
    return bChangedEntity;
}
//---------------------------------------------------------------------------
bool NiEntityStreaming::VerifySubPrefabEntities(NiScene* pkScene, 
    NiEntityInterface* pkPrefab, NiEntityInterface* pkPrefabSrc,
    bool& bIsError)
{
    unsigned int uiCount;
    NIVERIFY(pkPrefabSrc->GetElementCount(
        NiPrefabComponent::PROP_PREFAB_ENTITIES(), uiCount));
    bool bChangedEntity = false;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NiEntityInterface* pkEntity;
        if (!pkPrefabSrc->GetPropertyData(
            NiPrefabComponent::PROP_PREFAB_ENTITIES(), pkEntity, ui))
        {
            bIsError = true;
            break;
        }
        if (!pkEntity)
            continue;
        bChangedEntity |= VerifyEntityInPrefab(pkScene, pkPrefab, pkEntity, 
            bIsError);
        if (bIsError)
            break;
    }
    return bChangedEntity;
}
//---------------------------------------------------------------------------
bool NiEntityStreaming::VerifyEntityInPrefab(NiScene* pkScene, 
    NiEntityInterface* pkPrefab, NiEntityInterface* pkEntity, 
    bool& bIsError)
{
    unsigned int uiCount;
    NIVERIFY(pkPrefab->GetElementCount(
        NiPrefabComponent::PROP_PREFAB_ENTITIES(), uiCount));
    bool bChangedEntity = false;
    NiEntityInterface* pkPrefabEntity = NULL;
    unsigned int ui;
    for (ui = 0; ui < uiCount; ui++)
    {
        if (!pkPrefab->GetPropertyData(
            NiPrefabComponent::PROP_PREFAB_ENTITIES(), pkPrefabEntity, ui))
        {
            bIsError = true;
            return bChangedEntity;
        }
        if (pkPrefabEntity)
        {
            // Every entity in the prefab should have a master
            NiEntityInterface* pkMasterEntity = 
                pkPrefabEntity->GetMasterEntity();
            if (pkMasterEntity)
            {
                // We have matched the entity in the prefab with
                // the entity in the scene
                if (pkMasterEntity == pkEntity)
                    break;
            }
            else
            {
                bIsError = true;
                return bChangedEntity;
            }
        }
    }

    // We found an entity that needs to be cloned from the prefab scene
    // and added to the prefab entity.
    if (ui == uiCount)
    {
        char acFilenameErrorMsg[512];
        NiSprintf(acFilenameErrorMsg, 512, "GSA File Error: %s", 
            (const char*)pkScene->GetSourceFilename());
        NiFixedString kPrefabPath;
        if (pkEntity->GetPropertyData(
            NiPrefabComponent::PROP_PREFAB_PATH(), kPrefabPath))
        {
            // We have to clone a whole prefab tree
            NiEntityCloneHelper kHelper;
            kHelper.Clone(pkEntity, pkEntity->GetName(), true);
            kHelper.Fixup(true);
            unsigned int uiCloneCount = kHelper.GetEntityCount();
            
            if (uiCloneCount == 0)
            {
                bIsError = true;
                return bChangedEntity;
            }

            // The entity at 0 is the prefab root.  Set its root to the
            // prefab root we have been verifying.
            pkPrefabEntity = kHelper.GetEntityAt(0);
            pkPrefabEntity->SetPrefabRoot(pkPrefab);

            // Add all these entities to the scene
            for (unsigned int uiClone = 0; uiClone < uiCloneCount; 
                uiClone++)
            {
                pkScene->AddEntity(kHelper.GetEntityAt(uiClone));
            }
            ReportError("CONFLICT: Prefab missing subprefab that exists "
                "on master.  Adding a clone of the subprefab.",
                acFilenameErrorMsg, pkPrefab->GetName(), 
                pkEntity->GetName());
        }
        else
        {
            // We have to clone an entity
            pkPrefabEntity = pkEntity->Clone(pkEntity->GetName(), true);
            pkPrefabEntity->SetPrefabRoot(pkPrefab);
            pkScene->AddEntity(pkPrefabEntity);
            ReportError("CONFLICT: Prefab missing entity that exists "
                "on master.  Adding a clone of that entity to the "
                "prefab.", acFilenameErrorMsg, pkPrefab->GetName(), 
                pkEntity->GetName());
        }
        NiEntityInterface* pkSourceEntity = NULL;
        if (pkPrefabEntity->GetPropertyData(
            NiTransformationComponent::PROP_SOURCE_ENTITY(),
            pkSourceEntity))
        {
            pkPrefabEntity->SetPropertyData(
                NiTransformationComponent::PROP_SOURCE_ENTITY(), 
                pkPrefab);
        }
        else
        {
            pkPrefabEntity->AddComponent(
                NiNew NiTransformationComponent(pkPrefab));
        }
        bChangedEntity = true;
    }
    else
    {
        // We found a matching entity.  If the entity is a prefab, then 
        // we need to make sure it has everything it is supposed to have
        NIVERIFY(pkPrefabEntity);
        NiFixedString kPrefabPath;
        if (pkEntity->GetPropertyData(
            NiPrefabComponent::PROP_PREFAB_PATH(), kPrefabPath))
        {
            if (VerifySubPrefabEntities(pkScene, pkPrefabEntity, pkEntity, 
                bIsError))
            {
                bChangedEntity = true;
            }
        }
    }
    return bChangedEntity;
}
//---------------------------------------------------------------------------
