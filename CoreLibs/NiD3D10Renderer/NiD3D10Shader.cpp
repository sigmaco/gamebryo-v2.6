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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10Shader.h"

#include "NiD3D10Error.h"
#include "NiD3D10MeshMaterialBinding.h"
#include "NiD3D10GeometryShader.h"
#include "NiD3D10Pass.h"
#include "NiD3D10PixelFormat.h"
#include "NiD3D10PixelShader.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10RenderStateManager.h"
#include "NiD3D10RenderStateGroup.h"
#include "NiD3D10ShaderConstantManager.h"
#include "NiD3D10ShaderConstantMap.h"
#include "NiD3D10VertexShader.h"

#include <NiMesh.h>
#include <NiSCMExtraData.h>
#include <NiShaderConstantMapEntry.h>
#include <NiShadowGenerator.h>
#include <NiShadowMap.h>

NiImplementRTTI(NiD3D10Shader, NiD3D10ShaderInterface);

NiFixedString NiD3D10Shader::ms_kEmergentShaderMapName = NULL;
NiD3D10Shader::DynamicEffectPacker NiD3D10Shader::ms_apfnDynEffectPackers[
    NiTextureEffect::NUM_COORD_GEN];

//---------------------------------------------------------------------------
void NiD3D10Shader::_SDMInit()
{
    ms_kEmergentShaderMapName = "__NDL_SCMData";

    ms_apfnDynEffectPackers[NiTextureEffect::WORLD_PARALLEL] = 
        &PackWorldParallelEffect;
    ms_apfnDynEffectPackers[NiTextureEffect::WORLD_PERSPECTIVE] = 
        &PackWorldPerspectiveEffect;
    ms_apfnDynEffectPackers[NiTextureEffect::SPHERE_MAP] = 
        &PackWorldSphereEffect;
    ms_apfnDynEffectPackers[NiTextureEffect::SPECULAR_CUBE_MAP] = 
        &PackSpecularCubeEffect;
    ms_apfnDynEffectPackers[NiTextureEffect::DIFFUSE_CUBE_MAP] = 
        &PackDiffuseCubeEffect;
}
//---------------------------------------------------------------------------
void NiD3D10Shader::_SDMShutdown()
{
    ms_kEmergentShaderMapName = NULL;
}
//---------------------------------------------------------------------------
NiD3D10Shader::NiD3D10Shader() :
    m_uiCurrentPass(0),
    m_pkCurrentPass(NULL),
    m_uiBoneMatrixRegisters(4),
    m_bTransposeBones(false),
    m_bWorldSpaceBones(false),
    m_bUsesNiRenderState(false)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10Shader::~NiD3D10Shader()
{
}
//---------------------------------------------------------------------------
bool NiD3D10Shader::IsInitialized()
{
    return m_bInitialized;
}
//---------------------------------------------------------------------------
bool NiD3D10Shader::Initialize()
{
    return NiD3D10ShaderInterface::Initialize();
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::PreProcessPipeline(const NiRenderCallContext& kRCC)
{
    if (!m_bInitialized)
    {
        // Safety catch - fail if the shader hasn't been initialized
        return UINT_MAX;
    }

    NIASSERT(NiD3D10Renderer::GetRenderer());

    NiD3D10RenderStateManager* pkRenderState = 
        NiD3D10Renderer::GetRenderer()->GetRenderStateManager();

    NIASSERT(pkRenderState);

    if (m_bUsesNiRenderState)
        pkRenderState->SetProperties(kRCC.m_pkState);

    if (m_spRenderStateGroup)
        pkRenderState->SetRenderStateGroup(m_spRenderStateGroup);

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::FirstPass()
{
    m_uiCurrentPass = 0;
    unsigned int uiPassCount = m_kPasses.GetSize();
    if (m_uiCurrentPass < uiPassCount)
        m_pkCurrentPass = m_kPasses.GetAt(m_uiCurrentPass);
    else
        m_pkCurrentPass = NULL;

    return uiPassCount;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::SetupRenderingPass(const NiRenderCallContext& kRCC)
{
    NIASSERT(m_pkCurrentPass);
    NIASSERT(kRCC.m_uiPass == m_uiCurrentPass);

    // 
    // Set up Stream Output
    // 
    //   This must be done here (not in PreRenderSubmesh()) because the
    //   clearing of the stream out targets (if enabled) should only happen 
    //   once per MESH, not once per submesh. (Just in case we ever add
    //   support for submeshes + Stream Output.)
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    ID3D10Device* pkDevice = pkRenderer->GetD3D10Device();
    NiStreamOutSettings& kSOS = m_pkCurrentPass->GetStreamOutSettings();
    unsigned int uiStreamTargetCount = kSOS.GetStreamOutTargetCount();
    if (uiStreamTargetCount == 0)
    {
        pkDevice->SOSetTargets(0, NULL, NULL);
    }
    else
    {
        NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);
        unsigned int uiMeshOutputStreamCount = 
            pkMesh->GetCurrentMaterialOutputStreamRefCount();

        // Append or Clear?
        bool bAppend = kSOS.GetStreamOutAppend();
        if (pkMesh->CheckIfActiveMaterialStreamOutBuffersNeedCleared())
            bAppend = false;
        if (!bAppend)  // flag the mesh that we're doing the clear.
            pkMesh->OnActiveMaterialStreamOutBuffersCleared();

        ID3D10Buffer* ppSOTargets[D3D10_SO_BUFFER_SLOT_COUNT];
        unsigned int ppOffsets[D3D10_SO_BUFFER_SLOT_COUNT];
        for (unsigned int i = 0; i < uiStreamTargetCount; i++)
        {
            // offset: 0 = write to stream start; -1 = append
            ppOffsets[i] = bAppend ? -1 : 0;

            // Find the correct stream, by name.
            ppSOTargets[i] = NULL;
            const NiFixedString& kStreamName = kSOS.GetStreamOutTarget(i);
            for (unsigned int j = 0; j < uiMeshOutputStreamCount; j++)
            {
                const NiDataStreamRef* pkStreamRef = 
                    pkMesh->GetCurrentMaterialOutputStreamRefAt(j);
                const NiFixedString& kStreamName2 = 
                    pkMesh->GetCurrentMaterialOutputStreamRefNameAt(j);
                if (kStreamName == kStreamName2)
                {
                    // We've now matched desired output stream by name, to an
                    // output stream on the mesh with that name as its 
                    // semantic.
                    NiD3D10DataStream* pkStream = NiVerifyStaticCast(
                        NiD3D10DataStream, pkStreamRef->GetDataStream());
                    NIASSERT(0 !=
                        (pkStream->GetUsage() | NiDataStream::USAGE_VERTEX));

                    ppSOTargets[i] = pkStream->GetBuffer();
                    if (!ppSOTargets[i])
                    {
                        pkStream->UpdateD3D10Buffers();
                        ppSOTargets[i] = pkStream->GetBuffer();
                    }
                    NIASSERT(ppSOTargets[i]);

                    break;
                }
            }
            if (!ppSOTargets[i])
            {
                NiD3D10Error::ReportWarning(
                    "NiD3D10Shader::SetupRenderingPass(): Error: "
                    "Failed to bind one or more Stream Output targets.\n");
                return UINT_MAX;
            }
        }
        pkDevice->SOSetTargets(uiStreamTargetCount, ppSOTargets, ppOffsets);
    } 

    return m_pkCurrentPass->SetupRenderingPass(kRCC);
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::SetupTransformations(
    const NiRenderCallContext& kRCC)
{
    NIASSERT(NiD3D10Renderer::GetRenderer());
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();

    if (m_uiCurrentPass == 0)
        pkRenderer->SetModelTransform(*kRCC.m_pkWorld);

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::SetupShaderPrograms(
    const NiRenderCallContext& kRCC)
{
    // If there is an NiSCMExtraData object on this geometry, we want to 
    // reset the iterator to 0 so we can hit that cache when we set 
    // attribute constants.
    ResetSCMExtraData(kRCC.m_pkMesh);

    // Setup the shader programs and constants for the current pass.
    // If this is the first pass, then also set the 'global' constants
    // and render states.
    // Set the shader constants
    NIASSERT(m_pkCurrentPass);
    unsigned int uiRet = m_pkCurrentPass->ApplyShaderPrograms(kRCC);

    // Now that the shaders have been set, if there are any 'global' 
    // mappings, set them now. This has to occur now since the pixel
    // shader has to be set prior to setting pixel shader constants.
    // This must be done every pass, since using a different shader on
    // a different pass may require constant remapping.
    NiShaderError eErr = NISHADERERR_OK;

    NIASSERT(NiD3D10Renderer::GetRenderer());

    NiD3D10ShaderConstantManager* pkShaderConstantManager = 
        NiD3D10Renderer::GetRenderer()->GetShaderConstantManager();
    NIASSERT(pkShaderConstantManager);

    // Vertex shader mappings
    NiD3D10VertexShader* pkVertexShader = m_pkCurrentPass->GetVertexShader();
    if (pkVertexShader)
    {
        const unsigned int uiMapCount = m_kVertexShaderConstantMaps.GetSize();
        NIASSERT(uiMapCount == 0);
        for (unsigned int i = 0; i < uiMapCount; i++)
        {
            NiD3D10ShaderConstantMap* pkMap = 
                m_kVertexShaderConstantMaps.GetAt(i);
            if (pkMap)
            {
                eErr = pkMap->UpdateShaderConstants(kRCC, true);
                pkShaderConstantManager->SetShaderConstantMap(
                    pkVertexShader, pkMap);
            }
        }
    }

    // Geometry shader mappings
    NiD3D10GeometryShader* pkMeshShader = 
        m_pkCurrentPass->GetGeometryShader();
    if (pkMeshShader)
    {
        const unsigned int uiMapCount = 
            m_kGeometryShaderConstantMaps.GetSize();
        NIASSERT(uiMapCount == 0);
        for (unsigned int i = 0; i < uiMapCount; i++)
        {
            NiD3D10ShaderConstantMap* pkMap = 
                m_kGeometryShaderConstantMaps.GetAt(i);
            if (pkMap)
            {
                eErr = pkMap->UpdateShaderConstants(kRCC, true);
                pkShaderConstantManager->SetShaderConstantMap(
                    pkMeshShader, pkMap);
            }
        }
    }

    // Pixel shader mappings
    NiD3D10PixelShader* pkPixelShader = m_pkCurrentPass->GetPixelShader();
    if (pkPixelShader)
    {
        const unsigned int uiMapCount = m_kPixelShaderConstantMaps.GetSize();
        NIASSERT(uiMapCount == 0);
        for (unsigned int i = 0; i < uiMapCount; i++)
        {
            NiD3D10ShaderConstantMap* pkMap = 
                m_kPixelShaderConstantMaps.GetAt(i);
            if (pkMap)
            {
                eErr = pkMap->UpdateShaderConstants(kRCC, true);
                pkShaderConstantManager->SetShaderConstantMap(
                    pkPixelShader, pkMap);
            }
        }
    }

    uiRet = m_pkCurrentPass->ApplyShaderConstants(kRCC);

    return uiRet;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::PreRenderSubmesh(const NiRenderCallContext& kRCC)
{
    NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);
    NiD3D10MeshMaterialBinding* pkGeometryData = 
        (NiD3D10MeshMaterialBinding*)kRCC.m_pkMeshMaterialBinding;
//    unsigned int uiStreamCount = pkMesh->GetStreamRefCount();

    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    ID3D10Device* pkDevice = pkRenderer->GetD3D10Device();

    NiD3D10MeshMaterialBinding* pkMMB = 
        (NiD3D10MeshMaterialBinding*)kRCC.m_pkMeshMaterialBinding;

    bool bDrawAuto = pkMesh->GetInputDataIsFromStreamOut();

    //
    // Set Index Stream
    //
    NiUInt32 uiIndexStreamsSet = 0;
    const NiDataStreamRef* pkIndexStreamRef = pkMMB->GetIndexStreamRef();
    bool bAdjacency = false;

    if (pkIndexStreamRef)
    {
        NiD3D10DataStream* pkIndexStream = NiVerifyStaticCast(
            NiD3D10DataStream, 
            pkIndexStreamRef->GetDataStream());

        pkIndexStream->UpdateD3D10Buffers();

        // Require GPU Read
        if ((pkIndexStream->GetAccessMask() & NiDataStream::ACCESS_GPU_READ)
            == 0)
        {
            return UINT_MAX;
        }

        // Only element 0 allowed for index buffers
        const NiDataStreamElement& kElem = pkIndexStream->GetElementDescAt(0);

        DXGI_FORMAT eIBFormat = 
            NiD3D10PixelFormat::DetermineDXGIFormat(kElem.GetFormat());

        if (eIBFormat == DXGI_FORMAT_UNKNOWN ||
            !pkRenderer->DoesFormatSupportFlag(eIBFormat, 
            D3D10_FORMAT_SUPPORT_IA_INDEX_BUFFER))
        {
            return UINT_MAX;
        }

        // Get the appropriate slot and the offset to that slot
        const NiDataStream::Region& kRegion = 
            pkIndexStreamRef->GetRegionForSubmesh(kRCC.m_uiSubmesh);

        const NiUInt32 uiStride = pkIndexStream->GetStride();               
        const NiUInt32 uiRegionOffset = kRegion.GetStartIndex() * uiStride;
        pkDevice->IASetIndexBuffer(
            pkIndexStream->GetBuffer(), 
            eIBFormat, 
            uiRegionOffset);
        uiIndexStreamsSet++;
    }


    //
    // Set Vertex Streams
    //
    ID3D10Buffer* apkVertexBuffers[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    NiUInt32 auiVBStrides[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    NiUInt32 auiVBOffsets[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    unsigned int uiVertexStreams = 0;

    const NiUInt16* puiStreamsToSet = pkMMB->GetStreamsToSetArray();
    NiUInt32 uiStreamCount = pkMMB->GetLastValidStream() + 1;
    NIASSERT(uiStreamCount < D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
    for (NiUInt32 i = 0; i < uiStreamCount; i++)
    {
        NiUInt16 uiStreamRefIndex = puiStreamsToSet[i];
        NiDataStreamRef* pkStreamRef = 
            pkMesh->GetStreamRefAt(uiStreamRefIndex);

        NiD3D10DataStream* pkStream = NiVerifyStaticCast(NiD3D10DataStream, 
            pkStreamRef->GetDataStream());
        NIASSERT((pkStream->GetUsage() == NiDataStream::USAGE_VERTEX) ||
            (pkStream->GetUsage() == NiDataStream::USAGE_DISPLAYLIST));

        pkStream->UpdateD3D10Buffers();

        // Get the appropriate slot and the offset to that slot
        NiUInt32 uiStride = pkStream->GetStride();
        if (pkStream->GetGPUConstantSingleEntry())
        {
            uiStride = 0;
        }

        NiUInt32 uiRegionOffset = 0;
        if (!bDrawAuto)
        {
            NiDataStream::Region& kRegion = 
                pkStreamRef->GetRegionForSubmesh(kRCC.m_uiSubmesh);
            uiRegionOffset = kRegion.GetStartIndex() * uiStride;
        }

        // Require GPU Read
        if ((pkStream->GetAccessMask() & NiDataStream::ACCESS_GPU_READ) == 0)
            continue;

        apkVertexBuffers[uiVertexStreams] = pkStream->GetBuffer();
        auiVBStrides[uiVertexStreams] = uiStride;
        auiVBOffsets[uiVertexStreams] = uiRegionOffset;
        uiVertexStreams++;
    }

    pkDevice->IASetVertexBuffers(0,
        uiVertexStreams,
        apkVertexBuffers,
        auiVBStrides,
        auiVBOffsets);

    //
    // Set topology
    //
    D3D10_PRIMITIVE_TOPOLOGY eTopology = 
        NiD3D10Renderer::GetD3D10TopologyFromPrimitiveType(
        pkMesh->GetPrimitiveType(), bAdjacency);
    if (eTopology == D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED)
    {
        return UINT_MAX;
    }
    pkDevice->IASetPrimitiveTopology(eTopology);

    //
    // Set input layout
    //

    // Update input layout with current pass's input signature
    ID3D10Blob* pkByteCode = m_pkCurrentPass->GetInputSignature();
    NIASSERT(pkByteCode);
    pkGeometryData->UpdateInputLayout(pkByteCode->GetBufferPointer(),
        (NiUInt32)pkByteCode->GetBufferSize());

    pkDevice->IASetInputLayout(pkGeometryData->GetInputLayout());

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::NextPass()
{
    // Close out the current pass
    if (m_pkCurrentPass != NULL)
        m_pkCurrentPass->PostProcessRenderingPass(m_uiCurrentPass);

    m_uiCurrentPass++;
    unsigned int uiPassCount = m_kPasses.GetSize();
    unsigned int uiRemainingPasses = 0;
    if (m_uiCurrentPass < uiPassCount)
    {
        m_pkCurrentPass = m_kPasses.GetAt(m_uiCurrentPass);
        uiRemainingPasses = uiPassCount - m_uiCurrentPass; 
    }
    else
    {
        m_pkCurrentPass = NULL;
    }

    return uiRemainingPasses;
}
//---------------------------------------------------------------------------
bool NiD3D10Shader::SetupGeometry(NiRenderObject* pkMesh, 
    NiMaterialInstance* pkMaterialInstance)
{
    // Fill this in with the geometry getting assigned 'default' extra
    // data instances the shader expects to see. By default, NiD3DShader
    // simply sets up the NiSCMExtraData so that the engine does not have
    // to call strcmp too many times when rendering.
    SetupSCMExtraData(this, pkMesh);

    pkMaterialInstance->UpdateSemanticAdapterTable(pkMesh);

    // Create the vertex declaration
    NiD3D10MeshMaterialBindingPtr spMMB = 
        NiD3D10MeshMaterialBinding::Create(
            NiVerifyStaticCast(NiMesh, pkMesh), 
            pkMaterialInstance->GetSemanticAdapterTable());

    pkMaterialInstance->SetVertexDeclarationCache(
        (NiVertexDeclarationCache)spMMB);

    return (spMMB != NULL);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::SetupSCMExtraData(NiD3D10Shader* pkShader, 
    NiRenderObject* pkMesh)
{
    // Remove any previous instances of the extra data cache.
    pkMesh->RemoveExtraData(ms_kEmergentShaderMapName);

    // Determine the number of entries and allocate a new instance so that
    // we get a tightly packed array of entries.
    unsigned int uiNumVertexEntries = 0;
    unsigned int uiNumGeometryEntries = 0;
    unsigned int uiNumPixelEntries = 0;

    // Establish the number of entries in the global constant map times the
    // number of passes.

    // Vertex Shader Constants
    const unsigned int uiVSMapCount = 
        pkShader->m_kVertexShaderConstantMaps.GetSize();
    unsigned int i = 0;
    for (; i < uiVSMapCount; i++)
    {
        NiD3D10ShaderConstantMap* pkMap = 
            pkShader->m_kVertexShaderConstantMaps.GetAt(i);
        if (pkMap)
        {
            unsigned int uiEntryCount = pkMap->GetEntryCount();
            for (unsigned int j = 0; j < uiEntryCount; j++)
            {
                NiShaderConstantMapEntry* pkEntry = pkMap->GetEntryAtIndex(j);
                if (pkEntry && pkEntry->IsAttribute())
                    ++uiNumVertexEntries;
            }
        }
    }
    // If the passes array is not tightly packed, this could cause some
    // waste in the arrays, but that is not likely.
    uiNumVertexEntries *= pkShader->m_kPasses.GetSize();

    // Geometry Shader Constants
    const unsigned int uiGSMapCount = 
        pkShader->m_kGeometryShaderConstantMaps.GetSize();
    for (i = 0; i < uiGSMapCount; i++)
    {
        NiD3D10ShaderConstantMap* pkMap = 
            pkShader->m_kGeometryShaderConstantMaps.GetAt(i);
        if (pkMap)
        {
            unsigned int uiEntryCount = pkMap->GetEntryCount();
            for (unsigned int j = 0; j < uiEntryCount; j++)
            {
                NiShaderConstantMapEntry* pkEntry = pkMap->GetEntryAtIndex(j);
                if (pkEntry && pkEntry->IsAttribute())
                    ++uiNumGeometryEntries;
            }
        }
    }
    // If the passes array is not tightly packed, this could cause some
    // waste in the arrays, but that is not likely.
    uiNumGeometryEntries *= pkShader->m_kPasses.GetSize();

    // Pixel Shader Constants
    const unsigned int uiPSMapCount = 
        pkShader->m_kPixelShaderConstantMaps.GetSize();
    for (i = 0; i < uiPSMapCount; i++)
    {
        NiD3D10ShaderConstantMap* pkMap = 
            pkShader->m_kPixelShaderConstantMaps.GetAt(i);
        if (pkMap)
        {
            unsigned int uiEntryCount = pkMap->GetEntryCount();
            for (unsigned int j = 0; j < uiEntryCount; j++)
            {
                NiShaderConstantMapEntry* pkEntry = pkMap->GetEntryAtIndex(j);
                if (pkEntry && pkEntry->IsAttribute())
                    ++uiNumPixelEntries;
            }
        }
    }
    // If the passes array is not tightly packed, this could cause some
    // waste in the arrays, but that is not likely.
    uiNumPixelEntries *= pkShader->m_kPasses.GetSize();

    unsigned int uiPasses;
    unsigned int uiNumPasses = pkShader->m_kPasses.GetSize();
    for (uiPasses = 0; uiPasses < uiNumPasses; uiPasses++)
    {
        NiD3D10Pass* pkPass = pkShader->m_kPasses.GetAt(uiPasses);
        if (pkPass)
        {
            // Check each pass for constant maps and then increment the entry
            // count if that entry is an attribute type entry.
            const unsigned int uiVSPerPassMapCount = 
                pkPass->GetVertexConstantMapCount();
            for (i = 0; i < uiVSPerPassMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkPass->GetVertexConstantMap(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                            ++uiNumVertexEntries;
                    }
                }
            }

            const unsigned int uiGSPerPassMapCount = 
                pkPass->GetGeometryConstantMapCount();
            for (i = 0; i < uiGSPerPassMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkPass->GetGeometryConstantMap(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                            ++uiNumGeometryEntries;
                    }
                }
            }

            const unsigned int uiPSPerPassMapCount = 
                pkPass->GetPixelConstantMapCount();
            for (i = 0; i < uiPSPerPassMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkPass->GetPixelConstantMap(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                            ++uiNumPixelEntries;
                    }
                }
            }
        }
    }

    // Create the extra data cache table if necessary. We don't want to waste
    // time if there are no attribute type constant map entries.
    NiSCMExtraData* pkShaderExtraData = 0;
    if (uiNumVertexEntries > 0 || uiNumGeometryEntries > 0 ||
        uiNumPixelEntries > 0)
    {
        pkShaderExtraData = NiNew NiSCMExtraData(ms_kEmergentShaderMapName, 
            uiNumVertexEntries, uiNumGeometryEntries, uiNumPixelEntries);
    }
    else
    {
        return;
    }

    // Populate the extra data with pointers. Again, we only insert entries
    // into our cache when the entry in the constant map is of the attribute
    // type.
    for (uiPasses = 0; uiPasses < uiNumPasses; uiPasses++)
    {
        NiD3D10Pass* pkPass = pkShader->m_kPasses.GetAt(uiPasses);
        if (pkPass)
        {
            // Add global constant maps per pass so that we hit them
            // multiple times when iterating through the lists.
            for (i = 0; i < uiVSMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkShader->m_kVertexShaderConstantMaps.GetAt(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                        {
                            NiExtraData* pkExtra = 
                                pkMesh->GetExtraData(pkEntry->GetKey());
                            if (pkExtra)
                            {
                                pkShaderExtraData->AddEntry(
                                    pkEntry->GetShaderRegister(), 0, 
                                    NiGPUProgram::PROGRAM_VERTEX, pkExtra, 
                                    true);
                            }
                        }
                    }
                }
            }

            for (i = 0; i < uiGSMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkShader->m_kGeometryShaderConstantMaps.GetAt(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                        {
                            NiExtraData* pkExtra = 
                                pkMesh->GetExtraData(pkEntry->GetKey());
                            if (pkExtra)
                            {
                                pkShaderExtraData->AddEntry(
                                    pkEntry->GetShaderRegister(), 0, 
                                    NiGPUProgram::PROGRAM_GEOMETRY, pkExtra, 
                                    true);
                            }
                        }
                    }
                }
            }

            for (i = 0; i < uiPSMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkShader->m_kPixelShaderConstantMaps.GetAt(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                        {
                            NiExtraData* pkExtra = 
                                pkMesh->GetExtraData(pkEntry->GetKey());
                            if (pkExtra)
                            {
                                pkShaderExtraData->AddEntry(
                                    pkEntry->GetShaderRegister(), 0, 
                                    NiGPUProgram::PROGRAM_PIXEL, pkExtra, 
                                    true);
                            }
                        }
                    }
                }
            }

            // Add per pass shader constants.
            const unsigned int uiVSPerPassMapCount = 
                pkPass->GetVertexConstantMapCount();
            for (i = 0; i < uiVSPerPassMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkPass->GetVertexConstantMap(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                        {
                            NiExtraData* pkExtra = 
                                pkMesh->GetExtraData(pkEntry->GetKey());
                            if (pkExtra)
                            {
                                pkShaderExtraData->AddEntry(
                                    pkEntry->GetShaderRegister(),
                                    0, NiGPUProgram::PROGRAM_VERTEX,
                                    pkExtra, false);
                            }
                        }
                    }
                }
            }

            const unsigned int uiGSPerPassMapCount = 
                pkPass->GetGeometryConstantMapCount();
            for (i = 0; i < uiGSPerPassMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkPass->GetGeometryConstantMap(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                        {
                            NiExtraData* pkExtra = 
                                pkMesh->GetExtraData(pkEntry->GetKey());
                            if (pkExtra)
                            {
                                pkShaderExtraData->AddEntry(
                                    pkEntry->GetShaderRegister(),
                                    0, NiGPUProgram::PROGRAM_GEOMETRY,
                                    pkExtra, false);
                            }
                        }
                    }
                }
            }

            const unsigned int uiPSPerPassMapCount = 
                pkPass->GetPixelConstantMapCount();
            for (i = 0; i < uiPSPerPassMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkPass->GetPixelConstantMap(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                        {
                            NiExtraData* pkExtra = 
                                pkMesh->GetExtraData(pkEntry->GetKey());
                            if (pkExtra)
                            {
                                pkShaderExtraData->AddEntry(
                                    pkEntry->GetShaderRegister(),
                                    0, NiGPUProgram::PROGRAM_PIXEL,
                                    pkExtra, false);
                            }
                        }
                    }
                }
            }
        }
    }

    // Attach the NiSCMExtraData object which holds our cached values.
    pkMesh->AddExtraData(pkShaderExtraData);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::ResetSCMExtraData(NiRenderObject* pkMesh)
{
    if (pkMesh)
    {
        NiSCMExtraData* pkShaderData = 
            (NiSCMExtraData*)
            pkMesh->GetExtraData(NiD3D10Shader::ms_kEmergentShaderMapName);
        if (pkShaderData)
            pkShaderData->Reset();
    }
}
//---------------------------------------------------------------------------
void NiD3D10Shader::DestroyRendererData()
{
    const unsigned int uiPassCount = m_kPasses.GetSize();
    for (unsigned int i = 0; i < uiPassCount; i++)
    {
        NiD3D10Pass* pkPass = m_kPasses.GetAt(i);
        if (pkPass)
        {
            NiD3D10VertexShader* pkVS = pkPass->GetVertexShader();
            if (pkVS)
                pkVS->DestroyRendererData();

            NiD3D10GeometryShader* pkGS = pkPass->GetGeometryShader();
            if (pkGS)
                pkGS->DestroyRendererData();

            NiD3D10PixelShader* pkPS = pkPass->GetPixelShader();
            if (pkPS)
                pkPS->DestroyRendererData();
        }
    }
    m_bInitialized = false;
}
//---------------------------------------------------------------------------
void NiD3D10Shader::RecreateRendererData()
{
    if (NiD3D10Renderer::GetRenderer() == NULL)
    {
        NIASSERT(!"NiD3D10Shader::RecreateRendererData> Invalid renderer!");
        return;
    }

    const unsigned int uiPassCount = m_kPasses.GetSize();
    for (unsigned int i = 0; i < uiPassCount; i++)
    {
        NiD3D10Pass* pkPass = m_kPasses.GetAt(i);
        if (pkPass)
        {
            NiD3D10VertexShader* pkVS = pkPass->GetVertexShader();
            if (pkVS)
                pkVS->RecreateRendererData();

            NiD3D10GeometryShader* pkGS = pkPass->GetGeometryShader();
            if (pkGS)
                pkGS->RecreateRendererData();

            NiD3D10PixelShader* pkPS = pkPass->GetPixelShader();
            if (pkPS)
                pkPS->RecreateRendererData();
        }
    }
    m_bInitialized = true;
}
//---------------------------------------------------------------------------
const NiFixedString& NiD3D10Shader::GetEmergentShaderMapName()
{
    return ms_kEmergentShaderMapName;
}
//---------------------------------------------------------------------------
bool NiD3D10Shader::ObtainTexture(
    NiUInt32 uiGBMapFlags, 
    NiUInt16 uiObjectTextureFlags,
    const NiRenderCallContext& kRCC,
    NiTexture*& pkTexture, 
    NiTexturingProperty::ClampMode& eClampMode, 
    NiTexturingProperty::FilterMode& eFilterMode,
    unsigned short& usMaxAnisotropy)
{
    NIASSERT(kRCC.m_pkState != NULL);
    const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();
    const NiDynamicEffectState* pkEffects = kRCC.m_pkEffects;
    NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);

    if (uiObjectTextureFlags == NiD3D10Pass::OTF_IGNORE)
    {
        if (uiGBMapFlags != NiD3D10Pass::GB_MAP_IGNORE)
        {
            const NiTexturingProperty::Map* pkMap = NULL;
            NIASSERT(pkTexProp);
            if ((uiGBMapFlags & NiD3D10Pass::GB_MAP_TYPEMASK) != 0)
            {
                switch (uiGBMapFlags & NiD3D10Pass::GB_MAP_TYPEMASK)
                {
                case NiD3D10Pass::GB_MAP_NONE:
                    break;
                case NiD3D10Pass::GB_MAP_BASE:
                    pkMap = pkTexProp->GetBaseMap();
                    break;
                case NiD3D10Pass::GB_MAP_DARK:
                    pkMap = pkTexProp->GetDarkMap();
                    break;
                case NiD3D10Pass::GB_MAP_DETAIL:
                    pkMap = pkTexProp->GetDetailMap();
                    break;
                case NiD3D10Pass::GB_MAP_GLOSS:
                    pkMap = pkTexProp->GetGlossMap();
                    break;
                case NiD3D10Pass::GB_MAP_GLOW:
                    pkMap = pkTexProp->GetGlowMap();
                    break;
                case NiD3D10Pass::GB_MAP_BUMP:
                    pkMap = pkTexProp->GetBumpMap();
                    break;
                case NiD3D10Pass::GB_MAP_NORMAL:
                    pkMap = pkTexProp->GetNormalMap();
                    break;
                case NiD3D10Pass::GB_MAP_PARALLAX:
                    pkMap = pkTexProp->GetParallaxMap();
                    break;
                case NiD3D10Pass::GB_MAP_DECAL:
                    {
                        unsigned int uiIndex = uiGBMapFlags &
                            NiD3D10Pass::GB_MAP_INDEX_MASK;
                        if (uiIndex < pkTexProp->GetDecalArrayCount())
                            pkMap = pkTexProp->GetDecalMap(uiIndex);
                        break;
                    }
                case NiD3D10Pass::GB_MAP_SHADER:
                    {
                        unsigned int uiIndex = uiGBMapFlags &
                            NiD3D10Pass::GB_MAP_INDEX_MASK;
                        if (uiIndex < pkTexProp->GetShaderArrayCount())
                            pkMap = pkTexProp->GetShaderMap(uiIndex);
                        break;
                    }
                }
            }

            if (pkMap)
            {
                pkTexture = pkMap->GetTexture();
                eClampMode = pkMap->GetClampMode();
                eFilterMode = pkMap->GetFilterMode();
                usMaxAnisotropy = pkMap->GetMaxAnisotropy();

                // Note that the texture coordinate index is ignored, since
                // that value must be specified in the pixel shader.
            }
        }
    }
    else if (pkEffects)
    {
        NiTextureEffect* pkTextureEffect = NULL;
        unsigned short usObjectIndex = static_cast<unsigned short>(
            uiObjectTextureFlags & NiD3D10Pass::OTF_INDEX_MASK);
        switch ((uiObjectTextureFlags & NiD3D10Pass::OTF_TYPE_MASK) >>
            NiD3D10Pass::OTF_TYPE_SHIFT)
        {
        case NiShaderAttributeDesc::OT_EFFECT_DIRSHADOWMAP:
        {
            NiLight* pkLight = (NiLight*)
                NiShaderConstantMap::GetDynamicEffectForObject(
                pkEffects, 
                NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT, 
                usObjectIndex);

            NIASSERT(pkLight);

            NiShadowGenerator* pkGenerator = pkLight->GetShadowGenerator();
            NIASSERT(pkGenerator);

            NiShadowMap* pkShadowMap = 
                pkGenerator->RetrieveShadowMap(
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, pkMesh);
            NIASSERT(pkShadowMap);

            pkTexture = pkShadowMap->GetTexture();
            eClampMode = pkShadowMap->GetClampMode();
            eFilterMode = pkShadowMap->GetFilterMode();
            usMaxAnisotropy = pkShadowMap->GetMaxAnisotropy();
            break;
        }
        case NiShaderAttributeDesc::OT_EFFECT_POINTSHADOWMAP:
        {
            NiLight* pkLight = (NiLight*)
                NiShaderConstantMap::GetDynamicEffectForObject(
                pkEffects, NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT, 
                usObjectIndex);
            NIASSERT(pkLight);

            NiShadowGenerator* pkGenerator = pkLight->GetShadowGenerator();
            NIASSERT(pkGenerator);

            NiShadowMap* pkShadowMap = 
                pkGenerator->RetrieveShadowMap(
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, pkMesh);
            NIASSERT(pkShadowMap);

            pkTexture = pkShadowMap->GetTexture();
            eClampMode = pkShadowMap->GetClampMode();
            eFilterMode = pkShadowMap->GetFilterMode();
            usMaxAnisotropy = pkShadowMap->GetMaxAnisotropy();
            break;
        }
        case NiShaderAttributeDesc::OT_EFFECT_SPOTSHADOWMAP:
        {
            NiLight* pkLight = (NiLight*)
                NiShaderConstantMap::GetDynamicEffectForObject(
                pkEffects, NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT, 
                usObjectIndex);
            NIASSERT(pkLight);

            NiShadowGenerator* pkGenerator = pkLight->GetShadowGenerator();
            NIASSERT(pkGenerator);

            NiShadowMap* pkShadowMap = 
                pkGenerator->RetrieveShadowMap(
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, pkMesh);
            NIASSERT(pkShadowMap);

            pkTexture = pkShadowMap->GetTexture();
            eClampMode = pkShadowMap->GetClampMode();
            eFilterMode = pkShadowMap->GetFilterMode();
            usMaxAnisotropy = pkShadowMap->GetMaxAnisotropy();
            break;
        }
        case NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP:
            pkTextureEffect = pkEffects->GetEnvironmentMap();
            break;
        case NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP:
        {
            unsigned short usIndex = 0;
            NiDynEffectStateIter kIter =
                pkEffects->GetProjShadowHeadPos();
            while (kIter)
            {
                NiTextureEffect* pkProjShadow =
                    pkEffects->GetNextProjShadow(kIter);
                if (pkProjShadow && pkProjShadow->GetSwitch() == true)
                {
                    if (usIndex++ == usObjectIndex)
                    {
                        pkTextureEffect = pkProjShadow;
                        break;
                    }
                }
            }
            break;
        }
        case NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP:
        {
            unsigned short usIndex = 0;
            NiDynEffectStateIter kIter =
                pkEffects->GetProjLightHeadPos();
            while (kIter)
            {
                NiTextureEffect* pkProjLight =
                    pkEffects->GetNextProjLight(kIter);
                if (pkProjLight && pkProjLight->GetSwitch() == true)
                {
                    if (usIndex++ == usObjectIndex)
                    {
                        pkTextureEffect = pkProjLight;
                        break;
                    }
                }
            }
            break;
        }
        case NiShaderAttributeDesc::OT_EFFECT_FOGMAP:
            pkTextureEffect = pkEffects->GetFogMap();
            break;
        default:
            // This assertion is hit when the object type is not one
            // that has a texture.
            NIASSERT(false);
            break;
        }

        if (pkTextureEffect && pkTextureEffect->GetSwitch())
        {
            pkTexture = pkTextureEffect->GetEffectTexture();
            eClampMode = pkTextureEffect->GetTextureClamp();
            eFilterMode = pkTextureEffect->GetTextureFilter();
            usMaxAnisotropy = pkTextureEffect->GetMaxAnisotropy();

        }
    }
    return (pkTexture != NULL);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackDynamicEffect(
    const NiTextureEffect* pkTextureEffect, NiD3D10Pass* pkPass, 
    unsigned int uiSampler)
{
    ms_apfnDynEffectPackers[(unsigned int)
        pkTextureEffect->GetTextureCoordGen()](
        pkTextureEffect->GetWorldProjectionMatrix(),
        pkTextureEffect->GetWorldProjectionTranslation(), pkPass, uiSampler);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::UseWorldSpaceSphereMaps(bool bWorldSpace)
{
    ms_apfnDynEffectPackers[NiTextureEffect::SPHERE_MAP] =
        (bWorldSpace ? &PackWorldSphereEffect : &PackCameraSphereEffect);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackWorldParallelEffect(const NiMatrix3&,
    const NiPoint3&, NiD3D10Pass*, unsigned int)
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackWorldPerspectiveEffect(const NiMatrix3&,
    const NiPoint3&, NiD3D10Pass*, unsigned int)
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackWorldSphereEffect(const NiMatrix3&,
    const NiPoint3&, NiD3D10Pass*, unsigned int)
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackCameraSphereEffect(const NiMatrix3&,
    const NiPoint3&, NiD3D10Pass*, unsigned int)
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackSpecularCubeEffect(const NiMatrix3&,
    const NiPoint3&, NiD3D10Pass*, unsigned int)
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackDiffuseCubeEffect(const NiMatrix3&,
    const NiPoint3&, NiD3D10Pass*, unsigned int)
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
