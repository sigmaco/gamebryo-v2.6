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
// NSBStateGroup inline functions
//---------------------------------------------------------------------------
inline NSBStateGroup::NSBSGEntry::NSBSGEntry() :
    m_uiFlags(0), 
    m_uiState(0xffffffff), 
    m_uiValue(0xffffffff), 
    m_pcAttribute(0)
{
}
//---------------------------------------------------------------------------
inline NSBStateGroup::NSBSGEntry::~NSBSGEntry()
{
    NiFree(m_pcAttribute);
}
//---------------------------------------------------------------------------
inline unsigned int NSBStateGroup::NSBSGEntry::GetFlags() const
{
    return m_uiFlags;
}
//---------------------------------------------------------------------------
inline bool NSBStateGroup::NSBSGEntry::IsSaved() const
{
    return ((m_uiFlags & NSBSG_SAVE) ? true : false);
}
//---------------------------------------------------------------------------
inline bool NSBStateGroup::NSBSGEntry::UsesAttribute() const
{
    return ((m_uiFlags & NSBSG_ATTRIBUTE) ? true : false);
}
//---------------------------------------------------------------------------
inline bool NSBStateGroup::NSBSGEntry::UsesMapValue() const
{
    return ((m_uiFlags & NSBSG_USEMAPVALUE) ? true : false);
}
//---------------------------------------------------------------------------
inline unsigned int NSBStateGroup::NSBSGEntry::GetState() const
{
    return m_uiState;
}
//---------------------------------------------------------------------------
inline unsigned int NSBStateGroup::NSBSGEntry::GetValue() const
{
    return m_uiValue;
}
//---------------------------------------------------------------------------
inline const char* NSBStateGroup::NSBSGEntry::GetAttribute() const
{
    return m_pcAttribute;
}
//---------------------------------------------------------------------------
inline void NSBStateGroup::NSBSGEntry::SetFlags(unsigned int uiFlags)
{
    m_uiFlags = uiFlags;
}
//---------------------------------------------------------------------------
inline void NSBStateGroup::NSBSGEntry::SetSaved(bool bSave)
{
    if (bSave)
        m_uiFlags |= NSBSG_SAVE;
    else
        m_uiFlags &= ~NSBSG_SAVE;
}
//---------------------------------------------------------------------------
inline void NSBStateGroup::NSBSGEntry::SetUseAttribute(bool bUse)
{
    if (bUse)
        m_uiFlags |= NSBSG_ATTRIBUTE;
    else
        m_uiFlags &= ~NSBSG_ATTRIBUTE;
}
//---------------------------------------------------------------------------
inline void NSBStateGroup::NSBSGEntry::SetUseMapValue(bool bUse)
{
    if (bUse)
        m_uiFlags |= NSBSG_USEMAPVALUE;
    else
        m_uiFlags &= ~NSBSG_USEMAPVALUE;
}
//---------------------------------------------------------------------------
inline void NSBStateGroup::NSBSGEntry::SetState(unsigned int uiState)
{
    m_uiState = uiState;
}
//---------------------------------------------------------------------------
inline void NSBStateGroup::NSBSGEntry::SetValue(unsigned int uiValue)
{
    m_uiValue = uiValue;
}
//---------------------------------------------------------------------------
