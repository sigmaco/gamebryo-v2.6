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

#include "NiMesh.h"
#include "NiMorphMeshModifier.h"
#include "NiMorphingKernel.h"
#include "NiCloningProcess.h"
#include "NiRenderObjectMaterialOption.h"
#include "NiMeshUtilities.h"

#include <NiFloat16.h>

NiImplementRTTI(NiMorphMeshModifier, NiMeshModifier);

//---------------------------------------------------------------------------
NiMorphMeshModifier::NiMorphMeshModifier(NiUInt16 uiNumTargets)
{
    m_uiNumTargets = uiNumTargets;
    m_uFlags = 0;
    m_pkWeightsSPStream = 0;
}
//---------------------------------------------------------------------------
NiMorphMeshModifier::~NiMorphMeshModifier()
{
    NiUInt32 uiNumElements = m_kElementData.GetSize();
    for (NiUInt32 ui = 0; ui < uiNumElements; ui++)
    {
        ElementData* pkData = m_kElementData.GetAt(ui);

        pkData->m_spTask = 0;
        pkData->m_spWorkflow = 0;

        NiUInt32 uiNumTargets = pkData->m_kTargetSPStreams.GetSize();
        for (NiUInt32 uj = 0; uj < uiNumTargets; uj++)
            NiDelete pkData->m_kTargetSPStreams.GetAt(uj);

        NiDelete pkData->m_pkMorphingKernel;

        NiDelete pkData;
    }

    NiDelete m_pkWeightsSPStream;
    m_pkWeightsSPStream = 0;
}
//---------------------------------------------------------------------------
bool NiMorphMeshModifier::RequiresMaterialOption(
    const NiFixedString& kMaterialOption, bool& bResult) const
{
    if (kMaterialOption == NiRenderObjectMaterialOption::MORPHING())
    {
        bResult = false;
        return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Set up functions, must be called before AreRequirementsMet
//---------------------------------------------------------------------------
NiUInt32 NiMorphMeshModifier::AddMorphedElement(
    const NiFixedString& kSemantic, NiUInt32 uiSemanticIdx)
{
    // We only support 32 morphed targets
    if (m_kElementData.GetSize() >= MAX_MORPH_TARGETS)
    {
        NILOG("Too many morphed targets added. The maximum supported is %d",
            MAX_MORPH_TARGETS);
        return UINT_MAX;
    }

    ElementData* pkData = NiNew ElementData();
    
    pkData->m_kSemantic = kSemantic;
    pkData->m_uiSemanticIdx = uiSemanticIdx;
    pkData->m_pkMorphingKernel = 0;
    pkData->m_spTask = 0;
    pkData->m_spWorkflow = 0;
        
    if (kSemantic == NiCommonSemantics::NORMAL())
    {
        pkData->m_uiNormalizeFlag = 1;
    }
    else
    {
        pkData->m_uiNormalizeFlag = 0;
    }

    return m_kElementData.Add(pkData);
}
//---------------------------------------------------------------------------
bool NiMorphMeshModifier::IsMorphedElement(const NiFixedString& kSemantic, 
    NiUInt32 uiSemanticIdx, NiUInt32& uiIndexReturn) const
{
    NiUInt32 uiCount = m_kElementData.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        ElementData* pkData = m_kElementData.GetAt(ui);
        if (pkData->m_kSemantic == kSemantic &&
            pkData->m_uiSemanticIdx == uiSemanticIdx)
        {
            uiIndexReturn = ui;
            return true;
        }
    }
    
    return false;
}
//---------------------------------------------------------------------------
bool NiMorphMeshModifier::GetMorphedElement(
    const NiUInt32 uiIndex, NiFixedString& kSemantic, 
    NiUInt32& uiSemanticIdx)
{
    if (uiIndex < m_kElementData.GetSize())
    {
        ElementData* pkData = m_kElementData.GetAt(uiIndex);
        NIASSERT(pkData);
        kSemantic = pkData->m_kSemantic;
        uiSemanticIdx = pkData->m_uiSemanticIdx;
        return true;
    }
       
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Requirements functionality
//---------------------------------------------------------------------------
bool NiMorphMeshModifier::AreRequirementsMet(NiMesh* pkMesh,
    NiSystemDesc::RendererID eRenderer) const
{
    // Validate setup.
    NiDataStreamRef* pkDSRef = 0;

    // Check for weights
    pkDSRef = pkMesh->FindStreamRef(NiCommonSemantics::MORPHWEIGHTS(), 0,
        NiDataStreamElement::F_UNKNOWN);
    if (!pkDSRef)
    {
        NILOG("NiMorphMeshModifier: There must be a MORPHWEIGHTS data stream "
            " on the output mesh.\n");
        return false;
    }
    if (!pkDSRef->GetAccessMask() & NiDataStream::ACCESS_CPU_READ)
    {
        NILOG("NiMorphMeshModifier: The MORPHWEIGHTS data stream must "
            "be CPU readable data.\n");
        return false;
    }
    if (pkDSRef->GetTotalCount() < m_uiNumTargets)
    {
        NILOG("NiMorphMeshModifier: The MORPHWEIGHTS data stream contains "
            "too few entries for the given number of morph targets.\n");
        return false;
    }
    NiDataStreamElement kMeshFormat = pkDSRef->GetElementDescAt(0);
    if (kMeshFormat.GetFormat() != NiDataStreamElement::F_FLOAT32_1)
    {
        NILOG("NiMorphMeshModifier: Morph weights must be of format "
            "F_FLOAT32_1.\n");
        return false;
    }

    if (GetUpdateNormals())
    {
        pkDSRef = pkMesh->FindStreamRef(NiCommonSemantics::INDEX(), 0);
        if (!pkDSRef ||
            !(pkDSRef->GetAccessMask() & NiDataStream::ACCESS_CPU_READ))
        {
            char acMsg[256];
            NiSprintf(acMsg, 256,
                "NiMorphMeshModifier: The %s data stream must exist and "
                "be CPU readable data if normals are to be recomputed.\n",
                (const char*)NiCommonSemantics::INDEX);
            NILOG(acMsg);
            return false;
        }

        const NiFixedString& kPosnSemantic = GetSkinned() ?
            NiCommonSemantics::POSITION_BP() : NiCommonSemantics::POSITION();
        pkDSRef = pkMesh->FindStreamRef(kPosnSemantic, 0);
        if (!pkDSRef ||
            !(pkDSRef->GetAccessMask() & NiDataStream::ACCESS_CPU_READ))
        {
            char acMsg[256];
            NiSprintf(acMsg, 256,
                "NiMorphMeshModifier: The %s data stream must exist and "
                "be CPU readable data if normals are to be recomputed.\n",
                (const char*)kPosnSemantic);
            NILOG(acMsg);
            return false;
        }

        const NiFixedString& kNormSemantic = GetSkinned() ?
            NiCommonSemantics::NORMAL_BP() : NiCommonSemantics::NORMAL();
        pkDSRef = pkMesh->FindStreamRef(kNormSemantic, 0);
        if (!pkDSRef ||
            !((pkDSRef->GetAccessMask() & NiDataStream::ACCESS_CPU_READ) &&
            (pkDSRef->GetAccessMask() &
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE)))
        {
            char acMsg[256];
            NiSprintf(acMsg, 256,
                "NiMorphMeshModifier: The %s data stream must exist and be "
                "CPU readable and writable if normals are to be recomputed.\n",
                (const char*)kNormSemantic);
            NILOG(acMsg);
            return false;
        }
    }
        
    // Work through the morphed elements
    NiUInt32 uiElementCount = m_kElementData.GetSize();
    for (NiUInt32 uiElement = 0; uiElement < uiElementCount; uiElement++)
    {
        ElementData* pkData = m_kElementData.GetAt(uiElement);

        // Find the matching output element on the NiMesh object.
        pkDSRef = pkMesh->FindStreamRef(pkData->m_kSemantic,
            pkData->m_uiSemanticIdx);
        if (!pkDSRef)
        {
            NILOG("NiMorphMeshModifier: Expected %s(%d) data stream.\n", 
                (const char*)pkData->m_kSemantic,
                pkData->m_uiSemanticIdx);
            return false;
        }

        // Right now we restrict ourselves to F_FLOAT32_3 or F_FLOAT16_4 
        // format. This limitation could be removed if there were kernels for 
        // different types.

        kMeshFormat = pkDSRef->GetElementDescAt(0);
        if (kMeshFormat.GetFormat() != NiDataStreamElement::F_FLOAT32_3 &&
           kMeshFormat.GetFormat() != NiDataStreamElement::F_FLOAT16_4 &&
           !((eRenderer == NiSystemDesc::RENDERER_PS3 ||
              eRenderer == NiSystemDesc::RENDERER_GENERIC) &&
              kMeshFormat.GetFormat() == NiDataStreamElement::F_FLOAT16_3))
        {
            NILOG("NiMorphMeshModifier: Morphed NiMesh object elements must "
                "be of format F_FLOAT32_3 or F_FLOAT16_4 or F_FLOAT16_3 (PS3 "
                "only).\n");
            return false;
        }

        // Verify access masks
        if (!(pkDSRef->GetAccessMask() &
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE ||
            pkDSRef->GetAccessMask() &
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE))
        {
            NILOG("NiMorphMeshModifier: Morphed NiMesh object elements must "
                "be accessible as writable data.\n");
            return false;
        }

        // Look for morph target data
        NiFixedString kTargetSemantic = MorphElementSemantic(
            pkData->m_kSemantic);
        
        // Find out how many targets we have
        NiInt32 iMaxIndex = pkMesh->GetMaxIndexBySemantic(kTargetSemantic);
        if (-1 == iMaxIndex)
        {
            NILOG("NiMorphMeshModifier: No targets for morphed semantic.\n");
            return false;
        }
        if (m_uiNumTargets != (NiUInt32)iMaxIndex + 1)
        {
            NILOG("NiMorphMeshModifier: All morphed semantics on a mesh must "
                "have the same number of morph targets.\n");
            return false;
        }

        // Make sure we do not exceed the limit for this renderer.
        switch (eRenderer)
        {
        case NiSystemDesc::RENDERER_GENERIC:
        case NiSystemDesc::RENDERER_PS3:
            {
                // 20 Morph targets is the maximum amount for PS3. This is a 
                // hardware limit dictated by the SPURS. The "generic" renderer
                // uses this limit to maintain validity on all platforms.
                if (m_uiNumTargets > 20)
                {
                    NILOG("NiMorphMeshModifier: Number of morph targets "
                        "exceed the limit for this renderer.\n");
                    return false;
                }
                break;
            }
        default:
            {
                if (m_uiNumTargets > MAX_MORPH_TARGETS)
                {
                    NILOG("NiMorphMeshModifier: Number of morph targets "
                        "exceed the limit for this renderer.\n");
                    return false;
                }
                break;
            }
        }

        NiDataStreamElement kTargetFormat;
        for (NiUInt32 uiTarget = 0; uiTarget < m_uiNumTargets; uiTarget++)
        {
            pkDSRef = pkMesh->FindStreamRef(kTargetSemantic, uiTarget);
            if (!pkDSRef)
            {
                NILOG("NiMorphMeshModifier: Missing data for a morph target, "
                    "or data in the wrong format.\n");
                return false;
            }

            if (kTargetFormat.GetFormat() == NiDataStreamElement::F_UNKNOWN)
            {
                kTargetFormat = pkDSRef->GetElementDescAt(0);

                if (kTargetFormat.GetFormat() != 
                    NiDataStreamElement::F_FLOAT32_3 &&
                    kTargetFormat.GetFormat() != 
                    NiDataStreamElement::F_FLOAT16_4 &&
                    !((eRenderer == NiSystemDesc::RENDERER_PS3 ||
                       eRenderer == NiSystemDesc::RENDERER_GENERIC) &&
                       kTargetFormat.GetFormat() == 
                       NiDataStreamElement::F_FLOAT16_3))
                {
                    NILOG("NiMorphMeshModifier: Morphed NiMesh object "
                        "elements must be of format F_FLOAT32_3 or "
                        "F_FLOAT16_4 or F_FLOAT16_3 (PS3 only).\n");
                    return false;
                }
            }
            else
            {
                if (kTargetFormat.GetFormat() != 
                    pkDSRef->GetElementDescAt(0).GetFormat())
                {
                    NILOG("NiMorphMeshModifier: Morphed NiMesh object "
                        "targets must be in the same format.\n");
                    return false;
                }
            }
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiMorphMeshModifier::RetrieveRequirements(NiMeshRequirements&
    kRequirements) const
{
    NiSystemDesc::RendererID eRenderer = kRequirements.GetRenderer();
    NiUInt32 uiReqIndex;

    NiUInt32 uiSet1Index = kRequirements.CreateNewRequirementSet();
    NiUInt32 uiMorphTargetStreamID = 1;
    NiUInt32 uiVertexDataStreamID = 2;
    NiUInt32 uiStreamID = 3;

    // If exporting for the Wii, we want to allow interleaved streams for morphing, 
    // since skinned objects require interleaved bind poses on the Wii.
    bool bAllowInterleavedStreams;
    if (eRenderer == NiSystemDesc::RENDERER_WII)
        bAllowInterleavedStreams = true;
    else
        bAllowInterleavedStreams = false;

    NiMeshRequirements::StreamBehavior eMorphStreamBehaviour;
    if (bAllowInterleavedStreams)
        eMorphStreamBehaviour = NiMeshRequirements::FLOATER;
    else
        eMorphStreamBehaviour = NiMeshRequirements::STRICT_INTERLEAVE;

    // Weights
    uiReqIndex = kRequirements.AddRequirement(uiSet1Index, NiCommonSemantics::
        MORPHWEIGHTS(), 0, NiMeshRequirements::STRICT_INTERLEAVE,
        uiStreamID++, NiDataStream::USAGE_USER, NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE, NiDataStreamElement::
        F_FLOAT32_1);
    NiMeshRequirements::SemanticRequirement* pkWeightsReq =
        kRequirements.GetRequirement(uiSet1Index, uiReqIndex);

    const NiFixedString& kPosnSemantic = GetSkinned() ?
        NiCommonSemantics::POSITION_BP() : NiCommonSemantics::POSITION();
    const NiFixedString& kNormSemantic = GetSkinned() ?
        NiCommonSemantics::NORMAL_BP() : NiCommonSemantics::NORMAL();

    // Figure out if we need normal re computation
    NiUInt32 uiSet2Index = (NiUInt32)~0;
    NiMeshRequirements::SemanticRequirement* pkPosnReq = 0;
    NiMeshRequirements::SemanticRequirement* pkNormReq = 0;
    NiMeshRequirements::SemanticRequirement* pkIndexReq = 0;
    NiMeshRequirements::SemanticRequirement* pkTargReq = 0;
    if (GetUpdateNormals())
    {
        NiUInt32 uiIndicesStreamID, uiPositionStreamID, uiNormalStreamID;
        NiUInt32 uiNormalShareIndexStreamID, uiNormalShareGroupStreamID;
        if (bAllowInterleavedStreams)
        {
            uiIndicesStreamID = uiStreamID++;
            uiPositionStreamID = uiVertexDataStreamID;
            uiNormalStreamID = uiVertexDataStreamID;
            uiNormalShareIndexStreamID = uiStreamID++;
            uiNormalShareGroupStreamID = uiStreamID++;
        }
        else
        {
            uiIndicesStreamID = uiStreamID++;
            uiPositionStreamID = uiStreamID++;
            uiNormalStreamID = uiStreamID++;
            uiNormalShareIndexStreamID = uiStreamID++;
            uiNormalShareGroupStreamID = uiStreamID++;
        }
        
        // Indices
        uiReqIndex = kRequirements.AddRequirement(uiSet1Index,
            NiCommonSemantics::INDEX(), 0, NiMeshRequirements::CAN_SPLIT,
            uiIndicesStreamID, NiDataStream::USAGE_VERTEX_INDEX, NiDataStream::
            ACCESS_CPU_READ, NiDataStreamElement::F_UNKNOWN);
        pkIndexReq = kRequirements.GetRequirement(uiSet1Index, uiReqIndex);

        // Position
        uiReqIndex = kRequirements.AddRequirement(uiSet1Index,
            kPosnSemantic, 0, eMorphStreamBehaviour,
            uiPositionStreamID, NiDataStream::USAGE_VERTEX, NiDataStream::
            ACCESS_CPU_READ | NiDataStream::
            ACCESS_CPU_WRITE_MUTABLE, NiDataStreamElement::F_UNKNOWN);
        pkPosnReq = kRequirements.GetRequirement(uiSet1Index, uiReqIndex);

        // Normal
        uiReqIndex = kRequirements.AddRequirement(uiSet1Index,
            kNormSemantic, 0, eMorphStreamBehaviour,
            uiNormalStreamID, NiDataStream::USAGE_VERTEX, NiDataStream::
            ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_MUTABLE, 
            NiDataStreamElement::F_UNKNOWN);
        pkNormReq = kRequirements.GetRequirement(uiSet1Index, uiReqIndex);

        // Create a second set with the normal recalculation streams
        uiSet2Index = kRequirements.CreateNewRequirementSet();

        // Index, Position, Normal, Weights, and Flags for set 2.
        kRequirements.AddRequirement(uiSet2Index, pkIndexReq);
        kRequirements.AddRequirement(uiSet2Index, pkPosnReq);
        kRequirements.AddRequirement(uiSet2Index, pkNormReq);
        kRequirements.AddRequirement(uiSet2Index, pkWeightsReq);

        // Normal Share Index
        kRequirements.AddRequirement(uiSet2Index, NiCommonSemantics::
            NORMALSHAREINDEX(), 0, NiMeshRequirements::CAN_SPLIT,
            uiNormalShareIndexStreamID, NiDataStream::USAGE_VERTEX, NiDataStream::
            ACCESS_CPU_READ, NiDataStreamElement::F_UINT32_1);

        // Normal Share Group
        kRequirements.AddRequirement(uiSet2Index, NiCommonSemantics::
            NORMALSHAREGROUP(), 0, NiMeshRequirements::CAN_SPLIT,
            uiNormalShareGroupStreamID, NiDataStream::USAGE_USER,
            NiDataStream::ACCESS_CPU_READ, NiDataStreamElement::F_UINT32_1);
    }

    // Work through the morphed elements
    NiUInt32 uiElementCount = m_kElementData.GetSize();
    for (NiUInt32 uiElement = 0; uiElement < uiElementCount; uiElement++)
    {
        ElementData* pkData = m_kElementData.GetAt(uiElement);

        const NiFixedString& kSemantic = pkData->m_kSemantic;
        const NiUInt8 kIndex = (NiUInt8) pkData->m_uiSemanticIdx;
        
        // there may already have been requirements added for position
        // and normal - if these are encountered, then modify the existing
        // requirement instead of creating a new one.
        if (pkPosnReq && kSemantic == kPosnSemantic && kIndex == 0)
        {
            pkPosnReq->m_kFormats.RemoveAll();
            pkPosnReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT32_3);
            pkPosnReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
            if (eRenderer == NiSystemDesc::RENDERER_PS3 ||
                eRenderer == NiSystemDesc::RENDERER_GENERIC)
                pkPosnReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
            if (!bAllowInterleavedStreams)
                pkPosnReq->m_eStreamBehavior = NiMeshRequirements::STRICT_INTERLEAVE;
        }
        else if (pkNormReq && kSemantic == kNormSemantic && kIndex == 0)
        {
            pkNormReq->m_kFormats.RemoveAll();
            pkNormReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT32_3);
            pkNormReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
            if (eRenderer == NiSystemDesc::RENDERER_PS3 ||
                eRenderer == NiSystemDesc::RENDERER_GENERIC)
                pkNormReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
            if (!bAllowInterleavedStreams)
                pkNormReq->m_eStreamBehavior = NiMeshRequirements::STRICT_INTERLEAVE;
        }
        else
        {
            // Target
            NiUInt8 uiAccess = static_cast<NiUInt8>(                
                GetSWSkinned() ?
                NiDataStream::ACCESS_CPU_WRITE_MUTABLE :
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE);

            if (!bAllowInterleavedStreams)
                uiVertexDataStreamID = uiStreamID++;
            uiReqIndex = kRequirements.AddRequirement(uiSet1Index, kSemantic,
                kIndex, eMorphStreamBehaviour, uiVertexDataStreamID,
                NiDataStream::USAGE_VERTEX, 
                uiAccess, NiDataStreamElement::F_FLOAT32_3);
            pkTargReq = kRequirements.GetRequirement(uiSet1Index, uiReqIndex);
            pkTargReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
            if (eRenderer == NiSystemDesc::RENDERER_PS3 ||
                eRenderer == NiSystemDesc::RENDERER_GENERIC)
                pkTargReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);
            
            if (GetUpdateNormals())
            {
                // Target
                kRequirements.AddRequirement(uiSet2Index, pkTargReq);
            }
        }
            
        // Look for morph target data
        NiFixedString kTargetSemantic = MorphElementSemantic(kSemantic);
        for (NiUInt32 uiTarget = 0; uiTarget < m_uiNumTargets; uiTarget++)
        {
            if (!bAllowInterleavedStreams)
                uiMorphTargetStreamID = uiStreamID++;

            uiReqIndex = kRequirements.AddRequirement(uiSet1Index,
                kTargetSemantic, uiTarget,
                eMorphStreamBehaviour, uiMorphTargetStreamID,
                NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_CPU_READ,
                NiDataStreamElement::F_FLOAT32_3);
            pkTargReq = kRequirements.GetRequirement(uiSet1Index,
                uiReqIndex);
            pkTargReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_4);
            if (eRenderer == NiSystemDesc::RENDERER_PS3 ||
                eRenderer == NiSystemDesc::RENDERER_GENERIC)
                pkTargReq->m_kFormats.Add(NiDataStreamElement::F_FLOAT16_3);

            if (GetUpdateNormals())
            {
                kRequirements.AddRequirement(uiSet2Index, pkTargReq);
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Attach and detach functionality
//---------------------------------------------------------------------------
bool NiMorphMeshModifier::Attach(NiMesh* pkMesh)
{
    // Set sync points
    if (!ResetSyncPoints(pkMesh))
        return false;

    // Set up SP stream for weights
    NiDataStreamRef* pkWeightsDataStreamRef =
        pkMesh->FindStreamRef(NiCommonSemantics::MORPHWEIGHTS(), 0);
    NIASSERT(pkWeightsDataStreamRef);
    NIASSERT(pkWeightsDataStreamRef->GetTotalCount() >= m_uiNumTargets);

    NIASSERT(!m_pkWeightsSPStream);
    m_pkWeightsSPStream = NiNew NiTSPFixedInput<float>();
    m_pkWeightsSPStream->SetDataSource(
        pkWeightsDataStreamRef->GetDataStream());
    m_pkWeightsSPStream->SetBlockCount(
        pkWeightsDataStreamRef->GetTotalCount());

    // Add a task for each morphed element.
    NiDataStreamRef* pkDSRef = 0;
    const NiUInt32 uiElementCount = m_kElementData.GetSize();
    for (NiUInt32 uiElement = 0; uiElement < uiElementCount; uiElement++)
    {
        ElementData* pkData = m_kElementData.GetAt(uiElement);

        // Get new task
        pkData->m_spTask = NiSPTask::GetNewTask(m_uiNumTargets + 2, 1);

        // Set Kernel
        pkData->m_pkMorphingKernel = NiNew NiMorphingKernel;
        NIASSERT(pkData->m_pkMorphingKernel);
        pkData->m_spTask->SetKernel(pkData->m_pkMorphingKernel);

        // Add output
        NiDataStreamElement kSourceElement;
        pkMesh->FindStreamRefAndElementBySemantic(
            pkData->m_kSemantic, pkData->m_uiSemanticIdx,
            NiDataStreamElement::F_UNKNOWN, pkDSRef, kSourceElement);
        NIASSERT(pkDSRef);

        pkData->m_kOutputSPStream.SetDataSource(
            pkDSRef->GetDataStream(), false, 0, kSourceElement.GetOffset());
        pkData->m_kOutputSPStream.SetStride((NiUInt16)pkDSRef->GetStride());
        pkData->m_kOutputSPStream.SetBlockCount(pkDSRef->GetTotalCount());
        pkData->m_spTask->AddOutput(&pkData->m_kOutputSPStream);

        // Add the inputs

        // Add input: targets
        NiFixedString kTargetSemantic =
            MorphElementSemantic(pkData->m_kSemantic);

        NiUInt32 uiTargetElementStride = 0;
        NiUInt32 uiTargetComponentSize = 0;
        for (NiUInt32 uiTarget = 0; uiTarget < m_uiNumTargets; uiTarget++)
        {
            NiDataStreamElement kTargetElement;
            pkMesh->FindStreamRefAndElementBySemantic(
                kTargetSemantic, uiTarget, NiDataStreamElement::F_UNKNOWN, 
                pkDSRef, kTargetElement);
            NIASSERT(pkDSRef);

            if (!uiTargetElementStride)
            {
                uiTargetComponentSize = kTargetElement.GetComponentSize();
                uiTargetElementStride = uiTargetComponentSize *
                    kTargetElement.GetComponentCount();
            }
            else
            {
                NIASSERT(uiTargetElementStride == 
                    (NiUInt32)(kTargetElement.GetComponentSize() * 
                        kTargetElement.GetComponentCount()));
            }

            NiSPStream* pkSPStream = NiNew NiSPStream;
            pkSPStream->SetDataSource(
                pkDSRef->GetDataStream(), false, 0, kTargetElement.GetOffset());
            pkSPStream->SetStride((NiUInt16)pkDSRef->GetStride());
            pkSPStream->SetBlockCount(pkDSRef->GetTotalCount());
            pkData->m_spTask->AddInput(pkSPStream);
            pkData->m_kTargetSPStreams.Add(pkSPStream);
        }

        // Add input: weights
        pkData->m_spTask->AddInput(m_pkWeightsSPStream);

        // Set the combination
        pkData->m_kMorphingKernelData.SetCombination(
            kSourceElement.GetComponentSize(), 
            uiTargetComponentSize, pkData->m_uiNormalizeFlag == 0 ? false : true);
        
        // Add input, fixed parameter controlling normalization
        pkData->m_kMorphingKernelDataStream.SetData(
            &pkData->m_kMorphingKernelData);
        pkData->m_kMorphingKernelDataStream.SetBlockCount(1);
        pkData->m_spTask->AddInput(&pkData->m_kMorphingKernelDataStream);

        pkData->m_spTask->SetIsCompacted(true);
    }
    
    SetNeedsUpdate(false);
    SetNeedsCompletion(false);

    return true;
}
//---------------------------------------------------------------------------
bool NiMorphMeshModifier::Detach(NiMesh*)
{
    m_kSubmitPoints.RemoveAll();
    m_kCompletePoints.RemoveAll();

    // Remove the task and streams for each morphed element
    const NiUInt32 uiElementCount = m_kElementData.GetSize();
    for (NiUInt32 uiElement = 0; uiElement < uiElementCount; uiElement++)
    {
        ElementData* pkData = m_kElementData.GetAt(uiElement);

        if (!pkData->m_spTask)
            continue;

        pkData->m_spTask = 0;

        NiDelete pkData->m_pkMorphingKernel;
        pkData->m_pkMorphingKernel = 0;

        pkData->m_kOutputSPStream.ClearTaskArrays();

        pkData->m_kMorphingKernelDataStream.ClearTaskArrays();

        for (NiUInt32 uiTarget = 0; uiTarget < m_uiNumTargets; uiTarget++)
        {
            NiDelete pkData->m_kTargetSPStreams.GetAt(uiTarget);
        }
        pkData->m_kTargetSPStreams.RemoveAll();
    }

    NiDelete m_pkWeightsSPStream;
    m_pkWeightsSPStream = 0;

    return true;
}
//---------------------------------------------------------------------------
bool NiMorphMeshModifier::SubmitTasks(NiMesh* pkMesh,
    NiSyncArgs*, NiSPWorkflowManager* pkWFManager)
{
    if (!GetNeedsUpdate())
        return false;
        
    // Complete any outstanding workflow.
    const NiUInt32 uiElementCount = m_kElementData.GetSize();
    if (GetNeedsCompletion())
    {
        for (NiUInt32 uiElement = 0; uiElement < uiElementCount; uiElement++)
        {
            ElementData* pkData = m_kElementData.GetAt(uiElement);
            
            if (pkData->m_spWorkflow)
            {
                NiSyncArgs kCompleteArgs;
                kCompleteArgs.m_uiSubmitPoint = m_kSubmitPoints.GetAt(0);
                kCompleteArgs.m_uiCompletePoint = m_kCompletePoints.GetAt(0);
                CompleteTasks(pkMesh, &kCompleteArgs);
                break;
            }
        }
    }

    for (NiUInt32 uiElement = 0; uiElement < uiElementCount; uiElement++)
    {
        ElementData* pkData = m_kElementData.GetAt(uiElement);

        if (!pkData->m_spTask)
            continue;

        // Set the task status appropriately
        NiUInt32 uiTaskGroup = NiSyncArgs::GetTaskGroupID(
            m_kSubmitPoints.GetAt(0), m_kCompletePoints.GetAt(0));
        pkData->m_spWorkflow =
            pkWFManager->AddRelatedTask(pkData->m_spTask, uiTaskGroup,
            m_kSubmitPoints.GetAt(0) == NiSyncArgs::SYNC_POST_UPDATE);

        SetNeedsUpdate(false);
        SetNeedsCompletion(true);
    }
        
    return true;
}
//---------------------------------------------------------------------------
bool NiMorphMeshModifier::CompleteTasks(NiMesh* pkMesh, NiSyncArgs*)
{
    if (GetNeedsCompletion())
    {
        const NiUInt32 uiElementCount = m_kElementData.GetSize();
        for (NiUInt32 uiElement = 0; uiElement < uiElementCount; uiElement++)
        {
            ElementData* pkData = m_kElementData.GetAt(uiElement);

            if (!pkData->m_spWorkflow)
                continue;

            NiStreamProcessor::Get()->Wait(pkData->m_spWorkflow);
            pkData->m_spWorkflow = 0;
        }
            
        if (GetUpdateNormals())
        {
            if (GetSkinned())
            {
                NiMeshUtilities::RecalculateNormals(pkMesh,
                    NiCommonSemantics::POSITION_BP(), 0,
                    NiCommonSemantics::NORMAL_BP(), 0);
            }
            else
            {
                NiMeshUtilities::RecalculateNormals(pkMesh,
                    NiCommonSemantics::POSITION(), 0,
                    NiCommonSemantics::NORMAL(), 0);
            }
        }
            
        SetNeedsCompletion(false);
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool NiMorphMeshModifier::CalculateMorphBound(NiMesh* pkMeshBase)
{
    // Calculate morph bound by combining bounds of all targets
    // Assumes no target will have a weight of greater than 1.0f or less
    // than 0.0f, and that the sum of weights will be 1.0f, or the bound 
    // may be invalid.

    NIASSERT(NiIsKindOf(NiMesh, pkMeshBase));
    NiMesh* pkMesh = (NiMesh*)pkMeshBase;

    // Initialize a "null" bound
    NiBound kBound;
    kBound.SetCenterAndRadius(NiPoint3::ZERO, 0.0f);

    // Find out if we are working with relative targets
    bool bRelativeTargets = GetRelativeTargets();

    // Find the element that contains the position info
    NiUInt32 uiNumElements = m_kElementData.GetSize();
    if (!uiNumElements)
        return false;

    ElementData* pkData = 0;
    NiUInt32 uiIndex = 0;
    while (uiIndex < uiNumElements)
    {
        pkData = m_kElementData.GetAt(uiIndex);
        if (pkData->m_kSemantic == NiCommonSemantics::POSITION())
        {
            break;
        }
        uiIndex++;
    }
    if (uiIndex == uiNumElements)
        return false;

    NiFixedString kTargetSemantic = MorphElementSemantic(
        NiCommonSemantics::POSITION());

    NIASSERT(m_uiNumTargets <=
        pkMesh->GetMaxIndexBySemantic(kTargetSemantic) + 1);

    // At this point we know that we have all the required info
    // (at least in theory). So start computing the bound.

    NiUInt32 uiNumPoints;
    
    NiDataStreamRef* pkBaseStreamRef = pkMesh->FindStreamRef(
        kTargetSemantic, 0, NiDataStreamElement::F_UNKNOWN);
    NIASSERT(pkBaseStreamRef);
    uiNumPoints = pkBaseStreamRef->GetTotalCount();
    NiDataStream* pkBaseStream = pkBaseStreamRef->GetDataStream();
    NiUInt32 uiBaseStreamStride = pkBaseStreamRef->GetStride();
    const void* pkBasePoints = pkBaseStream->Lock(NiDataStream::LOCK_READ);
    kBound.ComputeFromData(uiNumPoints, pkBasePoints, uiBaseStreamStride);
    
    void* pkSumPts = 0;
    if (bRelativeTargets)
    {
        if (uiBaseStreamStride == (sizeof(float) * 3))
        {
            pkSumPts = NiAlloc(float, uiNumPoints * 3);
        }
        else
        {
            pkSumPts = NiAlloc(NiFloat16, uiNumPoints * 4);
        }
    }

    NiDataStream* pkStream;
    const void* pkPoints;
    for (NiUInt32 ui = 1; ui < m_uiNumTargets; ui++)
    {
        pkStream = pkMesh->FindStreamRef(kTargetSemantic, ui, 
            NiDataStreamElement::F_UNKNOWN)->GetDataStream();
        NIASSERT(pkStream);
        NIASSERT(uiNumPoints == pkStream->GetTotalCount());
        pkPoints = pkStream->Lock(NiDataStream::LOCK_READ);

        NiBound kTargetBound;
        if (bRelativeTargets)
        {
            if (uiBaseStreamStride == (sizeof(float) * 3))
            {
                float* pkBasePoints32 = (float*)pkBasePoints;
                float* pkPoints32 = (float*)pkPoints;
                float* pkSumPts32 = (float*)pkSumPts;
                for (unsigned int i = 0; i < uiNumPoints * 3; i++)
                {
                    pkSumPts32[i] = pkBasePoints32[i] + pkPoints32[i];
                }
            }
            else
            {
                NiFloat16* pkBasePoints16 = (NiFloat16*)pkBasePoints;
                NiFloat16* pkPoints16 = (NiFloat16*)pkPoints;
                NiFloat16* pkSumPts16 = (NiFloat16*)pkSumPts;
                for (unsigned int i = 0; i < uiNumPoints * 4; i++)
                {
                    pkSumPts16[i] = pkBasePoints16[i] + pkPoints16[i];
                }
            }

            kTargetBound.ComputeFromData(uiNumPoints, pkSumPts, 
                uiBaseStreamStride);
        }
        else
        {
            kTargetBound.ComputeFromData(uiNumPoints, pkPoints, 
                uiBaseStreamStride);
        }

        pkStream->Unlock(NiDataStream::LOCK_READ);

        kBound.Merge(&kTargetBound);
    }

    pkBaseStream->Unlock(NiDataStream::LOCK_READ);

    if (bRelativeTargets)
        NiFree(pkSumPts);

    pkMeshBase->SetModelBound(kBound);

    return true;
}
//---------------------------------------------------------------------------
bool NiMorphMeshModifier::ResetSyncPoints(NiMesh*)
{
    // Remove all possible existing sync points
    RemoveSubmitSyncPoint(NiSyncArgs::SYNC_UPDATE);
    RemoveSubmitSyncPoint(NiSyncArgs::SYNC_POST_UPDATE);
    RemoveSubmitSyncPoint(NiSyncArgs::SYNC_VISIBLE);
    RemoveCompleteSyncPoint(NiSyncArgs::SYNC_VISIBLE);
    RemoveCompleteSyncPoint(NiSyncArgs::SYNC_RENDER);
    
    // Pre-size arrays
    m_kSubmitPoints.Realloc(1);
    m_kCompletePoints.Realloc(1);
    
    if (GetAlwaysUpdate())
    {
        // We start in update
        AddSubmitSyncPoint(NiSyncArgs::SYNC_UPDATE);
    }
    else if (GetSWSkinned())
    {
        // We start in post update
        AddSubmitSyncPoint(NiSyncArgs::SYNC_POST_UPDATE);
    }
    else
    {
        // Start when we know we're visible
        AddSubmitSyncPoint(NiSyncArgs::SYNC_VISIBLE);
    }
    
    if (GetSWSkinned())
    {
        // Get done in time for rendering
        AddCompleteSyncPoint(NiSyncArgs::SYNC_VISIBLE);
    }
    else
    {
        // Get done in time for rendering
        AddCompleteSyncPoint(NiSyncArgs::SYNC_RENDER);
    }
    
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiMorphMeshModifier);
//---------------------------------------------------------------------------
void NiMorphMeshModifier::CopyMembers(NiMorphMeshModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiMeshModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;
    
    pkDest->m_uiNumTargets = m_uiNumTargets;

    NiUInt32 uiNumElements = m_kElementData.GetSize();
    pkDest->m_kElementData.SetSize(uiNumElements);
    for (NiUInt32 uiElement = 0; uiElement < uiNumElements; uiElement++)
    {
        ElementData* pkData = m_kElementData.GetAt(uiElement);
        
        // Copy the morphed element references. We need to do a deep copy
        // because the objects are not reference counted and we do not have
        // a pointer to the mesh we are attached to in the ProcessClone
        // stage. That is, we can't get pointers to the element refs on the
        // mesh we are morphing.
        NiFixedString kSemantic;
        NiUInt32 uiSemanticIdx = uiElement;
        GetMorphedElement(uiElement, kSemantic, uiSemanticIdx);
        pkDest->AddMorphedElement(kSemantic, uiSemanticIdx);

        ElementData* pkDestData = pkDest->m_kElementData.GetAt(uiElement);
        pkDestData->m_uiNormalizeFlag = pkData->m_uiNormalizeFlag;
    }
}
//---------------------------------------------------------------------------
void NiMorphMeshModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiMeshModifier::ProcessClone(kCloning);
    
    // Most of the process clone work is done when this clone is added to
    // to its mesh clone, because Prepare is then called.
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMorphMeshModifier);
//---------------------------------------------------------------------------
void NiMorphMeshModifier::LoadBinary(NiStream& kStream)
{
    NiMeshModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);
    NiStreamLoadBinary(kStream, m_uiNumTargets);

    NiUInt32 uiNumElements = 0;
    NiStreamLoadBinary(kStream, uiNumElements);
    
    m_kElementData.SetSize(uiNumElements);
    for (NiUInt32 uiElement = 0; uiElement < uiNumElements; uiElement++)
    {
        NiFixedString kSemantic;
        kStream.LoadFixedString(kSemantic);
        NiUInt32 uiSemIndex = 0;
        NiStreamLoadBinary(kStream, uiSemIndex);
        AddMorphedElement(kSemantic, uiSemIndex);
        
        ElementData *pkData = m_kElementData.GetAt(uiElement);
        
        NiStreamLoadBinary(kStream, pkData->m_uiNormalizeFlag);
    }
}
//---------------------------------------------------------------------------
void NiMorphMeshModifier::LinkObject(NiStream& kStream)
{
    NiMeshModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiMorphMeshModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiMeshModifier::RegisterStreamables(kStream))
        return false;

    // We don't stream any component objects - just pointers that are
    // the responsibility of someone else.

    for (NiUInt32 uiElement = 0; uiElement < m_kElementData.GetSize();
        ++uiElement)
    {
        ElementData* pkData = m_kElementData.GetAt(uiElement);
        kStream.RegisterFixedString(pkData->m_kSemantic);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiMorphMeshModifier::SaveBinary(NiStream& kStream)
{
    NiMeshModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);
    NiStreamSaveBinary(kStream, m_uiNumTargets);

    NiUInt32 uiNumElements = m_kElementData.GetSize();
    NiStreamSaveBinary(kStream, uiNumElements);
    
    for (NiUInt32 uiElement = 0; uiElement < uiNumElements; uiElement++)
    {
        // Save the morphed element references.
        ElementData* pkData = m_kElementData.GetAt(uiElement);
        kStream.SaveFixedString(pkData->m_kSemantic);
        NiStreamSaveBinary(kStream, pkData->m_uiSemanticIdx);
        NiStreamSaveBinary(kStream, pkData->m_uiNormalizeFlag);
    }
}
//---------------------------------------------------------------------------
bool NiMorphMeshModifier::IsEqual(NiObject* pkObject)
{
    if (!NiMeshModifier::IsEqual(pkObject))
        return false;
        
    if (!NiIsKindOf(NiMorphMeshModifier, pkObject))
        return false;
        
    NiMorphMeshModifier* pkOther = (NiMorphMeshModifier*)pkObject;

    if (GetAlwaysUpdate() != pkOther->GetAlwaysUpdate() ||
        GetRelativeTargets() != pkOther->GetRelativeTargets() ||
        GetUpdateNormals() != pkOther->GetUpdateNormals())
    {
        return false;
    }
    
    if (m_uiNumTargets != pkOther->m_uiNumTargets)
        return false;

    NiUInt32 uiNumElements = m_kElementData.GetSize();
    if (pkOther->m_kElementData.GetSize() != uiNumElements)
        return false;
    for (NiUInt32 ui = 0; ui < uiNumElements; ui++)
    {
        ElementData* pkData = m_kElementData.GetAt(ui);
        ElementData* pkOtherData = pkOther->m_kElementData.GetAt(ui);
        if (!pkData || !pkOtherData)
            return false;

        if ((pkData->m_kSemantic !=
            pkOtherData->m_kSemantic) ||
            (pkData->m_uiSemanticIdx !=
            pkOtherData->m_uiSemanticIdx))
        {
            return false;
        }
        
        if (pkData->m_uiNormalizeFlag != pkOtherData->m_uiNormalizeFlag)
            return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiMorphMeshModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiMeshModifier::GetViewerStrings(pkStrings);
    pkStrings->Add(NiGetViewerString(NiMorphMeshModifier::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("RelativeTargets", GetRelativeTargets()));
    pkStrings->Add(NiGetViewerString("UpdateNormals", GetUpdateNormals()));
    pkStrings->Add(NiGetViewerString("AlwaysUpdate", GetAlwaysUpdate()));

    pkStrings->Add(NiGetViewerString("NumTargets", m_uiNumTargets));

    for (NiUInt32 ui = 0; ui < m_kElementData.GetSize(); ui++)
    {
        ElementData* pkData = m_kElementData.GetAt(ui);
        if (!pkData)
            continue;
            
        char pcPrefix[64];
        NiSprintf(pcPrefix, 64, "Morphed Semantic %d", ui);
        pkStrings->Add(NiGetViewerString(pcPrefix, pkData->m_kSemantic));
        
        NiSprintf(pcPrefix, 64, "Morphed Semantic Index %d", ui);
        pkStrings->Add(NiGetViewerString(pcPrefix, pkData->m_uiSemanticIdx));
    }
}
//---------------------------------------------------------------------------
