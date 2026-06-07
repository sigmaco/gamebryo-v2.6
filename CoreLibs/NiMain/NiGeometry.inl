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
//  NiRenderObject inline functions

//---------------------------------------------------------------------------
inline NiGeometryData* NiGeometry::GetModelData() const
{
    return m_spModelData;
}
//---------------------------------------------------------------------------
inline unsigned short NiGeometry::GetVertexCount() const
{
    return m_spModelData->GetVertexCount();
}
//---------------------------------------------------------------------------
inline NiPoint3* NiGeometry::GetVertices() const
{
    return m_spModelData->GetVertices();
}
//---------------------------------------------------------------------------
inline NiPoint3* NiGeometry::GetNormals() const
{
    return m_spModelData->GetNormals();
}
//---------------------------------------------------------------------------
inline NiShaderRequirementDesc::NBTFlags
    NiGeometry::GetNormalBinormalTangentMethod() const
{
    return m_spModelData->GetNormalBinormalTangentMethod();
}
//---------------------------------------------------------------------------
inline void NiGeometry::StoreNormalBinormalTangent(
    NiShaderRequirementDesc::NBTFlags eNBTMethod)
{
    m_spModelData->StoreNormalBinormalTangent(eNBTMethod);
}
//---------------------------------------------------------------------------
inline void NiGeometry::SetModelBound(const NiBound& kBound)
{
    m_spModelData->SetBound(kBound);
}
//---------------------------------------------------------------------------
inline NiBound& NiGeometry::GetModelBound() const
{
    return m_spModelData->GetBound();
}
//---------------------------------------------------------------------------
inline void NiGeometry::CreateColors(bool bInitialize)
{
    m_spModelData->CreateColors(bInitialize);
}
//---------------------------------------------------------------------------
inline NiColorA* NiGeometry::GetColors() const
{
    return m_spModelData->GetColors();
}
//---------------------------------------------------------------------------
inline void NiGeometry::CreateTextures(bool bInitialize,
    unsigned short usSets)
{
    m_spModelData->CreateTextures(bInitialize,usSets);
}
//---------------------------------------------------------------------------
inline NiUInt16 NiGeometry::GetTextureSets() const
{
    return m_spModelData->GetTextureSets();
}
//---------------------------------------------------------------------------
inline NiPoint2* NiGeometry::GetTextures() const
{
    return m_spModelData->GetTextures();
}
//---------------------------------------------------------------------------
inline NiPoint2* NiGeometry::GetTextureSet(unsigned short usSet) const
{
    return m_spModelData->GetTextureSet(usSet);
}
//---------------------------------------------------------------------------
inline NiSkinInstance* NiGeometry::GetSkinInstance() const
{
    return m_spSkinInstance;
}
//---------------------------------------------------------------------------
inline void NiGeometry::SetSkinInstance(NiSkinInstance* pSkinInstance)
{
    m_spSkinInstance = pSkinInstance;
}
//---------------------------------------------------------------------------
inline NiAdditionalGeometryData* NiGeometry::GetAdditionalGeometryData()
{
    if (m_spModelData)
    {
        return m_spModelData->GetAdditionalGeometryData();
    }
    return NULL;
}
//---------------------------------------------------------------------------
inline void NiGeometry::SetAdditionalGeometryData(
    NiAdditionalGeometryData* pkData)
{
    NIASSERT(m_spModelData);
    m_spModelData->SetAdditionalGeometryData(pkData);
}
//---------------------------------------------------------------------------
inline void NiGeometry::SetSortObject(bool bSort)
{
    // Explicit scoping is used here, as a function of the same name is 
    // declared as protected in NiAVObject, and it is the AVObject version
    // that we are wrapping with a public interface for NiRenderObject.  This
    // avoids bloating NiRenderObject with a flag vector
    NiAVObject::SetSortObject(bSort);
}
//---------------------------------------------------------------------------
inline bool NiGeometry::GetSortObject() const
{
    // Explicit scoping is used here, as a function of the same name is 
    // declared as protected in NiAVObject, and it is the AVObject version
    // that we are wrapping with a public interface for NiRenderObject.  This
    // avoids bloating NiRenderObject with a flag vector
    return NiAVObject::GetSortObject();
}
//---------------------------------------------------------------------------
inline bool NiGeometry::IsMaterialApplied(const NiFixedString& kName) const
{
    return IsMaterialApplied(NiMaterial::GetMaterial(kName));
}
//---------------------------------------------------------------------------
inline bool NiGeometry::SetActiveMaterial(const NiFixedString& kName)
{
    return SetActiveMaterial(NiMaterial::GetMaterial(kName));
}
//---------------------------------------------------------------------------
inline bool NiGeometry::SetActiveMaterial(const NiMaterial* pkMaterial)
{
    unsigned int uiNumMaterials = m_kMaterials.GetSize();
    for (unsigned int i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterialInstance& kMatInst = m_kMaterials.GetAt(i);
        if (kMatInst.GetMaterial() == pkMaterial)
        {
            m_uiActiveMaterial = i;
            return true;
        }
    }

    m_uiActiveMaterial = (unsigned int) NO_MATERIAL;
    return false;
}
//---------------------------------------------------------------------------
inline const NiMaterial* NiGeometry::GetActiveMaterial() const
{
    if (m_uiActiveMaterial >= m_kMaterials.GetSize())
        return NULL;

    const NiMaterialInstance& kMatInst = 
        m_kMaterials.GetAt(m_uiActiveMaterial);
    return kMatInst.GetMaterial();
}
//---------------------------------------------------------------------------
inline bool NiGeometry::GetStreamingAppliesDefaultMaterial()
{
    return ms_bStreamingAppliesDefaultMaterial;
}
//---------------------------------------------------------------------------
inline void NiGeometry::SetStreamingAppliesDefaultMaterial(bool bApplyDefault)
{
    ms_bStreamingAppliesDefaultMaterial = bApplyDefault;
}
//---------------------------------------------------------------------------
inline bool NiGeometry::GetDefaultMaterialNeedsUpdateFlag() const
{
    return m_bDefaultMaterialDirtyFlag;
}
//---------------------------------------------------------------------------
inline void NiGeometry::SetActiveVertexCount(unsigned short usActive)
{
    m_spModelData->SetActiveVertexCount(usActive);
}
//---------------------------------------------------------------------------
inline unsigned short NiGeometry::GetActiveVertexCount() const
{
    return m_spModelData->GetActiveVertexCount();
}
//---------------------------------------------------------------------------
inline NiPropertyStatePtr NiGeometry::GetPropertyState() const
{
    return m_spPropertyState;
}
//---------------------------------------------------------------------------
inline NiDynamicEffectStatePtr NiGeometry::GetEffectState() const
{
    return m_spEffectState;
}
//---------------------------------------------------------------------------
inline void NiGeometry::SetPropertyState(NiPropertyState* pkState)
{
    m_spPropertyState = pkState;
}
//---------------------------------------------------------------------------
inline void NiGeometry::SetEffectState(NiDynamicEffectState* pkState)
{
    m_spEffectState = pkState;
}
//---------------------------------------------------------------------------
inline NiGeometryData::Consistency NiGeometry::GetConsistency() const
{
    return m_spModelData->GetConsistency();
}
//---------------------------------------------------------------------------
inline void NiGeometry::SetConsistency(
    NiGeometryData::Consistency eConsistency)
{
    m_spModelData->SetConsistency(eConsistency);
}
//---------------------------------------------------------------------------
inline NiShaderDeclaration* NiGeometry::GetShaderDecl()
{
    return m_spShaderDecl;
}
//---------------------------------------------------------------------------
inline void NiGeometry::SetShaderDecl(NiShaderDeclaration* pkDecl)
{
    m_spShaderDecl = pkDecl;
}
//---------------------------------------------------------------------------
