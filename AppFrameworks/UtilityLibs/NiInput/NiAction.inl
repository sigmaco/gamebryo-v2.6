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
// NiAction inline functions
//---------------------------------------------------------------------------
inline const char* NiAction::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline const unsigned int NiAction::GetAppData() const
{
    return m_uiAppData;
}
//---------------------------------------------------------------------------
inline const unsigned int NiAction::GetSemantic() const
{
    return m_uiSemantic;
}
//---------------------------------------------------------------------------
inline const unsigned int NiAction::GetFlags() const
{
    return m_uiFlags;
}
//---------------------------------------------------------------------------
inline const unsigned int NiAction::GetDeviceID() const
{
    return m_uiDeviceID;
}
//---------------------------------------------------------------------------
inline const void NiAction::GetRange(int& iLow, int& iHigh) const
{
    iLow = m_iRangeLow;
    iHigh = m_iRangeHigh;
}
//---------------------------------------------------------------------------
inline void* NiAction::GetContext()
{
    return m_pvContext;
}
//---------------------------------------------------------------------------
inline const unsigned int NiAction::GetKeyModifiers() const
{
    return m_uiKeyModifiers;
}
//---------------------------------------------------------------------------
inline const unsigned int NiAction::GetMouseModifiers() const
{
    return m_uiMouseModifiers;
}
//---------------------------------------------------------------------------
inline const unsigned int NiAction::GetPadModifiers() const
{
    return m_uiPadModifiers;
}
//---------------------------------------------------------------------------
inline const unsigned int NiAction::GetControl() const
{
    return m_uiControl;
}
//---------------------------------------------------------------------------
inline const unsigned int NiAction::GetHow() const
{
    return m_uiHow;
}
//---------------------------------------------------------------------------
inline void NiAction::SetAppData(unsigned int uiAppData)
{
    m_uiAppData = uiAppData;
}
//---------------------------------------------------------------------------
inline void NiAction::SetSemantic(unsigned int uiSemantic)
{
    m_uiSemantic = uiSemantic;
}
//---------------------------------------------------------------------------
inline void NiAction::SetFlags(unsigned int uiFlags)
{
    m_uiFlags = uiFlags;
}
//---------------------------------------------------------------------------
inline void NiAction::SetDeviceID(unsigned int uiDeviceID)
{
    m_uiDeviceID = uiDeviceID;
}
//---------------------------------------------------------------------------
inline void NiAction::SetRange(int iLow, int iHigh)
{
    m_iRangeLow = iLow;
    m_iRangeHigh = iHigh;
}
//---------------------------------------------------------------------------
inline void NiAction::SetContext(void* pvContext)
{
    m_pvContext = pvContext;
}
//---------------------------------------------------------------------------
inline void NiAction::SetKeyModifiers(unsigned int uiModifiers)
{
    m_uiKeyModifiers = uiModifiers;
}
//---------------------------------------------------------------------------
inline void NiAction::SetMouseModifiers(unsigned int uiModifiers)
{
    m_uiMouseModifiers = uiModifiers;
}
//---------------------------------------------------------------------------
inline void NiAction::SetPadModifiers(unsigned int uiModifiers)
{
    m_uiPadModifiers = uiModifiers;
}
//---------------------------------------------------------------------------
inline void NiAction::SetControl(unsigned int uiControl)
{
    m_uiControl = uiControl;
}
//---------------------------------------------------------------------------
inline void NiAction::SetHow(unsigned int uiHow)
{
    m_uiHow = uiHow;
}
//---------------------------------------------------------------------------
