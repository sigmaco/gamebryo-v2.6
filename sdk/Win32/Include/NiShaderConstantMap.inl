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
//---------------------------------------------------------------------------
// NiShaderConstantMap inline functions
//---------------------------------------------------------------------------
inline NiGPUProgram::ProgramType NiShaderConstantMap::GetProgramType() const
{
    return m_eProgramType;
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMap::IsLightCorrectType(NiLight* pkLight,
    NiShaderAttributeDesc::ObjectType eType)
{
    NIASSERT(pkLight);

    // If light switch is off, do not use.
    bool bLightOn = pkLight->GetSwitch();
    unsigned int uiEffectType = pkLight->GetEffectType();

    // The logic below goes as follows:
    // If the light is not an ambient light, it will be the same as the 
    // effect type making the first statement true.
    // If the effect type is general, then we want any light other than an
    // ambient light. Ambient lights map to 0. In that case, the light type 
    // will not match the effect type hence the XOR.
    bool bType = (uiEffectType == (unsigned int)eType) ^ 
        ((uiEffectType > (unsigned int)eType) & 
        (eType == NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT));
    
    return bType & bLightOn;
}
//---------------------------------------------------------------------------
inline const char* NiShaderConstantMap::GetTimeExtraDataName()
{
    return ms_pcTimeExtraDataName;
}
//---------------------------------------------------------------------------
inline NiShaderAttributeDesc::AttributeType 
NiShaderConstantMap::LookUpObjectMappingType(
    unsigned int uiMappingID, unsigned int& uiRegCount,
    unsigned int& uiFloatCount)
{
    // Clamp to valid range.
    uiMappingID = NiMin((int)uiMappingID, (int)SCM_OBJ_COUNT);

    uiRegCount = ms_akObjectMappingTypeInfo[uiMappingID].m_uiRegCount;
    uiFloatCount = ms_akObjectMappingTypeInfo[uiMappingID].m_uiFloatCount;
    return ms_akObjectMappingTypeInfo[uiMappingID].m_eAttribType;
}
//---------------------------------------------------------------------------
