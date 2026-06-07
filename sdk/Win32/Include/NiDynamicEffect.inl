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
//  NiDynamicEffect inline functions

//---------------------------------------------------------------------------
inline bool NiDynamicEffect::GetSwitch() const
{
    return m_bOn;
}
//---------------------------------------------------------------------------
inline int NiDynamicEffect::GetIndex() const
{
    return m_iIndex;
}
//---------------------------------------------------------------------------
inline void NiDynamicEffect::IncRevisionID()
{
    m_uiRevID++;
}
//---------------------------------------------------------------------------
inline unsigned int NiDynamicEffect::GetRevisionID() const
{
    return m_uiRevID;
}
//---------------------------------------------------------------------------
inline void NiDynamicEffect:: SetRevisionID(unsigned int uiRevID)
{
    m_uiRevID = uiRevID;
}
//---------------------------------------------------------------------------
inline void NiDynamicEffect::IncPushCount()
{
    m_uiPushCount++;
}
//---------------------------------------------------------------------------
inline void NiDynamicEffect::DecPushCount()
{
    m_uiPushCount--;
}
//---------------------------------------------------------------------------
inline int NiDynamicEffect::GetPushCount() const
{
    return m_uiPushCount;
}
//---------------------------------------------------------------------------
inline const NiNodeList& NiDynamicEffect::GetAffectedNodeList()
{
    return m_kAffectedNodeList;
}
//---------------------------------------------------------------------------
inline const NiNodeList& NiDynamicEffect::GetUnaffectedNodeList()
{
    return m_kUnaffectedNodeList;
}
//---------------------------------------------------------------------------
inline NiDynamicEffect::EffectType NiDynamicEffect::GetEffectType() const
{
    return (EffectType)m_ucEffectType;
}
//---------------------------------------------------------------------------
inline void NiDynamicEffect::SetEffectType(unsigned char ucEffectType)
{
    m_ucEffectType = ucEffectType;
}
//---------------------------------------------------------------------------
inline NiShadowGenerator*  NiDynamicEffect::GetShadowGenerator()
{
    return m_pkShadowGenerator;
}
//---------------------------------------------------------------------------
