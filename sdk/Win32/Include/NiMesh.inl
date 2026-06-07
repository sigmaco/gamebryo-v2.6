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
inline void NiMesh::SetModelBound(const NiBound& kBound)
{
    m_kBound = kBound;
}
//---------------------------------------------------------------------------
inline const NiBound& NiMesh::GetModelBound() const
{
    return m_kBound;
}
//---------------------------------------------------------------------------
inline void NiMesh::SetPrimitiveType(
    NiPrimitiveType::Type ePrimitiveType)
{
    m_eMeshPrimitiveType = ePrimitiveType;
}
//---------------------------------------------------------------------------
inline NiPrimitiveType::Type NiMesh::GetPrimitiveType() const
{
    return m_eMeshPrimitiveType;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMesh::GetSubmeshCount() const
{
    return m_uiSubmeshCount;
}
//---------------------------------------------------------------------------
inline void NiMesh::SetSubmeshCount(NiUInt32 uiSubmeshCount,
    NiUInt16 uiInitialValue)
{
    m_uiSubmeshCount = (NiUInt16) uiSubmeshCount;
    for (NiUInt32 ui = 0; ui < m_kDataStreams.GetSize(); ui++)
    {
        m_kDataStreams.GetAt(ui)->SetSubmeshCount(uiSubmeshCount,
            uiInitialValue);
    }
}
//---------------------------------------------------------------------------
inline const NiDataStreamRef* NiMesh::FindStreamRef(
    const NiFixedString& kSemantic, NiUInt32 uiSemanticIndex, 
    NiDataStreamElement::Format eFormat) const
{
    const NiDataStreamRef* pkRef = NULL;
    NiDataStreamElement kElement;

    if (FindStreamRefAndElementBySemantic(kSemantic, uiSemanticIndex, eFormat,
        pkRef, kElement))
    {
        return pkRef;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
inline NiDataStreamRef* NiMesh::FindStreamRef(
    const NiFixedString& kSemantic, NiUInt32 uiSemanticIndex, 
    NiDataStreamElement::Format eFormat)
{
    NiDataStreamRef* pkRef = NULL;
    NiDataStreamElement kElement;

    if (FindStreamRefAndElementBySemantic(kSemantic, uiSemanticIndex, eFormat,
        pkRef, kElement))
    {
        return pkRef;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
inline bool NiMesh::FindStreamRefAndElementBySemantic(
    const NiFixedString& kSemantic,
    NiUInt32 uiSemanticIndex, 
    NiDataStreamElement::Format eFormat, 
    const NiDataStreamRef*& pkRef, 
    NiDataStreamElement& kElement) const
{
    pkRef = NULL;

    NiUInt32 uiStreamIdx = UINT_MAX;
    NiUInt32 uiElementIdx = UINT_MAX;
    if (FindStreamAndElementIndex(kSemantic, uiSemanticIndex,
        uiStreamIdx, uiElementIdx))
    {
        const NiDataStreamRef* pkFoundRef = m_kDataStreams.GetAt(uiStreamIdx);
        const NiDataStreamElement& kFoundElement = 
            pkFoundRef->GetElementDescAt(uiElementIdx);

        if (eFormat != NiDataStreamElement::F_UNKNOWN)
        {
            if (kFoundElement.GetFormat() == eFormat)
            {
                pkRef = pkFoundRef;
                kElement = kFoundElement;
                return true;
            }
            else 
            {
                return false;
            }
        }
        else
        {
            pkRef = pkFoundRef;
            kElement = kFoundElement;
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
inline bool NiMesh::FindStreamRefAndElementBySemantic(
    const NiFixedString& kSemantic,
    NiUInt32 uiSemanticIndex, 
    NiDataStreamElement::Format eFormat, 
    NiDataStreamRef*& pkRef, 
    NiDataStreamElement& kElement)
{
    pkRef = NULL;

    NiUInt32 uiStreamIdx = UINT_MAX;
    NiUInt32 uiElementIdx = UINT_MAX;
    if (FindStreamAndElementIndex(kSemantic, uiSemanticIndex,
        uiStreamIdx, uiElementIdx))
    {
        NiDataStreamRef* pkFoundRef = m_kDataStreams.GetAt(uiStreamIdx);
        const NiDataStreamElement& kFoundElement = 
            pkFoundRef->GetElementDescAt(uiElementIdx);

        if (eFormat != NiDataStreamElement::F_UNKNOWN)
        {
            if (kFoundElement.GetFormat() == eFormat)
            {
                pkRef = pkFoundRef;
                kElement = kFoundElement;
                return true;
            }
            else 
            {
                return false;
            }
        }
        else
        {
            pkRef = pkFoundRef;
            kElement = kFoundElement;
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
inline bool NiMesh::FindStreamAndElementIndex(
    const NiFixedString& kSemanticName, NiUInt32 uiSemanticIdx, 
    NiUInt32& uiStreamIdx, NiUInt32& uiElementIdx) const
{
    for (NiUInt32 ui = 0; ui < m_kDataStreams.GetSize(); ui++)
    {
        NiUInt32 uiIndex = m_kDataStreams.GetAt(ui)->FindElementDescIndex(
            kSemanticName, uiSemanticIdx);
        
        if (uiIndex != UINT_MAX)
        {
            uiStreamIdx = ui;
            uiElementIdx = uiIndex;
            return true;
        }
    }

    uiStreamIdx = UINT_MAX;
    uiElementIdx = UINT_MAX;
    return false;
}
//---------------------------------------------------------------------------
inline const NiDataStreamRef* NiMesh::GetFirstStreamRef(
    NiDataStream::Usage eUsage, 
    NiUInt32 uiAccessRequiredMask) const
{
    for (NiUInt32 ui = 0; ui < m_kDataStreams.GetSize(); ui++)
    {
        const NiDataStreamRef* pkRef = m_kDataStreams.GetAt(ui);

        if (pkRef->GetUsage() == eUsage)
        {
            if (pkRef->GetAccessMask() & uiAccessRequiredMask)
            {
                return pkRef;
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline NiDataStreamRef* NiMesh::GetFirstStreamRef(
    NiDataStream::Usage eUsage, 
    NiUInt32 uiAccessRequiredMask)
{
    // Call const version of function - cast to be non-const
    return const_cast<NiDataStreamRef*>(
        ((const NiMesh*)this)->GetFirstStreamRef(
            eUsage, uiAccessRequiredMask));
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMesh::GetCount(NiUInt32 uiSubmesh, 
    const NiFixedString& kSemantic, NiUInt32 uiSemanticIndex) const
{
    const NiDataStreamRef* pkStreamRef = 
        FindStreamRef(kSemantic, uiSemanticIndex);

    if (!pkStreamRef)
        return 0;

    return pkStreamRef->GetCount(uiSubmesh);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMesh::GetTotalCount(const NiFixedString&
        kSemantic, NiUInt32 uiSemanticIndex) const
{
    NiUInt32 uiTotal = 0;
    for(NiUInt32 ui=0; ui < GetSubmeshCount(); ui++)
    {
        uiTotal += GetCount(ui, kSemantic, uiSemanticIndex);
    }
    return uiTotal;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMesh::GetStreamRefCount() const
{
    return m_kDataStreams.GetSize();
}
//---------------------------------------------------------------------------
inline const NiDataStreamRef* NiMesh::GetStreamRefAt(
    NiUInt32 uiStreamRefIdx) const
{
    NIASSERT(uiStreamRefIdx < GetStreamRefCount());
    return m_kDataStreams.GetAt(uiStreamRefIdx);
}
//---------------------------------------------------------------------------
inline NiDataStreamRef* NiMesh::GetStreamRefAt(NiUInt32 uiStreamRefIdx)
{
    NIASSERT(uiStreamRefIdx < GetStreamRefCount());
    return m_kDataStreams.GetAt(uiStreamRefIdx);
}
//---------------------------------------------------------------------------
inline NiDataStreamRef* NiMesh::AddStreamRef(NiDataStream* pkStream,
    const NiFixedString& kSemanticName, NiUInt32 uiSemanticIndex,
    bool bPerInstanceStream)
{
    NiDataStreamRef* pkRef = NiNew NiDataStreamRef();
    m_kDataStreams.Add(pkRef);
    
    NIASSERT(pkStream);
    pkRef->SetSubmeshCount(m_uiSubmeshCount, 0);
    
    pkRef->SetDataStream(pkStream);
    
    NIASSERT(pkStream && pkStream->GetElementDescCount() == 1);
    pkRef->BindSemanticToElementDescAt(0, kSemanticName, uiSemanticIndex);
    
    pkRef->SetPerInstance(bPerInstanceStream);

    return pkRef;    
}
//---------------------------------------------------------------------------
inline NiDataStreamRef* NiMesh::AddStreamRef()
{
    NiDataStreamRef* pkRef = NiNew NiDataStreamRef();
    m_kDataStreams.Add(pkRef);
    pkRef->SetSubmeshCount(m_uiSubmeshCount);
    return pkRef;
}
//---------------------------------------------------------------------------
inline NiDataStreamRef* NiMesh::AddStreamRef(const NiDataStreamRef* pkRef)
{
    if (!pkRef)
        return NULL;

    // Let the copy constructor do most of the work for us
    NiDataStreamRef* pkCopyRef = NiNew NiDataStreamRef(*pkRef);
    m_kDataStreams.Add(pkCopyRef);

    // Make sure the submesh counts line up
    pkCopyRef->SetSubmeshCount(m_uiSubmeshCount, 0);
    return pkCopyRef;
}
//---------------------------------------------------------------------------
inline NiDataStreamElementLock NiMesh::AddStreamGetLock(
    const NiFixedString& kSemantic, NiUInt32 uiSemanticIndex, 
    NiDataStreamElement::Format eFormat, NiUInt32 uiCount, 
    NiUInt8 uiAccessMask, NiDataStream::Usage eUsage, 
    bool bForceToolDSCreate, bool bCreateDefaultRegion,
    NiUInt8 uiLockMask)
{
    AddStream(kSemantic, uiSemanticIndex, eFormat, uiCount, uiAccessMask, 
        eUsage, 0, bForceToolDSCreate, bCreateDefaultRegion);

    return NiDataStreamElementLock(this, kSemantic, uiSemanticIndex,
        eFormat, uiLockMask);
}
//---------------------------------------------------------------------------
inline void NiMesh::RemoveStreamRef(const NiDataStreamRef* pkRef)
{
    for (NiUInt32 ui = 0; ui < m_kDataStreams.GetSize(); ui++)
    {
        if (m_kDataStreams.GetAt(ui) == pkRef)
        {
            NiDelete pkRef;
            m_kDataStreams.OrderedRemoveAt(ui);
            break;
        }
    }
}
//---------------------------------------------------------------------------
inline void NiMesh::RemoveAllStreamRefs()
{
    for (NiUInt32 ui = 0; ui < m_kDataStreams.GetSize(); ui++)
    {
        NiDataStreamRef* pkRef = m_kDataStreams.GetAt(ui);
        NiDelete pkRef;
    }
    m_kDataStreams.RemoveAll();
}
//---------------------------------------------------------------------------
inline void NiMesh::RemoveOutputStreamsForMaterial(
    const NiMaterial* pkMaterial)
{
    NiUInt32 i = 0;
    while (i < m_kOutputDataStreamsByMaterial.GetSize())
    {
        OutputStream* pkOutputStream = m_kOutputDataStreamsByMaterial.GetAt(i);
        if (pkOutputStream && pkOutputStream->m_pkMaterial == pkMaterial)
        {
            NiDelete pkOutputStream;
            m_kOutputDataStreamsByMaterial.RemoveAt(i);
        }
        else
        {
            // Only advance to the next index if an item wasn't removed, 
            // since we're iterating over an NiTSet removing an item will
            // copy the last item in the set to the current index.
            i++;
        }
    }
}
//---------------------------------------------------------------------------
inline void NiMesh::RemoveOutputStreamsForAllMaterials()
{
    const NiUInt32 uiCount = m_kOutputDataStreamsByMaterial.GetSize();
    for (NiUInt32 i = 0; i < uiCount; i++)
    {
        OutputStream* pkOutputStream = m_kOutputDataStreamsByMaterial.GetAt(i);
        NiDelete pkOutputStream;
    }

    m_kOutputDataStreamsByMaterial.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMesh::GetPrimitiveCount(NiUInt32 uiSubmesh) const
{
    if (uiSubmesh == 0 && m_uiFirstSubmeshPrimitiveCount != 0xFFFFFFFF)
        return  m_uiFirstSubmeshPrimitiveCount;

    return GetPrimitiveCountHelper(uiSubmesh);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMesh::GetPrimitiveCountHelper(NiUInt32 uiSubmesh) const
{
    NiUInt32 uiCount = 0;

    // Determine primitive count from index stream. If index stream
    // does not exist then use position stream. If that fails return 0
    uiCount = GetCount(uiSubmesh, NiCommonSemantics::INDEX(), 0);
    if (uiCount == 0)
    {
        uiCount = GetCount(
            uiSubmesh, NiCommonSemantics::POSITION(), 0);
    }

    return (uiCount > 0) ? 
        GetPrimitiveCountFromElementCount(uiCount) : 0;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMesh::GetTotalPrimitiveCount() const
{
    NiUInt32 uiTotal = 0;
    for(NiUInt32 ui=0; ui < GetSubmeshCount(); ui++)
    {
        uiTotal += GetPrimitiveCount(ui);
    }
    return uiTotal;
}
//---------------------------------------------------------------------------
inline void NiMesh::UpdateCachedPrimitiveCount()
{
    m_uiFirstSubmeshPrimitiveCount = 0xFFFFFFFF;
    m_uiFirstSubmeshPrimitiveCount = GetPrimitiveCount(0);
}
//---------------------------------------------------------------------------
inline void NiMesh::SetInstanced(bool bInstanced)
{
    if (m_bInstancingEnabled == bInstanced)
        return;
    
    m_bInstancingEnabled = bInstanced;

    SetMaterialNeedsUpdate(true);
}
//---------------------------------------------------------------------------
inline const bool NiMesh::GetInstanced() const
{
    return m_bInstancingEnabled;
}
//---------------------------------------------------------------------------
inline void NiMesh::SetBaseInstanceStream(NiDataStreamRef* pkStream)
{   
    m_pkBaseInstanceStream = pkStream;
}
//---------------------------------------------------------------------------
inline NiDataStreamRef* NiMesh::GetBaseInstanceStream() const
{
    return m_pkBaseInstanceStream;
}
//---------------------------------------------------------------------------
inline void NiMesh::SetVisibleInstanceStream(NiDataStreamRef* pkStream)
{
    m_pkVisibleInstanceStream = pkStream;
}
//---------------------------------------------------------------------------
inline NiDataStreamRef* NiMesh::GetVisibleInstanceStream() const
{
    return m_pkVisibleInstanceStream;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMesh::GetSemanticCount(const NiFixedString& kSemantic)
    const
{
    NiUInt32 uiReturn = 0;
    for (NiUInt32 ui = 0; ui < m_kDataStreams.GetSize(); ui++)
    {
        uiReturn += m_kDataStreams.GetAt(ui)->GetSemanticCount(kSemantic);
    }
    return uiReturn;
}
//---------------------------------------------------------------------------
inline NiInt32 NiMesh::GetMaxIndexBySemantic(
    const NiFixedString& kSemantic) const
{
    NiInt32 iReturn = -1;
    for (NiUInt32 ui = 0; ui < m_kDataStreams.GetSize(); ui++)
    {
        NiInt32 iValue = m_kDataStreams.GetAt(ui)->GetMaxIndexBySemantic(
            kSemantic);

        if (iValue > iReturn)
            iReturn = iValue;
    }
    return iReturn;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiMesh::GetModifierCount() const
{
    return m_uiNumModifiers;
}
//---------------------------------------------------------------------------
inline NiMeshModifier* NiMesh::GetModifierAt(NiUInt32 uiModifierIndex) const
{
    NIASSERT(uiModifierIndex < m_uiNumModifiers);
    return m_aspMeshModifiers[uiModifierIndex];
}
//---------------------------------------------------------------------------
inline bool NiMesh::AddModifier(NiMeshModifier* pkModifier, bool bAttach)
{
    return AddModifierAt(m_uiNumModifiers, pkModifier, bAttach);
}
//---------------------------------------------------------------------------
inline void NiMesh::SetUsePreloading(bool bPreload)
{
    ms_bPreload = bPreload;
}
//---------------------------------------------------------------------------
inline bool NiMesh::GetUsePreloading()
{
    return ms_bPreload;
}
//---------------------------------------------------------------------------
inline void NiMesh::SetDestroyAppDataFlag(bool bDestroy)
{
    ms_bDestroyAppData = bDestroy;
}
//---------------------------------------------------------------------------
inline bool NiMesh::GetDestroyAppDataFlag()
{
    return ms_bDestroyAppData;
}
//---------------------------------------------------------------------------
inline bool NiMesh::GetInputDataIsFromStreamOut() const
{ 
    return m_bInputDataIsFromStreamOut; 
}
//---------------------------------------------------------------------------
inline void NiMesh::SetInputDataIsFromStreamOut(bool bInputDataIsFromStreamOut)
{ 
    m_bInputDataIsFromStreamOut = bInputDataIsFromStreamOut; 
}
//---------------------------------------------------------------------------
#define NiGetModifier(classname, pkMesh) \
    ((classname*) (pkMesh)->GetModifierByType(&classname::ms_RTTI))
//---------------------------------------------------------------------------

