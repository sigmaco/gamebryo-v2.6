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

#ifndef NINAVIGATIONMANAGER_H
#define NINAVIGATIONMANAGER_H

#include "NiUserInterfaceLibType.h"
#include <NiRect.h>
#include <NiTArray.h>
#include <NiRefObject.h>
#include "NiUISlot.h"
#include "NiUILabel.h"
#include "NiUIButton.h"
#include "NiUIGroup.h"
#include "NiNavBaseController.h"
//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiNavManager : public NiRefObject
{
private:
    inline NiNavManager();
    ~NiNavManager();

    static NiNavManager* m_pkNavManager;
public:
    static bool Create();
    static NiNavManager* GetNavManager();
    static void Shutdown();

    unsigned int AddNavigationController(NiNavBaseController* pkNavController);
    void RemoveNavigationController(NiNavBaseController* pkNavController);
    inline void Update(float fCurTime);
    inline float GetFrameTime();
    inline unsigned int GetNumNavigationControllers() const;

    NiNavBaseController* GetNavControllerAt(unsigned int ui) const;
    NiNavBaseController* GetCurrentNavController() const;
    unsigned int GetCurrentNavControllerIndex() const;
    inline int GetControllerIdx(NiNavBaseController* pkNavController);

    void AdvanceController();
    void SetActiveController(NiNavBaseController* pkNavController);
    void SubscribeToControllerChanged(
        NiUIBaseSlot1<NiNavBaseController*>* kControllerChangedSlot);

protected:
    NiUISignal1<NiNavBaseController*> m_kActiveControllerChangedSignal;

    float m_fLastFrameTime;
    float m_fThisFrameDelta;
    NiTObjectArray<NiNavBaseControllerPtr> m_kControllers;
    unsigned int m_uiCurController;
};
NiSmartPointer(NiNavManager);
//---------------------------------------------------------------------------
#include "NiNavManager.inl"
//---------------------------------------------------------------------------

#endif

