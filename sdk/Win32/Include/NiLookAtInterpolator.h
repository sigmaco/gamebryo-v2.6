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

#ifndef NILOOKATINTERPOLATOR_H
#define NILOOKATINTERPOLATOR_H

#include "NiInterpolator.h"
#include <NiFlags.h>
#include <NiQuaternion.h>
#include "NiQuatTransform.h"

class NiAVObject;

class NIANIMATION_ENTRY NiLookAtInterpolator : public NiInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiLookAtInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    enum Axis
    {
        X,
        Y,
        Z
    };

    NiLookAtInterpolator();
    NiLookAtInterpolator(NiAVObject* pkLookAt, Axis eAxis = X,
        bool bFlip = false);
    ~NiLookAtInterpolator();
    
    // Virtual function overrides from base class.
    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        NiQuatTransform& kValue);
    virtual bool IsTransformValueSupported() const;
    
    inline NiAVObject* GetLookAt() const;
    inline void SetLookAt(NiAVObject* pkLookAt);

    inline bool GetFlip() const;
    inline void SetFlip(bool bFlip);

    Axis GetAxis() const;
    inline void SetAxis(Axis eAxis);

    inline NiInterpolator* GetTranslateInterpolator();
    inline void SetTranslateInterpolator(NiInterpolator* pkInterp);
    
    inline NiInterpolator* GetRollInterpolator();
    inline void SetRollInterpolator(NiInterpolator* pkInterp);
    
    inline NiInterpolator* GetScaleInterpolator();
    inline void SetScaleInterpolator(NiInterpolator* pkInterp);

    virtual void Collapse();

    // *** begin Emergent internal use only ***
    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    // This method can be used to resolve any dependencies that might
    // be broken by storing the interpolator in a stream.
    virtual bool ResolveDependencies(NiAVObjectPalette* pkPalette);
    virtual bool SetUpDependencies();
    virtual void GetActiveTimeRange(float& fBeginKeyTime, float& fEndKeyTime)
        const;
    virtual void GuaranteeTimeRange(float fStartTime, 
        float fEndTime);
    virtual NiInterpolator* GetSequenceInterpolator(float fStartTime,
        float fEndTime);
    virtual bool AlwaysUpdate() const;
    const NiFixedString& GetLookAtName() const;
    // *** end Emergent internal use only ***

protected:
    inline void SetLookAtName(const NiFixedString& kName);


    // Flags.
    enum
    {
        FLIP_MASK   = 0x0001,
        AXIS_MASK   = 0x0006,
        AXIS_POS    = 1
    };

    NiAVObject* m_pkLookAt;
    NiFixedString m_kLookAtName;

    NiQuatTransform m_kTransformValue;
    NiInterpolatorPtr m_aspInterpolators[3];
};

NiSmartPointer(NiLookAtInterpolator);

#include "NiLookAtInterpolator.inl"

#endif  // #ifndef NILOOKATINTERPOLATOR_H
