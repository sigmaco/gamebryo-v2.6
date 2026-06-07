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

#ifndef NIENTITYERRORINTERFACE_H
#define NIENTITYERRORINTERFACE_H

#include "NiEntityLibType.h"
#include "NiInterfaceSmartPointer.h"
#include <NiFixedString.h>

class NIENTITY_ENTRY NiEntityErrorInterface
{
public:
    // This function is called whenever a reference is obtained to an
    // NiEntityErrorInterface.
    virtual void AddReference() = 0;

    // This function is called whenever a reference to an
    // NiEntityErrorInterface is discarded.
    virtual void RemoveReference() = 0;

    // Adds the specified error to the collection of errors.
    virtual void ReportError(const NiFixedString& kErrorMessage,
        const NiFixedString& kErrorDescription,
        const NiFixedString& kEntityName, const NiFixedString& kPropertyName)
        = 0;

    // Gets the count of all reported errors.
    virtual unsigned int GetErrorCount() const = 0;

    // Gets the message for the error at the specified index.
    virtual const NiFixedString& GetErrorMessage(unsigned int uiIndex) const
        = 0;

    // Gets the description for the error at the specified index.
    virtual const NiFixedString& GetErrorDescription(unsigned int uiIndex)
        const = 0;

    // Gets the entity name for the error at the specified index.
    virtual const NiFixedString& GetEntityName(unsigned int uiIndex) const
        = 0;

    // Gets the property name for the error at the specified index.
    virtual const NiFixedString& GetPropertyName(unsigned int uiIndex) const
        = 0;

    // Clears the array of all reported errors.
    virtual void ClearErrors() = 0;

    // *** begin Emergent internal use only ***
    inline virtual ~NiEntityErrorInterface() {}
    // *** end Emergent internal use only ***

};

NiInterfaceSmartPointer(NiEntityErrorInterface);

#endif // NIENTITYERRORINTERFACE_H
