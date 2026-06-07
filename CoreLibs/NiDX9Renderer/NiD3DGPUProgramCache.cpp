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
#include "NiD3DRendererPCH.h"

#include "NiD3DGPUProgramCache.h"
#include "NiD3DHLSLPixelShader.h"
#include "NiD3DHLSLVertexShader.h"
#include "NiD3DShaderProgramCreatorHLSL.h"
#include "NiD3DShaderProgramFactory.h"
#include "NiDX9Headers.h"
#include <direct.h>

//---------------------------------------------------------------------------
NiD3DGPUProgramCache::NiD3DGPUProgramCache(unsigned int uiVersion, 
    const char* pcWorkingDir, NiGPUProgram::ProgramType eType, 
    const NiFixedString& kShaderProfile, const char* pcMaterialIdentifier,
    bool bAutoWriteCacheToDisk, bool bWriteDebugHLSLFile,
    bool bLocked, bool bLoad) : 
    NiGPUProgramCache(uiVersion, bAutoWriteCacheToDisk, 
    bLocked)
{
    m_bWriteDebugHLSLFile = bWriteDebugHLSLFile;
    m_eType = eType;
    m_kMaterialIdentifier = pcMaterialIdentifier;

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
        if (m_kShaderProfile.EqualsNoCase("vs_1_1"))
        {
            m_kValidTargets.Add("vs_1_1");
        }
        else if (m_kShaderProfile.EqualsNoCase("vs_2_0"))
        {
            m_kValidTargets.Add("vs_2_0");
            m_kValidTargets.Add("vs_1_1");
        }
        else if (m_kShaderProfile.EqualsNoCase("vs_2_a"))
        {
            m_kValidTargets.Add("vs_2_a");
            m_kValidTargets.Add("vs_2_0");
            m_kValidTargets.Add("vs_1_1");
        }
        else if (m_kShaderProfile.EqualsNoCase("vs_3_0"))
        {
            m_kValidTargets.Add("vs_3_0");
            m_kValidTargets.Add("vs_2_a");
            m_kValidTargets.Add("vs_2_0");
            m_kValidTargets.Add("vs_1_1");
        }
    }
    else if (eType == NiGPUProgram::PROGRAM_PIXEL)
    {
        if (m_kShaderProfile.EqualsNoCase("ps_1_1"))
        {
            m_kValidTargets.Add("ps_1_1");
        }
        else if (m_kShaderProfile.EqualsNoCase("ps_1_2"))
        {
            m_kValidTargets.Add("ps_1_2");
            m_kValidTargets.Add("ps_1_1");
        }
        else if (m_kShaderProfile.EqualsNoCase("ps_1_3"))
        {
            m_kValidTargets.Add("ps_1_3");
            m_kValidTargets.Add("ps_1_2");
            m_kValidTargets.Add("ps_1_1");
        }
        else if (m_kShaderProfile.EqualsNoCase("ps_1_4"))
        {
            m_kValidTargets.Add("ps_1_4");
            m_kValidTargets.Add("ps_1_3");
            m_kValidTargets.Add("ps_1_2");
            m_kValidTargets.Add("ps_1_1");
        }
        else if (m_kShaderProfile.EqualsNoCase("ps_2_0"))
        {
            m_kValidTargets.Add("ps_2_0");
            m_kValidTargets.Add("ps_1_4");
            m_kValidTargets.Add("ps_1_3");
            m_kValidTargets.Add("ps_1_2");
            m_kValidTargets.Add("ps_1_1");
        }
        else if (m_kShaderProfile.EqualsNoCase("ps_2_a"))
        {
            m_kValidTargets.Add("ps_2_a");
            m_kValidTargets.Add("ps_2_0");
            m_kValidTargets.Add("ps_1_4");
            m_kValidTargets.Add("ps_1_3");
            m_kValidTargets.Add("ps_1_2");
            m_kValidTargets.Add("ps_1_1");
        }
        else if (m_kShaderProfile.EqualsNoCase("ps_2_b"))
        {
            m_kValidTargets.Add("ps_2_b");
            m_kValidTargets.Add("ps_2_0");
            m_kValidTargets.Add("ps_1_4");
            m_kValidTargets.Add("ps_1_3");
            m_kValidTargets.Add("ps_1_2");
            m_kValidTargets.Add("ps_1_1");
        }
        else if (m_kShaderProfile.EqualsNoCase("ps_3_0"))
        {
            m_kValidTargets.Add("ps_3_0");
            m_kValidTargets.Add("ps_2_a");
            m_kValidTargets.Add("ps_2_0");
            m_kValidTargets.Add("ps_1_4");
            m_kValidTargets.Add("ps_1_3");
            m_kValidTargets.Add("ps_1_2");
            m_kValidTargets.Add("ps_1_1");
        }
    }

    if (bLoad && pcWorkingDir)
    {
        Load();
    }
}
//---------------------------------------------------------------------------
const char* NiD3DGPUProgramCache::GetPlatformSpecificCodeID() const
{
    return "hlsl";
}
//---------------------------------------------------------------------------
NiGPUProgram* NiD3DGPUProgramCache::GenerateProgram(const char* pcName, 
    const char* pcProgramText, 
    NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms)
{
    bool bDidNotCompilePreviously = false;
    NI_UNUSED_ARG(bDidNotCompilePreviously);
    NIASSERT(!FindCachedProgram(pcName, kUniforms, bDidNotCompilePreviously));
    NIASSERT(bDidNotCompilePreviously == false);
    if (m_bLocked)
        return NULL;

    char acFilename[NI_MAX_PATH];

    NiGPUProgramPtr spProgram;
    size_t stProgramLength = strlen(pcProgramText);

    size_t stNewProgramLength = stProgramLength + NI_MAX_PATH*2;
    char* pcNewProgramText = NiAlloc(char, stNewProgramLength);

#if defined(NIDEBUG)
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    NIASSERT(pkRenderer);
    NiUInt32 uiCurrentFlags = pkRenderer->GetGlobalShaderCreationFlags();
    pkRenderer->SetGlobalShaderCreationFlags(
        D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION);
#endif //#if defined(NIDEBUG)

    if (m_eType == NiGPUProgram::PROGRAM_VERTEX)
    {
        NiSprintf(acFilename, NI_MAX_PATH, "%s\\%s\\%s-V.%s",  
            GetWorkingDirectory(), m_kMaterialIdentifier, pcName, 
            GetPlatformSpecificCodeID());

        NiSprintf(pcNewProgramText, stNewProgramLength,"#line 2 \"%s\"\n%s", 
            acFilename, pcProgramText);

        spProgram = NiD3DShaderProgramFactory::CreateVertexShaderFromBuffer(
            pcNewProgramText, (unsigned int)strlen(pcNewProgramText),
            acFilename, acFilename, "Main", (const char*)m_kShaderProfile, 
            NULL, 0, true);
    }
    else if (m_eType == NiGPUProgram::PROGRAM_PIXEL)
    {
        NiSprintf(acFilename, NI_MAX_PATH, "%s\\%s\\%s-P.%s", 
            GetWorkingDirectory(), m_kMaterialIdentifier, pcName, 
            GetPlatformSpecificCodeID());
        
        NiSprintf(pcNewProgramText, stNewProgramLength, "#line 2 \"%s\"\n%s", 
            acFilename, pcProgramText);

        spProgram = NiD3DShaderProgramFactory::CreatePixelShaderFromBuffer(
            pcNewProgramText, (unsigned int)strlen(pcNewProgramText),
            acFilename, acFilename, "Main", (const char*)m_kShaderProfile, 
            true);
    }

    if (m_bWriteDebugHLSLFile)
    {           
        SaveDebugHLSLFile(acFilename, pcNewProgramText);
    }

#if defined(NIDEBUG)
    // Revert changed flags
    pkRenderer->SetGlobalShaderCreationFlags(uiCurrentFlags);
#endif //#if defined(NIDEBUG)

    if (spProgram && PruneUnusedConstants(spProgram, kUniforms))
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
    
    NiFree(pcNewProgramText);

    if (spProgram != NULL)
        return spProgram;

    kUniforms.RemoveAll();
    return NULL;
}
//---------------------------------------------------------------------------
const char* NiD3DGPUProgramCache::GetPlatform() const
{
    return "DX9";
}
//---------------------------------------------------------------------------
bool NiD3DGPUProgramCache::SaveGPUProgram(NiBinaryStream& kStream,
    NiGPUProgram* pkProgram)
{
    NiD3DShaderProgram* pkShader = NiSmartPointerCast(NiD3DShaderProgram,
        pkProgram);

    if (!pkShader)
        return false;

    const NiRTTI* pkRTTI = pkShader->GetRTTI();
    SaveString(kStream, pkRTTI->GetName());

    unsigned int uiCodeSize = pkShader->GetCodeSize();
    BYTE* pbCode = (BYTE*)pkShader->GetCode();

    NiStreamSaveBinary(kStream, uiCodeSize);
    NIASSERT(uiCodeSize > 0);
    NiStreamSaveBinary(kStream, pbCode, uiCodeSize);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DGPUProgramCache::LoadGPUProgram(NiBinaryStream& kStream, 
    const NiFixedString& kName, NiGPUProgramPtr& spProgram, bool bSkip)
{
    spProgram = NULL;
    unsigned int uiCodeSize = 0;

    char acName[256];
    LoadString(kStream, acName, 256);

    bool bHLSLVertex = false;
    if (NiStricmp(acName, NiD3DHLSLVertexShader::ms_RTTI.GetName()) == 0)
    {
        bHLSLVertex = true;
    }
    else if (NiStricmp(acName, NiD3DHLSLPixelShader::ms_RTTI.GetName()) == 0)
    {
        bHLSLVertex = false;
    }
    else
    {
        return false;
    }

    NiStreamLoadBinary(kStream, uiCodeSize);

    if (bSkip)
    {
        kStream.Seek(uiCodeSize);
        return true;
    }

    LPD3DXBUFFER pkCode = NULL;
    if (FAILED(D3DXCreateBuffer(uiCodeSize, &pkCode)))
    {
        return false;
    }

    NiStreamLoadBinary(kStream, (BYTE*)pkCode->GetBufferPointer(), uiCodeSize);

    NiD3DShaderProgramCreatorHLSL* pkCreator = 
        NiD3DShaderProgramCreatorHLSL::GetInstance();

    NIASSERT(pkCreator);

    if (bHLSLVertex)
    {
        spProgram = pkCreator->CreateVertexShaderFromD3DXBuffer(pkCode, 
            uiCodeSize, kName, "Main", GetShaderProfile(), 0, 0);
        if (spProgram)
        {
            NIASSERT(NiD3DShaderProgramFactory::GetInstance());
            NiD3DShaderProgramFactory::GetInstance()->
                InsertVertexShaderIntoMap(
                NiSmartPointerCast(NiD3DVertexShader, spProgram));
        }
    }
    else
    {
        spProgram = pkCreator->CreatePixelShaderFromD3DXBuffer(pkCode, 
            uiCodeSize, kName, "Main", GetShaderProfile());
        if (spProgram)
        {
            NIASSERT(NiD3DShaderProgramFactory::GetInstance());
            NiD3DShaderProgramFactory::GetInstance()->
                InsertPixelShaderIntoMap(
                NiSmartPointerCast(NiD3DPixelShader, spProgram));
        }
      
    }

    int iRelease = pkCode->Release();
    NI_UNUSED_ARG(iRelease);
    NIASSERT(iRelease == 0);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiD3DGPUProgramCache::SaveDebugHLSLFile(const char* pcFilename, 
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
            pkFile->Write(pcProgramText, (unsigned int)strlen(pcProgramText));
            NiDelete pkFile;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
