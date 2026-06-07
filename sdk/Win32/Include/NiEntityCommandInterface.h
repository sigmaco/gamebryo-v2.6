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

#ifndef NIENTITYCOMMANDINTERFACE_H
#define NIENTITYCOMMANDINTERFACE_H

#include "NiEntityLibType.h"
#include "NiInterfaceSmartPointer.h"
#include <NiFixedString.h>
#include "NiEntityErrorInterface.h"

class NIENTITY_ENTRY NiEntityCommandInterface
{
public:
    // This function is called whenever a reference is obtained to this
    // interface.
    virtual void AddReference() = 0;

    // This function is called whenever a reference to this interface is
    // discarded.
    virtual void RemoveReference() = 0;

    // Returns the name of this command.
    virtual NiFixedString GetName() = 0;

    // Executes the command.
    virtual void DoCommand(NiEntityErrorInterface* pkErrors, 
        bool bUndoable) = 0;

    // Undoes the command.
    virtual void UndoCommand(NiEntityErrorInterface* pkErrors) = 0;

    inline virtual ~NiEntityCommandInterface() {}
};


NiInterfaceSmartPointer(NiEntityCommandInterface);

#endif // NIENTITYCOMMANDINTERFACE_H
