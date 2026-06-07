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

//---------------------------------------------------------------------------
//
// THIS CLASS HAS BEEN DEPRECATED.
// Functionality here exists only to enable streaming and conversion to the
// NiMorphMeshModifier class.
//
//---------------------------------------------------------------------------


#include "NiAnimationPCH.h"

#include "NiGeomMorpherController.h"

NiImplementRTTI(NiGeomMorpherController, NiInterpController);

//---------------------------------------------------------------------------
NiGeomMorpherController::NiGeomMorpherController(NiMorphData* pkData)
    :
    m_spMorphData(pkData), m_pkInterpItems(NULL)
{
    m_uFlags = 0;
    if (pkData)
    {
        AllocateInterpArray();
    }
}
//---------------------------------------------------------------------------
NiGeomMorpherController::~NiGeomMorpherController()
{
    m_spMorphData = 0;
    NiDelete[] m_pkInterpItems;
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::ResetTimeExtrema()
{
    if (GetManagerControlled())
    {
        // Do nothing if this controller is being controlled by a manager.
        return;
    }

    unsigned int uiNumTargets = GetNumTargets();

    m_fLoKeyTime = NI_INFINITY;
    m_fHiKeyTime = -NI_INFINITY;

    for (unsigned int uiI = 0; uiI < uiNumTargets; uiI++)
    {
        NiInterpolator* pkInterp = GetInterpolator((unsigned char)uiI);
        if (pkInterp)
        {
            float fHi = -NI_INFINITY;
            float fLo = NI_INFINITY;
            pkInterp->GetActiveTimeRange(fLo, fHi);
            
            if (fLo < m_fLoKeyTime)
                m_fLoKeyTime = fLo;
            if (fHi > m_fHiKeyTime)
                m_fHiKeyTime = fHi;
        }
    }
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::Update(float) 
{
    NiOutputDebugString("NiGeomMorpherController::Update: This class has "
        "been deprecated and does nothing.\n");
        
    return;

}
//---------------------------------------------------------------------------
void NiGeomMorpherController::GuaranteeTimeRange(float fStartTime,
    float fEndTime)
{
    for (unsigned int uiTarget = 0; uiTarget < 
         m_spMorphData->GetNumTargets();  uiTarget++)
    {
        NiInterpolator* pkInterp = GetInterpolator((unsigned char)uiTarget);
        if (pkInterp)
            pkInterp->GuaranteeTimeRange(fStartTime, fEndTime);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiGeomMorpherController);
//---------------------------------------------------------------------------
void NiGeomMorpherController::LoadBinary(NiStream& kStream)
{
    NiInterpController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);

    kStream.ReadLinkID();   // m_spMorphData

    NiBool bAlwaysUpdate;
    NiStreamLoadBinary(kStream, bAlwaysUpdate);
    m_bAlwaysUpdate = (bAlwaysUpdate != 0);
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 104))
    {
        if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 3))
        {
            kStream.ReadMultipleLinkIDs(); // m_aspInterpolators
        }
        else
        {
            unsigned int uiArraySize;
            NiStreamLoadBinary(kStream, uiArraySize);
            m_pkInterpItems = NiNew InterpItem[uiArraySize];
            for (unsigned short ui = 0; ui < uiArraySize; ui++)
            {
                m_pkInterpItems[ui].m_spInterpolator =
                    (NiInterpolator*) kStream.ResolveLinkID();
                NiStreamLoadBinary(kStream, m_pkInterpItems[ui].m_fWeight);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::LinkObject(NiStream& kStream)
{
    NiInterpController::LinkObject(kStream);

    m_spMorphData = (NiMorphData*) kStream.GetObjectFromLinkID();

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 3))
    {
        // Allocate interp items array and load weights.
        AllocateInterpArray();
        for (unsigned int ui = 0; ui < GetNumTargets(); ui++)
        {
            m_pkInterpItems[ui].m_fWeight =
                m_spMorphData->GetTarget(ui)->GetLegacyWeight();
        }
    }

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104)
        && m_spMorphData)
    {
        // we do not have to allocate the interpolator pointer array,
        // as it has already been allocated above
        unsigned int uiNumTargets = m_spMorphData->GetNumTargets();
        for (unsigned int ui = 0; ui < uiNumTargets; ui++)
        {
            NiMorphData::MorphTarget* pkTarget = 
                m_spMorphData->GetTarget(ui);
            NIASSERT(pkTarget != NULL);
            NiInterpolator* pkInterp = pkTarget->GetLegacyInterpolator();
            SetInterpolator(pkInterp, (unsigned char)ui);
        }
    }
    else if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 3))
    {
        // link interpolators
        // we do not have to allocate the interpolator pointer array,
        // as it has already been allocated above
        unsigned int uiSize = kStream.GetNumberOfLinkIDs();
        if (uiSize)
        {
            NIASSERT(uiSize == GetNumTargets());
            NIASSERT(uiSize == GetInterpolatorCount());
            for (unsigned int ui = 0; ui < uiSize; ui++)
            {
                NiInterpolator* pkInterp = 
                    (NiInterpolator*) kStream.GetObjectFromLinkID();
                SetInterpolator(pkInterp, (unsigned char)ui);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiGeomMorpherController::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::SaveBinary(NiStream& kStream)
{
    // This class has been deprecated. SaveBinary should no longer be called 
    // since all NiGeomMorpherController objects should be converted to mesh 
    // modifiers and removed during loading and conversion. Exporters should 
    // no longer create this controller.
    NIASSERT(false);

    NiObject::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiGeomMorpherController::IsEqual(NiObject* pkObject)
{
    if (!NiInterpController::IsEqual(pkObject))
        return false;

    NiGeomMorpherController* pkMorph = (NiGeomMorpherController*) pkObject;

    if (!m_spMorphData->IsEqual(pkMorph->m_spMorphData))
        return false;

    for (unsigned int ui = 0; ui < GetNumTargets(); ui++)
    {
        InterpItem& kCompareItem = pkMorph->m_pkInterpItems[ui];
        if ((kCompareItem.m_spInterpolator &&
                !m_pkInterpItems[ui].m_spInterpolator) ||
            (!kCompareItem.m_spInterpolator &&
                m_pkInterpItems[ui].m_spInterpolator) ||
            (kCompareItem.m_spInterpolator &&
                !kCompareItem.m_spInterpolator->IsEqual(
                m_pkInterpItems[ui].m_spInterpolator)))
        {
            return false;
        }
        
        if (kCompareItem.m_fWeight != m_pkInterpItems[ui].m_fWeight)
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiInterpController methods
//---------------------------------------------------------------------------
unsigned short NiGeomMorpherController::GetInterpolatorCount() const
{
    return (unsigned short) GetNumTargets();
}
//---------------------------------------------------------------------------
const char* NiGeomMorpherController::GetInterpolatorID(unsigned short 
    usIndex) 
{
    NIASSERT(usIndex < GetInterpolatorCount());
    NiMorphData::MorphTarget* pkTarget = m_spMorphData->GetTarget(usIndex);
    NIASSERT(pkTarget);
    return pkTarget->GetName();
}
//---------------------------------------------------------------------------
unsigned short NiGeomMorpherController::GetInterpolatorIndex(const char* pcID) 
    const
{
    if (pcID == NULL)
        return INVALID_INDEX;

    for (unsigned short us = 0; us < GetNumTargets(); us++)
    {
        NiMorphData::MorphTarget* pkTarget = 
            m_spMorphData->GetTarget(us);
        NIASSERT(pkTarget);
        if (NiStricmp(pkTarget->GetName(), pcID) == 0)
            return us;
    }

    return INVALID_INDEX;
}
//---------------------------------------------------------------------------
NiInterpolator* NiGeomMorpherController::GetInterpolator(
    unsigned short usIndex) const
{
    NIASSERT(m_pkInterpItems != NULL);
    NIASSERT(usIndex < GetInterpolatorCount());
    return m_pkInterpItems[usIndex].m_spInterpolator;
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::SetInterpolator(NiInterpolator* pkInterpolator,
    unsigned short usIndex)
{
    NIASSERT(m_pkInterpItems != NULL);
    NIASSERT(usIndex < GetInterpolatorCount());
    NIASSERT(!pkInterpolator || InterpolatorIsCorrectType(pkInterpolator,
        usIndex));
    m_pkInterpItems[usIndex].m_spInterpolator = pkInterpolator;
}
//---------------------------------------------------------------------------
NiEvaluator* NiGeomMorpherController::CreatePoseEvaluator(
    unsigned short)
{
    NIASSERT(false && "NiGeomMorpherController is deprecated.\n");
    return 0;
}
//---------------------------------------------------------------------------
NiInterpolator* NiGeomMorpherController::CreatePoseInterpolator(
    unsigned short)
{
    NIASSERT(false && "NiGeomMorpherController is deprecated.\n");
    return 0;
}
//---------------------------------------------------------------------------
void NiGeomMorpherController::SynchronizePoseInterpolator(
    NiInterpolator*, unsigned short)
{
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiGeomMorpherController::CreateBlendInterpolator(
    unsigned short, bool, float, unsigned char) const
{
    NIASSERT(false && "NiGeomMorpherController is deprecated.\n");
    return 0;
}
//---------------------------------------------------------------------------
bool NiGeomMorpherController::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, unsigned short) const
{
    NIASSERT(pkInterpolator);
    return pkInterpolator->IsFloatValueSupported();
}
//---------------------------------------------------------------------------
