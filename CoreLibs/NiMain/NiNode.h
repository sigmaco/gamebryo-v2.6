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

#ifndef NINODE_H
#define NINODE_H

#include "NiAVObject.h"
#include "NiDynamicEffect.h"
#include "NiProperty.h"
#include "NiTArray.h"

class NiBitVector;
NiSmartPointer(NiNode);

class NIMAIN_ENTRY NiNode : public NiAVObject
{
    NiDeclareRTTI;
    NiDeclareClone(NiNode);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    // construction and destruction
    NiNode(unsigned int uiNumChildren = 0);
    virtual ~NiNode();

    // children
    inline unsigned int GetArrayCount() const;
    inline unsigned int GetChildCount() const;
    virtual void AttachChild(NiAVObject* pkChild, bool bFirstAvail = false);
    virtual NiAVObjectPtr DetachChild(NiAVObject* pkChild);
    virtual NiAVObjectPtr DetachChildAt(unsigned int i);
    virtual NiAVObjectPtr SetAt(unsigned int i, NiAVObject* pkChild);
    inline NiAVObject* GetAt(unsigned int i) const;
    virtual bool GrowChildArray(unsigned int uiNewSize);

    // dynamic effects
    inline const NiDynamicEffectList& GetEffectList() const;
    void AttachEffect(NiDynamicEffect* pkEffect);
    void DetachEffect(NiDynamicEffect* pkEffect);
    void DetachAllEffects();

    // name search
    virtual NiAVObject* GetObjectByName(const NiFixedString& kName);

    // type search
    virtual void GetObjectsByType(const NiRTTI* pkRTTI, 
        NiTPointerList<NiAVObject*>& kObjects);

    // *** begin Emergent internal use only ***

    // updates
    virtual void UpdateControllers(float fTime);
    virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateUpwardPass();
    virtual void UpdateWorldBound();
    virtual void UpdateRigidDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateNodeBound();

    NiPropertyStatePtr UpdatePropertiesUpward();
    NiDynamicEffectStatePtr UpdateEffectsUpward();
    NiDynamicEffectStatePtr PushLocalEffects(
        NiDynamicEffectState* pkParentState, bool bCopyOnChange);
    virtual void UpdatePropertiesDownward(NiPropertyState* pkParentState);
    virtual void UpdateEffectsDownward(NiDynamicEffectState* pkParentState);

    // culling (separate from display)
    virtual void OnVisible(NiCullingProcess& kCuller);

    // Returns 'true' iff both trees have same parent-child topology and
    // the RTTI of each pair of corresponding nodes are identical.
    static bool SameTopology(NiAVObject* pkObj0, NiAVObject* pkObj1);

    // needed by NiOptimize functions
    inline void RemoveAllChildren();
    inline void CompactChildArray(const bool bRecursive = false);

    // set selective update flags
    virtual void SetSelectiveUpdateFlags(bool& bSelectiveUpdate, 
        bool bSelectiveUpdateTransforms, bool& bRigid);

    // renderer data
    virtual void PurgeRendererData(NiRenderer* pkRenderer);

    virtual void ProcessClone(NiCloningProcess& kCloning);
    // *** end Emergent internal use only ***

protected:
    // cloning
    void CopyEffectListClones(const NiDynamicEffectList* pkList,
        NiCloningProcess& kCloning);

    // scene graph links
    NiTObjectArray<NiAVObjectPtr> m_kChildren;

    // lights
    NiDynamicEffectList m_kEffectList;

    // --- Begin NIF conversion code
    // Deprecated flags - use new flag code
    enum
    {
        MAX_POS = 8
    };
    // --- End NIF conversion code

    // bound of subtree 
    NiBound m_kBound;
};

// note these are stars, not smart pointers
typedef NiTPointerList<NiNode*> NiNodeList;

#include "NiNode.inl"

#endif

