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

#include "NiMeshPCH.h"
#include "NiCalculateBoneMatricesKernel.h"
#include <NiUniversalTypes.h>
#include <NiTransform.h>
#include <NiMatrix3x4.h>

#ifdef __SPU__
#define FetchTransform(ls, ea) cellDmaGet(ls, (uint64_t)ea, \
    sizeof(NiTransform), m_pkContext->dmaTag, 0,0); \
    cellDmaWaitTagStatusAll(1<<m_pkContext->dmaTag);
#else
#define FetchTransform(ls, ea) *ls = *ea;
#endif

//---------------------------------------------------------------------------
NiSPBeginKernelImpl(NiCalculateBoneMatricesKernel)
{
    // Inputs
    NiCBMKernelData* pkKernelData = kWorkload.GetInput<NiCBMKernelData>(0);
    NiTransform** ppkBoneWorld = kWorkload.GetInput<NiTransform*>(1);
    NiTransform* pkSkinToBone = kWorkload.GetInput<NiTransform>(2);

    // Outputs
    NiMatrix3x4* pkBoneMatrices = kWorkload.GetOutput<NiMatrix3x4>(0);
    
    NiUInt32 uiBlockCount = kWorkload.GetBlockCount();

#if defined(_WII)
    const NiTransform& kWorldToSkin = pkKernelData->m_kWorldToSkin;
    for (NiUInt32 ui = 0; ui < uiBlockCount; ui++)
    {
        // Compute bone matrix [S-B][B-W][W-S] (reversed)
        pkBoneMatrices[ui] = 
            kWorldToSkin * (*ppkBoneWorld[ui]) * pkSkinToBone[ui];
    }
#else
    NiTransform kBoneWorld;
    
    // Skinning - includes the multiplication by kWorldToSkin
    NiTransform kWorldToSkin = pkKernelData->m_kWorldToSkin;
    for (NiUInt32 ui = 0; ui < uiBlockCount; ui++)
    {
        // Compute bone matrix [S-B][B-W][W-S] (reversed)
        NiTransform kSkinToBone = pkSkinToBone[ui];
        FetchTransform(&kBoneWorld, ppkBoneWorld[ui]);
        NiTransform kBoneMatrix = kWorldToSkin * kBoneWorld * kSkinToBone;
        pkBoneMatrices[ui] = kBoneMatrix;
    }
#endif
}
NiSPEndKernelImpl(NiCalculateBoneMatricesKernel) 
                                 


