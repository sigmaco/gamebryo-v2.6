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

#ifndef NIADDREMOVECOMPONENTCOMMAND_H
#define NIADDREMOVECOMPONENTCOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityInterface.h"
#include "NiEntityComponentInterface.h"

class NIENTITY_ENTRY NiAddRemoveComponentCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiAddRemoveComponentCommand(NiEntityInterface* pkEntity,
        NiEntityComponentInterface* pkComponent, bool bAddComponent,
        bool bPerformErrorChecking);

    inline NiEntityInterface* GetEntity() const;
    inline NiEntityComponentInterface* GetComponent() const;
    inline NiBool GetAddComponent() const;
    inline NiBool GetPerformErrorChecking() const;

private:
    NiFixedString m_kCommandName;

    NiEntityInterfaceIPtr m_spEntity;
    NiEntityComponentInterfaceIPtr m_spComponent;
    bool m_bAddComponent;
    bool m_bPerformErrorChecking;

    bool m_bSuccessfullyExecuted;

public:
    // NiEntityCommandInterface overrides.
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetName();
    virtual void DoCommand(NiEntityErrorInterface* pkErrors, bool bUndoable);
    virtual void UndoCommand(NiEntityErrorInterface* pkErrors);
};

NiSmartPointer(NiAddRemoveComponentCommand);

#include "NiAddRemoveComponentCommand.inl"

#endif // NIADDREMOVECOMPONENTCOMMAND_H
