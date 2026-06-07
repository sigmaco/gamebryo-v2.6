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

#ifndef NIRESETPROPERTYDATACOMMANDFACTORY_H
#define NIRESETPROPERTYDATACOMMANDFACTORY_H

#include "NiEntityLibType.h"

class NiChangePropertyCommand;
class NiEntityPropertyInterface;
class NiFixedString;

class NIENTITY_ENTRY NiResetPropertyDataCommandFactory
{
public:
    static NiChangePropertyCommand* GetResetDataCommand(
        NiEntityPropertyInterface* pkEntityPropertyInterface,
        const NiFixedString& kPropertyName);
};

#endif // NIRESETPROPERTYDATACOMMANDFACTORY_H
