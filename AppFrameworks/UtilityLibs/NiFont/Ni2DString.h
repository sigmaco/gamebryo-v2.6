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

#ifndef NI2DSTRING_H
#define NI2DSTRING_H

#include "NiFontString.h"

#include <NiRect.h>
#include <NiMeshScreenElements.h>
#include "NiFont.h"

#define NiWChar unsigned short

// Ni2DString
class NIFONT_ENTRY Ni2DString : public NiFontString
{
public:

    enum {
        DEFAULT_POINT_SIZE = 0
    };

    // Unicode Constructor
    Ni2DString(NiFont* pkFont, 
        unsigned int uiFlags,
        unsigned int uiMaxStringLen,
        const NiWChar* pkString,
        const NiColorA& kColor = NiColorA::WHITE,
        short sX = 0,
        short sY = 0,
        unsigned char ucDirection = NIFONTSTRING_LEFT_TO_RIGHT,
        unsigned short usPointSize = DEFAULT_POINT_SIZE, 
        const NiRenderTargetGroup* pkTarget = NULL,
        bool bStatic = false);

    // ASCII Constructor
    Ni2DString(NiFont* pkFont, 
        unsigned int uiFlags,
        unsigned int uiMaxStringLen,
        const char* pcString,
        const NiColorA& kColor = NiColorA::WHITE,
        short sX = 0,
        short sY = 0,
        unsigned char ucDirection = NIFONTSTRING_LEFT_TO_RIGHT,
        unsigned short usPointSize = DEFAULT_POINT_SIZE, 
        const NiRenderTargetGroup* pkTarget = NULL,
        bool bStatic = false);

    virtual ~Ni2DString();

    void sprintf(const char* pcFormatString, ...);
    void sprintf(const NiWChar* pkFormatString, ...);

    // Get/Set the position of this string on the renderer target in pixels.
    // The most likely renderer target is the screen.
    inline void GetPosition(short& sX, short& sY);
    inline void SetPosition(short sX, short sY);

    // Override base implementation because color is handled by material,
    // not vertices.
    virtual void SetColor(const NiColorA& kColor);

    // Rendering Functions
    void Draw(NiRenderer* pkRenderer);

    // Get Size, etc.
    void GetTextExtent(float& fWidth, float& fHeight);

    // Get and set the final point size
    inline unsigned short GetPointSize();
    inline void SetPointSize(unsigned short usPointSize);

    // Get and Set the NiRenderTargetGroup so we can have the target size
    // for converting between pixels and normalized display coordinates.
    // The target is NOT used for rendering. The string will use the active
    // render target when drawing.
    inline const NiRenderTargetGroup* GetRenderTargetGroup();
    inline void SetRenderTargetGroup(NiRenderTargetGroup* pkTarget);

    // Get the NiMeshScreenElements associated with this string. The 
    // NiMeshScreenElements may be NULL if a string has not been set yet.
    inline NiMeshScreenElements* GetScreenElements(unsigned char ucIndex);

    //*** begin Emergent internal use only
    void CreateScreenElements();
    void UpdateScreenElements(bool bForceRecreate = false);
    void SetUseBatchUpdate(bool bUseBatchUpdate);
    //*** end Emergent internal use only

protected:

    inline void CreateScreenPolygon(NiWChar ch, short& sCurrX, short& sCurrY, 
         short sW, short sH, float& fScreenWidth, float& fScreenHeight, 
         float* pfUVSets);

    unsigned short m_usPointSize;

    short m_sX;     // Position in pixel coordinates
    short m_sY;

    NiTObjectArray<NiMeshScreenElementsPtr> m_kMeshScreenElements;
    const NiRenderTargetGroup* m_pkTarget;

    bool m_bUseBatchUpdate;
};

typedef NiPointer<Ni2DString> Ni2DStringPtr;

#include "Ni2DString.inl"

#endif // NI2DSTRING_H
