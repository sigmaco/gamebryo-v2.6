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

#ifndef NIAVOBJECT_H
#define NIAVOBJECT_H

#include "NiBound.h"
#include "NiObjectNET.h"
#include "NiPlane.h"
#include "NiPoint3.h"
#include "NiProperty.h"
#include "NiFlags.h"
#include "NiQuaternion.h"
#include "NiTransform.h"
#include "NiCollisionObject.h"
#include "NiUpdateProcess.h"

class NiBitVector;
class NiCullingProcess;
class NiNode;
class NiPick;
class NiRenderer;

NiSmartPointer(NiAVObject);
NiSmartPointer(NiPropertyState);
NiSmartPointer(NiDynamicEffectState);

class NIMAIN_ENTRY NiAVObject : public NiObjectNET
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiAVObject);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;
public:
    // destruction(abstract base class, constructor is protected)
    virtual ~NiAVObject();

    // parent access
    inline NiNode* GetParent();
    inline const NiNode* GetParent() const;

    // updates
    void Update(NiUpdateProcess& kUpdateProcess);
    void UpdateSelected(NiUpdateProcess& kUpdateProcess);
    virtual void UpdateControllers(float fTime);
    inline void UpdateObjectControllers(float fTime, bool bProperties = true);
    void UpdateProperties();
    void UpdateEffects();
    virtual void UpdateNodeBound();

    // The following forms of update are deprecated and calls to them should
    // be replaced with ones that take an NiUpdateProcess
    void Update(float fTime, bool bUpdateControllers = true);
    void UpdateSelected(float fTime);

    // transforms
    inline void SetTranslate(const NiPoint3& kTrn);
    inline void SetTranslate(float x, float y, float z);
    inline const NiPoint3& GetTranslate() const;
    inline void SetRotate(const NiMatrix3& kRot);
    inline const NiMatrix3& GetRotate() const;
    inline void SetRotate(float fAngle, float x, float y, float z);
    inline void GetRotate(float& fAngle, float& x, float& y, float& z) const;
    inline void SetRotate(const NiQuaternion& kQuat);
    inline void GetRotate(NiQuaternion& kQuat) const;
    inline void SetRotateAboutCenter(const NiMatrix3& kRot,
        const NiPoint3& kCen,
        const NiPoint3& kTrans = NiPoint3::ZERO);
    inline void SetScale(float fScale);
    inline float GetScale() const;
    inline const NiPoint3& GetWorldTranslate() const;
    inline const NiMatrix3& GetWorldRotate() const;
    inline float GetWorldScale() const;

    // Set local transform of object
    inline void SetLocalTransform(const NiTransform& kTransform);

    // Sets the local transform such that it's world position will match
    // the provided world transform. 
    // NOTE: Parent must have been updated to have correct world location.
    // NOTE: This object must have update called to reflect this transform.
    inline void SetLocalFromWorldTransform(
        const NiTransform& kWorldTransform);


    // bounds
    inline const NiBound& GetWorldBound() const;

    // properties
    inline NiPropertyList& GetPropertyList();
    inline const NiPropertyList& GetPropertyList() const;
    inline void AttachProperty(NiProperty* pkProperty);
    inline void DetachProperty(NiProperty* pkProperty);
    inline void DetachAllProperties();
    NiProperty* GetProperty(int iType) const;
    NiPropertyPtr RemoveProperty(int iType);

    // application-specified culling
    inline void SetAppCulled(bool bAppCulled);
    inline bool GetAppCulled() const;
    // name search
    virtual NiAVObject* GetObjectByName(const NiFixedString& kName);
    // type search
    virtual void GetObjectsByType(const NiRTTI* pkRTTI, 
        NiTPointerList<NiAVObject*>& kObjects);

    // application-specified selective update
    inline bool GetSelectiveUpdate() const;
    inline void SetSelectiveUpdate(bool bUpdate);
    inline bool GetSelectiveUpdateTransforms() const;
    inline void SetSelectiveUpdateTransforms(bool bUpdate);
    inline bool GetSelectiveUpdatePropertyControllers() const;
    inline void SetSelectiveUpdatePropertyControllers(bool bUpdate);
    inline bool GetSelectiveUpdateRigid() const;
    inline void SetSelectiveUpdateRigid(bool bRigid);
    inline bool GetSelectiveUpdateTransformsOverride() const;
    inline void SetSelectiveUpdateTransformsOverride(bool bOverride);
    virtual void SetSelectiveUpdateFlags(bool& bSelectiveUpdate, 
        bool bSelectiveUpdateTransforms, bool& bRigid);

    // for occlusion culling
    inline void SetDisplayObject(bool bDisplay);
    inline bool GetDisplayObject() const;

    inline void SetCollisionObject(NiCollisionObject* pkCollisionObject);
    inline NiCollisionObject* GetCollisionObject() const;

    inline bool IsNode();
    inline bool IsLeaf();

    // *** begin Emergent internal use only ***

    // Updates intended only for use with 3rd party physics engine.
    inline void SetWorldTranslate(const NiPoint3& kTrn);
    inline void SetWorldRotate(const NiMatrix3& kRot);
    inline void SetWorldScale(float fScale);
    inline void SetWorldBound(const NiBound& kBound);

    // updates
    virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateRigidDownwardPass(NiUpdateProcess& kUpdate);
    inline bool IsVisualObject() const;

    virtual void UpdatePropertiesDownward(NiPropertyState* pkParentState);
    virtual void UpdateEffectsDownward(NiDynamicEffectState* pkParentState);

    virtual void UpdateWorldData();
    virtual void UpdateWorldBound();

    // property updates
    NiPropertyStatePtr PushLocalProperties(NiPropertyState* pkParentState,
        bool bCopyOnChange);

    // parent manipulation(applications should never explicity call these)
    void AttachParent(NiNode* pkParent);
    inline void DetachParent();

    // culling pass (separate from display pass)
    void Cull(NiCullingProcess& kCuller);
    virtual void OnVisible(NiCullingProcess& kCuller);

    // application-specified selective update
    inline void DoSelectedUpdate(NiUpdateProcess& kUpdate);

    // cloning
    virtual void ProcessClone(NiCloningProcess& kCloning);

    // transforms
    inline const NiTransform& GetWorldTransform() const;
    inline const NiTransform& GetLocalTransform() const;

    // renderer data
    virtual void PurgeRendererData(NiRenderer* pkRenderer);

    // Clones this object into a new NiAVObject 
    NiAVObject* CloneInPlace(NiAVObject* pkDest, 
        NiCloningProcess& kCloning);

    static void RegisterDefaultUpdateProcess(
        NiUpdateProcess* pkUpdateProcess);

    // *** end Emergent internal use only ***

protected:
    // construction(abstract base class)
    NiAVObject();

    // Declared here as there is already a flag vector available.  However,
    // only subclasses actually make access to this public
    inline void SetSortObject(bool bSort);
    inline bool GetSortObject() const;

    inline void SetNodeBit();

    // Declare flags early to avoid cache misses when flags are checked
    // shortly after a virtual function call. The flags will share a cache
    // line with the vtable. Note that NiDeclareFlags will set the access mode
    // to private so we must redeclare protected access.
    NiDeclareFlags(unsigned short);
protected:

    // parent pointer
    NiNode* m_pkParent;

    // --- Begin NIF conversion code
    // Deprecated flags - use new flag code
    enum
    {
        MAX_POS = 6
    };
    // --- End NIF conversion code

    // flags
    enum
    {
        APP_CULLED_MASK                 = 0x0001,

        // Selective update flags
        SELECTIVE_UPDATE_MASK           = 0x0002,
        SELECTIVE_XFORMS_MASK           = 0x0004,
        SELECTIVE_PROP_CONTROLLER_MASK  = 0x0008,
        SELECTIVE_RIGID_MASK            = 0x0010,

        // For use with occlusion culling system
        DISPLAY_OBJECT_MASK             = 0x0020,

        // For use with sorting/accumulation system
        DISABLE_SORTING                 = 0x0040,
        
        // Selective update over-ride flags
        SELECTIVE_XFORMS_OVERRIDE_MASK  = 0x0080,

        // Is the object a leaf or a node.
        IS_NODE                         = 0x0100
    };

    // bound
    NiBound m_kWorldBound;

    // transforms
    // NOTE: Variable declarations whose order effects assembly language code 
    // begin here. Do not change.
    void CopyTransforms(const NiAVObject* pkSrc);
    NiTransform m_kLocal;
    NiTransform m_kWorld;
    // NOTE: Variable declarations whose order effects assembly language code
    // end here.
    // properties
    NiPropertyList m_kPropertyList;

    // cloning
    void CloneProperties(
        NiAVObject* pkDest, 
        NiPropertyList* pkList,
        NiCloningProcess& kCloning);
    void ProcessPropertyClones(
        NiPropertyList* pkList,
        NiCloningProcess& kCloning);

    // set selective update flags
    bool HasTransformController() const;
    bool HasPropertyController() const;

    NiCollisionObjectPtr m_spCollisionObject;

    // Default update process
    static NiUpdateProcessPtr ms_spDefaultUpdateProcess;
private:
    // Deprecated functions, kept around so that derived classes will not
    // compile if they attempt to call these functions.
    virtual void UpdateDownwardPass(float fTime, bool bUpdateControllers);
    virtual void UpdateSelectedDownwardPass(float fTime);
    virtual void UpdateRigidDownwardPass(float fTime);
};

#include "NiAVObject.inl"

#endif

