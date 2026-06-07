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

#ifndef NISCENE_H
#define NISCENE_H

#include "NiEntityLibType.h"
#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityInterface.h"
#include "NiEntitySelectionSet.h"
#include "NiTPtrSet.h"
#include <NiAVObject.h>

class NiEntityErrorInterface;
class NiExternalAssetManager;

class NIENTITY_ENTRY NiScene : public NiRefObject
{
public:
    NiScene(const NiFixedString& kName, unsigned int uiEntityArraySize = 0,
        unsigned int uiSelectionSetArraySize = 0,
        unsigned int uiLayerFilenameArrayCount = 0);

    // Functions for getting and setting the scene name.
    inline const NiFixedString& GetName() const;
    inline void SetName(const NiFixedString& kName);

    // Functions for managing entities.
    inline unsigned int GetEntityCount() const;
    inline NiEntityInterface* GetEntityAt(unsigned int uiIndex) const;
    inline NiEntityInterface* GetEntityByName(const NiFixedString& kName) const;
    inline NiEntityInterface* GetEntityByID(const NiUniqueID& kID) const;
    inline NiBool AddEntity(NiEntityInterface* pkEntity);
    inline void RemoveEntity(NiEntityInterface* pkEntity);
    inline void RemoveEntityAt(unsigned int uiIndex);
    inline void RemoveAllEntities();
    inline NiBool IsEntityInScene(NiEntityInterface* pkEntity) const;

    // Functions for managing selection sets.
    inline unsigned int GetSelectionSetCount() const;
    inline NiEntitySelectionSet* GetSelectionSetAt(unsigned int uiIndex) const;
    inline NiEntitySelectionSet* GetSelectionSetByName(const NiFixedString& kName)
        const;
    inline NiBool AddSelectionSet(NiEntitySelectionSet* pkSelectionSet);
    inline void RemoveSelectionSet(NiEntitySelectionSet* pkSelectionSet);
    inline void RemoveSelectionSetAt(unsigned int uiIndex);
    inline void RemoveAllSelectionSets();

    // Functions for managing layer filenames.
    inline unsigned int GetLayerFilenameCount() const;
    inline const NiFixedString& GetLayerFilenameAt(unsigned int uiIndex) const;
    inline NiBool HasLayerFilename(const NiFixedString& kLayerFilename) const;
    inline NiBool AddLayerFilename(const NiFixedString& kLayerFilename);
    inline NiBool RenameLayerFilename(
        const NiFixedString& kOldLayerFilename,
        const NiFixedString& kNewLayerFilename);
    inline void RemoveLayerFilename(const NiFixedString& kLayerFilename);
    inline void RemoveLayerFilenameAt(unsigned int uiIndex);
    inline void RemoveAllLayerFilenames();

    // Computes the world-space bound of the scene.
    inline void GetBound(NiBound& kBound);

    // Updates all entites.
    inline void Update(float fTime, NiEntityErrorInterface* pkErrors,
        NiExternalAssetManager* pkAssetManager);

    // BuildVisibleSet for all entities.
    inline void BuildVisibleSet(NiEntityRenderingContext* pkRenderingContext,
        NiEntityErrorInterface* pkErrors);

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    inline const NiFixedString& GetSourceFilename() const;
    inline void SetSourceFilename(
        const NiFixedString& kSourceFilename,
        bool bUpdateEntities = true);
    // *** end Emergent internal use only ***

private:
    NiFixedString m_kName;
    NiTObjectPtrSet<NiEntityInterfaceIPtr> m_kEntities;
    NiTPointerMap<NiEntityInterface*, bool> m_kQuickEntityLookUpMap;
    NiTObjectPtrSet<NiEntitySelectionSetPtr> m_kSelectionSets;
    NiTObjectSet<NiFixedString> m_kLayerFilenames;
    NiFixedString m_kSourceFilename;

    static NiFixedString ms_kSceneRootName;
};

NiSmartPointer(NiScene);

#include "NiScene.inl"

#endif // NISCENE_H
