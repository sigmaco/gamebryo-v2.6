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

#ifndef NIINTERPCONTROLLER_H
#define NIINTERPCONTROLLER_H

#include <NiTimeController.h>
#include "NiAnimationLibType.h"
#include <NiColor.h>
#include <NiQuaternion.h>

NiSmartPointer(NiInterpolator);
class NiBlendInterpolator;
class NiEvaluator;
class NiEvaluatorSPData;
class NiControllerSequence;

class NIANIMATION_ENTRY NiInterpController : public NiTimeController
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiInterpController);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

public:
    static const unsigned short INVALID_INDEX;
    static const float INVALID_TIME;

    virtual void Update(float fTime) = 0;
    virtual unsigned short GetInterpolatorCount() const = 0;

    virtual const char* GetInterpolatorID(unsigned short usIndex = 0)
        = 0;
    virtual unsigned short GetInterpolatorIndex(const char* pcID) const = 0;
    
    virtual NiInterpolator* GetInterpolator(unsigned short usIndex = 0) const
        = 0;
    virtual void SetInterpolator(NiInterpolator* pkInterpolator,
        unsigned short usIndex = 0) = 0;

    inline bool GetManagerControlled() const;
    inline void SetManagerControlled(bool bManagerControlled);

    // This method should be called whenever the key time range must be reset.
    // Note that this may cause the key range to be set to (0.0f, 0.0f) if it
    // contains any pose interpolators.
    virtual void ResetTimeExtrema();

    // Returns an ID for this controller type that can uniquely identify this
    // controller among others of the same type on the same NiObjectNET. This
    // function is used primarily for controller extraction.
    virtual const char* GetCtlrID();

    // These methods push the specified values into the associated 
    // scene graph attribute. False is returned when the data type is
    // not supported.
    virtual bool UpdateValue(float fTime, bool bBool, 
        unsigned short usIndex = 0);
    virtual bool UpdateValue(float fTime, float fFloat, 
        unsigned short usIndex = 0);
    virtual bool UpdateValue(float fTime, const NiColorA& kColor, 
        unsigned short usIndex = 0);
    virtual bool UpdateValue(float fTime, const NiPoint3& kPoint3, 
        unsigned short usIndex = 0);
    virtual bool UpdateValue(float fTime, const NiQuaternion& kRot, 
        unsigned short usIndex = 0);
    virtual bool UpdateValue(float fTime, 
        NiControllerSequence* pkControllerSequence, 
        NiEvaluator* pkEvaluator, 
        NiEvaluatorSPData** ppkEvalSPData,
        unsigned short usIndex = 0);

    // *** begin Emergent internal use only ***
    virtual NiEvaluator* CreatePoseEvaluator(unsigned short usIndex = 0) = 0;
    virtual NiInterpolator* CreatePoseInterpolator(unsigned short usIndex = 0)
        = 0;
    virtual void SynchronizePoseInterpolator(NiInterpolator* pkInterp, 
        unsigned short usIndex = 0) = 0;
    virtual NiBlendInterpolator* CreateBlendInterpolator(
        unsigned short usIndex = 0, bool bManagerControlled = false, 
        float fWeightThreshold = 0.0f, unsigned char ucArraySize = 2) 
        const = 0;
    virtual void GuaranteeTimeRange(float fStartTime, float fEndTime) = 0;

    static void GuaranteeTimeRange(NiAVObject* pkObject, float fStartTime,
        float fEndTime, const char* pcExcludedControllerTypes);
    // *** end Emergent internal use only ***

protected:
    NiInterpController();

    // For debug only.
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator,
        unsigned short usIndex) const = 0;
};

NiSmartPointer(NiInterpController);

#include "NiInterpController.inl"

#endif  // #ifndef NIINTERPCONTROLLER_H
