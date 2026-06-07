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

#include "NiBackToFrontAccumulator.h"
#include "NiCamera.h"
#include "NiRenderer.h"

NiImplementRTTI(NiBackToFrontAccumulator, NiAccumulator);

//---------------------------------------------------------------------------
NiBackToFrontAccumulator::NiBackToFrontAccumulator() :
    m_iNumItems(0),
    m_iMaxItems(0),
    m_ppkItems(NULL),
    m_pfDepths(NULL)
{ /* */ }
//---------------------------------------------------------------------------
NiBackToFrontAccumulator::~NiBackToFrontAccumulator()
{
    NiFree(m_ppkItems);
    NiFree(m_pfDepths);

    // DO NOT delete the NiGeometries!
    while (m_kItems.GetSize())
        m_kItems.RemoveHead();
}
//---------------------------------------------------------------------------
void NiBackToFrontAccumulator::RegisterObjectArray(NiVisibleArray& kArray)
{
    const unsigned int uiQuantity = kArray.GetCount();
    for (unsigned int i = 0; i < uiQuantity; i++)
    {
        NiRenderObject* pkObject = &kArray.GetAt(i);
        // All geometry gets added to the list
        m_kItems.AddTail(pkObject);
    }
}
//---------------------------------------------------------------------------
void NiBackToFrontAccumulator::FinishAccumulating()
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    if (!pkRenderer)
        return;

    Sort();
    m_iCurrItem = m_iNumItems;

    NiAVObject* pkItem =  m_iCurrItem ? m_ppkItems[--m_iCurrItem] : 0;

    while (pkItem)
    {
        NIASSERT(NiIsKindOf(NiRenderObject, pkItem));
        reinterpret_cast<NiRenderObject*>(pkItem)->RenderImmediate(pkRenderer);
        pkItem = m_iCurrItem ? m_ppkItems[--m_iCurrItem] : 0;
    }

    // DO NOT delete the NiGeometries!
    while (m_kItems.GetSize())
        m_kItems.RemoveHead();

    NiAccumulator::FinishAccumulating();
}
//---------------------------------------------------------------------------
void NiBackToFrontAccumulator::Sort()
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
        m_pfDepths = NiAlloc(float, m_iMaxItems);
        NIASSERT(m_pfDepths);
    }

    NiTListIterator kPos = m_kItems.GetHeadPos();

    NiPoint3 kViewDir = m_pkCamera->GetWorldDirection();

    for (int i = 0; i < m_iNumItems; i++)
    {
        m_ppkItems[i] = m_kItems.GetNext(kPos);
        m_pfDepths[i] = m_ppkItems[i]->GetWorldBound().GetCenter() * kViewDir;
    }

    SortObjectsByDepth(0, m_iNumItems - 1);
}
//---------------------------------------------------------------------------
void NiBackToFrontAccumulator::SortObjectsByDepth(int l, int r)
{
    if (r > l)
    {
        int i, j;

        i = l - 1;
        j = r + 1;
        float fPivot = ChoosePivot(l, r);

        for (;;)
        {
            do 
            {
                j--;
            } while (fPivot < m_pfDepths[j]);

            do
            {
                i++;
            } while (m_pfDepths[i] < fPivot);

            if (i < j)
            {
                NiRenderObject* pkObjTemp = m_ppkItems[i];
                m_ppkItems[i] = m_ppkItems[j];
                m_ppkItems[j] = pkObjTemp;
                float fTemp = m_pfDepths[i];
                m_pfDepths[i] = m_pfDepths[j];
                m_pfDepths[j] = fTemp;
            }
            else
            {
                break;
            }
        }

        if (j == r)
        {
            SortObjectsByDepth(l, j - 1);
        }
        else
        {
            SortObjectsByDepth(l, j);
            SortObjectsByDepth(j + 1, r);
        }
    }
}
//---------------------------------------------------------------------------
float NiBackToFrontAccumulator::ChoosePivot(int l, int r) const
{
    // Check the first, middle, and last element. Choose the one which falls
    // between the other two. This has a good chance of discouraging 
    // quadratic behavior from qsort.
    // In the case when all three are equal, this code chooses the middle
    // element, which will prevent quadratic behavior for a list with 
    // all elements equal.

    int m = (l + r) >> 1;

    if (m_pfDepths[l] < m_pfDepths[m])
    {
        if (m_pfDepths[m] < m_pfDepths[r])
        {
            return m_pfDepths[m];
        }
        else
        {
            if (m_pfDepths[l] < m_pfDepths[r])
                return m_pfDepths[r];
            else
                return m_pfDepths[l];
        }
    }
    else
    {
        if (m_pfDepths[l] < m_pfDepths[r])
        {
            return m_pfDepths[l];
        }
        else
        {
            if (m_pfDepths[m] < m_pfDepths[r])
                return m_pfDepths[r];
            else
                return m_pfDepths[m];
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBackToFrontAccumulator);
//---------------------------------------------------------------------------
void NiBackToFrontAccumulator::CopyMembers(
    NiBackToFrontAccumulator* pkDest,
    NiCloningProcess& kCloning)
{
    NiAccumulator::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBackToFrontAccumulator);
//---------------------------------------------------------------------------
void NiBackToFrontAccumulator::LoadBinary(NiStream& kStream)
{
    NiAccumulator::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiBackToFrontAccumulator::LinkObject(NiStream& kStream)
{
    NiAccumulator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBackToFrontAccumulator::RegisterStreamables(NiStream& kStream)
{
    return NiAccumulator::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBackToFrontAccumulator::SaveBinary(NiStream& kStream)
{
    NiAccumulator::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiBackToFrontAccumulator::IsEqual(NiObject* pkObject)
{
    if (!NiAccumulator::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
