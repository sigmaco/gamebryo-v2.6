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

#ifndef MYILIGHT_H
#define MYILIGHT_H

#include <NiSmartPointer.h>
#include "MyiObject.h"

NiSmartPointer(NiLight);

class MyiLight : public MyiObject
{
public:
    MyiLight(int iLightID);
    MyiLight();
    inline NiLight *GetLight();

protected:
    bool MakePointLight();
    bool MakeSpotLight();
    bool MakeInfiniteLight();
    bool MakeAmbientLight();
    void CheckAttributes();
    void SetAttenuation();
    void SetColors();
    void CheckForAnimatedColor();

    NiLightPtr m_spLight;
    int m_iLightID;
};

//---------------------------------------------------------------------------
inline NiLight *MyiLight::GetLight()
{
    return m_spLight;
}
//---------------------------------------------------------------------------

#endif 
