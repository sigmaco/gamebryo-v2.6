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

#ifndef NISKININSTANCE_H
#define NISKININSTANCE_H

#include "NiAVObject.h"
#include "NiTransform.h"
#include "NiSkinData.h"
#include "NiSkinPartition.h"

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiSkinningMeshModifier in the NiMesh library.
*/

class NIMAIN_ENTRY NiSkinInstance : public NiObject
{
    NiDeclareRTTI;
    NiDeclareClone(NiSkinInstance);
    NiDeclareStream;
    
public:
    // *** begin Emergent internal use only ***

    // The NiSkinInstance class assumes control of the ppkBones array passed
    // to its constructor. (i.e. Its destructor deletes it.)
    NiSkinInstance(NiSkinData* pkSkinData, NiAVObject* pkRootParent,
        NiAVObject** ppkBones);
    virtual ~NiSkinInstance();

    NiSkinData* GetSkinData() const;
    NiAVObject* GetRootParent() const;
    NiAVObject*const* GetBones() const;
    NiSkinPartition* GetSkinPartition() const;
    void SetBone(unsigned int uiIndex, NiAVObject* pkBone);

    void UpdateModelBound(NiBound& kBound);

    // Besides being used internally, these functions provide an interface
    // which eases the development of custom skinning code.
    void GetWorldToSkinTransform(NiTransform& kWorldToSkin) const;
    void ConcatWorldToSkinTransform(const NiTransform& kSourceMat, 
        NiTransform& kDestMat) const;
    void GetBoneWorldTransform(unsigned int uiBone, 
        NiTransform& kBoneWorld) const;
    void ConcatBoneWorldTransform(unsigned int uiBone, 
        const NiTransform& kSourceMat, NiTransform& kDestMat) const;
    void GetSkinToBoneTransform(unsigned int uiBone, 
        NiTransform& kSkinToBone) const;
    void ConcatSkinToBoneTransform(unsigned int uiBone, 
        const NiTransform& kSourceMat, NiTransform& kDestMat) const;

    NiSkinInstance();
    void SetSkinData(NiSkinData* pkSkinData);
    void SetRootParent(NiAVObject* pkRootParent);
    void SetSkinPartition(NiSkinPartition* pkSkinPartition);

    // streaming support
    virtual void PostLinkObject(NiStream& kStream);

    // *** end Emergent internal use only ***
protected:
    NiSkinDataPtr m_spSkinData;
    NiSkinPartitionPtr m_spSkinPartition;

    // Bone pointers are regular pointers rather than smart pointers to avoid
    // reference cycles that arise when a skin is a descendant of the root 
    // bone.
    NiAVObject* m_pkRootParent;
    NiAVObject** m_ppkBones;
};

typedef NiPointer<NiSkinInstance> NiSkinInstancePtr;

#endif // NISKININSTANCE_H
