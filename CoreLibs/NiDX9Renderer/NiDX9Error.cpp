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

#include "NiDX9Error.h"

//---------------------------------------------------------------------------
char* NiDX9ErrorString(unsigned int uiErrorCode)
{
    switch (uiErrorCode)
    {
    case D3DERR_CONFLICTINGRENDERSTATE:
        return "D3DERR_CONFLICTINGRENDERSTATE";
    case D3DERR_CONFLICTINGTEXTUREFILTER:
        return "D3DERR_CONFLICTINGTEXTUREFILTER";
    case D3DERR_CONFLICTINGTEXTUREPALETTE:
        return "D3DERR_CONFLICTINGTEXTUREPALETTE";
    case D3DERR_DEVICELOST:
        return "D3DERR_DEVICELOST";
    case D3DERR_DEVICENOTRESET:
        return "D3DERR_DEVICENOTRESET";
    case D3DERR_DRIVERINTERNALERROR:
        return "D3DERR_DRIVERINTERNALERROR";
    case D3DERR_INVALIDCALL:
        return "D3DERR_INVALIDCALL";
    case D3DERR_INVALIDDEVICE:
        return "D3DERR_INVALIDDEVICE";
    case D3DERR_MOREDATA:
        return "D3DERR_MOREDATA";
    case D3DERR_NOTAVAILABLE:
        return "D3DERR_NOTAVAILABLE";
    case D3DERR_NOTFOUND:
        return "D3DERR_NOTFOUND";
    case D3DERR_OUTOFVIDEOMEMORY:
        return "D3DERR_OUTOFVIDEOMEMORY";
    case D3DERR_TOOMANYOPERATIONS:
        return "D3DERR_TOOMANYOPERATIONS";
    case D3DERR_UNSUPPORTEDALPHAARG:
        return "D3DERR_UNSUPPORTEDALPHAARG";
    case D3DERR_UNSUPPORTEDALPHAOPERATION:
        return "D3DERR_UNSUPPORTEDALPHAOPERATION";
    case D3DERR_UNSUPPORTEDCOLORARG:
        return "D3DERR_UNSUPPORTEDCOLORARG";
    case D3DERR_UNSUPPORTEDCOLOROPERATION:
        return "D3DERR_UNSUPPORTEDCOLOROPERATION";
    case D3DERR_UNSUPPORTEDFACTORVALUE:
        return "D3DERR_UNSUPPORTEDFACTORVALUE";
    case D3DERR_UNSUPPORTEDTEXTUREFILTER:
        return "D3DERR_UNSUPPORTEDTEXTUREFILTER";
    case D3DERR_WRONGTEXTUREFORMAT:
        return "D3DERR_WRONGTEXTUREFORMAT";
    case E_FAIL:
        return "E_FAIL";
    case E_INVALIDARG:
        return "E_INVALIDARG";
    case E_OUTOFMEMORY:
        return "E_OUTOFMEMORY";
    }

    return "UNKNOWN!";
}
