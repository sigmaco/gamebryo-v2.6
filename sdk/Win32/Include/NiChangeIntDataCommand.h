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

#ifndef NICHANGEINTDATACOMMAND_H
#define NICHANGEINTDATACOMMAND_H

#include "NiChangePropertyCommand.h"

class NIENTITY_ENTRY NiChangeIntDataCommand :
    public NiChangePropertyCommand
{
public:
    NiChangeIntDataCommand(
        NiEntityPropertyInterface* pkEntityPropertyInterface,
        const NiFixedString& kPropertyName, int iData,
        unsigned int uiPropertyIndex = 0);

private:
    int m_iNewData;
    int m_iOldData;

protected:
    // NiChangePropertyCommand overrides.
    virtual NiBool StoreOldData();
    virtual NiBool SetNewData();
    virtual NiBool SetOldData();
};

NiSmartPointer(NiChangeIntDataCommand);

#endif // NICHANGEINTDATACOMMAND_H
