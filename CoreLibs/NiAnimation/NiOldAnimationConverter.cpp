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
#include "NiAnimationPCH.h"

#include "NiOldAnimationConverter.h"

//---------------------------------------------------------------------------
void NiOldAnimationConverter::Convert(NiStream& kStream, 
    NiTLargeObjectArray<NiObjectPtr>& kTopObjects)
{
    // If this file is new enough, then it is already correct
    if (kStream.GetFileVersion() >= NiAnimationConstants::GetPoseVersion())
    {
        return;
    }

    for (unsigned int i = 0; i < kTopObjects.GetSize(); i++)
    {
        NiObject* pkObject = kTopObjects.GetAt(i);
        if (NiIsKindOf(NiObjectNET, pkObject))
        {
            RecurseScene(kStream, (NiObjectNET*) pkObject);
        }
    }
}
//---------------------------------------------------------------------------
void NiOldAnimationConverter::RecurseScene(NiStream& kStream, 
    NiObjectNET* pkObject)
{
    if (pkObject == NULL)
        return;

    if (NiIsKindOf (NiAVObject, pkObject))
    {
        // Iterate over all time controllers, converting as necessary
        NiTimeController* pkController = pkObject->GetControllers();
        NiTimeController* pkNextController = NULL;
        while (pkController != NULL)
        {
            // Hold on to the next pointer, as it might change in the
            // conversion process
            pkNextController = pkController->GetNext();
            if (NiIsKindOf(NiMultiTargetTransformController, 
                pkController))
            {
                // Remove the deprecated NiMultiTargetTransformController.
                // These are no longer required for sequence animation
                // since the NiMultiTargetPoseHandler communicates directly
                // with the scene graph transform nodes.
                pkObject->RemoveController(pkController);

                // Set the SelectiveUpdate flags now that the controller
                // has been removed.
                bool bSelectiveUpdate, bRigid;
                ((NiAVObject*)pkObject)->SetSelectiveUpdateFlags(
                    bSelectiveUpdate, true, bRigid);
            }
            
            pkController = pkNextController;
        }

        // If we ever need to convert property time controllers, this code 
        // should be uncommented
//        NiPropertyList& kPropList = pkAVObject->GetPropertyList();
//        NiTListIterator kNext = kPropList.GetHead();
//        while (kNext != NULL)
//        {
//            NiProperty* pkProperty = kPropList.GetNext(kNext);
//            RecurseScene(kStream, kProperty);
//        }

        // If this is an NiNode, we need to convert its children
        if (NiIsKindOf(NiNode, pkObject))
        {
            NiNode* pkNode = (NiNode*) pkObject;
            for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
                RecurseScene(kStream, pkNode->GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOldAnimationConverter::SynchronizeTimeControllers(
    const NiTimeController* pkSrc, NiTimeController* pkDest)
{
    pkDest->SetAnimType(pkSrc->GetAnimType());
    pkDest->SetCycleType(pkSrc->GetCycleType());
    pkDest->SetPlayBackwards(pkSrc->GetPlayBackwards());
    pkDest->SetFrequency(pkSrc->GetFrequency());
    pkDest->SetBeginKeyTime(pkSrc->GetBeginKeyTime());
    pkDest->SetEndKeyTime(pkSrc->GetEndKeyTime());
    pkDest->SetActive(pkSrc->GetActive());
}
//---------------------------------------------------------------------------
