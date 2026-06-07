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

#ifndef NICHANGEPOINT3DATACOMMAND_H
#define NICHANGEPOINT3DATACOMMAND_H

#include "NiChangePropertyCommand.h"
#include <NiPoint3.h>

class NIENTITY_ENTRY NiChangePoint3DataCommand :
    public NiChangePropertyCommand
{
public:
    NiChangePoint3DataCommand(
        NiEntityPropertyInterface* pkEntityPropertyInterface,
        const NiFixedString& kPropertyName, const NiPoint3& kData,
        unsigned int uiPropertyIndex = 0);

private:
    NiPoint3 m_kNewData;
    NiPoint3 m_kOldData;

protected:
    // NiChangePropertyCommand overrides.
    virtual NiBool StoreOldData();
    virtual NiBool SetNewData();
    virtual NiBool SetOldData();
};

NiSmartPointer(NiChangePoint3DataCommand);

#endif // NICHANGEPOINT3DATACOMMAND_H
