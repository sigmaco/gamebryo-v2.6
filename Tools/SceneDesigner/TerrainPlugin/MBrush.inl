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

inline float MBrush::GetSizeInner()
{
    if(GetBrushType())
    {
        return ms_pmRegisteredBrushTypes->get_Item(m_uiCurrentBrushType)->
            GetSizeInner();
    }
    else
    {
        return 0.0f;
    }
}
//---------------------------------------------------------------------------
inline float MBrush::GetSizeOuter()
{
    if(GetBrushType())
    {
        return ms_pmRegisteredBrushTypes->get_Item(m_uiCurrentBrushType)->
            GetSizeOuter();
    }
    else
    {
        return 0.0f;
    }
}
//---------------------------------------------------------------------------
inline void MBrush::SetScale(float fScale)
{
    if (GetBrushType() && m_fCurrentScale != fScale)
    {
        m_fCurrentScale = fScale;
        ms_pmRegisteredBrushTypes->get_Item(m_uiCurrentBrushType)->
            ResizeBrushOverlay(fScale);
    }
}
//---------------------------------------------------------------------------
inline float MBrush::GetScale()
{
    return m_fCurrentScale;
}
//---------------------------------------------------------------------------
inline void MBrush::SetUpdateLightingRealtime(bool bValue)
{
    m_bUpdateLightingRealtime = bValue;
}
//---------------------------------------------------------------------------