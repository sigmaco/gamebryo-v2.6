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

#ifndef NIADDREMOVEENTITYCOMMAND_H
#define NIADDREMOVEENTITYCOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiScene.h"

class NIENTITY_ENTRY NiAddRemoveEntityCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiAddRemoveEntityCommand(NiScene* pkScene,
        NiEntityInterface* pkEntity, bool bAddEntity);

    inline NiScene* GetScene() const;
    inline NiEntityInterface* GetEntity() const;
    inline NiBool GetAddEntity() const;

private:
    NiFixedString m_kCommandName;

    NiScenePtr m_spScene;
    NiEntityInterfaceIPtr m_spEntity;
    bool m_bAddEntity;

    bool m_bSuccessfullyExecuted;

public:
    // NiEntityCommandInterface overrides.
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetName();
    virtual void DoCommand(NiEntityErrorInterface* pkErrors, bool bUndoable);
    virtual void UndoCommand(NiEntityErrorInterface* pkErrors);
};

NiSmartPointer(NiAddRemoveEntityCommand);

#include "NiAddRemoveEntityCommand.inl"

#endif // NIADDREMOVEENTITYCOMMAND_H
