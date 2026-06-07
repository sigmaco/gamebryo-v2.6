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

#ifndef NIMULTITARGETTRANSFORMCONTROLLER_H
#define NIMULTITARGETTRANSFORMCONTROLLER_H

#include "NiInterpController.h"

//---------------------------------------------------------------------------
//
// THIS CLASS HAS BEEN DEPRECATED.
//
// This class has been replaced by NiMultiTargetPoseHandler which performs
// a similar role of propagating animated sequence data to scene graph 
// transform nodes. Functionality exists here only to enable streaming 
// of existing assets.
//
//---------------------------------------------------------------------------

class NiInterpolator;

class NIANIMATION_ENTRY NiMultiTargetTransformController :
    public NiInterpController
{
    NiDeclareRTTI;
    NiDeclareClone(NiMultiTargetTransformController);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    // *** begin Emergent internal use only ***
    virtual ~NiMultiTargetTransformController();

    virtual void Update(float fTime);
    
    virtual unsigned short GetInterpolatorCount() const;

    virtual const char* GetInterpolatorID(unsigned short usIndex = 0);
    virtual unsigned short GetInterpolatorIndex(const char* pcID) const;
    
    virtual NiInterpolator* GetInterpolator(unsigned short usIndex = 0) const;
    virtual void SetInterpolator(NiInterpolator* pkInterpolator,
        unsigned short usIndex = 0);

    // This method should be called whenever the key time range must be reset.
    // Note that this may cause the key range to be set to (0.0f, 0.0f) if it
    // contains any pose interpolators.
    virtual void ResetTimeExtrema();

    virtual NiEvaluator* CreatePoseEvaluator(unsigned short usIndex = 0);
    virtual NiInterpolator* CreatePoseInterpolator(
        unsigned short usIndex = 0);
    virtual void SynchronizePoseInterpolator(NiInterpolator* pkMultiTransform, 
        unsigned short usIndex = 0);
    virtual NiBlendInterpolator* CreateBlendInterpolator(
        unsigned short usIndex = 0, bool bManagerControlled = false, 
        float fWeightThreshold = 0.0f, unsigned char ucArraySize = 2) const;
    virtual void GuaranteeTimeRange(float fStartTime, float fEndTime);
    virtual bool TargetIsRequiredType() const;
    // *** end Emergent internal use only ***

protected:
    // Protected constructor to prevent instantiation of this class outside
    // of stream loading.
    NiMultiTargetTransformController();

    // For debug only.
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator,
        unsigned short usIndex) const;
};

NiSmartPointer(NiMultiTargetTransformController);

#include "NiMultiTargetTransformController.inl"

#endif  // #ifndef NIMULTITARGETTRANSFORMCONTROLLER_H
