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
inline NiGeneralComponent::NiGeneralComponent() :
    m_kProperties(0)
{
}
//---------------------------------------------------------------------------
inline NiGeneralComponent::NiGeneralComponent(
    const NiFixedString& kName,  
    const NiUniqueID& kTemplateID,
    unsigned int uiPropertyArraySize) 
    :
    m_kName(kName),
    m_kProperties(uiPropertyArraySize),
    m_kTemplateID(kTemplateID)
{
}
//---------------------------------------------------------------------------
inline NiGeneralComponent::NiGeneralComponent(
    const NiFixedString& kName,
    const NiUniqueID& kTemplateID, 
    unsigned int uiPropertyArraySize,
    NiGeneralComponent* pkMasterComponent) 
    :
    m_kName(kName),
    m_kProperties(uiPropertyArraySize),
    m_spMasterComponent(pkMasterComponent),
    m_kTemplateID(kTemplateID)
{
}
//---------------------------------------------------------------------------
inline NiGeneralComponentProperty* NiGeneralComponent::FindPropertyByName(
    const NiFixedString& kPropertyName, bool bSearchMaster) const
{
    unsigned int uiSize = m_kProperties.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiGeneralComponentProperty* pkProperty = m_kProperties.GetAt(ui);
        if (pkProperty->GetPropertyName() == kPropertyName)
        {
            return pkProperty;
        }
    }

    if (bSearchMaster && m_spMasterComponent)
    {
        return m_spMasterComponent->FindPropertyByName(kPropertyName,
            bSearchMaster);
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline unsigned int NiGeneralComponent::FindPropertyIndexByName(
    const NiFixedString& kPropertyName) const
{
    unsigned int uiSize = m_kProperties.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiGeneralComponentProperty* pkProperty = m_kProperties.GetAt(ui);
        if (pkProperty->GetPropertyName() == kPropertyName)
        {
            return ui;
        }
    }

    return INVALID_INDEX;
}
//---------------------------------------------------------------------------
