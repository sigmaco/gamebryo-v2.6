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

#include "NiObjectNET.h"
#include "NiCloningProcess.h"
#include "NiVertWeightsExtraData.h"
#include <NiSystem.h>
#include <stddef.h>

NiImplementRTTI(NiObjectNET, NiObject);

NiObjectNET::EqualityType NiObjectNET::ms_eEqualityType = 
    NiObjectNET::EQUAL_EXACT;
NiObjectNET::CopyType NiObjectNET::ms_eCopyType = NiObjectNET::COPY_NONE;
char NiObjectNET::ms_cAppendCharacter = '$';

//---------------------------------------------------------------------------
NiObjectNET::NiObjectNET() :
    m_ppkExtra(NULL),
    m_usExtraDataSize(0),
    m_usMaxSize(0)
{
}
//---------------------------------------------------------------------------
NiObjectNET::~NiObjectNET()
{
    RemoveAllExtraData();
}

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiObjectNET::CopyMembers(NiObjectNET* pkDest, NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    // Copy object names.
    if (kCloning.m_eCopyType == COPY_EXACT)
    {
        pkDest->SetName(GetName());
    }
    else if (kCloning.m_eCopyType == COPY_UNIQUE)
    {
        if (GetName())
        {
            size_t stQuantity = strlen(GetName()) + 2;
            char* pcNamePlus = NiAlloc(char, stQuantity);
            NIASSERT(pcNamePlus);
            NiStrcpy(pcNamePlus, stQuantity, GetName());
            pcNamePlus[stQuantity - 2] = kCloning.m_cAppendChar;
            pcNamePlus[stQuantity - 1] = 0;
            pkDest->SetName(pcNamePlus);
            NiFree(pcNamePlus);
        }
    }

    // Clone extra data.
    for (unsigned short i = 0; i < m_usExtraDataSize; i++)
    {
        NiExtraData* pkExtra = GetExtraDataAt(i);

        if (pkExtra->IsCloneable())
        {   
            NiExtraData* pkNewExtra = (NiExtraData*)(pkExtra->CreateClone(
                kCloning));
            pkDest->AddExtraData(pkNewExtra);
        }
    }

    // Clone time controllers.
    if (m_spControllers)
    {
        // The TimeController's CreateClone call will clone the entire list.
        pkDest->SetControllers(
            (NiTimeController*) m_spControllers->CreateClone(kCloning));
    }
}
//---------------------------------------------------------------------------
void NiObjectNET::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);

    // Call process clone on the extra data.
    for (unsigned short i = 0; i < m_usExtraDataSize; i++)
    {
        NiExtraData* pkExtra = GetExtraDataAt(i);
        pkExtra->ProcessClone(kCloning);
    }

    // Call process clone on the time controllers.
    if (m_spControllers)
    {
        m_spControllers->ProcessClone(kCloning);
    }
}

//---------------------------------------------------------------------------
// Names
//---------------------------------------------------------------------------

void NiObjectNET::SetName(const NiFixedString& strName)
{
    m_kName = strName;
}

//---------------------------------------------------------------------------
// Extra Data - INTERNAL use.
//---------------------------------------------------------------------------
void NiObjectNET::DeleteExtraData(unsigned short usExtraDataIndex)
{
    if (usExtraDataIndex >= m_usExtraDataSize)  // Sanity check.
    {
        return;
    }

    if (m_ppkExtra[usExtraDataIndex])
    {
        m_ppkExtra[usExtraDataIndex]->DecRefCount();
    }

    // Shuffle downward.
    for (unsigned short i = usExtraDataIndex; 
        i < (m_usExtraDataSize - 1); i++)
    {
        m_ppkExtra[i] = m_ppkExtra[i + 1];
    }

    m_usExtraDataSize--;
    m_ppkExtra[m_usExtraDataSize] = NULL;
}
//---------------------------------------------------------------------------

// Bias toward fast retrieval time (binary search) at the expense of insertion
//    time performance.

bool NiObjectNET::InsertExtraData(NiExtraData* pkExtra)
{
    if (!pkExtra)
    {
        return false;
    }

    pkExtra->IncRefCount();

    if (m_usMaxSize < 1)
    {
        m_usExtraDataSize = m_usMaxSize = 1;
        m_ppkExtra = NiAlloc(NiExtraData*, m_usMaxSize);
        m_ppkExtra[0] = pkExtra;
        return true;
    }
    else if (m_usExtraDataSize == m_usMaxSize)
    {
        // Array is full.  Increase size.
        m_usMaxSize = (m_usMaxSize * 2) + 1; // Balance memory fragmentation
                                             //    with storage efficiency.
        NiExtraData** ppkNewExtra = NiAlloc(NiExtraData*, m_usMaxSize);

        unsigned int uiDestSize = m_usMaxSize * sizeof(NiExtraData*);
        NiMemcpy(ppkNewExtra, uiDestSize, m_ppkExtra,
            m_usExtraDataSize * sizeof(NiExtraData*));

        NiFree(m_ppkExtra);
        m_ppkExtra = ppkNewExtra;
    }

    unsigned short i;

    m_ppkExtra[m_usExtraDataSize] = pkExtra;    // Insert new entry.
    m_usExtraDataSize++;

    for (i = m_usExtraDataSize; i < m_usMaxSize; i++)
    {
        m_ppkExtra[i] = NULL;
    }

    for (i = (m_usExtraDataSize - 1); i > 0; i--)   // Sort.
    {
        ptrdiff_t dtCompare = m_ppkExtra[i - 1]->GetName() -
            m_ppkExtra[i]->GetName();

        if (dtCompare == 0)  // Equal keys are a problem.  Reset.
        {
#ifndef __SPU__
            NiOutputDebugString(
                "Error:  unsuccessful attempt to insert ExtraData with ");
            NiOutputDebugString(
                "same key (name) for same object.\n");
#endif
            DeleteExtraData(i);

            return false;
        }
        else if (dtCompare > 0)  // Previous key is greater.  Swap.
        {
            NiExtraData* pkTmpED = m_ppkExtra[i - 1];
            m_ppkExtra[i - 1] = m_ppkExtra[i];
            m_ppkExtra[i] = pkTmpED;
        }
        else    // Array is sorted.  Discontinue processing.
        {
            break;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
// Extra Data.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Pre-allocate ExtraData array to a particular size when the size is known.
bool NiObjectNET::SetExtraDataSize(unsigned short usSize)
{
    if (usSize == 0)
    {
        return false;
    }

    NIASSERT(usSize < 1000);

    if (m_usMaxSize == 0)
    {
        m_usMaxSize = usSize;
        m_ppkExtra = NiAlloc(NiExtraData*, m_usMaxSize);

        m_usExtraDataSize = 0;  // Force current count to zero.
    }
    else
    {
        m_usMaxSize = usSize;

        NiExtraData** ppkNewExtra = NiAlloc(NiExtraData*, m_usMaxSize);

        unsigned int uiDestSize = m_usMaxSize * sizeof(NiExtraData*);
        NiMemcpy(ppkNewExtra, uiDestSize, m_ppkExtra, m_usExtraDataSize *
            sizeof(NiExtraData*));

        NiFree(m_ppkExtra);
        m_ppkExtra = ppkNewExtra;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiObjectNET::AddExtraData(const NiFixedString& kKey,
    NiExtraData* pkExtra)
{
    if (!kKey.Exists())
    {
#ifndef __SPU__

        NiOutputDebugString(
            "Error:  Attempt to add ExtraData with null name.");
#endif
        return false;
    }

    if (!pkExtra)
    {
#ifndef __SPU__
        NiOutputDebugString("Error:  Attempt to add null ExtraData.");
#endif
        return false;
    }
    
    if (!pkExtra->GetName().Exists())
    {
        pkExtra->SetName(kKey);
    }
    else if (kKey != pkExtra->GetName())
    {
#ifndef __SPU__
        NiOutputDebugString(
            "Error:  Conflict between ExtraData key and ExtraData name");
#endif
        return false;
    }

    return InsertExtraData(pkExtra);
}
//---------------------------------------------------------------------------
bool NiObjectNET::AddExtraData(NiExtraData* pkExtra)
{
    if (!pkExtra)
    {
#ifndef __SPU__
        NiOutputDebugString("Error:  Attempt to add null ExtraData.");
#endif
        return false;
    }
    
    if (!pkExtra->GetName())
    {
        // Generate a name.  It will be of the form "Ni<type>ED<seq#>", e.g.,
        // NiBooleanED1, NiEnumED9, NiIntegerED17, etc., based on the RTTI
        // name.  The 2-digit suffix is used only to guarantee uniqueness.
        const char* pcRTTI = pkExtra->GetRTTI()->GetName();

        if (pcRTTI && (strlen(pcRTTI) > 0))
        {
            NIASSERT(m_usExtraDataSize < 1000);
            char pcSuffix[6];
            NiSprintf(pcSuffix, 6, "ED%03d", m_usExtraDataSize);

            const char* pcSubstr = strstr(pcRTTI, "ExtraData");
            unsigned int uiStrLength = 0;
            if (pcSubstr > pcRTTI)
                uiStrLength = (unsigned int)(pcSubstr - pcRTTI);

            unsigned int uiLen = uiStrLength + strlen(pcSuffix) + 1;
            char* pcGeneratedKey = NiAlloc(char, uiLen);

            NiStrncpy(pcGeneratedKey, uiLen, pcRTTI, uiStrLength);
            NiStrcat(pcGeneratedKey, uiLen, pcSuffix);

            pkExtra->SetName(pcGeneratedKey);

            NiFree(pcGeneratedKey);
        }
        else
        {
            NIASSERT(0);
        }
    }

    return InsertExtraData(pkExtra);
}
//---------------------------------------------------------------------------

// Bias toward fast retrieval time (binary search) at the expense of insertion
// time performance.  This binary search has an average of:
//    2.4 string comparisons for 7 entries, and
//    3.3 string comparisons for 15 entries.

NiExtraData* NiObjectNET::GetExtraData(const NiFixedString& kKey) const
{
    if (!kKey.Exists())
    {
#ifndef __SPU__
        NiOutputDebugString(
            "Error:  Attempt to retrieve ExtraData with null name.");
#endif
        return NULL;
    }

    NIASSERT(m_usExtraDataSize < SHRT_MAX);

    short sBottom = 0;
    short sTop = (short)m_usExtraDataSize - 1;
    short sMiddle = 0;

    while (sBottom <= sTop)
    {
        sMiddle = (sTop + sBottom) >> 1; // Average to get the middle.

        ptrdiff_t dtCompare = ((const char*)kKey) - 
            ((const char*)m_ppkExtra[sMiddle]->GetName());

        if (dtCompare == 0)  // Equal keys.  Return found extra data.
        {
            return m_ppkExtra[sMiddle];
        }
        else if (dtCompare > 0)  // Search key is > "middle" key.
        {
            sBottom = sMiddle + 1;
        }
        else    // Search key is < "middle" key.
        {
            sTop = sMiddle - 1;
        }
    }

    return NULL;    // Couldn't find Extra Data for the given search key.
}
//---------------------------------------------------------------------------
bool NiObjectNET::RemoveExtraData(const NiFixedString& kKey)
{
    if (m_usExtraDataSize == 0)
        return NULL;

    if (!kKey.Exists())
    {
#ifndef __SPU__
        NiOutputDebugString(
            "Error:  Attempt to remove ExtraData using a null name.");
#endif
        return false;
    }

    NIASSERT(m_usExtraDataSize < SHRT_MAX);

    short sBottom = 0;
    short sTop = (short)m_usExtraDataSize - 1;
    short sMiddle = 0;

    while (sBottom <= sTop)
    {
        sMiddle = (sTop + sBottom) >> 1; // Average to get the middle.

        ptrdiff_t dtCompare = ((const char*)kKey) - 
            ((const char*) m_ppkExtra[sMiddle]->GetName());

        if (dtCompare == 0)  // Equal keys.  Return found extra data.
        {
            DeleteExtraData(sMiddle);

            return true;
        }
        else if (dtCompare > 0)  // Search key is > "middle" key.
        {
            sBottom = sMiddle + 1;
        }
        else    // Search key is < "middle" key.
        {
            sTop = sMiddle - 1;
        }
    }

    return false;    // Couldn't find Extra Data for the given search key.
}
//---------------------------------------------------------------------------
bool NiObjectNET::RemoveExtraDataAt(unsigned short usExtraDataIndex)
{
    if (usExtraDataIndex < m_usExtraDataSize)
    {
        DeleteExtraData(usExtraDataIndex);

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiObjectNET::RemoveAllExtraData()
{
    // Remove from end of list, to preserve validity of index for remaining
    // entries.
    for (short i = (((short) m_usExtraDataSize) - 1); i >= 0; i--)
    {
        DeleteExtraData((unsigned short) i);
    }

    NiFree(m_ppkExtra);
    m_ppkExtra = NULL;
    m_usExtraDataSize = m_usMaxSize = 0;
}

//---------------------------------------------------------------------------
// Time controllers
//---------------------------------------------------------------------------
void NiObjectNET::PrependController(NiTimeController* pkControl)
{
    // Add controller to front of list.
    NIASSERT(pkControl);

    if (pkControl)
    {
        pkControl->SetNext(m_spControllers);
        m_spControllers = pkControl;
    }
}
//---------------------------------------------------------------------------
void NiObjectNET::SetControllers(NiTimeController* pkControl)
{
#ifdef NIDEBUG
    if (m_spControllers)
    {
        if (m_spControllers->GetRefCount() == 1)
        {
            NiOutputDebugString("WARNING. SetControllers destroying list.\n");
        }
    }
#endif
    m_spControllers = pkControl;
}
//---------------------------------------------------------------------------
void NiObjectNET::RemoveController(NiTimeController* pkControl)
{
#ifdef NIDEBUG
    static char s_pcWarning[] =
        "WARNING.  RemoveController is destroying a controller.\n";
#endif

    if (pkControl && m_spControllers)
    {
        if (m_spControllers == pkControl)
        {
            // Control object at beginning of list; strip it off.
            // Prevent early deletion.
            NiTimeControllerPtr spSave = pkControl;
            m_spControllers = pkControl->GetNext();
            pkControl->SetNext(NULL);

#ifdef NIDEBUG
            // At this point if the reference count on pControl is 1, when
            // this function returns, spSave will destruct and decrement the
            // reference count on pControl to 0, causing it to be deleted.
            if (pkControl->GetRefCount() == 1)
                NiOutputDebugString(s_pcWarning);
#endif
        }
        else
        {
            // Search for control object.
            NiTimeController* pkPrev = m_spControllers;
            NiTimeController* pkCurr = pkPrev->GetNext();
            while (pkCurr && pkCurr != pkControl)
            {
                pkPrev = pkCurr;
                pkCurr = pkCurr->GetNext();
            }
            if (pkCurr)
            {
                // Found the control; remove it.
                NiTimeControllerPtr spSave = pkControl;
                pkPrev->SetNext(pkControl->GetNext());
                pkControl->SetNext(NULL);

#ifdef NIDEBUG
                // At this point if the reference count on pControl is 1,
                // when this function returns, spSave will destruct and
                // decrement the reference count on pControl to 0, causing
                // it to be deleted.
                if (pkControl->GetRefCount() == 1)
                    NiOutputDebugString(s_pcWarning);
#endif
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiObjectNET::RemoveAllControllers()
{
    m_spControllers = NULL;
}
//---------------------------------------------------------------------------
NiTimeController* NiObjectNET::GetController(const NiRTTI* pkRTTI) const
{
    NiTimeController* pkCtrl;
    
    for (pkCtrl = m_spControllers; pkCtrl != NULL; pkCtrl = pkCtrl->GetNext())
    {
        if (pkCtrl->IsKindOf(pkRTTI))
        {
            return pkCtrl;
        }
    }

    return NULL;
}
#ifndef __SPU__
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiObjectNET::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        kStream.LoadCStringAsFixedString(m_kName);
    }
    else
    {
        kStream.LoadFixedString(m_kName);
    }
    
    // Read ExtraData pointers.
    kStream.ReadMultipleLinkIDs();  // m_ppkExtra

    kStream.ReadLinkID();   // m_spControllers

}
//---------------------------------------------------------------------------
void NiObjectNET::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
    
    // Link ExtraData pointers.
    unsigned int uiSize = kStream.GetNumberOfLinkIDs();
    // Preallocate exact size needed.
    SetExtraDataSize((unsigned short)uiSize);

    for (unsigned int i = 0; i < uiSize; i++)
    {
        NiExtraData* pkNewExtra = 
            (NiExtraData*) (kStream.GetObjectFromLinkID());
        AddExtraData(pkNewExtra);
    }

    m_spControllers = (NiTimeController*) kStream.GetObjectFromLinkID();

}
//---------------------------------------------------------------------------
bool NiObjectNET::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
    {
        return false;
    }

    if (!kStream.RegisterFixedString(m_kName))
    {
        return false;
    }

    // Register ExtraData.
    for (unsigned short i = 0; i < m_usExtraDataSize; i++)
    {
        NiExtraData* pkExtra = m_ppkExtra[i];
        if (pkExtra && pkExtra->IsStreamable())
        {
            pkExtra->RegisterStreamables(kStream);
        }
    }

    if (m_spControllers)
    {
        m_spControllers->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiObjectNET::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    kStream.SaveFixedString(m_kName);

    // Save ExtraData. The extra data size streamed may not be the same as
    // what exists in memory if some of the extra data is not streamable.

    // Must remain an unsigned int for ReadMultipleLinkIDs to work
    unsigned int uiExtraDataSize = 0;
    for (unsigned short i = 0; i < m_usExtraDataSize; i++)
    {
        NiExtraData* pkExtra = m_ppkExtra[i];
        if (pkExtra->IsStreamable())
            ++uiExtraDataSize;
    }
    NiStreamSaveBinary(kStream, uiExtraDataSize);

    for (unsigned short j = 0; j < m_usExtraDataSize; j++)
    {
        NiExtraData* pkExtra = m_ppkExtra[j];
        if (pkExtra->IsStreamable())
            kStream.SaveLinkID(pkExtra);
    }

    // Ensure we save LinkID only for streamable NiTimeControllers
    NiTimeController* pkController = m_spControllers;
    while (pkController && !pkController->IsStreamable())
        pkController = pkController->GetNext();
    kStream.SaveLinkID(pkController);
}
//---------------------------------------------------------------------------
void NiObjectNET::PostLinkObject(NiStream& kStream)
{
    NiObject::PostLinkObject(kStream);
    
    // Remove instances of deprecated NiVertWeightsExtraData class since they
    // are no longer supported.  The NiVertWeightsExtraData class persists
    // merely for successful streaming of legacy Nif files.
    for (int i = 0; (unsigned int)i < GetExtraDataSize(); i++)
    {
        NiExtraData* pkExtraData = GetExtraDataAt((unsigned short)i);
        if (NiIsExactKindOf(NiVertWeightsExtraData, pkExtraData))
        {
            RemoveExtraDataAt((unsigned short)i);
            i = -1;  //Reset to -1 since the loop increment will make it zero.
        }
    }
}
//---------------------------------------------------------------------------
bool NiObjectNET::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiObjectNET* pkNET = (NiObjectNET*) pkObject;

    // Test for name equality.

    if (ms_eEqualityType == EQUAL_EXACT)
    {
        if (m_kName != pkNET->m_kName)
            return false;
    }

    // Test for extra data equality, but ignore non-streamable data such as
    // shader constant map extra data.

    unsigned short i = 0;
    NiTPrimitiveArray<unsigned short> kSelfEntries;
    NiTPrimitiveArray<unsigned short> kOtherEntries;
    for (i=0; i < m_usExtraDataSize; i++)
    {
        if (GetExtraDataAt(i)->IsStreamable())
            kSelfEntries.Add(i);
    }
    for (i=0; i < pkNET->GetExtraDataSize(); i++)
    {
        if (pkNET->GetExtraDataAt(i)->IsStreamable())
            kOtherEntries.Add(i);
    }

    if (kSelfEntries.GetSize() != kOtherEntries.GetSize())
        return false;
    for (i = 0; i < kSelfEntries.GetSize(); i++)
    {
        if (!GetExtraDataAt(kSelfEntries.GetAt(i))->IsEqual(
            pkNET->GetExtraDataAt(kOtherEntries.GetAt(i))))
        {
            return false;
        }
    }

    // Test for controller equality.

    NiTimeController* pkControl0 = m_spControllers;
    NiTimeController* pkControl1 = pkNET->m_spControllers;
    
    unsigned int uiControlCount0 = pkControl0 ? pkControl0->ItemsInList() : 0;
    unsigned int uiControlCount1 = pkControl1 ? pkControl1->ItemsInList() : 0;
    if (uiControlCount0 != uiControlCount1)
        return false;
    
    while (pkControl0 && pkControl1)
    {
        if (!pkControl0->IsEqual(pkControl1))
            return false;
        
        pkControl0 = pkControl0->GetNext();
        pkControl1 = pkControl1->GetNext();
    }

    return true;
}
//---------------------------------------------------------------------------
void NiObjectNET::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiObjectNET::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_kName", (const char*) m_kName));

    for (unsigned short i = 0; i < m_usExtraDataSize; i++)
    {
        GetExtraDataAt(i)->GetViewerStrings(pkStrings);
    }

    if (m_spControllers)
    {
        NiTimeController* pkControl = m_spControllers;
        for (/**/; pkControl; pkControl = pkControl->GetNext())
            pkControl->GetViewerStrings(pkStrings);
    }
}
//---------------------------------------------------------------------------
#endif
