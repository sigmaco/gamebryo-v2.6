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
inline void NiLogger::SetOutputToDebugWindow(int iMessageType, bool bOutput)
{
    ms_akMessageOptions[iMessageType].m_bOutputToDebugWindow = bOutput;
}
//---------------------------------------------------------------------------
inline bool NiLogger::GetOutputToDebugWindow(int iMessageType)
{
    return ms_akMessageOptions[iMessageType].m_bOutputToDebugWindow;
}
//---------------------------------------------------------------------------
inline void NiLogger::SetPrependTimestamp(int iMessageType, bool bTimestamp)
{
    ms_akMessageOptions[iMessageType].m_bPrependTimestamp = bTimestamp;
}
//---------------------------------------------------------------------------
inline bool NiLogger::GetPrependTimestamp(int iMessageType)
{
    return ms_akMessageOptions[iMessageType].m_bPrependTimestamp;
}
//---------------------------------------------------------------------------
inline void NiLogger::SetOutputToLogFile(int iMessageType, int iLogID)
{
    ms_akMessageOptions[iMessageType].m_iLogID = iLogID;
}
//---------------------------------------------------------------------------
inline int NiLogger::GetOutputToLogFile(int iMessageType)
{
    return ms_akMessageOptions[iMessageType].m_iLogID;
}
//---------------------------------------------------------------------------


