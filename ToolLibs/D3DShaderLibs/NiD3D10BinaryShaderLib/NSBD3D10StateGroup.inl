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
// NSBD3D10StateGroup inline functions
//---------------------------------------------------------------------------
inline NSBD3D10StateGroup::NSBD3D10SGEntry::NSBD3D10SGEntry() :
    m_uiFlags(0), 
    m_uiState(0xffffffff), 
    m_uiValue(0xffffffff), 
    m_pcAttribute(0)
{
}
//---------------------------------------------------------------------------
inline NSBD3D10StateGroup::NSBD3D10SGEntry::~NSBD3D10SGEntry()
{
    NiFree(m_pcAttribute);
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10StateGroup::NSBD3D10SGEntry::GetFlags() const
{
    return m_uiFlags;
}
//---------------------------------------------------------------------------
inline bool NSBD3D10StateGroup::NSBD3D10SGEntry::IsSaved() const
{
    return ((m_uiFlags & NSBSG_SAVE) ? true : false);
}
//---------------------------------------------------------------------------
inline bool NSBD3D10StateGroup::NSBD3D10SGEntry::UsesAttribute() const
{
    return ((m_uiFlags & NSBSG_ATTRIBUTE) ? true : false);
}
//---------------------------------------------------------------------------
inline bool NSBD3D10StateGroup::NSBD3D10SGEntry::UsesMapValue() const
{
    return ((m_uiFlags & NSBSG_USEMAPVALUE) ? true : false);
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10StateGroup::NSBD3D10SGEntry::GetState() const
{
    return m_uiState;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10StateGroup::NSBD3D10SGEntry::GetValue() const
{
    return m_uiValue;
}
//---------------------------------------------------------------------------
inline const char* NSBD3D10StateGroup::NSBD3D10SGEntry::GetAttribute() const
{
    return m_pcAttribute;
}
//---------------------------------------------------------------------------
inline void NSBD3D10StateGroup::NSBD3D10SGEntry::SetFlags(unsigned int uiFlags)
{
    m_uiFlags = uiFlags;
}
//---------------------------------------------------------------------------
inline void NSBD3D10StateGroup::NSBD3D10SGEntry::SetSaved(bool bSave)
{
    if (bSave)
        m_uiFlags |= NSBSG_SAVE;
    else
        m_uiFlags &= ~NSBSG_SAVE;
}
//---------------------------------------------------------------------------
inline void NSBD3D10StateGroup::NSBD3D10SGEntry::SetUseAttribute(bool bUse)
{
    if (bUse)
        m_uiFlags |= NSBSG_ATTRIBUTE;
    else
        m_uiFlags &= ~NSBSG_ATTRIBUTE;
}
//---------------------------------------------------------------------------
inline void NSBD3D10StateGroup::NSBD3D10SGEntry::SetUseMapValue(bool bUse)
{
    if (bUse)
        m_uiFlags |= NSBSG_USEMAPVALUE;
    else
        m_uiFlags &= ~NSBSG_USEMAPVALUE;
}
//---------------------------------------------------------------------------
inline void NSBD3D10StateGroup::NSBD3D10SGEntry::SetState(unsigned int uiState)
{
    m_uiState = uiState;
}
//---------------------------------------------------------------------------
inline void NSBD3D10StateGroup::NSBD3D10SGEntry::SetValue(unsigned int uiValue)
{
    m_uiValue = uiValue;
}
//---------------------------------------------------------------------------
