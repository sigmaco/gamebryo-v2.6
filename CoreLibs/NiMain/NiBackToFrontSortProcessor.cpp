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

#include "NiBackToFrontSortProcessor.h"
#include "NiRenderer.h"
#include "NiRenderObject.h"

NiImplementRTTI(NiBackToFrontSortProcessor, NiRenderListProcessor);

//---------------------------------------------------------------------------
NiBackToFrontSortProcessor::~NiBackToFrontSortProcessor()
{
    NiFree(m_pfDepths);
}
//---------------------------------------------------------------------------
void NiBackToFrontSortProcessor::PreRenderProcessList(
    const NiVisibleArray* pkInput, NiVisibleArray& kOutput, void*)
{
    // If the input array pointer is null, do nothing.
    if (!pkInput)
    {
        return;
    }

    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Get camera data from renderer.
    NiPoint3 kWorldLoc, kWorldDir, kWorldUp, kWorldRight;
    NiFrustum kFrustum;
    NiRect<float> kViewport;
    pkRenderer->GetCameraData(kWorldLoc, kWorldDir, kWorldUp, kWorldRight,
        kFrustum, kViewport);

    const unsigned int uiInputCount = pkInput->GetCount();

    // Initialize size of object depths array.
    if (m_uiAllocatedDepths < uiInputCount)
    {
        NiFree(m_pfDepths);
        m_pfDepths = NiAlloc(float, uiInputCount);
        m_uiAllocatedDepths = uiInputCount;
    }

    // Iterate over input geometry array.
    for (unsigned int ui = 0; ui < uiInputCount; ui++)
    {
        NiRenderObject& kGeometry = pkInput->GetAt(ui);

        // Add geometry to output array.
        kOutput.Add(kGeometry);

        // Compute and store depth from camera.
        m_pfDepths[ui] = ComputeDepth(kGeometry, kWorldDir);
    }

    // Sort output array by depth.
    SortObjectsByDepth(kOutput, 0, kOutput.GetCount() - 1);
}
//---------------------------------------------------------------------------
void NiBackToFrontSortProcessor::SortObjectsByDepth(
    NiVisibleArray& kArrayToSort, int l, int r)
{
    // This recursive function implements a quick sort of kArrayToSort. It is
    // taken directly from the function of the same name in
    // NiBackToFrontAccumulator except that it sorts back to front instead of
    // front to back.

    if (r > l)
    {
        int i = l - 1;
        int j = r + 1;
        float fPivot = ChoosePivot(l, r);

        for( ; ; )
        {
            do 
            {
                j--;
            } while (fPivot > m_pfDepths[j]);

            do
            {
                i++;
            } while (m_pfDepths[i] > fPivot);

            if (i < j)
            {
                // Swap array elements.
                NiRenderObject* pkTempObj = &kArrayToSort.GetAt(i);
                kArrayToSort.SetAt(i, kArrayToSort.GetAt(j));
                kArrayToSort.SetAt(j, *pkTempObj);

                float fTempDepth = m_pfDepths[i];
                m_pfDepths[i] = m_pfDepths[j];
                m_pfDepths[j] = fTempDepth;
            }
            else
            {
                break;
            }
        }

        if (j == r)
        {
            SortObjectsByDepth(kArrayToSort, l, j - 1);
        }
        else
        {
            SortObjectsByDepth(kArrayToSort, l, j);
            SortObjectsByDepth(kArrayToSort, j + 1, r);
        }
    }
}
//---------------------------------------------------------------------------
float NiBackToFrontSortProcessor::ChoosePivot(int l, int r) const
{
    // Check the first, middle, and last element. Choose the one which falls
    // between the other two. This has a good chance of discouraging 
    // quadratic behavior from qsort.
    // In the case when all three are equal, this code chooses the middle
    // element, which will prevent quadratic behavior for a list with 
    // all elements equal.

    int m = (l + r) >> 1;

    const float fDepth_l = m_pfDepths[l];
    const float fDepth_r = m_pfDepths[r];
    const float fDepth_m = m_pfDepths[m];

    if (fDepth_l > fDepth_m)
    {
        if (fDepth_m > fDepth_r)
        {
            return fDepth_m;
        }
        else
        {
            if (fDepth_l > fDepth_r)
            {
                return fDepth_r;
            }
            else
            {
                return fDepth_l;
            }
        }
    }
    else
    {
        if (fDepth_l > fDepth_r)
        {
            return fDepth_l;
        }
        else
        {
            if (fDepth_m > fDepth_r)
            {
                return fDepth_r;
            }
            else
            {
                return fDepth_m;
            }
        }
    }
}
//---------------------------------------------------------------------------
