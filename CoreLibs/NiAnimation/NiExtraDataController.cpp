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
#include "NiAnimationPCH.h"

#include "NiExtraDataController.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiExtraDataController, NiSingleInterpController);

//---------------------------------------------------------------------------
NiExtraDataController::NiExtraDataController(
    const NiFixedString& kExtraDataName) 
{
    SetExtraDataName(kExtraDataName);
}
//---------------------------------------------------------------------------
NiExtraDataController::NiExtraDataController() 
{
}
//---------------------------------------------------------------------------
NiExtraDataController::~NiExtraDataController()
{
}
//---------------------------------------------------------------------------
void NiExtraDataController::SetTarget(NiObjectNET* pkTarget)
{
    // Clear the extra data pointer.
    m_spExtraData = NULL;

    // Call the base class SetTarget.
    NiSingleInterpController::SetTarget(pkTarget);

    // If the target was set to something other than NULL, retreive the extra
    // data pointer using the name.
    if (m_pkTarget)
    {
        GetExtraDataPointerFromName();
    }
}
//---------------------------------------------------------------------------
bool NiExtraDataController::InterpTargetIsCorrectType(NiObjectNET* pkTarget)
    const
{
    if (!NiIsKindOf(NiObjectNET, pkTarget) || !m_kExtraDataName.Exists())
    {
        return false;
    }

    if (pkTarget)
    {
        for (unsigned int ui = 0 ; ui < pkTarget->GetExtraDataSize(); ui++)
        {
            NiExtraData* pkExtraData = 
                pkTarget->GetExtraDataAt((unsigned char)ui);
            const NiFixedString& kExtraDataName = pkExtraData->GetName();

            if (kExtraDataName == m_kExtraDataName)
            {
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
const char* NiExtraDataController::GetCtlrID()
{
    return m_kExtraDataName;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiExtraDataController::CopyMembers(NiExtraDataController* pkDest,
    NiCloningProcess& kCloning)
{
    NiSingleInterpController::CopyMembers(pkDest, kCloning);

    pkDest->SetExtraDataName(m_kExtraDataName);
}
//---------------------------------------------------------------------------
void NiExtraDataController::ProcessClone(
    NiCloningProcess& kCloning)
{
    NiSingleInterpController::ProcessClone(kCloning);

    NiObject* pkCloneObj = 0;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkCloneObj));

    NiExtraDataController* pkClone = (NiExtraDataController*)pkCloneObj;

    if (m_pkTarget && pkClone->m_pkTarget)
    {
        NIASSERT(m_spExtraData);

        NiObject* pkClonedExtraData = 0;
        NIVERIFY(kCloning.m_pkCloneMap->GetAt(m_spExtraData, 
            pkClonedExtraData));

        pkClone->m_spExtraData = (NiExtraData*)pkClonedExtraData;
    }
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiExtraDataController::LoadBinary(NiStream& kStream)
{
    NiSingleInterpController::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        kStream.LoadCStringAsFixedString(m_kExtraDataName);
    }
    else
    {
        kStream.LoadFixedString(m_kExtraDataName);
    }
}
//---------------------------------------------------------------------------
void NiExtraDataController::LinkObject(NiStream& kStream)
{
    NiSingleInterpController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiExtraDataController::PostLinkObject(NiStream& kStream)
{
    NiSingleInterpController::PostLinkObject(kStream);

    if (m_pkTarget)
    {
        GetExtraDataPointerFromName();
    }
}
//---------------------------------------------------------------------------
bool NiExtraDataController::RegisterStreamables(NiStream& kStream)
{
    if (!NiSingleInterpController::RegisterStreamables(kStream))
    {
        return false;
    }

    kStream.RegisterFixedString(m_kExtraDataName);

    return true;
}
//---------------------------------------------------------------------------
void NiExtraDataController::SaveBinary(NiStream& kStream)
{
    NiSingleInterpController::SaveBinary(kStream);

    NIASSERT(m_kExtraDataName);
    kStream.SaveFixedString(m_kExtraDataName);
}
//---------------------------------------------------------------------------
bool NiExtraDataController::IsEqual(NiObject* pkObject)
{
    if (!NiSingleInterpController::IsEqual(pkObject))
    {
        return false;
    }

    NiExtraDataController* pkControl = (NiExtraDataController*) pkObject;

    if ((m_spExtraData && !pkControl->m_spExtraData) ||
        (!m_spExtraData && pkControl->m_spExtraData) ||
        (m_spExtraData && pkControl->m_spExtraData && 
            !m_spExtraData->IsEqual(pkControl->m_spExtraData)))
    {
        return false;
    }

    if (pkControl->m_kExtraDataName != m_kExtraDataName)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiExtraDataController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiSingleInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiExtraDataController::ms_RTTI
        .GetName()));

    NIASSERT(m_kExtraDataName);
    pkStrings->Add(NiGetViewerString("Extra Data Name", (const char*)
        m_kExtraDataName));
}
//---------------------------------------------------------------------------
