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

#include "NiSortAdjustNode.h"
#include "NiRenderObject.h"

NiImplementRTTI(NiSortAdjustNode, NiNode);

//---------------------------------------------------------------------------
NiSortAdjustNode::NiSortAdjustNode()
{
    m_eSortingMode = SORTING_INHERIT;
}
//---------------------------------------------------------------------------
NiSortAdjustNode::~NiSortAdjustNode()
{
    /* */
}
//---------------------------------------------------------------------------
void NiSortAdjustNode::OnVisible(NiCullingProcess& kCuller)
{
    // determine whether we are clearing or setting the no sort flag
    bool bUseSorting = (m_eSortingMode != SORTING_OFF);

    // Store the index in which the next visible object will be stored
    unsigned int uiNextIndexPre = kCuller.GetVisibleSet()->GetCount();

    // cull normally
    NiNode::OnVisible(kCuller);

    NiVisibleArray* pkVisible = kCuller.GetVisibleSet();

    // Store the index in which the next visible object will be stored
    unsigned int uiNextIndexPost = pkVisible->GetCount();

    // turn on/off sorting on each object that was added while culling the
    // subtree under these items
    unsigned int i;
    for (i = uiNextIndexPre; i < uiNextIndexPost; i++)
        pkVisible->GetAt(i).SetSortObject(bUseSorting);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiSortAdjustNode);
//---------------------------------------------------------------------------
void NiSortAdjustNode::CopyMembers(NiSortAdjustNode* pDest,
    NiCloningProcess& kCloning)
{
    NiNode::CopyMembers(pDest, kCloning);
    
    pDest->m_eSortingMode = m_eSortingMode;
}
//---------------------------------------------------------------------------
void NiSortAdjustNode::ProcessClone(NiCloningProcess& kCloning)
{
    NiNode::ProcessClone(kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSortAdjustNode);
//---------------------------------------------------------------------------
void NiSortAdjustNode::LoadBinary(NiStream& stream)
{
    NiNode::LoadBinary(stream);

    NiStreamLoadEnum(stream, m_eSortingMode);

// --- Begin NIF conversion code
    // Older versions supported SUBSORT, which we now remap to INHERIT
    if (m_eSortingMode >= SORTING_MAX)
        m_eSortingMode = SORTING_INHERIT;

    // The accumulator has been removed, but older NIFs will still include it
    if (stream.GetFileVersion() < NiStream::GetVersion(20, 0, 0, 4))
    {
        stream.ReadLinkID();    // m_spAccum
    }
// --- End NIF conversion code
}
//---------------------------------------------------------------------------
void NiSortAdjustNode::LinkObject(NiStream& stream)
{
    NiNode::LinkObject(stream);

// --- Begin NIF conversion code
    // The accumulator has been removed, but older NIFs will still include it
    if (stream.GetFileVersion() < NiStream::GetVersion(20, 0, 0, 4))
    {
        // Old version of code.  This throws a warning on PS3, so it
        // was changed to not declare the local variable.  If there are
        // side effects, it should be changed back.
        //NiAccumulator* pkAccum 
        //    = (NiAccumulator*) stream.GetObjectFromLinkID();
        
        stream.GetObjectFromLinkID();
    }
// --- End NIF conversion code
}
//---------------------------------------------------------------------------
bool NiSortAdjustNode::RegisterStreamables(NiStream& stream)
{
    return NiNode::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiSortAdjustNode::SaveBinary(NiStream& stream)
{
    NiNode::SaveBinary(stream);

    NiStreamSaveEnum(stream, m_eSortingMode);
}
//---------------------------------------------------------------------------
bool NiSortAdjustNode::IsEqual(NiObject* pObject)
{
    if ( !NiNode::IsEqual(pObject) )
        return false;

    NiSortAdjustNode* pSortNode = (NiSortAdjustNode*) pObject;

    if(m_eSortingMode != pSortNode->m_eSortingMode)
        return false;

    return true;
}
//---------------------------------------------------------------------------

