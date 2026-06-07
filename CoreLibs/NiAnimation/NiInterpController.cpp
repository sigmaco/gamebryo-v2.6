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

#include "NiInterpController.h"
#include <NiNode.h>
#include <NiBool.h>

NiImplementRTTI(NiInterpController, NiTimeController);

const unsigned short NiInterpController::INVALID_INDEX = (unsigned short) -1;
const float NiInterpController::INVALID_TIME = -FLT_MAX;

//---------------------------------------------------------------------------
NiInterpController::NiInterpController()
{
    SetManagerControlled(false);
}
//---------------------------------------------------------------------------
void NiInterpController::ResetTimeExtrema()
{
    if (GetManagerControlled())
    {
        // Do nothing if this controller is being controlled by a manager.
        return;
    }

    m_fLoKeyTime = 0.0f;
    m_fHiKeyTime = 0.0f;
}
//---------------------------------------------------------------------------
const char* NiInterpController::GetCtlrID() 
{
    return NULL;
}
//---------------------------------------------------------------------------
bool NiInterpController::UpdateValue(float, bool, unsigned short)
{
    // This data type is not supported in the base class.
    NIASSERT(false);
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpController::UpdateValue(float, float, unsigned short)
{
    // This data type is not supported in the base class.
    NIASSERT(false);
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpController::UpdateValue(float, const NiColorA&, unsigned short)
{
    // This data type is not supported in the base class.
    NIASSERT(false);
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpController::UpdateValue(float, const NiPoint3&, unsigned short)
{
    // This data type is not supported in the base class.
    NIASSERT(false);
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpController::UpdateValue(float, const NiQuaternion&, 
    unsigned short)
{
    // This data type is not supported in the base class.
    NIASSERT(false);
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpController::UpdateValue(float, 
    NiControllerSequence*, 
    NiEvaluator*, 
    NiEvaluatorSPData**,
    unsigned short)
{
    // This data type is not supported in the base class.
    NIASSERT(false);
    return false;
}
//---------------------------------------------------------------------------
void NiInterpController::GuaranteeTimeRange(NiAVObject* pkObject,
    float fStartTime, float fEndTime, const char* pcExcludedControllerTypes)
{
    // Guarantee keys at start and end times
    // for any node with an NiInterpController and keys
    // (the NiInterpController-derived class must have implemented
    // GuaranteeTimeRange).

    char acTemp[NI_MAX_PATH];

    // Guarantee keys for each controller on this object.
    NiTimeController* pkCtlr = pkObject->GetControllers();
    while (pkCtlr)
    {
        NiInterpController* pkInterpCtlr = NiDynamicCast(NiInterpController,
            pkCtlr);
        if (pkInterpCtlr)
        {
            NiSprintf(acTemp, NI_MAX_PATH, "%s;",
                pkInterpCtlr->GetRTTI()->GetName());
            if (!pcExcludedControllerTypes ||
                !strstr(pcExcludedControllerTypes, acTemp))
            {
                pkInterpCtlr->GuaranteeTimeRange(fStartTime, fEndTime);
            }
        }
        pkCtlr = pkCtlr->GetNext();
    }

    // Iterate through the properties on this object.
    NiTListIterator kPos = pkObject->GetPropertyList().GetHeadPos();
    while (kPos)
    {
        NiProperty* pkProp = pkObject->GetPropertyList().GetNext(kPos);
        if (pkProp)
        {
            // Guarantee keys for each controller on this property.
            pkCtlr = pkProp->GetControllers();
            while (pkCtlr)
            {
                NiInterpController* pkInterpCtlr = NiDynamicCast(
                    NiInterpController, pkCtlr);
                if (pkInterpCtlr)
                {
                    NiSprintf(acTemp, NI_MAX_PATH, "%s;",
                        pkInterpCtlr->GetRTTI()->GetName());
                    if (!pcExcludedControllerTypes ||
                        !strstr(pcExcludedControllerTypes, acTemp))
                    {
                        pkInterpCtlr->GuaranteeTimeRange(fStartTime, fEndTime);
                    }
                }
                pkCtlr = pkCtlr->GetNext();
            }
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                GuaranteeTimeRange(pkChild, fStartTime, fEndTime,
                    pcExcludedControllerTypes);
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiInterpController::CopyMembers(NiInterpController* pkDest,
    NiCloningProcess& kCloning)
{
    NiTimeController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiInterpController::LoadBinary(NiStream& kStream)
{
    NiTimeController::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiInterpController::LinkObject(NiStream& kStream)
{
    NiTimeController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiInterpController::RegisterStreamables(NiStream& kStream)
{
    return NiTimeController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiInterpController::SaveBinary(NiStream& kStream)
{
    NiTimeController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiInterpController::IsEqual(NiObject* pkObject)
{
    return NiTimeController::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiInterpController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiTimeController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiInterpController::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("IsManagerControlled",
        GetManagerControlled()));
}
//---------------------------------------------------------------------------
