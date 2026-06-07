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
inline int NiMAXShaderClassDesc::IsPublic() 
{ 
    return TRUE; 
}
//---------------------------------------------------------------------------
inline void *NiMAXShaderClassDesc::Create(BOOL)     
{ 
    return NiExternalNew NiMAXShader(); 
}
//---------------------------------------------------------------------------
inline const TCHAR *NiMAXShaderClassDesc::ClassName()                   
{ 
    return GetString(IDS_CLASSNAME); 
}
//---------------------------------------------------------------------------
inline SClass_ID NiMAXShaderClassDesc::SuperClassID()               
{ 
    return SHADER_CLASS_ID; 
}
//---------------------------------------------------------------------------
inline Class_ID NiMAXShaderClassDesc::ClassID()                 
{ 
    return NISHADER_CLASSID; 
}
//---------------------------------------------------------------------------
inline const TCHAR *NiMAXShaderClassDesc::Category()                    
{ 
    return _T("");  
}
//---------------------------------------------------------------------------
// Hardwired name, used by MAX Script as unique identifier
inline const TCHAR *NiMAXShaderClassDesc::InternalName()                
{ 
    return _T("NetImmerseShader"); 
}
//---------------------------------------------------------------------------
inline HINSTANCE NiMAXShaderClassDesc::HInstance()                  
{ 
    return hInstance; 
}
//---------------------------------------------------------------------------
