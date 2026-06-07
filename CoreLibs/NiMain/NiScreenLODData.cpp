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

#include "NiLODNode.h"
#include "NiCamera.h"
#include "NiScreenLODData.h"
#include "NiMath.h"

NiImplementRTTI(NiScreenLODData, NiLODData);

//---------------------------------------------------------------------------
NiScreenLODData::NiScreenLODData()
{
    m_kBound.SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
    m_kWorldBound.SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
    m_uiNumProportions = 0;
    m_pfProportions = NULL;
}
//---------------------------------------------------------------------------
NiScreenLODData::~NiScreenLODData()
{
    NiFree(m_pfProportions);
}
//---------------------------------------------------------------------------
void NiScreenLODData::UpdateWorldData(NiLODNode* pkLOD)
{
    // Automatically create a bound if there isn't one
    if (m_kBound.GetRadius() == 0.0f)
    {
        AutoSetBound(pkLOD);
        return;
    }

    m_kWorldBound.Update(m_kBound, pkLOD->GetWorldTransform());
}
//---------------------------------------------------------------------------
NiLODData* NiScreenLODData::Duplicate()
{
    NiScreenLODData* pkData = NiNew NiScreenLODData;
    NIASSERT(pkData);

    pkData->SetBound(m_kBound);

    pkData->SetNumProportions(m_uiNumProportions);

    unsigned int uiDestSize = sizeof(m_pfProportions[0]) * 
        m_uiNumProportions;
    NiMemcpy(pkData->m_pfProportions, m_pfProportions, uiDestSize);

    return (NiLODData*)pkData;
}
//---------------------------------------------------------------------------
void NiScreenLODData::SetNumProportions(unsigned int uiNumProportions)
{
    // Don't allocate anything new if we already have what we neeed
    if (uiNumProportions == m_uiNumProportions)
        return;

    // Check for 0 proportions
    if (uiNumProportions == 0)
    {
        NiFree(m_pfProportions);
        m_pfProportions = NULL;
        m_uiNumProportions = 0;
        return;
    }

    float* pfNewProportions = NiAlloc(float, uiNumProportions);
    unsigned int uiDestSize = uiNumProportions * sizeof(float);

    NIASSERT(pfNewProportions);

    if (m_pfProportions)
    {
        NiMemcpy(pfNewProportions, uiDestSize, m_pfProportions,
            sizeof(pfNewProportions[0]) * NiMin((int)m_uiNumProportions,
            (int)uiNumProportions));
    }

    NiFree(m_pfProportions);

    m_pfProportions = pfNewProportions;
    m_uiNumProportions = uiNumProportions;
}
//---------------------------------------------------------------------------
int NiScreenLODData::GetLODLevel(const NiCamera* pkCamera, 
    NiLODNode* pkLOD) const
{
    NI_UNUSED_ARG(pkLOD);
    NIASSERT(pkCamera);
    NIASSERT(pkLOD);

// The Rigerous solution for screen area is
//
// 4.0 * NI_PI * BoundRadius Squared
// ------------------------------------
// Distance Square * (Left - Right) * (Top - Bottom)
//
//
// We will be using simplifed algorithm that uses the proportion of
// (bound radius / Distance) / (Minimum Screen axis)
//
//  fMinScreenDimmension = NiMin( (fRight - fLeft) / 2.0f, 
//      (Top - Bottom) / 2.0f );
//  fProportion =  (fRatio / fMinScreenDimmension) 
//
// With the final simplification of inverting the screene dimminsion we can 
// turn the final division into a multiplication and gain performance
//
//  fInvMinScreenDimmension = NiMax(  2.0f / (fRight - fLeft),  
//      2.0f / (Top - Bottom));
//  fProportion =  fRatio * fMinScreenDimmension; 

    float fBoundViewDist = (m_kWorldBound.GetCenter() - 
        pkCamera->GetWorldTranslate()).Length();

    // Check for being inside the bounds or distance is near zero then return 
    // the highest level of detail
    if ((fBoundViewDist < m_kWorldBound.GetRadius()) ||
        (fBoundViewDist <= 0.000001f))
        return 0;

    float fRatio;
    const NiFrustum& kFrustum = pkCamera->GetViewFrustum();
    if (kFrustum.m_bOrtho)
    {
        fRatio = m_kWorldBound.GetRadius();
    }
    else
    {
        fRatio = m_kWorldBound.GetRadius() / fBoundViewDist;
    }

    // Find the inverted minimum Screen Dimminsion.
    float fInvMinScreenDimmension = NiMax( 
        2.0f / (kFrustum.m_fRight - kFrustum.m_fLeft), 
        2.0f / (kFrustum.m_fTop - kFrustum.m_fBottom));


    float fProportion = 
        fRatio * fInvMinScreenDimmension * pkCamera->GetLODAdjust();

    //  Inherent Ordering.
    //
    //      It is assumed that the LOD node has its children ordered from 
    // Highest level of detail(child 0) to the lowest level of detail (Child
    // N). It also assumes that only 1 child will ever be visible at any given
    // time and one child will always be visible (aka. NiSwitchNode) Given
    // these assumptions the NiScreenLODData needs to only save the far
    // boundary for each LOD level. A particular LOD level will be drawn until
    // it's screen space boundary is small then a "Far" proportion.
    //
    unsigned int uiIndex;
    for (uiIndex = 0; uiIndex < m_uiNumProportions; uiIndex++)
    {
        if (fProportion >= m_pfProportions[uiIndex])
        {
            return (int)uiIndex;
        }
    }
    
    //  Lowest Detail LOD has No Proportion
    //
    //      The lowest level of detail is assumed to have a far proportion of
    // zero because it should always be shown. For this reason lowest level of
    // detail has no proportion. The number of proportions will be one less
    // than the number of LOD levels. (m_uiNumProportions = (Num LOD -1)).
    // You are only storing the bound conditions between LOD levels and the
    // lowest LOD has no far boundary. For this reason we can just return 
    // uiIndex which is now the lowest LOD level.
    return (int)uiIndex;
}
//---------------------------------------------------------------------------
void NiScreenLODData::AutoSetBound(NiLODNode* pkLOD)
{
    // Clear the old world bound
    m_kWorldBound.SetRadius(0.0f);

    // Sum the world bound for all of the children
    for (NiUInt32 ui = 0; ui < pkLOD->GetArrayCount(); ++ui)
    {
        NiAVObject* pkChild = pkLOD->GetAt(ui);
        
        if (pkChild != NULL)
        {
            m_kWorldBound.Merge(&pkChild->GetWorldBound());
        }
    }

    // Invert to a Local Bound
    NiTransform kWorldInverse;
    pkLOD->GetWorldTransform().Invert(kWorldInverse);
    m_kBound.Update(m_kWorldBound, kWorldInverse);
}
//---------------------------------------------------------------------------
int NiScreenLODData::GetLODIndex(int iLODLevel) const
{
    if (iLODLevel < 0)
        return 0;
    if (iLODLevel >= (int) m_uiNumProportions)
        return m_uiNumProportions;

    return iLODLevel;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiScreenLODData);
//---------------------------------------------------------------------------
void NiScreenLODData::CopyMembers(NiScreenLODData* pkDest, 
    NiCloningProcess&)
{
    pkDest->SetBound(m_kBound);

    pkDest->SetNumProportions(m_uiNumProportions);

    unsigned int uiDestSize = sizeof(m_pfProportions[0]) * 
        m_uiNumProportions;
    NiMemcpy(pkDest->m_pfProportions, m_pfProportions, uiDestSize);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiScreenLODData);
//---------------------------------------------------------------------------
void NiScreenLODData::LoadBinary(NiStream& kStream)
{
    NiLODData::LoadBinary(kStream);

    // Stream in the Bounds
    m_kBound.LoadBinary(kStream);
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 4, 0, 13))
    {
        NiBound kDummy;
        kDummy.LoadBinary(kStream);
    }

    // Load the number of proportions
    unsigned int uiProportions;
    NiStreamLoadBinary(kStream, uiProportions);

    SetNumProportions(uiProportions);

    // Load each of the proportions
    for (unsigned int i = 0; i < uiProportions; i++)
    {
        NiStreamLoadBinary(kStream, m_pfProportions[i]);
    }
}
//---------------------------------------------------------------------------
void NiScreenLODData::LinkObject(NiStream& kStream)
{
    NiLODData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiScreenLODData::RegisterStreamables(NiStream& kStream)
{
    return NiLODData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiScreenLODData::SaveBinary(NiStream& kStream)
{
    NiLODData::SaveBinary(kStream);

    // Save the Bounds
    m_kBound.SaveBinary(kStream);

    // Save the number of proportions
    NiStreamSaveBinary(kStream, m_uiNumProportions);

    // Save each proportion
    for (unsigned int i = 0; i < m_uiNumProportions; i++)
    {
        NiStreamSaveBinary(kStream, m_pfProportions[i]);
    }
}
//---------------------------------------------------------------------------
bool NiScreenLODData::IsEqual(NiObject* pkObject)
{
    NiScreenLODData* pkLOD = (NiScreenLODData*) pkObject;

    if (m_kBound != pkLOD->m_kBound)
        return false;

    if (m_uiNumProportions != pkLOD->m_uiNumProportions)
        return false;

    for (unsigned int i = 0; i < m_uiNumProportions; i++)
    {
        if (pkLOD->m_pfProportions[i] != m_pfProportions[i])
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiScreenLODData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    pkStrings->Add(NiGetViewerString(NiScreenLODData::ms_RTTI.GetName()));

    pkStrings->Add(m_kBound.GetViewerString("Bound"));

    pkStrings->Add(m_kWorldBound.GetViewerString("World Bound"));

    for (unsigned int i = 0; i < m_uiNumProportions; i++)
    {
        char* pcString = NiAlloc(char, 128);

        NiSprintf(pcString, 128, "Proportion[%d] = %g", i, 
            m_pfProportions[i]);
        pkStrings->Add(pcString);
    }
}
//---------------------------------------------------------------------------
