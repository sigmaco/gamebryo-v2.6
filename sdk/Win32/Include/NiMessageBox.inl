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
inline void NiMessageBoxUtilities::SetMessageBoxFunction(
    MessageBoxFunction pfnMessageBox)
{
    ms_pfnMessageBox = pfnMessageBox;
}
//---------------------------------------------------------------------------
inline MessageBoxFunction NiMessageBoxUtilities::GetMessageBoxFunction()
{
    return ms_pfnMessageBox;
}
//---------------------------------------------------------------------------
inline unsigned int NiMessageBoxUtilities::DisplayMessage(const char* pcText, 
    const char* pcCaption, void* pvExtraData)
{
    if (ms_pfnMessageBox == NULL)
        return 0;

    return (*ms_pfnMessageBox)(pcText, pcCaption, pvExtraData);
}
//---------------------------------------------------------------------------
inline unsigned int NiMessageBox(const char* pcText, const char* pcCaption, 
    void* pvExtraData)
{
    return NiMessageBoxUtilities::DisplayMessage(
        pcText, pcCaption, pvExtraData);
}
//---------------------------------------------------------------------------
