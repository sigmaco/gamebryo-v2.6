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
#include "NiRangeLODData.h"
#include "NiCullingProcess.h"

NiImplementRTTI(NiLODNode, NiSwitchNode);

int NiLODNode::ms_iGlobalLOD = -1;

//---------------------------------------------------------------------------
NiLODNode::NiLODNode()
{
    m_iIndex = 0;
    m_bLODActive = true;
}
//---------------------------------------------------------------------------
NiLODNode::~NiLODNode()
{
}
//---------------------------------------------------------------------------
void NiLODNode::UpdateWorldData()
{
    NiSwitchNode::UpdateWorldData();
    
    if (m_spLODData)
    {
        m_spLODData->UpdateWorldData(this);
    }
}
//---------------------------------------------------------------------------
void NiLODNode::OnVisible(NiCullingProcess& kCuller)
{
    if (m_bLODActive)
    {
        if (m_spLODData)
        {
            m_iIndex = m_spLODData->GetLODLevel(kCuller.GetCamera(), this);

            // Check to see if there is a global LOD setting. If this is used
            // then override what the LOD Data has indicated. 
            if (ms_iGlobalLOD >= 0)
            {
                m_iIndex = m_spLODData->GetLODIndex(ms_iGlobalLOD);
            }

            // Scan Backwords to Make Sure we arn't selecting past the end 
            // of our children
            while((m_iIndex >= 0) && ((m_iIndex >= (int)m_kChildren.GetSize())
                || (m_kChildren.GetAt(m_iIndex) == NULL)))
            {
                m_iIndex--;
            }
        }
    }

    NiSwitchNode::OnVisible(kCuller);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiLODNode);
//---------------------------------------------------------------------------
void NiLODNode::CopyMembers(NiLODNode* pkDest, 
    NiCloningProcess& kCloning)
{
    NiSwitchNode::CopyMembers(pkDest, kCloning);

    if (m_spLODData)
        pkDest->SetLODData(m_spLODData->Duplicate());
    else
        pkDest->SetLODData(NULL);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiLODNode);
//---------------------------------------------------------------------------
void NiLODNode::LoadBinary(NiStream& kStream)
{
    NiSwitchNode::LoadBinary(kStream);

    // Read the Link ID for the LOD data.
    kStream.ReadLinkID();
}
//---------------------------------------------------------------------------
void NiLODNode::LinkObject(NiStream& kStream)
{
    NiSwitchNode::LinkObject(kStream);

    m_spLODData = (NiLODData*)kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiLODNode::RegisterStreamables(NiStream& kStream)
{
    if(!NiSwitchNode::RegisterStreamables(kStream))
        return false;

    if (m_spLODData)
    {
        m_spLODData->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiLODNode::SaveBinary(NiStream& kStream)
{
    NiSwitchNode::SaveBinary(kStream);

    kStream.SaveLinkID(m_spLODData);
}
//---------------------------------------------------------------------------
bool NiLODNode::IsEqual(NiObject* pkObject)
{
    if (!NiSwitchNode::IsEqual(pkObject))
        return false;

    NiLODNode* pkLOD = (NiLODNode*) pkObject;

    NiLODData* pkObjectData = pkLOD->GetLODData();

    // Check that both have LOD data
    if ((m_spLODData && !pkObjectData) ||
        (!m_spLODData && pkObjectData))
    {
        return false;
    }

    // Both have LOD data of the correct type and they are 
    // not equal
    if (m_spLODData && pkObjectData &&
        (m_spLODData->GetRTTI() == pkObjectData->GetRTTI()) &&
        !m_spLODData->IsEqual(pkObjectData))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiLODNode::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiSwitchNode::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiLODNode::ms_RTTI.GetName()));

    if (m_spLODData)
    {
        m_spLODData->GetViewerStrings(pkStrings);
    }
    else
    {
#if defined(_PS3)
        char cStr[16];
        NiSprintf(cStr, sizeof(cStr), "NULL LOD Data");
        pkStrings->Add(cStr);
#else
        char* pStr = "NULL LOD Data";
        pkStrings->Add(pStr);
#endif
    }
}
//---------------------------------------------------------------------------
