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
inline NiD3DShaderProgramCreatorCgObj::NiD3DShaderProgramCreatorCgObj()
{ /* */ }
//---------------------------------------------------------------------------
inline NiD3DShaderProgramCreatorCgObj::~NiD3DShaderProgramCreatorCgObj()
{ /* */ }
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreatorCgObj::_SDMInit()
{
    ms_pkCreator = NiNew NiD3DShaderProgramCreatorCgObj();

    NiD3DShaderProgramFactory::RegisterShaderCreator("cgo", ms_pkCreator);
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreatorCgObj::_SDMShutdown()
{
    Shutdown();
}
//---------------------------------------------------------------------------
inline NiD3DShaderProgramCreatorCgObj* 
    NiD3DShaderProgramCreatorCgObj::GetInstance()
{
    return ms_pkCreator;
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreatorCgObj::Shutdown()
{
    if (ms_pkCreator)
        NiD3DShaderProgramFactory::UnregisterShaderCreator(ms_pkCreator);
    NiDelete ms_pkCreator;
    ms_pkCreator = NULL;
}
//---------------------------------------------------------------------------
