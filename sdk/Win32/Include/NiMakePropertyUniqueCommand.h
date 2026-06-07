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

#ifndef NIMAKEPROPERTYUNIQUECOMMAND_H
#define NIMAKEPROPERTYUNIQUECOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityInterface.h"

class NIENTITY_ENTRY NiMakePropertyUniqueCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiMakePropertyUniqueCommand(
        NiEntityPropertyInterface* pkEntityPropertyInterface,
        const NiFixedString& kPropertyName);

    inline NiEntityPropertyInterface* GetEntityPropertyInterface() const;
    inline const NiFixedString& GetPropertyName() const;

protected:
    NiEntityPropertyInterfaceIPtr m_spEntityPropertyInterface;
    NiFixedString m_kPropertyName;
    bool m_bMadeUnique;

private:
    NiFixedString m_kCommandName;

public:
    // NiEntityCommandInterface overrides.
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetName();
    virtual void DoCommand(NiEntityErrorInterface* pkErrors, bool bUndoable);
    virtual void UndoCommand(NiEntityErrorInterface* pkErrors);
};

NiSmartPointer(NiMakePropertyUniqueDataCommand);

#include "NiMakePropertyUniqueCommand.inl"

#endif // NIMAKEPROPERTYUNIQUECOMMAND_H
