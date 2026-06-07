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
// NiInputGamePad inline functions
//---------------------------------------------------------------------------
inline unsigned int NiInputGamePad::GetMaxButtonCount()
{
    return ms_uiMaxButtonCount;
}
//---------------------------------------------------------------------------
inline void NiInputGamePad::SetMaxButtonCount(unsigned int uiCount)
{
    ms_uiMaxButtonCount = uiCount;
}
//---------------------------------------------------------------------------
inline unsigned int NiInputGamePad::GetMaxAxes()
{
    return ms_uiMaxAxes;
}
//---------------------------------------------------------------------------
inline void NiInputGamePad::SetMaxAxes(unsigned int uiCount)
{
    ms_uiMaxAxes = uiCount;
}
//---------------------------------------------------------------------------
inline int NiInputGamePad::GetDefaultAnalogThreshold()
{
    return ms_iDefaultAnalogThreshold;
}
//---------------------------------------------------------------------------
inline void NiInputGamePad::SetDefaultAnalogThreshold(int iThreshold)
{
    ms_iDefaultAnalogThreshold = iThreshold;
}
//---------------------------------------------------------------------------
inline float NiInputGamePad::GetDefaultAnalogDeadZone()
{
    return ms_fDefaultAnalogDeadZone;
}
//---------------------------------------------------------------------------
inline void NiInputGamePad::SetDefaultAnalogDeadZone(float fThreshold)
{
    ms_fDefaultAnalogDeadZone = fThreshold;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputGamePad::GetPadIndex() const
{
    return m_uiPadIndex;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputGamePad::GetStickInvertFlags() const
{
    return m_uiStickInvertFlags;
}
//---------------------------------------------------------------------------
inline void NiInputGamePad::SetStickInvertFlags(unsigned int uiFlags)
{
    m_uiStickInvertFlags = uiFlags;
}
//---------------------------------------------------------------------------
inline void NiInputGamePad::AddStickInvertFlag(
    NiInputGamePad::StickInvert eStickInvert)
{
    m_uiStickInvertFlags |= eStickInvert;
}
//---------------------------------------------------------------------------
inline void NiInputGamePad::RemoveStickInvertFlag(
    NiInputGamePad::StickInvert eStickInvert)
{
    m_uiStickInvertFlags &= ~eStickInvert;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputGamePad::GetButtonCount() const
{
    return m_uiButtonCount;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputGamePad::GetCurrentButtons() const
{
    return m_uiCurrentButtons;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputGamePad::GetNewButtons() const
{
    return m_uiNewButtons;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputGamePad::GetAxisCount() const
{
    return m_uiAxisCount;
}
//---------------------------------------------------------------------------
inline void NiInputGamePad::SetPadIndex(unsigned int uiIndex)
{
    m_uiPadIndex = uiIndex;
}
//---------------------------------------------------------------------------
inline void NiInputGamePad::SetAxisRange(int iLow, int iHigh)
{
    for (unsigned int i = 0; i < m_uiAxisCount; i++)
    {
        m_pkAxes[i].SetRange(iLow, iHigh);
    }
}
//---------------------------------------------------------------------------
inline void NiInputGamePad::GetAxisRange(int &iLow, int &iHigh) const
{
    iLow = iHigh = 0;
    if (m_uiAxisCount > 0)
    {
        m_pkAxes[0].GetRange(iLow, iHigh);
    }

#ifdef NIDEBUG
    for (unsigned int i = 1; i < m_uiAxisCount; i++)
    {
        int iLow2, iHigh2;
        m_pkAxes[i].GetRange(iLow2, iHigh2);
        NIASSERT(iLow == iLow2 && iHigh == iHigh2 && 
            "All axes should have same range");
    }
#endif
}
//---------------------------------------------------------------------------
inline void NiInputGamePad::SetButtonCount(unsigned int uiCount)
{
    m_uiButtonCount = uiCount;
}
//---------------------------------------------------------------------------
inline void NiInputGamePad::SetAxisCount(unsigned int uiCount)
{
    m_uiAxisCount = uiCount;
}
//---------------------------------------------------------------------------
