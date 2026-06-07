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

#ifndef NIOLDANIMATIONCONVERTER_H
#define NIOLDANIMATIONCONVERTER_H

#include <NiMain.h>
#include <NiAnimation.h>


class NIANIMATION_ENTRY NiOldAnimationConverter
{
public:
    // *** begin Emergent internal use only ***

    static void Convert(NiStream& kStream, 
        NiTLargeObjectArray<NiObjectPtr>& kTopObjects);
    static void RecurseScene(NiStream& kStream, NiObjectNET* pkObject);
    // *** end Emergent internal use only ***

protected:
    static void SynchronizeTimeControllers(const NiTimeController* pkSrc,
        NiTimeController* pkDest);
};

#endif  // #ifndef NIOLDANIMATIONCONVERTER_H
