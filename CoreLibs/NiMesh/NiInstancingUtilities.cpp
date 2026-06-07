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

#include <NiRenderer.h>
#include <NiPoint4.h>
#include "NiMesh.h"
#include "NiRenderObjectMaterialOption.h"
#include "NiInstancingUtilities.h"
#include "NiInstancingMeshModifier.h"

//---------------------------------------------------------------------------
bool NiInstancingUtilities::InstancingIndexDuplicationRequired()
{
    if (NiSystemDesc::GetSystemDesc().GetToolMode())
    {
        NiSystemDesc::RendererID eRendererID = 
            NiSystemDesc::GetSystemDesc().GetToolModeRendererID();
        if (eRendererID == NiSystemDesc::RENDERER_GENERIC ||
            eRendererID == NiSystemDesc::RENDERER_PS3 ||
            eRendererID == NiSystemDesc::RENDERER_XENON)
        {
            return true;
        }
    }

#ifdef _XENON
    return true;
#elif _PS3
    return true;
#else
    return false;
#endif
}
//---------------------------------------------------------------------------
void NiInstancingUtilities::ValidateInstancingData(NiMesh* pkMesh)
{
    if (!NiRenderer::GetRenderer() ||
        !(NiRenderer::GetRenderer()->GetFlags() 
        & NiRenderer::CAPS_HARDWAREINSTANCING))
    {
        NiOutputDebugString("Warning: Current hardware does not support"
            " instancing. Instancing will be disabled for the provided "
            "mesh.\n");
        DisableMeshInstancing(pkMesh);
        return;
    }
    
    // Validate Instancing Data
    NiDataStreamRef* pkIndexStreamRef = 
        pkMesh->FindStreamRef(NiCommonSemantics::INDEX());
    NIASSERT(pkIndexStreamRef);

    bool bIndexDuplicationRequired = InstancingIndexDuplicationRequired();

    NIASSERT(pkIndexStreamRef->GetDataStream());
    bool bIsIndexDataDuplicated = 
        pkIndexStreamRef->GetDataStream()->GetRegionCount() == 3;

    if (bIsIndexDataDuplicated != bIndexDuplicationRequired)
    {
        // The index data duplication state does not match the required
        // index data duplication state. Attempt to patch this issue now.
        NiOutputDebugString("Warning: Instance data provided is not valid"
            " for the current platform. The required data will now be"
            " reconstructed. Re-export this asset for this platform to"
            " avoid instance data reconstruction at load time.\n");

        if (bIndexDuplicationRequired)
        {
            // Duplicated index data is required but not present. So we 
            // will create the required duplicated index data now.
            NiUInt32 uiTotalIndexCount = 
                GetTotalIndexCount(pkIndexStreamRef->GetDataStream());

            // For strip primitive types add in space for degenerate 
            // tris/lines.
            if (pkMesh->GetPrimitiveType() == 
                NiPrimitiveType::PRIMITIVE_TRISTRIPS)
            {
                uiTotalIndexCount += 2;
            }
            else if (pkMesh->GetPrimitiveType() == 
                NiPrimitiveType::PRIMITIVE_LINESTRIPS)
            {
                uiTotalIndexCount += 1;
            }

            NiUInt32 uiMaxTransforms = GetMaxInstanceCount(pkMesh);
            
            // Default to a max of 65536 indicies per submesh. This allows us
            // to use 16bit indicies.
            NiUInt32 uiTransformsPerSubmesh = 
                NiMax(uiTotalIndexCount, 65536) / uiTotalIndexCount;
            uiTransformsPerSubmesh = 
                NiMin((int)uiTransformsPerSubmesh, (int)uiMaxTransforms);

            CreateDuplicateIndexDataStream(pkMesh, pkIndexStreamRef,
                uiTotalIndexCount, uiTransformsPerSubmesh, uiMaxTransforms);

            NiUInt32 uiRequiredSubmeshes = 
                uiMaxTransforms / uiTransformsPerSubmesh;
            if (uiMaxTransforms % uiTransformsPerSubmesh)
                uiRequiredSubmeshes++;

            // Remove data stream regions from the instances streams then 
            // compute new regions.
            NiDataStreamRef* pkBaseStreamRef = pkMesh->GetBaseInstanceStream();
            NIASSERT(pkBaseStreamRef && pkBaseStreamRef->GetDataStream());
            pkBaseStreamRef->GetDataStream()->RemoveAllRegions();

            NiDataStreamRef* pkVisStreamRef = 
                pkMesh->GetVisibleInstanceStream();
            NIASSERT(pkVisStreamRef && pkVisStreamRef->GetDataStream());
            pkVisStreamRef->GetDataStream()->RemoveAllRegions();

            SetupTransformStreamRegions(pkMesh, 
                NiCommonSemantics::TRANSFORMS(),
                uiRequiredSubmeshes, uiTransformsPerSubmesh, 
                uiMaxTransforms);

            SetupTransformStreamRegions(pkMesh, 
                NiCommonSemantics::INSTANCETRANSFORMS(), 
                uiRequiredSubmeshes, uiTransformsPerSubmesh, 
                uiMaxTransforms);

            pkMesh->SetSubmeshCount(uiRequiredSubmeshes);  
        }
        else
        {
            NiUInt32 uiMaxTransforms = GetMaxInstanceCount(pkMesh);

            // Duplicated index data is _not_ required for the current platform
            // but exists in the mesh. So we destroy the duplicate index data
            // as it is unneeded.
            DestroyDuplicatedIndexData(pkMesh, pkIndexStreamRef);


            // Remove data stream regionss from the instances streams then 
            // compute new regions.
            NiDataStreamRef* pkBaseStreamRef = pkMesh->GetBaseInstanceStream();
            NIASSERT(pkBaseStreamRef && pkBaseStreamRef->GetDataStream());
            pkBaseStreamRef->GetDataStream()->RemoveAllRegions();

            NiDataStreamRef* pkVisStreamRef = 
                pkMesh->GetVisibleInstanceStream();
            NIASSERT(pkVisStreamRef && pkVisStreamRef->GetDataStream());
            pkVisStreamRef->GetDataStream()->RemoveAllRegions();

            SetupTransformStreamRegions(
                pkMesh, NiCommonSemantics::TRANSFORMS(), 1, 
                uiMaxTransforms, uiMaxTransforms);

            SetupTransformStreamRegions(
                pkMesh, NiCommonSemantics::INSTANCETRANSFORMS(), 
                1, uiMaxTransforms, uiMaxTransforms);
        }

    }

}
//---------------------------------------------------------------------------
bool NiInstancingUtilities::AddMeshInstance(NiMesh* pkMesh,
    NiMeshHWInstance* pkHWInstance)
{
    if (!pkMesh || !pkMesh->GetInstanced())
        return false;
    
    NiMeshModifier* pkModifier = 
        NiGetModifier(NiInstancingMeshModifier, pkMesh);

    if (pkModifier)
    {
        NiInstancingMeshModifier* pkInstModifier = 
            NiVerifyStaticCast(NiInstancingMeshModifier, pkModifier);

        pkInstModifier->AddMeshInstance(pkHWInstance);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiInstancingUtilities::RemoveMeshInstance(NiMesh* pkMesh,
    NiMeshHWInstance* pkHWInstance)
{
    if (!pkMesh || !pkMesh->GetInstanced())
        return false;

    NiMeshModifier* pkModifier = 
        NiGetModifier(NiInstancingMeshModifier, pkMesh);

    if (pkModifier)
    {
        NiInstancingMeshModifier* pkInstModifier = 
            NiVerifyStaticCast(NiInstancingMeshModifier, pkModifier);

        pkInstModifier->RemoveMeshInstance(pkHWInstance);
    }

    return true;
}
//---------------------------------------------------------------------------
NiUInt32 NiInstancingUtilities::GetTotalIndexCount(
    NiDataStream* pkIndexDataStream)
{
    if (!pkIndexDataStream)
        return 0;

    // Calculate the total number of indices used by the entire mesh.
    NiUInt32 uiTotalIndexCount = 0;

    for (NiUInt32 uiRegionIndex = 0; 
        uiRegionIndex < pkIndexDataStream->GetRegionCount(); uiRegionIndex++)
    {
        NiDataStream::Region kRegion =
            pkIndexDataStream->GetRegion(uiRegionIndex);
        uiTotalIndexCount += kRegion.GetRange();
    }

    return uiTotalIndexCount;
}
//---------------------------------------------------------------------------
NiUInt32 NiInstancingUtilities::GetTotalIndexCount(
    const NiDataStream* pkIndexDataStream)
{
    if (!pkIndexDataStream)
        return 0;

    // Calculate the total number of indices used by the entire mesh.
    NiUInt32 uiTotalIndexCount = 0;

    for (NiUInt32 uiRegionIndex = 0; 
        uiRegionIndex < pkIndexDataStream->GetRegionCount(); uiRegionIndex++)
    {
        NiDataStream::Region kRegion =
            pkIndexDataStream->GetRegion(uiRegionIndex);
        uiTotalIndexCount += kRegion.GetRange();
    }

    return uiTotalIndexCount;
}
//---------------------------------------------------------------------------
bool NiInstancingUtilities::DisableMeshInstancing(NiMesh* pkMesh)
{
    if (!pkMesh || !pkMesh->GetInstanced())
        return false;

    NiDataStreamRef* pkIndexStreamRef = pkMesh->FindStreamRef(
        NiCommonSemantics::INDEX(), 0);

    if (!pkIndexStreamRef)
        return false;
    
    if (pkIndexStreamRef->GetDataStream()->GetRegionCount() == 3)
    {
        // Duplicated index data is present. Destroy the duplicated index data.
        DestroyDuplicatedIndexData(pkMesh, pkIndexStreamRef);
    }
    
    // Destroy the transform streams.
    NiDataStreamRef* pkBaseInstanceStream = pkMesh->GetBaseInstanceStream();
    NiDataStreamRef* pkVisibleInstanceStream = 
        pkMesh->GetVisibleInstanceStream();

    if (pkBaseInstanceStream)
    {
        pkMesh->RemoveStreamRef(pkBaseInstanceStream);
    }

    if (pkVisibleInstanceStream && 
        pkBaseInstanceStream != pkVisibleInstanceStream)
    {
        pkMesh->RemoveStreamRef(pkBaseInstanceStream);
    }

    // Detach and remove the NiInstancingMeshModifier
    NiInstancingMeshModifier* pkModifier = 
        NiGetModifier(NiInstancingMeshModifier, pkMesh);

    if (pkModifier)
    {
        pkMesh->DetachModifier(pkModifier);
        pkMesh->RemoveModifier(pkModifier);
    }

    // Set the instanced flag on the mesh.
    pkMesh->SetInstanced(false);

    return true;
}
//---------------------------------------------------------------------------
bool NiInstancingUtilities::EnableMeshInstancing(
    NiMesh* pkMesh, NiUInt32 uiMaxTransforms, NiTransform* pkTransforms, 
    NiUInt32 uiActiveTransforms, NiUInt32 uiMaxIndicesPerSubmesh,
    bool bCullPerInstance, bool bStaticBounds, 
    bool bCPURead, void* pvIndexData, NiUInt32 uiIndexCount,
    NiDataStreamElement::Format eInIndexFormat)
{
    if (!pkMesh)
        return false;

    if (uiMaxTransforms == 0)
        return false;

    if (!NiSystemDesc::GetSystemDesc().GetToolMode())
    {
        // Only concern ourselves with the current hardware if we are not in 
        // tool mode.
        if (!NiRenderer::GetRenderer() ||
            !(NiRenderer::GetRenderer()->GetFlags() 
            & NiRenderer::CAPS_HARDWAREINSTANCING))
        {
            // Current hardware does not have hardware instancing support.
            return false;
        }
    }

    if (pkMesh->GetSubmeshCount() != 1)
    {
        NiOutputDebugString("Error: Instancing is not supported for meshes "
            "with more than one submesh. This is normally skinned meshes.\n");
            return false;
    }

    if (pkMesh->GetInstanced())
    {
        NiOutputDebugString("Error: NiInstancingUtilities could not enable "
            "mesh instancing on mesh. The provided mesh is already instanced."
            "\n");
        return false;
    }

    NiDataStreamRef* pkIndexStreamRef =
        pkMesh->FindStreamRef(NiCommonSemantics::INDEX(), 0);

    if (!pkIndexStreamRef)
    {
        NiOutputDebugString("Error: An index data stream is required to enable"
            " instancing. The provided mesh will not be instanced.\n");
        return false;
    }

    if (!bCPURead && bCullPerInstance)
    {
        NiOutputDebugString("Warning: Culling per instance requires CPU read "
            "to be enabled. CPU read will be forcefully enabled.\n");
        bCPURead = true;
    }

    NiUInt32 uiTransformsPerSubmesh = 0;
    NiUInt32 uiTotalIndexCount = 0;
    if (pvIndexData)
    {
        uiTotalIndexCount = uiIndexCount;
    }
    else 
    {
        uiTotalIndexCount = 
            GetTotalIndexCount(pkIndexStreamRef->GetDataStream());
    }

    // For strip primitive types add in space for degenerate 
    // tris/lines.
    if (pkMesh->GetPrimitiveType() == NiPrimitiveType::PRIMITIVE_TRISTRIPS)
    {
        uiTotalIndexCount += 2;
    }
    else if (pkMesh->GetPrimitiveType() == 
        NiPrimitiveType::PRIMITIVE_LINESTRIPS)
    {
        uiTotalIndexCount += 1;
    }

    uiMaxIndicesPerSubmesh = 
        NiMax((int)uiTotalIndexCount, (int)uiMaxIndicesPerSubmesh);

    if (uiMaxIndicesPerSubmesh > MAX_INDICIES_PER_SUBMESH)
    {
        NiOutputDebugString("Warning: More than 524288 instances per submesh "
            "were requested. A maximum of 524288 is allowed, this request "
            "will be clamped to 524288 indicies per submesh.\n");
        uiMaxIndicesPerSubmesh = MAX_INDICIES_PER_SUBMESH;
    }

    if (InstancingIndexDuplicationRequired())
    {
        uiTransformsPerSubmesh = uiMaxIndicesPerSubmesh / uiTotalIndexCount;
        uiTransformsPerSubmesh = 
            NiMin((int)uiTransformsPerSubmesh, (int)uiMaxTransforms);

        if (!CreateDuplicateIndexDataStream(pkMesh, pkIndexStreamRef, 
            uiTotalIndexCount, uiTransformsPerSubmesh, uiMaxTransforms,
            pvIndexData, uiIndexCount, eInIndexFormat) )
        {
            NiOutputDebugString("Error: Failed to duplicate index data for"
                " instancing. Instancing will not be enabled for the mesh.\n");
            return false;
        }
    }
    else
    {
        uiTransformsPerSubmesh = uiMaxTransforms;
    }

    // Ensure that we don't have zero transforms per submesh since that would
    // preclude rendering and will cause a divide by zero exception. A zero 
    // result will only occur when uiMaxIndicesPerSubmesh is too low.
    NIASSERT(uiTransformsPerSubmesh);

    if (!CreateInstanceTransformStream(pkMesh, uiTotalIndexCount, 
        uiMaxTransforms, uiActiveTransforms, pkTransforms, bCPURead, 
        bCullPerInstance))
    {
        return false;
    }

    NiUInt32 uiRequiredSubmeshes = 
        uiMaxTransforms / uiTransformsPerSubmesh;
    if (uiMaxTransforms % uiTransformsPerSubmesh)
        uiRequiredSubmeshes++;

    SetupTransformStreamRegions(
        pkMesh, NiCommonSemantics::TRANSFORMS(), uiRequiredSubmeshes, 
        uiTransformsPerSubmesh, uiMaxTransforms);

    SetupTransformStreamRegions(
        pkMesh, NiCommonSemantics::INSTANCETRANSFORMS(), uiRequiredSubmeshes, 
        uiTransformsPerSubmesh, uiMaxTransforms);
    
    pkMesh->SetSubmeshCount(uiRequiredSubmeshes);    
    pkMesh->SetInstanced(true);
    SetActiveInstanceCount(pkMesh, uiActiveTransforms);
    
    NiInstancingMeshModifier* pkInstCullModifier = 
        NiNew NiInstancingMeshModifier();
    pkInstCullModifier->SetPerInstanceCulling(bCullPerInstance);
    pkInstCullModifier->SetStaticBounds(bStaticBounds);
    NIVERIFY(pkMesh->AddModifier(pkInstCullModifier));

    if (bStaticBounds)
        ComputeWorldBound(pkMesh);

    pkMesh->UpdateCachedPrimitiveCount();
    return true;
}
//---------------------------------------------------------------------------
bool NiInstancingUtilities::CreateDuplicateIndexDataStream(
    NiMesh* pkMesh, 
    NiDataStreamRef* pkIndexStreamRef, 
    NiUInt32 uiTotalIndexCount,
    NiUInt32 uiTransformsPerSubmesh,
    NiUInt32 uiMaxTransforms,
    void* pvIndexData, 
    NiUInt32,
    NiDataStreamElement::Format eInIndexFormat)
{
    NIASSERT(pkIndexStreamRef->GetElementDescCount() == 1);

    const NiDataStreamPtr spDataStream = pkIndexStreamRef->GetDataStream();

    NiDataStreamElement::Format eOutIndexFormat = eInIndexFormat;
    void* pvTempOldIndexData = NULL;

    if (pvIndexData)
    {
        pvTempOldIndexData = pvIndexData;
    }
    else
    {
        pvTempOldIndexData = GetAllIndexData(pkMesh, uiTotalIndexCount);
        eInIndexFormat = pkIndexStreamRef->GetElementDescAt(0).GetFormat();
    }

    if (!pvTempOldIndexData)
        return false;

    if (eInIndexFormat == NiDataStreamElement::F_UINT16_1 &&
        uiTotalIndexCount * uiTransformsPerSubmesh > USHRT_MAX)
    {
        char acBuff[NI_MAX_PATH];
        NiSprintf(acBuff, NI_MAX_PATH, "Warning: NiInstancingUtilities is "
            "unable to use a 16 bit index buffer to store the duplicate "
            "index data. A 16 bit index is not large enough to index into "
            "%d vertices.", uiTotalIndexCount * uiTransformsPerSubmesh);
        NiOutputDebugString(acBuff);
        NiOutputDebugString(" Switching to a 32 bit index buffer. \n");
        eOutIndexFormat = NiDataStreamElement::F_UINT32_1;
    }


    void* pvTempNewIndexData = NULL;
    if (eOutIndexFormat == NiDataStreamElement::F_UINT16_1)
    {
        pvTempNewIndexData = NiMalloc(uiTotalIndexCount * 
            sizeof(NiUInt16) * uiTransformsPerSubmesh);
    }
    else if (eOutIndexFormat == NiDataStreamElement::F_UINT32_1)
    {
        pvTempNewIndexData = NiMalloc(uiTotalIndexCount * 
            sizeof(NiUInt32) * uiTransformsPerSubmesh);
    }
    NIASSERT(pvTempNewIndexData);

    DuplicateIndexData(pvTempOldIndexData, pvTempNewIndexData, 
        spDataStream, eInIndexFormat, eOutIndexFormat, uiTotalIndexCount, 
        uiTransformsPerSubmesh, pkMesh->GetPrimitiveType());

    // Write access to the index stream is restricted. So we will 
    // create a new index data stream and reference that one instead.
    NiDataStream* pkNewDataStream = 
        NiDataStream::CreateSingleElementDataStream(
        eOutIndexFormat,
        (uiTotalIndexCount * uiTransformsPerSubmesh),
        (NiUInt8)(~NiDataStream::ACCESS_CPU_WRITE_STATIC_INITIALIZED & 
        spDataStream->GetAccessMask()),
        NiDataStream::USAGE_VERTEX_INDEX,
        pvTempNewIndexData,
        false);
    NIASSERT(pkNewDataStream);

    // Replace reference to old datastream
    pkIndexStreamRef->SetDataStream(pkNewDataStream);

    NIASSERT(spDataStream->GetRegionCount() == 1);

    NiUInt32 uiMaxIndicesPerSubmesh = 
        uiTransformsPerSubmesh * uiTotalIndexCount;
    NiUInt32 uiAvaliableIndices = uiMaxTransforms * uiTotalIndexCount;

    NiUInt32 uiRequiredSubmeshes = 
        uiMaxTransforms / uiTransformsPerSubmesh;
    if (uiMaxTransforms % uiTransformsPerSubmesh)
        uiRequiredSubmeshes++;

    for (NiUInt32 uiSubMesh = 0; uiSubMesh < uiRequiredSubmeshes; 
        uiSubMesh++)
    {
        NiUInt32 uiIndicesCount = 0;
        if (uiAvaliableIndices > uiMaxIndicesPerSubmesh)
        {
            uiIndicesCount = uiMaxIndicesPerSubmesh;
            uiAvaliableIndices -= uiMaxIndicesPerSubmesh;
        }
        else
        {
            uiIndicesCount = uiAvaliableIndices;
            uiAvaliableIndices = 0;
        }

        NiDataStream::Region kNewRegion(0, uiIndicesCount);
        pkIndexStreamRef->BindRegionToSubmesh(uiSubMesh, kNewRegion);
    }

    while(pkNewDataStream->GetRegionCount() != 3)
    {
        NiDataStream::Region kPlaceHolderRegion(0, 0);
        pkNewDataStream->AddRegion(kPlaceHolderRegion);
    }

    NiFree(pvTempNewIndexData);
    NiFree(pvTempOldIndexData);

    return true;
}
//---------------------------------------------------------------------------
void NiInstancingUtilities::SetupTransformStreamRegions(NiMesh* pkMesh,
    NiFixedString kTransformSemantic, NiUInt32 uiSubmeshCount, 
    NiUInt32 uiTransformsPerSubmesh, NiUInt32 uiMaxTransforms)
{

    for (NiUInt32 uiStreamIdx = 0; uiStreamIdx < 3; uiStreamIdx++)
    {
        NiDataStreamRef* pkStreamRef = 
            pkMesh->FindStreamRef(kTransformSemantic, uiStreamIdx);

        if (pkStreamRef)
        {
            for (NiUInt32 uiSubMesh = 0; uiSubMesh < uiSubmeshCount;
                uiSubMesh++)
            {
                NiDataStream::Region kNewRegion(
                    uiSubMesh * uiTransformsPerSubmesh,
                    (uiSubMesh + 1) * uiTransformsPerSubmesh);

                if ((uiSubMesh + 1) * uiTransformsPerSubmesh >
                    uiMaxTransforms)
                {
                    NiUInt32 uiDiff = 
                        uiMaxTransforms - kNewRegion.GetStartIndex();
                    kNewRegion.SetRange(uiDiff);
                }

                pkStreamRef->BindRegionToSubmesh(uiSubMesh, kNewRegion);
            }
        }

    }
}
//---------------------------------------------------------------------------
void* NiInstancingUtilities::GetAllIndexData(NiMesh* pkMesh, 
    NiUInt32 uiTotalIndexCount)
{
    if (!pkMesh)
        return NULL;

    NiDataStreamRef* pkIndexStreamRef =
        pkMesh->FindStreamRef(NiCommonSemantics::INDEX(), 0);

    NIASSERT(pkIndexStreamRef->GetElementDescCount() == 1);

    NiDataStream* pkDataStream = pkIndexStreamRef->GetDataStream();

    NiDataStreamElement kElement = pkIndexStreamRef->GetElementDescAt(0);

    // Check to see if CPU_READ is allowed on the index buffer. This is 
    // required since we will need to duplicate the index buffer.
    if (!NiSystemDesc::GetSystemDesc().GetToolMode())
    {
        if (!(pkDataStream->GetAccessMask() & NiDataStream::ACCESS_CPU_READ)) 
        {
            NiOutputDebugString("NiInstancingUtilities Error: Could not " \
                "enable mesh instancing on mesh.\n The provided mesh is " \
                "indexed but does not allow CPU_READ of the index data " \
                "stream.\n");
            return NULL;                
        }
    }

    NiUInt32 uiTotalIndexSize = uiTotalIndexCount * pkDataStream->GetStride();

    void* pvTempOldIndexData = NiMalloc(uiTotalIndexSize);
    NIASSERT(pvTempOldIndexData);

    // Read the index data stream into the temporary buffer.
    for (NiUInt32 uiRegionIndex = 0; 
        uiRegionIndex < pkDataStream->GetRegionCount(); uiRegionIndex++)
    {
        NiDataStream::Region kRegion =
            pkDataStream->GetRegion(uiRegionIndex);

        NiUInt32 uiSize = kRegion.ComputeSizeInBytes(
            pkDataStream->GetStride());

        const void* pvData = pkDataStream->LockRegion(uiRegionIndex,
            NiDataStream::LOCK_READ);
        NIASSERT(pvData && "pkDataStream->LockImpl failed!");
        NiMemcpy((NiUInt8*)pvTempOldIndexData, 
            pvData, uiSize);
        pkDataStream->Unlock(NiDataStream::LOCK_READ);
    }

    return pvTempOldIndexData;
}
//---------------------------------------------------------------------------
bool NiInstancingUtilities::DestroyDuplicatedIndexData(NiMesh* pkMesh, 
    NiDataStreamRef* pkIndexStreamRef)
{
    NIASSERT(pkMesh && pkIndexStreamRef);

    NiDataStream* pkOldDataStream = pkIndexStreamRef->GetDataStream();
    NIASSERT(pkOldDataStream);

    NiUInt32 uiTotalIndexCount = pkIndexStreamRef->GetTotalCount();
    NiUInt32 uiFrequency = uiTotalIndexCount / GetTransformsPerSubmesh(pkMesh);

    void* pvIndexData = GetAllIndexData(pkMesh, uiTotalIndexCount);

    if (pvIndexData)
    {
        // If possible destroy the duplicated index data and replace it with 
        // a single set of the index data.
        NiDataStreamElement::Format eFormat = 
            pkIndexStreamRef->GetElementDescAt(0).GetFormat();

        NiDataStream* pkNewDataStream = 
            NiDataStream::CreateSingleElementDataStream(
            eFormat,
            uiFrequency,
            (NiUInt8)(~NiDataStream::ACCESS_CPU_WRITE_STATIC_INITIALIZED & 
            pkOldDataStream->GetAccessMask()),
            NiDataStream::USAGE_VERTEX_INDEX,
            pvIndexData,
            false);
        NIASSERT(pkNewDataStream);

        // Replace reference to old datastream
        pkIndexStreamRef->SetDataStream(pkNewDataStream);

        NiFree(pvIndexData);
    }

    NiDataStream::Region kNewRegion(0, uiFrequency);
    pkIndexStreamRef->BindRegionToSubmesh(0, kNewRegion);
    pkMesh->SetSubmeshCount(1);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiInstancingUtilities::DuplicateIndexData(void* pvSourceData, 
    void* pvDestData, const NiDataStream* pkSourceDataStream, 
    NiDataStreamElement::Format eInFormat, 
    NiDataStreamElement::Format eOutFormat,
    NiUInt32 uiFrequency, NiUInt32 uiMaxDuplications,
    NiUInt32 ePrimitiveType)
{
    if (!pvSourceData || !pvDestData || !pkSourceDataStream)
        return false;

    // For strip primitive types add in space for degenerate 
    // tris/lines.
    NiUInt32 uiDegenerateCount = 0;
    NiUInt32 auiDegenerates[2];
    if (ePrimitiveType == NiPrimitiveType::PRIMITIVE_TRISTRIPS)
        uiDegenerateCount = 2;
    else if (ePrimitiveType == NiPrimitiveType::PRIMITIVE_LINESTRIPS)
        uiDegenerateCount = 1;
    
    if (uiDegenerateCount)
    {
        uiFrequency -= uiDegenerateCount;

        if (eInFormat == NiDataStreamElement::F_UINT32_1)
        {
            auiDegenerates[0] = ((NiUInt32*)pvSourceData)[uiFrequency-1];
            auiDegenerates[1] = ((NiUInt32*)pvSourceData)[0] + uiFrequency + 
                uiDegenerateCount;
        }
        else if (eInFormat == NiDataStreamElement::F_UINT16_1)
        {
            auiDegenerates[0] = ((NiUInt16*)pvSourceData)[uiFrequency-1];
            auiDegenerates[1] = ((NiUInt16*)pvSourceData)[0] + uiFrequency + 
                uiDegenerateCount;
        }

    }
    
    // Populate new index data
    NiUInt32 uiCurrentIndex = 0;
    for (NiUInt32 uiRegionIndex = 0; 
        uiRegionIndex < pkSourceDataStream->GetRegionCount(); uiRegionIndex++)
    {
        NiDataStream::Region kRegion =
            pkSourceDataStream->GetRegion(uiRegionIndex);

        NiUInt32 uiOffset = 0;
        for (NiUInt32 uiInstance = 0; uiInstance < uiMaxDuplications; 
            uiInstance++)
        {
            if (eInFormat == NiDataStreamElement::F_UINT32_1)
            {
                if (eOutFormat == NiDataStreamElement::F_UINT32_1)
                {
                    for (NiUInt32 uiIndex = 0; uiIndex < uiFrequency; 
                        uiIndex++)
                    {
                        ((NiUInt32*)pvDestData)[uiCurrentIndex++] =
                            ((NiUInt32*)pvSourceData)[uiIndex] + 
                            uiOffset;
                    }
                    
                    // Add in required degenerates
                    for (NiUInt32 uiIndex = 0; uiIndex < uiDegenerateCount; 
                        uiIndex++)
                    {
                        ((NiUInt32*)pvDestData)[uiCurrentIndex++] =
                            auiDegenerates[uiIndex] + uiOffset;
                    }
                }
                else if (eOutFormat == NiDataStreamElement::F_UINT16_1)
                {
                    for (NiUInt32 uiIndex = 0; uiIndex < uiFrequency; 
                        uiIndex++)
                    {
                        NIASSERT( (((NiUInt32*)pvSourceData)[uiIndex] + 
                            uiOffset) < USHRT_MAX);
                        ((NiUInt16*)pvDestData)[uiCurrentIndex++] =
                            static_cast<NiUInt16>(
                            ((NiUInt32*)pvSourceData)[uiIndex] + uiOffset);
                    }

                    // Add in required degenerates
                    for (NiUInt32 uiIndex = 0; uiIndex < uiDegenerateCount; 
                        uiIndex++)
                    {
                        ((NiUInt16*)pvDestData)[uiCurrentIndex++] =
                            static_cast<NiUInt16>(
                            auiDegenerates[uiIndex] + uiOffset);
                    }
                }
                else
                {
                    NiOutputDebugString("EnableMeshInstancing Error: "
                        "Unsupported index stream format.\n");
                    NIASSERT(false);
                    return false;
                }
            }
            else if (eInFormat == NiDataStreamElement::F_UINT16_1)
            {
                if (eOutFormat == NiDataStreamElement::F_UINT16_1)
                {
                    for (NiUInt32 uiIndex = 0; uiIndex < uiFrequency; 
                        uiIndex++)
                    {
                        ((NiUInt16*)pvDestData)[uiCurrentIndex++] =
                            static_cast<NiUInt16>(
                            ((NiUInt16*)pvSourceData)[uiIndex] + uiOffset);
                    }

                    // Add in required degenerates
                    for (NiUInt32 uiIndex = 0; uiIndex < uiDegenerateCount; 
                        uiIndex++)
                    {
                        ((NiUInt16*)pvDestData)[uiCurrentIndex++] =
                            static_cast<NiUInt16>(
                            auiDegenerates[uiIndex] + uiOffset);
                    }
                }
                else if (eOutFormat == NiDataStreamElement::F_UINT32_1)
                {
                    for (NiUInt32 uiIndex = 0; uiIndex < uiFrequency; 
                        uiIndex++)
                    {
                        ((NiUInt32*)pvDestData)[uiCurrentIndex++] =
                            ((NiUInt16*)pvSourceData)[uiIndex] + 
                            uiOffset;
                    }

                    // Add in required degenerates
                    for (NiUInt32 uiIndex = 0; uiIndex < uiDegenerateCount; 
                        uiIndex++)
                    {
                        ((NiUInt32*)pvDestData)[uiCurrentIndex++] =
                            auiDegenerates[uiIndex] + uiOffset;
                    }
                }
                else
                {
                    NiOutputDebugString("EnableMeshInstancing Error: "
                        "Unsupported index stream format.\n");
                    NIASSERT(false);
                    return false;
                }

            }
            else
            {
                NiOutputDebugString("EnableMeshInstancing Error: "
                    "Unsupported index stream format.\n");
                NIASSERT(false);
                return false;
            }

            uiOffset += uiFrequency + uiDegenerateCount;
        }

    }


    // Clamp last degenerate to index values inside the vertex range.
    for (NiUInt32 uiIndex = uiDegenerateCount; uiIndex > 0; 
        uiIndex--)
    {
        if (eOutFormat == NiDataStreamElement::F_UINT32_1)
        {
            NiUInt32 uiDegenIndex = ((NiUInt32*)pvDestData)
                [uiCurrentIndex - uiDegenerateCount - 1];
            ((NiUInt32*)pvDestData)[uiCurrentIndex - uiIndex] = uiDegenIndex;
        }
        else
        {
            NiUInt16 uiDegenIndex = ((NiUInt16*)pvDestData)
                [uiCurrentIndex - uiDegenerateCount - 1];
            ((NiUInt16*)pvDestData)[uiCurrentIndex - uiIndex] = uiDegenIndex;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiInstancingUtilities::CreateInstanceTransformStream(NiMesh* pkMesh,
    NiUInt32, NiUInt32 uiMaxDuplications, 
    NiUInt32 uiActiveDuplications, NiTransform* pkTransforms,
    bool bCPURead, bool bCullPerInstance)
{
    NIASSERT(uiMaxDuplications >= uiActiveDuplications);

    // Create instance transforms
    NiPoint4* pvTempTransformData = 
        (NiPoint4*)NiMalloc(sizeof(NiPoint4) * uiMaxDuplications * 3);

    NIASSERT(pvTempTransformData);
    NiTransform kIdentTrans;
    kIdentTrans.m_Translate = NiPoint3(0, 0, 0);
    kIdentTrans.m_Rotate = NiMatrix3::IDENTITY;
    kIdentTrans.m_fScale = 1.0f;

    for (NiUInt32 uiInstance = 0; uiInstance < uiMaxDuplications; uiInstance++)
    {
        if (!pkTransforms || uiInstance >= uiActiveDuplications)
        {
            PackTransform(kIdentTrans, &pvTempTransformData[uiInstance * 3]);
        }
        else
        {
            PackTransform(pkTransforms[uiInstance], 
                &pvTempTransformData[uiInstance * 3]);
        }

    }

    // Create the element set to be used the be transform data stream(s).
    NiDataStreamElementSet kElementSet;
    kElementSet.AddElement(NiDataStreamElement::F_FLOAT32_4);
    kElementSet.AddElement(NiDataStreamElement::F_FLOAT32_4);
    kElementSet.AddElement(NiDataStreamElement::F_FLOAT32_4);

    // Set up access and usage parameters to the primary transform stream.
    NiFixedString kSemantic;
    NiUInt32 uiSemanticIndex;
    NiUInt8 uiAccessMask = NiDataStream::ACCESS_CPU_WRITE_MUTABLE | 
        NiDataStream::ACCESS_GPU_READ;
    NiDataStream::Usage eUsage = NiDataStream::USAGE_VERTEX;

    if (bCPURead | bCullPerInstance)
        uiAccessMask |= NiDataStream::ACCESS_CPU_READ;

    // Create the primary transform data stream.
    NiDataStream* pkTransDataStream = 
        NiDataStream::CreateDataStream(kElementSet, uiMaxDuplications,
        uiAccessMask, eUsage, false);
    NIASSERT(pkTransDataStream);

    NiDataStream::Region kRegion(0, uiActiveDuplications);
    pkTransDataStream->AddRegion(kRegion);

    // Copy transforms into the primary data stream.
    void *pvData = pkTransDataStream->Lock(NiDataStream::LOCK_WRITE);
    NIASSERT(pvData && "pkTransDataStream->LockImpl failed!");
    NiMemcpy(pvData, pvTempTransformData, pkTransDataStream->GetSize());
    pkTransDataStream->Unlock(NiDataStream::LOCK_WRITE);

    // Create stream reference
    NiDataStreamRef* pkTransStreamRef = pkMesh->AddStreamRef();
    NIASSERT(pkTransStreamRef && "Failed to allocate pkStreamRef");
    pkTransStreamRef->SetDataStream(pkTransDataStream);
    pkTransStreamRef->SetPerInstance(true);

    uiSemanticIndex = 0;
    if (bCullPerInstance)
    {
        // If per instance culling is used then we need to create two data
        // streams. One CPU only data stream to be passes as an input to the 
        // NiInstanceCullingKernel, and a second to GPU only stream to serve 
        // as the output from the kernel and input to the GPU. Here we create
        // the CPU only stream.
        kSemantic = NiCommonSemantics::TRANSFORMS();
    }
    else
    {
        // If per instance culling is not used then we only need one stream 
        // that will feed directly to the GPU. Here we create the GPU stream.
        kSemantic = NiCommonSemantics::INSTANCETRANSFORMS();
    }

    // Create element references
    for (NiUInt32 ui = 0; ui < 3; ui++)
    {
        pkTransStreamRef->BindSemanticToElementDescAt(ui, kSemantic, 
            uiSemanticIndex++);
    }

    pkMesh->SetBaseInstanceStream(pkTransStreamRef);

    if (!bCullPerInstance)
    {
        pkMesh->SetVisibleInstanceStream(pkTransStreamRef);
    }
    else
    {
        NIVERIFY(CreateCollectorDataStream(pkMesh, uiMaxDuplications));
    }

    NIASSERT(pkMesh->GetBaseInstanceStream());
    NIASSERT(pkMesh->GetVisibleInstanceStream());

    NiFree(pvTempTransformData);

    return true;
}
//---------------------------------------------------------------------------
NiDataStreamRef* NiInstancingUtilities::CreateCollectorDataStream(
    NiMesh* pkMesh, NiUInt32 uiMaxDuplications)
{
    if (!pkMesh || uiMaxDuplications == 0)
        return NULL;

    // Create the element set to be used the be transform data stream(s).
    NiDataStreamElementSet kElementSet;
    kElementSet.AddElement(NiDataStreamElement::F_FLOAT32_4);
    kElementSet.AddElement(NiDataStreamElement::F_FLOAT32_4);
    kElementSet.AddElement(NiDataStreamElement::F_FLOAT32_4);

    // Create collector data stream
    NiDataStream* pkCollectorTransDataStream = 
        NiDataStream::CreateDataStream(
        kElementSet,
        uiMaxDuplications,
        NiDataStream::ACCESS_GPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
        NiDataStream::USAGE_VERTEX,
        false);
    NIASSERT(pkCollectorTransDataStream);

    // This data stream does not contain any unique data. Since it is used
    // only to collect the results of the InstanceCullingKernel and pass
    // them to the GPU it will not be streamed along with the other data
    // streams.
    pkCollectorTransDataStream->SetStreamable(false);

    // Create stream reference
    NiDataStreamRef* pkCollectorTransStreamRef = pkMesh->AddStreamRef();
    NIASSERT(pkCollectorTransStreamRef && 
        "Failed to allocate pkStreamRef");
    pkCollectorTransStreamRef->SetDataStream(pkCollectorTransDataStream);
    pkCollectorTransStreamRef->SetPerInstance(true);

    // Create element references
    NiUInt32 uiSemanticIndex = 0;
    for (NiUInt32 ui = 0; ui < 3; ui++)
    {
        pkCollectorTransStreamRef->BindSemanticToElementDescAt(ui, 
            NiCommonSemantics::INSTANCETRANSFORMS(), 
            uiSemanticIndex++);
    }

    pkMesh->SetVisibleInstanceStream(pkCollectorTransStreamRef);

    return pkCollectorTransStreamRef;
}
//---------------------------------------------------------------------------
bool NiInstancingUtilities::SetInstanceTransformations(NiMesh* pkMesh, 
     NiTransform* pkTransforms, NiUInt32 uiTransformCount, 
     NiUInt32 uiInstanceOffset)
{
    NIASSERT(pkMesh);
    if (!pkMesh->GetInstanced())
    {
        NiOutputDebugString("Warning: Tried to set an instanced transform on "
            "a non-instanced mesh.\n");
        return false;
    }

    NiDataStreamRef* pkStreamRef = pkMesh->GetBaseInstanceStream();
    NiDataStream* pkDataStream = pkStreamRef->GetDataStream();

    // The stream should have a stride of 3 NiPoint4 objects
    NIASSERT(pkDataStream->GetStride() == sizeof(NiPoint4) * 3);

    const NiDataStream::Region kRegion = pkStreamRef->GetRegionForSubmesh(0);
    const NiUInt32 uiMaxInstances = GetMaxInstanceCount(pkMesh);

    if (uiInstanceOffset + uiTransformCount > uiMaxInstances)
    {
        NiOutputDebugString("Warning: Tried to apply an instanced transform "
            "to a mesh beyond the maximum number of supplied instanced "
            "transforms on the mesh.\n");
        return false;
    }

    NiDataStream::LockType eLock = NiDataStream::LOCK_WRITE;
    NiPoint4* pkData = (NiPoint4*)pkDataStream->Lock((NiUInt8)eLock);

    NIASSERT(pkData && "pkDataStream->LockImpl failed!");
    pkData = pkData + (uiInstanceOffset * 3);

    for (NiUInt32 uiInstance = 0; 
        uiInstance < uiTransformCount; uiInstance++)
    {
        PackTransform(pkTransforms[uiInstance], pkData);
        pkData += 3;
    }

    pkDataStream->Unlock((NiUInt8)eLock);

    return true;
}
//---------------------------------------------------------------------------
bool NiInstancingUtilities::ComputeWorldBound(NiMesh* pkMesh)
{   
    const bool bInstanced = pkMesh->GetInstanced();

    if (!bInstanced)
        return false;

    const NiUInt32 uiActiveInstances = 
        NiInstancingUtilities::GetActiveInstanceCount(pkMesh);

    if (uiActiveInstances == 0)
        return false;

    NiDataStreamRef* pkStreamRef = pkMesh->GetBaseInstanceStream();
    NiDataStream* pkDataStream = pkStreamRef->GetDataStream();

    // The stream should have a stride of 3 NiPoint4 objects
    NIASSERT(pkDataStream->GetStride() == sizeof(NiPoint4) * 3);

    // Lock the data stream instance data for read.
    const NiPoint4* pkData = 
        (const NiPoint4*)pkDataStream->Lock(NiDataStream::LOCK_READ);

    if (!pkData)
    {
        NiOutputDebugString("Warning: Could not update world bounds of an "
            "instanced object.\n");
        return false;
    }

    NiBound kWorldBound, kLocalBound;
    NiTransform kTransform;
    NiPoint3 kRow0, kRow1, kRow2;

    kRow0 = NiPoint3(pkData[0].X(), pkData[0].Y(), pkData[0].Z());
    kRow1 = NiPoint3(pkData[1].X(), pkData[1].Y(), pkData[1].Z());
    kRow2 = NiPoint3(pkData[2].X(), pkData[2].Y(), pkData[2].Z());

    kTransform.m_Translate.x = pkData[0].W();
    kTransform.m_Translate.y = pkData[1].W();
    kTransform.m_Translate.z = pkData[2].W();

    kTransform.m_fScale = 1.0f;

    kTransform.m_Rotate.SetRow(0, kRow0);
    kTransform.m_Rotate.SetRow(1, kRow1);
    kTransform.m_Rotate.SetRow(2, kRow2);

    kLocalBound = pkMesh->GetModelBound();
    kWorldBound.Update(kLocalBound, kTransform);

    for (NiUInt32 uiInst = 1; uiInst < uiActiveInstances; uiInst++)
    {       
        kTransform.m_fScale = 1.0f;

        kRow0 = NiPoint3(pkData[uiInst * 3].X(), 
            pkData[uiInst * 3].Y(), pkData[uiInst * 3].Z());
        kRow1 = NiPoint3(pkData[(uiInst * 3) + 1].X(), 
            pkData[(uiInst * 3) + 1].Y(), pkData[(uiInst * 3) + 1].Z());
        kRow2 = NiPoint3(pkData[(uiInst * 3) + 2].X(), 
            pkData[(uiInst * 3) + 2].Y(), pkData[(uiInst * 3) + 2].Z());
        
        kTransform.m_Translate.x = pkData[uiInst * 3].W();
        kTransform.m_Translate.y = pkData[(uiInst * 3) + 1].W();
        kTransform.m_Translate.z = pkData[(uiInst * 3) + 2].W();

        kTransform.m_Rotate.SetRow(0, kRow0);
        kTransform.m_Rotate.SetRow(1, kRow1);
        kTransform.m_Rotate.SetRow(2, kRow2);

        NiBound kBound;
        kBound.Update(kLocalBound, kTransform);
        kWorldBound.Merge(&kBound);
    }

    pkMesh->SetWorldBound(kWorldBound);

    pkDataStream->Unlock(NiDataStream::LOCK_READ);

    return true;
}
//---------------------------------------------------------------------------
const NiUInt32 NiInstancingUtilities::GetVisibleInstanceCount(
    const NiMesh* pkMesh)
{
    if (!pkMesh || !pkMesh->GetInstanced())
        return 0;

    const NiDataStreamRef* pkStreamRef = pkMesh->GetVisibleInstanceStream();
    NIASSERT(pkStreamRef);

    NiUInt32 uiVisibleCount = 0;
    for (NiUInt32 uiSubMesh = 0; uiSubMesh < pkMesh->GetSubmeshCount(); 
        uiSubMesh++)
    {
        const NiDataStream::Region& kRegion = 
            pkStreamRef->GetRegionForSubmesh(uiSubMesh);
        uiVisibleCount += kRegion.GetRange();
    }

    return uiVisibleCount;
}
//---------------------------------------------------------------------------
const NiUInt32 NiInstancingUtilities::GetActiveInstanceCount(
    const NiMesh* pkMesh)
{
    if (!pkMesh || !pkMesh->GetInstanced())
        return 0;

    const NiDataStreamRef* pkStreamRef = pkMesh->GetBaseInstanceStream();
    NIASSERT(pkStreamRef);

    NiUInt32 uiActiveCount = 0;
    for (NiUInt32 uiSubMesh = 0; uiSubMesh < pkMesh->GetSubmeshCount(); 
        uiSubMesh++)
    {
        const NiDataStream::Region& kRegion = 
            pkStreamRef->GetRegionForSubmesh(uiSubMesh);
        uiActiveCount += kRegion.GetRange();
    }

    return uiActiveCount;
}
//---------------------------------------------------------------------------
const NiUInt32 NiInstancingUtilities::GetMaxInstanceCount(
    const NiMesh* pkMesh)
{
    if (!pkMesh || !pkMesh->GetInstanced())
        return 0;

    const NiDataStreamRef* pkStreamRef = pkMesh->GetBaseInstanceStream();
    NIASSERT(pkStreamRef);

    const NiDataStream* pkDataStream = pkStreamRef->GetDataStream();

    return pkDataStream->GetTotalCount();
}
//---------------------------------------------------------------------------
const NiUInt32 NiInstancingUtilities::GetTransformsPerSubmesh(
    const NiMesh* pkMesh)
{
    if (!pkMesh || !pkMesh->GetInstanced())
        return 0;

    NiUInt32 uiTransformsPerSubmesh = 0;
    NiDataStreamRef* pkBaseInstStreamRef = pkMesh->GetBaseInstanceStream();
    NIASSERT(pkBaseInstStreamRef);

    if (pkMesh->GetSubmeshCount() == 1)
    {
        uiTransformsPerSubmesh = pkBaseInstStreamRef->GetTotalCount();
    }
    else if (pkMesh->GetSubmeshCount() > 1)
    {
        NiDataStream::Region& kRegion = 
            pkBaseInstStreamRef->GetRegionForSubmesh(1);
        uiTransformsPerSubmesh = kRegion.GetStartIndex();
    }
    else
    {
        NIASSERT(false);
        uiTransformsPerSubmesh = 0;
    }

    return uiTransformsPerSubmesh;
}
//---------------------------------------------------------------------------
const NiUInt32 NiInstancingUtilities::GetVisibleSubmeshCount(
    const NiMesh* pkMesh)
{    
    if (!pkMesh || !pkMesh->GetInstanced())
        return 0;

    NiDataStreamRef* pkVisibleStream = pkMesh->GetVisibleInstanceStream();
    NIASSERT(pkVisibleStream);

    NiUInt32 uiVisibleSubmeshCount = 0;
    for (NiUInt32 ui = 0; ui < pkMesh->GetSubmeshCount(); ui++)
    {
        const NiDataStream::Region& kRegion = 
            pkVisibleStream->GetRegionForSubmesh(ui);
        uiVisibleSubmeshCount += kRegion.GetRange() > 0;
    }

    return uiVisibleSubmeshCount;
}
//---------------------------------------------------------------------------
void NiInstancingUtilities::SetActiveInstanceCount(NiMesh* pkMesh,
    NiUInt32 uiActiveCount)
{
    if (!pkMesh || !pkMesh->GetInstanced())
        return;

#ifdef NIDEBUG
    if (uiActiveCount > NiInstancingUtilities::GetMaxInstanceCount(pkMesh))
    {
        NILOG("Error: Attempted to set active instance count to a value"
            " larger than the max instance count.\n");
        return;
    }
#endif 

    NiUInt32 uiFrequency = GetInstanceIndexFrequency(pkMesh, 0);
    NiUInt32 uiTransformsPerMesh = GetTransformsPerSubmesh(pkMesh);
    NiUInt32 uiTotalTransformCount = 0;
    NiUInt32 uiInc = 0;

    NiDataStreamRef* pkBaseStreamRef = pkMesh->GetBaseInstanceStream();
    NIASSERT(pkBaseStreamRef);

    NiDataStreamRef* pkIndexStreamRef = 
        pkMesh->FindStreamRef(NiCommonSemantics::INDEX());
    NIASSERT(pkIndexStreamRef);
    NiDataStream* pkIndexStream = pkIndexStreamRef->GetDataStream();
    
    if (pkIndexStream->GetRegionCount() == 3)
    {       
        NiUInt32 uiRequiredIndices = uiFrequency * uiActiveCount;
        NiUInt32 uiSecondRegionSize = 0;
        if (uiRequiredIndices < pkIndexStream->GetTotalCount() ||
            pkIndexStream->GetTotalCount() % uiRequiredIndices == 0)
        {
            // If the required number of indicies for 'uiActiveCount' instances
            // 'evenly matches' or is 'less than' the number of indicies we 
            // have available. Just reuse the first region in the index data 
            // stream and zero out the second region.
            NiDataStream::Region& kFirstRegion = pkIndexStream->GetRegion(0);
            kFirstRegion.SetRange(uiFrequency *
                NiMin((int)uiTransformsPerMesh, (int)uiActiveCount) );

            NiDataStream::Region& kSecondRegion = pkIndexStream->GetRegion(1);
            kSecondRegion.SetRange(kFirstRegion.GetRange());
            uiSecondRegionSize = kSecondRegion.GetRange();

            NIASSERT(pkIndexStream->GetRegion(2).GetRange() == 0);
        }
        else
        {
            NiDataStream::Region& kFirstRegion = pkIndexStream->GetRegion(0);
            kFirstRegion.SetRange(uiFrequency * uiTransformsPerMesh);

            NIASSERT(kFirstRegion.GetRange() == 
                pkIndexStream->GetTotalCount());

            NiDataStream::Region& kSecondRegion = pkIndexStream->GetRegion(1);
            NIASSERT((uiActiveCount % uiTransformsPerMesh) * uiFrequency < 
                pkIndexStream->GetTotalCount());
            kSecondRegion.SetRange(uiFrequency *
                (uiActiveCount % uiTransformsPerMesh) );
            uiSecondRegionSize = kSecondRegion.GetRange();
        
            NIASSERT(pkIndexStream->GetRegion(2).GetRange() == 0);
        }

        NiUInt32 uiSelectedRegion = 0;
        NiUInt32 uiTotalIndexCount = uiRequiredIndices;
        for (NiUInt32 uiSubmesh = 0; uiSubmesh < pkMesh->GetSubmeshCount();
            uiSubmesh++)
        {
            if (uiTotalIndexCount >= uiFrequency * uiTransformsPerMesh)
            {
                uiSelectedRegion = 0;
                uiTotalIndexCount -= uiFrequency * uiTransformsPerMesh;
            }
            else if (uiTotalIndexCount != 0)
            {
                uiSelectedRegion = 1;
                uiTotalIndexCount -= uiSecondRegionSize;
                NIASSERT(uiTotalIndexCount == 0);
            }
            else
            {
                uiSelectedRegion = 2;
            }

            pkIndexStreamRef->BindRegionToSubmesh(uiSubmesh, uiSelectedRegion);
        }
    }
  

    NiUInt32 uiActiveSubmeshCount = 0;
    for (NiUInt32 ui = 0; ui < pkMesh->GetSubmeshCount(); ui++)
    {     
        if (uiTotalTransformCount >= uiActiveCount)
        {
            uiInc = 0;
        }
        else
        {
            uiInc = NiMin((int)uiTransformsPerMesh, 
                uiActiveCount - uiTotalTransformCount);
            uiActiveSubmeshCount++;
        }

        uiTotalTransformCount = uiTotalTransformCount + uiInc;

        NiDataStream::Region& kBaseRegion = 
            pkBaseStreamRef->GetRegionForSubmesh(ui);
        kBaseRegion.SetRange(uiInc);
    }

    pkMesh->UpdateCachedPrimitiveCount();
}
//---------------------------------------------------------------------------
void NiInstancingUtilities::SetVisibleInstanceCount(NiMesh* pkMesh,
    NiUInt32 uiVisibleCount)
{
    if (!pkMesh || !pkMesh->GetInstanced())
        return;

#ifdef NIDEBUG
    if (uiVisibleCount > NiInstancingUtilities::GetMaxInstanceCount(pkMesh))
    {
        NILOG("Error: Attempted to set active instance count to a value"
            " larger than the max instance count.\n");
        return;
    }
#endif 

    NiUInt32 uiFrequency = GetInstanceIndexFrequency(pkMesh, 0);
    NiUInt32 uiTransformsPerMesh = GetTransformsPerSubmesh(pkMesh);
    NiUInt32 uiTotalTransformCount = 0;
    NiUInt32 uiInc = 0;

    NiDataStreamRef* pkVisibleStreamRef = pkMesh->GetVisibleInstanceStream();
    NIASSERT(pkVisibleStreamRef);


    NiDataStreamRef* pkIndexStreamRef = 
        pkMesh->FindStreamRef(NiCommonSemantics::INDEX());
    NIASSERT(pkIndexStreamRef);
    NiDataStream* pkIndexStream = pkIndexStreamRef->GetDataStream();

    if (pkIndexStream->GetRegionCount() == 3)
    {
        NiUInt32 uiRequiredIndices = uiFrequency * uiVisibleCount;
        NiUInt32 uiSecondRegionSize = 0;
        if (uiRequiredIndices < pkIndexStream->GetTotalCount() ||
            pkIndexStream->GetTotalCount() % uiRequiredIndices == 0)
        {
            // If the required number of indicies for 'uiVisibleCount' 
            // instances 'evenly matches' or is 'less than' the number of 
            // indicies we have available. Just reuse the first region in the
            // index data stream and zero out the second region.
            NiDataStream::Region& kFirstRegion = pkIndexStream->GetRegion(0);
            kFirstRegion.SetRange(uiFrequency *
                NiMin((int)uiTransformsPerMesh, (int)uiVisibleCount) );

            NiDataStream::Region& kSecondRegion = pkIndexStream->GetRegion(1);
            kSecondRegion.SetRange(kFirstRegion.GetRange());
            uiSecondRegionSize = kSecondRegion.GetRange();

            NIASSERT(pkIndexStream->GetRegion(2).GetRange() == 0);
        }
        else
        {
            NiDataStream::Region& kFirstRegion = pkIndexStream->GetRegion(0);
            kFirstRegion.SetRange(uiFrequency * uiTransformsPerMesh);

            NIASSERT(kFirstRegion.GetRange() == 
                pkIndexStream->GetTotalCount());

            NiDataStream::Region& kSecondRegion = pkIndexStream->GetRegion(1);
            NIASSERT((uiVisibleCount % uiTransformsPerMesh) * uiFrequency < 
                pkIndexStream->GetTotalCount());
            kSecondRegion.SetRange(uiFrequency *
                (uiVisibleCount % uiTransformsPerMesh) );
            uiSecondRegionSize = kSecondRegion.GetRange();

            NIASSERT(pkIndexStream->GetRegion(2).GetRange() == 0);
        }

        NiUInt32 uiSelectedRegion = 0;
        NiUInt32 uiTotalIndexCount = uiRequiredIndices;
        for (NiUInt32 uiSubmesh = 0; uiSubmesh < pkMesh->GetSubmeshCount();
            uiSubmesh++)
        {
            if (uiTotalIndexCount >= uiFrequency * uiTransformsPerMesh)
            {
                uiSelectedRegion = 0;
                uiTotalIndexCount -= uiFrequency * uiTransformsPerMesh;
            }
            else if (uiTotalIndexCount != 0)
            {
                uiSelectedRegion = 1;
                uiTotalIndexCount -= uiSecondRegionSize;
                NIASSERT(uiTotalIndexCount == 0);
            }
            else
            {
                uiSelectedRegion = 2;
            }

            pkIndexStreamRef->BindRegionToSubmesh(uiSubmesh, uiSelectedRegion);
        }
    }
  

    NiUInt32 uiActiveSubmeshCount = 0;
    for (NiUInt32 ui = 0; ui < pkMesh->GetSubmeshCount(); ui++)
    {     
        if (uiTotalTransformCount >= uiVisibleCount)
        {
            uiInc = 0;
        }
        else
        {
            uiInc = NiMin((int)uiTransformsPerMesh, 
                uiVisibleCount - uiTotalTransformCount);
            uiActiveSubmeshCount++;
        }

        uiTotalTransformCount = uiTotalTransformCount + uiInc;

        NiDataStream::Region& kBaseRegion = 
            pkVisibleStreamRef->GetRegionForSubmesh(ui);
        kBaseRegion.SetRange(uiInc);
    }

    pkMesh->UpdateCachedPrimitiveCount();
}
//---------------------------------------------------------------------------
const NiUInt32 NiInstancingUtilities::GetInstanceIndexFrequency(
    const NiMesh* pkMesh, NiUInt32 uiSubmeshIdx)
{
    const NiDataStreamRef* pkStreamRef = 
        pkMesh->FindStreamRef(NiCommonSemantics::INDEX());
    NIASSERT(pkStreamRef);
    
    return GetInstanceIndexFrequency(pkMesh, uiSubmeshIdx, pkStreamRef);
}
//---------------------------------------------------------------------------
const NiUInt32 NiInstancingUtilities::GetInstanceIndexFrequency(
    const NiMesh* pkMesh,
    NiUInt32 uiSubmeshIdx,
    const NiDataStreamRef* pkIndexBufferRef)
{
    NI_UNUSED_ARG(uiSubmeshIdx);
    NIASSERT(pkMesh->GetSubmeshCount() > uiSubmeshIdx);
    NIASSERT(pkMesh->GetInstanced());
    NIASSERT(pkIndexBufferRef);
    NIASSERT(pkIndexBufferRef->GetUsage() == NiDataStream::USAGE_VERTEX_INDEX);
    
    NiUInt32 uiIndexBufferCount = 
        pkIndexBufferRef->GetTotalCount();

    if (InstancingIndexDuplicationRequired())
    {
        // The index buffer's range should be:
        // IndicesPerInstance * MaxInstanceCount
        // We need IndicesPerInstance, so we divide through
        uiIndexBufferCount /= 
              NiInstancingUtilities::GetTransformsPerSubmesh(pkMesh);
    }

    return uiIndexBufferCount;
}
//---------------------------------------------------------------------------
const NiUInt32 NiInstancingUtilities::GetInstanceIndexCount(
    const NiMesh* pkMesh, NiUInt32 uiSubmeshIdx)
{
    const NiDataStreamRef* pkStreamRef = 
        pkMesh->FindStreamRef(NiCommonSemantics::INDEX());
    
    NIASSERT(pkStreamRef);
    return GetInstanceIndexCount(pkMesh, uiSubmeshIdx, pkStreamRef);
}
//---------------------------------------------------------------------------
const NiUInt32 NiInstancingUtilities::GetInstanceIndexCount(
    const NiMesh* pkMesh, NiUInt32 uiSubmeshIdx, 
    const NiDataStreamRef* pkIndexBufferRef)
{
    NIASSERT(pkMesh->GetSubmeshCount() > uiSubmeshIdx);
    NIASSERT(pkIndexBufferRef);
    NIASSERT(pkMesh->GetInstanced());
    NIASSERT(pkIndexBufferRef->GetUsage() == NiDataStream::USAGE_VERTEX_INDEX);

    const NiDataStream::Region& kRegion = 
        pkIndexBufferRef->GetRegionForSubmesh(uiSubmeshIdx);
    NiUInt32 uiIndexBufferCount = kRegion.GetRange();

#if defined(_XENON) || defined(_PS3)
    // The index buffer's range should be:
    // IndicesPerInstance * MaxInstanceCount
    // We need IndicesPerInstance, so we divide through
    uiIndexBufferCount /= 
        NiInstancingUtilities::GetMaxInstanceCount(pkMesh);
#endif

    // We now need the count to be:
    // IndicesPerInstance * VisibleInstanceCount
    uiIndexBufferCount *= 
        NiInstancingUtilities::GetVisibleInstanceCount(pkMesh);

    return uiIndexBufferCount;
}
//---------------------------------------------------------------------------
