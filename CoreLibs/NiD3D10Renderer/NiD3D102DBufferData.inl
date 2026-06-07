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
// Abstract base class designed to represent all rendering targets
  
//---------------------------------------------------------------------------
inline NiD3D102DBufferData::NiD3D102DBufferData() :
    m_pkTexture(NULL)
{
    /* */
}
//---------------------------------------------------------------------------
inline ID3D10RenderTargetView* NiD3D10RenderTargetBufferData::GetRenderTargetView()
    const
{
    return m_pkRenderTargetView;
}
//---------------------------------------------------------------------------
inline ID3D10Texture2D* NiD3D10RenderTargetBufferData::GetRenderTargetBuffer() const
{
    return m_pkTexture;
}
//---------------------------------------------------------------------------
inline NiD3D10SwapChainBufferData::NiD3D10SwapChainBufferData() :
    m_pkSwapChain(NULL)
{
    /* */
}
