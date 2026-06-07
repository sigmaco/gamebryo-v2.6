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

#include "NiPropertyState.h"
#include "NiAlphaProperty.h"
#include "NiDitherProperty.h"
#include "NiFogProperty.h"
#include "NiMaterialProperty.h"
#include "NiRendererSpecificProperty.h"
#include "NiShadeProperty.h"
#include "NiSpecularProperty.h"
#include "NiStencilProperty.h"
#include "NiTexturingProperty.h"
#include "NiVertexColorProperty.h"
#include "NiWireframeProperty.h"
#include "NiZBufferProperty.h"

//---------------------------------------------------------------------------
NiPropertyState::NiPropertyState()
{
    Reset();
}
//---------------------------------------------------------------------------
NiPropertyState::NiPropertyState(const NiPropertyState& kState)
: NiRefObject()
{
    unsigned int i;
    for (i = 0; i < NiProperty::MAX_TYPES; i++)
        m_aspProps[i] = kState.m_aspProps[i];
}
//---------------------------------------------------------------------------
NiPropertyState::~NiPropertyState()
{
}
//---------------------------------------------------------------------------
void NiPropertyState::Reset()
{
    m_aspProps[NiAlphaProperty::GetType()] 
        = NiAlphaProperty::GetDefault();
    m_aspProps[NiDitherProperty::GetType()] 
        = NiDitherProperty::GetDefault();
    m_aspProps[NiFogProperty::GetType()] 
        = NiFogProperty::GetDefault();
    m_aspProps[NiMaterialProperty::GetType()] 
        = NiMaterialProperty::GetDefault();
    m_aspProps[NiRendererSpecificProperty::GetType()] 
        = NiRendererSpecificProperty::GetDefault();
    m_aspProps[NiShadeProperty::GetType()] 
        = NiShadeProperty::GetDefault();
    m_aspProps[NiSpecularProperty::GetType()] 
        = NiSpecularProperty::GetDefault();
    m_aspProps[NiStencilProperty::GetType()] 
        = NiStencilProperty::GetDefault();
    m_aspProps[NiTexturingProperty::GetType()] 
        = NiTexturingProperty::GetDefault();
    m_aspProps[NiVertexColorProperty::GetType()] 
        = NiVertexColorProperty::GetDefault();
    m_aspProps[NiWireframeProperty::GetType()] 
        = NiWireframeProperty::GetDefault();
    m_aspProps[NiZBufferProperty::GetType()] 
        = NiZBufferProperty::GetDefault();
}
//---------------------------------------------------------------------------
NiPropertyState& NiPropertyState::operator=(const NiPropertyState& kState)
{
    unsigned int i;
    for (i = 0; i < NiProperty::MAX_TYPES; i++)
        m_aspProps[i] = kState.m_aspProps[i];

    return *this;
}
//---------------------------------------------------------------------------
void NiPropertyState::GetProperties(
    NiProperty* apkProps[NiProperty::MAX_TYPES]) const
{
    unsigned int i;
    for (i = 0; i < NiProperty::MAX_TYPES; i++)
        apkProps[i] = (NiProperty*)m_aspProps[i];
}
//---------------------------------------------------------------------------
