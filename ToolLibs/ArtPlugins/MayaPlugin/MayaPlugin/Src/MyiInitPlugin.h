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

#ifndef MYIINITPLUGIN_H
#define MYIINITPLUGIN_H

//---------------------------------------------------------------------------
class MyiInitPlugin : public NiMemObject
{
public:

    MyiInitPlugin();
    ~MyiInitPlugin();

    static void Init();
    static void Shutdown();

    static const char* GetShadowTechniqueName(const int iTechniqueIndex);
    static float GetShadowTechniqueBias(const int iTechniqueIndex,
        const int iLightType);

private:

    char* 
        m_apShadowTechniqueNames[NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES];
    float m_aafShadowTechniqueBias[NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES]
        [NiStandardMaterial::LIGHT_MAX];
};
//---------------------------------------------------------------------------

#endif