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

#include "NiD3D10Include.h"

#include "NiD3D10EffectLoader.h"

//---------------------------------------------------------------------------
void NiD3D10Include::SetBasePath(const char* pcBasePath)
{
    char acAbsolutePath[NI_MAX_PATH];

    NIASSERT(pcBasePath);
    // In case pcBasePath is relative, we must get an absolute path so
    // m_acBasePath is a valid absolute path.
    if (NiPath::IsRelative(pcBasePath))
    {
        NiPath::ConvertToAbsolute(acAbsolutePath, NI_MAX_PATH, pcBasePath,
            NULL);
    }
    else
    {
        NiStrcpy(acAbsolutePath, NI_MAX_PATH, pcBasePath);
    }

    NiFilename kFilename(acAbsolutePath);
    NiStrcpy(m_acBasePath, NI_MAX_PATH, kFilename.GetDrive());
    NiStrcat(m_acBasePath, NI_MAX_PATH, kFilename.GetDir());
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Include::Open(D3D10_INCLUDE_TYPE, LPCSTR pFileName, 
    LPCVOID, LPCVOID *ppData, UINT *pBytes)
{
    char acAbsolutePath[NI_MAX_PATH];

    NIASSERT(pFileName);
    if (NiPath::IsRelative(pFileName))
    {
        NiPath::ConvertToAbsolute(acAbsolutePath, NI_MAX_PATH, pFileName,
            m_acBasePath);
    }
    else
    {
        NiStrcpy(acAbsolutePath, NI_MAX_PATH, pFileName);
    }
    NiPath::RemoveDotDots(acAbsolutePath);

    UINT dwSize = 0;
    CHAR* pcFileBuffer = NiD3D10EffectLoader::GetShaderText(
        acAbsolutePath, dwSize);

    *ppData = (VOID*) pcFileBuffer;
    *pBytes = dwSize;

    return S_OK;
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Include::Close(LPCVOID pData)
{
    NiFree((void*)pData);
    return S_OK;
}
//---------------------------------------------------------------------------
