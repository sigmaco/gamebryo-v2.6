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

#ifndef NIRENDERVIEW_H
#define NIRENDERVIEW_H

#include "NiRect.h"
#include "NiVisibleArray.h"
#include "NiFixedString.h"

class NIMAIN_ENTRY NiRenderView : public NiRefObject
{
    NiDeclareRootRTTI(NiRenderView);

public:
    NiRenderView();

    // Functions for accessing the name of the render view.
    inline void SetName(const NiFixedString& kName);
    inline const NiFixedString& GetName() const;

    // Enumerated value to force building of PV geometry array.
    enum
    {
        FORCE_PV_GEOMETRY_UPDATE = UINT_MAX
    };

    // Returns the cached PV geometry array, computing it first if necessary.
    const NiVisibleArray& GetPVGeometry(unsigned int uiFrameID);

    // Clears the cached PV geometry array, forcing it to be recomputed the
    // next time GetPVGeometry is called.
    inline void ClearCachedPVGeometry();

    // Function to use the provided viewport to set the camera data for the
    // renderer.
    virtual void SetCameraData(const NiRect<float>& kViewport) = 0;

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    // Function to compute the array of potentially visible geometry.
    virtual void CalculatePVGeometry() = 0;

    // The last computed PV geometry array.
    NiVisibleArray m_kCachedPVGeometry;

    // The last frame ID (used for caching purposes).
    unsigned int m_uiLastFrameID;

    // The name of the render view.
    NiFixedString m_kName;

    // Default name for all render views.
    static NiFixedString ms_kDefaultName;
};

NiSmartPointer(NiRenderView);

#include "NiRenderView.inl"

#endif  // #ifndef NIRENDERVIEW_H
