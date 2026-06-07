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

#ifndef NISHADOWRENDERCLICK_H
#define NISHADOWRENDERCLICK_H

#include "NiViewRenderClick.h"

class NiShadowGenerator;

class NIMAIN_ENTRY NiShadowRenderClick : public NiViewRenderClick
{
    NiDeclareRTTI;

public:

    NiShadowRenderClick();
    virtual ~NiShadowRenderClick();

    // Functions for accessing the shadow generator associated with this
    // render click (may be NULL).
    inline void SetGenerator(NiShadowGenerator* pkGenerator);
    inline NiShadowGenerator* GetGenerator() const;

    // Set and get whether or not to force rendering (not used by this class).
    inline void SetForceRender(bool bForceRender);
    inline bool GetForceRender() const;

    // Dirty bit that is cleared when PerformRendering is called.
    inline void MarkAsDirty();
    inline bool IsDirty() const;

    // Get the frame id when this shadow click was last rendered
    unsigned int GetLastRenderedFrameID();

protected:

    // Flags
    enum 
    {
        FORCE_RENDER    = 0x0008,
        DIRTY           = 0x0010
    };

    // Calls base class version and clears the dirty bit.
    virtual void PerformRendering(unsigned int uiFrameID);

    // The shadow generator associated with this render click (used by
    // NiShadowClickValidator).
    NiShadowGenerator* m_pkGenerator;

    NiDeclareFlags(unsigned char);

    unsigned int m_uiLastRenderedFrameID;
};

NiSmartPointer(NiShadowRenderClick);

#include "NiShadowRenderClick.inl"

#endif  // #ifndef NISHADOWRENDERCLICK_H
