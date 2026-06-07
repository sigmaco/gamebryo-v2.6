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
inline void NiScreenFillingRenderViewImpl::RegisterFactoryMethod()
{
    CreateFunc = CreateImpl;
}
//---------------------------------------------------------------------------
inline NiScreenFillingRenderView* NiScreenFillingRenderViewImpl::CreateImpl()
{
    return NiNew NiScreenFillingRenderViewImpl();
}
//---------------------------------------------------------------------------
inline void NiScreenFillingRenderViewImpl::AttachProperty(
    NiProperty* pkProperty)
{
    NIASSERT(pkProperty);
    NIASSERT(m_spScreenFillingQuad);
    m_spScreenFillingQuad->AttachProperty(pkProperty);
    m_bPropertiesChanged = true;
}
//---------------------------------------------------------------------------
inline void NiScreenFillingRenderViewImpl::DetachProperty(
    NiProperty* pkProperty)
{
    NIASSERT(pkProperty);
    NIASSERT(m_spScreenFillingQuad);
    m_spScreenFillingQuad->DetachProperty(pkProperty);
    m_bPropertiesChanged = true;
}
//---------------------------------------------------------------------------
inline void NiScreenFillingRenderViewImpl::DetachAllProperties()
{
    NIASSERT(m_spScreenFillingQuad);
    m_spScreenFillingQuad->DetachAllProperties();
    m_bPropertiesChanged = true;
}
//---------------------------------------------------------------------------
inline NiProperty* NiScreenFillingRenderViewImpl::GetProperty(int iType) const
{
    NIASSERT(m_spScreenFillingQuad);
    return m_spScreenFillingQuad->GetProperty(iType);
}
//---------------------------------------------------------------------------
inline NiPropertyPtr NiScreenFillingRenderViewImpl::RemoveProperty(int iType)
{
    NIASSERT(m_spScreenFillingQuad);
    m_bPropertiesChanged = true;
    return m_spScreenFillingQuad->RemoveProperty(iType);
}
//---------------------------------------------------------------------------
inline NiRenderObject& 
NiScreenFillingRenderViewImpl::GetScreenFillingQuad() const
{
    NIASSERT(m_spScreenFillingQuad);
    return *m_spScreenFillingQuad;
}
//---------------------------------------------------------------------------
