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

#ifndef NICHANGEPROPERTYCOMMAND_H
#define NICHANGEPROPERTYCOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityPropertyInterface.h"

class NIENTITY_ENTRY NiChangePropertyCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiChangePropertyCommand(
        NiEntityPropertyInterface* pkEntityPropertyInterface, 
        const NiFixedString& kPropertyName, 
        unsigned int uiPropertyIndex);

    inline NiEntityPropertyInterface* GetEntityPropertyInterface() const;
    inline const NiFixedString& GetPropertyName() const;
    inline unsigned int GetPropertyIndex() const;

protected:
    virtual ~NiChangePropertyCommand();

    virtual NiBool StoreOldData() = 0;
    virtual NiBool SetNewData() = 0;
    virtual NiBool SetOldData() = 0;

    NiEntityPropertyInterfaceIPtr m_spEntityPropertyInterface;
    NiFixedString m_kPropertyName;
    unsigned int m_uiPropertyIndex;

private:
    NiFixedString m_kCommandName;
    bool m_bOldDataValid;
    bool m_bIndexInRange;
    bool m_bShouldBeReset;

public:
    // NiEntityCommandInterface overrides.
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetName();
    virtual void DoCommand(NiEntityErrorInterface* pkErrors, bool bUndoable);
    virtual void UndoCommand(NiEntityErrorInterface* pkErrors);
};

NiSmartPointer(NiChangePropertyDataCommand);

#include "NiChangePropertyCommand.inl"

#endif // NiChangePropertyCommand
