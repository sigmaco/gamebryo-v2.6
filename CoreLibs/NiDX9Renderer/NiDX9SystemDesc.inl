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
inline bool NiDX9DeviceDesc::DisplayFormatInfo::CanRenderFullscreen() const
{
    return m_bFullscreen;
}
//---------------------------------------------------------------------------
inline bool NiDX9DeviceDesc::DisplayFormatInfo::CanRenderWindowed() const
{
    return m_bWindowed;
}
//---------------------------------------------------------------------------
inline D3DFORMAT NiDX9DeviceDesc::DisplayFormatInfo::GetFormat() const
{
    return m_eFormat;
}
//---------------------------------------------------------------------------
inline bool NiDX9DeviceDesc::IsDeviceValid() const
{
    return m_kD3DCaps9.DeviceType ? true : false;
}
//---------------------------------------------------------------------------
inline D3DDEVTYPE NiDX9DeviceDesc::GetDeviceType() const
{
    return m_kD3DDevType;
}
//---------------------------------------------------------------------------
inline const D3DCAPS9& NiDX9DeviceDesc::GetCaps() const
{
    return m_kD3DCaps9;
}
//---------------------------------------------------------------------------
inline bool NiDX9DeviceDesc::CanRenderWindowed() const
{
    return m_bRenderWindowed;
}
//---------------------------------------------------------------------------
inline const NiDX9DeviceDesc::DisplayFormatInfo* 
    NiDX9DeviceDesc::GetFormatInfo(D3DFORMAT eFormat) const
{
    NiTListIterator pkIter = m_kScreenFormats.GetHeadPos();
    while (pkIter)
    {
        DisplayFormatInfo* pkMode = m_kScreenFormats.GetNext(pkIter);
        if (pkMode->GetFormat() == eFormat)
            return pkMode;
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9AdapterDesc::ModeDesc::GetClosestRefreshRate(
    unsigned int uiRefreshRate) const
{
    if (uiRefreshRate == D3DPRESENT_RATE_DEFAULT)
        return uiRefreshRate;

    NiTListIterator pkIter = m_kRefreshRates.GetHeadPos();

    NIASSERT(pkIter);

    unsigned int uiTestRefreshRate = m_kRefreshRates.GetNext(pkIter);

    unsigned int uiBestRefreshDiff 
        = abs((int)uiTestRefreshRate - (int)uiRefreshRate);

    unsigned int uiClosestRefreshRate = D3DPRESENT_RATE_DEFAULT;
    while (pkIter)
    {
        uiTestRefreshRate = m_kRefreshRates.GetNext(pkIter);

        unsigned int uiRefreshDiff 
            = abs((int)uiTestRefreshRate - (int)uiRefreshRate);

        if (uiRefreshDiff < uiBestRefreshDiff)
        {
            uiBestRefreshDiff = uiRefreshDiff;
            uiClosestRefreshRate = uiTestRefreshRate;
        }
    }
    return uiClosestRefreshRate;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9AdapterDesc::GetAdapterIndex() const
{
    return m_uiAdapter;
}
//---------------------------------------------------------------------------
inline const char* NiDX9AdapterDesc::GetStringDescription() const
{
    return m_kAdapterIdentifier.Description;
}
//---------------------------------------------------------------------------
inline const D3DADAPTER_IDENTIFIER9& NiDX9AdapterDesc::GetAdapterIdentifier()
    const
{
    return m_kAdapterIdentifier;
}
//---------------------------------------------------------------------------
inline const NiDX9DeviceDesc* NiDX9AdapterDesc::GetDevice(
    D3DDEVTYPE devType) const
{
    if(devType == D3DDEVTYPE_HAL)
    {
        if (m_pkHALDeviceDesc->IsDeviceValid())
            return m_pkHALDeviceDesc;
        else
            return NULL;
    }
    else
    {
        if (m_pkREFDeviceDesc->IsDeviceValid())
            return m_pkREFDeviceDesc;
        else
            return NULL;
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9AdapterDesc::GetModeCount() const
{
    return m_kModeList.GetSize();
}
//---------------------------------------------------------------------------
inline const NiDX9AdapterDesc::ModeDesc* NiDX9AdapterDesc::GetMode(
    unsigned int uiMode) const
{
    return m_kModeList.GetAt(uiMode);
}
//---------------------------------------------------------------------------
inline bool NiDX9AdapterDesc::CanDeviceRenderWindowed(
    D3DDEVTYPE devType) const
{
    const NiDX9DeviceDesc* pkDevice = GetDevice(devType);

    if (!pkDevice)
        return false;

    // if the current display mode is supported by the hardware as a windowed
    // mode, return true

    return pkDevice->CanRenderWindowed();
}
//---------------------------------------------------------------------------
inline const D3DCAPS9* NiDX9AdapterDesc::GetDeviceCaps(
    D3DDEVTYPE devType) const
{
    const NiDX9DeviceDesc* pkDevice = GetDevice(devType);

    if (pkDevice)
        return &(pkDevice->GetCaps());
    else
        return NULL;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9SystemDesc::GetAdapterCount() const
{
    return m_uiAdapterCount;
}
//---------------------------------------------------------------------------
inline const NiDX9AdapterDesc* NiDX9SystemDesc::GetAdapter(
    unsigned int uiAdapter) const
{
    if (uiAdapter < m_kAdapterList.GetSize())
    {
        return m_kAdapterList.GetAt(uiAdapter);
    }

    return NULL;
}
//---------------------------------------------------------------------------

