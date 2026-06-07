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

#include "NiBool.h"
#include "NiMainMetrics.h"
#include "NiSwitchNode.h"

NiImplementRTTI(NiSwitchNode, NiNode);

//---------------------------------------------------------------------------
NiSwitchNode::NiSwitchNode() :
    m_uFlags(0),
    m_iIndex(-1)
{
}
//---------------------------------------------------------------------------
NiAVObjectPtr NiSwitchNode::DetachChild(NiAVObject* pkChild)
{
    NiAVObjectPtr spObject = NiNode::DetachChild(pkChild);

    // Set the Index to -1 if we removed the current child
    if ((m_iIndex > -1) && ((m_iIndex >= (int)m_kChildren.GetSize()) ||
        (m_kChildren.GetAt(m_iIndex) == NULL)))
    {
        m_iIndex = -1;
    }

    return spObject;
}
//---------------------------------------------------------------------------
NiAVObjectPtr NiSwitchNode::DetachChildAt(unsigned int i)
{
    NiAVObjectPtr spObject = NiNode::DetachChildAt(i);

    // Set the Index to -1 if we removed the current child
    if ((m_iIndex > -1) && ((m_iIndex >= (int)m_kChildren.GetSize()) ||
        (m_kChildren.GetAt(m_iIndex) == NULL)))
    {
        m_iIndex = -1;
    }

    return spObject;
}
//---------------------------------------------------------------------------
void NiSwitchNode::OnVisible(NiCullingProcess& kCuller)
{
    if (m_iIndex >= 0)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(m_iIndex);
        if (pkChild)
        {
            pkChild->Cull(kCuller);
        }
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiSwitchNode);
//---------------------------------------------------------------------------
void NiSwitchNode::CopyMembers(NiSwitchNode* pkDest,
    NiCloningProcess& kCloning)
{
    NiNode::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;

    pkDest->m_iIndex = m_iIndex;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSwitchNode);
//---------------------------------------------------------------------------
void NiSwitchNode::LoadBinary(NiStream& kStream)
{
    NiNode::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);
    NiStreamLoadBinary(kStream, m_iIndex);
}
//---------------------------------------------------------------------------
void NiSwitchNode::LinkObject(NiStream& kStream)
{
    NiNode::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiSwitchNode::RegisterStreamables(NiStream& kStream)
{
    return NiNode::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiSwitchNode::SaveBinary(NiStream& kStream)
{
    NiNode::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_uFlags);
    NiStreamSaveBinary(kStream, m_iIndex);
}
//---------------------------------------------------------------------------
bool NiSwitchNode::IsEqual(NiObject* pkObject)
{
    if (!NiNode::IsEqual(pkObject))
        return false;

    NiSwitchNode* pkSwitch = (NiSwitchNode*) pkObject;

    if (m_iIndex != pkSwitch->m_iIndex)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSwitchNode::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiNode::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiSwitchNode::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_iIndex", m_iIndex));
}
//---------------------------------------------------------------------------
