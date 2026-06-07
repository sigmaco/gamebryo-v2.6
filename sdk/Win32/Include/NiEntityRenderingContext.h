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

#ifndef NIENTITYRENDERINGCONTEXT_H
#define NIENTITYRENDERINGCONTEXT_H

#include "NiEntityLibType.h"
#include <NiRefObject.h>
#include <NiSmartPointer.h>

class NiRenderer;
class NiCamera;
class NiCullingProcess;

class NIENTITY_ENTRY NiEntityRenderingContext : public NiRefObject
{
public:
    NiEntityRenderingContext();

    NiRenderer* m_pkRenderer;
    NiCamera* m_pkCamera;
    NiCullingProcess* m_pkCullingProcess;
};

NiSmartPointer(NiEntityRenderingContext);

#include "NiEntityRenderingContext.inl"

#endif // NIENTITYRENDERINGCONTEXT_H
