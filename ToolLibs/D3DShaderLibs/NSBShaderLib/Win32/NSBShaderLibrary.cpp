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

#include "NSBShaderLibrary.h"
#include "NSBLoader.h"
#include "NSBShader.h"
#include "NSBUtility.h"
#include <NiShaderLibraryDesc.h>
#include <NiD3DUtility.h>

//---------------------------------------------------------------------------
//NSBShaderLibraryPtr NSBShaderLibrary::ms_spShaderLibrary = 0;

//---------------------------------------------------------------------------
NSBShaderLibrary::NSBShaderLibrary() :
    NiD3DShaderLibrary("NSBShaderLib")
{
}
//---------------------------------------------------------------------------
NSBShaderLibrary::~NSBShaderLibrary()
{
    NiOutputDebugString("Releasing Binary Shaders!\n");

    m_kNSBShaderList.RemoveAll();
//    m_spShaderLibraryDesc = 0;
}
//---------------------------------------------------------------------------
NSBShaderLibrary* NSBShaderLibrary::Create(int iDirectoryCount, 
    const char* pacDirectories[], bool bRecurseSubFolders) 
{
    NSBShaderLibrary* pkShaderLibrary = NiNew NSBShaderLibrary();
    if (pkShaderLibrary)
    {
        for (int iCount = 0; iCount < iDirectoryCount; iCount++)
        {
            if (!pkShaderLibrary->LoadNSBShaders(
                pacDirectories[iCount], bRecurseSubFolders))
            {
                NiD3DUtility::Log("LoadNSBShaders failed on %s\n",
                    pacDirectories[iCount]);
            }
        }
    }

    if (pkShaderLibrary->GetNSBShaderCount() == 0)
    {
        NiDelete pkShaderLibrary;
        pkShaderLibrary = 0;
    }

    return pkShaderLibrary;
}
//---------------------------------------------------------------------------
void NSBShaderLibrary::Shutdown()
{
//    ms_spShaderLibrary = 0;
}
//---------------------------------------------------------------------------
NSBShaderLibrary* NSBShaderLibrary::GetLibrary()
{
//    return ms_spShaderLibrary;
    NIASSERT(!"Who is calling this???");
    return 0;
}
//---------------------------------------------------------------------------
NiShaderLibraryDesc* NSBShaderLibrary::GetShaderLibraryDesc()
{
    // Set it up if it isn't???

    return NiD3DShaderLibrary::GetShaderLibraryDesc();
}
//---------------------------------------------------------------------------
void NSBShaderLibrary::SetShaderLibraryDesc(NiShaderLibraryDesc* pkDesc)
{
    NiD3DShaderLibrary::SetShaderLibraryDesc(pkDesc);
}
//---------------------------------------------------------------------------
NiShader* NSBShaderLibrary::GetShader(NiRenderer*, 
    const char* pcName, unsigned int uiImplementation)
{
    NSBShader* pkNSBShader = GetNSBShader(pcName);
    if (pkNSBShader)
    {
        return pkNSBShader->GetBinaryShader(uiImplementation);
    }

    // Didn't exist? 
    return 0;
}
//---------------------------------------------------------------------------
bool NSBShaderLibrary::ReleaseShader(const char* pcName,
    unsigned int uiImplementation)
{
    NSBShader* pkNSBShader = GetNSBShader(pcName);
    if (pkNSBShader)
    {
        NSBImplementation* pkImplementation = 
            pkNSBShader->GetImplementationByIndex(uiImplementation);
        if (pkImplementation)
        {
            return pkImplementation->ReleaseShader();
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NSBShaderLibrary::ReleaseShader(NiShader* pkShader)
{
    if (pkShader == NULL)
        return false;
    return ReleaseShader(pkShader->GetName(), pkShader->GetImplementation());
}
 //---------------------------------------------------------------------------
bool NSBShaderLibrary::InsertNSBShaderIntoList(NSBShader* pkNSBShader)
{
    if (!pkNSBShader)
        return false;

    NSBShader* pkCheckShader = GetNSBShader(pkNSBShader->GetName());
    if (pkCheckShader)
    {
        NiD3DUtility::Log("Shader %s already in list??\n");
        return false;
    }

    m_kNSBShaderList.AddTail(pkNSBShader);

    // Add it to the description
    if (m_spShaderLibraryDesc == 0)
        m_spShaderLibraryDesc = NiNew NiShaderLibraryDesc();
    if (m_spShaderLibraryDesc)
    {
        NiShaderDesc* pkDesc = pkNSBShader->GetShaderDesc();
        NIASSERT(pkDesc);
        m_spShaderLibraryDesc->AddShaderDesc(pkDesc);

        m_spShaderLibraryDesc->AddVertexShaderVersion(
            pkDesc->GetMinVertexShaderVersion());
        m_spShaderLibraryDesc->AddVertexShaderVersion(
            pkDesc->GetMaxVertexShaderVersion());
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
unsigned int NSBShaderLibrary::GetNSBShaderCount()
{
    return m_kNSBShaderList.GetSize();
}
//---------------------------------------------------------------------------
NSBShader* NSBShaderLibrary::GetNSBShader(const char* pcName)
{
    NiTListIterator iter;

    iter = m_kNSBShaderList.GetHeadPos();
    while (iter)
    {
        NSBShader* pkNSBShader = m_kNSBShaderList.GetNext(iter);
        if (!pkNSBShader)
            continue;
        if (strcmp(pkNSBShader->GetName(), pcName) == 0)
        {
            // Found it
            return pkNSBShader;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
bool NSBShaderLibrary::LoadNSBShaders(const char* pcDirectory, 
    bool bRecurseSubFolders)
{
    bool bResult = true;

    NSBLoader* pkLoader = NSBLoader::Create();
    NIASSERT(pkLoader);

    bResult = pkLoader->LoadAllNSBFiles(this, pcDirectory, 
        bRecurseSubFolders);
    NSBLoader::Destroy();

    return bResult;
}
//---------------------------------------------------------------------------
