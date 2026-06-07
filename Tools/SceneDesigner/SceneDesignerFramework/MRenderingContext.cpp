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
#include "SceneDesignerFrameworkPCH.h"

#include "MRenderingContext.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MRenderingContext::MRenderingContext(
    NiEntityRenderingContext* pkRenderingContext) :
    m_pkRenderingContext(pkRenderingContext)
{
    MInitRefObject(m_pkRenderingContext);
}
//---------------------------------------------------------------------------
void MRenderingContext::Do_Dispose(bool)
{
    MDisposeRefObject(m_pkRenderingContext);
}
//---------------------------------------------------------------------------
NiEntityRenderingContext* MRenderingContext::GetRenderingContext()
{
    return m_pkRenderingContext;
}
//---------------------------------------------------------------------------
