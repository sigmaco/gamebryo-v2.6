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
inline void NiRenderView::_SDMInit()
{
    ms_kDefaultName = "Render View";
}
//---------------------------------------------------------------------------
inline void NiRenderView::_SDMShutdown()
{
    ms_kDefaultName = NULL;
}
//---------------------------------------------------------------------------
inline NiRenderView::NiRenderView() : m_kCachedPVGeometry(16, 16),
    m_uiLastFrameID((unsigned int) FORCE_PV_GEOMETRY_UPDATE), 
    m_kName(ms_kDefaultName)
{
}
//---------------------------------------------------------------------------
inline void NiRenderView::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiRenderView::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiRenderView::ClearCachedPVGeometry()
{
    m_kCachedPVGeometry.RemoveAll();
    m_uiLastFrameID = (unsigned int) FORCE_PV_GEOMETRY_UPDATE;
}
//---------------------------------------------------------------------------
