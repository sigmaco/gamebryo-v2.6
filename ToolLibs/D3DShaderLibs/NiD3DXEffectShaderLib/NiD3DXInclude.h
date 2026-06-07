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

#ifndef NID3DXINCLUDE_H
#define NID3DXINCLUDE_H

#include <NiD3DDefines.h>
#include <NiSystem.h>
#include <NiFilename.h>

#include "NiD3DXEffectShaderLibType.h"

class NID3DXEFFECTSHADER_ENTRY NiD3DXInclude : public ID3DXInclude
{
public:
    void SetBasePath(const char* pcBasePath);

    // We must declare these as STDMETHOD to comply with DX.
    virtual HRESULT STDMETHODCALLTYPE Open(D3DXINCLUDE_TYPE IncludeType, 
        LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes
#if defined(_XENON)
        , LPSTR pFullPath, DWORD cbFullPath);
#else
        );
#endif
    virtual HRESULT STDMETHODCALLTYPE Close(LPCVOID pData);

protected:
    char m_acBasePath[NI_MAX_PATH];
};

#endif  // #ifndef NID3DXINCLUDE_H
