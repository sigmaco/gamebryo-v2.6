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
#include "NiD3D10EffectShaderLibPCH.h"

#include "NiD3D10EffectShaderLibrary.h"

#include "NiD3D10EffectFile.h"
#include "NiD3D10EffectLoader.h"
#include <NiShaderLibraryDesc.h>
#include <NiD3D10Renderer.h>

NiD3D10EffectShaderLibrary* NiD3D10EffectShaderLibrary::ms_pkLibrary = 0;

//---------------------------------------------------------------------------
NiD3D10EffectShaderLibrary::NiD3D10EffectShaderLibrary() :
    NiD3D10ShaderLibrary("NiD3D10EffectShaderLib"),
    m_pkTempDevice(NULL)
{
    ms_pkLibrary = this;
}
//---------------------------------------------------------------------------
NiD3D10EffectShaderLibrary::~NiD3D10EffectShaderLibrary()
{
    NiOutputDebugString("Releasing D3D10Effect Shaders!\n");

    ReleaseTempDevice();

    NiTListIterator kIter = m_kD3D10EffectFileList.GetHeadPos();
    while (kIter)
    {
        NiD3D10EffectFile* pkFile = m_kD3D10EffectFileList.RemovePos(kIter);
        NiDelete pkFile;
    }

    ms_pkLibrary = NULL;
}
//---------------------------------------------------------------------------
NiD3D10EffectShaderLibrary* NiD3D10EffectShaderLibrary::Create(
    NiRenderer* pkRenderer, NiUInt32 uiDirectoryCount, 
    const char* apcDirectories[], bool bRecurseSubFolders) 
{
    if (pkRenderer && 
        pkRenderer->GetRendererID() != NiSystemDesc::RENDERER_D3D10)
    {
        pkRenderer = NULL;
    }

    ID3D10Device* pkTempDevice = NULL;
    // Create temp device if necessary
    if (pkRenderer == NULL || !NiIsKindOf(NiD3D10Renderer, pkRenderer))
    {
        pkTempDevice = NiD3D10Renderer::CreateTempDevice(
            NULL,
            D3D10_DRIVER_TYPE_REFERENCE,
            0);
        if (pkTempDevice == NULL)
        {
            // May not be a D3D10-capable situation - can't load library at all
            // because the shaders can't be parsed.
            return NULL;
        }
    }

    NiD3D10EffectShaderLibrary* pkShaderLibrary = NULL;
    if (ms_pkLibrary)
        pkShaderLibrary = ms_pkLibrary;
    else
        pkShaderLibrary = NiNew NiD3D10EffectShaderLibrary();

    if (pkTempDevice)
    {
        pkShaderLibrary->SetTempDevice(pkTempDevice);
        pkTempDevice->Release();
    }

    if (pkShaderLibrary)
    {

        for (NiUInt32 i = 0; i < uiDirectoryCount; i++)
        {
            bool bSuccess = pkShaderLibrary->LoadD3D10EffectFiles(
                apcDirectories[i], bRecurseSubFolders);
            if (!bSuccess)
            {
                NILOG(
                    "%s> LoadD3D10EffectFiles failed on %s\n",
                    __FUNCTION__,
                    apcDirectories[i]);
            }
        }

        pkShaderLibrary->ReleaseTempDevice();

        if (pkShaderLibrary->GetD3D10EffectFileCount() == 0)
        {
            // Use a smart pointer to delete this when it goes out of scope
            NiD3D10EffectShaderLibraryPtr spLibrary = pkShaderLibrary;
            pkShaderLibrary = 0;
        }
    }

    return pkShaderLibrary;
}
//---------------------------------------------------------------------------
NiShader* NiD3D10EffectShaderLibrary::GetShader(NiRenderer* pkRenderer, 
    const char* pcName, unsigned int uiImplementation)
{
    // Only the D3D10 renderer is supported by this shader library.
    if (!NiIsKindOf(NiD3D10Renderer, pkRenderer))
        return NULL;

    NiFixedString kName(pcName);
    NiD3D10EffectTechniqueSet* pkTechniqueSet = NULL;
    if (!m_kD3D10EffectTechniqueMap.GetAt(kName, pkTechniqueSet) || 
        pkTechniqueSet == NULL)
    {
        return NULL;
    }

    return (NiShader*)pkTechniqueSet->GetD3D10Shader(uiImplementation);
}
//---------------------------------------------------------------------------
bool NiD3D10EffectShaderLibrary::ReleaseShader(const char* pcName, 
    unsigned int uiImplementation)
{
    NiFixedString kName(pcName);
    NiD3D10EffectTechniqueSet* pkTechniqueSet = NULL;
    if (!m_kD3D10EffectTechniqueMap.GetAt(kName, pkTechniqueSet) || 
        pkTechniqueSet== NULL)
    {
        return false;
    }

    NiD3D10EffectTechnique* pkTechnique = 
        pkTechniqueSet->GetTechnique(uiImplementation);
    if (pkTechnique == NULL)
        return false;

    return pkTechnique->ReleaseShader();
}
//---------------------------------------------------------------------------
bool NiD3D10EffectShaderLibrary::ReleaseShader(NiShader* pkShader)
{
    if (pkShader == NULL)
        return false;
    return ReleaseShader(pkShader->GetName(), pkShader->GetImplementation());
}
//---------------------------------------------------------------------------
bool NiD3D10EffectShaderLibrary::InsertD3D10EffectFileIntoList(
    NiD3D10EffectFile* pkD3D10EffectFile)
{
    if (!pkD3D10EffectFile)
        return false;

    NiD3D10EffectFile* pkCheckFile = 
        GetD3D10EffectFile(pkD3D10EffectFile->GetName());

    if (pkCheckFile)
    {
        NILOG(
            "%s> File %s already in list??\n", 
            __FUNCTION__,
            pkD3D10EffectFile->GetName());
        return false;
    }

    m_kD3D10EffectFileList.AddTail(pkD3D10EffectFile);

    // Add it to the description
    if (m_spShaderLibraryDesc == 0)
        m_spShaderLibraryDesc = NiNew NiShaderLibraryDesc();
    NIASSERT(m_spShaderLibraryDesc);

    NiShaderDesc* pkDesc = 
        pkD3D10EffectFile->GetFirstShaderDesc();

    while (pkDesc)
    {
        m_spShaderLibraryDesc->AddShaderDesc(pkDesc);

        m_spShaderLibraryDesc->AddPixelShaderVersion(
            pkDesc->GetMinPixelShaderVersion());
        m_spShaderLibraryDesc->AddPixelShaderVersion(
            pkDesc->GetMaxPixelShaderVersion());
        m_spShaderLibraryDesc->AddVertexShaderVersion(
            pkDesc->GetMinVertexShaderVersion());
        m_spShaderLibraryDesc->AddVertexShaderVersion(
            pkDesc->GetMaxVertexShaderVersion());
        m_spShaderLibraryDesc->AddUserDefinedVersion(
            pkDesc->GetMinUserDefinedVersion());
        m_spShaderLibraryDesc->AddUserDefinedVersion(
            pkDesc->GetMaxUserDefinedVersion());
        m_spShaderLibraryDesc->AddPlatformFlags(
            pkDesc->GetPlatformFlags());
        
        pkDesc = pkD3D10EffectFile->GetNextShaderDesc();
    };

    NiD3D10EffectTechnique* pkTech = pkD3D10EffectFile->GetFirstTechnique();
    while (pkTech)
    {
        NiFixedString kName(pkTech->GetShaderName());
        NiD3D10EffectTechniqueSet* pkTechniqueSet = 
            pkD3D10EffectFile->GetTechniqueSet(kName);
        m_kD3D10EffectTechniqueMap.SetAt(kName, 
            pkTechniqueSet);
        pkTech = pkD3D10EffectFile->GetNextTechnique();
    }

    return true;
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10EffectShaderLibrary::GetD3D10EffectFileCount()
{
    return m_kD3D10EffectFileList.GetSize();
}
//---------------------------------------------------------------------------
NiD3D10EffectFile* NiD3D10EffectShaderLibrary::GetD3D10EffectFile(
    const char* pcName)
{
    NiTListIterator kIter = m_kD3D10EffectFileList.GetHeadPos();
    while (kIter)
    {
        NiD3D10EffectFile* pkFile = m_kD3D10EffectFileList.GetNext(kIter);
        if (pkFile->GetName() == pcName)
            return pkFile;
    }
    return NULL;
}
//---------------------------------------------------------------------------
bool NiD3D10EffectShaderLibrary::LoadD3D10EffectFiles(const char* pcDirectory, 
    bool bRecurseSubFolders)
{
    bool bResult = true;

    NiD3D10EffectLoader* pkLoader = NiD3D10EffectLoader::GetInstance();
    NIASSERT(pkLoader);

    bResult = pkLoader->LoadAllD3D10EffectFiles(this, pcDirectory, 
        bRecurseSubFolders);
    
    return bResult;
}
//---------------------------------------------------------------------------
ID3D10Device* NiD3D10EffectShaderLibrary::GetTempDevice() const
{
    return m_pkTempDevice;
}
//---------------------------------------------------------------------------
void NiD3D10EffectShaderLibrary::SetTempDevice(ID3D10Device* pkDevice)
{
    if (m_pkTempDevice == pkDevice)
        return;
    ReleaseTempDevice();
    m_pkTempDevice = pkDevice;
    m_pkTempDevice->AddRef();
}
//---------------------------------------------------------------------------
void NiD3D10EffectShaderLibrary::ReleaseTempDevice()
{
    if (m_pkTempDevice)
    {
        // Release all renderer data
        NiTListIterator pkIter = m_kD3D10EffectFileList.GetHeadPos();
        while (pkIter)
        {
            NiD3D10EffectFile* pkFile = m_kD3D10EffectFileList.GetNext(pkIter);
            pkFile->ReleaseD3D10Effect();
        }

        m_pkTempDevice->Release();
        m_pkTempDevice = NULL;
    }
}
//---------------------------------------------------------------------------
