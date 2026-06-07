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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiShaderTimeController.h"
#include "NiNode.h"

NiTPointerList<NiObjectNETPtr>* 
    NiShaderTimeController::ms_pkObjectsList = NULL;
NiTPointerList<NiShaderTimeControllerPtr>* 
    NiShaderTimeController::ms_pkControllersList = NULL;

NiImplementRTTI(NiShaderTimeController, NiTimeController);
//---------------------------------------------------------------------------
NiShaderTimeController::NiShaderTimeController()
{
    m_fLoKeyTime = -NI_INFINITY;
    m_fHiKeyTime = NI_INFINITY;
}
//---------------------------------------------------------------------------
NiShaderTimeController::~NiShaderTimeController()
{
}
//---------------------------------------------------------------------------
void NiShaderTimeController::RemoveBeforeStreaming(NiObjectNET* pkRoot)
{
    if (pkRoot == NULL)
        return;
    
    NiTimeController* pkController = pkRoot->GetControllers();
    unsigned int uiNumControllersQueued = 0;
    while (pkController)
    {
        if (NiIsKindOf(NiShaderTimeController, pkController))
        {
            if (ms_pkObjectsList == NULL)
            {
                ms_pkObjectsList = NiNew NiTPointerList<NiObjectNETPtr>;
            }
            if (ms_pkControllersList == NULL)
            {
                ms_pkControllersList = NiNew NiTPointerList<
                   NiShaderTimeControllerPtr>;
            }

            ms_pkObjectsList->AddHead(pkRoot);
            NiShaderTimeControllerPtr spCont =
                (NiShaderTimeController*) pkController;
            ms_pkControllersList->AddHead(spCont);
            uiNumControllersQueued++;
        }
        pkController = pkController->GetNext();
    }    

    NiTListIterator kIter = ms_pkControllersList->GetHeadPos();
    while (uiNumControllersQueued != 0)
    {
        NIASSERT(kIter != NULL);
        NiShaderTimeControllerPtr spController = 
            ms_pkControllersList->GetNext(kIter);
        NIASSERT(spController != NULL);
        spController->SetTarget(NULL);
        uiNumControllersQueued--;
    }

    if (NiIsKindOf(NiAVObject, pkRoot))
    {
        NiAVObject* pkObj = (NiAVObject*) pkRoot;
        const NiPropertyList& kPropList = pkObj->GetPropertyList();

        NiTListIterator kIt = kPropList.GetHeadPos();
        while (kIt != NULL)
        {
            NiProperty* pkProp = kPropList.GetNext(kIt);
            RemoveBeforeStreaming(pkProp);
        }
    }

    if (NiIsKindOf(NiNode, pkRoot))
    {
        NiNode* pkNode = (NiNode*) pkRoot;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            RemoveBeforeStreaming(pkNode->GetAt(ui));
        }

        const NiDynamicEffectList& kEffectList = pkNode->GetEffectList();
        
        NiTListIterator kIt = kEffectList.GetHeadPos();
        while (kIt != NULL)
        {
            NiDynamicEffect* pkEffect = kEffectList.GetNext(kIt);
            RemoveBeforeStreaming(pkEffect);
        }
    }
}
//---------------------------------------------------------------------------
void NiShaderTimeController::AddAfterStreaming()
{
    if (ms_pkObjectsList != NULL && ms_pkControllersList != NULL)
        return;

    NiTListIterator kContIter = ms_pkControllersList->GetHeadPos();
    NiTListIterator kObjIter = ms_pkObjectsList->GetHeadPos();

    while (kContIter != NULL && kObjIter != NULL)
    {
        NiShaderTimeControllerPtr spController =
            ms_pkControllersList->GetNext(kContIter);
        NiObjectNETPtr spObject = ms_pkObjectsList->GetNext(kObjIter);

        NIASSERT(spController != NULL);
        NIASSERT(spObject != NULL);

        spController->SetTarget(spObject);
    }

    ms_pkObjectsList->RemoveAll();
    ms_pkControllersList->RemoveAll();
}
//---------------------------------------------------------------------------
