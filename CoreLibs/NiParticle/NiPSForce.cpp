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
#include "NiParticlePCH.h"

#include "NiPSForce.h"
#include <NiBool.h>

NiImplementRTTI(NiPSForce, NiObject);

//---------------------------------------------------------------------------
NiPSForce::NiPSForce(
    const NiFixedString& kName,
    NiPSForceDefinitions::ForceType eType) :
    m_kName(kName),
    m_eType(eType),
    m_bActive(true)
{
}
//---------------------------------------------------------------------------
NiPSForce::NiPSForce() :
    m_eType(NiPSForceDefinitions::FORCE_BOMB),
    m_bActive(true)
{
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSForce::CopyMembers(NiPSForce* pkDest, NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_kName = m_kName;
    pkDest->m_eType = m_eType;
    pkDest->m_bActive = m_bActive;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSForce::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    kStream.LoadFixedString(m_kName);
    NiStreamLoadEnum(kStream, m_eType);
    NiBool bValue;
    NiStreamLoadBinary(kStream, bValue);
    m_bActive = NIBOOL_IS_TRUE(bValue);
}
//---------------------------------------------------------------------------
void NiPSForce::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSForce::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
    {
        return false;
    }

    kStream.RegisterFixedString(m_kName);

    return true;
}
//---------------------------------------------------------------------------
void NiPSForce::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    kStream.SaveFixedString(m_kName);
    NiStreamSaveEnum(kStream, m_eType);
    NiStreamSaveBinary(kStream, NiBool(m_bActive));
}
//---------------------------------------------------------------------------
bool NiPSForce::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiPSForce* pkDest = (NiPSForce*) pkObject;

    if (pkDest->m_kName != m_kName ||
        pkDest->m_eType != m_eType ||
        pkDest->m_bActive != m_bActive)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSForce::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSForce::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Name", m_kName));
    
    const char* pcType = NULL;
    switch (m_eType)
    {
        case NiPSForceDefinitions::FORCE_BOMB:
            pcType = "FORCE_BOMB";
            break;
        case NiPSForceDefinitions::FORCE_DRAG:
            pcType = "FORCE_DRAG";
            break;
        case NiPSForceDefinitions::FORCE_AIR_FIELD:
            pcType = "FORCE_AIR_FIELD";
            break;
        case NiPSForceDefinitions::FORCE_DRAG_FIELD:
            pcType = "FORCE_DRAG_FIELD";
            break;
        case NiPSForceDefinitions::FORCE_GRAVITY_FIELD:
            pcType = "FORCE_GRAVITY_FIELD";
            break;
        case NiPSForceDefinitions::FORCE_RADIAL_FIELD:
            pcType = "FORCE_RADIAL_FIELD";
            break;
        case NiPSForceDefinitions::FORCE_TURBULENCE_FIELD:
            pcType = "FORCE_TURBULENCE_FIELD";
            break;
        case NiPSForceDefinitions::FORCE_VORTEX_FIELD:
            pcType = "FORCE_VORTEX_FIELD";
            break;
        case NiPSForceDefinitions::FORCE_GRAVITY:
            pcType = "FORCE_GRAVITY";
            break;
        default:
            NIASSERT(!"Unknown ForceType!");
            break;
    }
    NIASSERT(pcType);
    pkStrings->Add(NiGetViewerString("Type", pcType));

    pkStrings->Add(NiGetViewerString("Active", m_bActive));
}
//---------------------------------------------------------------------------
