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

#ifndef NIMORPHWEIGHTSCONTROLLER_H
#define NIMORPHWEIGHTSCONTROLLER_H

#include <NiAVObject.h>
#include <NiMorphMeshModifier.h>
#include "NiInterpController.h"
#include "NiAnimationLibType.h"

NiSmartPointer(NiMorphWeightsController);

class NIANIMATION_ENTRY NiMorphWeightsController : public NiInterpController
{
    NiDeclareRTTI;
    NiDeclareClone(NiMorphWeightsController);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiMorphWeightsController(NiUInt32 uiNumTargets);
    virtual ~NiMorphWeightsController();
    
    // animation updates
    virtual void Update(float fTime);
    virtual bool UpdateValue(float fTime, float fFloat, 
        unsigned short usIndex = 0);

    inline NiUInt32 GetNumTargets() const;

    void GetTargetName(const NiUInt16 uiIndex, NiFixedString& kName) const;    
    void SetTargetName(const NiUInt16 uiIndex, const NiFixedString& kName); 

    inline NiMorphMeshModifier* GetModifier() const;

    // Overridden from NiTimeController so that the weights data stream
    // and the morphing modifier can be found and set.
    virtual void SetTarget(NiObjectNET* pkTarget);

    // This method should be called any time the interpolator changes
    // outside of the SetInterpolator method
    virtual void ResetTimeExtrema();

    //Methods implemented from NiInterpController
    virtual unsigned short GetInterpolatorCount() const;

    virtual const char* GetInterpolatorID(unsigned short usIndex = 0);
    virtual unsigned short GetInterpolatorIndex(const char* pcID) const;
    
    virtual NiInterpolator* GetInterpolator(unsigned short usIndex = 0) const;
    virtual void SetInterpolator(NiInterpolator* pkInterpolator,
        unsigned short usIndex = 0);

    // *** begin Emergent internal use only ***
    virtual NiEvaluator* CreatePoseEvaluator(unsigned short usIndex = 0);
    virtual NiInterpolator* CreatePoseInterpolator(unsigned short usIndex = 0);
    virtual void SynchronizePoseInterpolator(NiInterpolator* pkInterp, 
        unsigned short usIndex = 0);
    virtual NiBlendInterpolator* CreateBlendInterpolator(
        unsigned short usIndex = 0, bool bManagerControlled = false, 
        float fWeightThreshold = 0.0f, unsigned char ucArraySize = 2)
        const;

    virtual void GuaranteeTimeRange(float fStartTime, float fEndTime);

    virtual void ResetModifierData();

    virtual void ProcessClone(NiCloningProcess& kCloning);

    // *** end Emergent internal use only ***

protected:
    // Constructor for use in cloning and streaming.
    NiMorphWeightsController();

    // debugging
    virtual bool TargetIsRequiredType() const;

    // For debug only, required by NiInterpController
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator,
        unsigned short usIndex) const;

    // Try to extract the morph modifier and weights stream from the
    // target. Return true if successful.
    bool GetMeshProperties();
    
    // Adjust the size of the update value weight data to match the 
    // current number of targets.
    void ReallocUpdateValueWeightData();

    // Interpolators that drive the weights
    NiTObjectArray<NiInterpolatorPtr> m_kInterpolators;
    
    // Names used to associate interpolators with indexes in the
    // animation system.
    NiTObjectArray<NiFixedString> m_kTargetNames;
    
    // The data stream containing weight data (associated with the
    // target mesh). There are k weights, where k is the number of
    // morph targets.
    // Weight[0] is the special "base" target mesh. The remaining weights
    // [1] -> [k-1] are the remaining morph targets.
    NiDataStream* m_pkWeightsStream;
    
    // The modifier containing flags for controlling the morph.
    NiMorphMeshModifier* m_pkModifier;
    
    // Needed so we can accurately report interpolator counts.
    NiUInt32 m_uiNumTargets;

    // The last time that UpdateValue was called, 
    // and the expected end index for this time.
    float m_fLastUpdateValueTime;
    unsigned short m_usUpdateValueEndIndex;

    // Temporarily holds the weight data for UpdateValue until
    // all the weights have been updated.
    float* m_pfUpdateValueWeightData;
};

#include "NiMorphWeightsController.inl"

#endif

