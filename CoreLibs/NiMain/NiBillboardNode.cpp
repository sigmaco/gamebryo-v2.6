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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiBillboardNode.h"
#include "NiCamera.h"
#include "NiCullingProcess.h"
#include "NiMainMetrics.h"

#include <NiTNodeTraversal.h>

NiImplementRTTI(NiBillboardNode, NiNode);

//---------------------------------------------------------------------------
bool NiBillboardNode::RotateToCenter(const NiCamera* pkCamera, 
    const NiPoint3& kWorldTranslate, NiPoint3& kCamD, NiPoint3& kCamU, 
    NiPoint3& kCamR)
{
    // set kCamD, kCamU, and kCamR to look directly at kWorldTranslate

    NiPoint3 kAt = pkCamera->GetWorldLocation() - kWorldTranslate;
    if (kAt.SqrLength() < 0.001f)
        return false;

    kAt.Unitize();
    kCamD = -pkCamera->GetWorldDirection();
    kCamU = pkCamera->GetWorldUpVector();
    kCamR = pkCamera->GetWorldRightVector();
    float fDot = kAt.Dot(kCamD);
    if (fDot < 0.999999f) 
    {
        float fAngle = NiACos(fDot);
        NiPoint3 kAxis = kAt.UnitCross(kCamD);
        NiMatrix3 kRot;
        kRot.MakeRotation(fAngle, kAxis);
        kCamD = kAt; // equivalent to kRot * kCamD;
        kCamU = kRot * kCamU;
        kCamR = kRot * kCamR;
    }
    return true;
}
//-------------------------------------------------------------------------
class RecursivePushTransformsDownward
{
public:
    inline void operator() (NiAVObject* pkObject)
    {
        pkObject->UpdateWorldData();
    }
};
//---------------------------------------------------------------------------
void NiBillboardNode::RotateToCamera(const NiCamera* pkCamera)
{
    // get parent world transforms
    NiNode* pkParent = GetParent();
    NiTransform kParentXform;

    if (pkParent)
    {
        kParentXform = pkParent->GetWorldTransform();
    }
    else
    {
        kParentXform.MakeIdentity();
    }

    // compute billboard's world transforms
    m_kWorld = kParentXform * m_kLocal;

    // additional orienting of billboard based on selected mode
    NiMatrix3 kFaceMat = NiMatrix3::IDENTITY;
    FaceMode eFaceMode = GetMode();
    
    switch (eFaceMode)
    {
        case ALWAYS_FACE_CENTER:
        case ALWAYS_FACE_CAMERA:
        {
            NiPoint3 kCamD, kCamU, kCamR;

            if (eFaceMode == ALWAYS_FACE_CENTER)
            {
                if(!RotateToCenter(pkCamera, m_kWorld.m_Translate, kCamD, 
                    kCamU, kCamR))
                {
                    break;
                }
            }
            else
            {
                // billboard coordinates for world axes of camera
                kCamD = -pkCamera->GetWorldDirection(); 
                kCamU = pkCamera->GetWorldUpVector();
                kCamR = pkCamera->GetWorldRightVector();
            }
            kCamD = kCamD * m_kWorld.m_Rotate;
            kCamU = kCamU * m_kWorld.m_Rotate;
            kCamR = kCamR * m_kWorld.m_Rotate;

            // Rotated model up vector is that vector in the plane
            // orthogonal to the camera direction which minimizes the angle
            // between it and the original model up (0,1,0).
            float fRoot = NiSqrt(kCamR.y*kCamR.y+kCamU.y*kCamU.y);
            if (fRoot > 1e-06f)
            {
                float fInvRoot = 1.0f/fRoot;
                float fCos = kCamU.y*fInvRoot;
                float fSin = -kCamR.y*fInvRoot;
                kFaceMat.SetCol(0, 
                    fCos*kCamR.x+fSin*kCamU.x,
                    fCos*kCamR.y+fSin*kCamU.y,
                    fCos*kCamR.z+fSin*kCamU.z);
                kFaceMat.SetCol(1, 
                    -fSin*kCamR.x+fCos*kCamU.x,
                    -fSin*kCamR.y+fCos*kCamU.y,
                    -fSin*kCamR.z+fCos*kCamU.z);
                kFaceMat.SetCol(2, kCamD);
            }
            else
            {
                kFaceMat.SetCol(0, -kCamR);
                kFaceMat.SetCol(1, -kCamU);
                kFaceMat.SetCol(2, kCamD);
            }
            break;
        }
        case ROTATE_ABOUT_UP:
        {
            // transform the camera location to model space
            NiPoint3 kCamLoc = ((pkCamera->GetWorldLocation() - 
                m_kWorld.m_Translate) * m_kWorld.m_Rotate) / m_kWorld.m_fScale;
            float fLength = NiSqrt(kCamLoc.x*kCamLoc.x+kCamLoc.z*kCamLoc.z);
            if (fLength < 1e-12f) 
                break;

            // unitize NDz
            fLength = 1.0f/fLength;
            kCamLoc.x *= fLength;
            kCamLoc.z *= fLength;

            kFaceMat.SetCol(0, kCamLoc.z, 0.0f, -kCamLoc.x);
            kFaceMat.SetCol(1, 0.0f, 1.0f, 0.0f);
            kFaceMat.SetCol(2, kCamLoc.x, 0.0f, kCamLoc.z);
            break;
        }

        case RIGID_FACE_CAMERA:
        case RIGID_FACE_CENTER:
        {
            NiPoint3 kCamD, kCamU, kCamR;

            if (eFaceMode == RIGID_FACE_CENTER)
            {
                if(!RotateToCenter(pkCamera, m_kWorld.m_Translate, kCamD, 
                    kCamU, kCamR))
                {
                    break;
                }
            }
            else
            {
                // billboard coordinates for world axes of camera
                kCamD = -pkCamera->GetWorldDirection();
                kCamU = pkCamera->GetWorldUpVector();
                kCamR = pkCamera->GetWorldRightVector();
            }
            kCamD = kCamD * m_kWorld.m_Rotate;
            kCamU = kCamU * m_kWorld.m_Rotate;
            kCamR = kCamR * m_kWorld.m_Rotate;

            kFaceMat.SetCol(0, kCamR);
            kFaceMat.SetCol(1, kCamU);
            kFaceMat.SetCol(2, kCamD);
            break;
        }

        default:
        {
            NIASSERT(0);
        }
    }

    // adjust the billboard before applying the world transform
    m_kWorld.m_Rotate = m_kWorld.m_Rotate * kFaceMat;

    // need to push the transforms down while keeping the current m_kWorld for
    // this node, so we start at the children
    RecursivePushTransformsDownward kFunctor;
    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);

        if (pkChild)
            NiTNodeTraversal::DepthFirst_AllObjects(pkChild, kFunctor);
    }
}
//---------------------------------------------------------------------------
void NiBillboardNode::OnVisible(NiCullingProcess& kCuller)
{
    RotateToCamera(kCuller.GetCamera());
    NiNode::OnVisible(kCuller);
}
//---------------------------------------------------------------------------
void NiBillboardNode::UpdateDownwardPass(NiUpdateProcess& kUpdate)
{
    // NOTE: When changing UpdateDownwardPass, UpdateSelectedDownwardPass,
    // or UpdateRigidDownwardPass, remember to make equivalent changes
    // the all of these functions.
    NiNode::UpdateDownwardPass(kUpdate);

    UpdateWorldBound();
}
//---------------------------------------------------------------------------
void NiBillboardNode::UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate)
{
    // NOTE: When changing UpdateDownwardPass, UpdateSelectedDownwardPass,
    // or UpdateRigidDownwardPass, remember to make equivalent changes
    // the all of these functions.
    NiNode::UpdateSelectedDownwardPass(kUpdate);

    UpdateWorldBound();
}
//---------------------------------------------------------------------------
void NiBillboardNode::UpdateRigidDownwardPass(NiUpdateProcess& kUpdate)
{
    // NOTE: When changing UpdateDownwardPass, UpdateSelectedDownwardPass,
    // or UpdateRigidDownwardPass, remember to make equivalent changes
    // the all of these functions.
    NiNode::UpdateRigidDownwardPass(kUpdate);

    UpdateWorldBound();
}
//---------------------------------------------------------------------------
void NiBillboardNode::UpdateWorldBound()
{
    // Custom bound calculation to take the pivot point (center) into effect
    if (GetChildCount())
    {
        NiPoint3 kDiff = m_kWorldBound.GetCenter() - m_kWorld.m_Translate;
        m_kWorldBound.SetCenterAndRadius(m_kWorld.m_Translate,
            m_kWorldBound.GetRadius() + kDiff.Length());
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBillboardNode);
//---------------------------------------------------------------------------
void NiBillboardNode::CopyMembers(NiBillboardNode* pkDest,
    NiCloningProcess& kCloning)
{
    NiNode::CopyMembers(pkDest, kCloning);
    pkDest->m_uFlags = m_uFlags;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBillboardNode);
//---------------------------------------------------------------------------
void NiBillboardNode::LoadBinary(NiStream& kStream)
{
    NiNode::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);

    // Variable should not be streamed in; return to initialized value.
    SetUpdateControllers(true);
}
//---------------------------------------------------------------------------
void NiBillboardNode::LinkObject(NiStream& kStream)
{
    NiNode::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBillboardNode::RegisterStreamables(NiStream& kStream)
{
    return NiNode::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBillboardNode::SaveBinary(NiStream& kStream)
{
    NiNode::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
bool NiBillboardNode::IsEqual(NiObject* pkObject)
{
    if (!NiNode::IsEqual(pkObject))
        return false;

    NiBillboardNode* pkNode = (NiBillboardNode*) pkObject;

    if (GetMode() != pkNode->GetMode())
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiBillboardNode::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiNode::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBillboardNode::ms_RTTI.GetName()));

    pkStrings->Add(GetViewerString("m_eMode", GetMode()));
}
//---------------------------------------------------------------------------
char* NiBillboardNode::GetViewerString(const char* pcPrefix, FaceMode eMode)
{
    size_t stLen = strlen(pcPrefix) + 22;
    char* pcString = NiAlloc(char, stLen);

    switch (eMode)
    {
    case ALWAYS_FACE_CAMERA:
        NiSprintf(pcString, stLen, "%s = ALWAYS_FACE_CAMERA", pcPrefix);
        break;
    case ROTATE_ABOUT_UP:
        NiSprintf(pcString, stLen, "%s = ROTATE_ABOUT_UP", pcPrefix);
        break;
    case RIGID_FACE_CAMERA:
        NiSprintf(pcString, stLen, "%s = RIGID_FACE_CAMERA", pcPrefix);
        break;
    case ALWAYS_FACE_CENTER:
        NiSprintf(pcString, stLen, "%s = ALWAYS_FACE_CENTER", pcPrefix);
        break;
    case RIGID_FACE_CENTER:
        NiSprintf(pcString, stLen, "%s = RIGID_FACE_CENTER", pcPrefix);
        break;
    }

    return pcString;
}
//---------------------------------------------------------------------------
