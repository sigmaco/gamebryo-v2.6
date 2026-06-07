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

#include "NiTerrainPCH.h"

#include "NiWaterComponent.h"
#include "NiTerrainInteractor.h"
#include "NiWaterMaterial.h"
#include "NiTerrainComponent.h"

NiFixedString NiWaterComponent::ms_kClassName;
NiFixedString NiWaterComponent::ms_kComponentName;

NiFixedString NiWaterComponent::ms_kPropNameAssociatedTerrain;
NiFixedString NiWaterComponent::ms_kPropNameLength;
NiFixedString NiWaterComponent::ms_kPropNameWidth;
NiFixedString NiWaterComponent::ms_kPropNameMaxVerticesPerSide;
NiFixedString NiWaterComponent::ms_kPropNameDeepColor;
NiFixedString NiWaterComponent::ms_kPropNameShallowColor;
NiFixedString NiWaterComponent::ms_kPropNameDepthThreshold;
NiFixedString NiWaterComponent::ms_kPropNameNormalMapDir;
NiFixedString NiWaterComponent::ms_kPropNameNormalMapFile;
NiFixedString NiWaterComponent::ms_kPropNameTextureSizeRatio;
NiFixedString NiWaterComponent::ms_kPropNameSceneRoot;
NiFixedString NiWaterComponent::ms_kPropNameTexTransformVelocity;
NiFixedString NiWaterComponent::ms_kPropNameEnvMap;
NiFixedString NiWaterComponent::ms_kPropNameReflectionFactor;
NiFixedString NiWaterComponent::ms_kPropNameNormalWidth;
NiFixedString NiWaterComponent::ms_kPropNameNormalLength;
NiFixedString NiWaterComponent::ms_kPropNameDynamicEnvMap;
NiFixedString NiWaterComponent::ms_kPropNameRefractionMap;
NiFixedString NiWaterComponent::ms_kPropNameDynamicRefraction;
NiFixedString NiWaterComponent::ms_kPropNameSpecularity;
NiFixedString NiWaterComponent::ms_kPropNameDisturbance;
NiFixedString NiWaterComponent::ms_kPropNameFogDensity;
NiFixedString NiWaterComponent::ms_kPropNameFogDistance;
NiFixedString NiWaterComponent::ms_kPropNameFogFallOff;
NiFixedString NiWaterComponent::ms_kPropNameNormalAnimSpeed;

NiFixedString NiWaterComponent::ms_kPropNameTranslation;
NiFixedString NiWaterComponent::ms_kPropNameRotation;
NiFixedString NiWaterComponent::ms_kPropNameScale;


NiFixedString NiWaterComponent::ms_kPropDescAssociatedTerrain;
NiFixedString NiWaterComponent::ms_kPropDescLength;
NiFixedString NiWaterComponent::ms_kPropDescWidth;
NiFixedString NiWaterComponent::ms_kPropDescMaxVerticesPerSide;
NiFixedString NiWaterComponent::ms_kPropDescDeepColor;
NiFixedString NiWaterComponent::ms_kPropDescShallowColor;
NiFixedString NiWaterComponent::ms_kPropDescDepthThreshold;
NiFixedString NiWaterComponent::ms_kPropDescNormalMapDir;
NiFixedString NiWaterComponent::ms_kPropDescNormalMapFile;
NiFixedString NiWaterComponent::ms_kPropDescTextureSizeRatio;
NiFixedString NiWaterComponent::ms_kPropDescSceneRoot;
NiFixedString NiWaterComponent::ms_kPropDescTexTransformVelocity;
NiFixedString NiWaterComponent::ms_kPropDescEnvMap;
NiFixedString NiWaterComponent::ms_kPropDescReflectionFactor;
NiFixedString NiWaterComponent::ms_kPropDescNormalWidth;
NiFixedString NiWaterComponent::ms_kPropDescNormalLength;
NiFixedString NiWaterComponent::ms_kPropDescDynamicEnvMap;
NiFixedString NiWaterComponent::ms_kPropDescRefractionMap;
NiFixedString NiWaterComponent::ms_kPropDescDynamicRefraction;
NiFixedString NiWaterComponent::ms_kPropDescSpecularity;
NiFixedString NiWaterComponent::ms_kPropDescDisturbance;
NiFixedString NiWaterComponent::ms_kPropDescFogDensity;
NiFixedString NiWaterComponent::ms_kPropDescFogDistance;
NiFixedString NiWaterComponent::ms_kPropDescFogFallOff;
NiFixedString NiWaterComponent::ms_kPropDescNormalAnimSpeed;

NiWaterComponent::NiWaterComponent() :
    m_uFlags(0),
    m_kShallowColor(0.0f,0.5f,0.5f,0.15f),
    m_kDeepColor(0.0f,0.2f,0.4f,1.0f),
    m_kTexTransformVelocity(0.0f, 0.0f),    
    m_uiLength(128),
    m_uiWidth(128),
    m_uiMaxVerticesPerSide(50),
    m_uiNumVerticesLength(50),
    m_uiNumVerticesWidth(50),
    m_fDepthThreshold(50.0f),
    m_fNormalWidth(4.0f),
    m_fNormalLength(4.0f),
    m_fReflectionFactor(0.0f),
    m_fSpecularity(3.0f),
    m_fDisturbance(25.0f),
    m_fFogDensity(1.0f),
    m_fFogDistance(1000.0f),
    m_fFogFallOff(1.0f),
    m_fNormalAnimationSpeed(25.0f),
    m_uiTextureSizeRatio(64),
    m_kNormalMapDir(),
    m_kNormalMapFile("filename_%d.bmp"),    
    m_kEnvMap(),    
    m_kRefractionMap(),            
    m_spAssociatedTerrain(),   
    m_spMasterComponent(0),
    m_bPropertyChanged(true)
{
    m_spRootNode = NiNew NiWater();
}
//---------------------------------------------------------------------------
NiWaterComponent::NiWaterComponent(NiWaterComponent* pkTemplate) :
    m_uFlags(0),
    m_kShallowColor(pkTemplate->m_kShallowColor),
    m_kDeepColor(pkTemplate->m_kDeepColor),
    m_kTexTransformVelocity(pkTemplate->m_kTexTransformVelocity),    
    m_uiLength(pkTemplate->m_uiLength),
    m_uiWidth(pkTemplate->m_uiWidth),
    m_uiMaxVerticesPerSide(pkTemplate->m_uiMaxVerticesPerSide),
    m_uiNumVerticesLength(pkTemplate->m_uiNumVerticesLength),
    m_uiNumVerticesWidth(pkTemplate->m_uiNumVerticesWidth),
    m_fDepthThreshold(pkTemplate->m_fDepthThreshold),
    m_fNormalWidth(pkTemplate->m_fNormalWidth),
    m_fNormalLength(pkTemplate->m_fNormalLength),
    m_fReflectionFactor(pkTemplate->m_fReflectionFactor),
    m_fSpecularity(pkTemplate->m_fSpecularity),
    m_fDisturbance(pkTemplate->m_fDisturbance),
    m_fFogDensity(pkTemplate->m_fFogDensity),
    m_fFogDistance(pkTemplate->m_fFogDistance),
    m_fFogFallOff(pkTemplate->m_fFogFallOff),
    m_fNormalAnimationSpeed(pkTemplate->m_fNormalAnimationSpeed),
    m_uiTextureSizeRatio(pkTemplate->m_uiTextureSizeRatio),
    m_kNormalMapDir(pkTemplate->m_kNormalMapDir),
    m_kNormalMapFile(pkTemplate->m_kNormalMapFile),    
    m_kEnvMap(pkTemplate->m_kEnvMap),    
    m_kRefractionMap(pkTemplate->m_kRefractionMap),
    m_spAssociatedTerrain(pkTemplate->m_spAssociatedTerrain),
    m_spMasterComponent(pkTemplate),
    m_bPropertyChanged(true)
{
    m_spRootNode = NiNew NiWater(pkTemplate->m_spRootNode);
}
//---------------------------------------------------------------------------
NiWaterComponent::~NiWaterComponent()
{
    m_spMasterComponent = 0;
    m_spAssociatedTerrain = 0;
    m_spRootNode = 0;
}
//---------------------------------------------------------------------------
void NiWaterComponent::_SDMInit()
{
    ms_kClassName = "NiWaterComponent";
    ms_kComponentName = "Water";

    ms_kPropNameAssociatedTerrain = "Terrain entity";
    ms_kPropNameLength = "Surface length";
    ms_kPropNameWidth = "Surface width";
    ms_kPropNameMaxVerticesPerSide = "Max vertices per side";
    ms_kPropNameDeepColor = "Color deep";
    ms_kPropNameShallowColor = "Color shallow";
    ms_kPropNameDepthThreshold = "Depth threshold";
    ms_kPropNameTextureSizeRatio = "Resolution";
    ms_kPropNameSceneRoot = "Scene Root Pointer";
    ms_kPropNameTexTransformVelocity = "Map velocity";
    ms_kPropNameEnvMap = "Environment map";
    ms_kPropNameReflectionFactor = "Reflection factor";
    ms_kPropNameNormalMapDir = "Normal map directory";
    ms_kPropNameNormalMapFile = "Normal map file name mask";
    ms_kPropNameNormalLength = "Map x repetition factor";
    ms_kPropNameNormalWidth = "Map y repetition factor";
    ms_kPropNameDynamicEnvMap = "Dynamic Environment map";
    ms_kPropNameRefractionMap = "Refraction map path";
    ms_kPropNameDynamicRefraction = "Dynamic refraction map";
    ms_kPropNameSpecularity = "Specular factor"; 
    ms_kPropNameDisturbance = "Disturbance";
    ms_kPropNameFogDensity = "Density";
    ms_kPropNameFogDistance = "Distance";
    ms_kPropNameFogFallOff = "FallOff";
    ms_kPropNameNormalAnimSpeed = "Normal animation speed";

    ms_kPropNameTranslation = "Translation";
    ms_kPropNameRotation = "Rotation";
    ms_kPropNameScale = "Scale";

    ms_kPropDescAssociatedTerrain = "Terrain component associated to the"
        " water";
    ms_kPropDescLength = "Length of the mesh representing the water";
    ms_kPropDescWidth = "Width of the mesh representing the water";
    ms_kPropDescMaxVerticesPerSide = "The number of vertices on the longest "
        "side (width or height) of the water surface. For example, a value "
        "50 for a square water plane would result in 50 x 50 = 2500 "
        "vertices for the entire plane. More vertices improves the "
        "appearance of fog on the water, but decreases performance.";
    ms_kPropDescDeepColor = "The color the water will tend towards as it "
        "approaches the depth threshold";
    ms_kPropDescShallowColor = "The color the water will tend towards as it "
        "becomes shallow";
    ms_kPropDescDepthThreshold = "Depth at which the water color becomes "
        "totally 'deep'";
    ms_kPropDescTextureSizeRatio = "Size of the dynamic texture. It must be "
        "greater than or equal to 16";
    ms_kPropDescSceneRoot = "Pointer to the root object";
    ms_kPropDescTexTransformVelocity = "Velocity vector indicating the "
        "direction and speed the texture (normal map only) will move at";
    ms_kPropDescEnvMap = "The environment map used for reflection";
    ms_kPropDescReflectionFactor = "Value between 0 and 1 defining the "
        "strength of the water reflection. 1 is mirror like and 0 is no "
        "reflection at all.";
    ms_kPropDescNormalMapDir = "Path to the directory where all the normal "
        "maps are stored";
    ms_kPropDescNormalMapFile = "The format of the normal map file names. All "
        "maps should have a name matching the mask, ie: filename_%d.dds, "
        "where '%d' is substituted with a sequential number, beginning at 1.";
    ms_kPropDescNormalLength = "Ratio for the normal texture repetition on "
        "the x axis";
    ms_kPropDescNormalWidth = "Ratio for the normal texture repetition on "
        "the y axis";
    ms_kPropDescDynamicEnvMap = "Sets the environment map to a specific "
        "texture. This is mainly used for dynamic maps.";
    ms_kPropDescRefractionMap = "This property sets the path to a static "
        "refraction map";
    ms_kPropDescDynamicRefraction = "Sets the refraction map to a specific "
        "texture. This is used when the refraction map is rendered "
        "dynamically";
    ms_kPropDescSpecularity = "This property is used in the calculation"
        " of the specular highlight on the water. It requires a directional "
        "light to affect the water. The greater the value is the less specular"
        " the water will be."; 
    ms_kPropDescDisturbance = "This property is used for the displacement"
        " of the texture coordinates interfering with reflection and "
        "refraction. When the disturbance is small, the water appears still. "
        "This only affects the water if the water uses dynamic planar "
        "reflection or refraction."; 
    ms_kPropDescFogDensity = "The fog strength influences the transparency of"
        " the water. It is used primarily for refraction calculation but "
        " replaces the alpha value in normal cases to change transparency "
        " according to the distance of teh camera from the water. It only "
        " varies between 1 and 0.";
    ms_kPropDescFogDistance = "Distance at which fog density is maximum.";
    ms_kPropDescFogFallOff = "Rate at which the fog density changes. "
        "A value of 1 is linear.";
    ms_kPropDescNormalAnimSpeed = "Normal animation speed in number of frames "
        "per second.";
}
//---------------------------------------------------------------------------
void NiWaterComponent::_SDMShutdown()
{
    ms_kClassName = NULL;
    ms_kComponentName = NULL;

    ms_kPropNameAssociatedTerrain = NULL;
    ms_kPropNameLength = NULL;
    ms_kPropNameWidth = NULL;
    ms_kPropNameMaxVerticesPerSide = NULL;
    ms_kPropNameDeepColor = NULL;
    ms_kPropNameShallowColor = NULL;
    ms_kPropNameDepthThreshold = NULL;
    ms_kPropNameTextureSizeRatio = NULL;
    ms_kPropNameSceneRoot = NULL;
    ms_kPropNameTexTransformVelocity = NULL;
    ms_kPropNameEnvMap = NULL;
    ms_kPropNameReflectionFactor = NULL;
    ms_kPropNameNormalMapDir = NULL;
    ms_kPropNameNormalMapFile = NULL;
    ms_kPropNameNormalLength = NULL;
    ms_kPropNameNormalWidth = NULL;
    ms_kPropNameDynamicEnvMap = NULL;
    ms_kPropNameRefractionMap = NULL;
    ms_kPropNameDynamicRefraction = NULL;
    ms_kPropNameSpecularity = NULL; 
    ms_kPropNameDisturbance = NULL;
    ms_kPropNameFogDensity = NULL;
    ms_kPropNameFogDistance = NULL;
    ms_kPropNameFogFallOff = NULL;
    ms_kPropNameNormalAnimSpeed = NULL;

    ms_kPropNameTranslation = NULL;
    ms_kPropNameRotation = NULL;
    ms_kPropNameScale = NULL;

    ms_kPropDescAssociatedTerrain = NULL;
    ms_kPropDescLength = NULL;
    ms_kPropDescWidth = NULL;
    ms_kPropDescMaxVerticesPerSide = NULL;
    ms_kPropDescDeepColor = NULL;
    ms_kPropDescShallowColor = NULL;
    ms_kPropDescDepthThreshold = NULL;
    ms_kPropDescTextureSizeRatio = NULL;
    ms_kPropDescSceneRoot = NULL;
    ms_kPropDescTexTransformVelocity = NULL;
    ms_kPropDescEnvMap = NULL;
    ms_kPropDescReflectionFactor = NULL;
    ms_kPropDescNormalMapDir = NULL;
    ms_kPropDescNormalMapFile = NULL;
    ms_kPropDescNormalLength = NULL;
    ms_kPropDescNormalWidth = NULL;
    ms_kPropDescDynamicEnvMap = NULL;
    ms_kPropDescRefractionMap = NULL;
    ms_kPropDescDynamicRefraction = NULL;
    ms_kPropDescSpecularity = NULL; 
    ms_kPropDescDisturbance = NULL;
    ms_kPropDescFogDensity = NULL;
    ms_kPropDescFogDistance = NULL;
    ms_kPropDescFogFallOff = NULL;
    ms_kPropDescNormalAnimSpeed = NULL;
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiWaterComponent::Clone(bool bInheritProperties)
{
    NiWaterComponent* pkClone = NiNew NiWaterComponent(this);
    if (!bInheritProperties)
    {
        pkClone->m_spMasterComponent = this->m_spMasterComponent;
    }
    return pkClone;
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiWaterComponent::GetMasterComponent() const
{
    return m_spMasterComponent;
}
//---------------------------------------------------------------------------
void NiWaterComponent::SetMasterComponent(
    NiEntityComponentInterface* pkMasterComponent)
{
    if (pkMasterComponent)
    {
        // We were given a master component
        NIASSERT(pkMasterComponent->GetClassName() == GetClassName());
        m_spMasterComponent = (NiWaterComponent*)pkMasterComponent;
    }
    else
    {
        // We were told not to have a master component any more; ensure all our
        // properties are unique
        NiTObjectSet<NiFixedString> kPropertyNames(5);
        GetPropertyNames(kPropertyNames);
        NiUInt32 uiSize = kPropertyNames.GetSize();
        for (NiUInt32 ui = 0; ui < uiSize; ui++)
        {
            bool bMadeUnique;
            NIVERIFY(MakePropertyUnique(kPropertyNames.GetAt(ui),
                bMadeUnique));
        }
        m_spMasterComponent = NULL;
    }
}
//---------------------------------------------------------------------------
void NiWaterComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>& kDependentPropertyNames)
{
    kDependentPropertyNames.Add(ms_kPropNameTranslation);
    kDependentPropertyNames.Add(ms_kPropNameRotation);
    kDependentPropertyNames.Add(ms_kPropNameScale);
    return;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetTemplateID(const NiUniqueID&)
{
    // Not supported for custom components
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID NiWaterComponent::GetTemplateID()
{
    static const NiUniqueID skUniqueID = 
        NiUniqueID(0x9E, 0x94, 0x61, 0x13, 0x28,0xD7, 0x99,0x42, 0xA2,0xE6,
        0x18, 0xDF, 0x02, 0x33, 0x0A, 0xEA);
    return skUniqueID;
}
//---------------------------------------------------------------------------
void NiWaterComponent::AddReference()
{
    IncRefCount();
}
//---------------------------------------------------------------------------
void NiWaterComponent::RemoveReference()
{
    DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiWaterComponent::GetClassName() const
{
    return ms_kClassName;
}
//---------------------------------------------------------------------------
NiFixedString NiWaterComponent::GetName() const
{
    return ms_kComponentName;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetName(const NiFixedString&)
{
    // This component does not allow its name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::IsAnimated() const
{
    // Return true to make sure the texture is staying up to date.
    return true;
}
//---------------------------------------------------------------------------
void NiWaterComponent::CheckForChangesInMasterComponent()
{
    // Use member variables to hold current value, if the property is 
    // inherited from master, than compare current value with the master, 
    // if it has changed, update the appropriate dirty flags to cause
    // the water to refresh.

    if (!GetBit(PROP_SHALLOW_COLOR_CHANGED))
    {
        if (m_kShallowColor != GetShallowColor())
        {
            m_kShallowColor = GetShallowColor();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_DEEP_COLOR_CHANGED))
    {
        if (m_kDeepColor != GetDeepColor())
        {
            m_kDeepColor = GetDeepColor();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_ENVMAP_CHANGED))
    {
        if (m_kEnvMap != GetEnvMap())
        {
            m_kEnvMap = GetEnvMap();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_DEPTH_CHANGED))
    {
        if (m_fDepthThreshold != GetDepthThreshold())
        {
            m_fDepthThreshold = GetDepthThreshold();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_NORMAL_DIR_CHANGED))
    {
        if (m_kNormalMapDir != GetNormalMapDir())
        {
            m_kNormalMapDir = GetNormalMapDir();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_NORMAL_NAME_CHANGED))
    {
        if (m_kNormalMapFile != GetNormalFile())
        {
            m_kNormalMapFile = GetNormalFile();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_TEXTURE_SIZE_CHANGED))
    {
        if (m_uiTextureSizeRatio != GetTextureSizeRatio())
        {
            m_uiTextureSizeRatio = GetTextureSizeRatio();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_NORMAL_WIDTH_CHANGED))
    {
        if (m_fNormalWidth != GetNormalWidth())
        {
            m_fNormalWidth = GetNormalWidth();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_NORMAL_LENGTH_CHANGED))
    {
        if (m_fNormalLength != GetNormalLength())
        {
            m_fNormalLength = GetNormalLength();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_REFLECTION_FACTOR_CHANGED))
    {
        if (m_fReflectionFactor != GetReflectionFactor())
        {
            m_fReflectionFactor = GetReflectionFactor();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_TERRAIN_CHANGED))
    {
        if (m_spAssociatedTerrain != GetAssociatedTerrain())
        {
            m_spAssociatedTerrain = GetAssociatedTerrain();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_REFRACTION_MAP))
    {
        if (m_kRefractionMap != GetRefractionMap())
        {
            m_kRefractionMap = GetRefractionMap();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_SPECULAR_VALUE_CHANGED))
    {
        if (m_fSpecularity != GetSpecularity())
        {
            m_fSpecularity = GetSpecularity();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_DISTURBANCE_VALUE_CHANGED))
    {
        if (m_fDisturbance != GetDisturbance())
        {
            m_fDisturbance = GetDisturbance();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_DISTURBANCE_VALUE_CHANGED))
    {
        if (m_fFogDensity != GetFogDensity())
        {
            m_fFogDensity = GetFogDensity();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_FOGDISTANCE_VALUE_CHANGED))
    {
        if (m_fFogDistance != GetFogDistance())
        {
            m_fFogDistance = GetFogDistance();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_FOGFALLOFF_VALUE_CHANGED))
    {
        if (m_fFogFallOff != GetFogFallOff())
        {
            m_fFogFallOff = GetFogFallOff();
            m_bPropertyChanged = true;
        }       
    }

    if (!GetBit(PROP_NORMAL_ANIMATION_SPEED_CHANGED))
    {
        if (m_fNormalAnimationSpeed != GetNormalAnimationSpeed())
        {
            m_fNormalAnimationSpeed = GetNormalAnimationSpeed();
            m_bPropertyChanged = true;
        }       
    }

    // The following are updated every frame and don't require
    // a dirty bit:
    // PROP_NORMAL_VELOCITY_CHANGED

    // Not settable in the UI:
    //   PROP_DYNAMIC_ENVMAP 
    //   PROP_DYNAMIC_REFRACTION_MAP
}
//---------------------------------------------------------------------------
void NiWaterComponent::Update(NiEntityPropertyInterface* pkParentEntity,
    float fTime, NiEntityErrorInterface* pkErrors, NiExternalAssetManager*)
{
    if (!m_spRootNode)
        return;

    //Check dirty flags and if changes have occured in master component
    if (GetMasterComponent())
    {
        CheckForChangesInMasterComponent();
    }

    // Update properties if necessary
    if (m_bPropertyChanged)
    {
        if (m_spAssociatedTerrain)
        {
            NiObject* pkTerrainObj;
            m_spAssociatedTerrain->GetPropertyData("Scene Root Pointer", 
                pkTerrainObj);
            NiTerrain* pkTerrain = NiDynamicCast(NiTerrain, pkTerrainObj);
            if (pkTerrain)
                m_spRootNode->SetAssociatedTerrain(pkTerrain);
        }
        
        m_spRootNode->SetShallowColor(m_kShallowColor);
        m_spRootNode->SetDeepColor(m_kDeepColor);
        m_spRootNode->SetDepthThreshold(m_fDepthThreshold);         
        m_spRootNode->SetLength(m_uiLength);
        m_spRootNode->SetWidth(m_uiWidth);
        m_spRootNode->SetMaxVerticesPerSide(m_uiMaxVerticesPerSide);        
#ifndef _WII
        m_spRootNode->SetEnvMap(m_kEnvMap);
        m_spRootNode->SetReflectionFactor(m_fReflectionFactor);
        m_spRootNode->SetRefractionMap(m_kRefractionMap);
        m_spRootNode->SetNormalFile(m_kNormalMapFile);
        m_spRootNode->SetNormalLength(m_fNormalLength);
        
        if (!m_spRootNode->SetNormalMapDir(m_kNormalMapDir))
        {
            if (pkErrors)
            {
                char acError[1024];
                NiSprintf(acError, 1024, "Directory \"%s\" does not exist.",
                    (const char*)m_kNormalMapDir);
                pkErrors->ReportError("Normal maps failed to load", acError,
                    NULL, NULL);
            }
            m_kNormalMapDir = m_spRootNode->GetNormalMapDir();
        }

        m_spRootNode->SetNormalWidth(m_fNormalWidth);
#endif
        m_spRootNode->SetTextureSizeRatio(m_uiTextureSizeRatio);
        m_spRootNode->SetMaxVerticesPerSide(m_uiMaxVerticesPerSide);
        m_spRootNode->SetSpecularity(m_fSpecularity);
        m_spRootNode->SetDisturbance(m_fDisturbance);
        m_spRootNode->SetFogDensity(m_fFogDensity);
        m_spRootNode->SetFogDistance(m_fFogDistance);
        m_spRootNode->SetFogFallOff(m_fFogFallOff);
        m_spRootNode->SetNormalAnimationSpeed(m_fNormalAnimationSpeed);
        m_spRootNode->SetNormalVelocity(m_kTexTransformVelocity);

        m_bPropertyChanged = false;
    }

    NiMatrix3 kRotate;
    float fScale;
    NiPoint3 kTranslation;

    NiPoint3 kOldTranslate = m_spRootNode->GetTranslate();
    NiMatrix3 kOldRotate = m_spRootNode->GetRotate();
    float fOldScale = m_spRootNode->GetScale();
    
    pkParentEntity->GetPropertyData(ms_kPropNameTranslation, kTranslation, 0);
    pkParentEntity->GetPropertyData(ms_kPropNameRotation, kRotate, 0);
    pkParentEntity->GetPropertyData(ms_kPropNameScale, fScale, 0);

    if (kOldTranslate != kTranslation ||




        kOldRotate != kRotate ||
        fOldScale != fScale)
    {
        m_spRootNode->SetTranslate(kTranslation);
        m_spRootNode->SetRotate(kRotate);
        m_spRootNode->SetScale(fScale);                
    }

    m_spRootNode->Update(fTime);
}
//---------------------------------------------------------------------------
void NiWaterComponent::BuildVisibleSet(NiEntityRenderingContext* 
    pkRenderingContext, NiEntityErrorInterface*)
{
    if (!m_spRootNode || !m_spRootNode->GetAt(0))
        return;
   
    // Cull this sector's section of the scene graph:
	NiCullScene(pkRenderingContext->m_pkCamera,
		m_spRootNode,
		*pkRenderingContext->m_pkCullingProcess,
		*pkRenderingContext->m_pkCullingProcess->GetVisibleSet(), false);
}
//---------------------------------------------------------------------------
void NiWaterComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const
{
    kPropertyNames.Add(ms_kPropNameAssociatedTerrain);
    kPropertyNames.Add(ms_kPropNameLength);
    kPropertyNames.Add(ms_kPropNameWidth);
    kPropertyNames.Add(ms_kPropNameMaxVerticesPerSide);
    kPropertyNames.Add(ms_kPropNameDeepColor);
    kPropertyNames.Add(ms_kPropNameDepthThreshold);
    kPropertyNames.Add(ms_kPropNameNormalMapDir);
    kPropertyNames.Add(ms_kPropNameNormalMapFile);
    kPropertyNames.Add(ms_kPropNameShallowColor);
    kPropertyNames.Add(ms_kPropNameTextureSizeRatio);
    kPropertyNames.Add(ms_kPropNameSceneRoot);
    kPropertyNames.Add(ms_kPropNameTexTransformVelocity);
    kPropertyNames.Add(ms_kPropNameEnvMap);
    kPropertyNames.Add(ms_kPropNameReflectionFactor);
    kPropertyNames.Add(ms_kPropNameNormalLength);
    kPropertyNames.Add(ms_kPropNameNormalWidth);
    kPropertyNames.Add(ms_kPropNameDynamicEnvMap);
    kPropertyNames.Add(ms_kPropNameRefractionMap);
    kPropertyNames.Add(ms_kPropNameDynamicRefraction);
    kPropertyNames.Add(ms_kPropNameSpecularity);
    kPropertyNames.Add(ms_kPropNameDisturbance);
    kPropertyNames.Add(ms_kPropNameFogDensity);
    kPropertyNames.Add(ms_kPropNameFogDistance);
    kPropertyNames.Add(ms_kPropNameFogFallOff);
    kPropertyNames.Add(ms_kPropNameNormalAnimSpeed);
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::CanResetProperty(const NiFixedString& kPropertyName,
    bool& bCanReset) const
{
    if (kPropertyName == ms_kPropNameAssociatedTerrain)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_TERRAIN_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameLength)
    {
        bCanReset = false;
    }
    else if (kPropertyName == ms_kPropNameWidth)
    {
        bCanReset = false;
    }
    else if (kPropertyName == ms_kPropNameMaxVerticesPerSide)
    {
        bCanReset = false;
    }
    else if (kPropertyName == ms_kPropNameDeepColor)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_DEEP_COLOR_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameDepthThreshold)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_DEPTH_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameShallowColor)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_SHALLOW_COLOR_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameNormalMapDir)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_NORMAL_DIR_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameNormalMapFile)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_NORMAL_NAME_CHANGED);
    }
    else if(kPropertyName == ms_kPropNameTextureSizeRatio)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_TEXTURE_SIZE_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameSceneRoot)
    {
        bCanReset = false;
    }
    else if (kPropertyName == ms_kPropNameTexTransformVelocity)
    {
        bCanReset = m_spMasterComponent && 
            GetBit(PROP_NORMAL_VELOCITY_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameEnvMap)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_ENVMAP_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameReflectionFactor)
    {
        bCanReset = m_spMasterComponent && 
            GetBit(PROP_REFLECTION_FACTOR_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameNormalLength)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_NORMAL_LENGTH_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameNormalWidth)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_NORMAL_WIDTH_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameDynamicEnvMap)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_DYNAMIC_ENVMAP);
    }
    else if (kPropertyName == ms_kPropNameDynamicRefraction)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_DYNAMIC_REFRACTION_MAP);
    }
    else if (kPropertyName == ms_kPropNameRefractionMap)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_REFRACTION_MAP);
    }
    else if (kPropertyName == ms_kPropNameSpecularity)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_SPECULAR_VALUE_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameDisturbance)
    {
        bCanReset = 
            m_spMasterComponent && GetBit(PROP_DISTURBANCE_VALUE_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameFogDensity)
    {
        bCanReset = 
            m_spMasterComponent && GetBit(PROP_FOGDENSITY_VALUE_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameFogDistance)
    {
        bCanReset = 
            m_spMasterComponent && GetBit(PROP_FOGDISTANCE_VALUE_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameFogFallOff)
    {
        bCanReset = 
            m_spMasterComponent && GetBit(PROP_FOGFALLOFF_VALUE_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        bCanReset = m_spMasterComponent && 
            GetBit(PROP_NORMAL_ANIMATION_SPEED_CHANGED);
    }
    else
        return false;

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::ResetProperty(const NiFixedString& kPropertyName)
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset) || !bCanReset)
    {
        return false;
    }

    if (kPropertyName == ms_kPropNameAssociatedTerrain)
    {
        SetAssociatedTerrain(
            m_spMasterComponent->GetAssociatedTerrain());
        SetBit(false, PROP_TERRAIN_CHANGED);        
    }
    else if (kPropertyName == ms_kPropNameDeepColor)
    {
        SetDeepColor(m_spMasterComponent->GetDeepColor());
        SetBit(false, PROP_DEEP_COLOR_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameDepthThreshold)
    {
        SetDepthThreshold(m_spMasterComponent->GetDepthThreshold());
        SetBit(false, PROP_DEPTH_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameShallowColor)
    {
        SetShallowColor(m_spMasterComponent->GetShallowColor());
        SetBit(false, PROP_SHALLOW_COLOR_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameNormalMapDir)
    {
        SetNormalMapDir(m_spMasterComponent->GetNormalMapDir());
        SetBit(false, PROP_NORMAL_DIR_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameNormalMapFile)
    {
        SetNormalFile(m_spMasterComponent->GetNormalFile());
        SetBit(false, PROP_NORMAL_NAME_CHANGED);
    }
    else if(kPropertyName == ms_kPropNameTextureSizeRatio)
    {
        SetTextureSizeRatio(
            m_spMasterComponent->GetTextureSizeRatio());
        SetBit(false, PROP_TEXTURE_SIZE_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameTexTransformVelocity)
    {
        SetNormalVelocity(
            m_spMasterComponent->GetNormalVelocity());
        SetBit(false, PROP_NORMAL_VELOCITY_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameEnvMap)
    {
        SetEnvMap(m_spMasterComponent->GetEnvMap());
        SetBit(false, PROP_ENVMAP_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameReflectionFactor)
    {
        SetReflectionFactor(m_spMasterComponent->GetReflectionFactor());
        SetBit(false, PROP_REFLECTION_FACTOR_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameNormalLength)
    {
        SetNormalLength(m_spMasterComponent->GetNormalLength());
        SetBit(false, PROP_NORMAL_LENGTH_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameNormalWidth)
    {
        SetNormalWidth(m_spMasterComponent->GetNormalWidth());
        SetBit(false, PROP_NORMAL_WIDTH_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameDynamicEnvMap)
    {
        SetDynamicEnvMap(m_spMasterComponent->GetDynamicEnvMap());
        SetBit(false, PROP_DYNAMIC_ENVMAP);
    }
    else if (kPropertyName == ms_kPropNameDynamicRefraction)
    {
        SetDynamicRefractionMap(
            m_spMasterComponent->GetDynamicRefractionMap());
        SetBit(false, PROP_DYNAMIC_REFRACTION_MAP);
    }
    else if (kPropertyName == ms_kPropNameRefractionMap)
    {
        SetRefractionMap(m_spMasterComponent->GetRefractionMap());
        SetBit(false, PROP_REFRACTION_MAP);
    }
    else if (kPropertyName == ms_kPropNameSpecularity)
    {
        SetSpecularity(m_spMasterComponent->GetSpecularity());
        SetBit(false, PROP_SPECULAR_VALUE_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameDisturbance)
    {
        SetDisturbance(m_spMasterComponent->GetDisturbance());
        SetBit(false, PROP_DISTURBANCE_VALUE_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameFogDensity)
    {
        SetFogDensity(m_spMasterComponent->GetFogDensity());
        SetBit(false, PROP_FOGDENSITY_VALUE_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameFogDistance)
    {
        SetFogDistance(m_spMasterComponent->GetFogDistance());
        SetBit(false, PROP_FOGDISTANCE_VALUE_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameFogFallOff)
    {
        SetFogFallOff(m_spMasterComponent->GetFogFallOff());
        SetBit(false, PROP_FOGFALLOFF_VALUE_CHANGED);
    }
    else if (kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        SetNormalAnimationSpeed(
            m_spMasterComponent->GetNormalAnimationSpeed());
        SetBit(false, PROP_NORMAL_ANIMATION_SPEED_CHANGED);
    }
    else
        return false;

    return true;

}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::MakePropertyUnique(
    const NiFixedString& kPropertyName, bool& bMadeUnique)
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset))
    {
        return false;
    }

    if (!bCanReset && m_spMasterComponent)
    {
        bMadeUnique = true;
        if (kPropertyName == ms_kPropNameAssociatedTerrain)
        {
            SetAssociatedTerrain(GetAssociatedTerrain());
            SetBit(true, PROP_TERRAIN_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameTexTransformVelocity)
        {
            SetNormalVelocity(GetNormalVelocity());
            SetBit(true, PROP_NORMAL_VELOCITY_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameNormalLength)
        {
            SetNormalLength(GetNormalLength());
            SetBit(true, PROP_NORMAL_LENGTH_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameNormalWidth)
        {
            SetNormalWidth(GetNormalWidth());
            SetBit(true, PROP_NORMAL_WIDTH_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameNormalMapDir)
        {
            SetNormalMapDir(GetNormalMapDir());
            SetBit(true, PROP_NORMAL_DIR_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameNormalMapFile)
        {
            SetNormalFile(GetNormalFile());
            SetBit(true, PROP_NORMAL_NAME_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameDeepColor)
        {
            SetDeepColor(GetDeepColor());
            SetBit(true, PROP_DEEP_COLOR_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameShallowColor)
        {
            SetShallowColor(GetShallowColor());
            SetBit(true, PROP_SHALLOW_COLOR_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameDepthThreshold)
        {
            SetDepthThreshold(GetDepthThreshold());
            SetBit(true, PROP_DEPTH_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameTextureSizeRatio)
        {
            SetTextureSizeRatio(GetTextureSizeRatio());
            SetBit(true, PROP_TEXTURE_SIZE_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameFogDensity)
        {
            SetFogDensity(GetFogDensity());
            SetBit(true, PROP_FOGDENSITY_VALUE_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameFogDistance)
        {
            SetFogDistance(GetFogDistance());
            SetBit(true, PROP_FOGDISTANCE_VALUE_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameFogFallOff)
        {
            SetFogFallOff(GetFogFallOff());
            SetBit(true, PROP_FOGFALLOFF_VALUE_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameDisturbance)
        {
            SetDisturbance(GetDisturbance());
            SetBit(true, PROP_DISTURBANCE_VALUE_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameEnvMap)
        {
            SetEnvMap(GetEnvMap());
            SetBit(true, PROP_ENVMAP_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameReflectionFactor)
        {
            SetReflectionFactor(GetReflectionFactor());
            SetBit(true, PROP_REFLECTION_FACTOR_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameSpecularity)
        {
            SetSpecularity(GetSpecularity());
            SetBit(true, PROP_SPECULAR_VALUE_CHANGED);
        }
        else if (kPropertyName == ms_kPropNameNormalAnimSpeed)
        {
            SetNormalAnimationSpeed(GetNormalAnimationSpeed());
            SetBit(true, PROP_NORMAL_ANIMATION_SPEED_CHANGED);
        }
        else
        {
            bMadeUnique = false;
        }
    }
    else
    {
        bMadeUnique = false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetDisplayName(const NiFixedString& kPropertyName,
    NiFixedString& kDisplayName) const
{
    if (kPropertyName == ms_kPropNameAssociatedTerrain ||
        kPropertyName == ms_kPropNameLength ||
        kPropertyName == ms_kPropNameWidth ||
        kPropertyName == ms_kPropNameMaxVerticesPerSide ||
        kPropertyName == ms_kPropNameDeepColor ||
        kPropertyName == ms_kPropNameDepthThreshold ||
        kPropertyName == ms_kPropNameNormalMapDir ||
        kPropertyName == ms_kPropNameNormalMapFile ||
        kPropertyName == ms_kPropNameShallowColor ||
        kPropertyName == ms_kPropNameTextureSizeRatio ||
        kPropertyName == ms_kPropNameTexTransformVelocity ||
        kPropertyName == ms_kPropNameEnvMap ||        
        kPropertyName == ms_kPropNameReflectionFactor ||
        kPropertyName == ms_kPropNameNormalLength ||
        kPropertyName == ms_kPropNameNormalWidth ||
        kPropertyName == ms_kPropNameSpecularity ||
        kPropertyName == ms_kPropNameDisturbance ||
        kPropertyName == ms_kPropNameFogDensity ||
        kPropertyName == ms_kPropNameFogDistance ||
        kPropertyName == ms_kPropNameFogFallOff ||
        kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        kDisplayName = kPropertyName;
    }
    else if (kPropertyName == ms_kPropNameSceneRoot ||
        kPropertyName == ms_kPropNameDynamicEnvMap ||
        kPropertyName == ms_kPropNameDynamicRefraction ||
        kPropertyName == ms_kPropNameRefractionMap)
    {
        kDisplayName = NULL;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetDisplayName(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the display name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetPrimitiveType(const NiFixedString& kPropertyName,
    NiFixedString& kPrimitiveType) const
{
    if (kPropertyName == ms_kPropNameDepthThreshold ||
        kPropertyName == ms_kPropNameReflectionFactor ||
        kPropertyName == ms_kPropNameNormalLength ||
        kPropertyName == ms_kPropNameNormalWidth ||
        kPropertyName == ms_kPropNameSpecularity ||
        kPropertyName == ms_kPropNameDisturbance ||
        kPropertyName == ms_kPropNameFogDensity ||
        kPropertyName == ms_kPropNameFogDistance ||
        kPropertyName == ms_kPropNameFogFallOff ||
        kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        kPrimitiveType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kPropNameDeepColor ||
        kPropertyName == ms_kPropNameShallowColor)
    {
        kPrimitiveType = PT_COLORA;
    }
    else if (kPropertyName == ms_kPropNameAssociatedTerrain)
    {
        kPrimitiveType = PT_ENTITYPOINTER;
    }
    else if (kPropertyName == ms_kPropNameNormalMapDir ||
        kPropertyName == ms_kPropNameNormalMapFile ||
        kPropertyName == ms_kPropNameEnvMap ||
        kPropertyName == ms_kPropNameRefractionMap)
    {
        kPrimitiveType = PT_STRING;
    }
    else if (kPropertyName == ms_kPropNameWidth ||
        kPropertyName == ms_kPropNameLength ||
        kPropertyName == ms_kPropNameMaxVerticesPerSide ||
        kPropertyName == ms_kPropNameTextureSizeRatio)
    {
        kPrimitiveType = PT_UINT;
    }
    else if (kPropertyName == ms_kPropNameSceneRoot ||
        kPropertyName == ms_kPropNameDynamicEnvMap ||
        kPropertyName == ms_kPropNameDynamicRefraction)
    {
        kPrimitiveType = PT_NIOBJECTPOINTER;
    }
    else if (kPropertyName == ms_kPropNameTexTransformVelocity)
    {
        kPrimitiveType = PT_POINT2;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetPrimitiveType(const NiFixedString&,
    const NiFixedString&)
{
    // Do not allow the setting of primitive types
    return false;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetSemanticType(const NiFixedString& kPropertyName,
    NiFixedString& kSemanticType) const
{
    if (kPropertyName == ms_kPropNameDepthThreshold ||
        kPropertyName == ms_kPropNameReflectionFactor ||
        kPropertyName == ms_kPropNameNormalLength ||
        kPropertyName == ms_kPropNameNormalWidth ||
        kPropertyName == ms_kPropNameSpecularity ||
        kPropertyName == ms_kPropNameDisturbance ||
        kPropertyName == ms_kPropNameFogDensity ||
        kPropertyName == ms_kPropNameFogDistance ||
        kPropertyName == ms_kPropNameFogFallOff ||
        kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        kSemanticType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kPropNameDeepColor ||
        kPropertyName == ms_kPropNameShallowColor)
    {
        kSemanticType = PT_COLORA;
    }
    else if (kPropertyName == ms_kPropNameAssociatedTerrain)
    {
        kSemanticType = PT_ENTITYPOINTER;
    }
    else if (kPropertyName == ms_kPropNameEnvMap ||
        kPropertyName == ms_kPropNameRefractionMap)
    {
        kSemanticType = "Image file";
    }
    else if (kPropertyName == ms_kPropNameNormalMapFile)
    {
        kSemanticType = PT_STRING;
    }
    else if (kPropertyName == ms_kPropNameNormalMapDir)
    {
        kSemanticType = "Directory Path";
    }
    else if (kPropertyName == ms_kPropNameWidth ||
        kPropertyName == ms_kPropNameLength ||
        kPropertyName == ms_kPropNameMaxVerticesPerSide)
    {
        kSemanticType = PT_UINT;
    }
    else if (kPropertyName == ms_kPropNameTextureSizeRatio)
    {
        kSemanticType = "Texture Size";
    }
    else if (kPropertyName == ms_kPropNameSceneRoot ||
        kPropertyName == ms_kPropNameDynamicEnvMap ||
        kPropertyName == ms_kPropNameDynamicRefraction)
    {   
        kSemanticType = PT_NIOBJECTPOINTER;
    }
    else if (kPropertyName == ms_kPropNameTexTransformVelocity)
    {
        kSemanticType = PT_POINT2;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetSemanticType(const NiFixedString&,
    const NiFixedString&)
{
    // Do not allow the setting of semantic types
    return false;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetDescription(const NiFixedString& kPropertyName,
    NiFixedString& kDescription) const
{
    if (kPropertyName == ms_kPropNameAssociatedTerrain)
        kDescription = ms_kPropDescAssociatedTerrain;
    else if (kPropertyName == ms_kPropNameLength)
        kDescription = ms_kPropDescLength;
    else if (kPropertyName == ms_kPropNameWidth)
        kDescription = ms_kPropDescWidth;
    else if (kPropertyName == ms_kPropNameMaxVerticesPerSide)
        kDescription = ms_kPropDescMaxVerticesPerSide;
    else if (kPropertyName == ms_kPropNameDeepColor)
        kDescription = ms_kPropDescDeepColor;
    else if (kPropertyName == ms_kPropNameDepthThreshold)
        kDescription = ms_kPropDescDepthThreshold;
    else if (kPropertyName == ms_kPropNameShallowColor)
        kDescription = ms_kPropDescShallowColor;
    else if (kPropertyName == ms_kPropNameNormalMapDir)
        kDescription = ms_kPropDescNormalMapDir;
    else if (kPropertyName == ms_kPropNameNormalMapFile)
        kDescription = ms_kPropDescNormalMapFile;
    else if(kPropertyName == ms_kPropNameTextureSizeRatio)
        kDescription = ms_kPropDescTextureSizeRatio;
    else if (kPropertyName == ms_kPropNameSceneRoot)
        kDescription = ms_kPropDescSceneRoot;
    else if (kPropertyName == ms_kPropNameTexTransformVelocity)
        kDescription = ms_kPropDescTexTransformVelocity;
    else if (kPropertyName == ms_kPropNameEnvMap)
        kDescription = ms_kPropDescEnvMap;
    else if (kPropertyName == ms_kPropNameReflectionFactor)
        kDescription = ms_kPropDescReflectionFactor;
    else if (kPropertyName == ms_kPropNameNormalLength)
        kDescription = ms_kPropDescNormalLength;
    else if (kPropertyName == ms_kPropNameNormalWidth)
        kDescription = ms_kPropDescNormalWidth;
    else if (kPropertyName == ms_kPropNameDynamicEnvMap)
        kDescription = ms_kPropDescDynamicEnvMap;
    else if (kPropertyName == ms_kPropNameDynamicRefraction)
        kDescription = ms_kPropDescDynamicRefraction;
    else if (kPropertyName == ms_kPropNameRefractionMap)
        kDescription = ms_kPropDescRefractionMap;
    else if (kPropertyName == ms_kPropNameSpecularity)
        kDescription = ms_kPropDescSpecularity;
    else if (kPropertyName == ms_kPropNameDisturbance)
        kDescription = ms_kPropDescDisturbance;
    else if (kPropertyName == ms_kPropNameFogDensity)
        kDescription = ms_kPropDescFogDensity;
    else if (kPropertyName == ms_kPropNameFogDistance)
        kDescription = ms_kPropDescFogDistance;
    else if (kPropertyName == ms_kPropNameFogFallOff)
        kDescription = ms_kPropDescFogFallOff;
    else if (kPropertyName == ms_kPropNameNormalAnimSpeed)
        kDescription = ms_kPropDescNormalAnimSpeed;
    else
        return false;

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetDescription(const NiFixedString&,
    const NiFixedString&)
{
    // Do not allow the setting of descriptions
    return false;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetCategory(const NiFixedString& kPropertyName,
    NiFixedString& kCategory) const
{
    if (kPropertyName == ms_kPropNameAssociatedTerrain ||
        kPropertyName == ms_kPropNameLength ||
        kPropertyName == ms_kPropNameWidth ||        
        kPropertyName == ms_kPropNameMaxVerticesPerSide ||
        kPropertyName == ms_kPropNameSceneRoot ||        
        kPropertyName == ms_kPropNameRefractionMap
        )
    {
        kCategory = ms_kComponentName;
    }
    else if (kPropertyName == ms_kPropNameNormalMapDir ||
        kPropertyName == ms_kPropNameNormalMapFile ||
        kPropertyName == ms_kPropNameNormalLength ||
        kPropertyName == ms_kPropNameNormalWidth ||
        kPropertyName == ms_kPropNameTexTransformVelocity ||
        kPropertyName == ms_kPropNameNormalAnimSpeed
        )
    {
        kCategory = "Water normal maps";
    }
    else if (kPropertyName == ms_kPropNameEnvMap ||
        kPropertyName == ms_kPropNameReflectionFactor ||
        kPropertyName == ms_kPropNameDynamicEnvMap ||
        kPropertyName == ms_kPropNameDynamicRefraction ||
        kPropertyName == ms_kPropNameSpecularity ||        
        kPropertyName == ms_kPropNameDisturbance)
    {
        kCategory = "Water reflection";
    }
    else if (kPropertyName == ms_kPropNameFogDensity ||
        kPropertyName == ms_kPropNameFogDistance ||
        kPropertyName == ms_kPropNameFogFallOff)
    {
        kCategory = "Water fog";
    }
    else if (kPropertyName == ms_kPropNameDeepColor ||
        kPropertyName == ms_kPropNameDepthThreshold ||        
        kPropertyName == ms_kPropNameShallowColor ||
        kPropertyName == ms_kPropNameTextureSizeRatio)
    {
        kCategory = "Water color map";
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::IsPropertyReadOnly(const NiFixedString& kPropertyName,
    bool& bIsReadOnly)
{
    if (kPropertyName == ms_kPropNameAssociatedTerrain ||
        kPropertyName == ms_kPropNameLength ||
        kPropertyName == ms_kPropNameWidth ||
        kPropertyName == ms_kPropNameMaxVerticesPerSide ||
        kPropertyName == ms_kPropNameDeepColor ||
        kPropertyName == ms_kPropNameDepthThreshold ||
        kPropertyName == ms_kPropNameNormalMapDir ||
        kPropertyName == ms_kPropNameNormalMapFile ||
        kPropertyName == ms_kPropNameShallowColor ||
        kPropertyName == ms_kPropNameTextureSizeRatio ||
        kPropertyName == ms_kPropNameTexTransformVelocity ||
        kPropertyName == ms_kPropNameEnvMap ||
        kPropertyName == ms_kPropNameReflectionFactor ||
        kPropertyName == ms_kPropNameNormalLength ||
        kPropertyName == ms_kPropNameNormalWidth ||
        kPropertyName == ms_kPropNameDynamicEnvMap ||
        kPropertyName == ms_kPropNameDynamicRefraction ||
        kPropertyName == ms_kPropNameRefractionMap ||
        kPropertyName == ms_kPropNameSpecularity ||
        kPropertyName == ms_kPropNameDisturbance ||
        kPropertyName == ms_kPropNameFogDensity ||
        kPropertyName == ms_kPropNameFogDistance ||
        kPropertyName == ms_kPropNameFogFallOff ||
        kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        bIsReadOnly = false;
    }
    else if (kPropertyName == ms_kPropNameSceneRoot)
    {
        bIsReadOnly = true;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::IsPropertyUnique(const NiFixedString& kPropertyName,
    bool& bIsUnique)
{
    if (kPropertyName == ms_kPropNameAssociatedTerrain ||
        kPropertyName == ms_kPropNameDeepColor ||
        kPropertyName == ms_kPropNameDepthThreshold ||
        kPropertyName == ms_kPropNameNormalMapDir ||
        kPropertyName == ms_kPropNameNormalMapFile ||
        kPropertyName == ms_kPropNameShallowColor ||
        kPropertyName == ms_kPropNameTextureSizeRatio||
        kPropertyName == ms_kPropNameSceneRoot ||
        kPropertyName == ms_kPropNameTexTransformVelocity ||
        kPropertyName == ms_kPropNameEnvMap ||
        kPropertyName == ms_kPropNameReflectionFactor ||
        kPropertyName == ms_kPropNameNormalLength ||
        kPropertyName == ms_kPropNameNormalWidth ||
        kPropertyName == ms_kPropNameDynamicEnvMap ||
        kPropertyName == ms_kPropNameDynamicRefraction ||
        kPropertyName == ms_kPropNameRefractionMap ||
        kPropertyName == ms_kPropNameSpecularity ||
        kPropertyName == ms_kPropNameDisturbance ||
        kPropertyName == ms_kPropNameFogDensity ||
        kPropertyName == ms_kPropNameFogDistance ||
        kPropertyName == ms_kPropNameFogFallOff ||
        kPropertyName == ms_kPropNameNormalAnimSpeed) 
    {
        if (m_spMasterComponent)
        {
            NIVERIFY(CanResetProperty(kPropertyName, bIsUnique));
        }
        else
        {
            bIsUnique = true;
        }
    }
    else if (kPropertyName == ms_kPropNameLength ||
        kPropertyName == ms_kPropNameWidth ||
        kPropertyName == ms_kPropNameMaxVerticesPerSide)
    {
        bIsUnique = true;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::IsPropertySerializable(
    const NiFixedString& kPropertyName, bool& bIsSerializable)
{
    if (kPropertyName == ms_kPropNameAssociatedTerrain ||
        kPropertyName == ms_kPropNameLength ||
        kPropertyName == ms_kPropNameWidth ||
        kPropertyName == ms_kPropNameMaxVerticesPerSide ||
        kPropertyName == ms_kPropNameDeepColor ||
        kPropertyName == ms_kPropNameDepthThreshold ||
        kPropertyName == ms_kPropNameNormalMapDir ||
        kPropertyName == ms_kPropNameNormalMapFile ||
        kPropertyName == ms_kPropNameShallowColor ||
        kPropertyName == ms_kPropNameTextureSizeRatio||
        kPropertyName == ms_kPropNameTexTransformVelocity ||        
        kPropertyName == ms_kPropNameReflectionFactor ||
        kPropertyName == ms_kPropNameEnvMap ||
        kPropertyName == ms_kPropNameNormalLength ||
        kPropertyName == ms_kPropNameNormalWidth ||
        kPropertyName == ms_kPropNameRefractionMap ||
        kPropertyName == ms_kPropNameSpecularity ||
        kPropertyName == ms_kPropNameDisturbance ||
        kPropertyName == ms_kPropNameFogDensity ||
        kPropertyName == ms_kPropNameFogDistance ||
        kPropertyName == ms_kPropNameFogFallOff ||
        kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        bIsSerializable = true;
    }
    else if (kPropertyName == ms_kPropNameSceneRoot ||
        kPropertyName == ms_kPropNameDynamicEnvMap ||
        kPropertyName == ms_kPropNameDynamicRefraction)
    {
        bIsSerializable = false;
        
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable)
{
    if (kPropertyName == ms_kPropNameAssociatedTerrain ||
        kPropertyName == ms_kPropNameDeepColor ||
        kPropertyName == ms_kPropNameDepthThreshold ||
        kPropertyName == ms_kPropNameNormalMapDir ||
        kPropertyName == ms_kPropNameNormalMapFile ||
        kPropertyName == ms_kPropNameShallowColor ||
        kPropertyName == ms_kPropNameTextureSizeRatio||
        kPropertyName == ms_kPropNameTexTransformVelocity ||
        kPropertyName == ms_kPropNameEnvMap ||
        kPropertyName == ms_kPropNameReflectionFactor ||
        kPropertyName == ms_kPropNameNormalLength ||
        kPropertyName == ms_kPropNameNormalWidth ||
        kPropertyName == ms_kPropNameDynamicEnvMap ||
        kPropertyName == ms_kPropNameDynamicRefraction ||
        kPropertyName == ms_kPropNameRefractionMap ||
        kPropertyName == ms_kPropNameSpecularity ||
        kPropertyName == ms_kPropNameDisturbance ||
        kPropertyName == ms_kPropNameFogDensity ||
        kPropertyName == ms_kPropNameFogDistance ||
        kPropertyName == ms_kPropNameFogFallOff ||
        kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        bIsInheritable = true;
    }
    else if (kPropertyName == ms_kPropNameSceneRoot ||
        kPropertyName == ms_kPropNameLength ||
        kPropertyName == ms_kPropNameWidth ||
        kPropertyName == ms_kPropNameMaxVerticesPerSide)
    {
        bIsInheritable = false;
        
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName, NiUInt32, 
    bool& bIsExternalAssetPath) const
{
    if (kPropertyName == ms_kPropNameAssociatedTerrain ||
        kPropertyName == ms_kPropNameLength ||
        kPropertyName == ms_kPropNameWidth ||
        kPropertyName == ms_kPropNameMaxVerticesPerSide ||
        kPropertyName == ms_kPropNameDeepColor ||
        kPropertyName == ms_kPropNameDepthThreshold ||
        kPropertyName == ms_kPropNameNormalMapFile ||
        kPropertyName == ms_kPropNameShallowColor ||
        kPropertyName == ms_kPropNameTextureSizeRatio||
        kPropertyName == ms_kPropNameSceneRoot ||
        kPropertyName == ms_kPropNameTexTransformVelocity ||
        kPropertyName == ms_kPropNameReflectionFactor ||
        kPropertyName == ms_kPropNameNormalLength ||
        kPropertyName == ms_kPropNameNormalWidth ||
        kPropertyName == ms_kPropNameDynamicEnvMap ||
        kPropertyName == ms_kPropNameDynamicRefraction ||
        kPropertyName == ms_kPropNameSpecularity ||
        kPropertyName == ms_kPropNameDisturbance ||
        kPropertyName == ms_kPropNameFogDensity ||
        kPropertyName == ms_kPropNameFogDistance ||
        kPropertyName == ms_kPropNameFogFallOff ||
        kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        bIsExternalAssetPath = false;
    }
    else if (kPropertyName == ms_kPropNameNormalMapDir ||
        kPropertyName == ms_kPropNameEnvMap ||
        kPropertyName == ms_kPropNameRefractionMap)
    {
        bIsExternalAssetPath = true;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetElementCount(const NiFixedString& kPropertyName,
    NiUInt32, bool &bCountSet)
{
    if (kPropertyName == ms_kPropNameAssociatedTerrain ||
        kPropertyName == ms_kPropNameLength ||
        kPropertyName == ms_kPropNameWidth ||
        kPropertyName == ms_kPropNameMaxVerticesPerSide ||
        kPropertyName == ms_kPropNameDeepColor ||
        kPropertyName == ms_kPropNameDepthThreshold ||
        kPropertyName == ms_kPropNameShallowColor ||
        kPropertyName == ms_kPropNameTextureSizeRatio||
        kPropertyName == ms_kPropNameSceneRoot ||
        kPropertyName == ms_kPropNameTexTransformVelocity ||
        kPropertyName == ms_kPropNameEnvMap ||
        kPropertyName == ms_kPropNameNormalMapDir ||
        kPropertyName == ms_kPropNameNormalMapFile ||
        kPropertyName == ms_kPropNameReflectionFactor ||
        kPropertyName == ms_kPropNameNormalLength ||
        kPropertyName == ms_kPropNameNormalWidth ||
        kPropertyName == ms_kPropNameDynamicEnvMap ||
        kPropertyName == ms_kPropNameDynamicRefraction ||
        kPropertyName == ms_kPropNameRefractionMap ||
        kPropertyName == ms_kPropNameSpecularity ||
        kPropertyName == ms_kPropNameDisturbance ||
        kPropertyName == ms_kPropNameFogDensity ||
        kPropertyName == ms_kPropNameFogDistance ||
        kPropertyName == ms_kPropNameFogFallOff ||
        kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        bCountSet = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetElementCount(const NiFixedString& kPropertyName,
    NiUInt32& uiCount) const
{
    if (kPropertyName == ms_kPropNameAssociatedTerrain ||
        kPropertyName == ms_kPropNameLength ||
        kPropertyName == ms_kPropNameWidth ||
        kPropertyName == ms_kPropNameMaxVerticesPerSide ||
        kPropertyName == ms_kPropNameDeepColor ||
        kPropertyName == ms_kPropNameDepthThreshold ||
        kPropertyName == ms_kPropNameShallowColor ||
        kPropertyName == ms_kPropNameTextureSizeRatio||
        kPropertyName == ms_kPropNameSceneRoot ||
        kPropertyName == ms_kPropNameTexTransformVelocity ||
        kPropertyName == ms_kPropNameEnvMap ||
        kPropertyName == ms_kPropNameReflectionFactor ||
        kPropertyName == ms_kPropNameNormalMapDir ||
        kPropertyName == ms_kPropNameNormalMapFile ||
        kPropertyName == ms_kPropNameNormalLength ||
        kPropertyName == ms_kPropNameNormalWidth ||
        kPropertyName == ms_kPropNameDynamicEnvMap ||
        kPropertyName == ms_kPropNameDynamicRefraction ||
        kPropertyName == ms_kPropNameRefractionMap ||
        kPropertyName == ms_kPropNameSpecularity ||
        kPropertyName == ms_kPropNameDisturbance ||
        kPropertyName == ms_kPropNameFogDensity ||
        kPropertyName == ms_kPropNameFogDistance ||
        kPropertyName == ms_kPropNameFogFallOff ||
        kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        uiCount = 1;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::IsCollection(const NiFixedString& kPropertyName,
    bool& bIsCollection) const
{
    if (kPropertyName == ms_kPropNameAssociatedTerrain ||
        kPropertyName == ms_kPropNameLength ||
        kPropertyName == ms_kPropNameWidth ||
        kPropertyName == ms_kPropNameMaxVerticesPerSide ||
        kPropertyName == ms_kPropNameDeepColor ||
        kPropertyName == ms_kPropNameDepthThreshold ||
        kPropertyName == ms_kPropNameShallowColor ||
        kPropertyName == ms_kPropNameTextureSizeRatio ||
        kPropertyName == ms_kPropNameSceneRoot ||
        kPropertyName == ms_kPropNameTexTransformVelocity ||
        kPropertyName == ms_kPropNameEnvMap ||
        kPropertyName == ms_kPropNameReflectionFactor ||
        kPropertyName == ms_kPropNameNormalMapDir ||
        kPropertyName == ms_kPropNameNormalMapFile ||
        kPropertyName == ms_kPropNameNormalLength ||
        kPropertyName == ms_kPropNameNormalWidth ||
        kPropertyName == ms_kPropNameDynamicEnvMap ||
        kPropertyName == ms_kPropNameDynamicRefraction ||
        kPropertyName == ms_kPropNameRefractionMap ||
        kPropertyName == ms_kPropNameSpecularity ||
        kPropertyName == ms_kPropNameDisturbance ||
        kPropertyName == ms_kPropNameFogDensity ||
        kPropertyName == ms_kPropNameFogDistance ||
        kPropertyName == ms_kPropNameFogFallOff ||
        kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        bIsCollection = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiFixedString& kData, NiUInt32 uiIndex) const
{
    if (uiIndex > 0)
        return false;

    if (kPropertyName == ms_kPropNameNormalMapDir)
    {
        kData = GetNormalMapDir();
    }
    else if (kPropertyName == ms_kPropNameNormalMapFile)
    {
        kData = GetNormalFile();
    }
    else if (kPropertyName == ms_kPropNameEnvMap)
    {
        kData = GetEnvMap();
    }
    else if (kPropertyName == ms_kPropNameRefractionMap)
    {
        kData = GetRefractionMap();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiFixedString& kData, NiUInt32 uiIndex)
{
    if (uiIndex > 0)
        return false;

    if (kPropertyName == ms_kPropNameNormalMapDir)
    {
        SetNormalMapDir(kData);   
    }
    else if (kPropertyName == ms_kPropNameNormalMapFile)
    {
        SetNormalFile(kData);
    }
    else if (kPropertyName == ms_kPropNameEnvMap)
    {
        SetEnvMap(kData);
    }
    else if (kPropertyName == ms_kPropNameRefractionMap)
    {
        SetRefractionMap(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetPropertyData(const NiFixedString& kPropertyName,
    float& kData, NiUInt32 uiIndex) const
{
    if (uiIndex > 0)
        return false;

    if (kPropertyName == ms_kPropNameDepthThreshold)
    {
        kData = GetDepthThreshold();
        
    }
    else if (kPropertyName == ms_kPropNameReflectionFactor)
    {
        kData = GetReflectionFactor();
    }
    else if (kPropertyName == ms_kPropNameNormalLength)
    {
        kData = GetNormalLength();
    }
    else if (kPropertyName == ms_kPropNameNormalWidth)
    {
        kData = GetNormalWidth();
    }
    else if (kPropertyName == ms_kPropNameSpecularity)
    {
        kData = GetSpecularity();
    }
    else if (kPropertyName == ms_kPropNameDisturbance)
    {
        kData = GetDisturbance();
    }
    else if (kPropertyName == ms_kPropNameFogDensity)
    {
        kData = GetFogDensity();
    }
    else if (kPropertyName == ms_kPropNameFogDistance)
    {
        kData = GetFogDistance();
    }
    else if (kPropertyName == ms_kPropNameFogFallOff)
    {
        kData = GetFogFallOff();
    }
    else if (kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        kData = GetNormalAnimationSpeed();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetPropertyData(const NiFixedString& kPropertyName,
    float kData, NiUInt32 uiIndex)
{
    if (uiIndex > 0)
        return false;
    
    if (kPropertyName == ms_kPropNameDepthThreshold)
    {
        SetDepthThreshold(kData);
    }
    else if (kPropertyName == ms_kPropNameReflectionFactor)
    {
        SetReflectionFactor(kData);
    }
    else if (kPropertyName == ms_kPropNameNormalLength)
    {
        SetNormalLength(kData);     
    }
    else if (kPropertyName == ms_kPropNameNormalWidth)
    {
        SetNormalWidth(kData);
    }
    else if (kPropertyName == ms_kPropNameSpecularity)
    {
        SetSpecularity(kData);
    }
    else if (kPropertyName == ms_kPropNameDisturbance)
    {
        SetDisturbance(kData);
    }
    else if (kPropertyName == ms_kPropNameFogDensity) 
    {
        SetFogDensity(kData);
    }
    else if (kPropertyName == ms_kPropNameFogDistance) 
    {
        SetFogDistance(kData);
    }
    else if (kPropertyName == ms_kPropNameFogFallOff) 
    {
        SetFogFallOff(kData);
    }
    else if (kPropertyName == ms_kPropNameNormalAnimSpeed)
    {
        SetNormalAnimationSpeed(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiUInt32& kData, NiUInt32 uiIndex) const
{
    if (uiIndex > 0)
        return false;

    if (kPropertyName == ms_kPropNameWidth)        
    {
        kData = GetWidth();
       
    }
    else if (kPropertyName == ms_kPropNameLength)
    {
        kData = GetLength();
        
    }
    else if (kPropertyName == ms_kPropNameMaxVerticesPerSide)
    {
        kData = GetMaxVerticesPerSide();        
    }
    else if (kPropertyName == ms_kPropNameTextureSizeRatio)
    {
        kData = GetTextureSizeRatio();
        
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetPropertyData(const NiFixedString& kPropertyName,
    NiUInt32 kData, NiUInt32 uiIndex)
{
    if (uiIndex > 0)
        return false;

    if (kPropertyName == ms_kPropNameWidth)        
    {
        SetWidth(kData);
    }
    else if (kPropertyName == ms_kPropNameLength)
    {
        SetLength(kData);
    }
    else if (kPropertyName == ms_kPropNameMaxVerticesPerSide)
    {
        SetMaxVerticesPerSide(kData);
    }
    else if (kPropertyName == ms_kPropNameTextureSizeRatio)
    {
        SetTextureSizeRatio((NiUInt32)kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiColorA& kData, NiUInt32 uiIndex) const
{
    if (uiIndex > 0)
        return false;

    if (kPropertyName == ms_kPropNameDeepColor)
    {
        kData = GetDeepColor();
    }
    else if (kPropertyName == ms_kPropNameShallowColor)
    {
        kData = GetShallowColor();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetPropertyData(const NiFixedString& kPropertyName,
   const NiColorA& kData, NiUInt32 uiIndex)
{
    if (uiIndex > 0)
        return false;

    if (kPropertyName == ms_kPropNameDeepColor)
    {
        SetDeepColor(kData);
    }
    else if (kPropertyName == ms_kPropNameShallowColor)
    {
        SetShallowColor(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiEntityInterface*& pData, NiUInt32 uiIndex) const
{
    if (uiIndex > 0)
        return false;

    if (kPropertyName == ms_kPropNameAssociatedTerrain)
    {
        pData = GetAssociatedTerrain();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetPropertyData(const NiFixedString& kPropertyName,
    NiEntityInterface* pkData, NiUInt32 uiIndex)
{
    if (uiIndex > 0)
        return false;

    if (kPropertyName == ms_kPropNameAssociatedTerrain)
    {
        SetAssociatedTerrain(pkData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiObject*& pkData, NiUInt32 uiIndex) const
{
    if (uiIndex != 0) 
    {
        return false;
    }
    
    if (kPropertyName == ms_kPropNameSceneRoot)
    {
        if (m_spRootNode) 
        {
            pkData = m_spRootNode;
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (kPropertyName == ms_kPropNameDynamicEnvMap)
    {
        pkData = GetDynamicEnvMap();
        return true;
    }
    else if (kPropertyName == ms_kPropNameDynamicRefraction)
    {
        pkData = GetDynamicRefractionMap();
        return true;
    }
        
    return false;
    
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetPropertyData(const NiFixedString& kPropertyName,
    NiObject* pkData, NiUInt32 uiIndex)
{
    if (uiIndex != 0) 
    {
        return false;
    }

    if (kPropertyName == ms_kPropNameDynamicEnvMap)
    {
        SetDynamicEnvMap(NiDynamicCast(NiTexture, pkData));
        return true;
    }
    else if (kPropertyName == ms_kPropNameDynamicRefraction)
    {
        SetDynamicRefractionMap(NiDynamicCast(NiTexture, pkData));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetPropertyData(const NiFixedString& kPropertyName, 
    NiPoint2& kData, NiUInt32 uiIndex) const
{
    if (uiIndex > 0)
        return false;

    if (kPropertyName == ms_kPropNameTexTransformVelocity)
    {
        kData = GetNormalVelocity();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetPropertyData(const NiFixedString& kPropertyName, 
    const NiPoint2& kData, NiUInt32 uiIndex)
{
    if (uiIndex > 0)
        return false;

    if (kPropertyName == ms_kPropNameTexTransformVelocity)
    {
        SetNormalVelocity(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetPropertyData(const NiFixedString&, 
    bool&, NiUInt32) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetPropertyData(const NiFixedString&, 
    bool, NiUInt32)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::GetPropertyData(const NiFixedString&, 
        void*&, size_t&, NiUInt32) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiWaterComponent::SetPropertyData(const NiFixedString&, 
    const void*, size_t, NiUInt32)
{
    return false;
}
//---------------------------------------------------------------------------
void NiWaterComponent::SetAssociatedTerrain(
    NiEntityInterface* pkTerrainEntity)
{
    NiTerrainComponentPtr pkTerrainComponent =
        NiTerrainComponent::FindTerrainComponent(pkTerrainEntity);

    // The TerrainEntity can be NULL; in this case, it isn't required to 
    // have a Terrain Component.
    if (pkTerrainEntity && !pkTerrainComponent)
    {
        NiMessageBox("The selected entity does not contain a Terrain "
        " component.",
        "Water Component Editor");
        return;
    }

    if (GetAssociatedTerrain() != pkTerrainEntity)
    {
        if (pkTerrainEntity)
        {
            SetBit(true, PROP_TERRAIN_CHANGED);
            m_spAssociatedTerrain = pkTerrainEntity;
            m_bPropertyChanged = true;

        }
        else
        {
            m_spAssociatedTerrain = 0;
            m_bPropertyChanged = true;
        }
    }    
}
//---------------------------------------------------------------------------
void NiWaterComponent::SetMaxVerticesPerSide(
    NiUInt32 uiMaxVerticesPerSide)
{
    
    float fLongestSide = static_cast<float>(NiMax((int)GetWidth(),
        (int)GetLength()));

    NiUInt32 uiNumVerticesLength = static_cast<NiUInt32>(
        (float)uiMaxVerticesPerSide * ((float)GetLength() / fLongestSide));

    NiUInt32 uiNumVerticesWidth = static_cast<NiUInt32>(
        (float)uiMaxVerticesPerSide * ((float)GetWidth() / fLongestSide));

    if (uiMaxVerticesPerSide > 1 &&
        uiNumVerticesLength > 1 &&
        uiNumVerticesWidth > 1 &&
        (GetMaxVerticesPerSide() != uiMaxVerticesPerSide ||
        GetNumLengthVertices() != uiNumVerticesLength ||
        GetNumWidthVertices() != uiNumVerticesWidth))
    {
        m_uiMaxVerticesPerSide = uiMaxVerticesPerSide;
        m_bPropertyChanged = true;
        SetBit(true, PROP_MAX_VERTICES_PER_SIDE_CHANGED);
    }
}
//---------------------------------------------------------------------------
