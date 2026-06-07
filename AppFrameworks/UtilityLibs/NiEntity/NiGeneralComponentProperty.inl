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
inline NiGeneralComponentProperty::NiGeneralComponentProperty(
    const NiFixedString& kPropertyName, const NiFixedString& kDisplayName,
    const NiFixedString& kPrimitiveType, const NiFixedString& kSemanticType,
    const NiFixedString& kDescription, bool bIsCollection) : 
    m_kPropertyName(kPropertyName), m_kDisplayName(kDisplayName), 
    m_kPrimitiveType(kPrimitiveType), m_kSemanticType(kSemanticType), 
    m_kDescription(kDescription), m_bIsCollection(bIsCollection)
{
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiGeneralComponentProperty::GetPropertyName()
    const
{
    return m_kPropertyName;
}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetPropertyName(
    const NiFixedString& kPropertyName)
{
    m_kPropertyName = kPropertyName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiGeneralComponentProperty::GetDisplayName() const
{
    return m_kDisplayName;
}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetDisplayName(
    const NiFixedString& kDisplayName)
{
    m_kDisplayName = kDisplayName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiGeneralComponentProperty::GetPrimitiveType()
    const
{
    return m_kPrimitiveType;
}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetPrimitiveType(
    const NiFixedString& kPrimitiveType)
{
    m_kPrimitiveType = kPrimitiveType;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiGeneralComponentProperty::GetSemanticType()
    const
{
    return m_kSemanticType;
}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetSemanticType(
    const NiFixedString& kSemanticType)
{
    m_kSemanticType = kSemanticType;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiGeneralComponentProperty::GetDescription()
    const
{
    return m_kDescription;
}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetDescription(
    const NiFixedString& kDescription)
{
    m_kDescription = kDescription;
}
//---------------------------------------------------------------------------
inline NiBool NiGeneralComponentProperty::GetIsCollection() const
{
    return m_bIsCollection;
}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetIsCollection(bool bIsCollection)
{
    m_bIsCollection = bIsCollection;
}
//---------------------------------------------------------------------------
inline unsigned int NiGeneralComponentProperty::GetCollectionSize() const
{
    if (m_bIsCollection)
    {
        return m_kDataPointers.GetSize();
    }
    else
    {
        return 1;
    }

}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetCollectionSize(unsigned int uiSize)
{
    if (m_bIsCollection)
    {
        const unsigned int uiOldSize = m_kDataPointers.GetSize();
        for (unsigned int ui = uiSize; ui < uiOldSize; ui++)
        {
            SetData((char*) NULL, 0, ui);
        }
        m_kDataPointers.SetSize(uiSize);
        m_kDataSizesInBytes.SetSize(uiSize);
    }
}
//---------------------------------------------------------------------------
template<class T>
inline void NiGeneralComponentProperty::SetData(T* pvDataArray,
    size_t stArrayCount, unsigned int uiIndex)
{
    NIASSERT(m_bIsCollection || uiIndex == 0);

    void* pvData = NULL;
    if (uiIndex < m_kDataPointers.GetSize())
    {
        pvData = GetData(uiIndex);
    }

    if (pvData)
    {
        NiFree(pvData);
        m_kDataPointers.SetAt(uiIndex, NULL);
    }

    if (pvDataArray)
    {
        pvData = NiAlloc(T, stArrayCount);
        size_t stDataSizeInBytes = stArrayCount * sizeof(pvDataArray[0]);
        NIASSERT(stDataSizeInBytes > 0);
        NiMemcpy(pvData, stDataSizeInBytes, pvDataArray, stDataSizeInBytes);
        m_kDataPointers.SetAtGrow(uiIndex, pvData);
        m_kDataSizesInBytes.SetAtGrow(uiIndex, stDataSizeInBytes);
        NIASSERT(m_kDataPointers.GetSize() == m_kDataSizesInBytes.GetSize());
    }
}
//---------------------------------------------------------------------------
inline void* NiGeneralComponentProperty::GetData(unsigned int uiIndex) const
{
    NIASSERT(m_bIsCollection || uiIndex == 0);

    if (uiIndex < m_kDataPointers.GetSize())
    {
        return m_kDataPointers.GetAt(uiIndex);
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
inline size_t NiGeneralComponentProperty::GetDataSizeInBytes(
    unsigned int uiIndex) const
{
    NIASSERT(m_bIsCollection || uiIndex == 0);

    if (uiIndex < m_kDataPointers.GetSize())
    {
        if (m_kDataPointers.GetAt(uiIndex) == NULL)
        {
            return 0;
        }
        else
        {
            return m_kDataSizesInBytes.GetAt(uiIndex);
        }
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
inline NiGeneralComponentProperty* NiGeneralComponentProperty::Clone()
{
    NiGeneralComponentProperty* pkClone = NiNew NiGeneralComponentProperty(
        m_kPropertyName, m_kDisplayName, m_kPrimitiveType, m_kSemanticType,
        m_kDescription, m_bIsCollection);

    unsigned int uiArraySize = m_kDataPointers.GetSize();

    for (unsigned int ui = 0; ui < uiArraySize; ui++)
    {
        pkClone->SetData((char*)m_kDataPointers.GetAt(ui),
            m_kDataSizesInBytes.GetAt(ui), ui);
    }

    return pkClone;
}
//---------------------------------------------------------------------------
