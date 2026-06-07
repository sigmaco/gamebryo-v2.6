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

#ifndef NISCREENTEXTURE_H
#define NISCREENTEXTURE_H

#include "NiColor.h"
#include "NiObject.h"
#include "NiTSet.h"
#include "NiTexturingProperty.h"

NiSmartPointer(NiTexture);

/**
    This class is deprecated.

    It only exists to support loading old NIF files.
*/

class NIMAIN_ENTRY NiScreenTexture : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***

    virtual ~NiScreenTexture();

    class NIMAIN_ENTRY ScreenRect : public NiMemObject
    {
    public:
        short m_sPixTop;
        short m_sPixLeft;
        unsigned short m_usPixWidth;
        unsigned short m_usPixHeight;

        unsigned short m_usTexTop;
        unsigned short m_usTexLeft;

        NiColorA m_kColor;
    };

    const ScreenRect& GetScreenRect(unsigned int uiIndex) const;
    ScreenRect& GetScreenRect(unsigned int uiIndex);

    unsigned int GetNumScreenRects() const;

    NiTexturingProperty* GetTexturingProperty() const;

    // *** end Emergent internal use only ***
private:
    NiScreenTexture();

    NiTPrimitiveSet<ScreenRect> m_kScreenRects;
    NiTexturingPropertyPtr m_spTexProp;
};

NiSmartPointer(NiScreenTexture);
typedef NiTObjectArray<NiScreenTexturePtr> NiScreenTextureArray;

#endif // NISCREENTEXTURE_H

