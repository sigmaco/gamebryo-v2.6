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
//
// THIS CLASS HAS BEEN DEPRECATED.
// Functionality here exists only to enable streaming and conversion to the
// NiMorphMeshModifier class.
//
//---------------------------------------------------------------------------

#ifndef NIGEOMMORPHERCONTROLLER_H
#define NIGEOMMORPHERCONTROLLER_H

#include "NiInterpController.h"
#include "NiAnimationLibType.h"
#include "NiMorphData.h"

NiSmartPointer(NiGeomMorpherController);

class NIANIMATION_ENTRY NiGeomMorpherController : public NiInterpController
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareFlags(unsigned short);

public:
    // *** begin Emergent internal use only ***
    virtual ~NiGeomMorpherController();

    // shared data access
    inline NiMorphData* GetMorphData() const;

    // accessors
    inline unsigned int GetNumTargets() const;

    NiMorphData::MorphTarget* GetTarget(unsigned int uiIndex) const;

    // animation updates
    virtual void Update(float fTime);

    inline bool GetUpdateNormals() const;
    inline bool GetAlwaysUpdate() const;

    // This method should be called any time the interpolator changes
    // outside of the SetInterpolator method
    virtual void ResetTimeExtrema();

    inline float GetWeight(unsigned int uiIndex);

    virtual void GuaranteeTimeRange(float fStartTime, float fEndTime);
    // *** end Emergent internal use only ***

protected:
    NiGeomMorpherController(NiMorphData* pkData = 0);

    // flags
    enum
    {
        UPDATENORMAL_MASK   = 0x0001
    };

    NiMorphDataPtr m_spMorphData;

    class NIANIMATION_ENTRY InterpItem : public NiMemObject
    {
    public:
        InterpItem();

        NiInterpolatorPtr m_spInterpolator;
        float m_fWeight;
    };
    InterpItem* m_pkInterpItems;

    bool m_bAlwaysUpdate;

    //Methods implemented from NiInterpController
public:
    virtual unsigned short GetInterpolatorCount() const;

    virtual const char* GetInterpolatorID(unsigned short usIndex = 0);
    virtual unsigned short GetInterpolatorIndex(const char* pcID) const;
    
    virtual NiInterpolator* GetInterpolator(unsigned short usIndex = 0) const;
    virtual void SetInterpolator(NiInterpolator* pkInterpolator,
        unsigned short usIndex = 0);

    virtual NiEvaluator* CreatePoseEvaluator(unsigned short usIndex = 0);
    virtual NiInterpolator* CreatePoseInterpolator(unsigned short usIndex = 0);
    virtual void SynchronizePoseInterpolator(NiInterpolator* pkInterp, 
        unsigned short usIndex = 0);
    virtual NiBlendInterpolator* CreateBlendInterpolator(
        unsigned short usIndex = 0, bool bManagerControlled = false, 
        float fWeightThreshold = 0.0f, unsigned char ucArraySize = 2)
        const;

    // debugging
    virtual bool TargetIsRequiredType() const;

protected:
    // For debug only.
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator,
        unsigned short usIndex) const;

    inline void AllocateInterpArray();
};

#include "NiGeomMorpherController.inl"

#endif

