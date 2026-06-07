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

#ifndef NISHADERCONSTANTMAP_H
#define NISHADERCONSTANTMAP_H

#include "NiRefObject.h"
#include "NiFixedString.h"
#include "NiGPUProgram.h"
#include "NiLight.h"
#include "NiShaderAttributeDesc.h"
#include "NiSmartPointer.h"
#include "NiShaderError.h"

class NiDynamicEffect;
class NiDynamicEffectState;

class NIMAIN_ENTRY NiShaderConstantMap  : public NiRefObject
{
public:

    NiShaderConstantMap(NiGPUProgram::ProgramType eType);
    inline virtual ~NiShaderConstantMap(){};

    //*** Access functions
    inline NiGPUProgram::ProgramType GetProgramType() const;

    enum DefinedMappings
    {
        SCM_DEF_INVALID    = 0,
        // Common transforms
        SCM_DEF_PROJ,
        SCM_DEF_INVPROJ,
        SCM_DEF_VIEW,
        SCM_DEF_INVVIEW,
        SCM_DEF_WORLD,
        SCM_DEF_INVWORLD,
        SCM_DEF_WORLDVIEW,
        SCM_DEF_INVWORLDVIEW,
        SCM_DEF_VIEWPROJ,
        SCM_DEF_INVVIEWPROJ,
        SCM_DEF_WORLDVIEWPROJ,
        SCM_DEF_INVWORLDVIEWPROJ,
        // Transpose of common transforms
        SCM_DEF_PROJ_T,
        SCM_DEF_INVPROJ_T,
        SCM_DEF_VIEW_T,
        SCM_DEF_INVVIEW_T,
        SCM_DEF_WORLD_T,
        SCM_DEF_INVWORLD_T,
        SCM_DEF_WORLDVIEW_T,
        SCM_DEF_INVWORLDVIEW_T,
        SCM_DEF_VIEWPROJ_T,
        SCM_DEF_INVVIEWPROJ_T,
        SCM_DEF_WORLDVIEWPROJ_T,
        SCM_DEF_INVWORLDVIEWPROJ_T,
        // Bone matrices
        // These are used for matrix palette skinning. They supply the 
        // WorldViewProj matrix w/ the skin transform concated on as well.
        // This reduces the number of matrix operations that must be 
        // performed for each bone!
        SCM_DEF_SKINWORLDVIEW,
        SCM_DEF_INVSKINWORLDVIEW,
        SCM_DEF_SKINWORLDVIEW_T,
        SCM_DEF_INVSKINWORLDVIEW_T,
        SCM_DEF_SKINWORLDVIEWPROJ,
        SCM_DEF_INVSKINWORLDVIEWPROJ,
        SCM_DEF_SKINWORLDVIEWPROJ_T,
        SCM_DEF_INVSKINWORLDVIEWPROJ_T,
        SCM_DEF_BONE_MATRIX_3,
        SCM_DEF_BONE_MATRIX_4,
        SCM_DEF_SKINBONE_MATRIX_3,
        SCM_DEF_SKINBONE_MATRIX_4,
        // Texture matrices
        SCM_DEF_TEXTRANSFORMBASE,
        SCM_DEF_INVTEXTRANSFORMBASE,
        SCM_DEF_TEXTRANSFORMBASE_T,
        SCM_DEF_INVTEXTRANSFORMBASE_T,
        SCM_DEF_TEXTRANSFORMDARK,
        SCM_DEF_INVTEXTRANSFORMDARK,
        SCM_DEF_TEXTRANSFORMDARK_T,
        SCM_DEF_INVTEXTRANSFORMDARK_T,
        SCM_DEF_TEXTRANSFORMDETAIL,
        SCM_DEF_INVTEXTRANSFORMDETAIL,
        SCM_DEF_TEXTRANSFORMDETAIL_T,
        SCM_DEF_INVTEXTRANSFORMDETAIL_T,
        SCM_DEF_TEXTRANSFORMGLOSS,
        SCM_DEF_INVTEXTRANSFORMGLOSS,
        SCM_DEF_TEXTRANSFORMGLOSS_T,
        SCM_DEF_INVTEXTRANSFORMGLOSS_T,
        SCM_DEF_TEXTRANSFORMGLOW,
        SCM_DEF_INVTEXTRANSFORMGLOW,
        SCM_DEF_TEXTRANSFORMGLOW_T,
        SCM_DEF_INVTEXTRANSFORMGLOW_T,
        SCM_DEF_TEXTRANSFORMBUMP,
        SCM_DEF_INVTEXTRANSFORMBUMP,
        SCM_DEF_TEXTRANSFORMBUMP_T,
        SCM_DEF_INVTEXTRANSFORMBUMP_T,
        SCM_DEF_TEXTRANSFORMDECAL,
        SCM_DEF_INVTEXTRANSFORMDECAL,
        SCM_DEF_TEXTRANSFORMDECAL_T,
        SCM_DEF_INVTEXTRANSFORMDECAL_T,
        SCM_DEF_TEXTRANSFORMSHADER,
        SCM_DEF_INVTEXTRANSFORMSHADER,
        SCM_DEF_TEXTRANSFORMSHADER_T,
        SCM_DEF_INVTEXTRANSFORMSHADER_T,

        // Matrix count - used to quickly identify matrix-based entries
        SCM_DEF_MATRIXTYPE_COUNT,
        // Lighting
        SCM_DEF_LIGHT_POS_WS = SCM_DEF_MATRIXTYPE_COUNT,
        SCM_DEF_LIGHT_DIR_WS,
        SCM_DEF_LIGHT_POS_OS,
        SCM_DEF_LIGHT_DIR_OS,
        // Materials
        SCM_DEF_MATERIAL_DIFFUSE,
        SCM_DEF_MATERIAL_AMBIENT,
        SCM_DEF_MATERIAL_SPECULAR,
        SCM_DEF_MATERIAL_EMISSIVE,
        SCM_DEF_MATERIAL_POWER,
        // Eye
        SCM_DEF_EYE_POS, // World space
        SCM_DEF_EYE_DIR, // World space
        // Constants
        SCM_DEF_CONSTS_TAYLOR_SIN,
        SCM_DEF_CONSTS_TAYLOR_COS,
        SCM_DEF_CONSTS_TIME,           // RenderMonkey
        SCM_DEF_CONSTS_SINTIME,
        SCM_DEF_CONSTS_COSTIME,
        SCM_DEF_CONSTS_TANTIME,
        SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME,

        // Ambient lighting.
        SCM_DEF_AMBIENTLIGHT,

        // Additional texture transform of NiTexturingProperty::Map
        SCM_DEF_TEXTRANSFORMNORMAL,
        SCM_DEF_INVTEXTRANSFORMNORMAL,
        SCM_DEF_TEXTRANSFORMNORMAL_T,
        SCM_DEF_INVTEXTRANSFORMNORMAL_T,
        SCM_DEF_TEXTRANSFORMPARALLAX,
        SCM_DEF_INVTEXTRANSFORMPARALLAX,
        SCM_DEF_TEXTRANSFORMPARALLAX_T,
        SCM_DEF_INVTEXTRANSFORMPARALLAX_T,

        SCM_DEF_SKINWORLD,
        SCM_DEF_INVSKINWORLD,
        SCM_DEF_SKINWORLD_T,
        SCM_DEF_INVSKINWORLD_T,

        // Fog
        SCM_DEF_FOG_DENSITY,
        SCM_DEF_FOG_NEARFAR,
        SCM_DEF_FOG_COLOR,

        // Bump Map
        SCM_DEF_BUMP_MATRIX,
        SCM_DEF_BUMP_LUMA_OFFSET_AND_SCALE,

        // Parallax Map
        SCM_DEF_PARALLAX_OFFSET,

        // Texture dimensions
        SCM_DEF_TEXSIZEBASE,
        SCM_DEF_TEXSIZEDARK,
        SCM_DEF_TEXSIZEDETAIL,
        SCM_DEF_TEXSIZEGLOSS,
        SCM_DEF_TEXSIZEGLOW,
        SCM_DEF_TEXSIZEBUMP,
        SCM_DEF_TEXSIZENORMAL,
        SCM_DEF_TEXSIZEPARALLAX,
        SCM_DEF_TEXSIZEDECAL,
        SCM_DEF_TEXSIZESHADER,

        // Alpha test
        SCM_DEF_ALPHA_TEST_FUNC,
        SCM_DEF_ALPHA_TEST_REF,

        // Xenon instancing 
        SCM_DEF_XENON_INST_FREQUENCY,

        // Insert new predefines above here!
        SCM_DEF_COUNT
    };

    class NIMAIN_ENTRY PredefinedMapping : public NiMemObject
    {
    public:
        unsigned int m_uiMappingID;
        NiFixedString m_kMappingName;

        PredefinedMapping();
        PredefinedMapping(unsigned int uiMappingID, 
            const NiFixedString& kMappingName);

        PredefinedMapping& operator=(const PredefinedMapping& kOther);
    };

    enum ObjectMappings
    {
        SCM_OBJ_INVALID = 0,

        // Specific to dynamic effects.
        SCM_OBJ_DIMMER,
        SCM_OBJ_UNDIMMEDAMBIENT,
        SCM_OBJ_UNDIMMEDDIFFUSE,
        SCM_OBJ_UNDIMMEDSPECULAR,
        SCM_OBJ_AMBIENT,
        SCM_OBJ_DIFFUSE,
        SCM_OBJ_SPECULAR,
        SCM_OBJ_WORLDPOSITION,
        SCM_OBJ_MODELPOSITION,
        SCM_OBJ_WORLDDIRECTION,
        SCM_OBJ_MODELDIRECTION,
        SCM_OBJ_WORLDTRANSFORM,
        SCM_OBJ_MODELTRANSFORM,
        SCM_OBJ_SPOTATTENUATION,
        SCM_OBJ_ATTENUATION,
        SCM_OBJ_WORLDPROJECTIONMATRIX,
        SCM_OBJ_MODELPROJECTIONMATRIX,
        SCM_OBJ_WORLDPROJECTIONTRANSLATION,
        SCM_OBJ_MODELPROJECTIONTRANSLATION,
        SCM_OBJ_WORLDCLIPPINGPLANE,
        SCM_OBJ_MODELCLIPPINGPLANE,
        SCM_OBJ_TEXCOORDGEN,
        SCM_OBJ_WORLDPROJECTIONTRANSFORM,
        SCM_OBJ_MODELPROJECTIONTRANSFORM,
        SCM_OBJ_WORLDTOSHADOWMAPMATRIX,
        SCM_OBJ_SHADOWMAP,
        SCM_OBJ_SHADOWMAPTEXSIZE,
        SCM_OBJ_SHADOWBIAS,
        SCM_OBJ_SHADOW_VSM_POWER_EPSILON,

        // Insert new object mappings above here!
        SCM_OBJ_COUNT
    };

    virtual NiShaderError AddEntry(const char* pszKey, unsigned int uiFlags,
        unsigned int uiExtra, unsigned int uiShaderRegister, 
        unsigned int uiRegisterCount, const char* pszVariableName = NULL, 
        unsigned int uiDataSize = 0, unsigned int uiDataStride = 0, 
        const void* pvDataSource = NULL, bool bCopyData = false);
    virtual NiShaderError AddPredefinedEntry(const char* pszKey, 
        unsigned int uiExtra, unsigned int uiShaderRegister, 
        const char* pszVariableName);
    virtual NiShaderError AddAttributeEntry(const char* pszKey, 
        unsigned int uiFlags, unsigned int uiExtra, 
        unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
        const char* pszVariableName, unsigned int uiDataSize, 
        unsigned int uiDataStride, const void* pvDataSource, 
        bool bCopyData = true);
    virtual NiShaderError AddConstantEntry(const char* pszKey, 
        unsigned int uiFlags, unsigned int uiExtra, 
        unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
        const char* pszVariableName, unsigned int uiDataSize, 
        unsigned int uiDataStride, const void* pvDataSource, 
        bool bCopyData = true);
    virtual NiShaderError AddGlobalEntry(const char* pszKey, 
        unsigned int uiFlags, unsigned int uiExtra, 
        unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
        const char* pszVariableName, unsigned int uiDataSize, 
        unsigned int uiDataStride, const void* pvDataSource, 
        bool bCopyData = false);
    virtual NiShaderError AddOperatorEntry(const char* pszKey, 
        unsigned int uiFlags, unsigned int uiExtra, 
        unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
        const char* pszVariableName);
    virtual NiShaderError AddObjectEntry(const char* pszKey,
        unsigned int uiShaderRegister, const char* pszVariableName,
        unsigned int uiObjectIndex,
        NiShaderAttributeDesc::ObjectType eObjectType);

    // *** begin Emergent internal use only ***
    static const char* GetTimeExtraDataName();

    static bool ParseKeyName(const char* pszOrigName, char* pszTrueName, 
        unsigned int uiMaxLen, unsigned int& uiRegisterCount);
    static bool LookUpPredefinedMapping(const char* pszMapping,
        unsigned int& uiMappingID, unsigned int& uiNumRegisters);
    static NiShaderAttributeDesc::AttributeType LookUpPredefinedMappingType(
        unsigned int uiMappingID, unsigned int uiNumRegisters);
    static NiShaderAttributeDesc::AttributeType LookUpPredefinedMappingType(
        const char* pszMapping);
    static bool LookUpPredefinedMappingName(
        unsigned int uiMappingID, NiFixedString& kName);

    static bool IsObjectMappingValidForType(ObjectMappings eMapping,
        NiShaderAttributeDesc::ObjectType eType);
    static bool LookUpObjectMapping(const char* pcMapping,
        unsigned int& uiMappingID);
    static NiShaderAttributeDesc::AttributeType LookUpObjectMappingType(
        unsigned int uiMappingID, unsigned int& uiRegCount,
        unsigned int& uiFloatCount);
    static bool LookUpObjectMappingName(
        unsigned int uiMappingID, NiFixedString& kName);
    static bool IsLightCorrectType(NiLight* pkLight,
        NiShaderAttributeDesc::ObjectType eType);

    static void _SDMInit();
    static void _SDMShutdown();

    static NiDynamicEffect* GetDynamicEffectForObject(
        const NiDynamicEffectState* pkEffectState,
        NiShaderAttributeDesc::ObjectType eObjectType, 
        unsigned int uiObjectIndex);
    // *** end Emergent internal use only ***
protected:
    static void InitializeObjectMappingTypeInfo();

    NiGPUProgram::ProgramType  m_eProgramType;

    struct MappingTypeInfo
    {
        unsigned int m_uiFloatCount;
        unsigned int m_uiRegCount;
        NiShaderAttributeDesc::AttributeType m_eAttribType;
    };

    static const char* ms_pcTimeExtraDataName;
    static PredefinedMapping* ms_pkPredefinedMappings;
    static unsigned int ms_uiPredefinedMappingCount;
    static PredefinedMapping* ms_pkObjectMappings;
    static unsigned int ms_uiObjectMappingCount;

    static MappingTypeInfo ms_akObjectMappingTypeInfo[SCM_OBJ_COUNT];
};

NiSmartPointer(NiShaderConstantMap);

#include "NiShaderConstantMap.inl"

#endif
