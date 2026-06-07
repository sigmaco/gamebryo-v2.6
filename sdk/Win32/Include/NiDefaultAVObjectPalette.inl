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
inline NiDefaultAVObjectPalette::NiDefaultAVObjectPalette(
    NiAVObject* pkScene)
{
    m_pkScene = pkScene;
}
//---------------------------------------------------------------------------
inline NiAVObject* NiDefaultAVObjectPalette::GetAVObject(const char* pcName)
{
    NiAVObject* pkObj = NULL;
    if (m_kHash.GetAt(pcName, pkObj))
    {
        return pkObj;
    }
    else if (m_pkScene)
    {
        pkObj = m_pkScene->GetObjectByName(pcName);
        if (pkObj)
        {
            SetAVObject(pcName, pkObj);
        }

        return pkObj;
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline void NiDefaultAVObjectPalette::SetAVObject(const char* pcName, 
    NiAVObject* pkObj)
{
    if (pkObj == NULL)
        m_kHash.RemoveAt(pcName);
    else
        m_kHash.SetAt(pcName, pkObj);
}
//---------------------------------------------------------------------------
inline void NiDefaultAVObjectPalette::SetScene(NiAVObject* pkScene)
{
    m_pkScene = pkScene;
}
//---------------------------------------------------------------------------
inline NiAVObject* NiDefaultAVObjectPalette::GetScene()
{
    return m_pkScene;
}
//---------------------------------------------------------------------------
