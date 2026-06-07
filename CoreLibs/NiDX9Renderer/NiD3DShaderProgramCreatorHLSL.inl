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
inline NiD3DShaderProgramCreatorHLSL::NiD3DShaderProgramCreatorHLSL()
{ /* */ }
//---------------------------------------------------------------------------
inline NiD3DShaderProgramCreatorHLSL::~NiD3DShaderProgramCreatorHLSL()
{ /* */ }
//---------------------------------------------------------------------------
inline unsigned int NiD3DShaderProgramCreatorHLSL::GetShaderCreationFlags()
    const
{
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    NIASSERT(pkRenderer);
    return pkRenderer->GetShaderCreationFlags("hlsl");
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreatorHLSL::SetShaderCreationFlags(
    unsigned int uiFlags)
{
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    NIASSERT(pkRenderer);
    pkRenderer->SetShaderCreationFlags("hlsl", uiFlags);
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreatorHLSL::_SDMInit()
{
    ms_pkCreator = NiNew NiD3DShaderProgramCreatorHLSL();

    NiD3DShaderProgramFactory::RegisterShaderCreator("hlsl", ms_pkCreator);
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreatorHLSL::_SDMShutdown()
{
    Shutdown();
}
//---------------------------------------------------------------------------
inline NiD3DShaderProgramCreatorHLSL* 
    NiD3DShaderProgramCreatorHLSL::GetInstance()
{
    return ms_pkCreator;
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreatorHLSL::Shutdown()
{
    if (ms_pkCreator)
        NiD3DShaderProgramFactory::UnregisterShaderCreator(ms_pkCreator);
    NiDelete ms_pkCreator;
    ms_pkCreator = NULL;
}
//---------------------------------------------------------------------------
