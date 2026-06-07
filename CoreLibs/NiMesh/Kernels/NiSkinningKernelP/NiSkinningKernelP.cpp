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
#include "NiSkinningKernelP.h"
#include "NiSkinningKernel.h"
#include "NiSkinningKernelData.h"

//---------------------------------------------------------------------------
NiSPBeginKernelImpl(NiSkinningKernelP)
{
    NiSkinningKernelData* pkKernelData = 
        kWorkload.GetInput<NiSkinningKernelData>(0);

    switch(pkKernelData->GetCombination())
    {
    case SKDC_3_08_32_16_4_32:
        {
            NiSkinningKernelImpl<false, false, 3, NiUInt8, float, NiFloat16, 
                4, float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_3_16_32_16_4_32:
        {
            NiSkinningKernelImpl<false, false, 3, NiInt16, float, NiFloat16, 4,
                float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_3_08_32_32_32:
        {
            NiSkinningKernelImpl<false, false, 3, NiUInt8, float,  float, 3, 
                float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_3_16_32_32_32:
        {
            NiSkinningKernelImpl<false, false, 3, NiInt16, float,  float, 3, 
                float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_4_08_32_16_4_32:
        {
            NiSkinningKernelImpl<false, false, 4, NiUInt8, float, NiFloat16, 4,
                float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_4_08_16_16_4_32:
        {
            NiSkinningKernelImpl<false, false, 4, NiUInt8, NiFloat16, 
                NiFloat16, 4, float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_4_16_32_16_4_32:
        {
            NiSkinningKernelImpl<false, false, 4, NiInt16, float, NiFloat16, 4,
                float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_4_16_16_16_4_32:
        {
            NiSkinningKernelImpl<false, false, 4, NiInt16, NiFloat16, 
                NiFloat16, 4, float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_4_08_32_32_32:
        {
            NiSkinningKernelImpl<false, false, 4, NiUInt8, float,  float, 3,
                float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_4_08_16_32_32:
        {
            NiSkinningKernelImpl<false, false, 4, NiUInt8, NiFloat16, float,
                3, float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_4_16_32_32_32:
        {
            NiSkinningKernelImpl<false, false, 4, NiInt16, float, float, 3,
                float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_4_16_16_32_32:
        {
            NiSkinningKernelImpl<false, false, 4, NiInt16, NiFloat16, float,
                3, float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
#if _PS3
    case SKDC_3_08_32_16_3_32:
        {
            NiSkinningKernelImpl<false, false, 3, NiUInt8, float, NiFloat16, 3,
                float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_3_16_32_16_3_32:
        {
            NiSkinningKernelImpl<false, false, 3, NiInt16, float, NiFloat16, 3,
                float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_4_08_32_16_3_32:
        {
            NiSkinningKernelImpl<false, false, 4, NiUInt8, float, NiFloat16, 3,
                float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_4_08_16_16_3_32:
        {
            NiSkinningKernelImpl<false, false, 4, NiUInt8, NiFloat16, 
                NiFloat16, 3, float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_4_16_32_16_3_32:
        {
            NiSkinningKernelImpl<false, false, 4, NiInt16, float, NiFloat16, 3,
                float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
    case SKDC_4_16_16_16_3_32:
        {
            NiSkinningKernelImpl<false, false, 4, NiInt16, NiFloat16, 
                NiFloat16, 3, float, 3> kFunctor;
            kFunctor.Execute(kWorkload);
        }
        break;
#endif

    default:
        KernelDebugOutput("Unsupported combination of inputs for skinning "
            "deformation kernel (only position, position & normal, or "
            "position & NBT are allowed)\n");
    }
}
NiSPEndKernelImpl(NiSkinningKernelP)
//---------------------------------------------------------------------------
