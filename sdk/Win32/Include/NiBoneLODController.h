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

#ifndef NIBONELODCONTROLLER_H
#define NIBONELODCONTROLLER_H

#include <NiTimeController.h>
#include "NiAnimationLibType.h"
#include <NiSkinInstance.h>
#include <NiTArray.h>
#include <NiTSet.h>

class NiNode;
class NiTriBasedGeom;
class NiBoneLODController;

NiSmartPointer(NiBoneLODController);

class NIANIMATION_ENTRY NiBoneLODController : public NiTimeController
{
    NiDeclareRTTI;
    NiDeclareClone(NiBoneLODController);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    // Constructor and destructor.
    NiBoneLODController();
    virtual ~NiBoneLODController();

    virtual void Update(float fTime);

    bool SetBoneLOD(int iLOD);
    inline int GetBoneLOD() const;
    inline unsigned int GetNumberOfBoneLODs() const;

    // Function to remove all references to a particular skin geometry from
    // the controller. Call this function if you are removing the geometry
    // object from the scene graph. There is no way to add it back in.
    void RemoveSkinFromAllLODs(NiTriBasedGeom* pkSkinToRemove);

    // *** begin Emergent internal use only ***
    typedef NiTPrimitiveSet<NiSkinInstance*> NiSkinInstanceSet;
    typedef NiTPrimitiveSet<NiTriBasedGeom*> NiTriBasedGeomSet;

    static bool ProcessScene(NiNode* pkSceneRoot);
    bool InitializeData(NiNode* pkBoneRoot,
        const NiTPointerMap<NiTriBasedGeom*,
        NiUnsignedIntSet*>& kSkinToLOD);
    void GetSkinData(NiTriBasedGeomSet& kGeoms,
        NiSkinInstanceSet& kInstances);
    void ReplaceSkin(NiTriBasedGeom* pkOldSkin, NiTriBasedGeom* pkNewSkin);
    inline bool FindGeom(NiTriBasedGeom* pkGeom);

    // For conversion to NiSkinningLODController
    static void Convert(NiStream& kStream, 
        NiTLargeObjectArray<NiObjectPtr>& kTopObjects);
    static void RecurseScene(NiStream& kStream, NiObject* pkObject);
    static void ConvertBoneLODController(NiStream& kStream,
        NiBoneLODController* pkCont);

    // *** end Emergent internal use only ***

    // This is a global debug setting used to set all LOD Levels 
    static int GetGlobalLOD();
    static void SetGlobalLOD(int iNewLOD);

protected:
    class SkinInfo : public NiMemObject
    {
    public:
        NiTriBasedGeom* m_pkSkinGeom;
        NiSkinInstancePtr m_spSkinInst;
    };

    typedef NiTPrimitiveSet<SkinInfo*> SkinInfoSet;
    typedef NiTPrimitiveSet<NiNode*> NiNodeSet;
    typedef NiTPrimitiveSet<NiSkinData::BoneVertData*> BoneVertDataSet;

    class BoneDataObj : public NiMemObject
    {
    public:
        NiSkinData::BoneData* m_pkBoneData;
        BoneVertDataSet m_kBoneVertData;
    };
    typedef NiTPrimitiveSet<BoneDataObj*> BoneDataObjSet;

    static void FindBoneRootNodes(NiNode* pkNode,
        NiNodeSet& kBoneRootNodes);
    static void FindSkinGeometry(NiAVObject* pkObject,
        NiTPointerMap<NiTriBasedGeom*, NiUnsignedIntSet*>& kSkinToLOD,
        NiNode* pkBoneRoot);
    static void GetGeomChildren(NiNode* pkNode,
        NiTriBasedGeomSet& kGeomChildren);

    virtual bool TargetIsRequiredType() const;

    void CleanUp();

    void CollectMarkedBones(NiNode* pkBone,
        NiTPointerMap<NiNode*, unsigned int>& kBoneToLOD,
        NiTPointerMap<unsigned int, bool>& kUsedLODs);
    void BuildBoneMap(NiNode* pkBone,
        NiTPointerMap<NiNode*, unsigned int>& kBoneToLOD,
        unsigned int& uiCurrentLOD);
    bool FillSkinArray(
        const NiTPointerMap<NiTriBasedGeom*,
        NiUnsignedIntSet*>& kSkinToLOD,
        const NiTPointerMap<NiNode*, unsigned int>& kBoneToLOD,
        NiTPointerMap<unsigned int, bool>& kUsedLODs);
    NiSkinInstance* CreateLODSkinInstance(NiTriBasedGeom* pkOldGeom,
        unsigned int uiDesiredLOD,
        const NiTPointerMap<NiTriBasedGeom*,
        NiUnsignedIntSet*>& kSkinToLOD,
        const NiTPointerMap<NiNode*, unsigned int>& kBoneToLOD);

    // Cloning support.
    virtual void ProcessClone(
        NiCloningProcess& kCloning);

    // Streaming support.
    virtual void PostLinkObject(NiStream& kStream);

    int m_iLOD;
    unsigned int m_uiNumLODs;
    NiTPrimitiveArray<NiNodeSet*> m_kBoneArray;
    NiTPrimitiveArray<SkinInfoSet*> m_kSkinArray;
    NiTriBasedGeomSet m_kSkinSet;

    static int ms_iGlobalLOD;
};

#include "NiBoneLODController.inl"

#endif  // #ifndef NIBONELODCONTROLLER_H
