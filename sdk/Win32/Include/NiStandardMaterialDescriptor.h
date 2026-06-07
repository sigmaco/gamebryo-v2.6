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

#ifndef NISTANDARDMATERIALDESCRIPTOR_H
#define NISTANDARDMATERIALDESCRIPTOR_H

#include "NiMaterialDescriptor.h"
#include "NiBitfield.h"
#include "NiStandardMaterial.h"
#include "NiTexturingProperty.h"

class NIMAIN_ENTRY NiStandardMaterialDescriptor : public NiMaterialDescriptor
{
public:
    NiStandardMaterialDescriptor();

    // Add an NiLight to the bitfield. uiWhichLight is which slot to add the 
    // light in. If the light has a shadow generator, then shadows will be 
    // added as well. This function returns true if the light could be added.
    bool AddLight(NiLight* pkLight, unsigned int uiWhichLight,
        const NiRenderObject* pkGeometry = NULL);

    // If the specified light exists, determine its type. Returns true if the 
    // light exists.
    bool GetLightType(unsigned int uiWhichLight,
        NiStandardMaterial::LightType& eLightType);

    // Returns true if the specified light exists and it is a shadow caster.
    bool GetLightInfo(unsigned int uiWhichLight, bool& bShadowed);

    // Set whether or not a light is a shadow caster.
    void SetLightInfo(unsigned int uiWhichLight, bool bShadowed);

    // Set the input UV coordinate and the UV transformation for a given texture.
    void SetTextureUsage(unsigned int uiWhichTexture, unsigned int uiInputUV,
        NiStandardMaterial::TexGenOutput eTextureOutput);

    // Get the input UV coordinate and the UV transformation for a given texture.
    void GetTextureUsage(unsigned int uiWhichTexture, unsigned int& uiInputUV,
        NiStandardMaterial::TexGenOutput& eTextureOutput);

    // Set whether or not a projected light is clipped and whether it is 
    // parallel or perspective projected.
    void SetProjectedLight(unsigned int uiWhichLight, bool bClipped,
        NiStandardMaterial::TexEffectType eType);

    // Get whether or not a projected light is clipped and whether it is
    // parallel or perspective projected.
    void GetProjectedLight(unsigned int uiWhichLight, bool& bClipped,
        NiStandardMaterial::TexEffectType& eType);

    // Set whether or not a projected shadow is clipped and whether it is
    // parallel or perspective projected.
    void SetProjectedShadow(unsigned int uiWhichShadow, bool bClipped,
        NiStandardMaterial::TexEffectType eType);

    // Get whether or not a projected shadow is clipped and whether it is 
    // parallel or perspective projected.
    void GetProjectedShadow(unsigned int uiWhichShadow, bool& bClipped,
        NiStandardMaterial::TexEffectType& eType);

    // Get the number of textures used.
    unsigned int GetStandardTextureCount();

    // Returns a string representation of the state of the bitfield.
    NiString ToString();

    NiBeginDefaultBitfieldEnum()
        
    // First Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(TRANSFORM,       2,
        BITFIELDSTART,    0)
    NiDeclareDefaultIndexedBitfieldEntry(NORMAL,          2,
        TRANSFORM,        0)
    NiDeclareDefaultIndexedBitfieldEntry(SPECULAR,        1,
        NORMAL,           0)
    NiDeclareDefaultIndexedBitfieldEntry(VERTEXCOLORS,    1,
        SPECULAR,         0)
    NiDeclareDefaultIndexedBitfieldEntry(AMBDIFFEMISSIVE, 2,
        VERTEXCOLORS,     0)

    // Second Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(LIGHTINGMODE,    1,
        AMBDIFFEMISSIVE,  0)
    NiDeclareDefaultIndexedBitfieldEntry(APPLYMODE,       1,
        LIGHTINGMODE,     0)
    NiDeclareDefaultIndexedBitfieldEntry(FOGTYPE,         2,
        APPLYMODE,        0)
    NiDeclareDefaultIndexedBitfieldEntry(INPUTUVCOUNT,    3,
        FOGTYPE,          0)
    NiDeclareDefaultIndexedBitfieldEntry(PARALLAXMAPCOUNT,1,
        INPUTUVCOUNT,     0)

    //Third Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(BASEMAPCOUNT,    1,
        PARALLAXMAPCOUNT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(NORMALMAPCOUNT,  1,
        BASEMAPCOUNT,     0)
    NiDeclareDefaultIndexedBitfieldEntry(NORMALMAPTYPE,   2,
        NORMALMAPCOUNT,   0)
    NiDeclareDefaultIndexedBitfieldEntry(DARKMAPCOUNT,    1,
        NORMALMAPTYPE,    0)
    NiDeclareDefaultIndexedBitfieldEntry(DETAILMAPCOUNT,  1,
        DARKMAPCOUNT,     0)
    NiDeclareDefaultIndexedBitfieldEntry(BUMPMAPCOUNT,    1,
        DETAILMAPCOUNT,   0)
    NiDeclareDefaultIndexedBitfieldEntry(GLOSSMAPCOUNT,   1,
        BUMPMAPCOUNT,     0)

    //Fourth Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(GLOWMAPCOUNT,    1,
        GLOSSMAPCOUNT,    0)
    NiDeclareDefaultIndexedBitfieldEntry(ENVMAPTYPE,      3,
        GLOWMAPCOUNT,     0)
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP00COUNT,1,
        ENVMAPTYPE,       0)
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP01COUNT,1,
        CUSTOMMAP00COUNT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP02COUNT,1,
        CUSTOMMAP01COUNT, 0)
    NiDeclareDefaultLastIndexedBitfieldEntry(CUSTOMMAP03COUNT,1,
        CUSTOMMAP02COUNT,
        0)

    // First Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP04COUNT,  1,
        BITFIELDSTART, 1)
    NiDeclareDefaultIndexedBitfieldEntry(DECALMAPCOUNT,     2,
        CUSTOMMAP04COUNT,1)
    NiDeclareDefaultIndexedBitfieldEntry(PERVERTEXFORLIGHTS,1,
        DECALMAPCOUNT,  1)
    NiDeclareDefaultIndexedBitfieldEntry(POINTLIGHTCOUNT,   4,
        PERVERTEXFORLIGHTS, 1)

    // Second Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(SPOTLIGHTCOUNT,  4,
        POINTLIGHTCOUNT, 1)
    NiDeclareDefaultIndexedBitfieldEntry(DIRLIGHTCOUNT,   4,
        SPOTLIGHTCOUNT,  1)

    // Third Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(SHADOWMAPFORLIGHT,8,
        DIRLIGHTCOUNT,  1)

    // Fourth Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(PROJLIGHTMAPCOUNT,   2,
        SHADOWMAPFORLIGHT,  1)
    NiDeclareDefaultIndexedBitfieldEntry(PROJLIGHTMAPTYPES,   3,
        PROJLIGHTMAPCOUNT,  1)
    NiDeclareDefaultLastIndexedBitfieldEntry(PROJLIGHTMAPCLIPPED, 3,
        PROJLIGHTMAPTYPES,  1)

    // First Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(PROJSHADOWMAPCOUNT,   2,
        BITFIELDSTART,  2)
    NiDeclareDefaultIndexedBitfieldEntry(PROJSHADOWMAPTYPES,   3,
        PROJSHADOWMAPCOUNT,  2)
    NiDeclareDefaultIndexedBitfieldEntry(PROJSHADOWMAPCLIPPED, 3,
        PROJSHADOWMAPTYPES,  2)

    // Second Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(MAP00,    3,
        PROJSHADOWMAPCLIPPED, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP00TEXOUTPUT, 1,    MAP00, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP01,    3, MAP00TEXOUTPUT, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP01TEXOUTPUT, 1,    MAP01, 2)
    
    // Third Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(MAP02,    3,  MAP01TEXOUTPUT, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP02TEXOUTPUT, 1,    MAP02, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP03,    3, MAP02TEXOUTPUT, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP03TEXOUTPUT, 1,    MAP03, 2)

    // Fourth Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(MAP04,    3,  MAP03TEXOUTPUT, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP04TEXOUTPUT, 1,    MAP04, 2)
    NiDeclareDefaultIndexedBitfieldEntry(MAP05,    3, MAP04TEXOUTPUT, 2)
    NiDeclareDefaultLastIndexedBitfieldEntry(MAP05TEXOUTPUT, 1, MAP05, 2)
        
    // First Byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(MAP06,    3,  BITFIELDSTART, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP06TEXOUTPUT, 1,    MAP06, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP07,    3, MAP06TEXOUTPUT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP07TEXOUTPUT, 1,    MAP07, 3)

    // Second Byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(MAP08,    3, MAP07TEXOUTPUT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP08TEXOUTPUT, 1,    MAP08, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP09,    3, MAP08TEXOUTPUT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP09TEXOUTPUT, 1,    MAP09, 3)
    
    // Third Byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(MAP10,    3,  MAP09TEXOUTPUT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP10TEXOUTPUT, 1,    MAP10, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP11,    3, MAP10TEXOUTPUT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(MAP11TEXOUTPUT, 1,    MAP11, 3)

    // Fourth Byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(USERDEFINED00,    1,
        MAP11TEXOUTPUT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(USERDEFINED01,    1,
        USERDEFINED00, 3)
    NiDeclareDefaultIndexedBitfieldEntry(USERDEFINED02,    1,
        USERDEFINED01, 3)
    NiDeclareDefaultIndexedBitfieldEntry(USERDEFINED03,    1,
        USERDEFINED02, 3)
    NiDeclareDefaultIndexedBitfieldEntry(SHADOWTECHNIQUE,  2,
        USERDEFINED03, 3)       
    NiDeclareDefaultIndexedBitfieldEntry(ALPHATEST,  1, SHADOWTECHNIQUE, 3)
    // Leave 1 bits for future.

    NiEndDefaultBitfieldEnum();   


    // Set the transform bitfield based on the state of the mesh. If the 
    // mesh is hardware skinned, bHardwareSkinned is true.
    bool SetTransformDescriptor(const NiRenderObject* pkMesh, 
        bool& bHardwareSkinned);

    // Set the NBT bitfield entry based on the contents of the mesh. If the 
    // mesh is hardware skinned, the BP semantics are checked instead of the
    // normal semantics. If normals exist, bNormals is true. If binormals and
    // tangents exist, bNBTs is true.
    bool SetNBTDescriptor(const NiRenderObject* pkMesh,
        bool bHardwareSkinned, bool& bNormals, bool& bNBTs);

    // Set the vertex color bitfield based on existence of a vertex color
    // vertex stream on the mesh. If they exist, bVertexColors is true.
    bool SetVertexColorDescriptor(const NiRenderObject* pkMesh,
        bool& bVertexColors);

    // Set the AMBDIFFEMISSIVE and LIGHTINGMODE bitfield entries based on the
    // NiVertexColorProperty attached to the mesh.
    bool SetVertexColorPropertyDescriptor(const NiRenderObject* pkMesh,
        const NiPropertyState* pkPropState, bool bVertexColors);

    // Set the SPECULAR entry based on the NiSpecularProperty attached to the
    // mesh.
    bool SetSpecularPropertyDescriptor(const NiRenderObject* pkMesh,     
        const NiPropertyState* pkPropState, bool& bSpecularEnabled);

    // Since no bitfield entries are associated with the material property, 
    // this method currently does nothing.
    bool SetMaterialPropertyDescriptor(const NiRenderObject* pkMesh,
        const NiPropertyState* pkPropState);

    // Set the FOGTYPE based on the NiFogProperty attached to the mesh
    bool SetFogPropertyDescriptor(const NiRenderObject* pkMesh,
        const NiPropertyState* pkPropState);

    // Set the ALPHATEST property based on the NiAlphaProperty attached to the
    // mesh.
    bool SetAlphaPropertyDescriptor(const NiRenderObject* pkMesh,
        const NiPropertyState* pkPropState);

    // If the base map exists on the NiTexturingProperty on the mesh, the 
    // BASEMAPCOUNT is set to 1, the base map slot in the ppkActiveMaps array 
    // is set, and the current map count is incremented.
    bool SetBaseMapDescriptor(const NiTexturingProperty* pkTexProp,
        const NiTexturingProperty::Map** ppkActiveMaps, 
        NiUInt32& uiCurrentMapCount);

    // If the normal map exists on the NiTexturingProperty on the mesh and the 
    // NBT bitfield entries are set, the NORMALMAPCOUNT is set to 1, the normal
    // map slot in the ppkActiveMaps array is set, the normal map type is set 
    // based on the texture type, and the current map count is incremented.
    bool SetNormalMapDescriptor(const NiTexturingProperty* pkTexProp,
        const NiTexturingProperty::Map** ppkActiveMaps, 
        NiUInt32& uiCurrentMapCount);

    // If the parallax map exists on the NiTexturingProperty on the mesh and 
    // the NBT bitfield entry is set, the PARALLAXMAPCOUNT is set to 1, the
    // parallax map slot in the ppkActiveMaps array is set, and the current 
    // map count is incremented.
    bool SetParallaxMapDescriptor(const NiTexturingProperty* pkTexProp,     
        const NiTexturingProperty::Map** ppkActiveMaps, 
        NiUInt32& uiCurrentMapCount);

    // If the dark map exists on the NiTexturingProperty on the mesh, the 
    // DARKMAPCOUNT is set to 1, the dark map slot in the ppkActiveMaps array 
    // is set, and the current map count is incremented.
    bool SetDarkMapDescriptor(const NiTexturingProperty* pkTexProp,
        const NiTexturingProperty::Map** ppkActiveMaps, 
        NiUInt32& uiCurrentMapCount);

    // If the detail map exists on the NiTexturingProperty on the mesh, the 
    // DETAILMAPCOUNT is set to 1, the detail map slot in the ppkActiveMaps 
    // array is set, and the current map count is incremented.
    bool SetDetailMapDescriptor(const NiTexturingProperty* pkTexProp,
        const NiTexturingProperty::Map** ppkActiveMaps, 
        NiUInt32& uiCurrentMapCount);

    // If the bump map exists on the NiTexturingProperty on the mesh and 
    // bEnvMap is true, the BUMPMAPCOUNT is set to 1, the bump map slot in the
    // ppkActiveMaps array is set, and the current map count is incremented.
    bool SetBumpMapDescriptor(const NiTexturingProperty* pkTexProp,
        const NiTexturingProperty::Map** ppkActiveMaps, 
        NiUInt32& uiCurrentMapCount, bool bEnvMap);

    // If the gloss map exist on the NiTexturingProperty on the mesh and 
    // bSpecularEnabled is true or bEnvMap is true, the GLOSSMAPCOUNT is set to
    // 1, the gloass map slot in the ppkActiveMaps array is set, and the 
    // current map count is incremented.
    bool SetGlossMapDescriptor(const NiTexturingProperty* pkTexProp,
        const NiTexturingProperty::Map** ppkActiveMaps, 
        NiUInt32& uiCurrentMapCount, bool bEnvMap, bool bSpecularEnabled);

    // If the glow map exists on the NiTexturingProperty on the mesh, the 
    // GLOWMAPCOUNT is set to 1, the glow map slot in the ppkActiveMaps 
    // array is set, and the current map count is incremented.    
    bool SetGlowMapDescriptor(const NiTexturingProperty* pkTexProp,
        const NiTexturingProperty::Map** ppkActiveMaps, 
        NiUInt32& uiCurrentMapCount);

    // If any decal maps exist on the NiTexturingProperty on the mesh, the 
    // DECALMAPCOUNT is set to number of maps, the decal map slots in the 
    // ppkActiveMaps array are set, and the current map count is incremented by 
    // the decal map count.
    bool SetDecalMapsDescriptor(const NiTexturingProperty* pkTexProp,
        const NiTexturingProperty::Map** ppkActiveMaps, 
        NiUInt32& uiCurrentMapCount, NiUInt32 uiDecalMapMax);

    // If any shader maps exist on the NiTexturingProperty on the mesh, the 
    // CUSTOMMAPXXCOUNT entry is set where XX is the index of the shader map,
    // the shader map slots in the ppkActiveMaps array are set, and the 
    // current map count is incremented by shader map count.
    bool SetShaderMapsDescriptor(const NiTexturingProperty* pkTexProp,
        const NiTexturingProperty::Map** ppkActiveMaps, 
        NiUInt32& uiCurrentMapCount, NiUInt32 uiShaderMapMax);

    // This method sets the APPLYREPLACE bitfield as well as invoking the
    // previous texturing property map functions to set the textures on this 
    // bitfield.
    bool SetTexuringPropertyDescriptor(const NiRenderObject* pkMesh,
        const NiPropertyState* pkPropState, 
        const NiDynamicEffectState* pkEffectState,
        const NiTexturingProperty::Map** ppkActiveMaps,
        NiUInt32 uiMapMax, NiUInt32 uiDecalMapMax, NiUInt32 uiShaderMapMax,
        bool bSpecularEnabled);

    // This method sets the input UV count based on the active map slots passed
    // in.
    bool SetTextureMapUVsDescriptor(const NiRenderObject* pkMesh,
        const NiTexturingProperty::Map** ppkActiveMaps,
        NiUInt32 uiMapMax, NiUInt32 uiNumInputTextureSets,
        NiUInt32 uiMaxUsedMap);

    // This method invokes the AddLight method for every light in the input 
    // effect state.
    bool SetLightsDescriptor(const NiRenderObject* pkMesh, 
        const NiDynamicEffectState* pkEffectState);

    // This method sets the ENVMAPTYPE bitfield entry if an environment texture
    // effect exists in the effect state on the mesh.
    bool SetEnvMapDescriptor(const NiRenderObject* pkMesh, 
        const NiDynamicEffectState* pkEffectState);

    // This method sets up to three projected light maps based on the input
    // effect state.
    bool SetProjLightMapDescriptor(const NiRenderObject* pkMesh, 
        const NiDynamicEffectState* pkEffectState);

    // This method sets up to three projected shadow maps based on the input
    // effect state.
    bool SetProjShadowMapDescriptor(const NiRenderObject* pkMesh, 
        const NiDynamicEffectState* pkEffectState);

    // This method sets the shadowing bits based on the lights in the effect
    // state attached to the mesh.
    bool SetShadowTechniqueDescriptor(const NiRenderObject* pkMesh,
        const NiDynamicEffectState* pkEffectState);
};

#endif  //#ifndef NISTANDARDMATERIALDESCRIPTOR_H
