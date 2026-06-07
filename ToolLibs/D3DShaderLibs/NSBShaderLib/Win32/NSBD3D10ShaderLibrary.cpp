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
#include "NSBShaderLibPCH.h"

#include "NSBD3D10ShaderLibrary.h"
#include "NSBLoader.h"
#include <NSBD3D10Shader.h>
#include <NSBD3D10Utility.h>
#include <NiShaderLibraryDesc.h>

//---------------------------------------------------------------------------
//NSBD3D10ShaderLibraryPtr NSBD3D10ShaderLibrary::ms_spShaderLibrary = 0;

//---------------------------------------------------------------------------
NSBD3D10ShaderLibrary::NSBD3D10ShaderLibrary() :
    NiD3D10ShaderLibrary("NSBShaderLib")
{
}
//---------------------------------------------------------------------------
NSBD3D10ShaderLibrary::~NSBD3D10ShaderLibrary()
{
    NiOutputDebugString("Releasing Binary Shaders!\n");

    m_kNSBD3D10ShaderList.RemoveAll();
//    m_spShaderLibraryDesc = 0;
}
//---------------------------------------------------------------------------
NSBD3D10ShaderLibrary* NSBD3D10ShaderLibrary::Create(int iDirectoryCount, 
    const char* pacDirectories[], bool bRecurseSubFolders) 
{
    NSBD3D10ShaderLibrary* pkShaderLibrary = NiNew NSBD3D10ShaderLibrary();
    if (pkShaderLibrary)
    {
        for (int iCount = 0; iCount < iDirectoryCount; iCount++)
        {
            if (!pkShaderLibrary->LoadNSBD3D10Shaders(
                pacDirectories[iCount], bRecurseSubFolders))
            {
                NSBD3D10Utility::Log("LoadNSBD3D10Shaders failed on %s\n",
                    pacDirectories[iCount]);
            }
        }
    }

    if (pkShaderLibrary->GetNSBD3D10ShaderCount() == 0)
    {
        NiDelete pkShaderLibrary;
        pkShaderLibrary = 0;
    }

    return pkShaderLibrary;
}
//---------------------------------------------------------------------------
void NSBD3D10ShaderLibrary::Shutdown()
{
//    ms_spShaderLibrary = 0;
}
//---------------------------------------------------------------------------
NSBD3D10ShaderLibrary* NSBD3D10ShaderLibrary::GetLibrary()
{
//    return ms_spShaderLibrary;
    NIASSERT(!"Who is calling this???");
    return 0;
}
//---------------------------------------------------------------------------
NiShaderLibraryDesc* NSBD3D10ShaderLibrary::GetShaderLibraryDesc()
{
    // Set it up if it isn't???

    return NiD3D10ShaderLibrary::GetShaderLibraryDesc();
}
//---------------------------------------------------------------------------
void NSBD3D10ShaderLibrary::SetShaderLibraryDesc(NiShaderLibraryDesc* pkDesc)
{
    NiD3D10ShaderLibrary::SetShaderLibraryDesc(pkDesc);
}
//---------------------------------------------------------------------------
NiShader* NSBD3D10ShaderLibrary::GetShader(NiRenderer*, 
    const char* pcName, unsigned int uiImplementation)
{
    NSBD3D10Shader* pkNSBD3D10Shader = GetNSBD3D10Shader(pcName);
    if (pkNSBD3D10Shader)
    {
        return (NiShader*)(pkNSBD3D10Shader->GetD3D10Shader(uiImplementation));
    }

    // Didn't exist? 
    return 0;
}
//---------------------------------------------------------------------------
bool NSBD3D10ShaderLibrary::ReleaseShader(const char* pcName,
    unsigned int uiImplementation)
{
    NSBD3D10Shader* pkNSBD3D10Shader = GetNSBD3D10Shader(pcName);
    if (pkNSBD3D10Shader)
    {
        NSBD3D10Implementation* pkImplementation = 
            pkNSBD3D10Shader->GetImplementationByIndex(uiImplementation);
        if (pkImplementation)
        {
            return pkImplementation->ReleaseShader();
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NSBD3D10ShaderLibrary::ReleaseShader(NiShader* pkShader)
{
    if (pkShader == NULL)
        return false;
    return ReleaseShader(pkShader->GetName(), pkShader->GetImplementation());
}
 //---------------------------------------------------------------------------
bool NSBD3D10ShaderLibrary::InsertNSBD3D10ShaderIntoList(
    NSBD3D10Shader* pkNSBD3D10Shader)
{
    if (!pkNSBD3D10Shader)
        return false;

    NSBD3D10Shader* pkCheckShader
        = GetNSBD3D10Shader(pkNSBD3D10Shader->GetName());
    if (pkCheckShader)
    {
        NSBD3D10Utility::Log("Shader %s already in list??\n");
        return false;
    }

    m_kNSBD3D10ShaderList.AddTail(pkNSBD3D10Shader);

    // Add it to the description
    if (m_spShaderLibraryDesc == 0)
        m_spShaderLibraryDesc = NiNew NiShaderLibraryDesc();
    if (m_spShaderLibraryDesc)
    {
        NiShaderDesc* pkDesc = pkNSBD3D10Shader->GetShaderDesc();
        NIASSERT(pkDesc);
        m_spShaderLibraryDesc->AddShaderDesc(pkDesc);

        m_spShaderLibraryDesc->AddVertexShaderVersion(
            pkDesc->GetMinVertexShaderVersion());
        m_spShaderLibraryDesc->AddVertexShaderVersion(
            pkDesc->GetMaxVertexShaderVersion());
        m_spShaderLibraryDesc->AddGeometryShaderVersion(
            pkDesc->GetMinGeometryShaderVersion());
        m_spShaderLibraryDesc->AddGeometryShaderVersion(
            pkDesc->GetMaxGeometryShaderVersion());
        m_spShaderLibraryDesc->AddPixelShaderVersion(
            pkDesc->GetMinPixelShaderVersion());
        m_spShaderLibraryDesc->AddPixelShaderVersion(
            pkDesc->GetMaxPixelShaderVersion());
        m_spShaderLibraryDesc->AddUserDefinedVersion(
            pkDesc->GetMinUserDefinedVersion());
        m_spShaderLibraryDesc->AddUserDefinedVersion(
            pkDesc->GetMaxUserDefinedVersion());
        m_spShaderLibraryDesc->AddPlatformFlags(
            pkDesc->GetPlatformFlags());
    }

    return true;
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10ShaderLibrary::GetNSBD3D10ShaderCount()
{
    return m_kNSBD3D10ShaderList.GetSize();
}
//---------------------------------------------------------------------------
NSBD3D10Shader* NSBD3D10ShaderLibrary::GetNSBD3D10Shader(const char* pcName)
{
    NiTListIterator iter;

    iter = m_kNSBD3D10ShaderList.GetHeadPos();
    while (iter)
    {
        NSBD3D10Shader* pkNSBD3D10Shader
            = m_kNSBD3D10ShaderList.GetNext(iter);
        if (!pkNSBD3D10Shader)
            continue;
        if (strcmp(pkNSBD3D10Shader->GetName(), pcName) == 0)
        {
            // Found it
            return pkNSBD3D10Shader;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
bool NSBD3D10ShaderLibrary::LoadNSBD3D10Shaders(const char* pcDirectory, 
    bool bRecurseSubFolders)
{
    bool bResult = true;

    NSBLoader* pkLoader = NSBLoader::Create();
    NIASSERT(pkLoader);

    bResult = pkLoader->LoadAllNSBD3D10Files(this, pcDirectory,
        bRecurseSubFolders);
    NSBLoader::Destroy();

    return bResult;
}
//---------------------------------------------------------------------------
