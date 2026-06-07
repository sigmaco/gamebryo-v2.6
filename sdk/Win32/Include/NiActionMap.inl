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
// NiActionMap inline functions
//---------------------------------------------------------------------------
inline const char* NiActionMap::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline const void* NiActionMap::GetPlatformSpecificData() const
{
    return m_pvPlatformSpecificData;
}
//---------------------------------------------------------------------------
inline const unsigned int NiActionMap::GetActionCount() const
{
    return m_akActions.GetEffectiveSize();
}
//---------------------------------------------------------------------------
inline NiAction* NiActionMap::GetActionFromPool()
{
    if (!ms_pakActionPool)
        CreateActionPool();

    NIASSERT(ms_pakActionPool);
    
    return ms_pakActionPool->GetFreeObject();
}
//---------------------------------------------------------------------------
inline void NiActionMap::ReturnActionToPool(NiAction* pkAction)
{
    NIASSERT(ms_pakActionPool);
    ms_pakActionPool->ReleaseObject(pkAction);
}
//---------------------------------------------------------------------------
