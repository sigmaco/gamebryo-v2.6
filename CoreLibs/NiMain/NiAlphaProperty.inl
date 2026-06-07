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
//  NiAlphaProperty inline functions
//---------------------------------------------------------------------------
inline NiAlphaProperty::NiAlphaProperty()
{
    m_uFlags = 0;
    SetAlphaBlending(false);
    SetSrcBlendMode(ALPHA_SRCALPHA);
    SetDestBlendMode(ALPHA_INVSRCALPHA);
    SetAlphaTesting(false);
    SetTestMode(TEST_ALWAYS);
    SetTestRef(0);
}
//---------------------------------------------------------------------------
inline void NiAlphaProperty::SetAlphaBlending(bool bAlpha)
{
    SetBit(bAlpha, ALPHA_BLEND_MASK);
}
//---------------------------------------------------------------------------
inline bool NiAlphaProperty::GetAlphaBlending() const
{
    return GetBit(ALPHA_BLEND_MASK);
}
//---------------------------------------------------------------------------
inline void NiAlphaProperty::SetSrcBlendMode(AlphaFunction eSrcBlend) 
{ 
    SetField(NiInt32ToUInt16(eSrcBlend), SRC_BLEND_MASK, SRC_BLEND_POS);
}
//---------------------------------------------------------------------------
inline NiAlphaProperty::AlphaFunction 
NiAlphaProperty::GetSrcBlendMode() const
{ 
    return (NiAlphaProperty::AlphaFunction) 
        GetField(SRC_BLEND_MASK, SRC_BLEND_POS);
}
//---------------------------------------------------------------------------
inline void NiAlphaProperty::SetDestBlendMode(AlphaFunction eDestBlend)
{ 
    SetField(NiInt32ToUInt16(eDestBlend), DEST_BLEND_MASK, DEST_BLEND_POS);
}
//---------------------------------------------------------------------------
inline NiAlphaProperty::AlphaFunction NiAlphaProperty::GetDestBlendMode() 
    const
{ 
    return (NiAlphaProperty::AlphaFunction) 
        GetField(DEST_BLEND_MASK, DEST_BLEND_POS);
}
//---------------------------------------------------------------------------
inline void NiAlphaProperty::SetAlphaTesting(bool bAlpha)
{
    SetBit(bAlpha, TEST_ENABLE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiAlphaProperty::GetAlphaTesting() const
{
    return GetBit(TEST_ENABLE_MASK);
}
//---------------------------------------------------------------------------
inline void NiAlphaProperty::SetTestMode(TestFunction eTestFunc)
{ 
    SetField(NiInt32ToUInt16(eTestFunc), TEST_FUNC_MASK, TEST_FUNC_POS);
}
//---------------------------------------------------------------------------
inline NiAlphaProperty::TestFunction NiAlphaProperty::GetTestMode() const
{ 
    return (NiAlphaProperty::TestFunction) 
        GetField(TEST_FUNC_MASK, TEST_FUNC_POS);
}
//---------------------------------------------------------------------------
inline void NiAlphaProperty::SetTestRef(unsigned char ucRef)
{ 
    m_ucAlphaTestRef = ucRef;
}
//---------------------------------------------------------------------------
inline unsigned char NiAlphaProperty::GetTestRef() const
{ 
    return m_ucAlphaTestRef;
}
//---------------------------------------------------------------------------
inline void NiAlphaProperty::SetNoSorter(bool bNoSort)
{
    SetBit(bNoSort, ALPHA_NOSORTER_MASK);
}
//---------------------------------------------------------------------------
inline bool NiAlphaProperty::GetNoSorter() const
{
    return GetBit(ALPHA_NOSORTER_MASK);
}
//---------------------------------------------------------------------------
inline bool NiAlphaProperty::IsEqualFast(const NiAlphaProperty& kProp) const
{
    return m_uFlags == kProp.m_uFlags && 
        m_ucAlphaTestRef == kProp.m_ucAlphaTestRef;
}
//---------------------------------------------------------------------------
inline int NiAlphaProperty::Type() const
{
    return NiProperty::ALPHA;
}
//---------------------------------------------------------------------------
inline int NiAlphaProperty::GetType()
{
    return NiProperty::ALPHA;
}
//---------------------------------------------------------------------------
inline NiAlphaProperty* NiAlphaProperty::GetDefault()
{
    return ms_spDefault;
}
//---------------------------------------------------------------------------
