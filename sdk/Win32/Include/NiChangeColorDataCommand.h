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

#ifndef NICHANGECOLORDATACOMMAND_H
#define NICHANGECOLORDATACOMMAND_H

#include "NiChangePropertyCommand.h"
#include <NiColor.h>

class NIENTITY_ENTRY NiChangeColorDataCommand :
    public NiChangePropertyCommand
{
public:
    NiChangeColorDataCommand(
        NiEntityPropertyInterface* pkEntityPropertyInterface,
        const NiFixedString& kPropertyName, const NiColor& kData,
        unsigned int uiPropertyIndex = 0);

private:
    NiColor m_kNewData;
    NiColor m_kOldData;

protected:
    // NiChangePropertyCommand overrides.
    virtual NiBool StoreOldData();
    virtual NiBool SetNewData();
    virtual NiBool SetOldData();
};

NiSmartPointer(NiChangeColorDataCommand);

#endif // NICHANGECOLORDATACOMMAND_H
