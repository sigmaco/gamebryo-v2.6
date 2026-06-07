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

#ifndef NINAVINTERPTOCONTROLLER_H
#define NINAVINTERPTOCONTROLLER_H

#include "NiUserInterfaceLibType.h"
#include "NiNavBaseController.h"
//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiNavInterpToController : 
    public NiNavBaseController
{
public:
    NiNavInterpToController(
        NiAVObject* pkInterpToObject, 
        NiAVObject* pkControlledObject,
        const char* pcName,
        float fInterpTime);
    virtual ~NiNavInterpToController();

    virtual bool SetActive(bool bActive, NavState& eCurState);

    virtual const char* GetControlName(unsigned int uiControlId) const;

    inline void SetInterpToObject(NiAVObject* pkInterpToObject);
    inline void SetControlledObject(NiAVObject* pkControlledObject);

    void RestartInterpolation();

protected:
    virtual void MouseInput(unsigned int uiControlId, float fDelta);
    virtual void GamePadInput(unsigned int uiControlId, float fDelta);
    virtual void UpdatePose();

    NiAVObjectPtr m_spInterpToObject;
    NiAVObjectPtr m_spControlledObject;
    NavState m_kNavState;
    float m_fTime;
    float m_fInterpTime;
    NiTransform m_kStartingTransform;
};
NiSmartPointer(NiNavInterpToController);
//---------------------------------------------------------------------------

#endif
