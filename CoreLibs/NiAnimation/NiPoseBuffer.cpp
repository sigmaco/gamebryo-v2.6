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

#include "NiPoseBuffer.h"


NiImplementRTTI(NiPoseBuffer,NiObject);

//---------------------------------------------------------------------------
NiPoseBuffer::NiPoseBuffer(NiPoseBinding* pkPoseBinding) : 
    m_spPoseBinding(NULL), m_usNumTotalItems(0), m_usNumValidItems(0),
    m_uiNonAccumStartIndex(0), m_puiFlagWeightArray(NULL), m_pkDataBlock(NULL),
    m_usNumColorItems(0), m_usColorStartIndex(0), m_pkColorItems(NULL),
    m_usNumBoolItems(0), m_usBoolStartIndex(0), m_pkBoolItems(NULL), 
    m_usNumFloatItems(0), m_usFloatStartIndex(0), m_pkFloatItems(NULL),
    m_usNumPoint3Items(0), m_usPoint3StartIndex(0), m_pkPoint3Items(NULL),
    m_usNumRotItems(0), m_usRotStartIndex(0), m_pkRotItems(NULL),
    m_usNumReferencedItems(0), m_usReferencedStartIndex(0), 
    m_pkReferencedItems(NULL)
{
    Init(pkPoseBinding);
}
//---------------------------------------------------------------------------
NiPoseBuffer::~NiPoseBuffer()
{
    Shutdown();
}
//---------------------------------------------------------------------------
bool NiPoseBuffer::IsValid(NiPoseBufferHandle kPBHandle) const
{
    NiPoseBufferChannelType ePBChannelType = kPBHandle.GetChannelType();
    switch(ePBChannelType)
    {
    case PBCOLORCHANNEL:
        return IsColorValid(kPBHandle);
    case PBBOOLCHANNEL:
        return IsBoolValid(kPBHandle);
    case PBFLOATCHANNEL:
        return IsFloatValid(kPBHandle);
    case PBPOINT3CHANNEL:
        return IsPoint3Valid(kPBHandle);
    case PBROTCHANNEL:
        return IsRotValid(kPBHandle);
    case PBREFERENCEDCHANNEL:
        return IsReferencedItemValid(kPBHandle);
    default:
        NIASSERT(false);
    }

    return false;
}
//---------------------------------------------------------------------------
void NiPoseBuffer::SetValid(NiPoseBufferHandle kPBHandle, bool bValid)
{
    NiPoseBufferChannelType ePBChannelType = kPBHandle.GetChannelType();
    switch(ePBChannelType)
    {
    case PBCOLORCHANNEL:
        SetColorValid(kPBHandle, bValid);
        break;
    case PBBOOLCHANNEL:
        SetBoolValid(kPBHandle, bValid);
        break;
    case PBFLOATCHANNEL:
        SetFloatValid(kPBHandle, bValid);
        break;
    case PBPOINT3CHANNEL:
        SetPoint3Valid(kPBHandle, bValid);
        break;
    case PBROTCHANNEL:
        SetRotValid(kPBHandle, bValid);
        break;
    case PBREFERENCEDCHANNEL:
        SetReferencedItemValid(kPBHandle, bValid);
        break;
    default:
        NIASSERT(false);
    }
}
//---------------------------------------------------------------------------
float* NiPoseBuffer::GetDataPointer(NiPoseBufferHandle kPBHandle) const
{
    float* pfDataPointer = NULL;

    unsigned short usIndex = kPBHandle.GetChannelIndex();
    NiPoseBufferChannelType ePBChannelType = kPBHandle.GetChannelType();
    switch(ePBChannelType)
    {
    case PBCOLORCHANNEL:
        if (usIndex < m_usNumColorItems && IsItemValid(
            m_puiFlagWeightArray[m_usColorStartIndex + usIndex]))
        {
            pfDataPointer = (float*)(m_pkColorItems + usIndex);
        }
        break;
    case PBBOOLCHANNEL:
        if (usIndex < m_usNumBoolItems && IsItemValid(
            m_puiFlagWeightArray[m_usBoolStartIndex + usIndex]))
        {
            pfDataPointer = (float*)(m_pkBoolItems + usIndex);
        }
        break;
    case PBFLOATCHANNEL:
        if (usIndex < m_usNumFloatItems && IsItemValid(
            m_puiFlagWeightArray[m_usFloatStartIndex + usIndex]))
        {
            pfDataPointer = (float*)(m_pkFloatItems + usIndex);
        }
        break;
    case PBPOINT3CHANNEL:
        if (usIndex < m_usNumPoint3Items && IsItemValid(
            m_puiFlagWeightArray[m_usPoint3StartIndex + usIndex]))
        {
            pfDataPointer = (float*)(m_pkPoint3Items + usIndex);
        }
        break;
    case PBROTCHANNEL:
        if (usIndex < m_usNumRotItems && IsItemValid(
            m_puiFlagWeightArray[m_usRotStartIndex + usIndex]))
        {
            pfDataPointer = (float*)(m_pkRotItems + usIndex);
        }
        break;
    case PBREFERENCEDCHANNEL:
        if (usIndex < m_usNumReferencedItems && IsItemValid(
            m_puiFlagWeightArray[m_usReferencedStartIndex + usIndex]))
        {
            pfDataPointer = (float*)(m_pkReferencedItems + usIndex);
        }
        break;
    default:
        NIASSERT(false);
    }
    return pfDataPointer;
}
//---------------------------------------------------------------------------
void NiPoseBuffer::ValidateAccumDeltaItemsFromValidAccumItems()
{
    NIASSERT(m_uiNonAccumStartIndex == 2);

    // Index 0 holds the accum transform value.
    // Index 1 contains the accum transform delta.
    // This method marks the accum delta components (index 1) as valid 
    // whenever the corresponding accum component (index 0) is valid.

    // Validate scale delta.
    if (m_usNumFloatItems > 1 && 
        IsItemValid(m_puiFlagWeightArray[m_usFloatStartIndex]))
    {
        SetFloatValid(NiPoseBufferHandle(PBFLOATCHANNEL, 1), true);
    }

    // Validate pos delta.
    if (m_usNumPoint3Items > 1 && 
        IsItemValid(m_puiFlagWeightArray[m_usPoint3StartIndex]))
    {
        SetPoint3Valid(NiPoseBufferHandle(PBPOINT3CHANNEL, 1), true);
    }

    // Validate rot delta.
    if (m_usNumRotItems > 1 && 
        IsItemValid(m_puiFlagWeightArray[m_usRotStartIndex]))
    {
        SetRotValid(NiPoseBufferHandle(PBROTCHANNEL, 1), true);
    }
}
//---------------------------------------------------------------------------
bool NiPoseBuffer::AddNewItemsFromPoseBinding(bool bCopyFloatWeightArray,
    bool bCopyDataItems)
{
    // This function updates the pose buffer to match the current bindings 
    // in the associated pose binding. If the associated pose binding 
    // contains new mappings, the data blocks and flag weight array within 
    // the pose buffer will be expanded (which invalidates any previously 
    // cached pointers to the contained data). 
    //
    // Existing data within the flag weight array is copied to the new array 
    // when bCopyFloatWeightArray is true. 
    //
    // Similarly, data stored in the old data blocks is copied to the new 
    // blocks when bCopyDataItems is true.

    NIASSERT(m_spPoseBinding);

    NiPoseBinding* pkPoseBinding = m_spPoseBinding;
    unsigned short usNumTotalItems = 
        (unsigned short)pkPoseBinding->GetNumTotalBindings();
    if (m_usNumTotalItems >= usNumTotalItems)
    {
        NIASSERT(m_usNumTotalItems == usNumTotalItems);
        return false;   // No items were added.
    }

    unsigned short usNumColorItems = 
        (unsigned short)pkPoseBinding->GetNumColorBindings();
    unsigned short usNumBoolItems = 
        (unsigned short)pkPoseBinding->GetNumBoolBindings();
    unsigned short usNumFloatItems = 
        (unsigned short)pkPoseBinding->GetNumFloatBindings();
    unsigned short usNumPoint3Items = 
        (unsigned short)pkPoseBinding->GetNumPoint3Bindings();
    unsigned short usNumRotItems = 
        (unsigned short)pkPoseBinding->GetNumRotBindings();
    unsigned short usNumReferencedItems =
        (unsigned short)pkPoseBinding->GetNumReferencedBindings();

    NIASSERT(usNumTotalItems == usNumColorItems + usNumBoolItems + 
        usNumFloatItems + usNumPoint3Items + usNumRotItems + 
        usNumReferencedItems);
    NIASSERT(usNumColorItems >= m_usNumColorItems);
    NIASSERT(usNumBoolItems >= m_usNumBoolItems);
    NIASSERT(usNumFloatItems >= m_usNumFloatItems);
    NIASSERT(usNumPoint3Items >= m_usNumPoint3Items);
    NIASSERT(usNumRotItems >= m_usNumRotItems);
    NIASSERT(usNumReferencedItems >= m_usNumReferencedItems);

    // Create new flag weight array.
    unsigned int *puiNewFlagWeightArray = NiAlloc(
        unsigned int, usNumTotalItems);

    // Invalidate all items in new flag weight array.
    memset(puiNewFlagWeightArray, 0, 
        usNumTotalItems * sizeof(unsigned int));

    // Copy existing flag weight array.
    if (bCopyFloatWeightArray)
    {
        unsigned int *puiNewFlagWeight = puiNewFlagWeightArray;
        for (unsigned int ui = 0; ui < m_usNumTotalItems; ui++)
        {
            if (ui == m_usBoolStartIndex)
            {
                puiNewFlagWeight += (usNumColorItems - m_usNumColorItems);
            }
            if (ui == m_usFloatStartIndex)
            {
                puiNewFlagWeight += (usNumBoolItems - m_usNumBoolItems);
            }
            if (ui == m_usPoint3StartIndex)
            {
                puiNewFlagWeight += (usNumFloatItems - m_usNumFloatItems);
            }
            if (ui == m_usRotStartIndex)
            {
                puiNewFlagWeight += (usNumPoint3Items - m_usNumPoint3Items);
            }
            if (ui == m_usReferencedStartIndex)
            {
                puiNewFlagWeight += (usNumRotItems - m_usNumRotItems);
            }
            *puiNewFlagWeight = m_puiFlagWeightArray[ui];
            puiNewFlagWeight++;
        }
    }
    else
    {
        m_usNumValidItems = 0;
    }

    // Delete existing flag weight array.
    NiFree(m_puiFlagWeightArray);

    // Use new flag weight array.
    m_puiFlagWeightArray = puiNewFlagWeightArray;

    // Create new data block.
    unsigned int uiSize = usNumColorItems * sizeof(ColorItem) + 
        usNumBoolItems * sizeof(BoolItem) + 
        usNumFloatItems * sizeof(FloatItem) + 
        usNumPoint3Items * sizeof(Point3Item) + 
        usNumRotItems * sizeof(RotItem) +
        usNumReferencedItems * sizeof(ReferencedItem);
    NIASSERT(uiSize % 4 == 0);
    float* pkNewDataBlock = NiAlloc(float, uiSize / 4);

    // Copy existing data items.
    if (bCopyDataItems)
    {
        float* pkOldData = m_pkDataBlock;
        float* pkNewData = pkNewDataBlock;
        unsigned int uiItemSize = sizeof(ColorItem) / 4;
        for (unsigned int ui = 0; ui < m_usNumTotalItems; ui++)
        {
            if (ui == m_usBoolStartIndex)
            {
                pkNewData += (usNumColorItems - m_usNumColorItems) * 
                    uiItemSize;
                uiItemSize = sizeof(BoolItem) / 4;
            }
            if (ui == m_usFloatStartIndex)
            {
                pkNewData += (usNumBoolItems - m_usNumBoolItems) * 
                    uiItemSize;
                uiItemSize = sizeof(FloatItem) / 4;
            }
            if (ui == m_usPoint3StartIndex)
            {
                pkNewData += (usNumFloatItems - m_usNumFloatItems) * 
                    uiItemSize;
                uiItemSize = sizeof(Point3Item) / 4;
            }
            if (ui == m_usRotStartIndex)
            {
                pkNewData += (usNumPoint3Items - m_usNumPoint3Items) * 
                    uiItemSize;
                uiItemSize = sizeof(RotItem) / 4;
            }
            if (ui == m_usReferencedStartIndex)
            {
                pkNewData += (usNumRotItems - m_usNumRotItems) * 
                    uiItemSize;
                uiItemSize = sizeof(ReferencedItem) / 4;
            }
            for (unsigned int uj = 0; uj < uiItemSize; uj++)
            {
                *pkNewData = *pkOldData;
                pkOldData++;
                pkNewData++;
            }
        }
    }

    // Delete existing data block.
    NiFree(m_pkDataBlock);

    // Use new data block.
    m_pkDataBlock = pkNewDataBlock;

    m_usNumTotalItems = usNumTotalItems;
    m_usNumColorItems = usNumColorItems;
    m_usNumBoolItems = usNumBoolItems;
    m_usNumFloatItems = usNumFloatItems;
    m_usNumPoint3Items = usNumPoint3Items;
    m_usNumRotItems = usNumRotItems;
    m_usNumReferencedItems = usNumReferencedItems;
    NIASSERT(m_usColorStartIndex == 0);
    m_usBoolStartIndex = m_usColorStartIndex + m_usNumColorItems;
    m_usFloatStartIndex = m_usBoolStartIndex + m_usNumBoolItems;
    m_usPoint3StartIndex = m_usFloatStartIndex + m_usNumFloatItems;
    m_usRotStartIndex = m_usPoint3StartIndex + m_usNumPoint3Items;
    m_usReferencedStartIndex = m_usRotStartIndex + m_usNumRotItems;
    NIASSERT(m_usNumTotalItems == m_usReferencedStartIndex + 
        m_usNumReferencedItems);

    m_pkColorItems = (ColorItem*)(m_pkDataBlock);
    m_pkBoolItems = (BoolItem*)(&m_pkColorItems[m_usNumColorItems]);
    m_pkFloatItems = (FloatItem*)(&m_pkBoolItems[m_usNumBoolItems]);
    m_pkPoint3Items = (Point3Item*)(&m_pkFloatItems[m_usNumFloatItems]);
    m_pkRotItems = (RotItem*)(&m_pkPoint3Items[m_usNumPoint3Items]);
    m_pkReferencedItems = (ReferencedItem*)(&m_pkRotItems[m_usNumRotItems]);
    NIASSERT(&m_pkDataBlock[uiSize / 4] == 
        (float*)(&m_pkReferencedItems[m_usNumReferencedItems]));

    return true;    // Items were added.
}
//---------------------------------------------------------------------------
void NiPoseBuffer::Init(NiPoseBinding* pkPoseBinding)
{
    NIASSERT(pkPoseBinding);
    NIASSERT(!m_spPoseBinding);
    NIASSERT(m_usNumTotalItems == 0);
    NIASSERT(m_usNumValidItems == 0);
    NIASSERT(!m_pkDataBlock);
    NIASSERT(!m_puiFlagWeightArray);

    m_spPoseBinding = pkPoseBinding;
    m_uiNonAccumStartIndex = pkPoseBinding->GetNonAccumStartIndex();
    m_usNumTotalItems = (unsigned short)pkPoseBinding->GetNumTotalBindings();
    m_usNumColorItems = (unsigned short)pkPoseBinding->GetNumColorBindings();
    m_usNumBoolItems = (unsigned short)pkPoseBinding->GetNumBoolBindings();
    m_usNumFloatItems = (unsigned short)pkPoseBinding->GetNumFloatBindings();
    m_usNumPoint3Items = (unsigned short)pkPoseBinding->GetNumPoint3Bindings();
    m_usNumRotItems = (unsigned short)pkPoseBinding->GetNumRotBindings();
    m_usNumReferencedItems = 
        (unsigned short)pkPoseBinding->GetNumReferencedBindings();

    NIASSERT(m_usNumTotalItems == m_usNumColorItems + m_usNumBoolItems + 
        m_usNumFloatItems + m_usNumPoint3Items + m_usNumRotItems + 
        m_usNumReferencedItems);

    if (m_usNumTotalItems > 0)
    {
        m_puiFlagWeightArray = NiAlloc(unsigned int, m_usNumTotalItems);
        unsigned int uiSize = m_usNumColorItems * sizeof(ColorItem) + 
            m_usNumBoolItems * sizeof(BoolItem) + 
            m_usNumFloatItems * sizeof(FloatItem) + 
            m_usNumPoint3Items * sizeof(Point3Item) + 
            m_usNumRotItems * sizeof(RotItem) +
            m_usNumReferencedItems * sizeof(ReferencedItem);
        NIASSERT(uiSize % 4 == 0);
        m_pkDataBlock = NiAlloc(float, uiSize / 4);

        NIASSERT(m_usColorStartIndex == 0);
        m_usBoolStartIndex = m_usColorStartIndex + m_usNumColorItems;
        m_usFloatStartIndex = m_usBoolStartIndex + m_usNumBoolItems;
        m_usPoint3StartIndex = m_usFloatStartIndex + m_usNumFloatItems;
        m_usRotStartIndex = m_usPoint3StartIndex + m_usNumPoint3Items;
        m_usReferencedStartIndex = m_usRotStartIndex + m_usNumRotItems;

        m_pkColorItems = (ColorItem*)(m_pkDataBlock);
        m_pkBoolItems = (BoolItem*)(&m_pkColorItems[m_usNumColorItems]);
        m_pkFloatItems = (FloatItem*)(&m_pkBoolItems[m_usNumBoolItems]);
        m_pkPoint3Items = (Point3Item*)(&m_pkFloatItems[m_usNumFloatItems]);
        m_pkRotItems = (RotItem*)(&m_pkPoint3Items[m_usNumPoint3Items]);
        m_pkReferencedItems =
            (ReferencedItem*)(&m_pkRotItems[m_usNumRotItems]);
        NIASSERT(&m_pkDataBlock[uiSize / 4] == 
            (float*)(&m_pkReferencedItems[m_usNumReferencedItems]));

        InvalidateAllItems();
    }
}
//---------------------------------------------------------------------------
void NiPoseBuffer::Shutdown()
{
    NiFree(m_puiFlagWeightArray);
    m_puiFlagWeightArray = NULL;

    NiFree(m_pkDataBlock);
    m_pkDataBlock = NULL;

    m_spPoseBinding = NULL;
    m_usNumTotalItems = 0;
    m_usNumValidItems = 0;

    m_usNumColorItems = 0;
    m_usColorStartIndex = 0;
    m_pkColorItems = NULL;

    m_usNumBoolItems = 0;
    m_usBoolStartIndex = 0;
    m_pkBoolItems = NULL;

    m_usNumFloatItems = 0;
    m_usFloatStartIndex = 0;
    m_pkFloatItems = NULL;

    m_usNumPoint3Items = 0;
    m_usPoint3StartIndex = 0;
    m_pkPoint3Items = NULL;

    m_usNumRotItems = 0;
    m_usRotStartIndex = 0;
    m_pkRotItems = NULL;

    m_usNumReferencedItems = 0;
    m_usReferencedStartIndex = 0;
    m_pkReferencedItems = NULL;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPoseBuffer);
//---------------------------------------------------------------------------
void NiPoseBuffer::CopyMembers(NiPoseBuffer* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    // Share the pose binding.
    pkDest->m_spPoseBinding = m_spPoseBinding;

    pkDest->m_usNumTotalItems = m_usNumTotalItems;
    pkDest->m_usNumValidItems = m_usNumValidItems;
    pkDest->m_uiNonAccumStartIndex = m_uiNonAccumStartIndex;

    pkDest->m_usNumColorItems = m_usNumColorItems;
    pkDest->m_usNumBoolItems = m_usNumBoolItems;
    pkDest->m_usNumFloatItems = m_usNumFloatItems;
    pkDest->m_usNumPoint3Items = m_usNumPoint3Items;
    pkDest->m_usNumRotItems = m_usNumRotItems;
    pkDest->m_usNumReferencedItems = m_usNumReferencedItems;

    NIASSERT(m_usColorStartIndex == 0);
    pkDest->m_usColorStartIndex = m_usColorStartIndex;
    pkDest->m_usBoolStartIndex = m_usColorStartIndex + m_usNumColorItems;
    pkDest->m_usFloatStartIndex = m_usBoolStartIndex + m_usNumBoolItems;
    pkDest->m_usPoint3StartIndex = m_usFloatStartIndex + m_usNumFloatItems;
    pkDest->m_usRotStartIndex = m_usPoint3StartIndex + m_usNumPoint3Items;
    pkDest->m_usReferencedStartIndex = m_usRotStartIndex + m_usNumRotItems;
    NIASSERT(pkDest->m_usNumTotalItems == pkDest->m_usReferencedStartIndex + 
        pkDest->m_usNumReferencedItems);

    if (m_usNumTotalItems > 0)
    {
        // Create new data block.
        unsigned int uiSize = m_usNumColorItems * sizeof(ColorItem) + 
            m_usNumBoolItems * sizeof(BoolItem) + 
            m_usNumFloatItems * sizeof(FloatItem) + 
            m_usNumPoint3Items * sizeof(Point3Item) + 
            m_usNumRotItems * sizeof(RotItem) +
            m_usNumReferencedItems * sizeof(ReferencedItem);
        NIASSERT(uiSize % 4 == 0);
        uiSize /= 4;
        pkDest->m_pkDataBlock = NiAlloc(float, uiSize);

        // Copy existing data items.
        for (unsigned int ui = 0; ui < uiSize; ui++)
        {
            pkDest->m_pkDataBlock[ui] = m_pkDataBlock[ui];
        }

        pkDest->m_pkColorItems = (ColorItem*)(pkDest->m_pkDataBlock);
        pkDest->m_pkBoolItems = 
            (BoolItem*)(&pkDest->m_pkColorItems[m_usNumColorItems]);
        pkDest->m_pkFloatItems = 
            (FloatItem*)(&pkDest->m_pkBoolItems[m_usNumBoolItems]);
        pkDest->m_pkPoint3Items = 
            (Point3Item*)(&pkDest->m_pkFloatItems[m_usNumFloatItems]);
        pkDest->m_pkRotItems = 
            (RotItem*)(&pkDest->m_pkPoint3Items[m_usNumPoint3Items]);
        pkDest->m_pkReferencedItems = 
            (ReferencedItem*)(&pkDest->m_pkRotItems[m_usNumRotItems]);
        NIASSERT(&pkDest->m_pkDataBlock[uiSize] == 
            (float*)(&pkDest->m_pkReferencedItems[m_usNumReferencedItems]));

        // Create new flag weight array.
        pkDest->m_puiFlagWeightArray = NiAlloc(unsigned int, 
            m_usNumTotalItems);

        // Copy existing flag weight array.
        for (unsigned int ui = 0; ui < m_usNumTotalItems; ui++)
        {
            pkDest->m_puiFlagWeightArray[ui] = m_puiFlagWeightArray[ui];
        }
    }
}
//---------------------------------------------------------------------------
void NiPoseBuffer::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);
}
//---------------------------------------------------------------------------
