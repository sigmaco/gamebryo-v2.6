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

#ifndef NIADDREMOVESELECTIONSETCOMMAND_H
#define NIADDREMOVESELECTIONSETCOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiScene.h"

class NIENTITY_ENTRY NiAddRemoveSelectionSetCommand :
    public NiRefObject, public NiEntityCommandInterface
{
public:
    NiAddRemoveSelectionSetCommand(
        NiScene* pkScene,
        NiEntitySelectionSet* pkSelectionSet, 
        bool bAddSelectionSet);

    inline NiScene* GetScene() const;
    inline NiEntitySelectionSet* GetSelectionSet() const;
    inline NiBool GetAddSelectionSet() const;

private:
    NiFixedString m_kCommandName;

    NiScenePtr m_spScene;
    NiEntitySelectionSetPtr m_spSelectionSet;
    bool m_bAddSelectionSet;

    bool m_bSuccessfullyExecuted;

public:
    // NiEntityCommandInterface overrides.
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetName();
    virtual void DoCommand(NiEntityErrorInterface* pkErrors, bool bUndoable);
    virtual void UndoCommand(NiEntityErrorInterface* pkErrors);
};

NiSmartPointer(NiAddRemoveSelectionSetCommand);

#include "NiAddRemoveSelectionSetCommand.inl"

#endif // NIADDREMOVESELECTIONSETCOMMAND_H
