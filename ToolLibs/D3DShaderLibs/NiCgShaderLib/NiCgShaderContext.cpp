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
#include "NiCgShaderLibPCH.h"

#include "NiCgShaderContext.h"
#include <NiSystem.h>

CGcontext NiCgShaderContext::ms_kContext = NULL;
unsigned int NiCgShaderContext::ms_uiRefCount = 0;

//---------------------------------------------------------------------------
CGcontext NiCgShaderContext::GetCgContext()
{
    return ms_kContext;
}
//---------------------------------------------------------------------------
void NiCgShaderContext::_SDMInit()
{
    if (ms_kContext == NULL)
        ms_kContext = cgCreateContext();
    ms_uiRefCount++;
}
//---------------------------------------------------------------------------
void NiCgShaderContext::_SDMShutdown()
{
    if (ms_kContext)
    {
        NIASSERT(ms_uiRefCount > 0);
        if (--ms_uiRefCount == 0)
        {
            cgDestroyContext(ms_kContext);
            ms_kContext = NULL;
        }
    }
}
//---------------------------------------------------------------------------
