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

#ifndef NICHANGEPOINT2DATACOMMAND_H
#define NICHANGEPOINT2DATACOMMAND_H

#include "NiChangePropertyCommand.h"
#include <NiPoint2.h>

class NIENTITY_ENTRY NiChangePoint2DataCommand :
    public NiChangePropertyCommand
{
public:
    NiChangePoint2DataCommand(
        NiEntityPropertyInterface* pkEntityPropertyInterface,
        const NiFixedString& kPropertyName, const NiPoint2& kData,
        unsigned int uiPropertyIndex = 0);

private:
    NiPoint2 m_kNewData;
    NiPoint2 m_kOldData;

protected:
    // NiChangePropertyCommand overrides.
    virtual NiBool StoreOldData();
    virtual NiBool SetNewData();
    virtual NiBool SetOldData();
};

NiSmartPointer(NiChangePoint2DataCommand);

#endif // NICHANGEPOINT2DATACOMMAND_H
