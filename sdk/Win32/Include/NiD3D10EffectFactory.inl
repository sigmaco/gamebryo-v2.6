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
inline NiUInt32 NiD3D10EffectFactory::GetD3D10EffectHLSLFlags()
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    return pkRenderer->GetShaderCreationFlags("fx");
}
//---------------------------------------------------------------------------
inline void NiD3D10EffectFactory::SetD3D10EffectHLSLFlags(NiUInt32 uiHLSLFlags)
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    pkRenderer->SetShaderCreationFlags("fx", uiHLSLFlags);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiD3D10EffectFactory::GetD3D10EffectFXFlags()
{
    return ms_uiFXFlags;
}
//---------------------------------------------------------------------------
inline void NiD3D10EffectFactory::SetD3D10EffectFXFlags(NiUInt32 uiFXFlags)
{
    ms_uiFXFlags = uiFXFlags;
}
//---------------------------------------------------------------------------
inline bool NiD3D10EffectFactory::GetFXLSupport()
{
    return ms_bFXLSupport;
}
//---------------------------------------------------------------------------
