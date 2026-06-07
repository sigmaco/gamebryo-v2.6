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

#ifndef NISHADERTIMECONTROLLER_H
#define NISHADERTIMECONTROLLER_H

#include "NiTimeController.h"
#include "NiTPointerList.h"
#include "NiObjectNET.h"

NiSmartPointer(NiShaderTimeController);

class NIMAIN_ENTRY NiShaderTimeController : public NiTimeController
{
    NiDeclareRTTI;

public:
    NiShaderTimeController();
    virtual ~NiShaderTimeController();

    static void RemoveBeforeStreaming(NiObjectNET* pkRoot);
    static void AddAfterStreaming();
protected:
    static NiTPointerList<NiObjectNETPtr>* ms_pkObjectsList;
    static NiTPointerList<NiShaderTimeControllerPtr>* ms_pkControllersList;
};

typedef NiPointer<NiShaderTimeController> NiShaderTimeControllerPtr;

#endif  // #ifndef NISHADERTIMECONTROLLER_H
