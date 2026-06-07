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
inline void NiMAXPlatformMaterial::NotifyChanged()
{
    NotifyDependents(FOREVER, (PartID)PART_ALL, REFMSG_CHANGE);
}
        
//---------------------------------------------------------------------------
inline void NiMAXPlatformMaterial::SetAmbient(Color, TimeValue) 
{
}       

//---------------------------------------------------------------------------
inline void NiMAXPlatformMaterial::SetDiffuse(Color, TimeValue) 
{
}       

//---------------------------------------------------------------------------
inline void NiMAXPlatformMaterial::SetSpecular(Color, TimeValue) 
{
}

//---------------------------------------------------------------------------
inline void NiMAXPlatformMaterial::SetShininess(float, TimeValue) 
{
}
//---------------------------------------------------------------------------
inline Class_ID NiMAXPlatformMaterial::ClassID()            
{ 
    return PLATFORM_MATERIAL_CLASS_ID; 
}

//---------------------------------------------------------------------------
inline SClass_ID NiMAXPlatformMaterial::SuperClassID()  
{ 
    return MATERIAL_CLASS_ID;
}

//---------------------------------------------------------------------------
inline void NiMAXPlatformMaterial::GetClassName(TSTR& s)    
{ 
    s=GetString(IDS_NI_PLATFORM_MATERIAL); 
}  

//---------------------------------------------------------------------------
inline int NiMAXPlatformMaterial::NumSubMtls()
{ 
    return NUM_PLATFORM_SUBMTLS; 
}
//---------------------------------------------------------------------------
inline int NiMAXPlatformMaterial::NumSubTexmaps()                    
{ 
    return 0; 
}
//---------------------------------------------------------------------------
inline Texmap* NiMAXPlatformMaterial::GetSubTexmap(int)            
{ 
    return NULL; 
} 
//---------------------------------------------------------------------------
inline TSTR NiMAXPlatformMaterial::GetSubTexmapSlotName(int)   
{ 
    return _T( "" ); 
}
//---------------------------------------------------------------------------
inline int NiMAXPlatformMaterial::NumSubs()                      
{ 
    return NUM_PLATFORM_SUBMTLS + 1; 
} 
//---------------------------------------------------------------------------
inline int NiMAXPlatformMaterial::SubNumToRefNum(int subNum)         
{ 
    return subNum; 
}
//---------------------------------------------------------------------------
inline int NiMAXPlatformMaterial::NumRefs()                      
{ 
    return MTL_NUM_REFS; 
}
//---------------------------------------------------------------------------
inline int  NiMAXPlatformMaterial::NumParamBlocks()             
{ 
    return 1; 
}       
//---------------------------------------------------------------------------
inline IParamBlock2* NiMAXPlatformMaterial::GetParamBlock(int)    
{ 
    return m_pkBlock; 
}
//---------------------------------------------------------------------------
inline IParamBlock2* NiMAXPlatformMaterial::GetParamBlockByID(BlockID id) 
{ 
    return (m_pkBlock->ID() == id) ? m_pkBlock : NULL; 
} 
//---------------------------------------------------------------------------

inline BOOL NiMAXPlatformMaterial::SupportsRenderElements()
{ 
    return TRUE; 
}
//---------------------------------------------------------------------------
