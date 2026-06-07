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

#ifndef NIBATCHCOMMAND_H
#define NIBATCHCOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include <NiTPtrSet.h>

class NIENTITY_ENTRY NiBatchCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiBatchCommand(const NiFixedString& kName,
        unsigned int uiCommandArraySize = 0);

    inline void SetName(const NiFixedString& kName);

    // Functions for managing commands.
    inline void AddCommand(NiEntityCommandInterface* pkCommand);
    inline unsigned int GetCommandCount() const;
    inline NiEntityCommandInterface* GetCommandAt(unsigned int uiIndex) const;
    inline void RemoveCommand(NiEntityCommandInterface* pkCommand);
    inline void RemoveCommandAt(unsigned int uiIndex);
    inline void RemoveAllCommands();

private:
    NiTObjectPtrSet<NiEntityCommandInterfaceIPtr> m_kCommands;
    NiFixedString m_kName;

public:
    // NiEntityCommandInterface overrides.
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetName();
    virtual void DoCommand(NiEntityErrorInterface* pkErrors, bool bUndoable);
    virtual void UndoCommand(NiEntityErrorInterface* pkErrors);
};

NiSmartPointer(NiBatchCommand);

#include "NiBatchCommand.inl"

#endif // NIBATCHCOMMAND_H
