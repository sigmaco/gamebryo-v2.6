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
inline NiD3DShaderProgramCreatorCg::NiD3DShaderProgramCreatorCg() :
    m_pcShaderTargetTemp(NULL),
    m_uiShaderTargetTempSize(0),
    m_pvConstantBuffer(NULL),
    m_uiConstantBufferSize(0)
{ /* */ }
//---------------------------------------------------------------------------
inline NiD3DShaderProgramCreatorCg::~NiD3DShaderProgramCreatorCg()
{
    NiFree(m_pvConstantBuffer);
    NiFree(m_pcShaderTargetTemp);
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DShaderProgramCreatorCg::GetShaderCreationFlags()
    const
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    return pkRenderer->GetShaderCreationFlags("cg");
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreatorCg::SetShaderCreationFlags(
    unsigned int uiFlags)
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    pkRenderer->SetShaderCreationFlags("cg", uiFlags);
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreatorCg::_SDMInit()
{
    ms_pkCreator = NiNew NiD3DShaderProgramCreatorCg();

    NiD3DShaderProgramFactory::RegisterShaderCreator("cg", ms_pkCreator);
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreatorCg::_SDMShutdown()
{
    Shutdown();
}
//---------------------------------------------------------------------------
inline NiD3DShaderProgramCreatorCg* 
    NiD3DShaderProgramCreatorCg::GetInstance()
{
    return ms_pkCreator;
}
//---------------------------------------------------------------------------
inline void NiD3DShaderProgramCreatorCg::Shutdown()
{
    if (ms_pkCreator)
        NiD3DShaderProgramFactory::UnregisterShaderCreator(ms_pkCreator);
    NiDelete ms_pkCreator;
    ms_pkCreator = NULL;
}
//---------------------------------------------------------------------------
