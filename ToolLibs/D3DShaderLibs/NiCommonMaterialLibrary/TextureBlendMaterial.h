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

class NICOMMONMATERIAL_ENTRY TextureBlendMaterial : public NiStandardMaterial
{
public:
    NiDeclareRTTI;
    TextureBlendMaterial(bool bAutoCreateCaches = true);

    enum
    {
        BLEND_TEXTURE_COUNT = 4,
        RED_MAP_INDEX   = 0,
        GREEN_MAP_INDEX = 1,
        BLUE_MAP_INDEX  = 2,
        ALPHA_MAP_INDEX = 3,
        CONTROL_MAP_INDEX = 4,
        TEXTURE_COUNT = 5,
        NITEXTUREBLENDMATERIAL_VERTEX_VERSION =  
            (NiStandardMaterial::VERTEX_VERSION << 8) + 1,
        NITEXTUREBLENDMATERIAL_PIXEL_VERSION =   
            (NiStandardMaterial::PIXEL_VERSION << 8) + 1,
        NITEXTUREBLENDMATERIAL_GEOMETRY_VERSION =   
            (NiStandardMaterial::GEOMETRY_VERSION << 8) + 1,
    };

    
    virtual void AddDefaultFallbacks();

    static bool SplitBlendTextureMaps(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);
protected:
    // overridden virtuals
    virtual bool HandleCustomMaps(Context& kContext, 
        NiStandardPixelProgramDescriptor* pkPixDesc, 
        unsigned int& uiWhichTexture, NiMaterialResource** apkUVSets,
        unsigned int uiNumStandardUVs, 
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
        NiMaterialResource*& pkSpecularTexAccum);
};
