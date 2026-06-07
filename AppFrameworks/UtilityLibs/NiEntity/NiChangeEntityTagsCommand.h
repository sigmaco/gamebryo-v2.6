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

#ifndef NICHANGEENTITYTAGSCOMMAND_H
#define NICHANGEENTITYTAGSCOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityInterface.h"

class NIENTITY_ENTRY NiChangeEntityTagsCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    enum TagOperation
    {
        TAGOP_SET,
        TAGOP_ADD,
        TAGOP_REMOVE
    };

    NiChangeEntityTagsCommand(
        NiEntityInterface* pkEntity,
        TagOperation eTagOp,
        const NiFixedString& kTag);

    inline NiEntityInterface* GetEntity() const;
    inline TagOperation GetTagOp() const;
    inline const NiFixedString& GetTag() const;

private:
    NiFixedString m_kCommandName;
    NiFixedString m_kOldTags;

    NiEntityInterfaceIPtr m_spEntity;
    NiFixedString m_kTag;
    TagOperation m_eTagOp;

    bool m_bOldDataValid;

// NiEntityCommandInterface overrides.
public:
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetName();
    virtual void DoCommand(NiEntityErrorInterface* pkErrors, bool bUndoable);
    virtual void UndoCommand(NiEntityErrorInterface* pkErrors);
};

NiSmartPointer(NiChangeEntityTagsCommand);

#include "NiChangeEntityTagsCommand.inl"

#endif  // #ifndef NICHANGEENTITYTAGSCOMMAND_H
