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

#include "NiMainPCH.h"
#include "NiDrawSceneUtility.h"
#include "NiRenderer.h"
#include "NiAccumulator.h"
#include "NiRenderObject.h"

//---------------------------------------------------------------------------
unsigned int NiDrawScene(NiCamera* pkCamera, NiAVObject* pkScene,
    NiCullingProcess& kCuller, NiVisibleArray* pkVisibleSet)
{
    // decide which visible set to use.  If there is none,
    // drawing cannot occur
    if (!pkVisibleSet)
    {
        pkVisibleSet = kCuller.GetVisibleSet();
        if (!pkVisibleSet)
        {
            return 0;
        }
    }
    NiCullScene(pkCamera, pkScene, kCuller, *pkVisibleSet);
    NiDrawVisibleArray(pkCamera, *pkVisibleSet);

    return pkVisibleSet->GetCount();
}
//---------------------------------------------------------------------------
unsigned int NiCullScene(NiCamera* pkCamera, NiAVObject* pkScene,
    NiCullingProcess& kCuller, NiVisibleArray& kVisibleSet, 
    bool bResetArray)
{
    // A renderer, camera, and scene are essential for drawing.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (!pkRenderer || !pkCamera || !pkScene)
        return 0;

    // Set up the renderer's camera data.
    pkRenderer->SetCameraData(pkCamera);

    // Construct the potentially visible set.
    unsigned int uiPreviousElementsInArray = 0;
    if (bResetArray)
    {
        kVisibleSet.RemoveAll();
    }
    else
    {
        uiPreviousElementsInArray = kVisibleSet.GetCount();
    }

    kCuller.Cull(pkCamera, pkScene, &kVisibleSet);
    return kVisibleSet.GetCount() - uiPreviousElementsInArray;
}
//---------------------------------------------------------------------------
unsigned int NiDrawVisibleArray(NiCamera* pkCamera, 
    NiVisibleArray& kVisibleSet)
{
    // A renderer, a camera, and a visible array are essential for drawing.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (!pkRenderer || !pkCamera)
        return 0;

    // Draw the potentially visible objects.  For now these are all
    // NiRenderObject objects.  Later, the design might change to support a
    // hierarchical PVS.
    NiAccumulatorPtr spSorter = pkRenderer->GetSorter();
    if (spSorter)
        spSorter->StartAccumulating(pkCamera);

    NiDrawVisibleArrayAppend(kVisibleSet);

    if (spSorter)
    {
        // Draw any objects that were accumulated during sorting.
        spSorter->FinishAccumulating();
    }

    return kVisibleSet.GetCount();
}
//---------------------------------------------------------------------------
unsigned int NiDrawVisibleArrayAppend(NiVisibleArray& kVisibleSet)
{
    // A renderer and a visible array are essential for drawing.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (!pkRenderer)
        return 0;

    // Draw the potentially visible objects.  For now these are all
    // NiRenderObject objects.  Later, the design might change to support a
    // hierarchical PVS.
    NiAccumulatorPtr spSorter = pkRenderer->GetSorter();
    if (spSorter)
    {
        // Sorted rendering is requested.
        spSorter->RegisterObjectArray(kVisibleSet);
    }
    else
    {
        const unsigned int uiQuantity = kVisibleSet.GetCount();
        unsigned int i;
        
        for (i = 0; i < uiQuantity; i++)
        {
            NiAVObject* pkAVObj = &kVisibleSet.GetAt(i);
            NIASSERT(NiIsKindOf(NiRenderObject, pkAVObj));
            reinterpret_cast<NiRenderObject*>(pkAVObj)->
                RenderImmediate(pkRenderer);
        }
    }

    return kVisibleSet.GetCount();
}
//---------------------------------------------------------------------------
