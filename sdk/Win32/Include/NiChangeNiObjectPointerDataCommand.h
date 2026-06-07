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

#ifndef NICHANGENIOBJECTPOINTERDATACOMMAND_H
#define NICHANGENIOBJECTPOINTERDATACOMMAND_H

#include "NiChangePropertyCommand.h"
#include <NiObject.h>

class NIENTITY_ENTRY NiChangeNiObjectPointerDataCommand :
    public NiChangePropertyCommand
{
public:
    NiChangeNiObjectPointerDataCommand(
        NiEntityPropertyInterface* pkEntityPropertyInterface,
        const NiFixedString& kPropertyName, NiObject* pkData,
        unsigned int uiPropertyIndex = 0);

private:
    NiObjectPtr m_spNewData;
    NiObjectPtr m_spOldData;

protected:
    // NiChangePropertyCommand overrides.
    virtual NiBool StoreOldData();
    virtual NiBool SetNewData();
    virtual NiBool SetOldData();
};

NiSmartPointer(NiChangeNiObjectPointerDataCommand);

#endif // NICHANGENIOBJECTPOINTERDATACOMMAND_H
