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
inline NiShadowRenderClick::NiShadowRenderClick() :
    m_pkGenerator(NULL), m_uFlags(0),
    m_uiLastRenderedFrameID(0)
{
    // Ensure that the shadow map associated with this click is always
    // filled with the maximum possible depth when being cleared.
    m_kBackgroudColor = NiColorA(1.0f, 1.0f, 1.0f, 1.0f);
    SetUseRendererBackgroundColor(false);

    // Click begins in dirty state.
    MarkAsDirty();
}
//---------------------------------------------------------------------------
inline NiShadowRenderClick::~NiShadowRenderClick()
{
    /* */
}
//---------------------------------------------------------------------------
inline void NiShadowRenderClick::SetGenerator(NiShadowGenerator* pkGenerator)
{
    m_pkGenerator = pkGenerator;
}
//---------------------------------------------------------------------------
inline NiShadowGenerator* NiShadowRenderClick::GetGenerator() const
{
    return m_pkGenerator;
}
//---------------------------------------------------------------------------
inline void NiShadowRenderClick::SetForceRender(bool bForceRender)
{
    SetBit(bForceRender, FORCE_RENDER);
}
//---------------------------------------------------------------------------
inline bool NiShadowRenderClick::GetForceRender() const
{
    return GetBit(FORCE_RENDER);
}
//---------------------------------------------------------------------------
inline void NiShadowRenderClick::MarkAsDirty()
{
    SetBit(true, DIRTY);
}
//---------------------------------------------------------------------------
inline bool NiShadowRenderClick::IsDirty() const
{
    return GetBit(DIRTY);
}
//---------------------------------------------------------------------------
inline unsigned int NiShadowRenderClick::GetLastRenderedFrameID()
{
    return m_uiLastRenderedFrameID;
}
//---------------------------------------------------------------------------
