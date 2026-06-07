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
#include "NiD3DXEffectShaderLibPCH.h"

#include "NiD3DXInclude.h"
#include "NiD3DXEffectLoader.h"

//---------------------------------------------------------------------------
void NiD3DXInclude::SetBasePath(const char* pcBasePath)
{
    NIASSERT(pcBasePath);
    NiFilename kFilename(pcBasePath);
    NiStrcpy(m_acBasePath, NI_MAX_PATH, kFilename.GetDrive());
    NiStrcat(m_acBasePath, NI_MAX_PATH, kFilename.GetDir());
}
//---------------------------------------------------------------------------
HRESULT NiD3DXInclude::Open(D3DXINCLUDE_TYPE, LPCSTR pFileName,
    LPCVOID, LPCVOID *ppData, UINT *pBytes
#if defined(_XENON)
    , LPSTR pFullPath, DWORD cbFullPath)
#else
    )
#endif
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
    CHAR* pcFileBuffer = NiD3DXEffectLoader::GetShaderText(
        acAbsolutePath, dwSize);

    *ppData = (VOID*) pcFileBuffer;
    *pBytes = dwSize;

#if defined(_XENON)
    pFullPath = NULL;
    cbFullPath = 0;
#endif

    return S_OK;
}
//---------------------------------------------------------------------------
HRESULT NiD3DXInclude::Close(LPCVOID pData)
{
    NiFree((void*)pData);
    return S_OK;
}
//---------------------------------------------------------------------------
