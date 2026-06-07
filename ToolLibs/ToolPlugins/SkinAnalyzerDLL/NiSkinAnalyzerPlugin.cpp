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

#include "stdafx.h"
#include "resource.h"
#include "NiSkinAnalyzerPlugin.h"
#include <NiMesh.h>
#include <NiSkinningMeshModifier.h>
#include <NiDataStreamElementLock.h>

NiImplementRTTI(NiSkinAnalyzerPlugin, NiPlugin);

//---------------------------------------------------------------------------
NiSkinAnalyzerPlugin::NiSkinAnalyzerPlugin() :
    NiPlugin("Skin Analyzer", "2.0", "Displays skin partitioning statistics",
    "Searches the scene graph for skinned objects, printing skin partition"
    " information.")
{
}
//---------------------------------------------------------------------------
bool NiSkinAnalyzerPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiSkinAnalyzerPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiSkinAnalyzerPlugin::FindSkinnedMesh(NiAVObject* pkRoot, 
    NiTList<NiMesh*>& kSkins)
{
    if (NiIsKindOf(NiNode, pkRoot))
    {
        NiNode* pkNode = (NiNode*)pkRoot;

        for (NiUInt32 ui = 0; ui < pkNode->GetArrayCount(); ++ui)
        {
            if (pkNode->GetAt(ui))
            {
                FindSkinnedMesh(pkNode->GetAt(ui), kSkins);
            }
        }
    }
    else if (NiIsKindOf(NiMesh, pkRoot))
    {
        NiMesh* pkMesh = (NiMesh*)pkRoot;

        if (NiGetModifier(NiSkinningMeshModifier, pkMesh))
            kSkins.AddTail(pkMesh);
    }
}
//---------------------------------------------------------------------------
NiUInt32 NiSkinAnalyzerPlugin::CountUsedBones(
    NiUInt32 uiSubmesh,
    NiUInt32 uiBonesInPartition,
    const NiDataStreamElementLock& kLockBlendweights,
    const NiDataStreamElementLock& kLockBlendindices,
    NiUInt32 uiVerts)
{
    typedef NiInt16 Index4[4];
    NIASSERT(sizeof(Index4) == 8);

    // Create iterators to access the stream data
    NiTStridedRandomAccessIterator<NiPoint3> kWeightIter =
        kLockBlendweights.begin<NiPoint3>(uiSubmesh);
    NiTStridedRandomAccessIterator<Index4> kIndexIter =
        kLockBlendindices.begin<Index4>(uiSubmesh);

    bool* pbBoneIsUsed = NiAlloc(bool, uiBonesInPartition);
    memset(pbBoneIsUsed, 0, sizeof(bool) * uiBonesInPartition);

    NiUInt32 uiBoneCount = 0;
    for (NiUInt32 j = 0; j < uiVerts; j++)
    {
        float fWeights[4];
        fWeights[0] = kWeightIter[j][0];
        fWeights[1] = kWeightIter[j][1];
        fWeights[2] = kWeightIter[j][2];
        fWeights[3] = 1.0f - fWeights[0] - fWeights[1] - fWeights[2];

        for (NiUInt32 k = 0; k < 4; k++)
        {
            // unused bones are set to exactly index 0 and
            // weight 0.0f
            float fWeight = fWeights[k];
            if (fWeight != 0.0f)
            {
                NiUInt32 uiBoneIndex = kIndexIter[j][k];
                NIASSERT(uiBoneIndex < uiBonesInPartition);

                if (!pbBoneIsUsed[uiBoneIndex])
                {
                    pbBoneIsUsed[uiBoneIndex] = true;
                    uiBoneCount++;
                }
            }
        }
    }

    NiFree(pbBoneIsUsed);

    return uiBoneCount;
}
//---------------------------------------------------------------------------
void NiSkinAnalyzerPlugin::AnalyzeMesh(NiMesh* pkMesh)
{
    const NiUInt32 uiTotalVerts = pkMesh->GetVertexCount();
    const NiUInt32 uiTotalTris = pkMesh->GetTotalPrimitiveCount();
    const NiUInt32 uiPartitionCount = pkMesh->GetSubmeshCount();

    // Log information about the skinned object (name, # bones, # parts)
    NiSkinningMeshModifier* pkSkin =
        NiGetModifier(NiSkinningMeshModifier, pkMesh);
    if (!pkSkin)
    {
        m_strMessage += "Error: No Skinning Mesh Modifier found on mesh!\n";
        return;
    }

    NiString strSubMsg;
    strSubMsg.Format(
        "Skin Object: %s (%u Bones,\t %u Partitions)\r\r\n", 
        pkMesh->GetName(),
        pkSkin->GetBoneCount(), 
        uiPartitionCount);
    m_strMessage += strSubMsg;

    // Verify that we have a blend weight stream before proceeding
    NiDataStreamRef* pkBlendWeightStreamRef = pkMesh->FindStreamRef(
        NiCommonSemantics::BLENDWEIGHT(), 0, NiDataStreamElement::F_FLOAT32_4);
    if (!pkBlendWeightStreamRef)
    {
        m_strMessage += "Error: No blend weight stream found on mesh!\n";
        return;
    }

    // prepare to sort the partitions by vertex count
    NiUInt32* puiIndexArray = NiAlloc(NiUInt32, uiPartitionCount);
    for (NiUInt32 ui = 0; ui < uiPartitionCount; ui++)
        puiIndexArray[ui] = ui;

    // sort the partitions by vertex count
    for (NiUInt32 i = (uiPartitionCount - 1); i > 0; i--)
    {
        for (NiUInt32 j = 0; j < i; j++)
        {
            if (pkMesh->GetVertexCount(j+1) > pkMesh->GetVertexCount(j))
            {
                NiUInt32 uiTemp = puiIndexArray[j+1];
                puiIndexArray[j+1] = puiIndexArray[j];
                puiIndexArray[j] = uiTemp;
            }

        }
    }

    // Is the object partitioned? (if so, it will have a BONES stream)
    NiDataStreamRef* pkBonePaletteStreamRef = pkMesh->FindStreamRef(
        NiCommonSemantics::BONE_PALETTE(), 0, NiDataStreamElement::F_UINT16_1);
    NiDataStream* pkBonePaletteStream = pkBonePaletteStreamRef ?
        pkBonePaletteStreamRef->GetDataStream() : NULL;

    // Lock the blend weight stream
    NiDataStreamElementLock kLockBlendweights(pkMesh,
        NiCommonSemantics::BLENDWEIGHT(), 0,
        NiDataStreamElement::F_FLOAT32_4,
        NiDataStream::LOCK_TOOL_READ);

    // Lock the blend index stream
    NiDataStreamElementLock kLockBlendindices(pkMesh,
        NiCommonSemantics::BLENDINDICES(), 0,
        NiDataStreamElement::F_INT16_4,
        NiDataStream::LOCK_TOOL_READ);

    // The bone palette stream count gives us the maximum bones that the
    // stream will hold, not the actual number of used bones.  This array
    // will store the actual number for display purposes.
    NiUInt32* puiActualBonesUsed = NiAlloc(NiUInt32, uiPartitionCount);
    memset(puiActualBonesUsed, 0, sizeof(NiUInt32) * uiPartitionCount);

    // Print partition summary
    for (NiUInt32 ui = 0; ui < uiPartitionCount; ui++)
    {
        const NiUInt32 uiSubmesh = puiIndexArray[ui];

        const NiUInt32 uiVerts = pkMesh->GetVertexCount(uiSubmesh);
        const NiUInt32 uiTris = pkMesh->GetPrimitiveCount(uiSubmesh);

        const NiUInt32 uiBonesInPartition = pkBonePaletteStreamRef ? 
            pkBonePaletteStream->GetCount(uiSubmesh) : pkSkin->GetBoneCount();

        puiActualBonesUsed[ui] = CountUsedBones(uiSubmesh, uiBonesInPartition,
            kLockBlendweights, kLockBlendindices, uiVerts);

        NiString strSubMsg;
        strSubMsg.Format(
            "\tPartition[%u]:\t  %u Bones,\t %u Verts (%f%%),"
            "\t%u Tris (%f%%)\r\r\n", 
            uiSubmesh,
            puiActualBonesUsed[ui],
            uiVerts,
            100.0f * ((float)uiVerts) / uiTotalVerts,
            uiTris,
            100.0f * ((float)uiTris) / uiTotalTris);
        m_strMessage += strSubMsg;
    }
    m_strMessage += "\r\r\n\r\r\n";

    strSubMsg.Format(
        "Skin Object: %s (%u Bones,\t %u Partitions)\r\r\n", 
        pkMesh->GetName(),
        pkSkin->GetBoneCount(), 
        uiPartitionCount);
    m_strMessage += strSubMsg;

    // Print out information for each partition, ordered by vertex count
    for (NiUInt32 ui = 0; ui < uiPartitionCount; ui++)
    {
        const NiUInt32 uiSubmesh = puiIndexArray[ui];

        const NiUInt32 uiVerts = pkMesh->GetVertexCount(uiSubmesh);
        const NiUInt32 uiTris = pkMesh->GetPrimitiveCount(uiSubmesh);
        NI_UNUSED_ARG(uiTris);

        const NiUInt32 uiBonesInPartition = pkBonePaletteStreamRef ? 
            pkBonePaletteStream->GetCount(uiSubmesh) : pkSkin->GetBoneCount();
        NIASSERT(uiBonesInPartition > 0);

        strSubMsg.Format(
            "\tPartition[%u]:\t  %u Bones (palette) \r\r\n",
            uiSubmesh,
            puiActualBonesUsed[ui]);
        m_strMessage += strSubMsg;

        // Get the bone palette (or a dummy one that counts up)
        NiUInt16* puiBonePalette;
        if (pkBonePaletteStream)
        {
            puiBonePalette =
                (NiUInt16*)pkBonePaletteStream->LockRegion(uiSubmesh,
                NiDataStream::LOCK_TOOL_READ);
        }
        else
        {
            puiBonePalette = NiAlloc(NiUInt16, uiBonesInPartition);
            for (NiUInt32 ui = 0; ui < uiBonesInPartition; ++ui)
                puiBonePalette[ui] = (NiUInt16)ui;
        }
        NIASSERT(puiBonePalette);

        if (puiBonePalette && kLockBlendweights.IsLocked() &&
            kLockBlendindices.IsLocked())
        {
            typedef NiInt16 Index4[4];
            NIASSERT(sizeof(Index4) == 8);

            // Create iterators to access the stream data
            NiTStridedRandomAccessIterator<NiPoint3> kWeightIter =
                kLockBlendweights.begin<NiPoint3>(uiSubmesh);
            NiTStridedRandomAccessIterator<Index4> kIndexIter =
                kLockBlendindices.begin<Index4>(uiSubmesh);

            // processing
            NiUInt32* puiBoneUses = NiAlloc(NiUInt32, uiBonesInPartition);
            memset(puiBoneUses, 0, sizeof(NiUInt32) * uiBonesInPartition);

            float* pfBoneWeightSum = NiAlloc(float, uiBonesInPartition);
            memset(pfBoneWeightSum, 0, sizeof(float) * uiBonesInPartition);

            float* pfBoneWeightMax = NiAlloc(float, uiBonesInPartition);
            memset(pfBoneWeightMax, 0, sizeof(float) * uiBonesInPartition);

            // Collect statistics on bone usage
            for (NiUInt32 j = 0; j < uiVerts; j++)
            {
                float fWeights[4];
                fWeights[0] = kWeightIter[j][0];
                fWeights[1] = kWeightIter[j][1];
                fWeights[2] = kWeightIter[j][2];
                fWeights[3] = 1.0f - fWeights[0] - fWeights[1] - fWeights[2];

                for (NiUInt32 k = 0; k < 4; k++)
                {
                    // unused bones are set to exactly index 0 and
                    // weight 0.0f
                    float fWeight = fWeights[k];
                    if (fWeight != 0.0f)
                    {
                        NiUInt32 uiBoneIndex = kIndexIter[j][k];
                        NIASSERT(uiBoneIndex < uiBonesInPartition);

                        puiBoneUses[uiBoneIndex]++;
                        if (pfBoneWeightMax[uiBoneIndex] < fWeight)
                            pfBoneWeightMax[uiBoneIndex] = fWeight;
                        pfBoneWeightSum[uiBoneIndex] += fWeight;
                    }
                }
            }

            // Convert the per-bone blendweight sums into average blendweights
            for (NiUInt32 j = 0; j < uiBonesInPartition; j++)
            {
                if (puiBoneUses[j])
                    pfBoneWeightSum[j] /= (float)(puiBoneUses[j]);
            }

            // Print each bone in the partition
            NiUInt32 uiBoneCounter = 0;
            for (NiUInt32 k = 0; k < uiBonesInPartition; k++)
            {
                if (puiBoneUses[k])
                {
                    const char* pcName =
                        pkSkin->GetBones()[puiBonePalette[k]]->GetName();

                    NiString strSubMsg;
                    strSubMsg.Format(
                        "\t\t%u is Bone[%u]: %s,\t "
                        "Mean Weight %f,\t Max Weight %f\r\r\n", 
                        uiBoneCounter,
                        puiBonePalette[k],
                        pcName ? pcName : "<unnamed>",
                        pfBoneWeightSum[k],
                        pfBoneWeightMax[k]);

                    m_strMessage += strSubMsg;

                    uiBoneCounter++;
                }
            }

            NiFree(puiBoneUses);
            NiFree(pfBoneWeightSum);
            NiFree(pfBoneWeightMax);
        }
        else
        {
            m_strMessage += "\tWarning: Blend indices and/or blend weights "
                "are in an unsupported format, cannot be locked for reading, "
                "or are not present. Perhaps the asset has been finalized. "
                "Some analysis steps will be skipped.\n";
        }

        if (pkBonePaletteStream)
            pkBonePaletteStream->Unlock(NiDataStream::LOCK_TOOL_READ);
        else
            NiFree(puiBonePalette);
    }

    NiFree(puiActualBonesUsed);
    NiFree(puiIndexArray);
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiSkinAnalyzerPlugin::Execute(
    const NiPluginInfo*)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Find all skinned meshes
    NiTList<NiMesh*> kSkins;
    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);
        FindSkinnedMesh(spScene, kSkins);
    }

    // Record information about each skinned mesh
    m_strMessage.Empty();

    NiTListIterator kIter = kSkins.GetHeadPos();
    while (kIter)
    {
        NiMesh* pkMesh = kSkins.GetNext(kIter);
        AnalyzeMesh(pkMesh);
    }

    NiString strSubMsg;
    strSubMsg.Format("\r\r\n\r\r\n");
    m_strMessage += strSubMsg;

    // Display or log the results of the analysis
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    bool bWriteResultsToLog = false;
    NiXMLLogger* pkLogger = NULL;

    if (pkExporterSharedData)
    {
        bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
        pkLogger = pkExporterSharedData->GetXMLLogger();
    }

    if (bWriteResultsToLog && pkLogger)
    {
        pkLogger->LogElement("SkinAnalyzer", m_strMessage);
    }
    else
    {
        NiInfoDialog kDlg("Skin Analyzer Results:");
        kDlg.SetText(m_strMessage);
        kDlg.DoModal();
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiSkinAnalyzerPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiSkinAnalyzerPlugin");
    pkPluginInfo->SetType("PROCESS");
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiSkinAnalyzerPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool NiSkinAnalyzerPlugin::DoManagementDialog(NiPluginInfo*,
    NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
