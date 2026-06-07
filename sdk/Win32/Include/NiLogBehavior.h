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

#ifndef NILOGBEHAVIOR_H
#define NILOGBEHAVIOR_H
#include "NiSystemLibType.h"

class NISYSTEM_ENTRY NiLogBehavior
{
public:
    static void Set(NiLogBehavior* pkBehavior);
    static NiLogBehavior* Get();

    // override this to change log behavior
    virtual void Initialize();

    virtual ~NiLogBehavior();

private:
    static NiLogBehavior* ms_pkSingleton;
    static NiLogBehavior ms_kDefault;
};

#endif // #ifndef NILOGBEHAVIOR_H
