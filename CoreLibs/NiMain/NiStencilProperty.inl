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
//  NiStencilProperty inline functions

//---------------------------------------------------------------------------
inline NiStencilProperty::NiStencilProperty() : 
    m_uFlags(0),
    m_uiRef(0),
    m_uiMask(0xffffffff)
{
    SetStencilOn(false);
    SetStencilFunction(TEST_GREATER);
    SetStencilFailAction(ACTION_KEEP);
    SetStencilPassZFailAction(ACTION_KEEP);
    SetStencilPassAction(ACTION_INCREMENT);
    SetDrawMode(DRAW_CCW_OR_BOTH);
}
//---------------------------------------------------------------------------
inline NiStencilProperty::~NiStencilProperty ()
{
    /* */
}
//---------------------------------------------------------------------------
inline void NiStencilProperty::SetStencilOn(bool bOn)
{
    SetBit(bOn, ENABLE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiStencilProperty::GetStencilOn() const
{
    return GetBit(ENABLE_MASK);
}
//---------------------------------------------------------------------------
inline void NiStencilProperty::SetStencilFunction(TestFunc eFunc)
{
    SetField(NiInt32ToUInt16(eFunc), TESTFUNC_MASK, TESTFUNC_POS);
}
//---------------------------------------------------------------------------
inline NiStencilProperty::TestFunc 
    NiStencilProperty::GetStencilFunction() const
{
    return (TestFunc)GetField(TESTFUNC_MASK, TESTFUNC_POS);
}
//---------------------------------------------------------------------------
inline void NiStencilProperty::SetStencilReference(unsigned int uiRef)
{
    m_uiRef = uiRef;
}
//---------------------------------------------------------------------------
inline unsigned int NiStencilProperty::GetStencilReference() const
{
    return m_uiRef;
}
//---------------------------------------------------------------------------
inline void NiStencilProperty::SetStencilMask(unsigned int uiMask)
{
    m_uiMask = uiMask;
}
//---------------------------------------------------------------------------
inline unsigned int NiStencilProperty::GetStencilMask() const
{
    return m_uiMask;
}
//---------------------------------------------------------------------------
inline void NiStencilProperty::SetStencilPassAction(Action eAct)
{
    SetField(NiInt32ToUInt16(eAct), PASSACTION_MASK, PASSACTION_POS);
}
//---------------------------------------------------------------------------
inline NiStencilProperty::Action 
    NiStencilProperty::GetStencilPassAction() const
{
    return (Action)GetField(PASSACTION_MASK, PASSACTION_POS);
}
//---------------------------------------------------------------------------
inline void NiStencilProperty::SetStencilPassZFailAction(Action eAct)
{
    SetField(NiInt32ToUInt16(eAct), ZFAILACTION_MASK, ZFAILACTION_POS);
}
//---------------------------------------------------------------------------
inline NiStencilProperty::Action 
    NiStencilProperty::GetStencilPassZFailAction() const
{
    return (Action)GetField(ZFAILACTION_MASK, ZFAILACTION_POS);
}
//---------------------------------------------------------------------------
inline void NiStencilProperty::SetStencilFailAction(Action eAct)
{
    SetField(NiInt32ToUInt16(eAct), FAILACTION_MASK, FAILACTION_POS);
}
//---------------------------------------------------------------------------
inline NiStencilProperty::Action 
    NiStencilProperty::GetStencilFailAction() const
{
    return (Action)GetField(FAILACTION_MASK, FAILACTION_POS);
}
//---------------------------------------------------------------------------
inline void NiStencilProperty::SetDrawMode(DrawMode eDraw)
{
    SetField(NiInt32ToUInt16(eDraw), DRAWMODE_MASK, DRAWMODE_POS);
}
//---------------------------------------------------------------------------
inline NiStencilProperty::DrawMode NiStencilProperty::GetDrawMode() const
{
    return (DrawMode)GetField(DRAWMODE_MASK, DRAWMODE_POS);
}
//---------------------------------------------------------------------------
inline bool NiStencilProperty::IsEqualFast(
    const NiStencilProperty& kProp) const
{
    return (m_uFlags == kProp.m_uFlags);
}
//---------------------------------------------------------------------------
inline int NiStencilProperty::Type() const
{
    return NiProperty::STENCIL;
}
//---------------------------------------------------------------------------
inline int NiStencilProperty::GetType()
{
    return NiProperty::STENCIL;
}
//---------------------------------------------------------------------------
inline NiStencilProperty* NiStencilProperty::GetDefault()
{
    return ms_spDefault;
}
//---------------------------------------------------------------------------

