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

#ifndef NIRESETBOOLDATACOMMAND_H
#define NIRESETBOOLDATACOMMAND_H

#include "NiChangePropertyCommand.h"

class NIENTITY_ENTRY NiResetBoolDataCommand :
    public NiChangePropertyCommand
{
public:
    NiResetBoolDataCommand(
        NiEntityPropertyInterface* pkEntityPropertyInterface,
        const NiFixedString& kPropertyName);
    virtual ~NiResetBoolDataCommand();

private:
    bool* m_pbOldData;
    unsigned int m_uiOldDataCount;

protected:
    // NiChangePropertyCommand overrides.
    virtual NiBool StoreOldData();
    virtual NiBool SetNewData();
    virtual NiBool SetOldData();
};

NiSmartPointer(NiResetBoolDataCommand);

#endif // NIRESETBOOLDATACOMMAND_H
