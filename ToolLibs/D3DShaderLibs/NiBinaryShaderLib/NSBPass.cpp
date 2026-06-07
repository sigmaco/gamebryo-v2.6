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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiBinaryShaderLibPCH.h"

#include "NSBPass.h"
#include "NSBStateGroup.h"
#include "NSBTexture.h"
#include "NSBTextureStage.h"
#include "NSBUtility.h"
#include "NSBShader.h"

#include <NiD3DPass.h>
#include <NiD3DRendererHeaders.h>

//---------------------------------------------------------------------------
NSBPass::NSBPass() :
    m_pcName(0),
    m_pkRenderStateGroup(0),
    m_bSoftwareVP(false)
{
    m_kVertexShaderConstantMaps.RemoveAll();
    m_kGeometryShaderConstantMaps.RemoveAll();
    m_kPixelShaderConstantMaps.RemoveAll();

    m_akStages.RemoveAll();
    m_akTextures.RemoveAll();
}
//---------------------------------------------------------------------------
NSBPass::~NSBPass()
{
    NiFree(m_pcName);
    NiDelete m_pkRenderStateGroup;

    // Clean up the shader constant maps.

    unsigned int uiSize = m_kVertexShaderConstantMaps.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBConstantMap* pkCMap = m_kVertexShaderConstantMaps.GetAt(i);
        if (pkCMap)
        {
            NiDelete pkCMap;
            m_kVertexShaderConstantMaps.ReplaceAt(i, NULL);
        }
    }
    m_kVertexShaderConstantMaps.RemoveAll();

    uiSize = m_kGeometryShaderConstantMaps.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBConstantMap* pkCMap = m_kGeometryShaderConstantMaps.GetAt(i);
        if (pkCMap)
        {
            NiDelete pkCMap;
            m_kGeometryShaderConstantMaps.ReplaceAt(i, NULL);
        }
    }
    m_kGeometryShaderConstantMaps.RemoveAll();

    uiSize = m_kPixelShaderConstantMaps.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBConstantMap* pkCMap = m_kPixelShaderConstantMaps.GetAt(i);
        if (pkCMap)
        {
            NiDelete pkCMap;
            m_kPixelShaderConstantMaps.ReplaceAt(i, NULL);
        }
    }
    m_kPixelShaderConstantMaps.RemoveAll();

    // Clean up the stages.
    uiSize = m_akStages.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBTextureStage* pkStage = m_akStages.GetAt(i);
        if (pkStage)
        {
            NiDelete pkStage;
            m_akStages.SetAt(i, 0);
        }
    }

    m_akStages.RemoveAll();

    // Clean up the textures.
    uiSize = m_akTextures.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBTexture* pkTexture = m_akTextures.GetAt(i);
        if (pkTexture)
        {
            NiDelete pkTexture;
            m_akTextures.SetAt(i, 0);
        }
    }

    m_akTextures.RemoveAll();
}
//---------------------------------------------------------------------------
void NSBPass::SetName(const char* pcName)
{
    NiRendererUtility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
NSBStateGroup* NSBPass::GetRenderStateGroup()
{
    if (m_pkRenderStateGroup == 0)
        m_pkRenderStateGroup = NiNew NSBStateGroup();

    return m_pkRenderStateGroup;
}
//---------------------------------------------------------------------------
bool NSBPass::GetShaderPresent(NiGPUProgram::ProgramType eType) const
{
    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        if (m_akInfo[i][eType].m_pcProgramFile != NULL)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NSBConstantMap* NSBPass::GetVertexConstantMap(unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kVertexShaderConstantMaps.GetSize());

    if (uiIndex == m_kVertexShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkVertexConstantMap = NiNew NSBConstantMap();
        NIASSERT(pkVertexConstantMap);
        pkVertexConstantMap->SetProgramType(NiGPUProgram::PROGRAM_VERTEX);
        m_kVertexShaderConstantMaps.Add(pkVertexConstantMap);
    }
    else if (uiIndex < m_kVertexShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkVertexConstantMap
            = m_kVertexShaderConstantMaps.GetAt(uiIndex);
        if (pkVertexConstantMap == NULL)
        {
            pkVertexConstantMap = NiNew NSBConstantMap();
            NIASSERT(pkVertexConstantMap);
            pkVertexConstantMap->SetProgramType(NiGPUProgram::PROGRAM_VERTEX);
            m_kVertexShaderConstantMaps.ReplaceAt(uiIndex,
                pkVertexConstantMap);
        }
    }

    return m_kVertexShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NSBConstantMap* NSBPass::GetGeometryConstantMap(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kGeometryShaderConstantMaps.GetSize());

    if (uiIndex == m_kGeometryShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkGeometryConstantMap = NiNew NSBConstantMap();
        NIASSERT(pkGeometryConstantMap);
        pkGeometryConstantMap->SetProgramType(NiGPUProgram::PROGRAM_GEOMETRY);
        m_kGeometryShaderConstantMaps.Add(pkGeometryConstantMap);
    }
    else if (uiIndex < m_kGeometryShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkGeometryConstantMap
            = m_kGeometryShaderConstantMaps.GetAt(uiIndex);
        if (pkGeometryConstantMap == NULL)
        {
            pkGeometryConstantMap = NiNew NSBConstantMap();
            NIASSERT(pkGeometryConstantMap);
            pkGeometryConstantMap->SetProgramType(
                NiGPUProgram::PROGRAM_GEOMETRY);
            m_kGeometryShaderConstantMaps.ReplaceAt(uiIndex,
                pkGeometryConstantMap);
        }
    }

    return m_kGeometryShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NSBConstantMap* NSBPass::GetPixelConstantMap(unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kPixelShaderConstantMaps.GetSize());

    if (uiIndex == m_kPixelShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkPixelConstantMap = NiNew NSBConstantMap();
        NIASSERT(pkPixelConstantMap);
        pkPixelConstantMap->SetProgramType(NiGPUProgram::PROGRAM_PIXEL);
        m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);
    }
    else if (uiIndex < m_kPixelShaderConstantMaps.GetSize())
    {
        NSBConstantMap* pkPixelConstantMap
            = m_kPixelShaderConstantMaps.GetAt(uiIndex);
        if (pkPixelConstantMap == NULL)
        {
            pkPixelConstantMap = NiNew NSBConstantMap();
            NIASSERT(pkPixelConstantMap);
            pkPixelConstantMap->SetProgramType(NiGPUProgram::PROGRAM_PIXEL);
            m_kPixelShaderConstantMaps.ReplaceAt(uiIndex, pkPixelConstantMap);
        }
    }

    return m_kPixelShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
unsigned int NSBPass::GetStageCount()
{
    return m_akStages.GetSize();
}
//---------------------------------------------------------------------------
NSBTextureStage* NSBPass::GetStage(unsigned int uiIndex, bool bCreate)
{
    NSBTextureStage* pkStage = 0;

    if (m_akStages.GetSize() > uiIndex)
        pkStage = m_akStages.GetAt(uiIndex);
    if (!pkStage)
    {
        if (bCreate)
        {
            // Create all stages up to NiNew one, to ensure
            // stage array is packed
            m_akStages.SetSize(uiIndex + 1);
            for (unsigned int i = m_akStages.GetSize(); i <= uiIndex; i++)
            {
                pkStage = NiNew NSBTextureStage();
                NIASSERT(pkStage);

                pkStage->SetStage(i);

                m_akStages.SetAt(i, pkStage);
            }
        }
    }

    return pkStage;
}
//---------------------------------------------------------------------------
unsigned int NSBPass::GetTextureCount()
{
    return m_akTextures.GetSize();
}
//---------------------------------------------------------------------------
NSBTexture* NSBPass::GetTexture(unsigned int uiIndex, bool bCreate)
{
    NSBTexture* pkTexture = 0;

    if (m_akTextures.GetSize() > uiIndex)
        pkTexture = m_akTextures.GetAt(uiIndex);
    if (!pkTexture)
    {
        if (bCreate)
        {
            // Create all stages up to new one, to ensure
            // stage array is packed
            m_akTextures.SetSize(m_akTextures.GetSize() + 1);
            for (unsigned int i = m_akTextures.GetSize(); i <= uiIndex; i++)
            {
                pkTexture = NiNew NSBTexture();
                NIASSERT(pkTexture);

                pkTexture->SetStage(i);

                m_akTextures.SetAt(i, pkTexture);
            }
        }
    }

    return pkTexture;
}
//---------------------------------------------------------------------------
bool NSBPass::SetupShaderPass(NiPlatformShaderPass& kPass,
    NiShaderDesc* pkShaderDesc)
{
    // Convert the NSB representation to a run-time NiPlatformShaderPass

    // RenderStateGroup
    NiD3DRenderStateGroup* pkRSGroup = 0;
    if (m_pkRenderStateGroup)
    {
        if (m_pkRenderStateGroup->GetStateCount())
        {
            pkRSGroup = NiD3DRenderStateGroup::GetFreeRenderStateGroup();
            NIASSERT(pkRSGroup);

            if (!m_pkRenderStateGroup->SetupRenderStateGroup(*pkRSGroup))
            {
                NiDelete pkRSGroup;
                return false;
            }
        }
    }
    kPass.SetRenderStateGroup(pkRSGroup);

    // Setup the 'local' constant mappings

    // Vertex
    if (m_kVertexShaderConstantMaps.GetSize() > 0)
    {
        NSBConstantMap* pkCM = m_kVertexShaderConstantMaps.GetAt(0);
        if (pkCM)
        {
            kPass.SetVertexConstantMap(pkCM->GetVertexConstantMap(
                pkShaderDesc));
        }
        else
        {
            kPass.SetVertexConstantMap(NULL);
        }
    }
    else
    {
        kPass.SetVertexConstantMap(NULL);
    }

    // Pixel
    if (m_kPixelShaderConstantMaps.GetSize() > 0)
    {
        NSBConstantMap* pkCM = m_kPixelShaderConstantMaps.GetAt(0);
        if (pkCM)
        {
            kPass.SetPixelConstantMap(pkCM->GetPixelConstantMap(
                pkShaderDesc));
        }
        else
        {
            kPass.SetPixelConstantMap(NULL);
        }
    }
    else
    {
        kPass.SetPixelConstantMap(NULL);
    }

    // Shader Programs
#if defined(_XENON) || defined(WIN32)
    #if defined(_XENON)
        NiSystemDesc::RendererID eRenderer = NiSystemDesc::RENDERER_XENON;
    #elif defined(WIN32)
        NiSystemDesc::RendererID eRenderer = NiSystemDesc::RENDERER_DX9;
    #endif

    kPass.SetVertexShaderProgramFileName(GetShaderProgramFile(
        eRenderer, NiGPUProgram::PROGRAM_VERTEX));
    kPass.SetPixelShaderProgramFileName(GetShaderProgramFile(
        eRenderer, NiGPUProgram::PROGRAM_PIXEL));

    kPass.SetVertexShaderProgramEntryPoint(GetShaderProgramEntryPoint(
        eRenderer, NiGPUProgram::PROGRAM_VERTEX));
    kPass.SetVertexShaderProgramShaderTarget(GetShaderProgramShaderTarget(
        eRenderer, NiGPUProgram::PROGRAM_VERTEX));
    kPass.SetPixelShaderProgramEntryPoint(GetShaderProgramEntryPoint(
        eRenderer, NiGPUProgram::PROGRAM_PIXEL));
    kPass.SetPixelShaderProgramShaderTarget(GetShaderProgramShaderTarget(
        eRenderer, NiGPUProgram::PROGRAM_PIXEL));
#elif defined (_PS3)
    // Use the shader program factory which has a cache
    char acFilePath[NI_MAX_PATH];

    const char* pcVertexProgram = GetShaderProgramFile(
        NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_VERTEX);
    const char* pcVertexEntry = GetShaderProgramEntryPoint(
        NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_VERTEX);

    // Convert the source shader name into a binary name for the VP
    NIVERIFY(NiPS3CgShaderProgram::GetCompiledShaderFilename(
        pcVertexProgram, acFilePath, NI_MAX_PATH,
        NiGPUProgram::PROGRAM_VERTEX, pcVertexEntry));

    // The factory will either return the cached binary or load it from disk
    NiPS3CgShaderProgram* pkVP =
        NiPS3ShaderProgramFactory::CreateShaderProgramFromFile(
        NiGPUProgram::PROGRAM_VERTEX, pcVertexProgram,
        acFilePath, pcVertexEntry);
    if (!pkVP)
        return false;

    const char* pcPixelProgram = GetShaderProgramFile(
        NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_PIXEL);
    const char* pcPixelEntry = GetShaderProgramEntryPoint(
        NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_PIXEL);

    // Convert the source shader name into a binary name for the FP
    NIVERIFY(NiPS3CgShaderProgram::GetCompiledShaderFilename(
        pcPixelProgram, acFilePath, NI_MAX_PATH,
        NiGPUProgram::PROGRAM_PIXEL, pcPixelEntry));

    // The factory will either return the cached binary or load it from disk
    NiPS3CgShaderProgram* pkFP =
        NiPS3ShaderProgramFactory::CreateShaderProgramFromFile(
        NiGPUProgram::PROGRAM_PIXEL, pcPixelProgram,
        acFilePath, pcPixelEntry);
    if (!pkFP)
        return false;

    // Look for UserData that controls special per-shader states
    NSBUserDefinedDataSet* pkSet = GetUserDefinedDataSet();
    if (pkSet)
    {
        NSBUserDefinedDataBlock* pkUDBlock =
            pkSet->GetBlock("FragmentShaderSettings", false);
        if (pkUDBlock)
        {
            NSBConstantMap::NSBCM_Entry* pkEntry =
                pkUDBlock->GetEntryByKey("RegisterCount");
            if (pkEntry && pkEntry->IsUnsignedInt())
            {
                unsigned int* puiRegisterCount =
                    (unsigned int*)pkEntry->GetDataSource();

                pkFP->SetFragmentProgramRegisterCount(*puiRegisterCount);
            }
        }
    }

    // Add the shaders to the pass
    kPass.SetVertexShader(pkVP);
    kPass.SetPixelShader(pkFP);
#else
    #error "Unsupported platform"
#endif

    NSBTextureStage* pkStage;
    unsigned int uiStageCount = m_akStages.GetSize();
    if (uiStageCount > NiPlatformShaderPass::ms_uiMaxSamplers)
        return false;

    for (unsigned int ui = 0; ui < uiStageCount; ui++)
    {
        pkStage = m_akStages.GetAt(ui);
        if (pkStage)
        {
            NiPointer<NiPlatformTextureSampler> spPlatformStage =
                kPass.GetStage(ui);
            if (!spPlatformStage)
            {
                spPlatformStage = NiNew NiPlatformTextureSampler();
                NIASSERT(spPlatformStage);
            }

            if (!pkStage->SetupTextureStage(*spPlatformStage))
            {
                return false;
            }

            kPass.SetStage(ui, spPlatformStage);
        }
    }

    // Textures should only modify existing stages
    NSBTexture* pkTexture;
    unsigned int uiTextureCount = m_akTextures.GetSize();
    if (uiTextureCount > uiStageCount)
        uiTextureCount = uiStageCount;

    for (unsigned int ui = 0; ui < uiTextureCount; ui++)
    {
        pkTexture = m_akTextures.GetAt(ui);

        if (pkTexture)
        {
            NiPointer<NiPlatformTextureSampler> spPlatformStage =
                kPass.GetStage(ui);
            if (!spPlatformStage)
            {
                continue;
            }

            if (!pkTexture->SetupTextureStage(*spPlatformStage))
            {
                return false;
            }
        }
    }

#if defined(_PS3)
    for (unsigned int ui = 0; ui < uiStageCount; ui++)
    {
        pkStage = m_akStages.GetAt(ui);
        if (pkStage)
        {
            // First, try to match up the sampler by name
            CGparameter pkSamplerParam = 0;

            if (pkStage->GetName())
            {
                 pkSamplerParam = cellGcmCgGetNamedParameter(
                    pkFP->GetShaderProgramHandle(), pkStage->GetName());
            }

            if (pkSamplerParam == 0)
            {
                // Match up sampler via resource index, not by name.
                // NSF sampler names currently ignored on other platforms.

                // Iterate though all samplers in the fragment program
                unsigned int uiNumFPSamplers = pkFP->GetSamplerCount();
                for (unsigned int uiFPSampler = 0;
                    uiFPSampler < uiNumFPSamplers;
                    uiFPSampler++)
                {
                    // Match the resource index
                    if (pkFP->GetSamplerParamResourceIndex(uiFPSampler) == ui)
                    {
                        pkSamplerParam = pkFP->GetSamplerParam(uiFPSampler);
                        break;
                    }
                }
            }

            // Record the sampler parameter hookup
            if (pkSamplerParam != 0)
            {
                unsigned int uiSamplerID = cellGcmCgGetParameterResource(
                    pkFP->GetShaderProgramHandle(),
                    pkSamplerParam) - CG_TEXUNIT0;

                NiPointer<NiPlatformTextureSampler> spPlatformStage =
                    kPass.GetSampler(ui);
                spPlatformStage->SetSamplerCGParam(pkSamplerParam,
                    uiSamplerID);
            }
            else
            {
                NILOG("Could not bind NSF sampler index %d to shader\n", ui);
            }
        }
    }
#endif

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::SaveBinary(NiBinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);

    // Version 1.6 added support for user-defined data...
    unsigned int uiUDDSetPresent = 0;
    if (m_spUserDefinedDataSet)
    {
        uiUDDSetPresent = 1;
        NiStreamSaveBinary(kStream, uiUDDSetPresent);
        if (!m_spUserDefinedDataSet->SaveBinary(kStream))
            return false;
    }
    else
    {
        NiStreamSaveBinary(kStream, uiUDDSetPresent);
    }

    if (!NSBUtility::SaveBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;

    if (!SaveBinaryShaderProgram(kStream, NiGPUProgram::PROGRAM_PIXEL))
        return false;
    unsigned int uiConstantMapCount = m_kPixelShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBUtility::SaveBinaryConstantMap(kStream,
            m_kPixelShaderConstantMaps.GetAt(i)))
        {
            return false;
        }
    }

    if (!SaveBinaryShaderProgram(kStream, NiGPUProgram::PROGRAM_VERTEX))
        return false;
    uiConstantMapCount = m_kVertexShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBUtility::SaveBinaryConstantMap(kStream,
            m_kVertexShaderConstantMaps.GetAt(i)))
        {
            return false;
        }
    }

    if (!SaveBinaryShaderProgram(kStream, NiGPUProgram::PROGRAM_GEOMETRY))
        return false;
    uiConstantMapCount = m_kGeometryShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBUtility::SaveBinaryConstantMap(kStream,
            m_kGeometryShaderConstantMaps.GetAt(i)))
        {
            return false;
        }
    }

    unsigned int uiValue = m_bSoftwareVP ? 1 : 0;
    NiStreamSaveBinary(kStream, uiValue);

    if (!SaveBinaryStages(kStream))
        return false;

    if (!SaveBinaryTextures(kStream))
        return false;

    if (!m_kStreamOutSettings.SaveBinary(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinary(NiBinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();

    // Version 1.6 added support for user-defined data...
    if (NSBShader::GetReadVersion() >= 0x00010006)
    {
        unsigned int uiUDDSetPresent;
        NiStreamLoadBinary(kStream, uiUDDSetPresent);
        if (uiUDDSetPresent)
        {
            m_spUserDefinedDataSet = NiNew NSBUserDefinedDataSet();
            NIASSERT(m_spUserDefinedDataSet);
            if (!m_spUserDefinedDataSet->LoadBinary(kStream))
                return false;
        }
        else
        {
            m_spUserDefinedDataSet = 0;
        }
    }

    NiDelete m_pkRenderStateGroup;
    m_pkRenderStateGroup = 0;
    if (!NSBUtility::LoadBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;


    if (NSBShader::GetReadVersion() < 0x00010013)
    {
        // Before 1.13, there was exactly one constant map for vertex and
        // pixel shaders.
        if (!LoadBinaryPixelShaderProgram(kStream))
            return false;
        m_kPixelShaderConstantMaps.RemoveAll();
        NSBConstantMap* pkPixelConstantMap = NULL;
        if (!NSBUtility::LoadBinaryConstantMap(kStream,
            pkPixelConstantMap))
        {
            return false;
        }
        m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);

        if (!LoadBinaryVertexShaderProgram(kStream))
            return false;
        m_kVertexShaderConstantMaps.RemoveAll();
        NSBConstantMap* pkVertexConstantMap = NULL;
        if (!NSBUtility::LoadBinaryConstantMap(kStream,
            pkVertexConstantMap))
        {
            return false;
        }
        m_kVertexShaderConstantMaps.Add(pkVertexConstantMap);
    }
    else
    {
        // Version 1.13 added support for multiple constant maps, D3D10, and
        // geometry shaders.

        if (!LoadBinaryPixelShaderProgram(kStream))
            return false;
        m_kPixelShaderConstantMaps.RemoveAll();
        unsigned int uiConstantMapCount = 0;
        NiStreamLoadBinary(kStream, uiConstantMapCount);
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBConstantMap* pkPixelConstantMap = NULL;
            if (!NSBUtility::LoadBinaryConstantMap(kStream,
                pkPixelConstantMap))
            {
                return false;
            }
            m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);
        }

        if (!LoadBinaryVertexShaderProgram(kStream))
            return false;
        m_kVertexShaderConstantMaps.RemoveAll();
        uiConstantMapCount = 0;
        NiStreamLoadBinary(kStream, uiConstantMapCount);
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBConstantMap* pkVertexConstantMap = NULL;
            if (!NSBUtility::LoadBinaryConstantMap(kStream,
                pkVertexConstantMap))
            {
                return false;
            }
            m_kVertexShaderConstantMaps.Add(pkVertexConstantMap);
        }

        if (!LoadBinaryGeometryShaderProgram(kStream))
            return false;
        m_kGeometryShaderConstantMaps.RemoveAll();
        uiConstantMapCount = 0;
        NiStreamLoadBinary(kStream, uiConstantMapCount);
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBConstantMap* pkGeometryConstantMap = NULL;
            if (!NSBUtility::LoadBinaryConstantMap(kStream,
                pkGeometryConstantMap))
            {
                return false;
            }
            m_kGeometryShaderConstantMaps.Add(pkGeometryConstantMap);
        }
    }

    unsigned int uiSoftwareVP = 0;
    NiStreamLoadBinary(kStream, uiSoftwareVP);
    m_bSoftwareVP = (uiSoftwareVP != 0);

    if (!LoadBinaryStages(kStream))
        return false;

    if (NSBShader::GetReadVersion() >= 0x00010014)
    {
        // Version 1.14 added separate NSBTexture objects.

        if (!LoadBinaryTextures(kStream))
            return false;
    }

    // Version 2.2 added support for stream output
    if (NSBShader::GetReadVersion() >= 0x00020002)
    {
        m_kStreamOutSettings.LoadBinary(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinaryShaderProgram(NiBinaryStream& kStream, 
    NiGPUProgram::ProgramType eType)
{
    const NiSystemDesc::RendererID akStreamedRenderers[] = 
    {
        NiSystemDesc::RENDERER_DX9,
        NiSystemDesc::RENDERER_XENON,
        NiSystemDesc::RENDERER_PS3,
        NiSystemDesc::RENDERER_D3D10
    };
    const unsigned int uiNumStreamed = sizeof(akStreamedRenderers) / 
        sizeof(NiSystemDesc::RendererID);

    for (unsigned int i = 0; i < uiNumStreamed; i++)
    {
        NiSystemDesc::RendererID eRenderer = akStreamedRenderers[i];

        char* pcTemp = kStream.ReadCString();
        SetShaderProgramFile(pcTemp, eRenderer, eType);
        NiFree(pcTemp);
        pcTemp = kStream.ReadCString();
        SetShaderProgramEntryPoint(pcTemp, eRenderer, eType);
        NiFree(pcTemp);
        pcTemp = kStream.ReadCString();
        SetShaderProgramShaderTarget(pcTemp, eRenderer, eType);
        NiFree(pcTemp);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::SaveBinaryShaderProgram(NiBinaryStream& kStream, 
    NiGPUProgram::ProgramType eType)
{
    const NiSystemDesc::RendererID akStreamedRenderers[] = 
    {
        NiSystemDesc::RENDERER_DX9,
        NiSystemDesc::RENDERER_XENON,
        NiSystemDesc::RENDERER_PS3,
        NiSystemDesc::RENDERER_D3D10
    };
    const unsigned int uiNumStreamed = sizeof(akStreamedRenderers) / 
        sizeof(NiSystemDesc::RendererID);

    for (unsigned int i = 0; i < uiNumStreamed; i++)
    {
        NiSystemDesc::RendererID eRenderer = akStreamedRenderers[i];

        kStream.WriteCString(GetShaderProgramFile(eRenderer, eType));
        kStream.WriteCString(GetShaderProgramEntryPoint(eRenderer, eType));
        kStream.WriteCString(GetShaderProgramShaderTarget(eRenderer, eType));
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::SaveBinaryStages(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_akStages.GetEffectiveSize();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBTextureStage* pkStage;
    for (unsigned int ui = 0; ui < m_akStages.GetSize(); ui++)
    {
        pkStage = m_akStages.GetAt(ui);
        if (pkStage)
        {
            if (!pkStage->SaveBinary(kStream))
                return false;
            uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::SaveBinaryTextures(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_akTextures.GetEffectiveSize();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBTexture* pkTexture;
    for (unsigned int ui = 0; ui < m_akTextures.GetSize(); ui++)
    {
        pkTexture = m_akTextures.GetAt(ui);
        if (pkTexture)
        {
            if (!pkTexture->SaveBinary(kStream))
                return false;
            uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinaryVertexShaderProgram(NiBinaryStream& kStream)
{
    if (NSBShader::GetReadVersion() >= 0x00020001)
        return LoadBinaryShaderProgram(kStream, NiGPUProgram::PROGRAM_VERTEX);

    if (NSBShader::GetReadVersion() == 0x00010001)
    {
        // Version 1.1 only had a single shader file name.  Load first into
        // PS3 name (arbitrary choice - doesn't matter) and then copy into
        // other platform's names.
        char* pcProgramFile = kStream.ReadCString();
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_PS3, 
            NiGPUProgram::PROGRAM_VERTEX);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_DX9, 
            NiGPUProgram::PROGRAM_VERTEX);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_XENON, 
            NiGPUProgram::PROGRAM_VERTEX);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_D3D10, 
            NiGPUProgram::PROGRAM_VERTEX);
        NiFree(pcProgramFile);
    }
    else
    {
        if (NSBShader::GetReadVersion() < 0x00010008)
        {
            char* pcTemp = 0;

            // Removed Xbox support in version 1.8
            pcTemp = kStream.ReadCString();
            NiFree(pcTemp);

            // Removed DX8 support in version 1.8
            pcTemp = kStream.ReadCString();
            NiFree(pcTemp);
        }

        char* pcTemp = kStream.ReadCString();
        SetShaderProgramFile(pcTemp,
            NiSystemDesc::RENDERER_DX9, NiGPUProgram::PROGRAM_VERTEX);
        NiFree(pcTemp);
        if (NSBShader::GetReadVersion() >= 0x00010008)
        {
            // Added Xenon support in version 1.8
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp, 
               NiSystemDesc::RENDERER_XENON, NiGPUProgram::PROGRAM_VERTEX);
            NiFree(pcTemp);
        }
        if (NSBShader::GetReadVersion() >= 0x00010012)
        {
            // Added PS3 support in version 1.12
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp, 
               NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_VERTEX);
            NiFree(pcTemp);
        }
        if (NSBShader::GetReadVersion() >= 0x00010013)
        {
            // Added D3D10 and geometry shader support in version 1.13.
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp, 
               NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_VERTEX);
            NiFree(pcTemp);
        }
    }

    // Added renderer-specific entry point and targets in version 2.1.
    char* pcProgramEntryPoint = kStream.ReadCString();
    char* pcProgramTarget = kStream.ReadCString();

    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        SetShaderProgramEntryPoint(pcProgramEntryPoint, 
            NiSystemDesc::RendererID(i), NiGPUProgram::PROGRAM_VERTEX);
        SetShaderProgramShaderTarget(pcProgramTarget, 
            NiSystemDesc::RendererID(i), NiGPUProgram::PROGRAM_VERTEX);
    }
    NiFree(pcProgramEntryPoint);
    NiFree(pcProgramTarget);

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinaryGeometryShaderProgram(NiBinaryStream& kStream)
{
    if (NSBShader::GetReadVersion() >= 0x00020001)
    {
        return LoadBinaryShaderProgram(kStream, 
            NiGPUProgram::PROGRAM_GEOMETRY);
    }


    if (NSBShader::GetReadVersion() < 0x00010013)
    {
        // Geometry shaders did not exist before version 1.13.
        return true;
    }

    // Added geometry shader support for DX9, Xenon, PS3, and D3D10 in
    // version 1.13.
    char* pcTemp = kStream.ReadCString();
    SetShaderProgramFile(pcTemp, 
       NiSystemDesc::RENDERER_DX9, NiGPUProgram::PROGRAM_GEOMETRY);
    NiFree(pcTemp);
    pcTemp = kStream.ReadCString();
    SetShaderProgramFile(pcTemp, 
       NiSystemDesc::RENDERER_XENON, NiGPUProgram::PROGRAM_GEOMETRY);
    NiFree(pcTemp);
    pcTemp = kStream.ReadCString();
    SetShaderProgramFile(pcTemp, 
       NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_GEOMETRY);
    NiFree(pcTemp);
    pcTemp = kStream.ReadCString();
    SetShaderProgramFile(pcTemp, 
       NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_GEOMETRY);
    NiFree(pcTemp);

    // Added renderer-specific entry point and targets in version 2.1.
    char* pcProgramEntryPoint = kStream.ReadCString();
    char* pcProgramTarget = kStream.ReadCString();

    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        SetShaderProgramEntryPoint(pcProgramEntryPoint, 
            NiSystemDesc::RendererID(i), NiGPUProgram::PROGRAM_GEOMETRY);
        SetShaderProgramShaderTarget(pcProgramTarget, 
            NiSystemDesc::RendererID(i), NiGPUProgram::PROGRAM_GEOMETRY);
    }
    NiFree(pcProgramEntryPoint);
    NiFree(pcProgramTarget);

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinaryPixelShaderProgram(NiBinaryStream& kStream)
{
    if (NSBShader::GetReadVersion() >= 0x00020001)
    {
        return LoadBinaryShaderProgram(kStream, 
            NiGPUProgram::PROGRAM_PIXEL);
    }

    if (NSBShader::GetReadVersion() == 0x00010001)
    {
        // Version 1.1 only had a single shader file name
        char* pcProgramFile = kStream.ReadCString();
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_PS3, 
            NiGPUProgram::PROGRAM_PIXEL);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_DX9, 
            NiGPUProgram::PROGRAM_PIXEL);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_XENON, 
            NiGPUProgram::PROGRAM_PIXEL);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_D3D10, 
            NiGPUProgram::PROGRAM_PIXEL);
        NiFree(pcProgramFile);
    }
    else
    {
        char* pcTemp = 0;
        if (NSBShader::GetReadVersion() < 0x00010008)
        {
            

            // Removed Xbox support in version 1.8
            pcTemp = kStream.ReadCString();
            NiFree(pcTemp);

            // Removed DX8 support in version 1.8
            pcTemp = kStream.ReadCString();
            NiFree(pcTemp);
        }

        pcTemp = kStream.ReadCString();
        SetShaderProgramFile(pcTemp, 
            NiSystemDesc::RENDERER_DX9, NiGPUProgram::PROGRAM_PIXEL);
        NiFree(pcTemp);

        if (NSBShader::GetReadVersion() >= 0x00010008)
        {
            // Added Xenon support in version 1.8
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp, 
                NiSystemDesc::RENDERER_XENON, NiGPUProgram::PROGRAM_PIXEL);
            NiFree(pcTemp);
        }
        if (NSBShader::GetReadVersion() >= 0x00010012)
        {
            // Added PS3 support in version 1.12
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp, 
                NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_PIXEL);
            NiFree(pcTemp);
        }
        if (NSBShader::GetReadVersion() >= 0x00010013)
        {
            // Added D3D10 and geometry shader support in version 1.13.
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp, 
                NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_PIXEL);
            NiFree(pcTemp);
        }
    }

    // Version 1.1 did not have entry points or targets.
    if (NSBShader::GetReadVersion() <= 0x00010001)
        return true;

    // Added renderer-specific entry point and targets in version 2.1.
    char* pcProgramEntryPoint = kStream.ReadCString();
    char* pcProgramTarget = kStream.ReadCString();

    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        SetShaderProgramEntryPoint(pcProgramEntryPoint, 
            NiSystemDesc::RendererID(i), NiGPUProgram::PROGRAM_PIXEL);
        SetShaderProgramShaderTarget(pcProgramTarget, 
            NiSystemDesc::RendererID(i), NiGPUProgram::PROGRAM_PIXEL);
    }
    NiFree(pcProgramEntryPoint);
    NiFree(pcProgramTarget);

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinaryStages(NiBinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    m_akStages.SetSize(uiCount);

    NSBTextureStage* pkStage;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkStage = NiNew NSBTextureStage();
        NIASSERT(pkStage);

        if (!pkStage->LoadBinary(kStream))
            return false;

        NIASSERT(ui == pkStage->GetStage());
        m_akStages.SetAt(ui, pkStage);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBPass::LoadBinaryTextures(NiBinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    m_akTextures.SetSize(uiCount);

    NSBTexture* pkTexture;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkTexture = NiNew NSBTexture();
        NIASSERT(pkTexture);

        if (!pkTexture->LoadBinary(kStream))
            return false;

        NIASSERT(ui == pkTexture->GetStage());
        m_akTextures.SetAt(ui, pkTexture);
    }

    return true;
}
//---------------------------------------------------------------------------
NSBPass::ShaderProgramInfo::ShaderProgramInfo() :
    m_pcProgramFile(NULL),
    m_pcEntryPoint(NULL),
    m_pcTarget(NULL)
{
}
//---------------------------------------------------------------------------
NSBPass::ShaderProgramInfo::~ShaderProgramInfo()
{
    NiFree(m_pcProgramFile);
    NiFree(m_pcEntryPoint);
    NiFree(m_pcTarget);
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBPass::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "Pass %s\n", m_pcName);
    NSBUtility::IndentInsert();

    if (m_pkRenderStateGroup)
    {
        NSBUtility::Dump(pf, true, "RenderStateGroup\n");
        NSBUtility::IndentInsert();
        m_pkRenderStateGroup->Dump(pf, NSBStateGroup::DUMP_RENDERSTATES);
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "***   NO RS GROUP   ***\n");
    }

    NSBUtility::Dump(pf, true, "\n");
    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        NiSystemDesc::RendererID eRenderer = NiSystemDesc::RendererID(i);
        const char* pcRend = NiSystemDesc::GetRendererString(eRenderer);
        NSBUtility::Dump(pf, true, "VertexShader Program File (%s) : %s\n",
            pcRend, GetShaderProgramFile(eRenderer, 
            NiGPUProgram::PROGRAM_VERTEX));
        NSBUtility::Dump(pf, true, 
            "VertexShader Program Entry Point (%s): %s\n", pcRend,
            GetShaderProgramEntryPoint(eRenderer, 
            NiGPUProgram::PROGRAM_VERTEX));
        NSBUtility::Dump(pf, true, 
            "VertexShader Program Shader Target (%s): %s\n", pcRend,
            GetShaderProgramShaderTarget(eRenderer, 
            NiGPUProgram::PROGRAM_VERTEX));
    }

    NSBUtility::Dump(pf, true, "\n");


    unsigned int uiConstantMapCount = m_kVertexShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBUtility::Dump(pf, true, "VertexConstantMaps\n");
        NSBUtility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kVertexShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "*** NO VERTEX CMAPS  ***\n");
    }

    NSBUtility::Dump(pf, true, "\n");
    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        NiSystemDesc::RendererID eRenderer = NiSystemDesc::RendererID(i);
        const char* pcRend = NiSystemDesc::GetRendererString(eRenderer);
        NSBUtility::Dump(pf, true, "GeometryShader Program File (%s) : %s\n",
            pcRend, GetShaderProgramFile(eRenderer, 
            NiGPUProgram::PROGRAM_GEOMETRY));
        NSBUtility::Dump(pf, true, 
            "GeometryShader Program Entry Point (%s): %s\n", pcRend,
            GetShaderProgramEntryPoint(eRenderer, 
            NiGPUProgram::PROGRAM_GEOMETRY));
        NSBUtility::Dump(pf, true, 
            "GeometryShader Program Shader Target (%s): %s\n", pcRend,
            GetShaderProgramShaderTarget(eRenderer, 
            NiGPUProgram::PROGRAM_GEOMETRY));
    }

    uiConstantMapCount = m_kGeometryShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBUtility::Dump(pf, true, "GeometryConstantMaps\n");
        NSBUtility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kGeometryShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "*** NO GEOMETRY CMAPS  ***\n");
    }

    NSBUtility::Dump(pf, true, "\n");
    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        NiSystemDesc::RendererID eRenderer = NiSystemDesc::RendererID(i);
        const char* pcRend = NiSystemDesc::GetRendererString(eRenderer);
        NSBUtility::Dump(pf, true, "PixelShader Program File (%s) : %s\n",
            pcRend, GetShaderProgramFile(eRenderer, 
            NiGPUProgram::PROGRAM_PIXEL));
        NSBUtility::Dump(pf, true, 
            "PixelShader Program Entry Point (%s): %s\n", pcRend,
            GetShaderProgramEntryPoint(eRenderer, 
            NiGPUProgram::PROGRAM_PIXEL));
        NSBUtility::Dump(pf, true, 
            "PixelShader Program Shader Target (%s): %s\n", pcRend,
            GetShaderProgramShaderTarget(eRenderer, 
            NiGPUProgram::PROGRAM_PIXEL));
    }

    uiConstantMapCount = m_kPixelShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBUtility::Dump(pf, true, "PixelConstantMaps\n");
        NSBUtility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kPixelShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "***  NO PIXEL CMAPS  ***\n");
    }

    NSBUtility::Dump(pf, true, "\n");
    NSBUtility::Dump(pf, true, "Software Vertex Processing: %s\n",
        m_bSoftwareVP ? "True" : "False");

    unsigned int uiCount = m_akStages.GetSize();
    NSBUtility::Dump(pf, true, "Stage Count = %d\n",
        m_akStages.GetEffectiveSize());
    NSBUtility::IndentInsert();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBTextureStage* pkStage = m_akStages.GetAt(ui);
        if (pkStage)
            pkStage->Dump(pf);
    }
    NSBUtility::IndentRemove();

    uiCount = m_akTextures.GetSize();
    NSBUtility::Dump(pf, true, "Texture Count = %d\n",
        m_akTextures.GetEffectiveSize());
    NSBUtility::IndentInsert();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBTexture* pkTexture = m_akTextures.GetAt(ui);
        if (pkTexture)
            pkTexture->Dump(pf);
    }
    NSBUtility::IndentRemove();

    NSBUtility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
