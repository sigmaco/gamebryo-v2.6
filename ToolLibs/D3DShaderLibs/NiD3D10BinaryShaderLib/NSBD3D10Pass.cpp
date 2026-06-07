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
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NSBD3D10Pass.h"
#include "NSBD3D10StateGroup.h"
#include "NSBD3D10Texture.h"
#include "NSBD3D10TextureStage.h"
#include "NSBD3D10Utility.h"
#include "NSBD3D10Shader.h"

#include <NiD3D10Pass.h>
#include <NiD3D10Renderer.h>

//---------------------------------------------------------------------------
NSBD3D10Pass::NSBD3D10Pass() :
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
NSBD3D10Pass::~NSBD3D10Pass()
{
    NiFree(m_pcName);
    NiDelete m_pkRenderStateGroup;

    // Clean up the shader constant maps.

    unsigned int uiSize = m_kVertexShaderConstantMaps.GetSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NSBD3D10ConstantMap* pkCMap = m_kVertexShaderConstantMaps.GetAt(i);
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
        NSBD3D10ConstantMap* pkCMap = m_kGeometryShaderConstantMaps.GetAt(i);
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
        NSBD3D10ConstantMap* pkCMap = m_kPixelShaderConstantMaps.GetAt(i);
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
        NSBD3D10TextureStage* pkStage = m_akStages.GetAt(i);
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
        NSBD3D10Texture* pkTexture = m_akTextures.GetAt(i);
        if (pkTexture)
        {
            NiDelete pkTexture;
            m_akTextures.SetAt(i, 0);
        }
    }

    m_akTextures.RemoveAll();
}
//---------------------------------------------------------------------------
void NSBD3D10Pass::SetName(const char* pcName)
{
    NSBD3D10Utility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
NSBD3D10StateGroup* NSBD3D10Pass::GetRenderStateGroup()
{
    if (m_pkRenderStateGroup == 0)
        m_pkRenderStateGroup = NiNew NSBD3D10StateGroup();

    return m_pkRenderStateGroup;
}
//---------------------------------------------------------------------------
bool NSBD3D10Pass::GetShaderPresent(NiGPUProgram::ProgramType eType) const
{
    return (m_akInfo[NiSystemDesc::RENDERER_D3D10][eType].m_pcProgramFile != 
        NULL);
}
//---------------------------------------------------------------------------
NSBD3D10ConstantMap* NSBD3D10Pass::GetVertexConstantMap(unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kVertexShaderConstantMaps.GetSize());

    if (uiIndex == m_kVertexShaderConstantMaps.GetSize())
    {
        NSBD3D10ConstantMap* pkVertexConstantMap = NiNew NSBD3D10ConstantMap();
        NIASSERT(pkVertexConstantMap);
        pkVertexConstantMap->SetProgramType(NiGPUProgram::PROGRAM_VERTEX);
        m_kVertexShaderConstantMaps.Add(pkVertexConstantMap);
    }
    else if (uiIndex < m_kVertexShaderConstantMaps.GetSize())
    {
        NSBD3D10ConstantMap* pkVertexConstantMap
            = m_kVertexShaderConstantMaps.GetAt(uiIndex);
        if (pkVertexConstantMap == NULL)
        {
            pkVertexConstantMap = NiNew NSBD3D10ConstantMap();
            NIASSERT(pkVertexConstantMap);
            pkVertexConstantMap->SetProgramType(NiGPUProgram::PROGRAM_VERTEX);
            m_kVertexShaderConstantMaps.ReplaceAt(uiIndex,
                pkVertexConstantMap);
        }
    }

    return m_kVertexShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NSBD3D10ConstantMap* NSBD3D10Pass::GetGeometryConstantMap(
    unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kGeometryShaderConstantMaps.GetSize());

    if (uiIndex == m_kGeometryShaderConstantMaps.GetSize())
    {
        NSBD3D10ConstantMap* pkGeometryConstantMap
            = NiNew NSBD3D10ConstantMap();
        NIASSERT(pkGeometryConstantMap);
        pkGeometryConstantMap->SetProgramType(NiGPUProgram::PROGRAM_GEOMETRY);
        m_kGeometryShaderConstantMaps.Add(pkGeometryConstantMap);
    }
    else if (uiIndex < m_kGeometryShaderConstantMaps.GetSize())
    {
        NSBD3D10ConstantMap* pkGeometryConstantMap
            = m_kGeometryShaderConstantMaps.GetAt(uiIndex);
        if (pkGeometryConstantMap == NULL)
        {
            pkGeometryConstantMap = NiNew NSBD3D10ConstantMap();
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
NSBD3D10ConstantMap* NSBD3D10Pass::GetPixelConstantMap(unsigned int uiIndex)
{
    NIASSERT(uiIndex <= m_kPixelShaderConstantMaps.GetSize());

    if (uiIndex == m_kPixelShaderConstantMaps.GetSize())
    {
        NSBD3D10ConstantMap* pkPixelConstantMap = NiNew NSBD3D10ConstantMap();
        NIASSERT(pkPixelConstantMap);
        pkPixelConstantMap->SetProgramType(NiGPUProgram::PROGRAM_PIXEL);
        m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);
    }
    else if (uiIndex < m_kPixelShaderConstantMaps.GetSize())
    {
        NSBD3D10ConstantMap* pkPixelConstantMap
            = m_kPixelShaderConstantMaps.GetAt(uiIndex);
        if (pkPixelConstantMap == NULL)
        {
            pkPixelConstantMap = NiNew NSBD3D10ConstantMap();
            NIASSERT(pkPixelConstantMap);
            pkPixelConstantMap->SetProgramType(NiGPUProgram::PROGRAM_PIXEL);
            m_kPixelShaderConstantMaps.ReplaceAt(uiIndex, pkPixelConstantMap);
        }
    }

    return m_kPixelShaderConstantMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Pass::GetStageCount()
{
    return m_akStages.GetSize();
}
//---------------------------------------------------------------------------
NSBD3D10TextureStage* NSBD3D10Pass::GetStage(unsigned int uiIndex,
    bool bCreate)
{
    NSBD3D10TextureStage* pkStage = 0;

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
                pkStage = NiNew NSBD3D10TextureStage();
                NIASSERT(pkStage);

                pkStage->SetStage(i);

                m_akStages.SetAt(i, pkStage);
            }
        }
    }

    return pkStage;
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Pass::GetTextureCount()
{
    return m_akTextures.GetSize();
}
//---------------------------------------------------------------------------
NSBD3D10Texture* NSBD3D10Pass::GetTexture(unsigned int uiIndex, bool bCreate)
{
    NSBD3D10Texture* pkTexture = 0;

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
                pkTexture = NiNew NSBD3D10Texture();
                NIASSERT(pkTexture);

                pkTexture->SetStage(i);

                m_akTextures.SetAt(i, pkTexture);
            }
        }
    }

    return pkTexture;
}
//---------------------------------------------------------------------------
bool NSBD3D10Pass::SetupNiD3D10Pass(NiD3D10Pass& kPass,
    NiShaderDesc* pkShaderDesc)
{
    // Convert the NSBD3D10 representation to a run-time NiD3D10Pass.

    // RenderStateGroup
    NiD3D10RenderStateGroup* pkRSGroup = 0;
    if (m_pkRenderStateGroup)
    {
        if (m_pkRenderStateGroup->GetStateCount())
        {
            pkRSGroup = NiNew NiD3D10RenderStateGroup();
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
    unsigned int uiConstantMapCount = m_kVertexShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBD3D10ConstantMap* pkCM = m_kVertexShaderConstantMaps.GetAt(i);
            if (pkCM)
            {
                kPass.SetVertexConstantMap(i, pkCM->GetVertexConstantMap(
                    pkShaderDesc));
            }
            else
            {
                kPass.SetVertexConstantMap(i, NULL);
            }
        }
    }

    // Geometry
    uiConstantMapCount = m_kGeometryShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBD3D10ConstantMap* pkCM = m_kGeometryShaderConstantMaps.GetAt(i);
            if (pkCM)
            {
                kPass.SetGeometryConstantMap(i, pkCM->GetGeometryConstantMap(
                    pkShaderDesc));
            }
            else
            {
                kPass.SetGeometryConstantMap(i, NULL);
            }
        }
    }

    // Stream Out
    kPass.SetStreamOutSettings(m_kStreamOutSettings);

    // Pixel
    uiConstantMapCount = m_kPixelShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBD3D10ConstantMap* pkCM = m_kPixelShaderConstantMaps.GetAt(i);
            if (pkCM)
            {
                kPass.SetPixelConstantMap(i, pkCM->GetPixelConstantMap(
                    pkShaderDesc));
            }
            else
            {
                kPass.SetPixelConstantMap(i, NULL);
            }
        }
    }

    // Shader Programs

    kPass.SetVertexShaderProgramFileName(GetShaderProgramFile(
        NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_VERTEX));
    kPass.SetVertexShaderProgramEntryPoint(GetShaderProgramEntryPoint(
        NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_VERTEX));
    kPass.SetVertexShaderProgramShaderTarget(GetShaderProgramShaderTarget(
        NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_VERTEX));

    kPass.SetGeometryShaderProgramFileName(GetShaderProgramFile(
        NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_GEOMETRY));
    kPass.SetGeometryShaderProgramEntryPoint(GetShaderProgramEntryPoint(
        NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_GEOMETRY));
    kPass.SetGeometryShaderProgramShaderTarget(GetShaderProgramShaderTarget(
        NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_GEOMETRY));

    kPass.SetPixelShaderProgramFileName(GetShaderProgramFile(
        NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_PIXEL));
    kPass.SetPixelShaderProgramEntryPoint(GetShaderProgramEntryPoint(
        NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_PIXEL));
    kPass.SetPixelShaderProgramShaderTarget(GetShaderProgramShaderTarget(
        NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_PIXEL));

    NSBD3D10TextureStage* pkStage;
    const unsigned int uiStageCount = m_akStages.GetSize();
    if (uiStageCount > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        return false;

    for (unsigned int ui = 0; ui < uiStageCount; ui++)
    {
        pkStage = m_akStages.GetAt(ui);
        if (pkStage)
        {
            if (!pkStage->SetupTextureStage(kPass))
            {
                return false;
            }
        }
    }

    NSBD3D10Texture* pkTexture;
    unsigned int uiTextureCount = m_akTextures.GetSize();
    if (uiTextureCount == 0)
    {
        // If no Texture { } blocks were found, use the Sampler { } stages
        // to create a complete set of textures.
        m_akTextures.SetSize(uiStageCount);
        for (unsigned int i = 0; i < uiStageCount; i++)
        {
            // For each stage, the NSB file stored the texture attribute
            // index; search the attributes for the texture with that index,
            // then use that to get the texture's name.
            pkStage = m_akStages.GetAt(i);
            pkTexture = NiNew NSBD3D10Texture;
            const char* pcName = NULL;
            unsigned int uiNumAttrib = pkShaderDesc->GetNumberOfAttributes();
            const NiShaderAttributeDesc* pkAttrib = 
                pkShaderDesc->GetFirstAttribute();
            for (unsigned int j = 0; j < uiNumAttrib; j++)
            {
                if (pkAttrib->GetType() == 
                    NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE)
                {
                    unsigned int uiValue;
                    const char* pcValue;
                    if (pkAttrib->GetValue_Texture(uiValue, pcValue) && 
                        uiValue == i)
                    {
                        pcName = pkAttrib->GetName();
                    }
                }
                if (j < uiNumAttrib - 1)
                    pkAttrib = pkShaderDesc->GetNextAttribute();
            }

            if (pcName == NULL)
                pcName = pkStage->GetName();

            NIASSERT(pcName);

            pkTexture->CreateFromTextureStage(pkStage, pcName);
            m_akTextures.SetAt(i, pkTexture);
        }
        uiTextureCount = uiStageCount;
    }

    for (unsigned int ui = 0; ui < uiTextureCount; ui++)
    {
        pkTexture = m_akTextures.GetAt(ui);

        if (pkTexture)
        {
            if (!pkTexture->SetupTextureStage(kPass))
            {
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Pass::SaveBinary(NiBinaryStream& kStream)
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

    if (!NSBD3D10Utility::SaveBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;

    if (!SaveBinaryShaderProgram(kStream, NiGPUProgram::PROGRAM_PIXEL))
        return false;
    unsigned int uiConstantMapCount = m_kPixelShaderConstantMaps.GetSize();
    NiStreamSaveBinary(kStream, uiConstantMapCount);
    for (unsigned int i=0; i < uiConstantMapCount; i++)
    {
        if (!NSBD3D10Utility::SaveBinaryConstantMap(kStream,
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
        if (!NSBD3D10Utility::SaveBinaryConstantMap(kStream,
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
        if (!NSBD3D10Utility::SaveBinaryConstantMap(kStream,
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

    // Version 2.2 added support for stream output
    m_kStreamOutSettings.SaveBinary(kStream);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Pass::LoadBinary(NiBinaryStream& kStream)
{
    m_pcName = kStream.ReadCString(); 

    // Version 1.6 added support for user-defined data...
    if (NSBD3D10Shader::GetReadVersion() >= 0x00010006)
    {
        unsigned int uiUDDSetPresent;
        NiStreamLoadBinary(kStream, uiUDDSetPresent);
        if (uiUDDSetPresent)
        {
            m_spUserDefinedDataSet = NiNew NSBD3D10UserDefinedDataSet();
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
    if (!NSBD3D10Utility::LoadBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;


    if (NSBD3D10Shader::GetReadVersion() < 0x00010013)
    {
        // Before 1.13, there was exactly one constant map for vertex and
        // pixel shaders.
        if (!LoadBinaryPixelShaderProgram(kStream))
            return false;
        m_kPixelShaderConstantMaps.RemoveAll();
        NSBD3D10ConstantMap* pkPixelConstantMap = NULL;
        if (!NSBD3D10Utility::LoadBinaryConstantMap(kStream,
            pkPixelConstantMap))
        {
            return false;
        }
        m_kPixelShaderConstantMaps.Add(pkPixelConstantMap);

        if (!LoadBinaryVertexShaderProgram(kStream))
            return false;
        m_kVertexShaderConstantMaps.RemoveAll();
        NSBD3D10ConstantMap* pkVertexConstantMap = NULL;
        if (!NSBD3D10Utility::LoadBinaryConstantMap(kStream,
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
            NSBD3D10ConstantMap* pkPixelConstantMap = NULL;
            if (!NSBD3D10Utility::LoadBinaryConstantMap(kStream,
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
            NSBD3D10ConstantMap* pkVertexConstantMap = NULL;
            if (!NSBD3D10Utility::LoadBinaryConstantMap(kStream,
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
            NSBD3D10ConstantMap* pkGeometryConstantMap = NULL;
            if (!NSBD3D10Utility::LoadBinaryConstantMap(kStream,
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

    if (NSBD3D10Shader::GetReadVersion() >= 0x00010014)
    {
        // Version 1.14 added separate NSBD3D10Texture objects.

        if (!LoadBinaryTextures(kStream))
            return false;
    }

    // Version 2.2 added support for stream output
    if (NSBD3D10Shader::GetReadVersion() >= 0x00020002)
        m_kStreamOutSettings.LoadBinary(kStream);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Pass::LoadBinaryShaderProgram(NiBinaryStream& kStream, 
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
bool NSBD3D10Pass::SaveBinaryShaderProgram(NiBinaryStream& kStream, 
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
bool NSBD3D10Pass::SaveBinaryStages(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_akStages.GetEffectiveSize();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBD3D10TextureStage* pkStage;
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
bool NSBD3D10Pass::SaveBinaryTextures(NiBinaryStream& kStream)
{
    unsigned int uiCount = m_akTextures.GetEffectiveSize();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBD3D10Texture* pkTexture;
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
bool NSBD3D10Pass::LoadBinaryVertexShaderProgram(NiBinaryStream& kStream)
{
    if (NSBD3D10Shader::GetReadVersion() >= 0x00020001)
        return LoadBinaryShaderProgram(kStream, NiGPUProgram::PROGRAM_VERTEX);

    if (NSBD3D10Shader::GetReadVersion() == 0x00010001)
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
        if (NSBD3D10Shader::GetReadVersion() < 0x00010008)
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
        if (NSBD3D10Shader::GetReadVersion() >= 0x00010008)
        {
            // Added Xenon support in version 1.8
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp, 
               NiSystemDesc::RENDERER_XENON, NiGPUProgram::PROGRAM_VERTEX);
            NiFree(pcTemp);
        }
        if (NSBD3D10Shader::GetReadVersion() >= 0x00010012)
        {
            // Added PS3 support in version 1.12
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp, 
               NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_VERTEX);
            NiFree(pcTemp);
        }
        if (NSBD3D10Shader::GetReadVersion() >= 0x00010013)
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

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Pass::LoadBinaryGeometryShaderProgram(NiBinaryStream& kStream)
{
    if (NSBD3D10Shader::GetReadVersion() >= 0x00020001)
    {
        return LoadBinaryShaderProgram(kStream, 
            NiGPUProgram::PROGRAM_GEOMETRY);
    }

    if (NSBD3D10Shader::GetReadVersion() < 0x00010013)
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
bool NSBD3D10Pass::LoadBinaryPixelShaderProgram(NiBinaryStream& kStream)
{
    if (NSBD3D10Shader::GetReadVersion() >= 0x00020001)
    {
        return LoadBinaryShaderProgram(kStream, 
            NiGPUProgram::PROGRAM_PIXEL);
    }

    if (NSBD3D10Shader::GetReadVersion() == 0x00010001)
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
        if (NSBD3D10Shader::GetReadVersion() < 0x00010008)
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

        if (NSBD3D10Shader::GetReadVersion() >= 0x00010008)
        {
            // Added Xenon support in version 1.8
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp, 
                NiSystemDesc::RENDERER_XENON, NiGPUProgram::PROGRAM_PIXEL);
            NiFree(pcTemp);
        }
        if (NSBD3D10Shader::GetReadVersion() >= 0x00010012)
        {
            // Added PS3 support in version 1.12
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp, 
                NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_PIXEL);
            NiFree(pcTemp);
        }
        if (NSBD3D10Shader::GetReadVersion() >= 0x00010013)
        {
            // Added D3D10 and geometry shader support in version 1.13.
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp, 
                NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_PIXEL);
            NiFree(pcTemp);
        }
    }

    // Version 1.1 did not have entry points or targets.
    if (NSBD3D10Shader::GetReadVersion() <= 0x00010001)
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
bool NSBD3D10Pass::LoadBinaryStages(NiBinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    m_akStages.SetSize(uiCount);

    NSBD3D10TextureStage* pkStage;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkStage = NiNew NSBD3D10TextureStage();
        NIASSERT(pkStage);

        if (!pkStage->LoadBinary(kStream))
            return false;

        NIASSERT(ui == pkStage->GetStage());
        m_akStages.SetAt(ui, pkStage);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Pass::LoadBinaryTextures(NiBinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    m_akTextures.SetSize(uiCount);

    NSBD3D10Texture* pkTexture;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkTexture = NiNew NSBD3D10Texture();
        NIASSERT(pkTexture);

        if (!pkTexture->LoadBinary(kStream))
            return false;

        NIASSERT(ui == pkTexture->GetStage());
        m_akTextures.SetAt(ui, pkTexture);
    }

    return true;
}
//---------------------------------------------------------------------------
NSBD3D10Pass::ShaderProgramInfo::ShaderProgramInfo() :
    m_pcProgramFile(NULL),
    m_pcEntryPoint(NULL),
    m_pcTarget(NULL)
{
}
//---------------------------------------------------------------------------
NSBD3D10Pass::ShaderProgramInfo::~ShaderProgramInfo()
{
    NiFree(m_pcProgramFile);
    NiFree(m_pcEntryPoint);
    NiFree(m_pcTarget);
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBD3D10Pass::Dump(FILE* pf)
{
    NSBD3D10Utility::Dump(pf, true, "Pass %s\n", m_pcName);
    NSBD3D10Utility::IndentInsert();

    if (m_pkRenderStateGroup)
    {
        NSBD3D10Utility::Dump(pf, true, "RenderStateGroup\n");
        NSBD3D10Utility::IndentInsert();
        m_pkRenderStateGroup->Dump(pf, NSBD3D10StateGroup::DUMP_RENDERSTATES);
        NSBD3D10Utility::IndentRemove();
    }
    else
    {
        NSBD3D10Utility::Dump(pf, true, "***   NO RS GROUP   ***\n");
    }

    NSBD3D10Utility::Dump(pf, true, "\n");
    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        NiSystemDesc::RendererID eRenderer = NiSystemDesc::RendererID(i);
        const char* pcRend = NiSystemDesc::GetRendererString(eRenderer);
        NSBD3D10Utility::Dump(pf, true,
            "VertexShader Program File (%s) : %s\n", pcRend,
            GetShaderProgramFile(eRenderer, 
            NiGPUProgram::PROGRAM_VERTEX));
        NSBD3D10Utility::Dump(pf, true, 
            "VertexShader Program Entry Point (%s): %s\n", pcRend,
            GetShaderProgramEntryPoint(eRenderer, 
            NiGPUProgram::PROGRAM_VERTEX));
        NSBD3D10Utility::Dump(pf, true, 
            "VertexShader Program Shader Target (%s): %s\n", pcRend,
            GetShaderProgramShaderTarget(eRenderer, 
            NiGPUProgram::PROGRAM_VERTEX));
    }

    NSBD3D10Utility::Dump(pf, true, "\n");


    unsigned int uiConstantMapCount = m_kVertexShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBD3D10Utility::Dump(pf, true, "VertexConstantMaps\n");
        NSBD3D10Utility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kVertexShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBD3D10Utility::IndentRemove();
    }
    else
    {
        NSBD3D10Utility::Dump(pf, true, "*** NO VERTEX CMAPS  ***\n");
    }

    NSBD3D10Utility::Dump(pf, true, "\n");
    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        NiSystemDesc::RendererID eRenderer = NiSystemDesc::RendererID(i);
        const char* pcRend = NiSystemDesc::GetRendererString(eRenderer);
        NSBD3D10Utility::Dump(pf, true,
            "GeometryShader Program File (%s) : %s\n", pcRend,
            GetShaderProgramFile(eRenderer, 
            NiGPUProgram::PROGRAM_GEOMETRY));
        NSBD3D10Utility::Dump(pf, true, 
            "GeometryShader Program Entry Point (%s): %s\n", pcRend,
            GetShaderProgramEntryPoint(eRenderer, 
            NiGPUProgram::PROGRAM_GEOMETRY));
        NSBD3D10Utility::Dump(pf, true, 
            "GeometryShader Program Shader Target (%s): %s\n", pcRend,
            GetShaderProgramShaderTarget(eRenderer, 
            NiGPUProgram::PROGRAM_GEOMETRY));
    }

    uiConstantMapCount = m_kGeometryShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBD3D10Utility::Dump(pf, true, "GeometryConstantMaps\n");
        NSBD3D10Utility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kGeometryShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBD3D10Utility::IndentRemove();
    }
    else
    {
        NSBD3D10Utility::Dump(pf, true, "*** NO GEOMETRY CMAPS  ***\n");
    }

    NSBD3D10Utility::Dump(pf, true, "\n");
    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        NiSystemDesc::RendererID eRenderer = NiSystemDesc::RendererID(i);
        const char* pcRend = NiSystemDesc::GetRendererString(eRenderer);
        NSBD3D10Utility::Dump(pf, true, "PixelShader Program File (%s) : %s\n",
            pcRend, GetShaderProgramFile(eRenderer, 
            NiGPUProgram::PROGRAM_PIXEL));
        NSBD3D10Utility::Dump(pf, true, 
            "PixelShader Program Entry Point (%s): %s\n", pcRend,
            GetShaderProgramEntryPoint(eRenderer, 
            NiGPUProgram::PROGRAM_PIXEL));
        NSBD3D10Utility::Dump(pf, true, 
            "PixelShader Program Shader Target (%s): %s\n", pcRend,
            GetShaderProgramShaderTarget(eRenderer, 
            NiGPUProgram::PROGRAM_PIXEL));
    }

    uiConstantMapCount = m_kPixelShaderConstantMaps.GetSize();
    if (uiConstantMapCount > 0)
    {
        NSBD3D10Utility::Dump(pf, true, "PixelConstantMaps\n");
        NSBD3D10Utility::IndentInsert();
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            m_kPixelShaderConstantMaps.GetAt(i)->Dump(pf);
        }
        NSBD3D10Utility::IndentRemove();
    }
    else
    {
        NSBD3D10Utility::Dump(pf, true, "***  NO PIXEL CMAPS  ***\n");
    }

    NSBD3D10Utility::Dump(pf, true, "\n");
    NSBD3D10Utility::Dump(pf, true, "Software Vertex Processing: %s\n",
        m_bSoftwareVP ? "True" : "False");

    unsigned int uiCount = m_akStages.GetSize();
    NSBD3D10Utility::Dump(pf, true, "Stage Count = %d\n", 
        m_akStages.GetEffectiveSize());
    NSBD3D10Utility::IndentInsert();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBD3D10TextureStage* pkStage = m_akStages.GetAt(ui);
        if (pkStage)
            pkStage->Dump(pf);
    }
    NSBD3D10Utility::IndentRemove();

    uiCount = m_akTextures.GetSize();
    NSBD3D10Utility::Dump(pf, true, "Texture Count = %d\n", 
        m_akTextures.GetEffectiveSize());
    NSBD3D10Utility::IndentInsert();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBD3D10Texture* pkTexture = m_akTextures.GetAt(ui);
        if (pkTexture)
            pkTexture->Dump(pf);
    }
    NSBD3D10Utility::IndentRemove();

    NSBD3D10Utility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
