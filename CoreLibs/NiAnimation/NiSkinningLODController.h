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

#ifndef NISKINNINGLODCONTROLLER_H
#define NISKINNINGLODCONTROLLER_H

#include "NiAnimationLibType.h"
#include <NiTimeController.h>
#include <NiTArray.h>
#include <NiNode.h>

class NiMesh;

NiSmartPointer(NiSkinningLODController);

class NIANIMATION_ENTRY NiSkinningLODController : public NiTimeController
{
    NiDeclareRTTI;
    NiDeclareClone(NiSkinningLODController);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    /// Returns the first NiSkinningLODController in a scene graph branch.
    static NiSkinningLODController* FindSkinningLODController(
        NiAVObject* pkObject);

    /// Default Constructor
    NiSkinningLODController();

    /// Destructor
    virtual ~NiSkinningLODController();

    /// Overridden from NiTimeController
    virtual void Update(float fTime);

    /// Sets the skinning LOD level to switch to during the next update. 
    /// The valid range of uiLOD is 0 to GetNumberOfBoneLODs() - 1 where
    /// 0 is the highest level of detail.
    void SetBoneLOD(NiUInt32 uiLOD);

    /// Returns the current skinning LOD level
    inline NiUInt32 GetBoneLOD() const;

    /// Returns the LOD level that will be set during the next update
    inline NiUInt32 GetNextBoneLOD() const;

    /// Returns the total number of LOD levels
    inline NiUInt32 GetNumberOfBoneLODs() const;

    /// @cond EMERGENT_INTERNAL

    /// Sets all of the bones for this controller
    void SetBones(NiUInt32 uiNumBones, NiNode** ppkBones);

    /// Hints the max number of bone LODs
    void SetMaxBoneLODs(NiUInt32 uiMaxBoneLODs);

    /// Adds information about a bone LOD (number of bones, active skin set)
    void AddBoneLOD(NiUInt32 uiBoneLOD, NiUInt32 uiNumBones, 
        NiUInt32 uiActiveSkins = 0);

    /// Returns the number of bones used at a given LOD level
    NiUInt32 GetNumBonesUsed(NiUInt32 uiLODLevel) const;

    /// Adds a skin to an LOD level
    void AddSkin(NiUInt32 uiBoneLOD, NiMesh* pkSkin);

    /// Returns the number of skins associated with a given LOD level.
    NiUInt32 GetNumberOfSkins(NiUInt32 uiBoneLOD) const;

    /// Use this method to iterate over the skins associated with a given
    /// LOD level. Use GetNumberOfSkins() to get the total number of skins 
    /// associated with a given LOD level.
    NiMesh* GetSkin(NiUInt32 uiBoneLOD, NiUInt32 uiSkin);

    /// Initializes a controller once streamed or programmatically created.
    void InitializeNewController();

    /// Returns the lowest level of detail at which the specified node has a
    /// visible effect on the scene graph. 0 is the highest level of detail,
    /// and larger values correspond to increasingly lower levels of detail.
    NiUInt32 GetLODForNode(const NiAVObject* pkObject) const;

    /// @endcond

protected:
    /// Is the target an NiNode?
    inline bool TargetIsRequiredType() const;

    /// Streaming support.
    virtual void PostLinkObject(NiStream& kStream);

    /// Cloning support.
    virtual void ProcessClone(NiCloningProcess& kCloning);

    /// The current LOD level
    NiUInt32 m_uiCurrentLOD;

    /// The next LOD level to change to on Update()
    NiUInt32 m_uiNextLOD;

    class LODInfo : public NiMemObject
    {
    public:
        LODInfo();
        LODInfo(NiUInt32 uiNumBones);

        bool operator ==(const LODInfo& kRHS) const;
        bool operator !=(const LODInfo& kRHS) const;
        const LODInfo& operator =(const LODInfo& kRHS);

        /// Number of bones active at this LOD level
        NiUInt32 m_uiNumBones;

        /// Indices into m_kSkins for the active skins in this LOD level
        NiTPrimitiveArray<NiUInt32> m_kActiveSkins;
    };

    /// LOD settings, indexed by bone LOD level
    NiTPrimitiveArray<LODInfo*> m_kLODs;

    /// Bone array, sorted by lowest active bone LOD level, ascending
    NiTPrimitiveArray<NiNode*> m_kBones;

    /// Skin meshes, unordered.  These may be shared amongst LOD levels.
    NiTPrimitiveArray<NiMesh*> m_kSkins;
};

#include "NiSkinningLODController.inl"

#endif  // #ifndef NISKINNINGLODCONTROLLER_H
