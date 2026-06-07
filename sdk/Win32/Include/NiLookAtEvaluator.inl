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
inline const NiFixedString& NiLookAtEvaluator::GetLookAtObjectName() const
{
    return m_kLookAtObjectName;
}
//---------------------------------------------------------------------------
inline void NiLookAtEvaluator::SetLookAtObjectName(const NiFixedString& kName)
{
    m_kLookAtObjectName = kName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiLookAtEvaluator::GetDrivenObjectName() const
{
    return m_kDrivenObjectName;
}
//---------------------------------------------------------------------------
inline void NiLookAtEvaluator::SetDrivenObjectName(const NiFixedString& kName)
{
    m_kDrivenObjectName = kName;
}
//---------------------------------------------------------------------------
inline bool NiLookAtEvaluator::GetFlip() const
{
    return GetBit(FLIP_MASK);
}
//---------------------------------------------------------------------------
inline void NiLookAtEvaluator::SetFlip(bool bFlip)
{
    SetBit(bFlip,FLIP_MASK);
}
//---------------------------------------------------------------------------
inline NiLookAtEvaluator::Axis NiLookAtEvaluator::GetAxis() const
{
    return (Axis)GetField(AXIS_MASK, AXIS_POS);
}
//---------------------------------------------------------------------------
inline void NiLookAtEvaluator::SetAxis(Axis eAxis)
{
    SetField((unsigned short)eAxis, AXIS_MASK, AXIS_POS);
}
//---------------------------------------------------------------------------
inline void NiLookAtEvaluator::SetTranslateEvaluator(NiEvaluator* pkEval)
{
    if (pkEval)
    {
        NIASSERT(pkEval->GetRawEvalPBChannelType(0) == PBPOINT3CHANNEL);
        m_aspEvaluators[TRANSLATEEVAL] = pkEval;
    }
    else
    {
        m_aspEvaluators[TRANSLATEEVAL] = NULL;
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline void NiLookAtEvaluator::SetRollEvaluator(NiEvaluator* pkEval)
{
    if (pkEval)
    {
        NIASSERT(pkEval->GetRawEvalPBChannelType(0) == PBFLOATCHANNEL);
        m_aspEvaluators[ROLLEVAL] = pkEval;
    }
    else
    {
        m_aspEvaluators[ROLLEVAL] = NULL;
    }
    // No need to call SetEvalChannelTypes(). Roll doesn't affect its outcome.
}
//---------------------------------------------------------------------------
inline void NiLookAtEvaluator::SetScaleEvaluator(NiEvaluator* pkEval)
{
    if (pkEval)
    {
        NIASSERT(pkEval->GetRawEvalPBChannelType(0) == PBFLOATCHANNEL);
        m_aspEvaluators[SCALEEVAL] = pkEval;
    }
    else
    {
        m_aspEvaluators[SCALEEVAL] = NULL;
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline NiEvaluator* NiLookAtEvaluator::GetTranslateEvaluator()
{
    return m_aspEvaluators[TRANSLATEEVAL];
}
//---------------------------------------------------------------------------
inline NiEvaluator* NiLookAtEvaluator::GetRollEvaluator()
{
    return m_aspEvaluators[ROLLEVAL];
}
//---------------------------------------------------------------------------
inline NiEvaluator* NiLookAtEvaluator::GetScaleEvaluator()
{
    return m_aspEvaluators[SCALEEVAL];
}
//---------------------------------------------------------------------------
