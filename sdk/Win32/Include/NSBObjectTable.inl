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
inline const char* NSBObjectTable::ObjectDesc::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline NiShaderAttributeDesc::ObjectType NSBObjectTable::ObjectDesc::GetType()
    const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline void NSBObjectTable::ObjectDesc::SetType(
    NiShaderAttributeDesc::ObjectType eType)
{
    m_eType = eType;
}
//---------------------------------------------------------------------------
inline unsigned int NSBObjectTable::ObjectDesc::GetIndex() const
{
    return m_uiIndex;
}
//---------------------------------------------------------------------------
inline void NSBObjectTable::ObjectDesc::SetIndex(unsigned int uiIndex)
{
    m_uiIndex = uiIndex;
}
//---------------------------------------------------------------------------
inline unsigned int NSBObjectTable::GetObjectCount() const
{
    return m_kObjectList.GetSize();
}
//---------------------------------------------------------------------------
inline NSBObjectTable::ObjectDesc* NSBObjectTable::GetFirstObject()
{
    m_kObjectListIter = m_kObjectList.GetHeadPos();
    return GetNextObject();
}
//---------------------------------------------------------------------------
inline NSBObjectTable::ObjectDesc* NSBObjectTable::GetNextObject()
{
    if (m_kObjectListIter)
    {
        return m_kObjectList.GetNext(m_kObjectListIter);
    }
    return NULL;
}
//---------------------------------------------------------------------------
