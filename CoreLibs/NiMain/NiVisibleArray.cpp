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

#include "NiMainPCH.h"
#include "NiRenderObject.h"
#include "NiVisibleArray.h"

//---------------------------------------------------------------------------
void NiVisibleArray::SetAllocatedSize(unsigned int uiSize)
{
    if (uiSize == m_uiAllocatedSize)
        return;

    // Need to truncate the current size if the allocated size is smaller
    // Since we are not using smart pointers, there is no need to clear the
    // array when we do this
    if (uiSize < m_uiCurrentSize)
        m_uiCurrentSize = uiSize;

    NiRenderObject** ppkSaveArray = m_ppkArray;
    m_uiAllocatedSize = uiSize;
    if (m_uiAllocatedSize > 0)
    {
        // allocate a new array
        m_ppkArray = NiAlloc(NiRenderObject*, m_uiAllocatedSize);
        NIASSERT(m_ppkArray);

        // Copy the existing array to the new space.
        // No refcounts to deal with, so memcpy will work fine
        if (m_uiCurrentSize)
        {
            NiMemcpy(m_ppkArray, ppkSaveArray, 
                m_uiCurrentSize * sizeof(NiRenderObject*));
        }
    }
    else
    {
        m_ppkArray = 0;
    }

    // delete old array
    NiFree(ppkSaveArray);
}
//---------------------------------------------------------------------------

