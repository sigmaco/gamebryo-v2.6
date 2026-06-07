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
#include "NiMeshPCH.h"
#include "NiMeshModifier.h"
#include "NiStream.h"
#include <NiRenderer.h>

//---------------------------------------------------------------------------
NiImplementRTTI(NiMeshModifier, NiObject);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
NiMeshModifier::NiMeshModifier()
{

}
//---------------------------------------------------------------------------
NiMeshModifier::~NiMeshModifier()
{
}
//---------------------------------------------------------------------------
bool NiMeshModifier::RequiresMaterialOption(
    const NiFixedString&, bool&) const
{
    return false;
}
//---------------------------------------------------------------------------
bool NiMeshModifier::AreRequirementsMet(NiMesh*,
    NiSystemDesc::RendererID) const
{
    // Modifiers that do not have any requirements do not need to
    // implement this function.
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshModifier::AreRequirementsMet(NiMesh* pkMesh) const
{
    NiSystemDesc::RendererID eRendererID = NiSystemDesc::RENDERER_GENERIC;
    if (!NiSystemDesc::GetSystemDesc().GetToolMode())
    {
        NiRenderer* pkRenderer = NiRenderer::GetRenderer();
        if (pkRenderer)
            eRendererID = pkRenderer->GetRendererID();
    }
    else
    {
        eRendererID = NiSystemDesc::GetSystemDesc().GetToolModeRendererID();
    }

    return AreRequirementsMet(pkMesh, eRendererID);
}
//---------------------------------------------------------------------------
void NiMeshModifier::RetrieveRequirements(NiMeshRequirements&) const
{
    // No requirements by default
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiMeshModifier::CopyMembers(NiMeshModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    NiUInt32 uiSyncCount = m_kSubmitPoints.GetSize();
    for (NiUInt32 ui = 0; ui < uiSyncCount; ui++)
    {
        pkDest->m_kSubmitPoints.Add(m_kSubmitPoints.GetAt(ui));
    }

    uiSyncCount = m_kCompletePoints.GetSize();
    for (NiUInt32 ui = 0; ui < uiSyncCount; ui++)
    {
        pkDest->m_kCompletePoints.Add(m_kCompletePoints.GetAt(ui));
    }
}
//---------------------------------------------------------------------------
void NiMeshModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiMeshModifier::PostLinkObject(NiStream& kStream)
{
    NiObject::PostLinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiMeshModifier::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    m_kSubmitPoints.RemoveAll();
    m_kCompletePoints.RemoveAll();

    NiUInt32 uiSyncCount = 0;
    NiStreamLoadBinary(kStream, uiSyncCount);
    for (NiUInt32 ui = 0; ui < uiSyncCount; ui++)
    {
        NiSyncArgs::SyncPoint uiSyncPt;
        NiStreamLoadBinary(kStream, uiSyncPt);
        m_kSubmitPoints.Add(uiSyncPt);
    }

    NiStreamLoadBinary(kStream, uiSyncCount);
    for (NiUInt32 ui = 0; ui < uiSyncCount; ui++)
    {
        NiSyncArgs::SyncPoint uiSyncPt;
        NiStreamLoadBinary(kStream, uiSyncPt);
        m_kCompletePoints.Add(uiSyncPt);
    }
}
//---------------------------------------------------------------------------
void NiMeshModifier::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiMeshModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
void NiMeshModifier::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiUInt32 uiSyncCount = m_kSubmitPoints.GetSize();
    NiStreamSaveBinary(kStream, uiSyncCount);
    for (NiUInt32 ui = 0; ui < uiSyncCount; ui++)
    {
        NiSyncArgs::SyncPoint uiSyncPt = m_kSubmitPoints.GetAt(ui);
        NiStreamSaveBinary(kStream, uiSyncPt);
    }

    uiSyncCount = m_kCompletePoints.GetSize();
    NiStreamSaveBinary(kStream, uiSyncCount);
    for (NiUInt32 ui = 0; ui < uiSyncCount; ui++)
    {
        NiSyncArgs::SyncPoint uiSyncPt = m_kCompletePoints.GetAt(ui);
        NiStreamSaveBinary(kStream, uiSyncPt);
    }
}
//---------------------------------------------------------------------------
bool NiMeshModifier::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiMeshModifier* pkMod = (NiMeshModifier*) pkObject;

    NiUInt32 uiSyncCount = m_kSubmitPoints.GetSize();
    for (NiUInt32 ui = 0; ui < uiSyncCount; ui++)
    {
        if (!pkMod->SupportsSubmitSyncPoint(m_kSubmitPoints.GetAt(ui)))
            return false;
    }

    uiSyncCount = m_kCompletePoints.GetSize();
    for (NiUInt32 ui = 0; ui < uiSyncCount; ui++)
    {
        if (!pkMod->SupportsCompleteSyncPoint(m_kCompletePoints.GetAt(ui)))
            return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiMeshModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);
    pkStrings->Add(NiGetViewerString(NiMeshModifier::ms_RTTI.GetName()));
    
    NiUInt32 uiSyncCount = m_kSubmitPoints.GetSize();
    for (NiUInt32 ui = 0; ui < uiSyncCount; ui++)
    {
        NiSyncArgs::SyncPoint uiSyncPt = m_kSubmitPoints.GetAt(ui);
        pkStrings->Add(NiGetViewerString("Submit Sync",
            NiSyncArgs::GetSyncPointName(uiSyncPt)));
    }

    uiSyncCount = m_kCompletePoints.GetSize();
    for (NiUInt32 ui = 0; ui < uiSyncCount; ui++)
    {
        NiSyncArgs::SyncPoint uiSyncPt = m_kCompletePoints.GetAt(ui);
        pkStrings->Add(NiGetViewerString("Complete Sync",
            NiSyncArgs::GetSyncPointName(uiSyncPt)));
    }
}
//---------------------------------------------------------------------------
