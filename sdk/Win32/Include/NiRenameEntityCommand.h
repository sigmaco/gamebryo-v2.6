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

#ifndef NIRENAMEENTITYCOMMAND_H
#define NIRENAMEENTITYCOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityInterface.h"

class NIENTITY_ENTRY NiRenameEntityCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiRenameEntityCommand(NiEntityInterface* pkEntity,
        const NiFixedString& kNewName);

    inline NiEntityInterface* GetEntity() const;
    inline const NiFixedString& GetNewName() const;
    inline const NiFixedString& GetOldName() const;

private:
    NiEntityInterfaceIPtr m_spEntity;
    NiFixedString m_kNewName;
    NiFixedString m_kOldName;

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

NiSmartPointer(NiRenameEntityCommand);

#include "NiRenameEntityCommand.inl"

#endif // NIRENAMEENTITYCOMMAND_H
