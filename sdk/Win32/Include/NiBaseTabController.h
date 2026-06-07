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

#ifndef NIBASETABCONTROLLER_H
#define NIBASETABCONTROLLER_H

#include <NiMemObject.h>

class NiBaseTabController : public NiMemObject
{
public:
    virtual ~NiBaseTabController();

    // Creates and initializes a dialog
    virtual NiWindowRef InitDialog(NiWindowRef pParentWnd) = 0;

    // Returns name of a tab
    virtual char* GetCaption() = 0;

    // Functions for changing a tab height
    virtual unsigned int SetBasicHeight() = 0;
    virtual unsigned int SetAdvancedHeight() = 0;
};

#include "NiBaseTabController.inl"

#endif // NIBASETABCONTROLLER_H