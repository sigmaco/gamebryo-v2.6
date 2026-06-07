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

#ifndef NIDX9RENDEREROPTIONSVIEW_H
#define NIDX9RENDEREROPTIONSVIEW_H

#include "NiBaseRendererOptionsView.h"
#include "NiDX9RendererDesc.h"

// Forward declarations
class NiDX9RendererDesc;

class NiDX9RendererOptionsView : public NiBaseRendererOptionsView
{
public:
    NiDX9RendererOptionsView();
    virtual ~NiDX9RendererOptionsView();

    // Returns name of a renderer
    virtual char* GetName();

    // Dialog creation and controls initialization function
    virtual NiWindowRef InitDialog(NiWindowRef pParentWnd);

protected:
    NiDX9RendererDesc* m_pkDX9RendererDesc;

    // Returns an instance of NiDX9RendererDesc
    virtual NiBaseRendererDesc* GetRendDesc();

};

#include "NiDX9RendererOptionsView.inl"

#endif // NIDX9RENDEREROPTIONSVIEW_H