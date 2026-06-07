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

#ifndef NICHANGEHIDDENSTATECOMMAND_H
#define NICHANGEHIDDENSTATECOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityInterface.h"

class NIENTITY_ENTRY NiChangeHiddenStateCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiChangeHiddenStateCommand(NiEntityInterface* pkEntity, 
        bool bNewHiddenState);

    inline NiEntityInterface* GetEntity() const;
    inline NiBool GetOldHiddenState() const;
    inline NiBool GetNewHiddenState() const;

private:
    NiEntityInterfaceIPtr m_spEntity;
    bool m_bOldHiddenState;
    bool m_bNewHiddenState;

    NiFixedString m_kCommandName;
    bool m_bOldDataValid;

public:
    // NiEntityCommandInterface overrides.
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetName();
    virtual void DoCommand(NiEntityErrorInterface* pkErrors, bool bUndoable);
    virtual void UndoCommand(NiEntityErrorInterface* pkErrors);
};

NiSmartPointer(NiChangeHiddenStateCommand);

#include "NiChangeHiddenStateCommand.inl"

#endif // NICHANGEHIDDENSTATECOMMAND_H
