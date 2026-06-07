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
// NiInputDevice inline functions
//---------------------------------------------------------------------------
// NiInputDevice::ControlDesc
//---------------------------------------------------------------------------
inline NiInputDevice::ControlDesc::ControlDesc() :
    m_uiIndex(0xffffffff), 
    m_pcName(0), 
    m_eType(NiInputDevice::ControlDesc::UNKNOWN), 
    m_uiFlags(0)
{
}
//---------------------------------------------------------------------------
inline NiInputDevice::ControlDesc::ControlDesc(unsigned int uiIndex, 
    const char* pcName, Type eType, unsigned int uiFlags) :
    m_uiIndex(uiIndex), 
    m_pcName(0), 
    m_eType(eType), 
    m_uiFlags(uiFlags)
{
    SetName(pcName);
}
//---------------------------------------------------------------------------
inline NiInputDevice::ControlDesc::~ControlDesc()
{
    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputDevice::ControlDesc::GetIndex() const
{
    return m_uiIndex;
}
//---------------------------------------------------------------------------
inline const char* NiInputDevice::ControlDesc::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline const NiInputDevice::ControlDesc::Type 
NiInputDevice::ControlDesc::GetType() const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputDevice::ControlDesc::GetFlags() const
{
    return m_uiFlags;
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::ControlDesc::IsDigital() const
{
    return ((m_uiFlags & DATA_MASK) == DATA_DIGITAL);
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::ControlDesc::IsAnalog() const
{
    return ((m_uiFlags & DATA_MASK) == DATA_ANALOG);
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::ControlDesc::IsInput() const
{
    return ((m_uiFlags & TYPE_MASK) == TYPE_INPUT);
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::ControlDesc::IsOutput() const
{
    return ((m_uiFlags & TYPE_MASK) == TYPE_OUTPUT);
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::ControlDesc::IsHybrid() const
{
    return ((m_uiFlags & TYPE_MASK) == TYPE_HYBRID);
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::ControlDesc::IsRumble() const
{
    return ((m_uiFlags & OUT_MASK) == OUT_RUMBLE);
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::ControlDesc::IsForceFeedback() const
{
    return ((m_uiFlags & OUT_MASK) == OUT_FORCEFB);
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ControlDesc::SetIndex(unsigned int uiIndex)
{
    m_uiIndex = uiIndex;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ControlDesc::SetName(const char* pcName)
{
    NiFree(m_pcName);
    m_pcName = 0;
    if (pcName && pcName[0] != '\0')
    {
        size_t stLen = strlen(pcName) + 1;
        m_pcName = NiAlloc(char, stLen);
        NIASSERT(m_pcName);
        NiStrcpy(m_pcName, stLen, pcName);
    }
    else
    {
        m_pcName = 0;
    }
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ControlDesc::SetType(
    NiInputDevice::ControlDesc::Type eType)
{
    m_eType = eType;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ControlDesc::SetFlags(unsigned int uiFlags)
{
    m_uiFlags = uiFlags;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ControlDesc::SetDigital(bool bEnabled)
{
    if (bEnabled)
        m_uiFlags |= DATA_DIGITAL;
    else
        m_uiFlags &= ~DATA_DIGITAL;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ControlDesc::SetAnalog(bool bEnabled)
{
    if (bEnabled)
        m_uiFlags |= DATA_ANALOG;
    else
        m_uiFlags &= ~DATA_ANALOG;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ControlDesc::SetInput(bool bEnabled)
{
    if (bEnabled)
        m_uiFlags |= TYPE_INPUT;
    else
        m_uiFlags &= ~TYPE_INPUT;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ControlDesc::SetOutput(bool bEnabled)
{
    if (bEnabled)
        m_uiFlags |= TYPE_OUTPUT;
    else
        m_uiFlags &= ~TYPE_OUTPUT;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ControlDesc::SetHybrid(bool bEnabled)
{
    if (bEnabled)
        m_uiFlags |= TYPE_HYBRID;
    else
        m_uiFlags &= ~TYPE_HYBRID;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ControlDesc::SetRumble(bool bEnabled)
{
    if (bEnabled)
        m_uiFlags |= OUT_RUMBLE;
    else
        m_uiFlags &= ~OUT_RUMBLE;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ControlDesc::SetForceFeedback(bool bEnabled)
{
    if (bEnabled)
        m_uiFlags |= OUT_FORCEFB;
    else
        m_uiFlags &= ~OUT_FORCEFB;
}
//---------------------------------------------------------------------------
inline const unsigned int 
NiInputDevice::ControlDesc::GetPlatformSpecificData() const
{
    return m_uiPlatformSpecificData;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ControlDesc::SetPlatformSpecificData(
    unsigned int uiPlatformSpecificData)
{
    m_uiPlatformSpecificData = uiPlatformSpecificData;
}
//---------------------------------------------------------------------------
// NiInputDevice::Description
//---------------------------------------------------------------------------
inline NiInputDevice::Description::Description() :
    m_eType(NiInputDevice::NIID_NULL), 
    m_uiPort(0), 
    m_uiSlot(0), 
    m_pcName(0), 
    m_pcDesc(0), 
    m_bDeviceOwned(false),
    m_uiButtonCount(0), 
    m_uiAxisCount(0), 
    m_uiPOVCount(0) 
{
    m_kControls.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiInputDevice::Description::Description(Type eType, 
    unsigned int uiPort, unsigned int uiSlot, const char* pcName) :
    m_eType(eType), 
    m_uiPort(uiPort), 
    m_uiSlot(uiSlot), 
    m_pcName(0), 
    m_pcDesc(0), 
    m_bDeviceOwned(false),
    m_uiButtonCount(0), 
    m_uiAxisCount(0), 
    m_uiPOVCount(0)
{
    SetName(pcName);
    m_kControls.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiInputDevice::Description::~Description()
{
    NiFree(m_pcDesc);
    NiFree(m_pcName);

    // Clear out the available device list.
    ControlDesc* pkDesc = GetFirstControlDesc();
    while (pkDesc)
    {
        NiDelete pkDesc;
        pkDesc = GetNextControlDesc();
    }
    m_kControls.RemoveAll();
}
//---------------------------------------------------------------------------
inline const NiInputDevice::Type NiInputDevice::Description::GetType() const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline const NiInputDevice::Status NiInputDevice::GetStatus() const
{
    return m_eStatus;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputDevice::Description::GetPort() const
{
    return m_uiPort;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputDevice::Description::GetSlot() const
{
    return m_uiSlot;
}
//---------------------------------------------------------------------------
inline const char* NiInputDevice::Description::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline const char* NiInputDevice::Description::GetDescriptionString() const
{
    return m_pcDesc;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Description::SetType(Type eType)
{
    m_eType = eType;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Description::SetPort(unsigned int uiPort)
{
    m_uiPort = uiPort;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Description::SetSlot(unsigned int uiSlot)
{
    m_uiSlot = uiSlot;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Description::SetName(const char* pcName)
{
    NiFree(m_pcName);
    m_pcName = 0;
    if (pcName && pcName[0] != '\0')
    {
        size_t stLen = strlen(pcName) + 1;
        m_pcName = NiAlloc(char, stLen);
        NIASSERT(m_pcName);
        NiStrcpy(m_pcName, stLen, pcName);
    }
    else
    {
        m_pcName = 0;
    }
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Description::SetDescriptionString(
    const char* pcDesc)
{
    NiFree(m_pcDesc);
    m_pcDesc = 0;
    if (pcDesc && pcDesc[0] != '\0')
    {
        size_t stLen = strlen(pcDesc) + 1;
        m_pcDesc = NiAlloc(char, stLen);
        NIASSERT(m_pcDesc);
        NiStrcpy(m_pcDesc, stLen, pcDesc);
    }
    else
    {
        m_pcDesc = 0;
    }
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::Description::GetDeviceOwned() const
{
    return m_bDeviceOwned;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Description::SetDeviceOwned(bool bOwned)
{
    m_bDeviceOwned = bOwned;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Description::IncrementButtonCount()
{
    m_uiButtonCount++;
}
//---------------------------------------------------------------------------
inline unsigned int NiInputDevice::Description::GetControlCount()
{
    return m_kControls.GetSize();
}
//---------------------------------------------------------------------------
inline NiInputDevice::ControlDesc* 
NiInputDevice::Description::GetFirstControlDesc()
{
    m_kControlsIter = m_kControls.GetHeadPos();
    if (m_kControlsIter)
        return m_kControls.GetNext(m_kControlsIter);
    return 0;
}
//---------------------------------------------------------------------------
inline NiInputDevice::ControlDesc* 
NiInputDevice::Description::GetNextControlDesc()
{
    if (m_kControlsIter)
        return m_kControls.GetNext(m_kControlsIter);
    return 0;
}
//---------------------------------------------------------------------------
inline NiInputDevice::ControlDesc* 
NiInputDevice::Description::GetControlDescAtIndex(unsigned int uiIndex)
{
    NiInputDevice::ControlDesc* pkDesc = GetFirstControlDesc();
    while (pkDesc)
    {
        if (pkDesc->GetIndex() == uiIndex)
            break;
        pkDesc = GetNextControlDesc();
    }
    return pkDesc;
}
//---------------------------------------------------------------------------
inline NiInputDevice::ControlDesc* 
NiInputDevice::Description::GetControlDescByName(const char* pcName)
{
    NiInputDevice::ControlDesc* pkDesc = GetFirstControlDesc();
    while (pkDesc)
    {
        if (NiStricmp(pkDesc->GetName(), pcName) == 0)
            break;
        pkDesc = GetNextControlDesc();
    }
    return pkDesc;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputDevice::Description::GetButtonCount() const
{
    return m_uiButtonCount;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputDevice::Description::GetAxisCount() const
{
    return m_uiAxisCount;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputDevice::Description::GetPOVCount() const
{
    return m_uiPOVCount;
}
//---------------------------------------------------------------------------
// NiInputDevice
//---------------------------------------------------------------------------
inline const NiInputDevice::Type NiInputDevice::GetType() const
{
    NIASSERT(m_spDescription);
    return m_spDescription->GetType();
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputDevice::GetPort() const
{
    NIASSERT(m_spDescription);
    return m_spDescription->GetPort();
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputDevice::GetSlot() const
{
    NIASSERT(m_spDescription);
    return m_spDescription->GetSlot();
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputDevice::GetModifiers() const
{
    return m_uiModifiers;
}
//---------------------------------------------------------------------------
inline bool NiInputDevice::ModifersAreDown(unsigned int uiModifiers) const
{
    return (m_uiModifiers & uiModifiers) == uiModifiers;
}
//---------------------------------------------------------------------------
inline bool NiInputDevice::ModifiersAreUp(unsigned int uiModifiers) const
{
    return (m_uiModifiers & uiModifiers) == 0;
}
//---------------------------------------------------------------------------
inline NiInputErr NiInputDevice::GetLastError()
{
    return m_eLastError;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::SetStatus(NiInputDevice::Status eStatus)
{
    m_eStatus = eStatus;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::SetPort(unsigned int uiPort)
{
    NIASSERT(m_spDescription);
    m_spDescription->SetPort(uiPort);
}
//---------------------------------------------------------------------------
inline void NiInputDevice::SetSlot(unsigned int uiSlot)
{
    NIASSERT(m_spDescription);
    m_spDescription->SetSlot(uiSlot);
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::GetActionMapped() const
{
    return m_bActionMapped;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::SetActionMapped(bool bMapped)
{
    m_bActionMapped = bMapped;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::AddModifiers(unsigned int uiMod)
{
    m_uiModifiers |= uiMod;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::RemoveModifiers(unsigned int uiMod)
{
    m_uiModifiers &= ~uiMod;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::ClearModifiers()
{
    m_uiModifiers = 0;
}
//---------------------------------------------------------------------------
// NiInputDevice::Button
//---------------------------------------------------------------------------
inline NiInputDevice::Button::Button() :
    m_ucValue(0), 
    m_bPressed(false),
    m_ucLastValue(0), 
    m_bLastPressed(false),
    m_bPressedLastUpdate(false),
    m_uiModifiers(0), 
    m_uiLastModifiers(0)
{
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputDevice::Button::GetModifiers() const
{
    return m_uiModifiers;
}
//---------------------------------------------------------------------------
inline bool NiInputDevice::Button::RemoveModifiers(unsigned int uiModifiers)
{
    if ((m_uiModifiers & uiModifiers) != 0)
    {
        m_uiModifiers &= ~uiModifiers;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::Button::IsPressed(
    unsigned int& uiModifiers) const
{
    uiModifiers = m_uiModifiers;
    return m_bPressed;
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::Button::WasPressed(
    unsigned int& uiModifiers) const
{
    uiModifiers = m_uiModifiers;
//    return m_bPressed && !m_bPressedLastUpdate;
    return m_bPressed && !m_bLastPressed;
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::Button::WasReleased(
    unsigned int& uiModifiers) const
{
    uiModifiers = m_uiLastModifiers;
//    return !m_bPressed && m_bPressedLastUpdate;
    return !m_bPressed && m_bLastPressed;
}
//---------------------------------------------------------------------------
inline const unsigned char NiInputDevice::Button::GetValue() const
{
    return m_ucValue;
}
//---------------------------------------------------------------------------
inline const unsigned char NiInputDevice::Button::GetValue(
    unsigned int& uiModifiers) const
{
    uiModifiers = m_uiModifiers;
    return m_ucValue;
}
//---------------------------------------------------------------------------
inline const unsigned char NiInputDevice::Button::GetLastValue() const
{
    return m_ucLastValue;
}
//---------------------------------------------------------------------------
inline const unsigned char NiInputDevice::Button::GetLastValue(
    unsigned int& uiModifiers) const
{
    uiModifiers = m_uiLastModifiers;
    return m_ucLastValue;
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::Button::ValueChanged() const
{
    return (m_ucValue != m_ucLastValue);
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::Button::ValueChanged(
    bool& bModifiersChanged) const
{
    bModifiersChanged = (m_uiModifiers != m_uiLastModifiers);
    return (m_ucValue != m_ucLastValue);
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Button::SetButtonPress(unsigned int uiModifiers, 
    bool bPressed, unsigned char ucValue)
{
    // Back-store the last values
    TouchValue();

    // Set the new values
    m_ucValue = ucValue;
    m_bPressed = bPressed;
    m_uiModifiers = uiModifiers;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Button::SetButtonRelease()
{        
    // Back-store the last values
    TouchValue();

    // Clear the current value
    m_ucValue = 0;
    m_bPressed = false;
    m_uiModifiers = 0;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Button::UpdatePressedLastUpdate()
{
    m_bPressedLastUpdate = m_bPressed;
}
//---------------------------------------------------------------------------
inline const bool NiInputDevice::Button::GetLastPressed() const
{
    return m_bLastPressed;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputDevice::Button::GetLastModifiers() const
{
    return m_uiLastModifiers;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Button::TouchValue()
{
    // Back-store the last values
    m_ucLastValue = m_ucValue;
    m_bLastPressed = m_bPressed;
    m_uiLastModifiers = m_uiModifiers;
}
//---------------------------------------------------------------------------
// NiInputDevice::Axis
//---------------------------------------------------------------------------
inline NiInputDevice::Axis::Axis() :
    m_iRawValue(0), 
    m_iValue(0), 
    m_iRawLow(0), 
    m_iRawHigh(255), 
    m_iLow(-1), 
    m_iHigh(1)
{
}
//---------------------------------------------------------------------------
inline NiInputDevice::Axis::Axis(int iLow, int iHigh) :
    m_iRawValue(0), 
    m_iValue(0), 
    m_iRawLow(0), 
    m_iRawHigh(255), 
    m_iLow(iLow), 
    m_iHigh(iHigh)
{
}
//---------------------------------------------------------------------------
inline NiInputDevice::Axis::~Axis()
{
    //** **//
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Axis::GetRange(int& iLow, int& iHigh) const
{
    iLow = m_iLow;
    iHigh = m_iHigh;
}
//---------------------------------------------------------------------------
inline const int NiInputDevice::Axis::GetRawValue() const
{
    return m_iRawValue;
}
//---------------------------------------------------------------------------
inline const int NiInputDevice::Axis::GetValue() const
{
    return m_iValue;
}
//---------------------------------------------------------------------------
inline const int NiInputDevice::Axis::GetLastRawValue() const
{
    return m_iLastRawValue;
}
//---------------------------------------------------------------------------
inline const int NiInputDevice::Axis::GetLastValue() const
{
    return m_iLastValue;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Axis::SetRawRange(int iLow, int iHigh)
{
    m_iRawLow = iLow;
    m_iRawHigh = iHigh;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Axis::SetRange(int iLow, int iHigh)
{
    m_iLow = iLow;
    m_iHigh = iHigh;
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Axis::SetValue(int iValue)
{
    // Back store
    m_iLastValue = m_iValue;

    m_iValue = iValue;
}
//---------------------------------------------------------------------------
inline int NiInputDevice::Axis::MapRawValue(int iValue)
{
    int iTotalRange = m_iHigh - m_iLow;
    float fRawFraction = (float)(iValue - m_iRawLow) / 
        (float)(m_iRawHigh - m_iRawLow);
    return (m_iLow + (int)(fRawFraction * iTotalRange));
}
//---------------------------------------------------------------------------
inline void NiInputDevice::Axis::TouchValue()
{
    // Back store
    m_iLastRawValue = m_iRawValue;
    m_iLastValue = m_iValue;
}
//---------------------------------------------------------------------------
// NiInputDevice
//---------------------------------------------------------------------------
inline void NiInputDevice::SetLastError(NiInputErr eErr)
{
    m_eLastError = eErr;
}
//---------------------------------------------------------------------------
