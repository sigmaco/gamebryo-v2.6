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

// MyiMaterial.h

#ifndef MYIMATERIAL_H
#define MYIMATERIAL_H

#include <NiSmartPointer.h>
#include <NiVertexColorProperty.h>
#include <NiShader.h>
#include <NiNode.h>

#include <maya/MFnDependencyNode.h>
#include "MyiObject.h"

NiSmartPointer(NiMaterialProperty);
class MyiPixelShader;
class NiShaderAttributeDesc;

class MyiMaterial : public MyiObject
{
public:
    MyiMaterial( int iMaterialID);
    NiMaterialProperty *GetMaterial();
    int GetShadingModel( void ) const;

    MyiPixelShader* CreatePixelShader();

protected:

    void CreateAnimatedMaterialColors();
    void CreateAnimatedDiffuseColor();
    void CreateAnimatedSpecularColor();
    void CreateAnimatedAmbientColor();
    void CreateAnimatedEmittanceColor();
    void CreateAnimatedAlpha();

    bool CreatePixelShaderAttribute(NiNodePtr spNode, 
        const char* pcShaderName, const NiShaderAttributeDesc* pkDesc,
        MFnDependencyNode& kNode);

    bool CreatePixelShaderArrayAttribute(NiNodePtr spNode, 
        const char* pcShaderName, const NiShaderAttributeDesc* pkDesc,
        MFnDependencyNode& kNode);

    void CreateAttributeName(const char* pcShaderName, 
        const NiShaderAttributeDesc* pkDesc, char* pcBuffer, 
        unsigned int uiBufferSize);

    void CreateFloatsAnimation(MPlug kMPlug, 
        NiFloatsExtraData* pkData, int iIndex, NiNodePtr spNode );


    NiMaterialPropertyPtr m_spMaterial;
    int m_iShadingModel;
    int m_iMaterialID;


};



class MyiPixelShader : public NiMemObject
{
public:
    MyiPixelShader();
    ~MyiPixelShader();

    bool IsValid() const;

    NiMaterialPtr m_spMaterial;
    unsigned int m_uiMaterialExtraData;
    NiNodePtr m_spAttributeNode;
    NiShaderRequirementDesc::NBTFlags m_eNBTMethod;
    unsigned int m_uiNBTUVSource;
    bool m_bConfigurableMaterial;
};


#endif