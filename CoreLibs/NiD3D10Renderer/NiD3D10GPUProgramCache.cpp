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

#include "NiD3D10GeometryShader.h"
#include "NiD3D10GPUProgramCache.h"
#include "NiD3D10PixelShader.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10ShaderProgramFactory.h"
#include "NiD3D10VertexShader.h"

#include <NiFile.h>

//---------------------------------------------------------------------------
NiD3D10GPUProgramCache::NiD3D10GPUProgramCache(unsigned int uiVersion, 
    const char* pcWorkingDir, NiGPUProgram::ProgramType eType, 
    const NiFixedString& kShaderProfile, const char* pcMaterialIdentifier,
    bool bAutoWriteCacheToDisk, bool bWriteDebugHLSLFile,
    bool bLocked, bool bLoad) : 
    NiGPUProgramCache(uiVersion, bAutoWriteCacheToDisk, bLocked),
    m_bWriteDebugHLSLFile(bWriteDebugHLSLFile),
    m_eType(eType),
    m_kMaterialIdentifier(pcMaterialIdentifier)
{
    char acFilename[NI_MAX_PATH];
    NiSprintf(acFilename, NI_MAX_PATH, 
#if defined(NIDEBUG)
        "%s_%s_%s_D.cache", 
#else
        "%s_%s_%s.cache", 
#endif
        pcMaterialIdentifier, (const char*)kShaderProfile, 
        (const char*)GetPlatform());

    m_kFilename = acFilename;
    SetWorkingDirectory(pcWorkingDir);

    m_kShaderProfile = kShaderProfile;
    
    if (eType == NiGPUProgram::PROGRAM_VERTEX)
    {
        if (m_kShaderProfile.EqualsNoCase("vs_4_0"))
        {
            m_kValidTargets.Add("vs_4_0");
        }
    }
    if (eType == NiGPUProgram::PROGRAM_VERTEX)
    {
        if (m_kShaderProfile.EqualsNoCase("gs_4_0"))
        {
            m_kValidTargets.Add("gs_4_0");
        }
    }
    else if (eType == NiGPUProgram::PROGRAM_PIXEL)
    {
        if (m_kShaderProfile.EqualsNoCase("ps_4_0"))
        {
            m_kValidTargets.Add("ps_4_0");
        }
    }

    if (bLoad && pcWorkingDir)
    {
        Load();
    }
}
//---------------------------------------------------------------------------
const char* NiD3D10GPUProgramCache::GetPlatformSpecificCodeID() const
{
    return "hlsl";
}
//---------------------------------------------------------------------------
NiGPUProgram* NiD3D10GPUProgramCache::GenerateProgram(const char* pcName, 
    const char* pcProgramText, 
    NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms)
{
#ifdef NIDEBUG
    bool bDidNotCompilePreviously = false;
    NIASSERT(!FindCachedProgram(pcName, kUniforms, bDidNotCompilePreviously));
    NIASSERT(bDidNotCompilePreviously == false);
#endif
    if (m_bLocked)
        return NULL;

    if (pcProgramText == NULL)
        return NULL;

    size_t stProgramLength = strlen(pcProgramText);
    if (stProgramLength == 0)
        return NULL;

    ID3D10Blob* pkShaderCode = NULL;
    HRESULT hr = 
        NiD3D10Renderer::D3D10CreateBlob(stProgramLength + 1, &pkShaderCode);
    NI_UNUSED_ARG(hr);
    NIASSERT(SUCCEEDED(hr) && pkShaderCode);

    NiStrcpy((char*)pkShaderCode->GetBufferPointer(), 
        pkShaderCode->GetBufferSize(), pcProgramText);

    NiUInt32 uiFlags = 0;
    // Temporarily set it to DX9-compatibility mode
    uiFlags |= D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;

    // Debug flags
#if defined(NIDEBUG)
    uiFlags |=  D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

    char acFilename[NI_MAX_PATH];
    NiGPUProgramPtr spProgram;
    if (m_eType == NiGPUProgram::PROGRAM_VERTEX)
    {
        NiSprintf(acFilename, NI_MAX_PATH, "%s\\%s\\%s-V.%s",  
            GetWorkingDirectory(), m_kMaterialIdentifier, pcName, 
            GetPlatformSpecificCodeID());

        NiD3D10VertexShaderPtr spVSProgram;
        NiD3D10ShaderProgramFactory::CreateVertexShaderFromBlob(
            pkShaderCode, acFilename, NULL, NULL, "Main", 
            (const char*)m_kShaderProfile, uiFlags, acFilename, spVSProgram);
        spProgram = spVSProgram;
    }
    if (m_eType == NiGPUProgram::PROGRAM_GEOMETRY)
    {
        NiSprintf(acFilename, NI_MAX_PATH, "%s\\%s\\%s-G.%s",  
            GetWorkingDirectory(), m_kMaterialIdentifier, pcName, 
            GetPlatformSpecificCodeID());

        NiD3D10GeometryShaderPtr spGSProgram;
        NiD3D10ShaderProgramFactory::CreateGeometryShaderFromBlob(
            pkShaderCode, acFilename, NULL, NULL, "Main", 
            (const char*)m_kShaderProfile, uiFlags, acFilename, spGSProgram);
        spProgram = spGSProgram;
    }
    else if (m_eType == NiGPUProgram::PROGRAM_PIXEL)
    {
        NiSprintf(acFilename, NI_MAX_PATH, "%s\\%s\\%s-P.%s", 
            GetWorkingDirectory(), m_kMaterialIdentifier, pcName, 
            GetPlatformSpecificCodeID());

        NiD3D10PixelShaderPtr spPSProgram;
        NiD3D10ShaderProgramFactory::CreatePixelShaderFromBlob(
            pkShaderCode, acFilename, NULL, NULL, "Main", 
            (const char*)m_kShaderProfile, uiFlags, acFilename, spPSProgram);
        spProgram = spPSProgram;
    }
    if (m_bWriteDebugHLSLFile)
    {           
        SaveDebugHLSLFile(acFilename, 
            (const char*)pkShaderCode->GetBufferPointer());
    }

    pkShaderCode->Release();

    // Don't prune unused constants here (as many of the other renderers do) 
    // - if the constants are in the shader, then they must be included
    // in the shader constant buffer.
    if (spProgram)
    {
        NiGPUProgramCache::NiGPUProgramDesc* pkDesc = NiNew 
            NiGPUProgramCache::NiGPUProgramDesc();

        for (unsigned int ui = 0; ui < kUniforms.GetSize(); ui++)
            pkDesc->m_kUniforms.Add(kUniforms.GetAt(ui));

        pkDesc->m_spProgram = spProgram;

        NiFixedString kName = pcName;

        if (m_bAutoWriteCacheToDisk)
        {
            if (!AppendEntry(pkDesc, kName))
            {
                NiOutputDebugString("Failed to write cache entry, \"");
                NiOutputDebugString((const char*) kName);
                NiOutputDebugString("\", to cache file: \"");
                NiOutputDebugString((const char*) m_kPathAndFilename);
                NiOutputDebugString("\"\n");
            }
        }

        m_kCachedPrograms.SetAt(kName, pkDesc);
    }

    if (spProgram != NULL)
        return spProgram;

    kUniforms.RemoveAll();
    return NULL;
}
//---------------------------------------------------------------------------
const char* NiD3D10GPUProgramCache::GetPlatform() const
{
    return "D3D10";
}
//---------------------------------------------------------------------------
bool NiD3D10GPUProgramCache::SaveGPUProgram(NiBinaryStream& kStream,
    NiGPUProgram* pkProgram)
{
    NiD3D10ShaderProgram* pkShader = NiSmartPointerCast(NiD3D10ShaderProgram,
        pkProgram);

    if (!pkShader)
        return false;

    const NiRTTI* pkRTTI = pkShader->GetRTTI();
    SaveString(kStream, pkRTTI->GetName());

    ID3D10Blob* pkShaderByteCode = pkShader->GetShaderByteCode();

    SIZE_T stCodeSize = pkShaderByteCode->GetBufferSize();
    NiStreamSaveBinary(kStream, stCodeSize);
    NIASSERT(stCodeSize > 0);
    NiStreamSaveBinary(kStream, 
        (const char*)pkShaderByteCode->GetBufferPointer(), 
        (unsigned int)stCodeSize);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10GPUProgramCache::LoadGPUProgram(NiBinaryStream& kStream, 
    const NiFixedString& kName, NiGPUProgramPtr& spProgram, bool bSkip)
{
    spProgram = NULL;
    unsigned int uiCodeSize = 0;

    char acName[_MAX_PATH];
    LoadString(kStream, acName, _MAX_PATH);

    NiGPUProgram::ProgramType eType;
    if (NiStricmp(acName, NiD3D10VertexShader::ms_RTTI.GetName()) == 0)
    {
        eType = NiGPUProgram::PROGRAM_VERTEX;
    }
    else if (NiStricmp(acName, NiD3D10GeometryShader::ms_RTTI.GetName()) == 0)
    {
        eType = NiGPUProgram::PROGRAM_GEOMETRY;
    }
    else if (NiStricmp(acName, NiD3D10PixelShader::ms_RTTI.GetName()) == 0)
    {
        eType = NiGPUProgram::PROGRAM_PIXEL;
    }
    else
    {
        return false;
    }

    NiStreamLoadBinary(kStream, uiCodeSize);
    NIASSERT(uiCodeSize > 0);

    if (bSkip)
    {
        kStream.Seek(uiCodeSize);
        return true;
    }

    ID3D10Blob* pkShaderByteCode = NULL;
    HRESULT hr = 
        NiD3D10Renderer::D3D10CreateBlob(uiCodeSize, &pkShaderByteCode);
    if (FAILED(hr) || pkShaderByteCode == NULL)
    {
        if (pkShaderByteCode)
            pkShaderByteCode->Release();
        return false;
    }

    NiStreamLoadBinary(kStream, (char*)pkShaderByteCode->GetBufferPointer(), 
        uiCodeSize);

    char acFilename[_MAX_PATH];
    NiSprintf(acFilename, _MAX_PATH, "%s.%s", kName, 
        GetPlatformSpecificCodeID());

    if (eType == NiGPUProgram::PROGRAM_VERTEX)
    {
        NiD3D10VertexShaderPtr spVSProgram;
        NiD3D10ShaderProgramFactory::CreateVertexShaderFromCompiledBlob(
            pkShaderByteCode, acFilename, NULL, NULL, "Main",
            GetShaderProfile(), 0, kName, spVSProgram);
        spProgram = spVSProgram;
    }
    else if (eType == NiGPUProgram::PROGRAM_GEOMETRY)
    {
        NiD3D10GeometryShaderPtr spGSProgram;
        NiD3D10ShaderProgramFactory::CreateGeometryShaderFromCompiledBlob(
            pkShaderByteCode, acFilename, NULL, NULL, "Main",
            GetShaderProfile(), 0, kName, spGSProgram);
        spProgram = spGSProgram;
    }
    else
    {
        NIASSERT(eType == NiGPUProgram::PROGRAM_PIXEL);
        NiD3D10PixelShaderPtr spPSProgram;
        NiD3D10ShaderProgramFactory::CreatePixelShaderFromCompiledBlob(
            pkShaderByteCode, acFilename, NULL, NULL, "Main",
            GetShaderProfile(), 0, kName, spPSProgram);
        spProgram = spPSProgram;
    }

    pkShaderByteCode->Release();
    
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10GPUProgramCache::SaveDebugHLSLFile(const char* pcFilename, 
    const char* pcProgramText)
{
    // Validate directory will validate the working directory
    if (ValidateDirectory())
    {      
        // Now the material identifier directory needs to be validated.
        char acPath[NI_MAX_PATH];
        NiStrcpy(acPath, NI_MAX_PATH, pcFilename);
        NiPath::Standardize(acPath);
        char* pcPathEnd = strrchr(acPath, '/');
        if (pcPathEnd)
        {
            pcPathEnd[0] = '\0';
        }
        else
        {
            pcPathEnd = strrchr(acPath, '\\');
            if (pcPathEnd)
                pcPathEnd[0] = '\0';
        }

        if (!NiFile::DirectoryExists(acPath) && 
            !NiFile::CreateDirectoryRecursive(acPath))
        {
            return false;
        }

        // Write out the HLSL file
        NiOutputDebugString(pcFilename);
        NiOutputDebugString("(0)\n");
        NiFile* pkFile = NiFile::GetFile(pcFilename, NiFile::WRITE_ONLY);
        if (pkFile)
        {
            pkFile->Write(pcProgramText,
                (unsigned int) strlen(pcProgramText));
            NiDelete pkFile;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
