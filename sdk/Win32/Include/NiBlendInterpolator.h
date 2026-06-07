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

#ifndef NIBLENDINTERPOLATOR_H
#define NIBLENDINTERPOLATOR_H

#include <NiInterpolator.h>
#include <NiTArray.h>

class NIANIMATION_ENTRY NiBlendInterpolator : public NiInterpolator
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiBlendInterpolator);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

    NiDeclareFlags(unsigned char);
public:
    virtual ~NiBlendInterpolator();

    static const unsigned char INVALID_INDEX;
    static const float INVALID_TIME;

    inline unsigned char GetArraySize() const;
    static unsigned char GetArrayGrowBy();
    static void SetArrayGrowBy(unsigned char ucArrayGrowBy);

    inline NiInterpolator* GetInterpolator(unsigned char ucIndex = 0) const;
    inline void SetInterpolator(NiInterpolator* pkInterpolator,
        unsigned char ucIndex = 0);

    inline float GetWeight(unsigned char ucIndex = 0) const;
    inline void SetWeight(float fWeight, unsigned char ucIndex = 0);

    inline char GetPriority(unsigned char ucIndex = 0) const;
    void SetPriority(char cPriority, unsigned char ucIndex = 0);

    inline float GetEaseSpinner(unsigned char ucIndex = 0) const;
    inline void SetEaseSpinner(float fEaseSpinner, unsigned char ucIndex = 0);

    inline float GetTime(unsigned char ucIndex = 0) const;
    inline void SetTime(float fTime, unsigned char ucIndex = 0);

    inline bool GetOnlyUseHighestWeight() const;
    inline void SetOnlyUseHighestWeight(bool bOnlyUseHighestWeight);

    virtual unsigned char AddInterpInfo(NiInterpolator* pkInterpolator,
        float fWeight, char cPriority = 0, float fEaseSpinner = 1.0f);
    virtual NiInterpolatorPtr RemoveInterpInfo(unsigned char ucIndex);

    inline float GetWeightThreshold() const;
    inline void SetWeightThreshold(float fWeightThreshold);

    // For use in reducing the memory footprint of an interpolator 
    virtual void Collapse();

    // *** begin Emergent internal use only ***
    inline bool GetManagerControlled() const;
    inline void SetManagerControlled(bool bManagerControlled);
    void ComputeNormalizedWeights();
    inline float GetNormalizedWeight(unsigned char ucIndex = 0) const;
    unsigned char GetHighestWeightedIndex() const;
    virtual void ProcessClone(NiCloningProcess& kCloning);
    virtual void GuaranteeTimeRange(float fStartTime, 
        float fEndTime);
    // *** end Emergent internal use only ***

protected:
    NiBlendInterpolator(bool bManagerControlled, float fWeightThreshold,
        unsigned char ucArraySize);

    // For cloning and streaming only.
    NiBlendInterpolator();

    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator)
        const = 0;

    virtual bool ReallocateArrays();
    
    void ComputeNormalizedWeightsFor2();
    inline void ClearWeightSums();

    struct InterpArrayItem : public NiMemObject
    {
        InterpArrayItem();

        inline void ClearValues();

        void LoadBinary(NiStream& kStream);
        void SaveBinary(NiStream& kStream);
        void RegisterStreamables(NiStream& kStream);
        bool IsEqual(InterpArrayItem& kDest);

        NiInterpolatorPtr m_spInterpolator;
        float m_fWeight;
        float m_fNormalizedWeight;
        char m_cPriority;
        float m_fEaseSpinner;
        float m_fUpdateTime;
    };

    // flags
    enum
    {
        MANAGER_CONTROLLED_MASK         = 0X0001,
        ONLY_USE_HIGHEST_WEIGHT_MASK    = 0X0002,
        COMPUTE_NORMALIZED_WEIGHTS_MASK = 0x0004
    };

    inline bool GetComputeNormalizedWeights() const;
    inline void SetComputeNormalizedWeights(
        bool bComputeNormalizedWeights);

    inline bool GetSingleUpdateTime(float& fTime);
    inline bool GetUpdateTimeForItem(float& fTime, InterpArrayItem& kItem);

    static unsigned char ms_ucArrayGrowBy;

    unsigned char m_ucArraySize;
    unsigned char m_ucInterpCount;
    unsigned char m_ucSingleIdx;
    char m_cHighPriority;
    char m_cNextHighPriority;

    InterpArrayItem* m_pkInterpArray;
    NiInterpolator* m_pkSingleInterpolator;

    float m_fWeightThreshold;
    float m_fSingleTime;
    float m_fHighSumOfWeights;
    float m_fNextHighSumOfWeights;
    float m_fHighEaseSpinner;
};

NiSmartPointer(NiBlendInterpolator);

#include "NiBlendInterpolator.inl"

#endif  // #ifndef NIBLENDINTERPOLATOR_H
