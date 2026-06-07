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


#ifndef NIPHYSXSCENEDESC_H
#define NIPHYSXSCENEDESC_H

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXActorDesc.h"
#include "NiPhysXBodyDesc.h"
#include "NiPhysXD6JointDesc.h"
#include "NiPhysXMaterialDesc.h"
#include "NiPhysXShapeDesc.h"

//#include "NiPhysXScene.h"
class NiPhysxScene;

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

/**
    NiPhysXSceneDesc objects are for caching the properties of NxScene objects.
    
    They are the objects streamed to save or load a NxScene and are the
    objects stored as snapshots in NiPhysXScene. Instances of this class are
    typically created in art tools, or modified after loading from a stream
    but before instantiating the PhysX.
    
    Cloning is supported, allowing the settings for a scene to be copied for
    creating another.
    
    @note At one time these were the objects that held all PhysX content, but
        but they have subsequently been replaced by NiPhysXProp.
        Functionality to handle back compatibility for old NIF files is still
        included in this class.
*/
class NIPHYSX_ENTRY NiPhysXSceneDesc : public NiObject
{

    /// @cond EMERGENT_INTERNAL
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXSceneDesc);
    /// @endcond

public:
    /// Constructor.
    ///
    /// Sets the internal properties to match the PhysX NxScene defaults. 
    NiPhysXSceneDesc();
    
    /// Destructor.
    virtual ~NiPhysXSceneDesc();

    /// Returns a reference to the internal NxSceneDesc object.
    virtual NxSceneDesc& GetNxSceneDesc();

    /// Copies the contents of its argument into the internal descriptor.
    virtual void SetNxSceneDesc(const NxSceneDesc& kSceneDesc);
    
    /**
        Get a group collision flag, with the same behavior as
        NxScene::getGroupCollisionFlag().
        
        The flags are stored separately in this class because they do not
        appear in a NxSceneDesc. This function returns the local copy.
    */
    bool GetGroupCollisionFlag(const unsigned char ucGroup1,
        const unsigned char ucGroup2) const;
        
    /**
        Set a group collision flag, with the same behavior as
        NxScene::setGroupCollisionFlag().
        
        The flags are stored separately in this class because they do not
        appear in a NxSceneDesc. This function should only be used before
        the PhysX scene is created, because the call is not passed through
        to the NxScene.
    */
    void SetGroupCollisionFlag(const unsigned char ucGroup1,
        const unsigned char ucGroup2, const bool bVal);

    /**
        Get the collision filtering operations, with the same behavior as
        NxScene::getFilterOps.
        
        The operators are stored separately in this class because they do not
        appear in a NxSceneDesc. This function returns the local copy.
    */
    void GetFilterOps(
        NxFilterOp& eOp0, NxFilterOp& eOp1, NxFilterOp& eOp2) const; 

    /**
        Set the collision filtering operations, with the same behavior as
        NxScene::setFilterOps.
        
        The operators are stored separately in this class because they do not
        appear in a NxSceneDesc. This function should only be used before
        the PhysX scene is created, because the call is not passed through
        to the NxScene.
    */
    void SetFilterOps(NxFilterOp eOp0, NxFilterOp eOp1, NxFilterOp eOp2); 

    /**
        Get the collision filtering boolean, with the same behavior as
        NxScene::getFilterBool.
        
        The filter boolean is stored separately in this class because it does
        not appear in a NxSceneDesc. This function returns the local copy.
    */
    bool GetFilterBool() const;

    /**
        Set the collision filtering boolean, with the same behavior as
        NxScene::setFilterBool.
        
        The filter boolean is stored separately in this class because it does
        not appear in a NxSceneDesc. This function should only be used before
        the PhysX scene is created, because the call is not passed through
        to the NxScene.
    */
    void SetFilterBool(bool bFlag);

    /**
        Get the first collision filtering constant, with the same behavior as
        NxScene::getFilterConstant0.
        
        The filter constants are stored separately in this class because they
        do not appear in a NxSceneDesc. This function returns the local copy.
    */
    void GetFilterConstant0(NxGroupsMask &kMask) const;

    /**
        Set the first collision filtering constant, with the same behavior as
        NxScene::setFilterConstant0.
        
        The filter constants are stored separately in this class because they
        do not appear in a NxSceneDesc. This function should only be used
        before the PhysX scene is created, because the call is not passed
        through to the NxScene.
    */
    void SetFilterConstant0(const NxGroupsMask &kMask);

    /**
        Get the second collision filtering constant, with the same behavior as
        NxScene::getFilterConstant1.
        
        The filter constants are stored separately in this class because they
        do not appear in a NxSceneDesc. This function returns the local copy.
    */
    void GetFilterConstant1(NxGroupsMask &kMask) const;

    /**
        Set the second collision filtering constant, with the same behavior as
        NxScene::setFilterConstant1.
        
        The filter constants are stored separately in this class because they
        do not appear in a NxSceneDesc. This function should only be used
        before the PhysX scene is created, because the call is not passed
        through to the NxScene.
    */
    void SetFilterConstant1(const NxGroupsMask &kMask);
    
    /**
        Access to compartments.
        
        This function returns a reference to the map used to store information
        about compartments in this scene. The values in the map should be
        allocated using NiAlloc or NiMalloc, and the memory is subsequently
        owned by the NiPhysXSceneDesc object. You must assign the return value
        to a reference variable, or you will get a compile error about a
        non-existent constructor.
        
        The keys from this map are used in NiPhysXPropDesc to indicate
        the appropriate compartment for various types of object.
    */
    NiTMap<NiUInt32, NxCompartmentDesc*>& GetCompartmentMap();

    /**
        Create a NxScene object using the stored parameters.

        Presumes that the PhysX SDK has been initialized through
        NiPhysXManager::Initialize.
    */
    virtual NxScene* CreateScene();
        
    /// @cond EMERGENT_INTERNAL
    
    // Non-default streaming functions
    virtual bool StreamCanSkip();

    //
    // These functions for back-compatable streaming ONLY.
    //
    // Access to the actor list
    NiUInt32 GetActorCountBackCompatable() const;
    NiPhysXActorDescPtr GetActorAtBackCompatable(NiUInt32 uiIndex);

    // Access to the joint list
    NiUInt32 GetJointCountBackCompatable() const;
    NiPhysXJointDescPtr GetJointAtBackCompatable(NiUInt32 uiIndex);

    // Access to the material map
    NiTMapIterator GetFirstMaterialBackCompatable();
    void GetNextMaterialBackCompatable(NiTMapIterator& kPos,
        NxMaterialIndex& usIndex, NiPhysXMaterialDescPtr& spMaterial);

    NiUInt32 GetNumStatesBackCompatable() const;

    void ClearBackCompatable();

    /// @endcond
            
protected:
    //
    // For streaming of deprecated PhysX member variables.
    //
    enum NiSceneDescNxBroadPhaseType
    {
        NX_BROADPHASE_QUADRATIC,
        NX_BROADPHASE_FULL,
        NX_BROADPHASE_COHERENT,

        NX_BROADPHASE_FORCE_DWORD = 0x7fffffff
    };
    enum NiSceneDescNxHwPipelineSpec
    {
        NX_HW_RB_PIPELINE_HLP_ONLY  = 0,
        NX_HW_PIPELINE_FULL         = 1,
        NX_HW_PIPELINE_DEBUG        = 2,

        NX_PSP_FORCE_DWORD = 0x7fffffff
    };
    enum NiSceneDescNxHwSceneType
    {
        NX_HW_SCENE_TYPE_RB             = 0,
        NX_HW_SCENE_TYPE_FLUID          = 1,
        NX_HW_SCENE_TYPE_FLUID_SOFTWARE = 2,
        NX_HW_SCENE_TYPE_CLOTH          = 3,

        NX_SCT_FORCE_DWORD = 0x7fffffff
    };

    // Most data is in a PhysX scene descriptor
    NxSceneDesc m_kNxSceneDesc;
    
    // These things are queryable from a scene, but not in a descriptor.
    bool m_abGroupCollisionFlags[32][32];
    NxFilterOp m_aeFilterOps[3];
    bool m_bFilterBool;
    NxGroupsMask m_auiFilterConstants[2];
    
    // These things are because we need to stream them ourselves.
    NiTMap<NiUInt32, NxCompartmentDesc*> m_kCompartmentMap;

    // Components of a scene, from an old incarnation. These things are now
    // stored in NiPhysXPropDesc objects, and should not be here. We keep
    // them only to support streaming of prior versions. Remove when we
    // drop support for streaming versions prior to Gamebryo 2.3.
    NiTObjectArray<NiPhysXActorDescPtr> m_kActorsBackCompatable;
    NiTObjectArray<NiPhysXJointDescPtr> m_kJointsBackCompatable;
    NiTMap<NxMaterialIndex, NiPhysXMaterialDescPtr> m_kMaterialsBackCompatable;
    NiUInt32 m_uiNumStatesBackCompatable;
};

#include "NiPhysXSceneDesc.inl"

#endif // NIPHYSXSCENEDESC_H
