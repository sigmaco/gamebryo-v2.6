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
//  NiZBufferProperty inline functions

//---------------------------------------------------------------------------
inline NiZBufferProperty::NiZBufferProperty() :
    m_uFlags(0)
{
    SetZBufferTest(true);
    SetZBufferWrite(true);
    SetTestFunction(TEST_LESSEQUAL);
}
//---------------------------------------------------------------------------
inline void NiZBufferProperty::SetZBufferTest(bool bZBufferTest)
{
    SetBit(bZBufferTest, ZBUFFERTEST_MASK);
}
//---------------------------------------------------------------------------
inline bool NiZBufferProperty::GetZBufferTest() const
{
    return GetBit(ZBUFFERTEST_MASK);
}
//---------------------------------------------------------------------------
inline void NiZBufferProperty::SetZBufferWrite(bool bZBufferWrite)
{
    SetBit(bZBufferWrite, ZBUFFERWRITE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiZBufferProperty::GetZBufferWrite() const
{
    return GetBit(ZBUFFERWRITE_MASK);
}
//---------------------------------------------------------------------------
inline void NiZBufferProperty::SetTestFunction(TestFunction eTest)
{
    SetField(NiInt32ToUInt16(eTest), TESTFUNCTION_MASK, TESTFUNCTION_POS);
}
//---------------------------------------------------------------------------
inline NiZBufferProperty::TestFunction NiZBufferProperty::GetTestFunction() 
    const
{
    return (TestFunction)GetField(TESTFUNCTION_MASK, TESTFUNCTION_POS);
}
//---------------------------------------------------------------------------
inline bool NiZBufferProperty::IsEqualFast(const NiZBufferProperty& kProp) 
    const
{
    return (m_uFlags == kProp.m_uFlags);
}
//---------------------------------------------------------------------------
inline int NiZBufferProperty::Type() const
{
    return NiProperty::ZBUFFER;
}
//---------------------------------------------------------------------------
inline int NiZBufferProperty::GetType()
{
    return NiProperty::ZBUFFER;
}
//---------------------------------------------------------------------------
inline NiZBufferProperty* NiZBufferProperty::GetDefault()
{
    return ms_spDefault;
}
//---------------------------------------------------------------------------
