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

#ifndef NISELECTENTITIESCOMMAND_H
#define NISELECTENTITIESCOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include <NiTArray.h>
#include "NiEntitySelectionSet.h"

class NIENTITY_ENTRY NiSelectEntitiesCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiSelectEntitiesCommand(
        NiEntitySelectionSet* pkSelectionSet,
        bool bAddToSelectionSet, 
        unsigned int uiEntityArraySize = 0,
        unsigned int uiEntityArrayGrowBy = 1);
    NiSelectEntitiesCommand(
        const NiFixedString& kCommandName,
        NiEntitySelectionSet* pkSelectionSet, 
        bool bAddToSelectionSet,
        unsigned int uiEntityArraySize = 0,
        unsigned int uiEntityArrayGrowBy = 1);

    inline NiEntitySelectionSet* GetSelectionSet() const;
    inline const NiTObjectArray<NiEntityInterfaceIPtr>& GetNewEntities() const;
    inline const NiTObjectArray<NiEntityInterfaceIPtr>& GetOldEntities() const;
    inline NiBool GetAddToSelectionSet() const;

    inline void AddEntityToArray(NiEntityInterface* pkEntity);

private:
    NiEntitySelectionSetPtr m_spSelectionSet;
    NiTObjectArray<NiEntityInterfaceIPtr> m_kNewEntities;
    NiTObjectArray<NiEntityInterfaceIPtr> m_kOldEntities;

    NiFixedString m_kCommandName;
    bool m_bAddToSelectionSet;
    bool m_bOldDataValid;
    bool m_bNewEntitiesValid;

public:
    // NiEntityCommandInterface overrides.
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetName();
    virtual void DoCommand(NiEntityErrorInterface* pkErrors, bool bUndoable);
    virtual void UndoCommand(NiEntityErrorInterface* pkErrors);
};

NiSmartPointer(NiSelectEntitiesCommand);

#include "NiSelectEntitiesCommand.inl"

#endif // NISELECTENTITIESCOMMAND_H
