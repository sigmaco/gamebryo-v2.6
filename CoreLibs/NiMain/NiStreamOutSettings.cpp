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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiStreamOutSettings.h"
#include "NiMain.h"
#include <NiBinaryLoadSave.h>

//---------------------------------------------------------------------------
NiStreamOutSettings::NiStreamOutSettings() : 
    NiRefObject(),
    m_bStreamOutAppend(false)
{
    /* */
}
//---------------------------------------------------------------------------
NiStreamOutSettings::~NiStreamOutSettings()
{
    /* */
}
//---------------------------------------------------------------------------
NiStreamOutSettings& NiStreamOutSettings::operator=(
    const NiStreamOutSettings& kStreamOutSettings)
{
    unsigned int uiCount = kStreamOutSettings.m_kStreamOutTargets.GetSize();
    m_kStreamOutTargets.RemoveAll();
    for (unsigned int i = 0; i < uiCount; i++)
        m_kStreamOutTargets.Add(kStreamOutSettings.m_kStreamOutTargets[i]);
    m_bStreamOutAppend = kStreamOutSettings.m_bStreamOutAppend;
    
    return *this;
}
//---------------------------------------------------------------------------
bool NiStreamOutSettings::LoadBinary(NiBinaryStream& kStream)
{
    // NOTE: This function uses an NiBinaryStream (rather than the
    //   typical NiStream used throughout NiMain) because some NSB*
    //   classes reuse this class.

    unsigned int uiNumTargets = 0;
    NiStreamLoadBinary(kStream, uiNumTargets);
    NiFixedString kString;
    for (unsigned int i = 0; i < uiNumTargets; i++)
    {
        NiFixedString::LoadCStringAsFixedString(kStream, kString);
        m_kStreamOutTargets.Add(kString);
    }

    NiBool bTemp;
    NiStreamLoadBinary(kStream, bTemp);
    m_bStreamOutAppend = (bTemp != 0);

    return true;
}
//---------------------------------------------------------------------------
bool NiStreamOutSettings::SaveBinary(NiBinaryStream& kStream) const
{
    // NOTE: This function uses an NiBinaryStream (rather than the
    //   typical NiStream used throughout NiMain) because some NSB*
    //   classes reuse this class.    

    unsigned int uiCount;
    
    uiCount = m_kStreamOutTargets.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    for (unsigned int i = 0; i < uiCount; i++)
    {
        const NiFixedString& kString = m_kStreamOutTargets.GetAt(i);
        NiFixedString::SaveFixedStringAsCString(kStream, kString);
    }

    NiBool bTemp = m_bStreamOutAppend;
    NiStreamSaveBinary(kStream, bTemp);

    return true;
}
//---------------------------------------------------------------------------
