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

#ifndef NICOLORDIALOG_H
#define NICOLORDIALOG_H

#include <NiOS.h>
#include <NiUniversalTypes.h>
#include "NiColor.h"

class NifColorDialog : public NiMemObject
{
public:
    static bool GetColor(NiWindowRef hWnd, NiColor& kColor);
    
protected:
    enum
    {
        NUM_CUST_COLORS = 16
    };
    static COLORREF ms_akCustColors[NUM_CUST_COLORS];
};

#endif // NICOLORDIALOG_H