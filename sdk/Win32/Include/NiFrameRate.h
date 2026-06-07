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

#ifndef NIFRAMERATE_H
#define NIFRAMERATE_H

#include <NiColor.h>
#include <NiMaterialProperty.h>
#include <NiMeshScreenElements.h>

class NiRenderTargetGroup;

class NiFrameRate : public NiMemObject
{
public:
    NiFrameRate();
    ~NiFrameRate();
    
    void Init(bool bDisplayFrameRate, unsigned int uiHistoSize = 0,
        const NiRenderTargetGroup* pkTarget = NULL);
    void Shutdown();

    // set the normalized upper left location of the highest order digit
    inline void SetLocation(float fX, float fY);
    inline void GetLocation(float& fX, float& fY) const;

    // Set the horizontal spacing of the digits
    // Default spacing == 3 pixels
    void SetDigitSpacing(unsigned short usSpacing);
    unsigned short GetDigitSpacing() const;

    inline void SetColor(const NiColor& kColor);
    inline const NiColor& GetColor() const;

    inline void SetAlpha(float fAlpha);
    inline float GetAlpha() const;

    void TakeSample();
    void GetStats(float& fAvg, float& fStdDev, float& fMin, float& fMax,
        unsigned int* piHisto = 0);
    inline NiMeshScreenElementsPtr GetElements() const;
    void Update();

    inline unsigned int GetFrameRate() const;

    inline const NiRenderTargetGroup* GetRenderTargetGroup() const;
    inline void SetRenderTargetGroup(const NiRenderTargetGroup* pkGroup);
 
protected:
    void SetupScreenElements();
    void DeleteScreenElements();

    enum 
    { 
        NUM_DIGITS = 3, 
        NUM_NUMS = 11, 
        WINDOW = 20, 
        DIGIT_HEIGHT = 16, 
        DIGIT_WIDTH = 8
    };

    static unsigned short ms_aausNumberBits[NUM_NUMS][DIGIT_HEIGHT];

    NiMeshScreenElementsPtr m_spElements;
    int m_aiPolygon[NUM_DIGITS];

    bool m_bDisplayFrameRate;

    unsigned int m_uiFrameRate;
    unsigned int m_uiLastFrameRate;
    unsigned int m_uiNumSamples;
    float m_fSummedSamples;
    float m_fSumOfSquares;

    bool m_bFirstSample;
    float m_fStartTime;
    float m_fLastTime;

    float m_afRunningSamples[WINDOW];
    unsigned int m_uiCurInsertionPoint;

    float m_fMin;
    float m_fMax;

    unsigned int m_uiHistoSize;
    unsigned int* m_puiHisto;

    float m_fXLocation;
    float m_fYLocation;
    float m_fDigitHeight;

    const NiRenderTargetGroup* m_pkRenderTarget;

    const unsigned short m_usDigitSpacing;

    NiMaterialPropertyPtr m_spMatProp;
private:
    NiFrameRate & operator=( const NiFrameRate & );
};

#include "NiFrameRate.inl"

#endif
