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
inline NiD3DShaderProgramCreator::NiD3DShaderProgramCreator()
{ /* */ }
//---------------------------------------------------------------------------
inline NiD3DShaderProgramCreator::~NiD3DShaderProgramCreator()
{ /* */ }
//---------------------------------------------------------------------------
inline unsigned int 
    NiD3DShaderProgramCreator::GetUniversalShaderCreationFlags()
{
    NiD3DRenderer* pkRenderer = NiD3DRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    return pkRenderer->GetGlobalShaderCreationFlags();
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreator::SetUniversalShaderCreationFlags(
    unsigned int uiFlags)
{
    NiD3DRenderer* pkRenderer = NiD3DRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    pkRenderer->SetGlobalShaderCreationFlags(uiFlags);
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreator::_SDMInit()
{ /* */ }
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreator::_SDMShutdown()
{
    SetD3DRenderer(NULL);
}
//---------------------------------------------------------------------------
