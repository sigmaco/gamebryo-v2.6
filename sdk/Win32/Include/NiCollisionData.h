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

#ifndef NICOLLISIONDATA_H
#define NICOLLISIONDATA_H

#include <NiCollisionObject.h> 
#include "NiCollisionLibType.h"
#include "NiCollisionGroup.h"
#include <NiPoint3.h>
#include <NiNode.h>
#include <NiTPrimitiveRandomAccessIterator.h>

class NiBoundingVolume;
class NiOBBRoot;
class NiPick;
class NiMesh;

#define NiGetCollisionData(pkNode) \
    ((NiCollisionData*)(pkNode->GetCollisionObject()))

class NICOLLISION_ENTRY NiCollisionData : public NiCollisionObject
{
    NiDeclareRTTI;
    NiDeclareClone(NiCollisionData);
    NiDeclareStream;
    NiDeclareViewerStrings;


public:
    NiCollisionData(NiAVObject* pkSceneObject);
    virtual ~NiCollisionData();

    virtual void SetSceneGraphObject(NiAVObject* pkSceneObject);

    // World geometry access
    // - vertices
    void CreateWorldVertices();
    void UpdateWorldVertices();
    void DestroyWorldVertices();
    inline const NiPoint3* GetWorldVertices() const;

    inline unsigned short GetTriangleCount();
    bool GetWorldTriangle( unsigned short usTriangle, NiPoint3*& pkP0,
        NiPoint3*& pkP1, NiPoint3*& pkP2);

    inline void MarkVerticesAsChanged();

    // - normals
    void CreateWorldNormals();
    void UpdateWorldNormals();
    void DestroyWorldNormals();
    inline const NiPoint3* GetWorldNormals() const;

    inline void MarkNormalsAsChanged();

    // Velocity access
    inline void SetLocalVelocity(const NiPoint3& kLocalVelocity);
    inline const NiPoint3& GetLocalVelocity() const;

    inline void SetWorldVelocity(const NiPoint3& kWorldVelocity);
    inline const NiPoint3& GetWorldVelocity() const;
    
    // Enable and disable world velocity computation
    static void SetEnableVelocity(bool bEnable);
    static bool GetEnableVelocity();
    enum PropagationMode
    {
        // A change here will effect the GetCollisionTestType fn.
        PROPAGATE_ON_SUCCESS,
        PROPAGATE_ON_FAILURE,   // PROPAGATE_ON_FAILURE has been deprecated.
        PROPAGATE_ALWAYS,
        PROPAGATE_NEVER,
        PROPAGATIONMODE_POS
    };

    enum CollisionMode
    {
        // A change here will effect the GetCollisionTestType fn as it uses
        // carefully constructed values to create CollisionTest enumerants
        // from CollisionMode enumerants.
        NOTEST      = 0x00000000,
        USE_OBB     = 0x00000001,
        USE_TRI     = 0x00000002,
        USE_ABV     = 0x00000004,
        USE_NIBOUND = 0x00000008
    };

    enum CollisionTest
    {
        // A change here will effect the GetCollisionTestType fn as it uses
        // carefully constructed values from CollisionMode enumerants to
        // create these CollisionTest enumerants. Note that each CollisionTest
        // value is the OR'd result of the two input CollisionMode values. 
        NOTEST_NOTEST       = 0x00000000,
        OBB_OBB             = 0x00000001,
        TRI_TRI             = 0x00000002,
        OBB_TRI             = 0x00000003,
        ABV_ABV             = 0x00000004,
        OBB_ABV             = 0x00000005,
        TRI_ABV             = 0x00000006,
        NIBOUND_NIBOUND     = 0x00000008,
        OBB_NIBOUND         = 0x00000009,
        TRI_NIBOUND         = 0x0000000A,
        ABV_NIBOUND         = 0x0000000C
    };
    // Propagation control

    inline void SetPropagationMode(PropagationMode ePropagation);
    PropagationMode GetPropagationMode() const;

    inline void SetCollisionMode(CollisionMode eCollision);
    CollisionMode GetCollisionMode() const;
    virtual void Initialize(void *pkData); 

    inline void SetEnableAuxCallbacks(bool bEnable);
    inline bool GetEnableAuxCallbacks();

    // Alternate Bounding Volume (ABV) Access
    //
    // The NiCollisionData object assumes ownership of the input 
    // bounding volume.  The application must therefore pass in 
    // dynamically allocated bounding volumes.  These should not 
    // be deleted by the application.
    void SetModelSpaceABV(NiBoundingVolume* pkABV);

    inline NiBoundingVolume* GetModelSpaceABV() const;
    inline NiBoundingVolume* GetWorldSpaceABV() const;

    // Collision Callback function and data access
    inline void SetCollideCallback(NiCollisionGroup::Callback
        pfnCollideCallback);
    inline NiCollisionGroup::Callback GetCollideCallback();
    inline void SetCollideCallbackData(void* pvCollideCallbackData);
    inline void* GetCollideCallbackData();

    // *** begin Emergent internal use only ***
    virtual void AddViewerStrings(NiViewerStringsArray* pkStrings);

    int FindCollisionProcessing(NiCollisionGroup::Intersect& kIntr,
        PropagationMode ePropagationMode1,PropagationMode ePropagationMode2);
    int TestCollisionProcessing(PropagationMode ePropagationMode1,
        PropagationMode ePropagationMode2);


    virtual void UpdateWorldData();
    virtual void RecreateWorldData();

    void CreateOBB(int iBinSize = 1);
    void DestroyOBB();

    bool TestOBBCollisions(float fDeltaTime, NiCollisionData* pkTestData,
        NiAVObject* pThisObject, NiAVObject* pOtherObj, int iMaxDepth,
        int iTestMaxDepth);

    int FindOBBCollisions(float fDeltaTime, NiCollisionData* pTestData, 
        NiAVObject* pThisRoot, NiAVObject* pOtherRoot, NiAVObject* 
        pThisObj, NiAVObject* pOtherObj, int iMaxDepth, int iTestMaxDepth,
        bool& bCollision);

    static CollisionTest GetCollisionTestType(
        NiAVObject* pkObj1, NiAVObject* pkObj2);

    NiMesh* GetCollisionMesh();

    // *** end Emergent internal use only ***

    static bool ValidateForCollision(NiAVObject* pkObj, CollisionMode eMode);

protected:
    // To prevent public access to a constructor with no scene association
    NiCollisionData();

    NiNode* FindAncestorWithCollisionData(NiAVObject* pkObj);

    void RemapOldCollisionMode();

    char* GetViewerString(const char* pcPrefix, PropagationMode eMode);
    char* GetViewerString(const char* pcPrefix, CollisionMode eMode);

    NiPoint3 m_kLocalVelocity;
    NiPoint3 m_kWorldVelocity;

    PropagationMode m_ePropagationMode;
    CollisionMode m_eCollisionMode;

    NiBoundingVolume* m_pkModelABV;
    NiBoundingVolume* m_pkWorldABV;

    NiOBBRoot* m_pkOBBRoot;

    NiCollisionGroup::Callback m_pfnCollideCallback;
    void* m_pvCollideCallbackData;

    // world geometry data
    NiPoint3* m_pkWorldVertex;
    NiPoint3* m_pkWorldNormal;
    bool m_bWorldVerticesNeedUpdate;
    bool m_bWorldNormalsNeedUpdate;
    unsigned short m_usNumVertices;
    unsigned short m_usNumTriangles;
    bool m_bAuxCallbacks;

    static bool ms_bComputeWorldVelocity;
};

#include "NiCollisionData.inl"

#endif

