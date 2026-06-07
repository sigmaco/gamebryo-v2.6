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

#ifndef NIBOOLTIMELINEINTERPOLATOR_H
#define NIBOOLTIMELINEINTERPOLATOR_H

#include "NiBoolInterpolator.h"

class NIANIMATION_ENTRY NiBoolTimelineInterpolator : public NiBoolInterpolator
{
    NiDeclareRTTI;
    NiDeclareClone(NiBoolTimelineInterpolator);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiBoolTimelineInterpolator();
    NiBoolTimelineInterpolator(NiBoolData* pkBoolData);
    NiBoolTimelineInterpolator(bool bPoseValue);

    virtual bool Update(float fTime, NiObjectNET* pkInterpTarget,
        bool& bValue);

protected:

    unsigned int m_uiPreviousIdx;
    bool m_bPreviousValue;
};

NiSmartPointer(NiBoolTimelineInterpolator);

#endif  // #ifndef NIBOOLTIMELINEINTERPOLATOR_H
