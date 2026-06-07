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

#ifndef NICHANGEBOOLDATACOMMAND_H
#define NICHANGEBOOLDATACOMMAND_H

#include "NiChangePropertyCommand.h"

class NIENTITY_ENTRY NiChangeBoolDataCommand :
    public NiChangePropertyCommand
{
public:
    NiChangeBoolDataCommand(
        NiEntityPropertyInterface* pkEntityPropertyInterface,
        const NiFixedString& kPropertyName, bool bData,
        unsigned int uiPropertyIndex = 0);

private:
    bool m_bNewData;
    bool m_bOldData;

protected:
    // NiChangePropertyCommand overrides.
    virtual NiBool StoreOldData();
    virtual NiBool SetNewData();
    virtual NiBool SetOldData();
};

NiSmartPointer(NiChangeBoolDataCommand);

#endif // NICHANGEBOOLDATACOMMAND_H
