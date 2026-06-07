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

#ifndef MYITEXTUREEFFECT_H
#define MYITEXTUREEFFECT_H

#include <NiSmartPointer.h>
#include <NiTList.h>
#include "MyiObject.h"

NiSmartPointer(NiTextureEffect);






class MyiTextureEffect : public MyiObject
{
public:
    MyiTextureEffect(int iLightID);
    MyiTextureEffect();
    inline NiTextureEffect *GetTextureEffect();
    inline int GetLightID();

protected:

    void LoadTextureEffect();

    NiTextureEffectPtr m_spTextureEffect;
    int m_iLightID;
};

//---------------------------------------------------------------------------
inline NiTextureEffect *MyiTextureEffect::GetTextureEffect()
{
    return m_spTextureEffect;
}
//---------------------------------------------------------------------------
inline int MyiTextureEffect::GetLightID()
{
    return m_iLightID;
}
//---------------------------------------------------------------------------


class MyiTextureEffectManager : public NiMemObject
{
public:
    
    MyiTextureEffectManager();
    ~MyiTextureEffectManager();

    void AddTextureEffect(int iLightID);

    NiTextureEffect *GetTextureEffect(int iLightID);
    inline bool Fail();

protected:

    bool IsTextureEffect(int iLightID);

    NiTList<MyiTextureEffect*> m_pTextureEffects;
    bool m_bFail;

};

//---------------------------------------------------------------------------
inline bool MyiTextureEffectManager::Fail()
{
    return m_bFail;
}
//---------------------------------------------------------------------------






#endif 
