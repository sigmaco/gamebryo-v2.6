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

#include "NiInstanceCullingKernel.h"
#include <NiUniversalTypes.h>
#include <NiPoint4.h>
#include <NiPoint3.h>
#include <NiPlane.h>
#include <NiBound.h>
#include <NiTransform.h>

#ifdef  __SPU__
#include <cell/atomic.h>
#endif

//---------------------------------------------------------------------------
NiSPBeginKernelImpl(NiInstanceCullingKernel)
{

#ifdef __SPU__
    NiPoint4* pkOutVerts = kWorkload.GetOutput<NiPoint4>(0);
#else
    NiPoint4* pkDummyOutData = kWorkload.GetOutput<NiPoint4>(1);
#endif

    NiPoint4* pfInTrans = kWorkload.GetInput<NiPoint4>(0);
    NiBound* pkModelBound = kWorkload.GetInput<NiBound>(1);
    NiPlane* pkPlanes = kWorkload.GetInput<NiPlane>(2);
    float* afScale = kWorkload.GetInput<float>(4);

    NiBound kWorldBound;
    NiTransform kWorldTransform;
    bool bVisible = false;
    NiUInt32 uiVisibleCount = 0;

    NiUInt32 uiBlockCount = kWorkload.GetBlockCount(); 

    kWorldTransform.m_fScale = 1.0f;

    for (NiUInt32 uiBlock = 0; uiBlock < uiBlockCount; uiBlock++)
    {
        bVisible = true;

        // Setup world transform that will be applied to the model bound.
        kWorldTransform.m_fScale = afScale[uiBlock];

        kWorldTransform.m_Translate =
            NiPoint3(pfInTrans[uiBlock * 3].W(),
            pfInTrans[(uiBlock * 3) + 1].W(), 
            pfInTrans[(uiBlock * 3) + 2].W());

        kWorldTransform.m_Rotate.SetCol(0, 
            pfInTrans[uiBlock*3].X(),
            pfInTrans[uiBlock*3].Y(),
            pfInTrans[uiBlock*3].Z());

        kWorldTransform.m_Rotate.SetCol(1, 
            pfInTrans[(uiBlock*3)+1].X(),
            pfInTrans[(uiBlock*3)+1].Y(),
            pfInTrans[(uiBlock*3)+1].Z());

        kWorldTransform.m_Rotate.SetCol(2, 
            pfInTrans[(uiBlock*3)+2].X(),
            pfInTrans[(uiBlock*3)+2].Y(),
            pfInTrans[(uiBlock*3)+2].Z());

        // Since we will manually apply the scale to kWorldBound via
        // NiBound::Update() we need to remove the scale embedded in the 
        // rotation matrix.
        kWorldTransform.m_Rotate = kWorldTransform.m_Rotate * 
            (1 / (kWorldTransform.m_fScale + FLT_EPSILON));

        kWorldBound.Update(*pkModelBound, kWorldTransform);

        // Perform visibility culling
        for (NiUInt32 uiPlane = 0; uiPlane < 6; uiPlane++)
        {
            if (pkPlanes[uiPlane].Distance(kWorldBound.GetCenter()) <=
                -kWorldBound.GetRadius())
            {
                bVisible = false;
                break;
            }
        }


        if (bVisible)            
        {
#ifdef __SPU__
            pkOutVerts[ uiVisibleCount * 3] = 
                pfInTrans[uiBlock * 3];
            pkOutVerts[(uiVisibleCount * 3) + 1] = 
                pfInTrans[(uiBlock * 3) + 1];
            pkOutVerts[(uiVisibleCount * 3) + 2] = 
                pfInTrans[(uiBlock * 3) + 2];
#else
            pkDummyOutData[ uiVisibleCount * 3] = 
                pfInTrans[uiBlock * 3];
            pkDummyOutData[(uiVisibleCount * 3) + 1] = 
                pfInTrans[(uiBlock * 3) + 1];
            pkDummyOutData[(uiVisibleCount * 3) + 2] = 
                pfInTrans[(uiBlock * 3) + 2];
#endif
            uiVisibleCount++;
        }

    }


#ifdef __SPU__
    NiUInt32 TempBuff[128/sizeof(uint32_t)] NI_DATA_ALIGNMENT(128);
    NiUInt32 uiOldVisCount = 
        cellAtomicAdd32(TempBuff, 
            (uint64_t)(kWorkload.m_pkInputsOutputs[3].m_uiBaseAddress),
            uiVisibleCount);

    // Control what data will be DMA transfered back to main memory.
    // This only works because the stride is a multiple of 16, otherwise
    // padding would have to be taken into account.
    kWorkload.m_pkInputsOutputs[kWorkload.m_uiInputCount].m_uiDataOffset =
        uiOldVisCount *
        kWorkload.m_pkInputsOutputs[kWorkload.m_uiInputCount].m_uiStride;
    kWorkload.m_uiBlockCount = uiVisibleCount;
#endif


#ifdef _XENON
    // InterlockedExchangeAdd does not create a memory barrier on 360. So 
    // we manually insert one via the light weight sync.
    __lwsync();
#endif

#if defined(WIN32) || defined(_XENON)
    NiUInt32 uiOldVisCount = 
        InterlockedExchangeAdd((LONG*)kWorkload.GetInputBase<NiUInt32>(3),
        uiVisibleCount);

    // Manually copy over the visible transforms into the correct indicies in 
    // the output array.
    NiPoint4* pkOutData = kWorkload.GetOutputBase<NiPoint4>(0) +
        (uiOldVisCount * 3);
    NiMemcpy(pkOutData, pkDummyOutData, sizeof(NiPoint4) * 3 * uiVisibleCount);
#endif

}
NiSPEndKernelImpl(NiInstanceCullingKernel)

