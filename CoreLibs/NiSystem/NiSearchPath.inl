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

//---------------------------------------------------------------------------
inline void NiSearchPath::SetDefaultPath(const char* pcPath) 
{ 
    NiStrncpy(ms_acDefPath, NI_MAX_PATH, pcPath, NI_MAX_PATH - 1); 
}
//---------------------------------------------------------------------------
inline const char* NiSearchPath::GetDefaultPath()
{ 
    return ms_acDefPath; 
}
//---------------------------------------------------------------------------
