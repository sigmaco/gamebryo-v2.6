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

#ifndef NISETELEMENTCOUNTCOMMAND_H
#define NISETELEMENTCOUNTCOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityInterface.h"

class NIENTITY_ENTRY NiSetElementCountCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiSetElementCountCommand(NiEntityPropertyInterface* pkPropertyInterface,
        const NiFixedString& kPropertyName, unsigned int uiNewCount);

    inline NiEntityPropertyInterface* GetPropertyInterface() const;
    inline const NiFixedString& GetPropertyName() const;
    inline unsigned int GetOldCount() const;
    inline unsigned int GetNewCount() const;

private:
    NiEntityPropertyInterfaceIPtr m_spPropertyInterface;
    NiFixedString m_kPropertyName;
    unsigned int m_uiOldCount;
    unsigned int m_uiNewCount;

    NiFixedString m_kCommandName;
    bool m_bOldCountValid;
    bool m_bAttemptUndo;

public:
    // NiEntityCommandInterface overrides.
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetName();
    virtual void DoCommand(NiEntityErrorInterface* pkErrors, bool bUndoable);
    virtual void UndoCommand(NiEntityErrorInterface* pkErrors);
};

NiSmartPointer(NiSetElementCountCommand);

#include "NiSetElementCountCommand.inl"

#endif // NISETELEMENTCOUNTCOMMAND_H
