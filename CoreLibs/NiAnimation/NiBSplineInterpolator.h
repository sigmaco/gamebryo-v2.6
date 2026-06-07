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

#ifndef NIBSPLINEINTERPOLATOR_H
#define NIBSPLINEINTERPOLATOR_H

#include <NiObject.h>
#include "NiAnimationLibType.h"
#include <NiBool.h>
#include "NiInterpolator.h"
#include "NiBSplineData.h"

class NIANIMATION_ENTRY NiBSplineInterpolator : public NiInterpolator
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiBSplineInterpolator);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

public:
    virtual ~NiBSplineInterpolator();

    enum {
        MAXIMUM_DIMENSIONALITY = 4
    };

    // Pure virtual methods
    virtual unsigned short GetChannelCount() const = 0;
    virtual unsigned int GetControlPointCount(unsigned short usChannel) 
        const;
    virtual unsigned int GetDimension(unsigned short usChannel) const = 0;
    virtual unsigned int GetDegree(unsigned short usChannel) const = 0; 
    virtual bool GetChannelPosed(unsigned short usChannel) const = 0;

    virtual bool UsesCompressedControlPoints() const;
    virtual unsigned int GetAllocatedSize(unsigned short usChannel) const;
    virtual void Collapse();

    // Data access.
    NiBSplineData* GetData() const;
    void SetData(NiBSplineData* pkData, NiBSplineBasisData* pkBasisData);
    NiBSplineBasisData* GetBasisData() const;
    virtual NiBSplineData::Handle GetControlHandle(
        unsigned short usChannel) const = 0;
    virtual void SetControlHandle(NiBSplineData::Handle kControlHandle, 
        unsigned short usChannel) = 0;
    
    // *** begin Emergent internal use only ***
    void SetTimeRange(float fStart, float fEnd);
    virtual void GetActiveTimeRange(float& fBeginTime, float& fEndTime)
        const;
    virtual void GuaranteeTimeRange(float fStartTime, 
        float fEndTime);
    virtual NiInterpolator* GetSequenceInterpolator(float fStartTime,
        float fEndTime);
    // *** end Emergent internal use only ***

protected:
    NiBSplineInterpolator(NiBSplineData* pkData = NULL, 
        NiBSplineBasisData* pkBasisData = NULL);

    float m_fStartTime;
    float m_fEndTime;
    
    NiBSplineDataPtr m_spData;
    
    // Handle into the NiBSplineData for access to the NiBSplineBasis
    NiBSplineBasisDataPtr m_spBasisData;
   
};

NiSmartPointer(NiBSplineInterpolator);

#endif  // #ifndef NIINTERPOLATOR_H
