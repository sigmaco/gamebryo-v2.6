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

#if !defined(_WII)
#ifndef NISKINNINGKERNEL_H
#define NISKINNINGKERNEL_H

#include <NiUniversalTypes.h>
#include <NiMatrix3x4.h>

//---------------------------------------------------------------------------
template<bool bHasNormals, bool bHasBTs, NiUInt32 uiWeightsPerBone,
    typename TIndicies, typename TWeights, 
    typename TPositions, NiUInt32 uiPositionCompCount, 
    typename TNBTs = float, NiUInt32 uiNBTsCompCount = 3>
class NiSkinningKernelImpl
{  
public:
    NI_NOINLINE void Execute(NiSPWorkload& kWorkload)
    {
        // Inputs
        NiUInt16 uiInput = 1;
        NiMatrix3x4* pkBones = kWorkload.GetInput<NiMatrix3x4>(uiInput++);
        TWeights* pfWeights = kWorkload.GetInput<TWeights>(uiInput++);
        TIndicies* pIndicies = kWorkload.GetInput<TIndicies>(uiInput++);

        TPositions* pkBindPosePos = kWorkload.GetInput<TPositions>(uiInput++);
        TNBTs* pkBindPoseN = NULL;
        TNBTs* pkBindPoseB = NULL;
        TNBTs* pkBindPoseT = NULL;

        // Outputs
        NiUInt16 uiOutput = 0;
        TPositions* pkPositions = kWorkload.GetOutput<TPositions>(uiOutput++);
        TNBTs* pkOutputN = NULL; 
        TNBTs* pkOutputB = NULL;
        TNBTs* pkOutputT = NULL;

        // Optional normals (input bind pose and deformed output)
        bool bHasNormalsParam = bHasNormals;
        if (bHasNormalsParam)
        {
            pkBindPoseN = kWorkload.GetInput<TNBTs>(uiInput++);

            pkOutputN = kWorkload.GetOutput<TNBTs>(uiOutput++);
        }

        // Optional binormals and tangents (input bind poses and deformed 
        // output)
        bool bHasBTsParam = bHasBTs;
        if (bHasBTsParam)
        {
            pkBindPoseB = kWorkload.GetInput<TNBTs>(uiInput++);
            pkBindPoseT = kWorkload.GetInput<TNBTs>(uiInput++);

            pkOutputB = kWorkload.GetOutput<TNBTs>(uiOutput++);
            pkOutputT = kWorkload.GetOutput<TNBTs>(uiOutput++);
        }

        // Deform vertices
        NiUInt32 uiBlockCount = kWorkload.GetBlockCount();
        for (NiUInt32 ui = 0; ui < uiBlockCount; ui++)
        {

            float fWeight0 = *pfWeights++;
            float fWeight1 = *pfWeights++;
            float fWeight2 = *pfWeights++;
            float fWeight3 = (1.0f - fWeight0 - fWeight1 - fWeight2);
            NiUInt32 uiWeightsPerBoneParam = uiWeightsPerBone;
            if (uiWeightsPerBoneParam == 4)
                *pfWeights++;

            NiInt16 iBlendIndex0 = *pIndicies++;
            NiInt16 iBlendIndex1 = *pIndicies++;
            NiInt16 iBlendIndex2 = *pIndicies++;
            NiInt16 iBlendIndex3 = *pIndicies++;

            NiMatrix3x4 kSkinBone = pkBones[iBlendIndex0] * fWeight0;
            NiMatrix3x4 kBone1 = pkBones[iBlendIndex1] * fWeight1;
            NiMatrix3x4 kBone2 = pkBones[iBlendIndex2] * fWeight2;
            NiMatrix3x4 kBone3 = pkBones[iBlendIndex3] * fWeight3;

            kSkinBone += kBone1;
            kSkinBone += kBone2;
            kSkinBone += kBone3;

            NiUInt32 uiIndex0 = ui * uiPositionCompCount;
            NiUInt32 uiIndex1 = uiIndex0 + 1;
            NiUInt32 uiIndex2 = uiIndex0 + 2;

            NiPoint3 kPosition = kSkinBone * NiPoint3(
                pkBindPosePos[uiIndex0],
                pkBindPosePos[uiIndex1], 
                pkBindPosePos[uiIndex2]);
            pkPositions[uiIndex0] = kPosition.x;
            pkPositions[uiIndex1] = kPosition.y;
            pkPositions[uiIndex2] = kPosition.z;

            uiIndex0 = ui * uiNBTsCompCount;
            uiIndex1 = uiIndex0 + 1;
            uiIndex2 = uiIndex0 + 2;

            if (bHasNormalsParam)
            {
                NiPoint3 kNormal = kSkinBone.ScaleAndRotate(
                    NiPoint3(
                        pkBindPoseN[uiIndex0],
                        pkBindPoseN[uiIndex1], 
                        pkBindPoseN[uiIndex2]));
                kNormal.Unitize();
                pkOutputN[uiIndex0] = kNormal.x;
                pkOutputN[uiIndex1] = kNormal.y;
                pkOutputN[uiIndex2] = kNormal.z;
            }

            if (bHasBTsParam)
            {
                NiPoint3 kBiNormal = kSkinBone.ScaleAndRotate(
                    NiPoint3(
                        pkBindPoseB[uiIndex0],
                        pkBindPoseB[uiIndex1], 
                        pkBindPoseB[uiIndex2]));
                kBiNormal.Unitize();
                pkOutputB[uiIndex0] = kBiNormal.x;
                pkOutputB[uiIndex1] = kBiNormal.y;
                pkOutputB[uiIndex2] = kBiNormal.z;

                NiPoint3 kTangent = kSkinBone.ScaleAndRotate(
                    NiPoint3(
                        pkBindPoseT[uiIndex0],
                        pkBindPoseT[uiIndex1], 
                        pkBindPoseT[uiIndex2]));
                kTangent.Unitize();
                pkOutputT[uiIndex0] = kTangent.x;
                pkOutputT[uiIndex1] = kTangent.y;
                pkOutputT[uiIndex2] = kTangent.z;
            }
        }
    }
};
//---------------------------------------------------------------------------

#endif
#endif