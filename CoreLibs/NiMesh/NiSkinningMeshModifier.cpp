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
#include "NiMeshPCH.h"

#include "NiSkinningMeshModifier.h"
#include <NiRenderObject.h>
#include <NiRenderObjectMaterialOption.h>
#include "NiMesh.h"
#include "NiCommonSemantics.h"
#include "NiDataStreamElementLock.h"
#include "NiSPWorkflow.h"
#include "NiCalculateBoneMatricesKernel.h"
#include <NiCloningProcess.h>
#include "NiMeshUtilities.h"
#include "NiTSimpleArray.h"
#include "NiUpdateProcess.h"
#include <NiNode.h>
#include <NiFloat16.h>

#if defined(_WII)
#include "NiWiiSkinningKernelP.h"
#include "NiWiiSkinningKernelPN.h"
#include "NiWiiSkinningKernelPNBT.h"
#else
#include "NiSkinningKernelP.h"
#include "NiSkinningKernelPN16.h"
#include "NiSkinningKernelPN32.h"
#include "NiSkinningKernelPNBT16.h"
#include "NiSkinningKernelPNBT32.h"
#endif

NiImplementRTTI(NiSkinningMeshModifier, NiMeshModifier);

//---------------------------------------------------------------------------
void NiSkinningMeshModifier::Initialize(NiUInt32 uiBoneCount)
{
    m_uiBoneCount = uiBoneCount;

    // Allocate Bones
    m_apkBones = NiAlloc(NiAVObject*, m_uiBoneCount);
    NIASSERT(m_apkBones);

    // Allocate transforms
    m_pkSkinToBoneTransforms = NiAlloc(NiTransform, m_uiBoneCount);
    NIASSERT(m_pkSkinToBoneTransforms);
}
//---------------------------------------------------------------------------
// Should not be called after Attach() is called
void NiSkinningMeshModifier::Resize(NiUInt32 uiNewBoneCount)
{
    NiFree(m_apkBones);
    NiFree(m_pkSkinToBoneTransforms);

    bool bHasBounds = GetBit(RECOMPUTE_BOUNDS);
    
    DisableDynamicBounds();
    Initialize(uiNewBoneCount);
    if (bHasBounds)
        EnableDynamicBounds();
}
//---------------------------------------------------------------------------
NiSkinningMeshModifier::NiSkinningMeshModifier(NiUInt32 uiBoneCount) :
    m_uFlags(0),
    m_uiBoneCount(0),
    m_pkRootBoneParent(NULL),
    m_apkBones(NULL),
    m_ppkBoneWorlds(NULL),
    m_pkSkinToBoneTransforms(NULL),
    m_pkBoneMatrices(NULL),
    m_pkBoneBounds(NULL),
    m_bBonesDirty(false), 
    m_spDeformTask(0),
    m_spDeformWorkflow(0),
    m_pkCBMKernel(NULL),
    m_spCBMTask(0),
    m_spCBMWorkflow(0),
    m_pkCBMKernelData(NULL),
    m_fUpdateTime(-1.0f)
{
    // Allocate memory arrays for bones, transforms, etc...
    Initialize(uiBoneCount);

    // Initialize flags
    SetBit(false, USE_SOFTWARE_SKINNING);
    SetBit(false, RECOMPUTE_BOUNDS);
}
//---------------------------------------------------------------------------
NiSkinningMeshModifier::NiSkinningMeshModifier() :
    m_uFlags(0),
    m_uiBoneCount(0),
    m_pkRootBoneParent(NULL),
    m_apkBones(NULL),
    m_ppkBoneWorlds(NULL),
    m_pkSkinToBoneTransforms(NULL),
    m_pkBoneMatrices(NULL),
    m_pkBoneBounds(NULL),
    m_bBonesDirty(false), 
    m_spDeformTask(0),
    m_spDeformWorkflow(0),
    m_pkCBMKernel(NULL),
    m_spCBMTask(0),
    m_spCBMWorkflow(0),
    m_pkCBMKernelData(NULL),
    m_fUpdateTime(-1.0f)
{
}
//---------------------------------------------------------------------------
NiSkinningMeshModifier::~NiSkinningMeshModifier()
{
    // Release the tasks and workflows before destroying the kernels because
    // the tasks hold references to the kernels.
    m_spDeformTask = 0;
    m_spCBMTask = 0;
    m_spDeformWorkflow = 0;
    m_spCBMWorkflow = 0;

    NiFree(m_pkCBMKernelData);
    NiDelete m_pkCBMKernel;
    NiFree(m_pkBoneMatrices);
    NiFree(m_apkBones);
    NiFree(m_ppkBoneWorlds);
    NiFree(m_pkSkinToBoneTransforms);

    DisableDynamicBounds();
}
//---------------------------------------------------------------------------
void NiSkinningMeshModifier::GetRootBones(
    NiTPointerList<NiAVObject*>& kRootBones) const
{
    // Iterate through the bones and find the bones that their parent is not
    // a bone.
    for (NiUInt32 ui0 = 0; ui0 < m_uiBoneCount; ui0++)
    {
        bool bFound = false;
        NiAVObject* pkParent0 = m_apkBones[ui0]->GetParent();
        for (NiUInt32 ui1 = 0; ui1 < m_uiBoneCount; ui1++)
        {
            if (pkParent0 == m_apkBones[ui1])
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            kRootBones.AddHead(m_apkBones[ui0]);
        }
    }
}
//---------------------------------------------------------------------------
bool NiSkinningMeshModifier::RequiresMaterialOption(
    const NiFixedString& kMaterialOption, bool& bResult) const
{
    if (kMaterialOption == NiRenderObjectMaterialOption::TRANSFORM_SKINNED())
    {
        bResult = !GetBit(USE_SOFTWARE_SKINNING);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiSkinningMeshModifier::AreRequirementsMet(NiMesh* pkMesh,
    NiSystemDesc::RendererID ePlatform) const
{
    bool bInvalid = false;

    NiTPrimitiveArray<NiDataStreamElement::Format> kFormats;

    if (GetBit(USE_SOFTWARE_SKINNING))
    {
        if (ePlatform == NiSystemDesc::RENDERER_WII)
        {
            // If the bind pose position and normal are in the same stream then the
            // destination position and normal must also be in the same stream for 
            // the Wii skinning kernels.
            NiDataStreamRef* pkPosBP = pkMesh->FindStreamRef(NiCommonSemantics::POSITION_BP());
            NiDataStreamRef* pkNrmBP = pkMesh->FindStreamRef(NiCommonSemantics::NORMAL_BP());
            NiDataStreamRef* pkPosDst = pkMesh->FindStreamRef(NiCommonSemantics::POSITION());
            NiDataStreamRef* pkNrmDst = pkMesh->FindStreamRef(NiCommonSemantics::NORMAL());

            if ((pkPosBP == pkNrmBP) != (pkPosDst == pkNrmDst))
            {
                bInvalid |= true;
                NILOG("NiSkinningMeshModifier(Software) requires that the POSITION "
                    "and NORMAL streams be interleaved if the POSITION_BP and "
                    "NORMAL_BP streams are interleaved (and vice versa) on "
                    "mesh (pointer: 0x%X, name: %s).\n",
                    pkMesh, 
                    (const char*) pkMesh->GetName());
            }
        }
        else
        {
            // On platforms other than Wii, PNBT and PNBT_BP must all be in their own stream.
            NiDataStreamRef* pkPosBP = 
                pkMesh->FindStreamRef(NiCommonSemantics::POSITION_BP());
            NiDataStreamRef* pkNrmBP = 
                pkMesh->FindStreamRef(NiCommonSemantics::NORMAL_BP());
            NiDataStreamRef* pkBinormBP = 
                pkMesh->FindStreamRef(NiCommonSemantics::BINORMAL_BP());
            NiDataStreamRef* pkTanBP = 
                pkMesh->FindStreamRef(NiCommonSemantics::TANGENT_BP());
            NiDataStreamRef* pkPosDst = 
                pkMesh->FindStreamRef(NiCommonSemantics::POSITION());
            NiDataStreamRef* pkNrmDst = 
                pkMesh->FindStreamRef(NiCommonSemantics::NORMAL());
            NiDataStreamRef* pkBinormDst = 
                pkMesh->FindStreamRef(NiCommonSemantics::BINORMAL());
            NiDataStreamRef* pkTanDst = 
                pkMesh->FindStreamRef(NiCommonSemantics::TANGENT());

            if (pkPosBP && pkPosBP->GetElementDescCount() != 1)
            {
                bInvalid |= true;
                NILOG("NiSkinningMeshModifier(Software) requires that the "
                    "POSITION_BP element be in its own data stream on mesh "
                    "(pointer: 0x%X, name: %s).\n",
                    pkMesh, 
                    (const char*) pkMesh->GetName());
            }

            if (pkNrmBP && pkNrmBP->GetElementDescCount() != 1)
            {
                bInvalid |= true;
                NILOG("NiSkinningMeshModifier(Software) requires that the "
                    "NORMAL_BP element be in its own data stream on mesh "
                    "(pointer: 0x%X, name: %s).\n",
                    pkMesh, 
                    (const char*) pkMesh->GetName());
            }

            if (pkBinormBP && pkBinormBP->GetElementDescCount() != 1)
            {
                bInvalid |= true;
                NILOG("NiSkinningMeshModifier(Software) requires that the "
                    "BINORMAL_BP element be in its own data stream on mesh "
                    "(pointer: 0x%X, name: %s).\n",
                    pkMesh, 
                    (const char*) pkMesh->GetName());
            }

            if (pkTanBP && pkTanBP->GetElementDescCount() != 1)
            {
                bInvalid |= true;
                NILOG("NiSkinningMeshModifier(Software) requires that the "
                    "TANGENT_BP element be in its own data stream on mesh "
                    "(pointer: 0x%X, name: %s).\n",
                    pkMesh, 
                    (const char*) pkMesh->GetName());
            }

            if (pkPosDst && pkPosDst->GetElementDescCount() != 1)
            {
                bInvalid |= true;
                NILOG("NiSkinningMeshModifier(Software) requires that the "
                    "POSITION element be in its own data stream on mesh "
                    "(pointer: 0x%X, name: %s).\n",
                    pkMesh, 
                    (const char*) pkMesh->GetName());
            }

            if (pkNrmDst && pkNrmDst->GetElementDescCount() != 1)
            {
                bInvalid |= true;
                NILOG("NiSkinningMeshModifier(Software) requires that the "
                    "NORMAL element be in its own data stream on mesh "
                    "(pointer: 0x%X, name: %s).\n",
                    pkMesh, 
                    (const char*) pkMesh->GetName());
            }

            if (pkBinormDst && pkBinormDst->GetElementDescCount() != 1)
            {
                bInvalid |= true;
                NILOG("NiSkinningMeshModifier(Software) requires that the "
                    "BINORMAL element be in its own data stream on mesh "
                    "(pointer: 0x%X, name: %s).\n",
                    pkMesh, 
                    (const char*) pkMesh->GetName());
            }

            if (pkTanDst && pkTanDst->GetElementDescCount() != 1)
            {
                bInvalid |= true;
                NILOG("NiSkinningMeshModifier(Software) requires that the "
                    "TANGENT element be in its own data stream on mesh "
                    "(pointer: 0x%X, name: %s).\n",
                    pkMesh, 
                    (const char*) pkMesh->GetName());
            }
        }

        // Blend indices and blend weights must be in their own stream for software skinning.
        NiDataStreamRef* pkBlendIndicesRef = 
            pkMesh->FindStreamRef(NiCommonSemantics::BLENDINDICES());
        if (pkBlendIndicesRef && pkBlendIndicesRef->GetElementDescCount() != 1)
        {
            bInvalid |= true;
            NILOG("NiSkinningMeshModifier(Software) requires that the "
                "BLENDINDICES element be in its own data stream on mesh "
                "(pointer: 0x%X, name: %s).\n",
                pkMesh, 
                (const char*) pkMesh->GetName());
        }

        NiDataStreamRef* pkBlendWeightsRef = 
            pkMesh->FindStreamRef(NiCommonSemantics::BLENDWEIGHT());
        if (pkBlendWeightsRef && pkBlendWeightsRef->GetElementDescCount() != 1)
        {
            bInvalid |= true;
            NILOG("NiSkinningMeshModifier(Software) requires that the "
                "BLENDWEIGHT element be in its own data stream on mesh "
                "(pointer: 0x%X, name: %s).\n",
                pkMesh, 
                (const char*) pkMesh->GetName());
        }

        kFormats.Add(NiDataStreamElement::F_FLOAT32_3);
        if (ePlatform != NiSystemDesc::RENDERER_WII)
            kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (ePlatform == NiSystemDesc::RENDERER_PS3)
            kFormats.Add(NiDataStreamElement::F_FLOAT16_3);

        bInvalid |= !NiMeshUtilities::ValidateStream(
            "NiSkinningMeshModifier(Software)", pkMesh,
            NiCommonSemantics::POSITION(), 0, kFormats,
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE);

        bInvalid |= !NiMeshUtilities::ValidateStream(
            "NiSkinningMeshModifier(Software)", pkMesh,
            NiCommonSemantics::POSITION_BP(), 0, kFormats,
            NiDataStream::ACCESS_CPU_READ);

        // Normals can be 16 bit ints on the Wii
        if (ePlatform == NiSystemDesc::RENDERER_WII)
            kFormats.Add(NiDataStreamElement::F_NORMINT16_3);

        if (pkMesh->FindStreamRef(NiCommonSemantics::NORMAL()) &&
            pkMesh->FindStreamRef(NiCommonSemantics::NORMAL_BP()))
        {
            bInvalid |= !NiMeshUtilities::ValidateStream(
                "NiSkinningMeshModifier(Software)", pkMesh,
                NiCommonSemantics::NORMAL(), 0, kFormats,
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE |
                NiDataStream::ACCESS_CPU_WRITE_MUTABLE);

            bInvalid |= !NiMeshUtilities::ValidateStream(
                "NiSkinningMeshModifier(Software)", pkMesh,
                NiCommonSemantics::NORMAL_BP(), 0, kFormats,
                NiDataStream::ACCESS_CPU_READ);
        
            if (pkMesh->FindStreamRef(NiCommonSemantics::TANGENT()) &&
                pkMesh->FindStreamRef(NiCommonSemantics::BINORMAL()) &&
                pkMesh->FindStreamRef(NiCommonSemantics::TANGENT_BP()) &&
                pkMesh->FindStreamRef(NiCommonSemantics::BINORMAL_BP()))
            {
                bInvalid |= !NiMeshUtilities::ValidateStream(
                    "NiSkinningMeshModifier(Software)", pkMesh,
                    NiCommonSemantics::TANGENT(), 0, kFormats,
                    NiDataStream::ACCESS_CPU_WRITE_MUTABLE | 
                    NiDataStream::ACCESS_CPU_WRITE_VOLATILE);

                bInvalid |= !NiMeshUtilities::ValidateStream(
                    "NiSkinningMeshModifier(Software)", pkMesh,
                    NiCommonSemantics::BINORMAL(), 0, kFormats,
                    NiDataStream::ACCESS_CPU_WRITE_MUTABLE |
                    NiDataStream::ACCESS_CPU_WRITE_VOLATILE);

                bInvalid |= !NiMeshUtilities::ValidateStream(
                    "NiSkinningMeshModifier(Software)", pkMesh,
                    NiCommonSemantics::TANGENT_BP(), 0, kFormats,
                    NiDataStream::ACCESS_CPU_READ);

                bInvalid |= !NiMeshUtilities::ValidateStream(
                    "NiSkinningMeshModifier(Software)", pkMesh,
                    NiCommonSemantics::BINORMAL_BP(), 0, kFormats,
                    NiDataStream::ACCESS_CPU_READ);
            }
        }

        kFormats.RemoveAll();
        kFormats.Add(NiDataStreamElement::F_FLOAT32_3);
        if (ePlatform != NiSystemDesc::RENDERER_WII)
        {
            kFormats.Add(NiDataStreamElement::F_FLOAT32_4);
            kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        }
        else
        {
            kFormats.Add(NiDataStreamElement::F_NORMUINT8_4);
        }
        if (ePlatform == NiSystemDesc::RENDERER_PS3)
            kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        bInvalid |= !NiMeshUtilities::ValidateStream(
            "NiSkinningMeshModifier(Software)", pkMesh,
            NiCommonSemantics::BLENDWEIGHT(), 0, kFormats,
            NiDataStream::ACCESS_CPU_READ);

        kFormats.RemoveAll();
        kFormats.Add(NiDataStreamElement::F_UINT8_4);
        if (ePlatform != NiSystemDesc::RENDERER_WII)
            kFormats.Add(NiDataStreamElement::F_INT16_4);
        bInvalid |= !NiMeshUtilities::ValidateStream(
            "NiSkinningMeshModifier(Software)", pkMesh,
            NiCommonSemantics::BLENDINDICES(), 0, kFormats,
            NiDataStream::ACCESS_CPU_READ);
    }
    else
    {
        kFormats.Add(NiDataStreamElement::F_FLOAT32_3);
        kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (ePlatform == NiSystemDesc::RENDERER_PS3)
            kFormats.Add(NiDataStreamElement::F_FLOAT16_3);

        bInvalid |= !NiMeshUtilities::ValidateStream(
            "NiSkinningMeshModifier(Hardware)", pkMesh,
            NiCommonSemantics::POSITION_BP(), 0, kFormats,
            NiDataStream::ACCESS_GPU_READ);

        if (pkMesh->FindStreamRef(NiCommonSemantics::NORMAL_BP()))
        {
            bInvalid |= !NiMeshUtilities::ValidateStream(
                "NiSkinningMeshModifier(Hardware)", pkMesh,
                NiCommonSemantics::NORMAL_BP(), 0, kFormats,
                NiDataStream::ACCESS_GPU_READ);
        
            if (pkMesh->FindStreamRef(NiCommonSemantics::TANGENT_BP()) &&
                pkMesh->FindStreamRef(NiCommonSemantics::BINORMAL_BP()))
            {
                bInvalid |= !NiMeshUtilities::ValidateStream(
                    "NiSkinningMeshModifier(Hardware)", pkMesh,
                    NiCommonSemantics::TANGENT_BP(), 0, kFormats,
                    NiDataStream::ACCESS_GPU_READ);

                bInvalid |= !NiMeshUtilities::ValidateStream(
                    "NiSkinningMeshModifier(Hardware)", pkMesh,
                    NiCommonSemantics::BINORMAL_BP(), 0, kFormats,
                    NiDataStream::ACCESS_GPU_READ);
            }
        }

        kFormats.RemoveAll();
        kFormats.Add(NiDataStreamElement::F_FLOAT32_3);
        kFormats.Add(NiDataStreamElement::F_FLOAT32_4);
        kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (ePlatform == NiSystemDesc::RENDERER_PS3)
            kFormats.Add(NiDataStreamElement::F_FLOAT16_3);

        bInvalid |= !NiMeshUtilities::ValidateStream(
            "NiSkinningMeshModifier(Hardware)", pkMesh,
            NiCommonSemantics::BLENDWEIGHT(), 0, kFormats,
            NiDataStream::ACCESS_GPU_READ);

        kFormats.RemoveAll();
        kFormats.Add(NiDataStreamElement::F_UINT8_4);
        kFormats.Add(NiDataStreamElement::F_INT16_4);
        kFormats.Add(NiDataStreamElement::F_NORMUINT8_4_BGRA);

        bInvalid |= !NiMeshUtilities::ValidateStream(
            "NiSkinningMeshModifier(Hardware)", pkMesh,
            NiCommonSemantics::BLENDINDICES(), 0, kFormats,
            NiDataStream::ACCESS_GPU_READ);

        bInvalid |= !NiMeshUtilities::ValidateStream(
            "NiSkinningMeshModifier(Hardware)", pkMesh,
            NiCommonSemantics::BONE_PALETTE(), 0,
            NiDataStreamElement::F_UINT16_1,
            NiDataStream::ACCESS_CPU_READ);
    }

    return !bInvalid;
}
//---------------------------------------------------------------------------
void NiSkinningMeshModifier::RetrieveRequirements(NiMeshRequirements&
    kRequirements) const
{
    NiSystemDesc::RendererID eRenderer = kRequirements.GetRenderer();
    NiMeshRequirements::SemanticRequirement* pkReq = 0;
    NiUInt32 uiReqIndex = 0;

    NiUInt32 uiWiiXNBTSet = 0;
    NiUInt32 uiWiiXNSet = 0;

    // Create the requirement sets
    NiUInt32 uiXNBTSet = kRequirements.CreateNewRequirementSet();
    if (eRenderer == NiSystemDesc::RENDERER_WII)
        uiWiiXNBTSet = kRequirements.CreateNewRequirementSet();
    NiUInt32 uiXNSet = kRequirements.CreateNewRequirementSet();
    if (eRenderer == NiSystemDesc::RENDERER_WII)
        uiWiiXNSet = kRequirements.CreateNewRequirementSet();
    NiUInt32 uiXSet = kRequirements.CreateNewRequirementSet();

    // The normal, binormal, and tangent streams are not required for
    // software skinning.  If an input stream is present, then an output
    // stream is required (we support X, XN, XNBT, where X is position, etc…) 
    if (GetBit(USE_SOFTWARE_SKINNING))
    {
        NiUInt32 auiPNBTStream[4] = {1, 2, 3, 4};
        NiUInt32 auiPNBTBPStream[4] = {5, 6, 7, 8};
        NiUInt8 uiPNBTBPCPUWriteAccess = 
            NiDataStream::ACCESS_CPU_WRITE_STATIC;
        if (eRenderer == NiSystemDesc::RENDERER_WII)
        {
            // On the Wii, the PNBT elements should always be interleaved
            // when using FLOAT32 based elements
            auiPNBTStream[0] = 1;
            auiPNBTStream[1] = 1;
            auiPNBTStream[2] = 1;
            auiPNBTStream[3] = 1;
            auiPNBTBPStream[0] = 2;
            auiPNBTBPStream[1] = 2;
            auiPNBTBPStream[2] = 2;
            auiPNBTBPStream[3] = 2;
        }

        // Position
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::POSITION(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, auiPNBTStream[0],
            NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        if (eRenderer != NiSystemDesc::RENDERER_WII)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        kRequirements.AddRequirement(uiXNSet, pkReq);
        kRequirements.AddRequirement(uiXSet, pkReq);
        if (eRenderer == NiSystemDesc::RENDERER_WII)
        {
            kRequirements.AddRequirement(uiWiiXNBTSet, pkReq);
            kRequirements.AddRequirement(uiWiiXNSet, pkReq);
        }

        // Position_BP
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::POSITION_BP(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, auiPNBTBPStream[0],
            NiDataStream::USAGE_VERTEX, 
            (NiUInt8)(NiDataStream::ACCESS_CPU_READ | uiPNBTBPCPUWriteAccess),
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        if (eRenderer != NiSystemDesc::RENDERER_WII)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        kRequirements.AddRequirement(uiXNSet, pkReq);
        kRequirements.AddRequirement(uiXSet, pkReq);
        if (eRenderer == NiSystemDesc::RENDERER_WII)
        {
            kRequirements.AddRequirement(uiWiiXNBTSet, pkReq);
            kRequirements.AddRequirement(uiWiiXNSet, pkReq);
        }

        // Normal
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::NORMAL(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, auiPNBTStream[1],
            NiDataStream::USAGE_VERTEX, 
            (NiUInt8)(NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE),
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        if (eRenderer != NiSystemDesc::RENDERER_WII)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        kRequirements.AddRequirement(uiXNSet, pkReq);
        if (eRenderer == NiSystemDesc::RENDERER_WII)
        {
            uiReqIndex = kRequirements.AddRequirement(uiWiiXNBTSet,
                NiCommonSemantics::NORMAL(), 0,
                NiMeshRequirements::STRICT_INTERLEAVE, 3,
                NiDataStream::USAGE_VERTEX, 
                (NiUInt8)(NiDataStream::ACCESS_GPU_READ | 
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE),
                NiDataStreamElement::F_NORMINT16_3);
            pkReq = kRequirements.GetRequirement(uiWiiXNBTSet, uiReqIndex);
            kRequirements.AddRequirement(uiWiiXNSet, pkReq);
        }

        // Normal_BP
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::NORMAL_BP(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, auiPNBTBPStream[1],
            NiDataStream::USAGE_VERTEX, 
            (NiUInt8)(NiDataStream::ACCESS_CPU_READ | uiPNBTBPCPUWriteAccess),
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        if (eRenderer != NiSystemDesc::RENDERER_WII)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        kRequirements.AddRequirement(uiXNSet, pkReq);
        if (eRenderer == NiSystemDesc::RENDERER_WII)
        {
            uiReqIndex = kRequirements.AddRequirement(uiWiiXNBTSet,
                NiCommonSemantics::NORMAL_BP(), 0,
                NiMeshRequirements::STRICT_INTERLEAVE, 4,
                NiDataStream::USAGE_VERTEX, 
                (NiUInt8)(NiDataStream::ACCESS_CPU_READ | uiPNBTBPCPUWriteAccess),
                NiDataStreamElement::F_NORMINT16_3);
            pkReq = kRequirements.GetRequirement(uiWiiXNBTSet, uiReqIndex);
            kRequirements.AddRequirement(uiWiiXNSet, pkReq);
        }

        // Binormal
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::BINORMAL(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, auiPNBTStream[2],
            NiDataStream::USAGE_VERTEX, 
            (NiUInt8)(NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE),
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        if (eRenderer != NiSystemDesc::RENDERER_WII)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        if (eRenderer == NiSystemDesc::RENDERER_WII)
        {
            uiReqIndex = kRequirements.AddRequirement(uiWiiXNBTSet,
                NiCommonSemantics::BINORMAL(), 0,
                NiMeshRequirements::STRICT_INTERLEAVE, 3,
                NiDataStream::USAGE_VERTEX, 
                (NiUInt8)(NiDataStream::ACCESS_GPU_READ | 
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE),
                NiDataStreamElement::F_NORMINT16_3);
        }

        // Tangent
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::TANGENT(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, auiPNBTStream[3],
            NiDataStream::USAGE_VERTEX, 
            (NiUInt8)(NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE), 
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        if (eRenderer != NiSystemDesc::RENDERER_WII)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        if (eRenderer == NiSystemDesc::RENDERER_WII)
        {
            uiReqIndex = kRequirements.AddRequirement(uiWiiXNBTSet,
                NiCommonSemantics::TANGENT(), 0,
                NiMeshRequirements::STRICT_INTERLEAVE, 3,
                NiDataStream::USAGE_VERTEX, 
                (NiUInt8)(NiDataStream::ACCESS_GPU_READ | 
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE), 
                NiDataStreamElement::F_NORMINT16_3);
        }

        // Binormal_BP
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::BINORMAL_BP(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, auiPNBTBPStream[2],
            NiDataStream::USAGE_VERTEX, 
            (NiUInt8)(NiDataStream::ACCESS_CPU_READ | uiPNBTBPCPUWriteAccess),
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        if (eRenderer != NiSystemDesc::RENDERER_WII)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        if (eRenderer == NiSystemDesc::RENDERER_WII)
        {
            uiReqIndex = kRequirements.AddRequirement(uiWiiXNBTSet,
                NiCommonSemantics::BINORMAL_BP(), 0,
                NiMeshRequirements::STRICT_INTERLEAVE, 4,
                NiDataStream::USAGE_VERTEX,
                (NiUInt8)(NiDataStream::ACCESS_CPU_READ | uiPNBTBPCPUWriteAccess),
                NiDataStreamElement::F_NORMINT16_3);
        }

        // Tangent_BP
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::TANGENT_BP(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, auiPNBTBPStream[3],
            NiDataStream::USAGE_VERTEX,
            (NiUInt8)(NiDataStream::ACCESS_CPU_READ | uiPNBTBPCPUWriteAccess),
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        if (eRenderer != NiSystemDesc::RENDERER_WII)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        if (eRenderer == NiSystemDesc::RENDERER_WII)
        {
            uiReqIndex = kRequirements.AddRequirement(uiWiiXNBTSet,
                NiCommonSemantics::TANGENT_BP(), 0,
                NiMeshRequirements::STRICT_INTERLEAVE, 4,
                NiDataStream::USAGE_VERTEX, 
                (NiUInt8)(NiDataStream::ACCESS_CPU_READ | uiPNBTBPCPUWriteAccess),
                NiDataStreamElement::F_NORMINT16_3);
        }

        // Blendweight
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::BLENDWEIGHT(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, 9,
            NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_CPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);        
        if (eRenderer != NiSystemDesc::RENDERER_WII)
        {
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT32_4);
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        }
        else
        {
            pkReq->m_kFormats.Add(NiDataStreamElement::F_NORMUINT8_4);
        }
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        kRequirements.AddRequirement(uiXSet, pkReq);
        kRequirements.AddRequirement(uiXNSet, pkReq);
        if (eRenderer == NiSystemDesc::RENDERER_WII)
        {
            kRequirements.AddRequirement(uiWiiXNBTSet, pkReq);
            kRequirements.AddRequirement(uiWiiXNSet, pkReq);
        }

        // Blendindices
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::BLENDINDICES(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, 10,
            NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_CPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStreamElement::F_UINT8_4);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        if (eRenderer != NiSystemDesc::RENDERER_WII)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        kRequirements.AddRequirement(uiXSet, pkReq);
        kRequirements.AddRequirement(uiXNSet, pkReq);
        if (eRenderer == NiSystemDesc::RENDERER_WII)
        {
            kRequirements.AddRequirement(uiWiiXNBTSet, pkReq);
            kRequirements.AddRequirement(uiWiiXNSet, pkReq);
        }
    }
    else
    {
        // Position_BP
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::POSITION_BP(), 0,
            NiMeshRequirements::FLOATER, 0,
            NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        kRequirements.AddRequirement(uiXNSet, pkReq);
        kRequirements.AddRequirement(uiXSet, pkReq);

        // Normal_BP
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::NORMAL_BP(), 0,
            NiMeshRequirements::FLOATER, 0,
            NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        kRequirements.AddRequirement(uiXNSet, pkReq);

        // Tangent_BP
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::TANGENT_BP(), 0,
            NiMeshRequirements::FLOATER, 0,
            NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);

        // BiNormal_BP
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::BINORMAL_BP(), 0,
            NiMeshRequirements::FLOATER, 0,
            NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);

        // Blendweight.
        // We could potentially support a more compressed form, such as
        // F_NORMUINT8_4 or F_NORMUINT8_3, although the latter has limited
        // hardware support.
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::BLENDWEIGHT(), 0,
            NiMeshRequirements::FLOATER, 0,
            NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStreamElement::F_FLOAT32_3);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT32_4);
        pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
        if (eRenderer == NiSystemDesc::RENDERER_PS3)
            pkReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
        kRequirements.AddRequirement(uiXSet, pkReq);
        kRequirements.AddRequirement(uiXNSet, pkReq);

        // Blendindices
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::BLENDINDICES(), 0,
            NiMeshRequirements::FLOATER, 0,
            NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStreamElement::F_UINT8_4);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        pkReq->m_kFormats.Add(NiDataStreamElement::F_NORMUINT8_4_BGRA);
        pkReq->m_kFormats.Add(NiDataStreamElement::F_INT16_1);
        kRequirements.AddRequirement(uiXSet, pkReq);
        kRequirements.AddRequirement(uiXNSet, pkReq);

        // Bone mapping for partitions
        uiReqIndex = kRequirements.AddRequirement(uiXNBTSet,
            NiCommonSemantics::BONE_PALETTE(), 0,
            NiMeshRequirements::FLOATER, 0, NiDataStream::USAGE_USER,
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStreamElement::F_UINT16_1);
        pkReq = kRequirements.GetRequirement(uiXNBTSet, uiReqIndex);
        kRequirements.AddRequirement(uiXSet, pkReq);
        kRequirements.AddRequirement(uiXNSet, pkReq);
    }
}
//---------------------------------------------------------------------------
bool NiSkinningMeshModifier::Attach(NiMesh* pkMesh)
{
    // Check for presence of bones before attaching
    if (m_pkRootBoneParent == NULL)
        return false;
    NiUInt32 ui = 0;
    for (; ui < m_uiBoneCount; ui++)
    {
        if (m_apkBones[ui] == NULL)
            return false;
    }

#if !defined(_WII)
    // Make sure the kernel combination is set.
    NiDataStreamElement kElement;
    NiDataStreamRef* pkDataStreamRef = NULL;

    pkMesh->FindStreamRefAndElementBySemantic(NiCommonSemantics::POSITION_BP(),
        0, NiDataStreamElement::F_UNKNOWN, pkDataStreamRef, kElement);
    if (!pkDataStreamRef)
        return false;
    NiUInt32 uiPositionStride = 
        kElement.GetComponentSize() * kElement.GetComponentCount();

    pkMesh->FindStreamRefAndElementBySemantic(NiCommonSemantics::NORMAL_BP(),
        0, NiDataStreamElement::F_UNKNOWN, pkDataStreamRef, kElement);
    NiUInt32 uiNormalStride = pkDataStreamRef ? 
        kElement.GetComponentSize() * kElement.GetComponentCount() : 
        sizeof(float) * 3;

    pkMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::BLENDINDICES(), 0, NiDataStreamElement::F_UNKNOWN,
        pkDataStreamRef, kElement);
    if (!pkDataStreamRef)
        return false;
    NiUInt32 uiBlendIndicesStride = 
        kElement.GetComponentSize() * kElement.GetComponentCount();

    pkMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::BLENDWEIGHT(), 0, NiDataStreamElement::F_UNKNOWN,
        pkDataStreamRef, kElement);
    if (!pkDataStreamRef)
        return false;
    NiUInt32 uiBlendWeightsStride = 
        kElement.GetComponentSize() * kElement.GetComponentCount();

    // Flags for streams
    NiUInt32 uiNumWeightsPerBone = 
        (NiUInt32)kElement.GetComponentCount();

    // Store the values for the kernel data
    m_kSkinningKernelData.SetCombination(uiNumWeightsPerBone, 
        uiBlendIndicesStride, uiBlendWeightsStride, uiPositionStride,
        uiNormalStride);

#else // !defined(_WII)

    // Make sure the kernel combination is set.
    NiDataStreamElement kBlendWeightElement;
    NiDataStreamElement kBlendIndicesElement;
    NiDataStreamElement kPositionElement;
    NiDataStreamElement kNormalElement;
    NiDataStreamRef* pkDataStreamRef = NULL;
    NiDataStreamRef* pkPosDataStreamRef = NULL;
    bool bBTStreams = false;
    bool bPNBTInterleaved = false;

    if (!pkMesh->FindStreamRefAndElementBySemantic(NiCommonSemantics::BLENDWEIGHT(),
        0, NiDataStreamElement::F_UNKNOWN, pkDataStreamRef, kBlendWeightElement))
    {
        return false;
    }

    if (!pkMesh->FindStreamRefAndElementBySemantic(NiCommonSemantics::BLENDINDICES(),
        0, NiDataStreamElement::F_UNKNOWN, pkDataStreamRef, kBlendIndicesElement))
    {
        return false;
    }

    if (!pkMesh->FindStreamRefAndElementBySemantic(NiCommonSemantics::POSITION_BP(),
        0, NiDataStreamElement::F_UNKNOWN, pkPosDataStreamRef, kPositionElement))
    {
        return false;
    }

    if (pkMesh->FindStreamRefAndElementBySemantic(NiCommonSemantics::NORMAL_BP(),
        0, NiDataStreamElement::F_UNKNOWN, pkDataStreamRef, kNormalElement))
    {
        if ((pkDataStreamRef->FindElementDescIndex(
                NiCommonSemantics::BINORMAL_BP()) != UINT_MAX) &&
            (pkDataStreamRef->FindElementDescIndex(
                NiCommonSemantics::TANGENT_BP()) != UINT_MAX))
        {
            bBTStreams = true;
        }

        if (pkPosDataStreamRef == pkDataStreamRef)
        {
            bPNBTInterleaved = true;
        }
    }


    // Store the values for the kernel data
    m_kWiiSkinningKernelData.SetCombination(kBlendWeightElement, kBlendIndicesElement,
        kPositionElement, kNormalElement, bBTStreams, bPNBTInterleaved);

#endif // !defined(_WII)

    AddSubmitSyncPoint(NiSyncArgs::SYNC_POST_UPDATE);
    AddCompleteSyncPoint(NiSyncArgs::SYNC_RENDER);

    // Verify that the bone palette is present if we intend to shader skin
    if (!GetBit(USE_SOFTWARE_SKINNING))
    {
        NIASSERT(pkMesh->FindStreamRef(NiCommonSemantics::BONE_PALETTE())
            != NULL);
    }

    // Create bone matrices
    NIASSERT(!m_pkBoneMatrices);
    m_pkBoneMatrices = NiAlloc(NiMatrix3x4, m_uiBoneCount);
    NIASSERT(m_pkBoneMatrices);

    // Create a list of pointers to world bounds for each bone
    // These will be DMAed on PS3, and dereferenced on other platforms
    m_ppkBoneWorlds = NiAlloc(NiTransform*, m_uiBoneCount);
    NIASSERT(m_ppkBoneWorlds);
    for (ui = 0; ui < m_uiBoneCount; ui++)
    {
        m_ppkBoneWorlds[ui] = 
            (NiTransform*)&m_apkBones[ui]->GetWorldTransform();
    }

    // Prepare the task for execution
    if (!AttachCalcBoneMatricesTask(pkMesh))
        return false;

    if (GetBit(USE_SOFTWARE_SKINNING))
    {
        AddSubmitSyncPoint(NiSyncArgs::SYNC_VISIBLE);
        AddCompleteSyncPoint(NiSyncArgs::SYNC_VISIBLE);
    
        if (!AttachDeformTask(pkMesh))
            return false;
    }
    
    // Reset the update time
    m_fUpdateTime = -1.0f;
    
    return true;
}
//---------------------------------------------------------------------------
bool NiSkinningMeshModifier::Detach(NiMesh* pkMesh)
{
    NI_UNUSED_ARG(pkMesh);
    NIASSERT(pkMesh);

    m_kSubmitPoints.RemoveAll();
    m_kCompletePoints.RemoveAll();

    if (m_spDeformTask)
        NiDelete m_spDeformTask->GetKernel();

    // Release the tasks before destroying the kernels because
    // the tasks hold references to the kernels.
    m_spDeformTask = 0;
    m_spCBMTask = 0;
    
    NiFree(m_pkCBMKernelData);
    m_pkCBMKernelData = 0;
    NiDelete m_pkCBMKernel;
    m_pkCBMKernel = 0;
    NiFree(m_ppkBoneWorlds);
    m_ppkBoneWorlds = 0;
    NiFree(m_pkBoneMatrices);
    m_pkBoneMatrices = 0;

    m_kBoneMatrixStream.ClearTaskArrays();
#if defined(_WII)
    m_kWiiSkinningKernelDataStream.ClearTaskArrays();
    m_kBindPoseStreams[0].ClearTaskArrays();
    m_kBindPoseStreams[1].ClearTaskArrays();
    m_kBlendWeightStream.ClearTaskArrays();
    m_kBlendIndiciesStream.ClearTaskArrays();
    m_kOutputStreams[0].ClearTaskArrays();
    m_kOutputStreams[1].ClearTaskArrays();
#else
    m_kSkinningKernelDataStream.ClearTaskArrays();
    m_kBPPositionsStream.ClearTaskArrays();
    m_kBPNormalsStream.ClearTaskArrays();
    m_kBPBinormalsStream.ClearTaskArrays();
    m_kBPTangentsStream.ClearTaskArrays();
    m_kBlendWeightStream.ClearTaskArrays();
    m_kBlendIndiciesStream.ClearTaskArrays();
    m_kPositionsStream.ClearTaskArrays();
    m_kNormalsStream.ClearTaskArrays();
    m_kBinormalsStream.ClearTaskArrays();
    m_kTangentsStream.ClearTaskArrays();
#endif
    m_kCBMKernelDataStream.ClearTaskArrays();
    m_kBoneWorldStream.ClearTaskArrays();
    m_kSkinToBoneTransformsStream.ClearTaskArrays();
    m_kBoneMatrixOutputStream.ClearTaskArrays();
    
    return true;
}
//---------------------------------------------------------------------------
bool NiSkinningMeshModifier::SubmitTasks(NiMesh* pkMesh,
    NiSyncArgs* pkArgs, NiSPWorkflowManager* pkWFManager)
{
    // In post update, update bounds if needed
    if (pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_POST_UPDATE)
    {
        // NiSyncArgs does not use RTTI. Static cast is safe here because
        // submit point is SYNC_POST_UPDATE and NiUpdateSyncArgs are 
        // guaranteed at that point.
        float fTime = ((NiUpdateSyncArgs*)pkArgs)->m_kUpdateProcess.GetTime();
        if (fTime == m_fUpdateTime)
            return false;
        m_fUpdateTime = fTime;
            
        // Complete any outstanding workflows, which might be outstanding
        // because the object was not visible or update was called multiple
        // times without rendering.
        if (m_spCBMWorkflow || m_spDeformWorkflow)
        {
            NiSyncArgs kCompleteArgs;
            kCompleteArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
            kCompleteArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
            CompleteTasks(pkMesh, &kCompleteArgs);
        }

        NiUInt32 uiTaskGroup = 0;
        if (GetBit(USE_SOFTWARE_SKINNING))
        {
            GetWorldToSkinTransform(m_pkCBMKernelData->m_kWorldToSkin);
            uiTaskGroup = NiSyncArgs::GetTaskGroupID(
                NiSyncArgs::SYNC_POST_UPDATE, NiSyncArgs::SYNC_VISIBLE);
        }
        else
        {
            NiTransform kWorldToSkin;
            GetWorldToSkinTransform(kWorldToSkin);

            // We can not call GetWorldTransform here because mesh modifiers
            // execute prior to UpdateWorldData. We must compute it.
            NiTransform kWorld;
            if (pkMesh->GetParent())
            {
                kWorld = pkMesh->GetParent()->GetWorldTransform() * 
                    pkMesh->GetLocalTransform();
            }
            else
            {
                kWorld = pkMesh->GetLocalTransform();
            }

            m_pkCBMKernelData->m_kWorldToSkin =
                kWorld * kWorldToSkin;
            uiTaskGroup = NiSyncArgs::GetTaskGroupID(
                NiSyncArgs::SYNC_POST_UPDATE, NiSyncArgs::SYNC_RENDER);
        }

        // Update model bounds from bones if not fixed at export
        if (GetBit(RECOMPUTE_BOUNDS))
        {
            UpdateModelBounds(pkMesh);
        }

        m_spCBMWorkflow =
            pkWFManager->AddRelatedTask(m_spCBMTask, uiTaskGroup, true);

        // Mark the bones as dirty
        m_bBonesDirty = true;
            
        return true;
    }
    else if (pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_VISIBLE)
    {
        // You hit this assert if there are outstanding workflows. There
        // are two likely reasons for this: the bone matrices workflow
        // was not completed, which should have been done in the calling
        // function (typically NiMesh::OnVisible); or the deformation
        // workflow has not completed which means that, despite being visible,
        // the object was not rendered. If you are culling something but not
        // rendering it, you should complete all the modifiers on objects
        // in the visible list.
        NIASSERT(!m_spCBMWorkflow && !m_spDeformWorkflow);

        // Only add the deform task if the bones are dirty.  If the CBM
        // task hasn't been run since the last time the object was rendered,
        // there's no reason to recalculate the skinning buffers (for instance,
        // if the object is rendered to a shadow map prior to being rendered
        // to the main scene).
        if (m_spDeformTask && m_bBonesDirty)
        {
            NiUInt32 uiTaskGroup =
                NiSyncArgs::GetTaskGroupID(NiSyncArgs::SYNC_VISIBLE,
                NiSyncArgs::SYNC_RENDER);

            m_spDeformWorkflow = pkWFManager->AddRelatedTask(m_spDeformTask,
                uiTaskGroup, false);

            // The deform task will update the skinned buffers based on
            // the current bones, so we can mark the bones as not dirty now.
            m_bBonesDirty = false;

            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiSkinningMeshModifier::CompleteTasks(NiMesh*, NiSyncArgs* pkArgs)
{
    if (m_spCBMWorkflow && 
        (pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_POST_UPDATE ||
         pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_ANY))
    {
        if (GetBit(USE_SOFTWARE_SKINNING))
        {
            if (pkArgs->m_uiCompletePoint == NiSyncArgs::SYNC_VISIBLE ||
                pkArgs->m_uiCompletePoint == NiSyncArgs::SYNC_ANY)
            {
                NiStreamProcessor::Get()->Wait(m_spCBMWorkflow);
                m_spCBMWorkflow = 0;
            }
        }
        else
        {
            if (pkArgs->m_uiCompletePoint == NiSyncArgs::SYNC_RENDER ||
                pkArgs->m_uiCompletePoint == NiSyncArgs::SYNC_ANY)
            {
                NiStreamProcessor::Get()->Wait(m_spCBMWorkflow);
                m_spCBMWorkflow = 0;
            }
        }
    }
    if (m_spDeformWorkflow &&
        (pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_VISIBLE ||
        pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_ANY))
    {
        if (pkArgs->m_uiCompletePoint == NiSyncArgs::SYNC_RENDER ||
            pkArgs->m_uiCompletePoint == NiSyncArgs::SYNC_ANY)
        {
            NiStreamProcessor::Get()->Wait(m_spDeformWorkflow);
            m_spDeformWorkflow = 0;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSkinningMeshModifier::EnableDynamicBounds()
{
    if (m_pkBoneBounds == NULL)
    {
        m_pkBoneBounds = NiAlloc(NiBound, m_uiBoneCount);
        NIASSERT(m_pkBoneBounds);
    }

    SetBit(true, RECOMPUTE_BOUNDS);
}
//---------------------------------------------------------------------------
void NiSkinningMeshModifier::DisableDynamicBounds()
{
    NiFree(m_pkBoneBounds);
    m_pkBoneBounds = NULL;

    SetBit(false, RECOMPUTE_BOUNDS);
}
//---------------------------------------------------------------------------
void NiSkinningMeshModifier::UpdateModelBounds(NiMesh* pkMesh)
{
    NIASSERT((m_pkBoneBounds != NULL) && (m_ppkBoneWorlds != NULL));

    NiBound kWorldBound;

    kWorldBound.Update(m_pkBoneBounds[0], *m_ppkBoneWorlds[0]);

    for (NiUInt32 i = 1; i < m_uiBoneCount; i++)
    {
        if (m_apkBones[i]->GetSelectiveUpdate())
        {
            NiBound kBoneBound;
            kBoneBound.Update(m_pkBoneBounds[i], *m_ppkBoneWorlds[i]);
            kWorldBound.Merge(&kBoneBound);
        }
    }

    NiBound& kMeshBound = const_cast<NiBound&>(pkMesh->GetModelBound());
    NiTransform kWorldToSkin;
    GetWorldToSkinTransform(kWorldToSkin);
    kMeshBound.Update(kWorldBound, kWorldToSkin);
}
//---------------------------------------------------------------------------
bool NiSkinningMeshModifier::AttachCalcBoneMatricesTask(NiMesh*)
{
    // Allocate the task
    m_spCBMTask = NiSPTask::GetNewTask(3, 1);
    
    // Create task and add the kernel 
    m_pkCBMKernel = NiNew NiCalculateBoneMatricesKernel;
    NIASSERT(m_pkCBMKernel);
    m_spCBMTask->SetKernel(m_pkCBMKernel);

    // Create the configuration structure for the kernel
    m_pkCBMKernelData = NiAlloc(NiCBMKernelData, 1);
    NIASSERT(m_pkCBMKernelData);
    m_pkCBMKernelData->m_bSoftwareSkinned = GetBit(USE_SOFTWARE_SKINNING);

    // Add the inputs
    m_kCBMKernelDataStream.SetData(m_pkCBMKernelData);
    m_kCBMKernelDataStream.SetBlockCount(1);
    m_spCBMTask->AddInput(&m_kCBMKernelDataStream);

    m_kBoneWorldStream.SetData(m_ppkBoneWorlds);
    m_kBoneWorldStream.SetBlockCount(m_uiBoneCount);
    m_spCBMTask->AddInput(&m_kBoneWorldStream);

    m_kSkinToBoneTransformsStream.SetData(m_pkSkinToBoneTransforms);
    m_kSkinToBoneTransformsStream.SetBlockCount(m_uiBoneCount);
    m_spCBMTask->AddInput(&m_kSkinToBoneTransformsStream);

    // Add the output
    m_kBoneMatrixOutputStream.SetData(m_pkBoneMatrices);
    m_kBoneMatrixOutputStream.SetBlockCount(m_uiBoneCount);
    m_spCBMTask->AddOutput(&m_kBoneMatrixOutputStream);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiSkinningMeshModifier::AttachDeformTask(NiMesh* pkMesh)
{
#if !defined(_WII)
    // Grab the output position stream and determine the number of verts
    NiDataStreamRef* pkPositionsRef = 
        pkMesh->FindStreamRef(NiCommonSemantics::POSITION());
    if (!pkPositionsRef)
        return false;

    NiDataStreamRef* pkBindPosePosRef = pkMesh->FindStreamRef(
        NiCommonSemantics::POSITION_BP());
    if (!pkBindPosePosRef)
        return false;

    NiDataStreamRef* pkBlendIdxsRef = pkMesh->FindStreamRef(
        NiCommonSemantics::BLENDINDICES());
    if (!pkBlendIdxsRef)
        return false;

    NiDataStreamElement kBlendWeightElement;
    NiDataStreamRef* pkBlendWeightRef = NULL;
    pkMesh->FindStreamRefAndElementBySemantic(NiCommonSemantics::BLENDWEIGHT(),
        0, NiDataStreamElement::F_UNKNOWN, pkBlendWeightRef, 
        kBlendWeightElement);
    if (!pkBlendWeightRef)
        return false;

    // Flags for streams
    bool bNStream = false;
    bool bBTStreams = false;
    NiUInt32 uiPositionStride = pkPositionsRef->GetStride();
    NiUInt32 uiNormalStride = sizeof(float) * 3;
    NiUInt32 uiBiNormalStride = sizeof(float) * 3;
    NiUInt32 uiTangentStride = sizeof(float) * 3;

    // Add the deform task to the workflow. Don't specify input/output counts
    // because we don't readily know what they are.
    NIASSERT(!m_spDeformTask);
    m_spDeformTask = NiSPTask::GetNewTask();

    const NiUInt32 uiBlockCount = pkPositionsRef->GetTotalCount();

    // Add the inputs
    m_kSkinningKernelDataStream.SetData(&m_kSkinningKernelData);
    m_kSkinningKernelDataStream.SetBlockCount(1);
    m_spDeformTask->AddInput(&m_kSkinningKernelDataStream);

    // Set bone matrices fixed input stream
    m_kBoneMatrixStream.SetData(m_pkBoneMatrices);
    m_kBoneMatrixStream.SetBlockCount(m_uiBoneCount);
    m_spDeformTask->AddInput(&m_kBoneMatrixStream);

    // Set blend weights
    m_kBlendWeightStream.SetDataSource(pkBlendWeightRef->GetDataStream());
    m_kBlendWeightStream.SetStride(
        (NiUInt16)pkBlendWeightRef->GetStride());
    m_kBlendWeightStream.SetBlockCount(uiBlockCount);
    m_spDeformTask->AddInput(&m_kBlendWeightStream);

    // Set blend indicies
    m_kBlendIndiciesStream.SetDataSource(pkBlendIdxsRef->GetDataStream());
    m_kBlendIndiciesStream.SetStride(
        (NiUInt16)pkBlendIdxsRef->GetStride());
    m_kBlendIndiciesStream.SetBlockCount(uiBlockCount);
    m_spDeformTask->AddInput(&m_kBlendIndiciesStream);

    // Set position bind pose as input data
    m_kBPPositionsStream.SetDataSource(pkBindPosePosRef->GetDataStream());
    m_kBPPositionsStream.SetStride(
        (NiUInt16)pkBindPosePosRef->GetStride());
    m_kBPPositionsStream.SetBlockCount(uiBlockCount);
    m_spDeformTask->AddInput(&m_kBPPositionsStream);

    // Set position output data
    m_kPositionsStream.SetDataSource(pkPositionsRef->GetDataStream());
    m_kPositionsStream.SetStride(
        (NiUInt16)pkPositionsRef->GetStride());
    m_kPositionsStream.SetBlockCount(uiBlockCount);
    m_spDeformTask->AddOutput(&m_kPositionsStream);

    // Normals
    NiDataStreamRef* pkNormalsRef = pkMesh->FindStreamRef(
        NiCommonSemantics::NORMAL());
    NiDataStreamRef* pkBindPoseNormRef = pkMesh->FindStreamRef(
        NiCommonSemantics::NORMAL_BP());
    if (pkNormalsRef && pkBindPoseNormRef)
    {
        uiNormalStride = pkNormalsRef->GetStride();
        uiBiNormalStride = uiNormalStride;
        uiTangentStride = uiNormalStride;

        // Normals from bind pose as an input stream
        m_kBPNormalsStream.SetDataSource(pkBindPoseNormRef->GetDataStream());
        m_kBPNormalsStream.SetStride(
            (NiUInt16)pkBindPoseNormRef->GetStride());
        m_kBPNormalsStream.SetBlockCount(uiBlockCount);
        m_spDeformTask->AddInput(&m_kBPNormalsStream);

        // Normals as an output destination
        m_kNormalsStream.SetDataSource(pkNormalsRef->GetDataStream());
        m_kNormalsStream.SetStride(
            (NiUInt16)pkNormalsRef->GetStride());
        m_kNormalsStream.SetBlockCount(uiBlockCount);
        m_spDeformTask->AddOutput(&m_kNormalsStream);

        bNStream = true;

        // There should be both binormals and tangents if either are present
        NiDataStreamRef* pkBinormalsRef = pkMesh->FindStreamRef(
            NiCommonSemantics::BINORMAL());
        NiDataStreamRef* pkTangentsRef = pkMesh->FindStreamRef(
            NiCommonSemantics::TANGENT());
        NiDataStreamRef* pkBindPoseTangentRef = 
                pkMesh->FindStreamRef(NiCommonSemantics::TANGENT_BP());
        NiDataStreamRef* pkBindPoseBinormRef =
                pkMesh->FindStreamRef(NiCommonSemantics::BINORMAL_BP());

        if (pkBinormalsRef && pkTangentsRef && pkBindPoseBinormRef && 
            pkBindPoseTangentRef)
        {
            uiBiNormalStride = pkBinormalsRef->GetStride();
            uiTangentStride = pkTangentsRef->GetStride();

            // Bind-pose binormal input stream
            m_kBPBinormalsStream.SetDataSource(
                pkBindPoseBinormRef->GetDataStream());
            m_kBPBinormalsStream.SetStride(
                (NiUInt16)pkBindPoseBinormRef->GetStride());
            m_kBPBinormalsStream.SetBlockCount(uiBlockCount);
            m_spDeformTask->AddInput(&m_kBPBinormalsStream);

            // Binormal output stream
            m_kBinormalsStream.SetDataSource(pkBinormalsRef->GetDataStream());
            m_kBinormalsStream.SetStride(
                (NiUInt16)pkBinormalsRef->GetStride());
            m_kBinormalsStream.SetBlockCount(uiBlockCount);
            m_spDeformTask->AddOutput(&m_kBinormalsStream);

            // Bind-pose tangent input stream
            m_kBPTangentsStream.SetDataSource(
                pkBindPoseTangentRef->GetDataStream());
            m_kBPTangentsStream.SetStride(
                (NiUInt16)pkBindPoseTangentRef->GetStride());
            m_kBPTangentsStream.SetBlockCount(uiBlockCount);
            m_spDeformTask->AddInput(&m_kBPTangentsStream);

            // Tangent output stream
            m_kTangentsStream.SetDataSource(pkTangentsRef->GetDataStream());
            m_kTangentsStream.SetStride(
                (NiUInt16)pkTangentsRef->GetStride());
            m_kTangentsStream.SetBlockCount(uiBlockCount);
            m_spDeformTask->AddOutput(&m_kTangentsStream);
            bBTStreams = true;
        }
    }

    // Make sure the NBTs are the same stride
    if (uiNormalStride != uiBiNormalStride ||
        uiNormalStride != uiTangentStride)
    {
        return false;
    }

    // Create and add skinning kernel
    switch(m_spDeformTask->GetInputCount())
    {
    case 5:
        {
            NiSkinningKernelP* pkSkinningKernelP = NiNew NiSkinningKernelP;
            NIASSERT(pkSkinningKernelP);
            m_spDeformTask->SetKernel(pkSkinningKernelP);
        }
        break;
    case 6:
        if (uiPositionStride == sizeof(float) * 3)
        {
            NiSkinningKernelPN32* pkSkinningKernel = 
                NiNew NiSkinningKernelPN32;
            NIASSERT(pkSkinningKernel);
            m_spDeformTask->SetKernel(pkSkinningKernel);
        }
        else
        {
            NiSkinningKernelPN16* pkSkinningKernel = 
                NiNew NiSkinningKernelPN16;
            NIASSERT(pkSkinningKernel);
            m_spDeformTask->SetKernel(pkSkinningKernel);
        }
        break;
    case 8:
        if (uiPositionStride == sizeof(float) * 3)
        {
            NiSkinningKernelPNBT32* pkSkinningKernel = 
                NiNew NiSkinningKernelPNBT32;
            NIASSERT(pkSkinningKernel);
            m_spDeformTask->SetKernel(pkSkinningKernel);
        }
        else
        {
            NiSkinningKernelPNBT16* pkSkinningKernel = 
                NiNew NiSkinningKernelPNBT16;
            NIASSERT(pkSkinningKernel);
            m_spDeformTask->SetKernel(pkSkinningKernel);
        }
        break;
    }

#else // if defined(_WII)

    // Flags for streams
    bool bInputBTStreams = false;
    bool bOutputBTStreams = false;

    // Grab the output position stream. The output streams will be in one of several
    // configurations (P); (P|N); (P),(N); (P|N|B|T); and (P),(N|B|T) where () is a stream
    // and | means the content of the stream is interleaved.
    NiDataStreamRef* pkOutputPosRef = pkMesh->FindStreamRef(NiCommonSemantics::POSITION());
    if (!pkOutputPosRef)
        return false;

    NiDataStreamRef* pkOutputNBTRef = pkMesh->FindStreamRef(NiCommonSemantics::NORMAL());
    if ((pkOutputNBTRef != 0) && 
        (pkOutputNBTRef->FindElementDescIndex(
            NiCommonSemantics::BINORMAL()) != UINT_MAX) &&
        (pkOutputNBTRef->FindElementDescIndex(
            NiCommonSemantics::TANGENT()) != UINT_MAX))
    {
        bOutputBTStreams = true;
    }

    // Grab the bind pose streams. The bind pose streams will be in one of several
    // configurations (P); (P|N); (P),(N); (P|N|B|T); and (P),(N|B|T) where () is a stream
    // and | means the content of the stream is interleaved.
    NiDataStreamRef* pkBindPosePosRef = pkMesh->FindStreamRef(NiCommonSemantics::POSITION_BP());
    if (!pkBindPosePosRef)
        return false;
    
    NiDataStreamRef* pkBindPoseNBTRef = 0;
    // If there's an output stream to deform normals into then look for input normals
    if (pkOutputNBTRef != 0)
    {
        pkBindPoseNBTRef = pkMesh->FindStreamRef(NiCommonSemantics::NORMAL_BP());
        if (pkBindPoseNBTRef != 0)
        {
            if (bOutputBTStreams && 
                (pkBindPoseNBTRef->FindElementDescIndex(
                NiCommonSemantics::BINORMAL_BP()) != UINT_MAX) &&
                (pkBindPoseNBTRef->FindElementDescIndex(
                    NiCommonSemantics::TANGENT_BP()) != UINT_MAX))
            {
                bInputBTStreams = true;
            }
            else
            {
                // No input BT so disable output BT
                bOutputBTStreams = false;
            }
        }
        else
        {
            // No input normals so disable any output for them
            pkOutputNBTRef = 0;
        }
    }

    // Grab the blend indices
    NiDataStreamRef* pkBlendIdxsRef = pkMesh->FindStreamRef(NiCommonSemantics::BLENDINDICES());
    if (!pkBlendIdxsRef)
        return false;

    // Grab the blend weights
    NiDataStreamRef* pkBlendWeightRef  = pkMesh->FindStreamRef(NiCommonSemantics::BLENDWEIGHT());
    if (!pkBlendWeightRef)
        return false;

    // Add the deform task to the workflow. Don't specify input/output counts
    // because we don't readily know what they are.
    NIASSERT(!m_spDeformTask);
    m_spDeformTask = NiSPTask::GetNewTask();

    const NiUInt32 uiBlockCount = pkOutputPosRef->GetTotalCount();

    // Add the inputs
    m_kWiiSkinningKernelDataStream.SetData(&m_kWiiSkinningKernelData);
    m_kWiiSkinningKernelDataStream.SetBlockCount(1);
    m_spDeformTask->AddInput(&m_kWiiSkinningKernelDataStream);

    // Set bone matrices fixed input stream
    m_kBoneMatrixStream.SetData(m_pkBoneMatrices);
    m_kBoneMatrixStream.SetBlockCount(m_uiBoneCount);
    m_spDeformTask->AddInput(&m_kBoneMatrixStream);

    // Set blend weights
    m_kBlendWeightStream.SetDataSource(pkBlendWeightRef->GetDataStream());
    m_kBlendWeightStream.SetStride((NiUInt16)pkBlendWeightRef->GetStride());
    m_kBlendWeightStream.SetBlockCount(uiBlockCount);
    m_spDeformTask->AddInput(&m_kBlendWeightStream);

    // Set blend indicies
    m_kBlendIndiciesStream.SetDataSource(pkBlendIdxsRef->GetDataStream());
    m_kBlendIndiciesStream.SetStride((NiUInt16)pkBlendIdxsRef->GetStride());
    m_kBlendIndiciesStream.SetBlockCount(uiBlockCount);
    m_spDeformTask->AddInput(&m_kBlendIndiciesStream);

    // Set bind pose as inputs which requires either 1 or 2 streams
    if ((pkBindPoseNBTRef == 0) || (pkBindPoseNBTRef == pkBindPosePosRef))
    {
        m_kBindPoseStreams[0].SetDataSource(pkBindPosePosRef->GetDataStream());
        m_kBindPoseStreams[0].SetStride((NiUInt16)pkBindPosePosRef->GetStride());
        m_kBindPoseStreams[0].SetBlockCount(uiBlockCount);
        m_spDeformTask->AddInput(&m_kBindPoseStreams[0]);
    }
    else
    {
        m_kBindPoseStreams[0].SetDataSource(pkBindPosePosRef->GetDataStream());
        m_kBindPoseStreams[0].SetStride((NiUInt16)pkBindPosePosRef->GetStride());
        m_kBindPoseStreams[0].SetBlockCount(uiBlockCount);
        m_spDeformTask->AddInput(&m_kBindPoseStreams[0]);

        m_kBindPoseStreams[1].SetDataSource(pkBindPoseNBTRef->GetDataStream());
        m_kBindPoseStreams[1].SetStride((NiUInt16)pkBindPoseNBTRef->GetStride());
        m_kBindPoseStreams[1].SetBlockCount(uiBlockCount);
        m_spDeformTask->AddInput(&m_kBindPoseStreams[1]);
    }

    // Set outputs which requires either 1 or 2 streams
    if ((pkOutputNBTRef == 0) || (pkOutputNBTRef == pkOutputPosRef))
    {
        m_kOutputStreams[0].SetDataSource(pkOutputPosRef->GetDataStream());
        m_kOutputStreams[0].SetStride((NiUInt16)pkOutputPosRef->GetStride());
        m_kOutputStreams[0].SetBlockCount(uiBlockCount);
        m_spDeformTask->AddOutput(&m_kOutputStreams[0]);
    }
    else
    {
        m_kOutputStreams[0].SetDataSource(pkOutputPosRef->GetDataStream());
        m_kOutputStreams[0].SetStride((NiUInt16)pkOutputPosRef->GetStride());
        m_kOutputStreams[0].SetBlockCount(uiBlockCount);
        m_spDeformTask->AddOutput(&m_kOutputStreams[0]);

        m_kOutputStreams[1].SetDataSource(pkOutputNBTRef->GetDataStream());
        m_kOutputStreams[1].SetStride((NiUInt16)pkOutputNBTRef->GetStride());
        m_kOutputStreams[1].SetBlockCount(uiBlockCount);
        m_spDeformTask->AddOutput(&m_kOutputStreams[1]);
    }

    // Create and add skinning kernel
    if (pkOutputNBTRef == 0)
    {
        NiWiiSkinningKernelP* pkSkinningKernelP = NiNew NiWiiSkinningKernelP;
        NIASSERT(pkSkinningKernelP);
        m_spDeformTask->SetKernel(pkSkinningKernelP);
    }
    else
    {
        if (!bOutputBTStreams)
        {
            NiWiiSkinningKernelPN* pkSkinningKernelPN = NiNew NiWiiSkinningKernelPN;
            NIASSERT(pkSkinningKernelPN);
            m_spDeformTask->SetKernel(pkSkinningKernelPN);

        }
        else
        {
            NiWiiSkinningKernelPNBT* pkSkinningKernelPNBT = NiNew NiWiiSkinningKernelPNBT;
            NIASSERT(pkSkinningKernelPNBT);
            m_spDeformTask->SetKernel(pkSkinningKernelPNBT);
        }
    }

#endif // if defined(_WII)

    return true;
}
//---------------------------------------------------------------------------
template < NiUInt32 uiWeightsPerBone, typename TIndicies, typename TWeights,
    typename TPositions, NiUInt32 uiPositionCompCount, 
    typename TNBTs = float, NiUInt32 uiNBTsCompCount = 3>
class DeformVertexTemp
{
public:
#if defined(_PS3)
    inline NI_NOINLINE void Execute(
#else
    inline void Execute(
#endif
        NiPoint3& kVertex,
        NiPoint3& kNormal,
        NiMesh* pkMesh,
        NiUInt32 uiWhichSubmesh,
        NiUInt32 uiWhichVertex,
        NiSkinningMeshModifier* pkThis,
        NiTransform** ppkBoneWorlds,
        NiTransform* pkSkinToBoneTransforms)
    {
        // Get blend weights data for submesh.
        NiDataStreamElementLock kBlendWeightsLock(pkMesh, 
            NiCommonSemantics::BLENDWEIGHT(), 0,
#if !defined(_WII)
            ((uiWeightsPerBone == 3 && sizeof(TWeights) == sizeof(float)) ? 
            NiDataStreamElement::F_FLOAT32_3 :
            (uiWeightsPerBone == 4 && sizeof(TWeights) == sizeof(float)) ? 
            NiDataStreamElement::F_FLOAT32_4 :
            (uiWeightsPerBone == 4 && sizeof(TWeights) == sizeof(NiFloat16)) ? 
            NiDataStreamElement::F_FLOAT16_4 :
            NiDataStreamElement::F_FLOAT16_3),
#else
            ((uiWeightsPerBone == 3) ? NiDataStreamElement::F_FLOAT32_3 :
            NiDataStreamElement::F_NORMUINT8_4),
#endif
            NiDataStream::LOCK_READ);
        NIASSERT(kBlendWeightsLock.IsLocked());

        if (!kBlendWeightsLock.IsLocked())
            return;
        
        NiDataStreamElementLock kBlendIndicesLock(pkMesh, 
            NiCommonSemantics::BLENDINDICES(), 0, 
            ((sizeof(TIndicies) == sizeof(NiUInt8)) ? 
            NiDataStreamElement::F_UINT8_4 :
            NiDataStreamElement::F_INT16_4),
            NiDataStream::LOCK_READ);
        NIASSERT(kBlendIndicesLock.IsLocked());

        if (!kBlendIndicesLock.IsLocked())
            return;

        NiDataStreamElementLock kBindPositionLock(pkMesh, 
            NiCommonSemantics::POSITION_BP(), 0, 
            ((sizeof(TPositions) == sizeof(float)) ? 
            NiDataStreamElement::F_FLOAT32_3 :
            (uiPositionCompCount == 4) ? 
            NiDataStreamElement::F_FLOAT16_4 :
            NiDataStreamElement::F_FLOAT16_3), 
            NiDataStream::LOCK_READ);
        NIASSERT(kBindPositionLock.IsLocked());

        if (!kBindPositionLock.IsLocked())
            return;

        NiDataStreamElementLock kBindNormalLock(pkMesh, 
            NiCommonSemantics::NORMAL_BP(), 0,
#if !defined(_WII)
            ((sizeof(TNBTs) == sizeof(float)) ? 
            NiDataStreamElement::F_FLOAT32_3 :
            (uiNBTsCompCount == 4) ? 
            NiDataStreamElement::F_FLOAT16_4 :
            NiDataStreamElement::F_FLOAT16_3), 
#else
            ((sizeof(TNBTs) == sizeof(float)) ? NiDataStreamElement::F_FLOAT32_3 :
            NiDataStreamElement::F_NORMINT16_3),
#endif
            NiDataStream::LOCK_READ);

        NiDataStreamElementLock kBonePaletteLock(pkMesh, 
            NiCommonSemantics::BONE_PALETTE(), 0, 
            NiDataStreamElement::F_UINT16_1, NiDataStream::LOCK_READ);

        // Get the iterator for the weights
        NiTStridedRandomAccessIterator<
            NiTSimpleArray<TWeights, uiWeightsPerBone> > kBlendWeightsIter = 
                kBlendWeightsLock.begin<
                    NiTSimpleArray<TWeights, uiWeightsPerBone> >
                        (uiWhichSubmesh);

        // Get the iterator for the indices
        NiTStridedRandomAccessIterator<NiTSimpleArray<TIndicies, 4> >
            kBlendIndicesIter = kBlendIndicesLock.begin<
                    NiTSimpleArray<TIndicies, 4> >(uiWhichSubmesh);

        // Get the iterator for the bind positions
        NiTStridedRandomAccessIterator<NiTSimpleArray<TPositions, 
            uiPositionCompCount> > kBindPositionIter = 
            kBindPositionLock.begin<NiTSimpleArray<TPositions, 
                uiPositionCompCount> >(uiWhichSubmesh);

        // Get blend weights.
        float fWeight[4];
#if !defined(_WII)
        fWeight[0] = kBlendWeightsIter[uiWhichVertex][0];
        fWeight[1] = kBlendWeightsIter[uiWhichVertex][1];
        fWeight[2] = kBlendWeightsIter[uiWhichVertex][2];
        fWeight[3] = 1.0f - fWeight[0] - fWeight[1] - fWeight[2];
#else
        if (uiWeightsPerBone == 3)
        {
            fWeight[0] = kBlendWeightsIter[uiWhichVertex][0];
            fWeight[1] = kBlendWeightsIter[uiWhichVertex][1];
            fWeight[2] = kBlendWeightsIter[uiWhichVertex][2];
            fWeight[3] = 1.0f - fWeight[0] - fWeight[1] - fWeight[2];
        }
        else
        {
            NiWiiMeshUtilities::InitNormUInt8Cast();
            NiWiiMeshUtilities::NiUInt8ToFloat(
                (NiUInt8*)&(kBlendWeightsIter[uiWhichVertex][0]), &(fWeight[0]));
            NiWiiMeshUtilities::NiUInt8ToFloat(
                (NiUInt8*)&(kBlendWeightsIter[uiWhichVertex][1]), &(fWeight[1]));
            NiWiiMeshUtilities::NiUInt8ToFloat(
                (NiUInt8*)&(kBlendWeightsIter[uiWhichVertex][2]), &(fWeight[2]));
            NiWiiMeshUtilities::NiUInt8ToFloat(
                (NiUInt8*)&(kBlendWeightsIter[uiWhichVertex][3]), &(fWeight[3]));
        }
#endif  // if !defined(_WII)

        // Get blend indices.
        NiUInt16 uiBlendIndex0 = kBlendIndicesIter[uiWhichVertex][0];
        NiUInt16 uiBlendIndex1 = kBlendIndicesIter[uiWhichVertex][1];
        NiUInt16 uiBlendIndex2 = kBlendIndicesIter[uiWhichVertex][2];
        NiUInt16 uiBlendIndex3 = kBlendIndicesIter[uiWhichVertex][3];
        if (kBonePaletteLock.IsLocked())
        {
            // Get the iterator for the bind positions
            NiTStridedRandomAccessIterator<NiUInt16> kBonePaletteIter = 
                kBonePaletteLock.begin<NiUInt16>(uiWhichSubmesh);
            uiBlendIndex0 = kBonePaletteIter[uiBlendIndex0];
            uiBlendIndex1 = kBonePaletteIter[uiBlendIndex1];
            uiBlendIndex2 = kBonePaletteIter[uiBlendIndex2];
            uiBlendIndex3 = kBonePaletteIter[uiBlendIndex3];
        }

        // Compute bone matrices.
        NiTransform kWorldToSkin;
        pkThis->GetWorldToSkinTransform(kWorldToSkin);
        NiTransform kBoneTransform = kWorldToSkin *
            (*ppkBoneWorlds[uiBlendIndex0]) *
            pkSkinToBoneTransforms[uiBlendIndex0];
        NiMatrix3x4 kBone0 = kBoneTransform;
        kBoneTransform = kWorldToSkin * (*ppkBoneWorlds[uiBlendIndex1]) *
            pkSkinToBoneTransforms[uiBlendIndex1];
        NiMatrix3x4 kBone1 = kBoneTransform;
        kBoneTransform = kWorldToSkin * (*ppkBoneWorlds[uiBlendIndex2]) *
            pkSkinToBoneTransforms[uiBlendIndex2];
        NiMatrix3x4 kBone2 = kBoneTransform;
        kBoneTransform = kWorldToSkin * (*ppkBoneWorlds[uiBlendIndex3]) *
            pkSkinToBoneTransforms[uiBlendIndex3];
        NiMatrix3x4 kBone3 = kBoneTransform;

        // Deform vertex.
        NiMatrix3x4 kSkinBone = kBone0 * fWeight[0];
        kSkinBone += kBone1 * fWeight[1];
        kSkinBone += kBone2 * fWeight[2];
        kSkinBone += kBone3 * fWeight[3];

        kVertex = kSkinBone * NiPoint3(
            kBindPositionIter[uiWhichVertex][0],
            kBindPositionIter[uiWhichVertex][1], 
            kBindPositionIter[uiWhichVertex][2]);

        // Deform normal.
        if (kBindNormalLock.IsLocked())
        {
            // Get the iterator for the bind positions
            NiTStridedRandomAccessIterator<NiTSimpleArray<TNBTs,
                uiNBTsCompCount> > kBindNormalIter = 
                    kBindNormalLock.begin<NiTSimpleArray<TNBTs, 
                        uiNBTsCompCount> >(uiWhichSubmesh);

            // Clear translation from the transform.
            kSkinBone.m_kEntry[0].SetW(0.0f);
            kSkinBone.m_kEntry[1].SetW(0.0f);
            kSkinBone.m_kEntry[2].SetW(0.0f);

#if !defined(_WII)
            // Apply scale/rotation to the normal.
            kNormal = kSkinBone * NiPoint3(
                kBindNormalIter[uiWhichVertex][0],
                kBindNormalIter[uiWhichVertex][1], 
                kBindNormalIter[uiWhichVertex][2]);
#else
            if (sizeof(TNBTs) == sizeof(float))
            {
               kNormal = kSkinBone * NiPoint3(
                    kBindNormalIter[uiWhichVertex][0],
                    kBindNormalIter[uiWhichVertex][1], 
                    kBindNormalIter[uiWhichVertex][2]); 
            }
            else
            {
                NiPoint3 kNrm;

                NiWiiMeshUtilities::InitNormInt16Cast();
                NiWiiMeshUtilities::NiInt16ToFloat(
                    (NiInt16*)&(kBindNormalIter[uiWhichVertex][0]), &kNrm.x);
                NiWiiMeshUtilities::NiInt16ToFloat(
                    (NiInt16*)&(kBindNormalIter[uiWhichVertex][1]), &kNrm.y);
                NiWiiMeshUtilities::NiInt16ToFloat(
                    (NiInt16*)&(kBindNormalIter[uiWhichVertex][2]), &kNrm.z);

               kNormal = kSkinBone * kNrm;
            }
#endif  // if !defined(_WII)

            // Unitize the normal.
            kNormal.Unitize();
        }
    }
};
//---------------------------------------------------------------------------
void NiSkinningMeshModifier::DeformVertex(
    NiPoint3& kVertex,
    NiPoint3& kNormal,
    NiMesh* pkMesh,
    NiUInt32 uiWhichSubmesh,
    NiUInt32 uiWhichVertex)
{
#if defined(_WII)
    NIASSERT(NiGetModifier(NiSkinningMeshModifier, pkMesh) == this);

    NiWiiSkinningKernelData::ECombination eCombination = 
        m_kWiiSkinningKernelData.GetCombination();
    switch (eCombination)
    {
    case NiWiiSkinningKernelData::SKDC_BW08_4_BI08_4_POS32_3:
    case NiWiiSkinningKernelData::SKDC_BW08_4_BI08_4_POS32_3_NRM32_3_IPNBT:  
    case NiWiiSkinningKernelData::SKDC_BW08_4_BI08_4_POS32_3_NBT32_3_IPNBT:
        {
            DeformVertexTemp<4, NiUInt8, NiUInt8, float, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;

    case NiWiiSkinningKernelData::SKDC_BW08_4_BI08_4_POS32_3_NRM16_3:
    case NiWiiSkinningKernelData::SKDC_BW08_4_BI08_4_POS32_3_NBT16_3:
        {
            DeformVertexTemp<4, NiUInt8, NiUInt8, float, 3, NiUInt16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;

    case NiWiiSkinningKernelData::SKDC_BW32_3_BI08_4_POS32_3:
    case NiWiiSkinningKernelData::SKDC_BW32_3_BI08_4_POS32_3_NRM32_3_IPNBT:
    case NiWiiSkinningKernelData::SKDC_BW32_3_BI08_4_POS32_3_NBT32_3_IPNBT:
        {
            DeformVertexTemp<3, NiUInt8, float, float, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;

    case NiWiiSkinningKernelData::SKDC_BW32_3_BI08_4_POS32_3_NRM16_3:
    case NiWiiSkinningKernelData::SKDC_BW32_3_BI08_4_POS32_3_NBT16_3:
        {
            DeformVertexTemp<3, NiUInt8, float, float, 3, NiUInt16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;

    default:
        NIASSERT(false && "Unsupported skinning data combination");
    }

#else  // if defined(_WII)

    NIASSERT(NiGetModifier(NiSkinningMeshModifier, pkMesh) == this);

    ESkinningKernelDataCombination eCombination = 
        m_kSkinningKernelData.GetCombination();
    switch (eCombination)
    {
    case SKDC_3_08_32_16_4_16_4:
        {
            DeformVertexTemp<3, NiUInt8, float, NiFloat16, 4, NiFloat16, 4>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_3_16_32_16_4_16_4:
        {
            DeformVertexTemp<3, NiInt16, float, NiFloat16, 4, NiFloat16, 4>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_3_08_32_32_16_4:
        {
            DeformVertexTemp<3, NiUInt8, float, float, 3, NiFloat16, 4>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_3_16_32_32_16_4:
        {
            DeformVertexTemp<3, NiInt16, float,  float, 3, NiFloat16, 4>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_3_08_32_16_4_32:
        {
            DeformVertexTemp<3, NiUInt8, float, NiFloat16, 4, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_3_16_32_16_4_32:
        {
            DeformVertexTemp<3, NiInt16, float, NiFloat16, 4, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_3_08_32_32_32:
        {
            DeformVertexTemp<3, NiUInt8, float,  float, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_3_16_32_32_32:
        {
            DeformVertexTemp<3, NiInt16, float,  float, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_32_16_4_16_4:
        {
            DeformVertexTemp<4, NiUInt8, float, NiFloat16, 4, NiFloat16, 4>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_16_16_4_16_4:
        {
            DeformVertexTemp<4, NiUInt8, NiFloat16, NiFloat16, 4, NiFloat16, 4>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_32_16_4_16_4:
        {
            DeformVertexTemp<4, NiInt16, float, NiFloat16, 4, NiFloat16, 4>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_16_16_4_16_4:
        {
            DeformVertexTemp<4, NiInt16, NiFloat16, NiFloat16, 4, NiFloat16, 4>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_32_32_16_4:
        {
            DeformVertexTemp<4, NiUInt8, float,  float, 3, NiFloat16, 4>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_16_32_16_4:
        {
            DeformVertexTemp<4, NiUInt8, NiFloat16,  float, 3, NiFloat16, 4>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_32_32_16_4:
        {
            DeformVertexTemp<4, NiInt16, float,  float, 3, NiFloat16, 4>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_16_32_16_4:
        {
            DeformVertexTemp<4, NiInt16, NiFloat16,  float, 3, NiFloat16, 4>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_32_16_4_32:
        {
            DeformVertexTemp<4, NiUInt8, float, NiFloat16, 4, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_16_16_4_32:
        {
            DeformVertexTemp<4, NiUInt8, NiFloat16, NiFloat16, 4, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_32_16_4_32:
        {
            DeformVertexTemp<4, NiInt16, float, NiFloat16, 4, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_16_16_4_32:
        {
            DeformVertexTemp<4, NiInt16, NiFloat16, NiFloat16, 4, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_32_32_32:
        {
            DeformVertexTemp<4, NiUInt8, float,  float, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_16_32_32:
        {
            DeformVertexTemp<4, NiUInt8, NiFloat16,  float, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_32_32_32:
        {
            DeformVertexTemp<4, NiInt16, float,  float, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_16_32_32:
        {
            DeformVertexTemp<4, NiInt16, NiFloat16,  float, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
#if _PS3
    case SKDC_3_08_32_16_3_32:
        {
            DeformVertexTemp<3, NiUInt8, float, NiFloat16, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_3_16_32_16_3_32:
        {
            DeformVertexTemp<3, NiInt16, float, NiFloat16, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_32_16_3_32:
        {
            DeformVertexTemp<4, NiUInt8, float, NiFloat16, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_16_16_3_32:
        {
            DeformVertexTemp<4, NiUInt8, NiFloat16, NiFloat16, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_32_16_3_32:
        {
            DeformVertexTemp<4, NiInt16, float, NiFloat16, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_16_16_3_32:
        {
            DeformVertexTemp<4, NiInt16, NiFloat16, NiFloat16, 3, float, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_3_08_32_32_16_3:
        {
            DeformVertexTemp<3, NiUInt8, float, float, 3, NiFloat16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_3_16_32_32_16_3:
        {
            DeformVertexTemp<3, NiInt16, float,  float, 3, NiFloat16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_32_32_16_3:
        {
            DeformVertexTemp<4, NiUInt8, float,  float, 3, NiFloat16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_16_32_16_3:
        {
            DeformVertexTemp<4, NiUInt8, NiFloat16,  float, 3, NiFloat16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_32_32_16_3:
        {
            DeformVertexTemp<4, NiInt16, float,  float, 3, NiFloat16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_16_32_16_3:
        {
            DeformVertexTemp<4, NiInt16, NiFloat16,  float, 3, NiFloat16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_3_08_32_16_3_16_3:
        {
            DeformVertexTemp<3, NiUInt8, float, NiFloat16, 3, NiFloat16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_3_16_32_16_3_16_3:
        {
            DeformVertexTemp<3, NiInt16, float, NiFloat16, 3, NiFloat16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_32_16_3_16_3:
        {
            DeformVertexTemp<4, NiUInt8, float, NiFloat16, 3, NiFloat16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_08_16_16_3_16_3:
        {
            DeformVertexTemp<4, NiUInt8, NiFloat16, NiFloat16, 3, NiFloat16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_32_16_3_16_3:
        {
            DeformVertexTemp<4, NiInt16, float, NiFloat16, 3, NiFloat16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
    case SKDC_4_16_16_16_3_16_3:
        {
            DeformVertexTemp<4, NiInt16, NiFloat16, NiFloat16, 3, NiFloat16, 3>
                kFunctor;
            kFunctor.Execute(kVertex,kNormal, pkMesh, uiWhichSubmesh,
                uiWhichVertex, this, m_ppkBoneWorlds, 
                m_pkSkinToBoneTransforms);
        }
        break;
#endif
    default:
        NIASSERT(false && "Unsupported skinning data combination");
    }
#endif // !defined(_WII)
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiSkinningMeshModifier);
//---------------------------------------------------------------------------
void NiSkinningMeshModifier::CopyMembers(NiSkinningMeshModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiMeshModifier::CopyMembers(pkDest, kCloning);

    // Copy the mode flags
    pkDest->m_uFlags = m_uFlags;

    // Allocate memory for copies of the bones, etc...
    pkDest->Initialize(m_uiBoneCount);

    // Copy the root bone parent
    pkDest->m_pkRootBoneParent = m_pkRootBoneParent;
    pkDest->m_kRootBoneParentToSkinTransform = 
        m_kRootBoneParentToSkinTransform;

    // Copy the bones
    for (unsigned int i = 0; i < m_uiBoneCount; i++)
    {
        pkDest->m_apkBones[i] = m_apkBones[i];
    }

    // Copy bone transforms
    for (NiUInt32 i = 0; i < m_uiBoneCount; i++)
    {
        NiTransform& kTransform = m_pkSkinToBoneTransforms[i];
        pkDest->m_pkSkinToBoneTransforms[i] = kTransform;
    }

    // Copy bone bounds if they are not fixed
    if (GetBit(RECOMPUTE_BOUNDS))
    {
        pkDest->EnableDynamicBounds();
        NiBound* pkDestBounds = pkDest->GetBoneBounds();
        NIASSERT(pkDestBounds);

        NiMemcpy(pkDestBounds, m_pkBoneBounds, 
            sizeof(NiBound) * m_uiBoneCount);
    }

    // Don't copy m_pkBoneMatrices, m_ppkBoneWorlds, kernel data, as they are
    // allocated during Prepare
}
//---------------------------------------------------------------------------
void NiSkinningMeshModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiMeshModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    if (!bCloned)
        return;
    
    NiSkinningMeshModifier* pkDest = (NiSkinningMeshModifier*)pkClone;
    
    bCloned =
        kCloning.m_pkCloneMap->GetAt(pkDest->m_pkRootBoneParent, pkClone);
    if (bCloned)
        pkDest->m_pkRootBoneParent = (NiAVObject*)pkClone;

    for (unsigned int i = 0; i < m_uiBoneCount; i++)
    {
        NiAVObject* pkBone = pkDest->m_apkBones[i];
        bCloned = kCloning.m_pkCloneMap->GetAt(pkBone, pkClone);
        if (bCloned)
        {
            pkDest->m_apkBones[i] = (NiAVObject*)pkClone;
        }
    }
}
//---------------------------------------------------------------------------
    
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSkinningMeshModifier);
//---------------------------------------------------------------------------
void NiSkinningMeshModifier::LoadBinary(NiStream& kStream)
{
    NiMeshModifier::LoadBinary(kStream);

    // Read the flags
    NiStreamLoadBinary(kStream, m_uFlags);

    // Read m_pkRootBone link id
    kStream.ReadLinkID();

    // Read m_kRootBoneToBindSpace transform
    m_kRootBoneParentToSkinTransform.LoadBinary(kStream);

    // Read bone count and allocate memory
    NiStreamLoadBinary(kStream, m_uiBoneCount);
    Initialize(m_uiBoneCount);

    // Read the link id's for the NiAVObjects representing the bones
    for (NiUInt32 i = 0; i < m_uiBoneCount; i++)
    {
        kStream.ReadLinkID();
    }

    // Read BindToBoneSpace transforms
    for (NiUInt32 i = 0; i < m_uiBoneCount; i++)
    {
        NiTransform kTransform;
        kTransform.LoadBinary(kStream);
        m_pkSkinToBoneTransforms[i] = kTransform;
    }

    // Read bone bounds if not fixed at export
    if (GetBit(RECOMPUTE_BOUNDS))
    {
        EnableDynamicBounds();
        NiBound* akBounds = GetBoneBounds();
        for (NiUInt32 i = 0; i < m_uiBoneCount; i++)
        {
            akBounds[i].LoadBinary(kStream);
        }
    }
}
//---------------------------------------------------------------------------
void NiSkinningMeshModifier::LinkObject(NiStream& kStream)
{
    NiMeshModifier::LinkObject(kStream);

    // Link root bone
    m_pkRootBoneParent = (NiAVObject*)kStream.GetObjectFromLinkID();

    // Link other bones
    NIASSERT(m_apkBones);
    for (NiUInt32 i = 0; i < m_uiBoneCount; i++)
    {
        m_apkBones[i] = (NiAVObject*)kStream.GetObjectFromLinkID();
    }
}
//---------------------------------------------------------------------------
bool NiSkinningMeshModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiMeshModifier::RegisterStreamables(kStream))
        return false;

    // Don't register root bone or array of bones because this class does
    // not hold references to them. If they are registered by this class, 
    // then they may be deleted during streaming without this class knowing
    // about the deletion.

    return true;
}
//---------------------------------------------------------------------------
void NiSkinningMeshModifier::SaveBinary(NiStream& kStream)
{
    NiMeshModifier::SaveBinary(kStream);

    // Save the flags
    NiStreamSaveBinary(kStream, m_uFlags);

    // Save root bone link
    kStream.SaveLinkID(m_pkRootBoneParent);
    
    // Save root bone transform
    m_kRootBoneParentToSkinTransform.SaveBinary(kStream);

    // Save Bone count
    NiStreamSaveBinary(kStream, m_uiBoneCount);
    
    // Save bones
    for (NiUInt32 i = 0; i < m_uiBoneCount; i++)
    {
        kStream.SaveLinkID(m_apkBones[i]);
    }

    // Save BindToBoneSpace transforms
    for (NiUInt32 i = 0; i < m_uiBoneCount; i++)
    {
        m_pkSkinToBoneTransforms[i].SaveBinary(kStream);
    }

    // Save the bone bounds if needed
    if (GetBit(RECOMPUTE_BOUNDS))
    {
        NIASSERT(m_pkBoneBounds);
        for (NiUInt32 i = 0; i < m_uiBoneCount; i++)
        {
            m_pkBoneBounds[i].SaveBinary(kStream);
        }
    }
}
//---------------------------------------------------------------------------
bool NiSkinningMeshModifier::IsEqual(NiObject* pkObject)
{
    if (!NiMeshModifier::IsEqual(pkObject))
        return false;
        
    if (!NiIsKindOf(NiSkinningMeshModifier, pkObject))
        return false;
        
    NiSkinningMeshModifier* pkOther = (NiSkinningMeshModifier*)pkObject;

    // Compare the per-modifier members
    if ((m_uFlags != pkOther->m_uFlags) ||
        (m_uiBoneCount != pkOther->m_uiBoneCount) ||
        (m_kRootBoneParentToSkinTransform != 
            pkOther->m_kRootBoneParentToSkinTransform))
    {
        return false;
    }

    // Note: the root bone parent is not compared, as it could be the same
    // mesh or a parent of the mesh that contains this modifier.  Thus,
    // calling IsEqual on the root bone parent could result in infinite
    // recursion.
    //
    // Theoretically, any of the bones could also have this issue, but that
    // would result in bad transforms anyways and thus is very unlikely.

    // Compare the per-bone members
    for (NiUInt32 ui = 0; ui < m_uiBoneCount; ++ui)
    {
        if (m_apkBones[ui]->GetParent() == m_pkRootBoneParent)
        {
            if (!pkOther->m_apkBones[ui]->GetParent() ||
                pkOther->m_apkBones[ui]->GetParent() != 
                pkOther->m_pkRootBoneParent)
            {
                return false;
            }

            continue;
        }

        if (!m_apkBones[ui]->IsEqual(pkOther->m_apkBones[ui]) ||
            (m_pkSkinToBoneTransforms[ui] !=
            pkOther->m_pkSkinToBoneTransforms[ui]))
        {
            return false;
        }

        if (GetBit(RECOMPUTE_BOUNDS))
        {
            if (m_pkBoneBounds[ui] != pkOther->m_pkBoneBounds[ui])
                return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSkinningMeshModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiMeshModifier::GetViewerStrings(pkStrings);
    pkStrings->Add(NiGetViewerString(ms_RTTI.GetName()));

    // m_uFlags
    pkStrings->Add(NiGetViewerString("Software Skinned",
        GetBit(USE_SOFTWARE_SKINNING)));
    pkStrings->Add(NiGetViewerString("Recompute Bounds",
        GetBit(RECOMPUTE_BOUNDS)));

    // m_pkRootBoneParent
    NiAVObject* pkParent = GetRootBoneParent(); 
    pkStrings->Add(NiGetViewerString("Root Bone Parent",
        (pkParent != NULL) ? (const char *)pkParent->GetName() : "(null)"));

    // m_uiBoneCount
    const NiUInt32 uiBoneCount = GetBoneCount();
    pkStrings->Add(NiGetViewerString("Bone Count", uiBoneCount));

    // m_apkBones
    for (NiUInt32 ui = 0; ui < uiBoneCount; ++ui)
    {
        const NiFixedString& kBoneName = m_apkBones[ui]->GetName();

        char acBone[10];
        NiSprintf(acBone, 10, "  %3d", ui);
        pkStrings->Add(NiGetViewerString(acBone,
            (kBoneName == NULL) ? "<noname>" : (const char *)kBoneName));
    }
}
//---------------------------------------------------------------------------

