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

#include "NiStandardMaterial.h"
#include "NiCommonMaterialLibType.h"

class NICOMMONMATERIAL_ENTRY AdditiveLightMapsMaterial
    : public NiStandardMaterial
{
public:
    NiDeclareRTTI;
    AdditiveLightMapsMaterial(bool bAutoCreateCaches = true);

    enum
    {
        NIADDITIVELIGHTMAPSMATERIAL_VERTEX_VERSION =  
            (NiStandardMaterial::VERTEX_VERSION << 8) + 1,
        NIADDITIVELIGHTMAPSMATERIAL_PIXEL_VERSION =   
            (NiStandardMaterial::PIXEL_VERSION << 8) + 1,
        NIADDITIVELIGHTMAPSMATERIAL_GEOMETRY_VERSION =   
            (NiStandardMaterial::GEOMETRY_VERSION << 8) + 1,
    };

protected:

    // overridden virtuals
    virtual bool HandlePreLightTextureApplication(Context& kContext,
        NiStandardPixelProgramDescriptor* pkPixelDesc, 
        NiMaterialResource*& pkWorldPos,
        NiMaterialResource*& pkWorldNormal,
        NiMaterialResource*& pkWorldBinormal,
        NiMaterialResource*& pkWorldTangent,
        NiMaterialResource*& pkWorldViewVector,
        NiMaterialResource*& pkTangentViewVector,
        NiMaterialResource*& pkMatDiffuseColor,
        NiMaterialResource*& pkMatSpecularColor,
        NiMaterialResource*& pkMatSpecularPower,
        NiMaterialResource*& pkMatGlossiness,
        NiMaterialResource*& pkMatAmbientColor,
        NiMaterialResource*& pkMatEmissiveColor,
        NiMaterialResource*& pkOpacityAccum,
        NiMaterialResource*& pkAmbientLightAccum,
        NiMaterialResource*& pkDiffuseLightAccum,
        NiMaterialResource*& pkSpecularLightAccum,
        NiMaterialResource*& pkDiffuseTexAccum,
        NiMaterialResource*& pkSpecularTexAccum,
        unsigned int& uiTexturesApplied,
        NiMaterialResource** apkUVSets,
        unsigned int uiNumStandardUVs,
        unsigned int uiNumTexEffectUVs);
};
