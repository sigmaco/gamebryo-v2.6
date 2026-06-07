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

#include "NiAlphaAccumulator.h"
#include "NiCamera.h"
#include "NiRenderer.h"
#include "NiVisibleArray.h"

NiImplementRTTI(NiAlphaAccumulator, NiBackToFrontAccumulator);

//---------------------------------------------------------------------------
NiAlphaAccumulator::NiAlphaAccumulator() :
    m_bObserveNoSortHint(true),
    m_bSortByClosestPoint(false)
{ /* */ }
//---------------------------------------------------------------------------
NiAlphaAccumulator::~NiAlphaAccumulator ()
{ /* */ }
//---------------------------------------------------------------------------
void NiAlphaAccumulator::RegisterObjectArray(NiVisibleArray& kArray)
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();

    const unsigned int uiQuantity = kArray.GetCount();
    for (unsigned int i = 0; i < uiQuantity; i++)
    {
        NiRenderObject& kObject = kArray.GetAt(i);

        const NiPropertyState* pkState = kObject.GetPropertyState();

        NIASSERT(pkState);

        const NiAlphaProperty *pkAlpha = pkState->GetAlpha();

        // Every property state should have a valid alpha property
        NIASSERT(pkAlpha);

        if (pkAlpha->GetAlphaBlending() && 
            !(m_bObserveNoSortHint && pkAlpha->GetNoSorter()) && 
            kObject.GetSortObject())
        {
            m_kItems.AddTail(&kObject);
        }
        else
        {
            NiAVObject* pkAVObj = &kObject;
            if (NiIsKindOf(NiRenderObject, pkAVObj))
            {
                kObject.RenderImmediate(pkRenderer);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiAlphaAccumulator::Sort()
{
    m_iNumItems = m_kItems.GetSize();
    if (!m_iNumItems)
        return;

    if (m_iNumItems > m_iMaxItems)
    {
        NiFree(m_ppkItems);

        m_iMaxItems = m_iNumItems;

        m_ppkItems = NiAlloc(NiRenderObject*, m_iMaxItems);
        NIASSERT(m_ppkItems);

        NiFree(m_pfDepths);
        m_pfDepths =  NiAlloc(float, m_iMaxItems);
        NIASSERT(m_pfDepths);
    }

    NiTListIterator pkPos = m_kItems.GetHeadPos();

    NiPoint3 kViewDir = m_pkCamera->GetWorldDirection();

    if (m_bSortByClosestPoint)
    {
        for (int i = 0; i < m_iNumItems; i++)
        {
            m_ppkItems[i] = m_kItems.GetNext(pkPos);
            m_pfDepths[i] = 
                m_ppkItems[i]->GetWorldBound().GetCenter() * kViewDir - 
                m_ppkItems[i]->GetWorldBound().GetRadius();
        }
    }
    else
    {
        for (int i = 0; i < m_iNumItems; i++)
        {
            m_ppkItems[i] = m_kItems.GetNext(pkPos);
            m_pfDepths[i] = 
                m_ppkItems[i]->GetWorldBound().GetCenter() * kViewDir;
        }
    }

    SortObjectsByDepth(0, m_iNumItems - 1);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiAlphaAccumulator);
//---------------------------------------------------------------------------
void NiAlphaAccumulator::CopyMembers(NiAlphaAccumulator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBackToFrontAccumulator::CopyMembers(pkDest, kCloning);

    pkDest->m_bObserveNoSortHint = m_bObserveNoSortHint;
    pkDest->m_bSortByClosestPoint = m_bSortByClosestPoint;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiAlphaAccumulator);
//---------------------------------------------------------------------------
void NiAlphaAccumulator::LoadBinary(NiStream& kStream)
{
    NiBackToFrontAccumulator::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiAlphaAccumulator::LinkObject(NiStream& kStream)
{
    NiBackToFrontAccumulator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiAlphaAccumulator::RegisterStreamables(NiStream& kStream)
{
    return NiBackToFrontAccumulator::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiAlphaAccumulator::SaveBinary(NiStream& kStream)
{
    NiBackToFrontAccumulator::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiAlphaAccumulator::IsEqual(NiObject* pkObject)
{
    if (!NiBackToFrontAccumulator::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
