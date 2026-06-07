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
#include <NiParticlePCH.h>

#include "NiPSysMeshUpdateModifier.h"

NiImplementRTTI(NiPSysMeshUpdateModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysMeshUpdateModifier::NiPSysMeshUpdateModifier()
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysMeshUpdateModifier);
//---------------------------------------------------------------------------
void NiPSysMeshUpdateModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    // The Order in previous versions was incorrect
    m_uiOrder = ORDER_POSTPOSUPDATE;

    kStream.ReadMultipleLinkIDs();  // m_kMeshes
}
//---------------------------------------------------------------------------
void NiPSysMeshUpdateModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    unsigned int uiSize = kStream.GetNumberOfLinkIDs();
    m_kMeshes.SetSize(uiSize);
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiAVObject* pkMesh = (NiAVObject*) kStream.GetObjectFromLinkID();
        m_kMeshes.SetAt(ui, pkMesh);
    }
}
//---------------------------------------------------------------------------
bool NiPSysMeshUpdateModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysModifier::RegisterStreamables(kStream))
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_kMeshes.GetSize(); ui++)
    {
        NiAVObject* pkMesh = m_kMeshes.GetAt(ui);
        if (pkMesh)
        {
            pkMesh->RegisterStreamables(kStream);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSysMeshUpdateModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    unsigned int uiSize = m_kMeshes.GetSize();
    NiStreamSaveBinary(kStream, uiSize);
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiAVObject* pkMesh = m_kMeshes.GetAt(ui);
        kStream.SaveLinkID(pkMesh);
    }
}
//---------------------------------------------------------------------------
bool NiPSysMeshUpdateModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysMeshUpdateModifier* pkDest = (NiPSysMeshUpdateModifier*) pkObject;

    if (m_kMeshes.GetSize() != pkDest->m_kMeshes.GetSize())
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_kMeshes.GetSize(); ui++)
    {
        NiAVObject* pkMesh1 = m_kMeshes.GetAt(ui);
        NiAVObject* pkMesh2 = pkDest->GetMeshAt(ui);
        if ((pkMesh1 && !pkMesh2) || (!pkMesh1 && pkMesh2) ||
            (pkMesh1 && pkMesh2 && !pkMesh1->IsEqual(pkMesh2)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
