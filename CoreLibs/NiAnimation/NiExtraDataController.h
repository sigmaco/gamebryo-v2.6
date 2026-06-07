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

#ifndef NIEXTRADATACONTROLLER_H
#define NIEXTRADATACONTROLLER_H

#include "NiSingleInterpController.h"
#include <NiExtraData.h>

class NiObjectNET;

class NIANIMATION_ENTRY NiExtraDataController :
    public NiSingleInterpController
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiExtraDataController);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

public:
    virtual ~NiExtraDataController();
    
    // Gets controller's copy of the extra data name (used to find extra data
    // pointer).
    inline const NiFixedString& GetExtraDataName();

    // Gets a pointer to the extra data object that this controller animates.
    // If the controller has no target, NULL is returned.
    inline NiExtraData* GetExtraDataPointer();

    // Overridden from NiTimeController so that m_spExtraData can be set
    // appropriately.
    virtual void SetTarget(NiObjectNET* pkTarget);

    // *** begin Emergent internal use only ***
    // Cloning support.
    virtual void ProcessClone(NiCloningProcess& kCloning);
    void PostLinkObject(NiStream& kStream);

    // Interpolator support
    virtual const char* GetCtlrID();
    // *** end Emergent internal use only ***

protected:
    NiExtraDataController(const NiFixedString& kExtraDataName);

    // For cloning and streaming only.
    NiExtraDataController();

    virtual bool InterpTargetIsCorrectType(NiObjectNET* pkTarget) const;

    NiFixedString m_kExtraDataName;
    NiExtraDataPtr m_spExtraData;

private:
    // Sets m_spExtraData to point to the correct extra data, using the
    // stored name.
    inline void GetExtraDataPointerFromName();

    // Sets the m_pcExtraDataName pointer, making its own copy of the string.
    inline void SetExtraDataName(const NiFixedString& kName);
};

NiSmartPointer(NiExtraDataController);

#include "NiExtraDataController.inl"

#endif  // #ifndef NIEXTRADATACONTROLLER_H
