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


#include "NiCollisionPCH.h" // Precompiled header

#include <NiMesh.h>
#include <NiDataStreamPrimitiveLock.h>
#include <NiDataStreamElementLock.h>

#include "NiBoundingVolume.h"
#include "NiCollisionData.h"
#include "NiOBBRoot.h"


NiImplementRTTI(NiCollisionData, NiCollisionObject);

// File Scope Helpers
void TransformPoints (NiMesh* pkMesh, float* pWorld);
void TransformVectors(NiMesh* pkMesh, float* pWorld);
NiOBBRoot* ConstructOBBRoot(NiMesh* pkMesh, NiPoint3* m_pkWorldVertex,
    int iBinSize);
void GetMeshTriangleIndices(NiMesh* pkMesh, 
    unsigned int uiTriangle, unsigned int& i0, unsigned int& i1,
    unsigned int& i2);

// Disable world velocity computing by default
bool NiCollisionData::ms_bComputeWorldVelocity = false;

//---------------------------------------------------------------------------
NiCollisionData::NiCollisionData(NiAVObject* pkSceneObject) :
    NiCollisionObject(pkSceneObject)
{
    m_pkWorldVertex = 0;
    m_pkWorldNormal = 0;
    m_bWorldVerticesNeedUpdate = false;
    m_bWorldNormalsNeedUpdate = false;

    m_kLocalVelocity = NiPoint3::ZERO;
    m_kWorldVelocity = NiPoint3::ZERO;

    m_ePropagationMode = PROPAGATE_ALWAYS;
    m_eCollisionMode = NOTEST;

    m_pkModelABV = 0;
    m_pkWorldABV = 0;

    m_pkOBBRoot = 0;

    m_usNumTriangles = 0;
    
    m_pfnCollideCallback = 0;
    m_pvCollideCallbackData = 0;
    m_bAuxCallbacks = false;
}
//---------------------------------------------------------------------------
// This constructor is protected.
NiCollisionData::NiCollisionData() : NiCollisionObject(0)
{
    m_pkWorldVertex = 0;
    m_pkWorldNormal = 0;
    m_bWorldVerticesNeedUpdate = false;
    m_bWorldNormalsNeedUpdate = false;

    m_kLocalVelocity = NiPoint3::ZERO;
    m_kWorldVelocity = NiPoint3::ZERO;

    m_ePropagationMode = PROPAGATE_ALWAYS;
    m_eCollisionMode = NOTEST;

    m_pkModelABV = 0;
    m_pkWorldABV = 0;

    m_pkOBBRoot = 0;

    m_usNumTriangles = 0;
    
    m_pfnCollideCallback = 0;
    m_pvCollideCallbackData = 0;
    m_bAuxCallbacks = false;
}
//---------------------------------------------------------------------------
NiCollisionData::~NiCollisionData()
{    
    NiDelete[] m_pkWorldVertex;
    NiDelete[] m_pkWorldNormal;
    NiDelete m_pkModelABV;
    NiDelete m_pkWorldABV;
    DestroyOBB();
}
//---------------------------------------------------------------------------
void NiCollisionData::SetSceneGraphObject(NiAVObject* pkSceneObject)
{
    NiCollisionObject::SetSceneGraphObject(pkSceneObject);
    NIASSERT(ValidateForCollision(m_pkSceneObject, m_eCollisionMode));
}
//---------------------------------------------------------------------------
NiNode* NiCollisionData::FindAncestorWithCollisionData(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return NULL; 

    NiNode* pkParent = pkObj->GetParent();

    if (pkParent == NULL)
        return NULL;

    NiCollisionData* pkData = NiGetCollisionData(pkParent);

    if (pkData)
        return pkParent;

    return FindAncestorWithCollisionData(pkParent);
}
//---------------------------------------------------------------------------
void NiCollisionData::UpdateWorldData()
{   
    if (m_pkModelABV)
    {
        m_pkWorldABV->UpdateWorldData(*m_pkModelABV, 
            m_pkSceneObject->GetWorldTransform());
    }

    if (ms_bComputeWorldVelocity)
    {
        NiAVObject* pkParent = m_pkSceneObject->GetParent();

        if (pkParent)
        {
            NiNode* pkAncestorWithCD
                = FindAncestorWithCollisionData(m_pkSceneObject);

            NiCollisionData* pkParentData = NULL;

            if (pkAncestorWithCD)
                pkParentData = NiGetCollisionData(pkAncestorWithCD);
          
            if (pkParentData)
            {
                m_kWorldVelocity = pkParent->GetWorldRotate() 
                    * m_kLocalVelocity + pkParentData->GetWorldVelocity();
            }
            else
            {
                m_kWorldVelocity = pkParent->GetWorldRotate() 
                    * m_kLocalVelocity;
            }
        }
        else
        {
            m_kWorldVelocity = m_kLocalVelocity;
        }
    }

    MarkVerticesAsChanged();
    MarkNormalsAsChanged();
}
//---------------------------------------------------------------------------
void NiCollisionData::RecreateWorldData()
{
}
//---------------------------------------------------------------------------
void NiCollisionData::DestroyOBB()
{
    NiDelete m_pkOBBRoot;
    m_pkOBBRoot = 0;
}
//---------------------------------------------------------------------------
bool NiCollisionData::TestOBBCollisions(float fDeltaTime,
    NiCollisionData* pkTestData, NiAVObject* pThisObj, NiAVObject* pOtherObj,
    int iMaxDepth, int iTestMaxDepth)
{
    m_pkOBBRoot->SetDeltaTime(fDeltaTime);
    m_pkOBBRoot->SetVelocity(m_kWorldVelocity);
    m_pkOBBRoot->Transform(m_pkSceneObject->GetWorldTransform());

    pkTestData->m_pkOBBRoot->SetDeltaTime(fDeltaTime);
    pkTestData->m_pkOBBRoot->SetVelocity(pkTestData->m_kWorldVelocity);
    pkTestData->m_pkOBBRoot->Transform(pOtherObj->GetWorldTransform());

    return m_pkOBBRoot->TestCollision(pkTestData->m_pkOBBRoot, pThisObj, 
        pOtherObj, iMaxDepth, iTestMaxDepth);
}
//---------------------------------------------------------------------------
int NiCollisionData::FindOBBCollisions(float fDeltaTime,
    NiCollisionData* pkTestData, NiAVObject* pThisRoot,
    NiAVObject* pOtherRoot, NiAVObject* pThisObj, NiAVObject* pOtherObj,
    int iMaxDepth, int iTestMaxDepth, bool& bCollision)
{
    m_pkOBBRoot->SetDeltaTime(fDeltaTime);
    m_pkOBBRoot->SetVelocity(m_kWorldVelocity);
    m_pkOBBRoot->Transform(m_pkSceneObject->GetWorldTransform());

    pkTestData->m_pkOBBRoot->SetDeltaTime(fDeltaTime);
    pkTestData->m_pkOBBRoot->SetVelocity(pkTestData->m_kWorldVelocity);
    pkTestData->m_pkOBBRoot->Transform(pOtherObj->GetWorldTransform());

    return m_pkOBBRoot->FindCollisions(pkTestData->m_pkOBBRoot, pThisRoot,
        pOtherRoot, pThisObj, pOtherObj, iMaxDepth, iTestMaxDepth,
        bCollision);
}
//---------------------------------------------------------------------------
void NiCollisionData::SetModelSpaceABV(NiBoundingVolume* pkBV)
{
    // Remove any existing model and world ABVs
    NiDelete m_pkModelABV;
    NiDelete m_pkWorldABV;

    // NiCollisionData takes ownership of the input bounding volume
    m_pkModelABV = pkBV;

    if (m_pkModelABV)  
    {
        // Create world bounding volume 
        m_pkWorldABV = m_pkModelABV->Create();

        // m_pkSceneObject may not exist while cloning, so check for it.
        if (m_pkSceneObject)
        {
            // Make it current, since collidees might not get updated again.
            m_pkWorldABV->UpdateWorldData(*m_pkModelABV, 
                m_pkSceneObject->GetWorldTransform());
        }
    }
    else
    {
        m_pkWorldABV = NULL;
    }
}
//----------------------------------------------------------------------------
// This "internal" member function exists primarily to simplify maintenance of
// propagation flags, in that it mirrors the functionality of the
// FindCollisionProcessing() member function which follows it.
//
// Here's the behavior for the propagation flags:
//
//  -  PROPAGATE_NEVER represets the case that a "real" collision is found.
//     No more collision detection work needs to be done for TestCollisions,
//     since it would not change the result, so we want to
//     TERMINATE_COLLISIONS to save unnecessary work for TestCollision.
//     (In the case of FindCollisions, the callback can choose whether to
//     CONTINUE_COLLISIONS (likely) or TERMINATE_COLLISIONS.)
//
//  -  PROPAGATE_ALWAYS represents another case that a "real" collision is
//     found.  Typically, this flag is used on the limbs of a character, where
//     we are actually interested in the collision result at a node as well as
//     wanting to propagate further down the hierarchy.  Thus, we want no
//     culling to be done in this case.  Still, no more work needs to be done
//     for TestCollisions, since one collision is enough to answer whether a
//     collision is found.  So, we TERMINATE_COLLISIONS.
//     (In the case of FindCollisions, the callback can choose whether to
//     CONTINUE_COLLISIONS (likely) or TERMINATE_COLLISIONS.)
//
//  -  PROPAGATE_ON_SUCCESS functions as a collision culling mechanism:  When
//     a collision is found, by default, this propagation flag tells the
//     collision system to "pass on" responsibility for testing collisions
//     further down the hierarchy.  In this case, we must CONTINUE_COLLISIONS
//     until a "real", non-culling collision is found.
//
//  -  (PROPAGATE_ON_FAILURE is typically not used.)
//
// (For auxiliary callbacks (a diagnostic tool), PROPAGATE_ON_SUCCESS is
// treated as a "real collision", so we TERMINATE_COLLISIONS in that case.)

int NiCollisionData::TestCollisionProcessing(
    PropagationMode ePropagationMode1,
    PropagationMode ePropagationMode2)
{
    bool bExecCallback = true;

    if (m_bAuxCallbacks == false)
    {
        if ((ePropagationMode1 == PROPAGATE_ON_SUCCESS ||
             ePropagationMode1 == PROPAGATE_ON_FAILURE) ||
            (ePropagationMode2 == PROPAGATE_ON_SUCCESS ||
             ePropagationMode2 == PROPAGATE_ON_FAILURE))
             bExecCallback = false;
    }

    if (bExecCallback)
    {
        return NiCollisionGroup::TERMINATE_COLLISIONS;
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//----------------------------------------------------------------------------
// This "internal" member function manages propagation flags and conditionally
// calls a FindCollision callback, to handle actual collision processing.
//
// Here's the behavior for the propagation flags:
//
//  -  PROPAGATE_NEVER represets the case that a "real" collision is found.
//     In the case of FindCollisions, the callback can choose whether or not
//     to TERMINATE_COLLISIONS.  However, the user will usually want to
//     CONTINUE_COLLISIONS, in order to return a detected collision if one
//     exists.
//
//  -  PROPAGATE_ALWAYS represents another case that a "real" collision is
//     found.  Typically, this flag is used on the limbs of a character, where
//     we are actually interested in the collision result at a node as well as
//     wanting to propagate further down the hierarchy.  Thus, we want no
//     culling to be done in this case.
//     In the case of FindCollisions, the callback can choose whether or not
//     to TERMINATE_COLLISIONS.  However, the user will usually want to
//     CONTINUE_COLLISIONS, in order to return a detected collision if one
//     exists.
//
//  -  PROPAGATE_ON_SUCCESS functions as a collision culling mechanism:  When
//     a collision is found, by default, this propagation flag tells the
//     collision system to "pass on" responsibility for testing collisions
//     further down the hierarchy.  In this case, we must CONTINUE_COLLISIONS
//     until a "real", non-culling collision is found, and the FindCollision
//     callback is not called.
//
//  -  (PROPAGATE_ON_FAILURE is typically not used.)
//
// (For auxiliary callbacks (a diagnostic tool), PROPAGATE_ON_SUCCESS is
// treated as a "real collision".)

int NiCollisionData::FindCollisionProcessing(
    NiCollisionGroup::Intersect& kIntr, PropagationMode ePropagationMode1,
    PropagationMode ePropagationMode2)
{
    int iRet0 = NiCollisionGroup::CONTINUE_COLLISIONS;
    int iRet1 = NiCollisionGroup::CONTINUE_COLLISIONS;

    bool bExecCallback = true;

    if (m_bAuxCallbacks == false)
    {
        if ((ePropagationMode1 == PROPAGATE_ON_SUCCESS ||
             ePropagationMode1 == PROPAGATE_ON_FAILURE) ||
            (ePropagationMode2 == PROPAGATE_ON_SUCCESS ||
             ePropagationMode2 == PROPAGATE_ON_FAILURE))
             bExecCallback = false;
    }

    if (bExecCallback && kIntr.pkObj0)
    {
        NiCollisionData* pkData = NiGetCollisionData(kIntr.pkObj0);

        if (pkData)
        {
            NiCollisionGroup::Callback pfnCB0 = pkData->GetCollideCallback();

            if (pfnCB0)
                iRet0 = pfnCB0(kIntr);
            else
            {
                // Fall back to root's callback.
                if (kIntr.pkRoot0)
                {
                    NiCollisionData* pkCollisionData = NiGetCollisionData(
                        kIntr.pkRoot0);

                    if (pkCollisionData)
                    {
                        NiCollisionGroup::Callback pfnCallback0 =
                            pkCollisionData->GetCollideCallback();

                        if (pfnCallback0)
                            iRet0 = pfnCallback0(kIntr);
                    }
                }
            }
        }
    }
           
    // Swap so that 0 object is always the calling callback.
    NiCollisionGroup::Intersect kIntrTmp = kIntr;
    kIntr.appkTri1 = kIntrTmp.appkTri2;
    kIntr.appkTri2 = kIntrTmp.appkTri1;
    kIntr.kNormal0 = kIntrTmp.kNormal1;
    kIntr.kNormal1 = kIntrTmp.kNormal0;
    kIntr.pkObj0 = kIntrTmp.pkObj1;
    kIntr.pkObj1 = kIntrTmp.pkObj0;
    kIntr.pkRoot0 = kIntrTmp.pkRoot1;
    kIntr.pkRoot1 = kIntrTmp.pkRoot0;

    if (bExecCallback && kIntr.pkObj0)
    {
        NiCollisionData* pkData = NiGetCollisionData(kIntr.pkObj0);

        if (pkData)
        {
            // Swap so that 0 object is the calling callback.
            NiCollisionGroup::Callback pfnCb1 = pkData->GetCollideCallback();

            if (pfnCb1)
            {
                iRet1 = pfnCb1(kIntr);
            }
            else
            {
                // Fall back to root's call back
                if (kIntr.pkRoot0)
                {
                    NiCollisionData* pkCollisionData
                        = NiGetCollisionData(kIntr.pkRoot0);

                    if (pkCollisionData)
                    {
                        NiCollisionGroup::Callback pfnCB0 
                            = pkCollisionData->GetCollideCallback();

                        if (pfnCB0)
                            iRet1 = pfnCB0(kIntr);
                    }
                }
            }
        }
    }
    // Return to original values.
    kIntr.appkTri1 = kIntrTmp.appkTri1;
    kIntr.appkTri2 = kIntrTmp.appkTri2;
    kIntr.kNormal0 = kIntrTmp.kNormal0;
    kIntr.kNormal1 = kIntrTmp.kNormal1;
    kIntr.pkObj0 = kIntrTmp.pkObj0;
    kIntr.pkObj1 = kIntrTmp.pkObj1;
    kIntr.pkRoot0 = kIntrTmp.pkRoot0;
    kIntr.pkRoot1 = kIntrTmp.pkRoot1;

    if (iRet0 == NiCollisionGroup::CONTINUE_COLLISIONS && 
        iRet1 == NiCollisionGroup::CONTINUE_COLLISIONS)
    {
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }
    else if (iRet0 == NiCollisionGroup::TERMINATE_COLLISIONS ||
        iRet1 == NiCollisionGroup::TERMINATE_COLLISIONS)
    {
        return NiCollisionGroup::TERMINATE_COLLISIONS;
    }
    else
    {
        return NiCollisionGroup::BREAKOUT_COLLISIONS;
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::DestroyWorldVertices()
{
    NiDelete[] m_pkWorldVertex;
    m_pkWorldVertex = 0;
    m_bWorldVerticesNeedUpdate = false;
}
//---------------------------------------------------------------------------
void NiCollisionData::DestroyWorldNormals()
{
    NiDelete[] m_pkWorldNormal;
    m_pkWorldNormal = 0;
    m_bWorldNormalsNeedUpdate = false;
}
//---------------------------------------------------------------------------
void NiCollisionData::Initialize(void *pkData)
{
    // Initialize in context of NiCollisionData is used only to convert
    // older Nif files.

    // Data is interpreted as (this, propagatemode, bool, kstream).
    size_t* puiArray = (size_t*)pkData;
    SetSceneGraphObject((NiAVObject*)puiArray[0]);

    // The m_ePropagateMode member no longer exist as it's functionality has
    // been split into 2 separate members:
    //   PropagationMode m_ePropagationMode;
    //   CollisionMode m_eCollisionMode;
    // for clarity and robustness. 
    // Hence, we store the value of the obsolete m_ePropagateMode member into
    // the new m_ePropagationMode member.  However, this value will need to be
    // "converted" to make since with the new system.  This step is done in
    // the convert function.
    SetPropagationMode((PropagationMode)puiArray[1]); 

    unsigned int uiCreateABVFromStream = (unsigned int) puiArray[2];

    if (uiCreateABVFromStream)
    {
        NiBoundingVolume* pkBV 
            = NiBoundingVolume::CreateFromStream(*(NiStream*)puiArray[3]);

        SetModelSpaceABV(pkBV);
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::RemapOldCollisionMode()
{
    unsigned int uiOldMode = m_eCollisionMode;
    switch(uiOldMode)
    {
    case 0:
        m_eCollisionMode = USE_OBB;
        break;
    case 1:
        m_eCollisionMode = USE_TRI;
        break;
    case 2:
        m_eCollisionMode = USE_ABV;
        break;
    case 3:
        m_eCollisionMode = NOTEST;
        break;
    case 4:
        m_eCollisionMode = USE_NIBOUND;
        break;
    default:
        // We should never hit this assert since it means we've streamed in
        // an old collision mode that didn't exist.
        NIASSERT(!"Unknown legacy collision mode!");
        m_eCollisionMode = NOTEST;
        break;
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiCollisionObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiCollisionData::ms_RTTI.GetName()));

    AddViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
void NiCollisionData::AddViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiCollisionObject::AddViewerStrings(pkStrings);
    
    pkStrings->Add(NiGetViewerString(NiCollisionData::ms_RTTI.GetName()));
    
    // Propagate Mode
    pkStrings->Add(GetViewerString("m_ePropagationMode",
        GetPropagationMode()));
    pkStrings->Add(GetViewerString("m_eCollisionMode", GetCollisionMode()));

    if (m_pkModelABV)
        m_pkModelABV->AddViewerStrings("m_pkModelABV->Type", pkStrings);

    // Local and world velocity
    pkStrings->Add(m_kLocalVelocity.GetViewerString("m_kLocalVelocity"));
    pkStrings->Add(m_kWorldVelocity.GetViewerString("m_kWorldVelocity"));
}
//---------------------------------------------------------------------------
char* NiCollisionData::GetViewerString(const char* pcPrefix,
    CollisionMode eMode)
{
    size_t stLen = strlen(pcPrefix) + 17;
    char* pString = NiAlloc(char, stLen);

    switch (eMode)
    {
    case USE_OBB:
        NiSprintf(pString, stLen, "%s = USE_OBB", pcPrefix);
        break;
    case USE_TRI:
        NiSprintf(pString, stLen, "%s = USE_TRI", pcPrefix);
        break;
    case USE_ABV:
        NiSprintf(pString, stLen, "%s = USE_ABV", pcPrefix);
        break;
    case NOTEST:
        NiSprintf(pString, stLen, "%s = NOTEST", pcPrefix);
        break;
    case USE_NIBOUND:
        NiSprintf(pString, stLen, "%s = USE_NIBOUND", pcPrefix);
        break;
    default:
        NiSprintf(pString, stLen, "%s = UNKNOWN!!!", pcPrefix);
        break;
    }

    return pString;
}
//---------------------------------------------------------------------------
char* NiCollisionData::GetViewerString(const char* pcPrefix,
    PropagationMode eMode)
{
    size_t stLen = strlen(pcPrefix) + 32;
    char* pString = NiAlloc(char, stLen);

    switch (eMode)
    {
    case PROPAGATE_ON_SUCCESS:
        NiSprintf(pString, stLen, "%s = PROPAGATE_ON_SUCCESS", pcPrefix);
        break;
    case PROPAGATE_ON_FAILURE:
        NiSprintf(pString, stLen, "%s = PROPAGATE_ON_FAILURE", pcPrefix);
        break;
    case PROPAGATE_ALWAYS:
        NiSprintf(pString, stLen, "%s = PROPAGATE_ALWAYS", pcPrefix);
        break;
    case PROPAGATE_NEVER:
        NiSprintf(pString, stLen, "%s = PROPAGATE_NEVER", pcPrefix);
        break;
    default:
        NiSprintf(pString, stLen, "%s = UNKNOWN!!!", pcPrefix);
        break;
    }

    return pString;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiCollisionData);
//---------------------------------------------------------------------------
void NiCollisionData::CopyMembers(NiCollisionData* pkDest,
    NiCloningProcess& kCloning)
{
    NiCollisionObject::CopyMembers(pkDest, kCloning);

    // m_pkScreneObject must be filled in by caller.

    pkDest->m_ePropagationMode = m_ePropagationMode;
    pkDest->m_eCollisionMode = m_eCollisionMode;

    if (m_pkModelABV)
        pkDest->SetModelSpaceABV(m_pkModelABV->Clone());

    if (m_pkOBBRoot)
    {
        if (pkDest->m_pkWorldVertex == NULL)
        {
            pkDest->CreateWorldVertices();
            pkDest->UpdateWorldVertices();
        }

        pkDest->CreateOBB();
    }

    // Adding to the NiCollisiongGroup, setting up the callbacks, data, 
    // etc, need to be done by the application, since cloning doesn't handle
    // it.
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiCollisionData);
//---------------------------------------------------------------------------
void NiCollisionData::LoadBinary(NiStream& kStream)
{
    NiCollisionObject::LoadBinary(kStream);

    NiStreamLoadEnum(kStream, m_ePropagationMode);
    NiStreamLoadEnum(kStream, m_eCollisionMode);
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 5))
        RemapOldCollisionMode();

    NiBool bABV;
    NiStreamLoadBinary(kStream,bABV);

    if (bABV)
    {
        NIASSERT(m_pkModelABV == NULL);
        NIASSERT(m_pkWorldABV == NULL);

        m_pkModelABV = NiBoundingVolume::CreateFromStream(kStream);

        // World ABV data can not be updated until streaming is done.
        m_pkWorldABV = m_pkModelABV->Create();
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::LinkObject(NiStream& kStream)
{
    NiCollisionObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiCollisionData::RegisterStreamables(NiStream& kStream)
{
    if (!NiCollisionObject::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiCollisionData::SaveBinary(NiStream& kStream)
{
    NiCollisionObject::SaveBinary(kStream);

    NiStreamSaveEnum(kStream, m_ePropagationMode);
    NiStreamSaveEnum(kStream, m_eCollisionMode);

    if (m_pkModelABV)
    {
        NiStreamSaveBinary(kStream, true);
        m_pkModelABV->SaveBinary(kStream);
    }
    else
    {
        NiStreamSaveBinary(kStream, false);
    }
}
//---------------------------------------------------------------------------
bool NiCollisionData::IsEqual(NiObject* pkObject)
{
    if (!NiCollisionObject::IsEqual(pkObject))
         return false;

    if (NiIsKindOf(NiCollisionData,pkObject) == false)
        return false;

    NiCollisionData* pkCD = (NiCollisionData*)pkObject;

    if (pkCD->GetCollisionMode() != GetCollisionMode())
        return false;

    if (pkCD->GetPropagationMode() != GetPropagationMode())
        return false;

    if (pkCD->GetModelSpaceABV() && GetModelSpaceABV())
    {
        // Both have same type of model ABVs.
        if (pkCD->GetModelSpaceABV()->Type() != GetModelSpaceABV()->Type())
            return false;
    }
    else
    {
        if (pkCD->GetModelSpaceABV() != GetModelSpaceABV())
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Mesh / Geometry releated methods
//---------------------------------------------------------------------------
NiMesh* NiCollisionData::GetCollisionMesh()
{
    NIASSERT(NiIsKindOf(NiMesh, m_pkSceneObject));
    return (NiMesh*)m_pkSceneObject;
}
//---------------------------------------------------------------------------
void NiCollisionData::CreateOBB(int iBinSize)
{
    if (m_pkOBBRoot)
        return;

    if (NiIsKindOf(NiMesh, m_pkSceneObject) == false)
    {
        NiOutputDebugString(
            "Warning:  OBB may be created only for NiMesh geometry.\n  "
            "NiCollisionData::CreateOBB() terminating without creating "
            "OBB...\n");
        return;
    }

    if (m_pkWorldVertex == NULL)
    {
        CreateWorldVertices();
        UpdateWorldVertices();
    }

    NiMesh* pkMesh = (NiMesh*)m_pkSceneObject;

    // NiOBBRoot constructor requires a tri-shape style connectivity list,
    // so a temporary one is created.
    m_pkOBBRoot = ConstructOBBRoot(pkMesh, m_pkWorldVertex, iBinSize);
    NIASSERT(m_pkOBBRoot);
}
//---------------------------------------------------------------------------
void NiCollisionData::CreateWorldVertices()
{
    if (m_pkWorldVertex)
        return;

    if (NiDynamicCast(NiMesh, m_pkSceneObject))
    {
        NiMesh* pkMesh = (NiMesh*)m_pkSceneObject;
        
        if (pkMesh->GetVertexCount())
        {
            m_usNumVertices = (unsigned short) pkMesh->GetVertexCount();
            m_pkWorldVertex = NiNew NiPoint3[m_usNumVertices];
            m_bWorldVerticesNeedUpdate = true;
            
            if (pkMesh->GetPrimitiveType() == 
                NiPrimitiveType::PRIMITIVE_TRIANGLES ||
                pkMesh->GetPrimitiveType() == 
                NiPrimitiveType::PRIMITIVE_TRISTRIPS)
            {
                m_usNumTriangles = 
                    (unsigned short)pkMesh->GetTotalPrimitiveCount();
            }
            else
            {
                m_usNumTriangles = 0;
            }
            NIASSERT(m_usNumTriangles);
            if (m_usNumTriangles == 0)
            {
                NiOutputDebugString(
                    "Warning:  NiCollisionData::CreateWorldVertices() has "
                    "failed since\n  the number of triangles in the "
                    "NiMesh-derived object is 0.\n");
            }
        }
    }
    else
    {
        NiOutputDebugString(
            "Warning:  World vertices may be created only for NiMesh "
            "geometry.\n  NiCollisionData::CreateWorldVertices() terminating "
            "without creating vertices for non-NiMesh-derived geometry...\n");
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::UpdateWorldVertices()
{
    if (m_pkWorldVertex == NULL || m_bWorldVerticesNeedUpdate == false)
        return;

    if (NiDynamicCast(NiMesh, m_pkSceneObject) == false)
    {
        NiOutputDebugString(
            "Warning:  World vertices may be updated only for NiMesh "
            "geometry.\n  NiCollisionData::UpdateWorldVertices() terminating "
            "without updating vertices...\n");
        return;
    }

    TransformPoints((NiMesh*)m_pkSceneObject, 
        (float*)m_pkWorldVertex);

    m_bWorldVerticesNeedUpdate = false;
}
//---------------------------------------------------------------------------
void NiCollisionData::CreateWorldNormals()
{
    if (m_pkWorldNormal)
        return;

    if (NiDynamicCast(NiMesh, m_pkSceneObject) == false)
    {
        NiOutputDebugString(
            "Warning:  World normals may be created only for NiMesh "
            "geometry.\n  NiCollisionData::CreateWorldNormals() terminating "
            "without creating normals...\n");
        return;
    }

    NiMesh* pkMesh = (NiMesh*)m_pkSceneObject;

    if (pkMesh->GetVertexCount())
    {
        NiDataStreamRef* pkRef = NULL;
        NiDataStreamElement kElement;

        // The collision detection system requires 32-bit floats for vertex
        // coordinates, but that's the default data type for export from the
        // art packages.
        pkMesh->FindStreamRefAndElementBySemantic(
            NiCommonSemantics::NORMAL(), 0,
            NiDataStreamElement::F_FLOAT32_3, pkRef, kElement);

        if (pkRef)
        {
            m_pkWorldNormal = NiNew NiPoint3[pkMesh->GetVertexCount()];
            m_bWorldNormalsNeedUpdate = true;
        }
    }
}
//---------------------------------------------------------------------------
void NiCollisionData::UpdateWorldNormals()
{
    if (m_pkWorldNormal == NULL || m_bWorldNormalsNeedUpdate == false)
        return;

    if (NiDynamicCast(NiMesh, m_pkSceneObject) == false)
    {
        NiOutputDebugString(
            "Warning:  World normals may be updated only for NiMesh "
            "geometry.\n  NiCollisionData::UpdateWorldNormals() terminating "
            "without updating normals...\n");
        return;
    }

    NiMesh* pkMesh = (NiMesh*)m_pkSceneObject;

    TransformVectors(pkMesh, (float*)m_pkWorldNormal);

    m_bWorldNormalsNeedUpdate = false;
}
//---------------------------------------------------------------------------
bool NiCollisionData::ValidateForCollision(NiAVObject* pkObj,
    NiCollisionData::CollisionMode eMode)
{
    if (pkObj == NULL)
    {
        // Assume this is null because data is in the process of being set
        // up.
        return true;
    }

    if (eMode != NiCollisionData::USE_OBB && 
        eMode != NiCollisionData::USE_TRI)
    {
        // Any NiAVObject may be used for collision as long as the mode is not
        // specifying using Oriented Bounding Boxes or Triangles.
        return true;
    }

    // Must be a Mesh derived object
    if (!NiIsKindOf(NiMesh, pkObj))
    {
        NiOutputDebugString(
            "Warning:  An NiAVObject has failed "
            "NiCollisionData::ValidateForCollision() since\n  it's not an "
            "NiMesh-derived object, but USE_OBB or USE_TRI has been set.\n");
        return false;
    }

    NiMesh* pkMesh = (NiMesh*)pkObj;

    // Must contain triangles or triangle strips.
    if (pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRIANGLES &&
        pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRISTRIPS)
    {
        NiOutputDebugString(
            "Warning:  An NiAVObject has failed "
            "NiCollisionData::ValidateForCollision() since\n  it's an "
            "NiMesh-derived object, but its primitive type is not triangles "
            "or tristrips.\n");
        return false;
    }

    NiDataStreamRef* pkStreamRef = NULL;
    NiDataStreamElement kElement;

    // The collision detection system requires 32-bit floats for vertex
    // coordinates, but that's the default data type for export from the
    // art packages.
    if (pkMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3,
        pkStreamRef, kElement) == false)
    {
        NiOutputDebugString(
            "Warning:  An NiAVObject has failed "
            "NiCollisionData::ValidateForCollision() since\n  a data stream "
            "with vertex coordinates of type FLOAT32_3 "
            "could not be found.\n");
        return false;
    }

    NIASSERT(pkStreamRef);
    if (pkStreamRef == NULL)
    {
        NiOutputDebugString(
            "Warning:  An NiAVObject has failed "
            "NiCollisionData::ValidateForCollision() since\n  the stream ref "
            "is NULL.\n");
        return false;
    }

    // Needs at least 1 submesh
    NiUInt32 uiSubmeshCount = pkMesh->GetSubmeshCount();
    if (uiSubmeshCount != 1)
    {
        NiOutputDebugString(
            "Warning:  An NiAVObject has failed "
            "NiCollisionData::ValidateForCollision() since\n  its submesh "
            "count is not exactly 1.\n");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiCollisionData::GetWorldTriangle( unsigned short usTriangle, 
    NiPoint3*& pkP0, NiPoint3*& pkP1, NiPoint3*& pkP2)
{
    if (m_pkWorldVertex == NULL ||
        NiDynamicCast(NiMesh, m_pkSceneObject) == NULL)
    {
        pkP0 = 0;
        pkP1 = 0;
        pkP2 = 0;
        return false;
    }

    NiMesh* pkMesh = (NiMesh*)m_pkSceneObject;
    NIASSERT(pkMesh->GetPrimitiveType() == 
        NiPrimitiveType::PRIMITIVE_TRIANGLES ||
        pkMesh->GetPrimitiveType() == 
        NiPrimitiveType::PRIMITIVE_TRISTRIPS);

    NIASSERT(usTriangle < pkMesh->GetTotalPrimitiveCount());

    // Only a single sub-mesh is supported
    NIASSERT(pkMesh->GetSubmeshCount() == 1);

    unsigned int i0, i1, i2;
    GetMeshTriangleIndices(pkMesh, (unsigned int)usTriangle, i0, i1, i2);
    
    if (i0 != i1 && i0 != i2 && i1 != i2)
    {
        pkP0 = &m_pkWorldVertex[i0];
        pkP1 = &m_pkWorldVertex[i1];
        pkP2 = &m_pkWorldVertex[i2];
        return true;
    }

    pkP0 = 0;
    pkP1 = 0;
    pkP2 = 0;

    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// File scope helpers
//---------------------------------------------------------------------------
NiOBBRoot* ConstructOBBRoot(NiMesh* pkMesh, NiPoint3* m_pkWorldVertex,
    int iBinSize)
{
    NIASSERT(m_pkWorldVertex);
    if (m_pkWorldVertex == NULL)
    {
        NiOutputDebugString(
            "Warning:  An NiMesh-derived object has failed "
            "NiCollisionData::ConstructOBBRoot() since\n  it has no world "
            "vertices.\n");
        return NULL;
    }

    if (pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRIANGLES &&
        pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRISTRIPS)
    {
        NiOutputDebugString(
            "Warning:  An NiMesh-derived object has failed "
            "NiCollisionData::ConstructOBBRoot() since\n  it's an "
            "NiMesh-derived object, but its primitive type is not triangles "
            "or tristrips.\n");
        return NULL;
    }

    unsigned int uiTriangles = 0;

    uiTriangles = pkMesh->GetTotalPrimitiveCount();

    if (uiTriangles == 0)
    {
        NiOutputDebugString(
            "Warning:  An NiMesh-derived object has failed "
            "NiCollisionData::ConstructOBBRoot() since\n  it's an "
            "NiMesh-derived object, but its triangle count is 0.\n");
        return NULL;
    }

    unsigned short* pTriList = NiAlloc(unsigned short, 3 * uiTriangles);
    NIASSERT(pTriList != NULL);

    // Only a single submesh and a single vertex region may exist for the
    // collision system.
    NIASSERT(NiCollisionData::ValidateForCollision(pkMesh,
        NiCollisionData::USE_OBB));

    // Iterate over all the triangles...
    NiDataStreamPrimitiveLock kTriLock(pkMesh, NiDataStream::LOCK_READ);

    unsigned int uiUsedTriangles = 0;
    NIASSERT(kTriLock.IndexBufferExists());
    if (kTriLock.Has32BitIndexBuffer())
    {
        NiIndexedPrimitiveIterator32 kBegin = kTriLock.BeginIndexed32();
        NiIndexedPrimitiveIterator32 kEnd = kTriLock.EndIndexed32();
        NiIndexedPrimitiveIterator32 kIter = kBegin;

        int j=0;
        while(kIter != kEnd)
        {
            NIASSERT((*kIter).count() == 3);    // Tri-based

            NiUInt32 ui0 = (*kIter)[0];
            NiUInt32 ui1 = (*kIter)[1];
            NiUInt32 ui2 = (*kIter)[2];

            // Check for degenerates.
            if (ui0 == ui1 || ui0 == ui2 || ui1 == ui2)
            {
                ++kIter;
                continue;
            }

            // Possible lost of data as OBB only supports 16-bit.
            pTriList[j] = (unsigned short) ui0;
            pTriList[j+1] = (unsigned short) ui1;
            pTriList[j+2] = (unsigned short) ui2;

            j += 3;
            uiUsedTriangles++;

            ++kIter;
        }
    }
    else if (kTriLock.Has16BitIndexBuffer())
    {
        NiIndexedPrimitiveIterator16 kBegin = kTriLock.BeginIndexed16();
        NiIndexedPrimitiveIterator16 kEnd = kTriLock.EndIndexed16();
        NiIndexedPrimitiveIterator16 kIter = kBegin;

        int j=0;
        while(kIter != kEnd)
        {
            NIASSERT((*kIter).count() == 3);    // Tri-based

            NiUInt16 ui0 = (*kIter)[0];
            NiUInt16 ui1 = (*kIter)[1];
            NiUInt16 ui2 = (*kIter)[2];

            // Check for degenerates.
            if (ui0 == ui1 || ui0 == ui2 || ui1 == ui2)
            {
                ++kIter;
                continue;
            }

            pTriList[j] = ui0;
            pTriList[j+1] = ui1;
            pTriList[j+2] = ui2;

            j += 3;
            uiUsedTriangles++;
            ++kIter;
        }
    }

    // Get the vertices...
    // The collision detection system requires 32-bit floats for vertex
    // coordinates, but that's the default data type for export from the
    // art packages.
    NiDataStreamElementLock kLockPosition(pkMesh,
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_READ);

    // Note that each mesh-based object should consist of exactly one sub-mesh
    // and must have exactly one vertex region.
    NiTStridedRandomAccessIterator<NiPoint3> kPosBegin =
        kLockPosition.begin<NiPoint3>();
    NiTStridedRandomAccessIterator<NiPoint3> kPosEnd =
        kLockPosition.end<NiPoint3>();
    NiPoint3* pVerts = NiAlloc(NiPoint3, pkMesh->GetVertexCount());

    NIASSERT(pVerts != NULL);

    int j=0;
    for (NiTStridedRandomAccessIterator<NiPoint3> kPosIter = kPosBegin; 
        kPosIter!= kPosEnd; ++kPosIter)
    {
        NiPoint3 kVertex = *kPosIter;
        pVerts[j++] = *kPosIter;
    }

    // A single vertex region is assumed, so trilist indices will point to
    // the correct location in the vertex region.
    NiOBBRoot* pkOBBRoot = NiNew NiOBBRoot((unsigned short)uiUsedTriangles,
        pTriList, pVerts, m_pkWorldVertex, iBinSize);

    NiFree(pTriList);
    NiFree(pVerts);
    return pkOBBRoot;
}
//---------------------------------------------------------------------------
void TransformPoints(NiMesh* pkMesh, float* pWorld)
{
    // The collision detection system requires 32-bit floats for vertex
    // coordinates, but that's the default data type for export from the
    // art packages.
    NiDataStreamElementLock kLockPosition(pkMesh,
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_READ);

    // Note that each mesh-based object should consist of exactly one sub-mesh
    // and must have exactly one vertex region.
    NiTStridedRandomAccessIterator<NiPoint3> kBegin =
        kLockPosition.begin<NiPoint3>();
    NiTStridedRandomAccessIterator<NiPoint3> kEnd =
        kLockPosition.end<NiPoint3>();

    const NiTransform kXForm = pkMesh->GetWorldTransform();

    // WARNING:  This typecast is valid because NiMatrix3 stores the matrix in
    // row-major order as float[3][3].  Any change to the data representation
    // in NiMatrix3 could invalidate this code.
    const NiMatrix3* pMatrix = &(kXForm.m_Rotate);

    for (NiTStridedRandomAccessIterator<NiPoint3> kIter = kBegin; 
        kIter!= kEnd; ++kIter, pWorld+=3)
    {
        NiPoint3 kVertex = *kIter;

        *(pWorld+0) = (
            pMatrix->GetEntry(0,0) * (kVertex.x) +
            pMatrix->GetEntry(0,1) * (kVertex.y) +
            pMatrix->GetEntry(0,2) * (kVertex.z)) * kXForm.m_fScale + 
            kXForm.m_Translate.x;
        *(pWorld+1) = (
            pMatrix->GetEntry(1,0) * (kVertex.x) +
            pMatrix->GetEntry(1,1) * (kVertex.y) +
            pMatrix->GetEntry(1,2) * (kVertex.z)) * kXForm.m_fScale + 
            kXForm.m_Translate.y;
        *(pWorld+2) = (
            pMatrix->GetEntry(2,0) * (kVertex.x) +
            pMatrix->GetEntry(2,1) * (kVertex.y) +
            pMatrix->GetEntry(2,2) * (kVertex.z)) * kXForm.m_fScale + 
            kXForm.m_Translate.z;
    }
}
//---------------------------------------------------------------------------
void TransformVectors(NiMesh* pkMesh, float* pWorld)
{
    // The collision detection system requires 32-bit floats for vertex
    // normals, but that's the default data type for export from the
    // art packages.
    NiDataStreamElementLock kLock(pkMesh, NiCommonSemantics::NORMAL(), 0,
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_READ);

    // Note that each mesh-based object should consist of exactly one
    // sub-mesh.
    NiTStridedRandomAccessIterator<NiPoint3> kBegin =
        kLock.begin<NiPoint3>();
    NiTStridedRandomAccessIterator<NiPoint3> kEnd =
        kLock.end<NiPoint3>();

    NiMatrix3* pMatrix = (NiMatrix3*)&(pkMesh->GetWorldRotate());

    for (NiTStridedRandomAccessIterator<NiPoint3> kIter = kBegin; 
        kIter!= kEnd; ++kIter, pWorld+=3)
    {
        NiPoint3 kVector = *kIter;

        *(pWorld+0) = (
            pMatrix->GetEntry(0,0) * (kVector.x) +
            pMatrix->GetEntry(0,1) * (kVector.y) +
            pMatrix->GetEntry(0,2) * (kVector.z));
        *(pWorld+1) = (
            pMatrix->GetEntry(1,0) * (kVector.x) +
            pMatrix->GetEntry(1,1) * (kVector.y) +
            pMatrix->GetEntry(1,2) * (kVector.z));
        *(pWorld+2) = (
            pMatrix->GetEntry(2,0) * (kVector.x) +
            pMatrix->GetEntry(2,1) * (kVector.y) +
            pMatrix->GetEntry(2,2) * (kVector.z));
    }
}
//---------------------------------------------------------------------------
void GetMeshTriangleIndices(NiMesh* pkMesh, 
    unsigned int uiTriangle, unsigned int& i0, unsigned int& i1,
    unsigned int& i2)
{
    NiDataStreamPrimitiveLock kTriLock(pkMesh, NiDataStream::LOCK_READ);

    NiDataStreamRef* pkRef;
    NiDataStreamElement kElem;
    if (pkMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UINT16_1, pkRef, kElem) == true)
    {
        NiIndexedPrimitiveIterator16 kTriIter = kTriLock.BeginIndexed16();
        kTriIter += uiTriangle;

        // Must be tri-based...
        NIASSERT((*kTriIter).count() == 3); 
        i0 = (*kTriIter)[0];
        i1 = (*kTriIter)[1];
        i2 = (*kTriIter)[2];
    }
    else
    {
        NiIndexedPrimitiveIterator32 kTriIter = kTriLock.BeginIndexed32();
        kTriIter += uiTriangle;

        // Must be tri-based...
        NIASSERT((*kTriIter).count() == 3); 
        i0 = (*kTriIter)[0];
        i1 = (*kTriIter)[1];
        i2 = (*kTriIter)[2];
    }

    return;
}
//---------------------------------------------------------------------------
