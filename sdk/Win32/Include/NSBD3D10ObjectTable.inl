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
inline const char* NSBD3D10ObjectTable::ObjectDesc::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline NiShaderAttributeDesc::ObjectType
    NSBD3D10ObjectTable::ObjectDesc::GetType()
    const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline void NSBD3D10ObjectTable::ObjectDesc::SetType(
    NiShaderAttributeDesc::ObjectType eType)
{
    m_eType = eType;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10ObjectTable::ObjectDesc::GetIndex() const
{
    return m_uiIndex;
}
//---------------------------------------------------------------------------
inline void NSBD3D10ObjectTable::ObjectDesc::SetIndex(unsigned int uiIndex)
{
    m_uiIndex = uiIndex;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10ObjectTable::GetObjectCount() const
{
    return m_kObjectList.GetSize();
}
//---------------------------------------------------------------------------
inline NSBD3D10ObjectTable::ObjectDesc* NSBD3D10ObjectTable::GetFirstObject()
{
    m_kObjectListIter = m_kObjectList.GetHeadPos();
    return GetNextObject();
}
//---------------------------------------------------------------------------
inline NSBD3D10ObjectTable::ObjectDesc* NSBD3D10ObjectTable::GetNextObject()
{
    if (m_kObjectListIter)
    {
        return m_kObjectList.GetNext(m_kObjectListIter);
    }
    return NULL;
}
//---------------------------------------------------------------------------
