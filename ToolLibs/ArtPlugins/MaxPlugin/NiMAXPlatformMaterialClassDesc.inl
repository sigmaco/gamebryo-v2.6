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
inline int NiMAXPlatformMaterialClassDesc::IsPublic() 
{ 
    return TRUE; 
}
//---------------------------------------------------------------------------
inline void *NiMAXPlatformMaterialClassDesc::Create(BOOL loading)
{ 
    return NiExternalNew NiMAXPlatformMaterial(loading);
}
//---------------------------------------------------------------------------
inline const TCHAR *NiMAXPlatformMaterialClassDesc::ClassName()
{ 
    return GetString(IDS_NI_PLATFORM_MATERIAL); 
}
//---------------------------------------------------------------------------
inline SClass_ID NiMAXPlatformMaterialClassDesc::SuperClassID()
{ 
    return MATERIAL_CLASS_ID;
}
//---------------------------------------------------------------------------
inline Class_ID NiMAXPlatformMaterialClassDesc::ClassID()
{ 
    return PLATFORM_MATERIAL_CLASS_ID;
}
//---------------------------------------------------------------------------
inline const TCHAR *NiMAXPlatformMaterialClassDesc::Category()
{ 
    return _T("");  
}
//---------------------------------------------------------------------------
// Hardwired name, used by MAX Script as unique identifier
inline const TCHAR *NiMAXPlatformMaterialClassDesc::InternalName()
{ 
    return _T("PlatformMaterial"); 
}
//---------------------------------------------------------------------------
inline HINSTANCE NiMAXPlatformMaterialClassDesc::HInstance()
{ 
    return hInstance; 
}
//---------------------------------------------------------------------------
