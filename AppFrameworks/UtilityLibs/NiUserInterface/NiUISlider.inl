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

#include <NiMath.h>

//---------------------------------------------------------------------------
inline void NiUISlider::IncrementPressed(unsigned char ucButtonState)
{
    if (ucButtonState & NiUIManager::WASPRESSED)
    {
        m_fLastStepTime = 0.0f;
    }

    float fCurrentTime = NiGetCurrentTimeInSec();
    if (fCurrentTime - m_fLastStepTime >= m_fTimeStepIncrement)
    {
        m_fLastStepTime = fCurrentTime;
        SetStep(m_iStep + 1);
    }
}
//---------------------------------------------------------------------------
inline void NiUISlider::DecrementPressed(unsigned char ucButtonState)
{
    if (ucButtonState & NiUIManager::WASPRESSED)
    {
        m_fLastStepTime = 0.0f;
    }

    float fCurrentTime = NiGetCurrentTimeInSec();
    if (fCurrentTime - m_fLastStepTime >= m_fTimeStepIncrement)
    {
        m_fLastStepTime = fCurrentTime;
        SetStep(m_iStep - 1);
    }
}
//---------------------------------------------------------------------------
inline void NiUISlider::HandleCursor(unsigned char ucButtonState)
{
    NiUIManager* pkManager = NiUIManager::GetUIManager();
    if (ucButtonState & NiUIManager::ISPRESSED)
    {
        NiCursor* pkCursor = pkManager->GetCursor();
        if (pkCursor)
        {
            int iX, iY;
            pkCursor->GetScreenSpacePosition(iX, iY);

            NiRenderer* pkRenderer = NiRenderer::GetRenderer();
            if (pkRenderer)
            {
                float fX, fY;
                pkRenderer->ConvertFromPixelsToNDC(
                    (unsigned int)iX, (unsigned int) iY, fX, fY);
                NiRect<float> kSliderNDC =
                    pkManager->ConvertNSCToNDC(m_kTrackOuterRect);

                // This should only be called when this is the focused entry.
                float fNormalizedPos = (fX - kSliderNDC.m_left) / 
                    kSliderNDC.GetWidth();

                int iStep = (int)(fNormalizedPos * m_uiMaxStep + 0.5f);
                SetStep(iStep);
            }
        }
    }
}
//---------------------------------------------------------------------------
inline void NiUISlider::LeftMouseButton(unsigned char ucButtonState)
{
    HandleCursor(ucButtonState);
}
//---------------------------------------------------------------------------
inline void NiUISlider::EnterPressed(unsigned char 
#if defined(_WII)
ucButtonState
#endif
)
{
#if defined(_WII)
    HandleCursor(ucButtonState);
#endif
}
//---------------------------------------------------------------------------
inline void NiUISlider::SetPressed(bool bPressed)
{
    m_bPressed = bPressed;

    ReinitializeDisplayElements();
}
//---------------------------------------------------------------------------
inline bool NiUISlider::IsPressed() const
{
    return m_bPressed;
}
//---------------------------------------------------------------------------
inline void NiUISlider::SetText(const char* pcText)
{
    m_kSliderName = pcText;
    UpdateLabelText();
}
//---------------------------------------------------------------------------
inline void NiUISlider::SubscribeToValueChangedEvent(
    NiUIBaseSlot1<float>* pkSlot)
{
    m_kValueChangedEvent.Subscribe(pkSlot);
}
//---------------------------------------------------------------------------
inline void NiUISlider::Draw(NiRenderer* pkRenderer)
{
    if (m_bVisible)
    {
        m_spLabel->Draw(pkRenderer);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiUISlider::NumQuadsRequired()
{
    return NUM_SLIDER_QUADS;
}
//---------------------------------------------------------------------------
inline void NiUISlider::SetVisible(bool bVisible)
{
    NiUIBaseElement::SetVisible(bVisible);

    //m_spLabel->SetVisible(bVisible);
}
//---------------------------------------------------------------------------
inline void NiUISlider::SetValue(float fValue)
{
    float fNormalizedValue;
    
    if (m_fMax == m_fMin)
    {
        fNormalizedValue = 0.5f;
    }
    else
    {
        fNormalizedValue = (fValue - m_fMin) / (m_fMax - m_fMin);
    }

    // Convert to step
    int iStep = (int)(fNormalizedValue * m_uiMaxStep + 0.5f);
    SetStep(iStep);
}
//---------------------------------------------------------------------------
inline void NiUISlider::SetStep(int iValue)
{
    m_iStep = NiClamp(iValue, 0, m_uiMaxStep);

    // Calculate value
    float fNormalizedValue = m_iStep / (float)m_uiMaxStep;
    m_fValue = NiLerp(fNormalizedValue, m_fMin, m_fMax);

    // Emit and Update Component
    m_kValueChangedEvent.EmitSignal(m_fValue);
    UpdateLabelText();
    ReinitializeDisplayElements();
}
//---------------------------------------------------------------------------
inline void NiUISlider::SetRange(float fMin, float fMax, 
    unsigned int uiNumSteps, bool bUpdateValue)
{
    NIASSERT(fMin <= fMax);
    m_fMin = fMin;
    m_fMax = fMax;
    m_uiMaxStep = NiMax(uiNumSteps, 2) - 1;

    if (bUpdateValue)
        SetValue(m_fValue);
}
//---------------------------------------------------------------------------
