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

// Precompiled Header
#include "NiApplicationPCH.h"

#include "NiFrameRate.h"
#include <NiAlphaProperty.h>
#include <NiPixelData.h>
#include <NiPixelFormat.h>
#include <NiSystem.h>
#include <NiTexturingProperty.h>
#include <NiVertexColorProperty.h>
#include <NiRenderTargetGroup.h>

//---------------------------------------------------------------------------
unsigned short NiFrameRate::ms_aausNumberBits[NUM_NUMS][DIGIT_HEIGHT] = 
{
    { 0x3c, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
      0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e, 0x3c }, // 0
    { 0x18, 0x38, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18,
      0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c }, // 1
    { 0x3c, 0x7e, 0xe7, 0xc3, 0x83, 0x03, 0x03, 0x03,
      0x06, 0x0e, 0x1c, 0x38, 0x70, 0xe0, 0xff, 0xff }, // 2
    { 0x3c, 0x7e, 0xe7, 0xc3, 0x03, 0x03, 0x06, 0x1c, 
      0x1c, 0x06, 0x03, 0x03, 0xc3, 0xe7, 0x7e, 0x3c }, // 3
    { 0x0e, 0x1e, 0x1e, 0x36, 0x36, 0x66, 0x66, 0xc6, 
      0xc6, 0xff, 0xff, 0x06, 0x06, 0x06, 0x06, 0x0f }, // 4
    { 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xfe, 
      0xc7, 0x03, 0x03, 0x03, 0x03, 0xc7, 0xfe, 0x7c }, // 5
    { 0x3c, 0x7e, 0xe7, 0xc3, 0xc0, 0xc0, 0xc0, 0xc0,
      0xfc, 0xfe, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e, 0x3c }, // 6
    { 0xff, 0xff, 0xc3, 0x03, 0x06, 0x06, 0x0c, 0x0c,
      0x18, 0x18, 0x30, 0x30, 0x60, 0x60, 0xc0, 0xc0 }, // 7
    { 0x3c, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0x66, 0x3c,
      0x3c, 0x66, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e, 0x3c }, // 8
    { 0x3c, 0x7e, 0xc7, 0xc3, 0xc3, 0xc3, 0xe7, 0x7f,
      0x3b, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03 },  // 9
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }  // blank
};
//---------------------------------------------------------------------------
NiFrameRate::NiFrameRate() :
    m_puiHisto(0),
#if defined(_XENON) || defined (_PS3)
    m_fXLocation(32.0f),
    m_fYLocation(32.0f),
#elif defined(WIN32)
    m_fXLocation(8.0f),
    m_fYLocation(8.0f),
#endif
    m_fDigitHeight(0.0f),
    m_pkRenderTarget(NULL),
    m_usDigitSpacing(3)
{
    Init(false, 0);

    m_spMatProp = NiNew NiMaterialProperty;
    m_spMatProp->SetEmittance(NiColor::WHITE);
    m_spMatProp->SetAlpha(1.0f);
}
//---------------------------------------------------------------------------
NiFrameRate::~NiFrameRate()
{
    Shutdown();
}
//---------------------------------------------------------------------------
void NiFrameRate::Init(bool bDisplayFrameRate, unsigned int uiHistoSize,
    const NiRenderTargetGroup* pkTarget)
{
    m_pkRenderTarget = pkTarget;

    if (m_bDisplayFrameRate)
        DeleteScreenElements();

    if (bDisplayFrameRate)
        SetupScreenElements();

    NiFree(m_puiHisto);
    m_uiHistoSize = uiHistoSize;
    if (m_uiHistoSize)
    {
        m_puiHisto = NiAlloc(unsigned int, m_uiHistoSize);
        if (m_puiHisto == 0)
            m_uiHistoSize = 0;
        else
            memset(m_puiHisto, 0, m_uiHistoSize * sizeof(int));
    }
    else
    {
        m_puiHisto = 0;
    }

    m_bDisplayFrameRate = bDisplayFrameRate;
    m_uiNumSamples = 0;
    m_fSummedSamples = 0;
    m_fSumOfSquares = 0;
    m_bFirstSample = true;
    m_fStartTime = 0;
    m_fLastTime = 0;
    m_uiCurInsertionPoint = 0;
    m_uiLastFrameRate = (unsigned int)~0;
    m_uiFrameRate = 0;

    m_fMin = 100000.0f;
    m_fMax = -100000.0f;
}
//---------------------------------------------------------------------------
void NiFrameRate::Shutdown()
{
    if (m_bDisplayFrameRate)
        DeleteScreenElements();

    NiFree(m_puiHisto);
}
//---------------------------------------------------------------------------
void NiFrameRate::SetupScreenElements()
{
    // Find texture size - lowest power-of-two greater than or equal to size
    unsigned int uiTextureWidth = 1;
    while (uiTextureWidth < DIGIT_WIDTH)
        uiTextureWidth <<= 1;
    unsigned int uiTextureHeight = 1;
    while (uiTextureHeight < DIGIT_HEIGHT * NUM_NUMS)
        uiTextureHeight <<= 1;

    m_fDigitHeight = (float)DIGIT_HEIGHT / (float)uiTextureHeight;

    NiPixelData* pkPixelData = NiNew NiPixelData(uiTextureWidth, 
        uiTextureHeight, NiPixelFormat::RGBA32);
    NIASSERT(pkPixelData);

    unsigned int i;
    for (i = 0; i < DIGIT_HEIGHT * NUM_NUMS; i++)
    {
        unsigned short usDataBits = 
            ms_aausNumberBits[i / DIGIT_HEIGHT][i % DIGIT_HEIGHT];
        unsigned short usSeekBit = 0x80;
        unsigned int j = 0;
        for (; j < DIGIT_WIDTH; j++)
        {
            unsigned char* pucTmp = (*pkPixelData)(j, i);

            if (usDataBits & usSeekBit)
            {
                pucTmp[0] = pucTmp[1] = pucTmp[2] = pucTmp[3] = 255;
            }
            else
            {
                pucTmp[0] = pucTmp[1] = pucTmp[2] = pucTmp[3] = 0;
            }

            usSeekBit >>= 1;
        }
        for (; j < uiTextureWidth; j++)
        {
            unsigned char* pucTmp = (*pkPixelData)(j, i);
            pucTmp[0] = pucTmp[1] = pucTmp[2] = pucTmp[3] = 0;
        }
    }
    if (i < uiTextureHeight)
    {
        unsigned char* pucTmp = (*pkPixelData)(0, i);
        memset(pucTmp, 0, uiTextureWidth * (uiTextureHeight - i) * 4);
    }

    // Create the geometric representation for the frame rate display.
    int iNumPolygons = NUM_DIGITS;
    int iNumVertices = 4 * NUM_DIGITS;
    int iNumTriangles = 2 * NUM_DIGITS;
    m_spElements = NiNew NiMeshScreenElements(false, false, 1, iNumPolygons,
        1, iNumVertices, 1, iNumTriangles, 1);

    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    const NiRenderTargetGroup* pkRTGroup = m_pkRenderTarget;
    if (pkRTGroup == NULL)
        pkRTGroup = pkRenderer->GetDefaultRenderTargetGroup();
    float fInvScrW = 1.0f / (float)pkRTGroup->GetWidth(0);
    float fInvScrH = 1.0f / (float)pkRTGroup->GetHeight(0);

    // to be used as initial texture coordinates
    NiPoint2 akTexCoords[4];
    memset(akTexCoords, 0, 4 * sizeof(NiPoint2));

    for (i = 0; i < NUM_DIGITS; i++)
    {
        m_aiPolygon[i] = m_spElements->Insert(4);

        // Set the frame object vertices.
        float fLeft =
            fInvScrW * (float)((DIGIT_WIDTH + m_usDigitSpacing) * i);
        float fTop = 0.0f;
        float fWidth = fInvScrW * (float)DIGIT_WIDTH;
        float fHeight = fInvScrH * (float)DIGIT_HEIGHT;
        m_spElements->SetRectangle(m_aiPolygon[i], fLeft, fTop, fWidth,
            fHeight);

        // All texture coordinates are initially zero.
        m_spElements->SetTextures(m_aiPolygon[i], 0, akTexCoords);
    }

    m_spElements->UpdateBound();

    NiTexturingProperty* pkTexProp = NiNew NiTexturingProperty(pkPixelData);
    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    pkTexProp->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
    pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
    m_spElements->AttachProperty(pkTexProp);

    NiVertexColorProperty* pkVCProp = NiNew NiVertexColorProperty;
    pkVCProp->SetSourceMode(NiVertexColorProperty::SOURCE_IGNORE);
    pkVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_spElements->AttachProperty(pkVCProp);

    NiAlphaProperty* pkAlphaProp = NiNew NiAlphaProperty;
    pkAlphaProp->SetAlphaBlending(true);
    pkAlphaProp->SetAlphaTesting(false);
    pkAlphaProp->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
    pkAlphaProp->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
    m_spElements->AttachProperty(pkAlphaProp);

    m_spElements->AttachProperty(m_spMatProp);

    m_spElements->UpdateProperties();
    m_spElements->UpdateEffects();
    m_spElements->Update(0.0f);
}
//---------------------------------------------------------------------------
void NiFrameRate::DeleteScreenElements()
{
    m_spElements = 0;
}
//---------------------------------------------------------------------------
void NiFrameRate::TakeSample()
{
    float fTime;
    float fDeltaTime;

    fTime = NiGetCurrentTimeInSec();

    if (m_bFirstSample)
    {
        m_bFirstSample = false;
        m_fLastTime = m_fStartTime = fTime;
        return;
    }

    m_uiNumSamples++;

    m_afRunningSamples[m_uiCurInsertionPoint] = fTime;
    m_uiCurInsertionPoint = (m_uiCurInsertionPoint + 1) % WINDOW;

    if (m_uiNumSamples >= WINDOW)
    {
        m_uiFrameRate = (int) (((WINDOW - 1.0f) /
            (fTime - m_afRunningSamples[m_uiCurInsertionPoint])) + 0.5f);
    }

    fDeltaTime = fTime - m_fLastTime;
    m_fSummedSamples += fDeltaTime;
    m_fSumOfSquares += fDeltaTime * fDeltaTime;

    m_fLastTime = fTime;

    if (fDeltaTime < m_fMin)
        m_fMin = fDeltaTime;
    if (fDeltaTime > m_fMax)
        m_fMax = fDeltaTime;

    if (m_puiHisto)
    {
        // Cast to int when converting from float to fixed to avoid software
        // conversion on some platforms:
        unsigned int uiIndex = (int) (1.0f / fDeltaTime);
        if (uiIndex >= m_uiHistoSize)
            uiIndex = m_uiHistoSize - 1;
        m_puiHisto[uiIndex]++;
    }
}
//---------------------------------------------------------------------------
void NiFrameRate::GetStats(float& fAvg, float& fStdDev, float& fMin,
    float& fMax, unsigned int* puiHisto)
{
    float fAvgFPerS;
    float fDeltaFPerS;

    fAvg = fStdDev = 0;

    if (m_uiNumSamples == 0)
        return;

    fAvgFPerS = m_uiNumSamples / (m_fLastTime - m_fStartTime);
    fDeltaFPerS = 0;

    // compute the standard deviation
    if (m_uiNumSamples > 1)
    {
        float fAvgSPerF;
        float fDeltaSPerF;

        fAvgSPerF = 1.0f / fAvgFPerS;

        fDeltaSPerF = (m_uiNumSamples * m_fSumOfSquares) - 
            (m_fSummedSamples * m_fSummedSamples);
        fDeltaSPerF /= m_uiNumSamples * (m_uiNumSamples - 1);
        fDeltaSPerF = NiSqrt(fDeltaSPerF);

        fDeltaFPerS = (2.0f * fDeltaSPerF) /
            ((fAvgSPerF - fDeltaSPerF) * (fAvgSPerF + fDeltaSPerF));
    }

    fAvg = fAvgFPerS;
    fStdDev = fDeltaFPerS;
    fMin = 1.0f / m_fMax;
    fMax = 1.0f / m_fMin;

    if (puiHisto)
    {
        unsigned int uiSize = m_uiHistoSize * sizeof(unsigned int);
        NiMemcpy(puiHisto, m_puiHisto, uiSize);
    }
}
//---------------------------------------------------------------------------
void NiFrameRate::Update()
{
    if (!m_bDisplayFrameRate || m_uiLastFrameRate == m_uiFrameRate)
        return;

    NIASSERT(m_spElements);

    m_uiLastFrameRate = m_uiFrameRate;
    unsigned int uiTempFrameRate = m_uiFrameRate;
    unsigned int auiNums[NUM_DIGITS];
    unsigned int i = NUM_DIGITS;
    while (i--)
    {
        auiNums[i] = uiTempFrameRate % 10;
        uiTempFrameRate /= 10;
    }

    bool bFirstDigit = false;
    for (i = 0; i < NUM_DIGITS; i++)
    {
        // Update screen elements
        unsigned int uiDigit = auiNums[i];
        if (uiDigit == 0 && bFirstDigit == false)
            uiDigit = 10;
        else
            bFirstDigit = true;

        float fTop = (float)uiDigit * m_fDigitHeight;
        float fBot = fTop + m_fDigitHeight;

        m_spElements->SetTexture(m_aiPolygon[i], 0, 0, NiPoint2(0.0f, fTop));
        m_spElements->SetTexture(m_aiPolygon[i], 1, 0, NiPoint2(0.0f, fBot));
        m_spElements->SetTexture(m_aiPolygon[i], 2, 0, NiPoint2(1.0f, fBot));
        m_spElements->SetTexture(m_aiPolygon[i], 3, 0, NiPoint2(1.0f, fTop));
    }

    // The translation is in screen coordinates, but must be converted to
    // normalized display coordinates.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    const NiRenderTargetGroup* pkRTGroup = m_pkRenderTarget;
    if (pkRTGroup == NULL)
        pkRTGroup = pkRenderer->GetDefaultRenderTargetGroup();
    float fInvScrW = 1.0f / (float)pkRTGroup->GetWidth(0);
    float fInvScrH = 1.0f / (float)pkRTGroup->GetHeight(0);
    m_spElements->SetTranslate(m_fXLocation * fInvScrW,
        m_fYLocation * fInvScrH, 0.0f);
    m_spElements->Update(0.0f);
}
//---------------------------------------------------------------------------
