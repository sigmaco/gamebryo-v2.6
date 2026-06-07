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
#include "NiUserInterfacePCH.h"

#include "NiUIImage.h"
NiImplementRTTI(NiUIImage, NiUIBaseElement);

//---------------------------------------------------------------------------
NiUIImage::NiUIImage() :
    NiUIBaseElement(false)
{
}
//---------------------------------------------------------------------------
NiUIImage::NiUIImage(NiRect<float> kTextureCoords) :
    // I have to make sure that the NiMeshScreenElementsPtr is initialized 
    // before I call the SetSize function
    NiUIBaseElement(false)
{
    SetTextureCoords(kTextureCoords);
}
//---------------------------------------------------------------------------
NiUIImage::~NiUIImage()
{
}
//---------------------------------------------------------------------------
void NiUIImage::ReinitializeDisplayElements()
{
    if (m_spScreenElements == NULL)
        return;

    if (m_bVisible)
    {
        bool bSuccess = InitializeQuad(m_spScreenElements, m_iBaseQuadIdx, 
            m_kNSCRect, m_kTextureCoords, NiColorA::WHITE);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUIImage.  "
                "Probable cause: invalid uiBaseIdx passed in to "
                "NiUIImage::ReinitializeDisplayElements.");
        }
    }
    else
    {
        if (m_spScreenElements)
        {
            m_spScreenElements->SetRectangle(m_iBaseQuadIdx, 
                0.0f, 0.0f, 0.0f, 0.0f);
        }
    }
    ReinitializeChildDisplayElements();
}
//---------------------------------------------------------------------------

