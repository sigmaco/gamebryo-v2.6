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

#ifndef NICLICKRENDERSTEP_H
#define NICLICKRENDERSTEP_H

#include "NiRenderStep.h"
#include "NiRenderClick.h"

class NIMAIN_ENTRY NiClickRenderStep : public NiRenderStep
{
    NiDeclareRTTI;

public:
    NiClickRenderStep();

    // Functions for getting and setting the render target group that will
    // contain the final output for this render step. This equates to the
    // render target group of the last render click in this render step. If no
    // clicks exist, NULL is returned.
    virtual bool SetOutputRenderTargetGroup(
        NiRenderTargetGroup* pkOutputRenderTargetGroup);
    virtual NiRenderTargetGroup* GetOutputRenderTargetGroup();

    // Functions for reporting statistics about the most recent frame.
    virtual unsigned int GetNumObjectsDrawn() const;
    virtual float GetCullTime() const;
    virtual float GetRenderTime() const;

protected:
    // Rendering function.
    virtual void PerformRendering();

    // Function that returns a list of render clicks.
    virtual const NiTPointerList<NiRenderClickPtr>& GetRenderClicks() = 0;

    // Function for rendering list of clicks.
    virtual void RenderClicks(
        const NiTPointerList<NiRenderClickPtr>& kRenderClicks);

    // Rendering statistics.
    unsigned int m_uiNumObjectsDrawn;
    float m_fCullTime;
    float m_fRenderTime;
};

NiSmartPointer(NiClickRenderStep);

#endif  // #ifndef NICLICKRENDERSTEP_H
