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
// NiInputDI8GamePad inline functions
//---------------------------------------------------------------------------
inline const LPDIRECTINPUTDEVICE8 NiInputDI8GamePad::GetDIDevice()
{
    return m_pkDIDevice;
}
//---------------------------------------------------------------------------
inline unsigned int NiInputDI8GamePad::GetMotorCount() const
{
    return 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiInputDI8GamePad::GetRumbleRange(unsigned int) const
{
    return 0;
}
//---------------------------------------------------------------------------
inline void NiInputDI8GamePad::SetRumbleValue(unsigned int, unsigned int,
    bool)
{
    return;
}
//---------------------------------------------------------------------------
inline void NiInputDI8GamePad::GetRumbleValue(unsigned int, 
    unsigned int& uiValue) const
{
    uiValue = 0;
    return;
}
//---------------------------------------------------------------------------
inline void NiInputDI8GamePad::StartRumble()
{
    return;
}
//---------------------------------------------------------------------------
inline void NiInputDI8GamePad::StopRumble(bool)
{
    return;
}
//---------------------------------------------------------------------------
