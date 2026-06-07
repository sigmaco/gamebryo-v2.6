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

#ifndef NIENTITYSTREAMING_H
#define NIENTITYSTREAMING_H

#include "NiEntityLibType.h"
#include "NiRefObject.h"
#include "NiScene.h"
#include <NiTPointerMap.h>
#include <NiTPtrSet.h>
#include <NiTFixedStringMap.h>

// Abstract class for dealing with various streaming methods
class NIENTITY_ENTRY NiEntityStreaming : public NiRefObject
{
public:
    NiEntityStreaming();
    virtual ~NiEntityStreaming();

    // Scene Management Functions.
    void InsertScene(NiScene* pkScene);
    void RemoveScene(NiScene* pkScene);
    void RemoveAllScenes();

    unsigned int GetSceneCount() const;
    unsigned int GetEffectiveSceneCount() const;
    NiScene* GetSceneAt(unsigned int uiIndex) const;

    // Adds the specified scene to a map indexed by source filename. This map
    // is retained between loads unless the ClearLoadedScenes function is
    // called.
    virtual void AddLoadedScene(NiScene* pkScene);

    // Removes the scene with the specified source filename from the loaded
    // scene map.
    virtual void RemoveLoadedScene(NiFixedString kSourceFilename);

    // Returns a pointer to the NiScene object with the specified source
    // filename if that scene has already been loaded. Returns NULL if the
    // scene has not been loaded.
    NiScene* GetLoadedScene(const NiFixedString& kSourceFilename) const;

    // Clears the map of loaded scenes.
    virtual void ClearLoadedScenes();

    unsigned int GetLoadedSceneCount() const;
    NiScene* GetLoadedSceneAt(unsigned int uiIndex) const;

    // Resets all cached data from the streaming object.
    virtual void Reset();

    // I/O Functions.
    virtual NiBool Load(
        const char* pcFileName,
        bool bUseSingleScene = true, 
        bool bFullyLoadDependencies = false) = 0;
    virtual NiBool Save(
        const char* pcFileName,
        unsigned int uiSceneIdx = 0) = 0;

    // Set Error Handler
    void SetErrorHandler(NiEntityErrorInterface* pkErrorIntf);

    // File Extension of format that can be load/saved.
    // Also used for registering factory so that different files
    // can be read and saved appropriately.
    virtual NiFixedString GetFileExtension() const = 0;
    virtual NiFixedString GetFileDescription() const = 0;

    // This function returns whether or not an old version of the file was
    // converted upon load. This can be used to mark a file as needing to be
    // saved.
    virtual NiBool HasBeenConverted(NiScene* pkScene) const = 0;

    // This function returns whether or not the scene has been modified during
    // the loading process.
    virtual NiBool HasBeenModified(NiScene* pkScene) const = 0;

protected:
    class NIENTITY_ENTRY IDLinkMap
    {
    public:
        void RemoveLinks();

        void Add(NiEntityPropertyInterface* pkPropIntf, NiScene* pkScene);
        NiEntityPropertyInterface* Get(const NiUniqueID& kID);
        void Remove(const NiUniqueID& kID);

        unsigned int GetCount() const;
        NiTMapIterator GetFirstPos() const;
        void GetNext(
            NiTMapIterator& kIter,
            NiUniqueID& kID,
            NiEntityPropertyInterface*& pkPropIntf,
            NiScene*& pkScene) const;

    private:
        struct MapItem : public NiMemObject
        {
            MapItem(){}
            MapItem(int){}

            NiEntityPropertyInterfaceIPtr m_spPropIntf;
            NiScenePtr m_spScene;
        };

        NiTMap<const NiUniqueID*, MapItem,
            NiUniqueID::HashFunctor, NiUniqueID::HashFunctor> m_kIDMap;
    };

    class NIENTITY_ENTRY FilenameSet
    {
    public:
        ~FilenameSet();

        void Clear();
        void Initialize(const NiFixedString& kCurrentFilename);
        void Add(
            const NiFixedString& kFilename,
            bool bLayer,
            bool bExplicit,
            NiFixedString kSourceFilename);

        const NiFixedString& GetCurrentFilename() const;
        unsigned int GetCount() const;
        const NiFixedString& GetAt(unsigned int uiIndex) const;
        void RemoveAt(unsigned int uiIndex);
        bool IsLayer(unsigned int uiIndex) const;
        bool IsLayer(const NiFixedString& kFilename) const;
        bool IsExplicit(unsigned int uiIndex) const;
        bool IsExplicit(const NiFixedString& kFilename) const;

        const NiTObjectSet<NiFixedString>* GetExplicitLayersForFilename(
            NiFixedString kFilename) const;

    private:
        struct FilenameItem : public NiMemObject
        {
            FilenameItem();
            FilenameItem(
                const NiFixedString& kFilename,
                bool bLayer,
                bool bExplicit);

            NiFixedString m_kFilename;
            bool m_bLayer;
            bool m_bExplicit;
        };
        NiTObjectSet<FilenameItem> m_kFilenames;
        NiTFixedStringMap<NiTObjectSet<NiFixedString>*> m_kExplicitLayerMap;
        NiFixedString m_kCurrentFilename;
    };
   
    void GatherInformation(
        IDLinkMap& kIDMap,
        FilenameSet& kFilenameSet,
        unsigned int uiSceneIdx);
    void GatherInformation(
        IDLinkMap& kIDMap,
        FilenameSet& kFilenameSet,
        const NiFixedString& kOldFilename,
        NiEntityPropertyInterface* pkPropIntf,
        bool bLayer);

    void ReportError(const NiFixedString& kErrorMessage,
        const NiFixedString& kErrorDescription,
        const NiFixedString& kEntityName, const NiFixedString& kPropertyName);

    // This function checks the components and component properties of
    // an entity against those of its master entity.  In the conflict 
    // resolution code elsewhere, a common description was used.  Pass
    // that description into this method in case of errors.
    // Return TRUE if verification caused changes in the entity.
    bool VerifyEntityComponents(NiEntityInterface* pkEntity);

    // This function checks the entities in the prefab entity against the 
    // master prefab scene.  Set bIsError=TRUE if the comparison cannot be 
    // completed.  Return TRUE if verification caused changes in the entity.
    bool VerifyPrefabEntities(NiScene* pkScene,
        NiEntityInterface* pkPrefabEntity, NiScene* pkPrefabScene, 
        bool& bIsError);

    // This function checks the entities of a prefab against the source
    // prefab.  Set bIsError=TRUE if the comparison cannot be 
    // completed.  Return TRUE if verification caused changes in the entity 
    bool VerifySubPrefabEntities(NiScene* pkScene, 
        NiEntityInterface* pkPrefab, NiEntityInterface* pkPrefabSrc,
        bool& bIsError);

    // This function goes through the entities in pkPrefab to find one with
    // a master entity of pkEntity.  If an entity is not found, clone pkEntity
    // and adds it to pkPrefab.  If one is found and pkEntity is a prefab, 
    // make sure all the entities in that prefab match the entities in the 
    // prefab specified by pkEntity.  Return TRUE if verification caused
    // changes in pkPrefab
    bool VerifyEntityInPrefab(NiScene* pkScene, NiEntityInterface* pkPrefab,
        NiEntityInterface* pkEntity, bool& bIsError);

    NiTObjectPtrSet<NiScenePtr> m_kScenes;
    NiTObjectPtrSet<NiScenePtr> m_kLoadedScenes;
    NiEntityErrorInterface* m_pkErrorHandler;
};

#endif // NIENTITYSTREAMING_H
