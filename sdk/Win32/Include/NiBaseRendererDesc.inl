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
// NiRendererSettings pointer retrieval
//---------------------------------------------------------------------------
inline NiRendererSettings* NiBaseRendererDesc::GetSettings()
{
    return NiRendererSettings::GetInstance();
}

//---------------------------------------------------------------------------
// Renderer selection methods
//---------------------------------------------------------------------------
inline void NiBaseRendererDesc::Activate()
{
    // Store thar our renderer is selected
    GetSettings()->m_bD3D10Renderer = IsD3D10RendererDesc();
}

//---------------------------------------------------------------------------
inline bool NiBaseRendererDesc::IsActive()
{
    // Check if D3D10 renderer is selected
    return GetSettings()->m_bD3D10Renderer;
}

//---------------------------------------------------------------------------
// VSync functions
//---------------------------------------------------------------------------
inline bool NiBaseRendererDesc::GetDefaultVSync()
{
    return GetSettings()->m_bVSync;
}

//---------------------------------------------------------------------------
inline void NiBaseRendererDesc::SelectVSync(bool bVSync)
{
    GetSettings()->m_bVSync = bVSync;
}
