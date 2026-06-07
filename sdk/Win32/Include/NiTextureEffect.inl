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
//  NiTextureEffect inline functions

//---------------------------------------------------------------------------
inline const NiMatrix3& NiTextureEffect::GetModelProjectionMatrix() const
{
    return m_kModelProjMat;
}
//---------------------------------------------------------------------------
inline void NiTextureEffect::SetModelProjectionMatrix(const NiMatrix3& kMat)
{
    m_kModelProjMat = kMat;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiTextureEffect::GetModelProjectionTranslation() const
{
    return m_kModelProjTrans;
}
//---------------------------------------------------------------------------
inline void 
NiTextureEffect::SetModelProjectionTranslation(const NiPoint3& kTrans)
{
    m_kModelProjTrans = kTrans;
}
//---------------------------------------------------------------------------
inline const NiMatrix3& NiTextureEffect::GetWorldProjectionMatrix() const
{
    return m_kWorldProjMat;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiTextureEffect::GetWorldProjectionTranslation() const
{
    return m_kWorldProjTrans;
}
//---------------------------------------------------------------------------
inline NiTexture* NiTextureEffect::GetEffectTexture() const
{
    return m_spTexture;
}
//---------------------------------------------------------------------------
inline void NiTextureEffect::SetEffectTexture(NiTexture* pkTexture)
{
    m_spTexture = pkTexture;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::FilterMode 
    NiTextureEffect::GetTextureFilter() const
{
    return m_eFilter;
}
//---------------------------------------------------------------------------
inline void 
    NiTextureEffect::SetTextureFilter(NiTexturingProperty::FilterMode eFilter)
{
    m_eFilter = eFilter;
}
//---------------------------------------------------------------------------
inline unsigned short  
    NiTextureEffect::GetMaxAnisotropy() const
{
    return m_usMaxAnisotropy;
}
//---------------------------------------------------------------------------
inline void NiTextureEffect::SetMaxAnisotropy(unsigned short usMaxAnisotropy)
{
    m_usMaxAnisotropy = usMaxAnisotropy;
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ClampMode 
    NiTextureEffect::GetTextureClamp() const
{
    return m_eClamp;
}
//---------------------------------------------------------------------------
inline void 
    NiTextureEffect::SetTextureClamp(NiTexturingProperty::ClampMode eClamp)
{
    m_eClamp = eClamp;
}
//---------------------------------------------------------------------------
inline NiTextureEffect::TextureType NiTextureEffect::GetTextureType() const
{
    return m_eTextureMode;
}
//---------------------------------------------------------------------------
inline void NiTextureEffect::SetTextureType(TextureType eTexType)
{
    m_eTextureMode = eTexType;
}
//---------------------------------------------------------------------------
inline NiTextureEffect::CoordGenType 
NiTextureEffect::GetTextureCoordGen() const
{
    return m_eCoordMode;
}
//---------------------------------------------------------------------------
inline void NiTextureEffect::SetTextureCoordGen(CoordGenType eGen)
{
    m_eCoordMode = eGen;

    // Texture coordinate generation may change the way the world projection
    // matrix is computed internally, so we must update it
    UpdateProjection();
}
//---------------------------------------------------------------------------
inline bool NiTextureEffect::GetClippingPlaneEnable() const
{
    return m_bPlaneEnable;
}
//---------------------------------------------------------------------------
inline void NiTextureEffect::SetClippingPlaneEnable(bool bEnable)
{
    m_bPlaneEnable = bEnable;
}
//---------------------------------------------------------------------------
inline const NiPlane& NiTextureEffect::GetModelClippingPlane() const
{
    return m_kModelPlane;    
}
//---------------------------------------------------------------------------
inline void NiTextureEffect::SetModelClippingPlane(const NiPlane& kPlane)
{
    m_kModelPlane = kPlane;    

    UpdateProjection();
}
//---------------------------------------------------------------------------
inline const NiPlane& NiTextureEffect::GetWorldClippingPlane() const
{
    return m_kWorldPlane;    
}
//---------------------------------------------------------------------------
