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

// MyiMaterialManager.h

#ifndef MYIMATERIALMANAGER_H
#define MYIMATERIALMANAGER_H

#include <NiSmartPointer.h>
#include <NiMaterialProperty.h>
#include <NiAlphaProperty.h>
#include <NiShader.h>

#include "MyiObject.h"

NiSmartPointer(NiMaterialProperty);
NiSmartPointer(NiSpecularProperty);
NiSmartPointer(NiVertexColorProperty);

class MyiPixelShader;
class NiMesh;

class MyiMaterialManager : public MyiObject
{
public:
    static const char *m_pTag;

    MyiMaterialManager(void);
    ~MyiMaterialManager(void);
    void GetMaterial(int iMaterialID, NiMaterialProperty *&pMaterial,
        NiSpecularProperty *&pSpecular, 
        NiVertexColorProperty *&pVertexColor,
        NiAlphaProperty *&pAlphaProperty);

    NiShaderRequirementDesc::NBTFlags GetNBTMethod(unsigned int iMaterialID);
    unsigned int GetNBTUVSource(unsigned int iMaterialID);

    void AddPixelShaderAndAttributes(unsigned int iMaterialID,
        NiMesh* pkMesh);

    void SetNBTMethod(unsigned int iMaterialID, 
        NiShaderRequirementDesc::NBTFlags eNBTMethod);
    void SetNBTUVSource(unsigned int iMaterialID, unsigned int uiNBTUVSource);

protected:
    bool IsSpecular(unsigned int i) const;
    bool SpecularInit();
    bool VertexColorInit();

    unsigned int m_uiMaterials;
    NiMaterialPropertyPtr *m_sppMaterials;
    MyiPixelShader** m_ppPixelShaders;

    NiSpecularPropertyPtr       m_spSpecularOn;
    NiSpecularPropertyPtr       m_spSpecularOff;
    NiVertexColorPropertyPtr    m_spVertexProperty;
};

#endif