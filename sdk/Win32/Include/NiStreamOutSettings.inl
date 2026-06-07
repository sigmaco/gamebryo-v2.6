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
inline void NiStreamOutSettings::SetStreamOutAppend(bool bStreamOutAppend) 
{ 
    m_bStreamOutAppend = bStreamOutAppend; 
}
//---------------------------------------------------------------------------
inline bool NiStreamOutSettings::GetStreamOutAppend() const
{ 
    return m_bStreamOutAppend;             
}
//---------------------------------------------------------------------------
inline unsigned int NiStreamOutSettings::GetStreamOutTargetCount() const
{ 
    return m_kStreamOutTargets.GetSize(); 
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiStreamOutSettings::GetStreamOutTarget(
    int uiIndex) const
{ 
    return m_kStreamOutTargets.GetAt(uiIndex); 
}
//---------------------------------------------------------------------------
inline void NiStreamOutSettings::ClearStreamOutTargets() 
{ 
    m_kStreamOutTargets.RemoveAll();
}
//---------------------------------------------------------------------------
inline void NiStreamOutSettings::AppendStreamOutTargets(
    const char* pcStreamOutTarget) 
{ 
    m_kStreamOutTargets.Add(NiFixedString(pcStreamOutTarget));
}
//---------------------------------------------------------------------------
inline void NiStreamOutSettings::SetStreamOutTargets(
    const NiTObjectArray<NiFixedString>& kStreamOutTargets) 
{ 
    unsigned int uiCount = kStreamOutTargets.GetSize();
    m_kStreamOutTargets.RemoveAll();
    for (unsigned int i = 0; i < uiCount; i++)
        m_kStreamOutTargets.Add(kStreamOutTargets.GetAt(i));
}
//---------------------------------------------------------------------------
