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


#ifndef NIDEFAULTERRORHANDLER_H
#define NIDEFAULTERRORHANDLER_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityErrorInterface.h"
#include <NiTPtrSet.h>

class NIENTITY_ENTRY NiDefaultErrorHandler : public NiRefObject,
    public NiEntityErrorInterface
{
public:
    NiDefaultErrorHandler(unsigned int uiErrorArraySize = 0);

    // NiEntityErrorInterface overrides.
    virtual void AddReference();
    virtual void RemoveReference();
    virtual void ReportError(const NiFixedString& kErrorMessage,
        const NiFixedString& kErrorDescription,
        const NiFixedString& kEntityName, const NiFixedString& kPropertyName);
    virtual unsigned int GetErrorCount() const;
    virtual const NiFixedString& GetErrorMessage(unsigned int uiIndex) const;
    virtual const NiFixedString& GetErrorDescription(unsigned int uiIndex)
        const;
    virtual const NiFixedString& GetEntityName(unsigned int uiIndex) const;
    virtual const NiFixedString& GetPropertyName(unsigned int uiIndex) const;
    virtual void ClearErrors();

protected:
    class Error : public NiRefObject
    {
    public:
        Error(const NiFixedString& kMessage,
            const NiFixedString& kDescription,
            const NiFixedString& kEntityName,
            const NiFixedString& kPropertyName);

        NiFixedString m_kMessage;
        NiFixedString m_kDescription;
        NiFixedString m_kEntityName;
        NiFixedString m_kPropertyName;
    };
    typedef NiPointer<Error> ErrorPtr;

    NiTObjectPtrSet<ErrorPtr> m_kErrors;
};

NiSmartPointer(NiDefaultErrorHandler);

#include "NiDefaultErrorHandler.inl"

#endif // NIDEFAULTERRORHANDLER_H
