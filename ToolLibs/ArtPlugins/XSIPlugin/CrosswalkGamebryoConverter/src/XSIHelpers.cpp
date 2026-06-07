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

#include "CrosswalkGamebryoHelpers.h"

#include "Model.h"
#include "Material.h"
#include "Visibility.h"
#include "FCurve.h"

namespace
{
    //---------------------------------------------------------------------------
    // Local helper functions.
    //---------------------------------------------------------------------------

    const char * GetTypeName(CSLTemplate::ETemplateType tmplType)
    {
        switch (tmplType)
        {
            case CSLTemplate::SI_ACTION_FCURVE:
                return "SI_ACTION_FCURVE";
            case CSLTemplate::SI_ANGLE: 
                return "SI_ANGLE";
            case CSLTemplate::SI_AMBIENCE: 
                return "SI_AMBIENCE";
            case CSLTemplate::SI_BASE_SHAPE: 
                return "SI_BASE_SHAPE";
            case CSLTemplate::SI_CAMERA: 
                return "SI_CAMERA";
            case CSLTemplate::SI_CLUSTER: 
                return "SI_CLUSTER";
            case CSLTemplate::SI_CONSTRAINT: 
                return "SI_CONSTRAINT";
            case CSLTemplate::SI_COORDINATE_SYSTEM: 
                return "SI_COORDINATE_SYSTEM";
            case CSLTemplate::SI_CURVE_LIST: 
                return "SI_CURVE_LIST";
            case CSLTemplate::SI_CUSTOM_PSET: 
                return "SI_CUSTOM_PSET";
            case CSLTemplate::SI_CUSTOM_PARAM_INFO: 
                return "SI_CUSTOM_PARAM_INFO";
            case CSLTemplate::SI_DIRECTIONAL_LIGHT: 
                return "SI_DIRECTIONAL_LIGHT";
            case CSLTemplate::SI_ENVELOPE: 
                return "SI_ENVELOPE";
            case CSLTemplate::SI_ENVELOPE_LIST: 
                return "SI_ENVELOPE_LIST";
            case CSLTemplate::SI_FCURVE: 
                return "SI_FCURVE";
            case CSLTemplate::SI_FILE_INFO: 
                return "SI_FILE_INFO";
            case CSLTemplate::SI_FOG: 
                return "SI_FOG";
            case CSLTemplate::SI_GLOBAL_MATERIAL: 
                return "SI_GLOBAL_MATERIAL";
            case CSLTemplate::SI_IK_EFFECTOR: 
                return "SI_IK_EFFECTOR";
            case CSLTemplate::SI_IK_JOINT: 
                return "SI_IK_JOINT";
            case CSLTemplate::SI_IK_ROOT: 
                return "SI_IK_ROOT";
            case CSLTemplate::SI_INFINITE_LIGHT: 
                return "SI_INFINITE_LIGHT";
            case CSLTemplate::SI_INSTANCE: 
                return "SI_INSTANCE";
            case CSLTemplate::SI_LIGHT_INFO: 
                return "SI_LIGHT_INFO";
            case CSLTemplate::SI_MATERIAL: 
                return "SI_MATERIAL";
            case CSLTemplate::SI_MATERIAL_LIBRARY: 
                return "SI_MATERIAL_LIBRARY";
            case CSLTemplate::SI_MESH: 
                return "SI_MESH";
            case CSLTemplate::SI_MODEL: 
                return "SI_MODEL";
            case CSLTemplate::SI_NULL_OBJECT: 
                return "SI_NULL_OBJECT";
            case CSLTemplate::SI_NURBS_CURVE: 
                return "SI_NURBS_CURVE";
            case CSLTemplate::SI_NURBS_SURFACE: 
                return "SI_NURBS_SURFACE";
            case CSLTemplate::SI_POINT_LIGHT: 
                return "SI_POINT_LIGHT";
            case CSLTemplate::SI_POLYGON_LIST: 
                return "SI_POLYGON_LIST";
            case CSLTemplate::SI_SCENE_INFO: 
                return "SI_SCENE_INFO";
            case CSLTemplate::SI_SHAPE: 
                return "SI_SHAPE";
            case CSLTemplate::SI_SHAPE35: 
                return "SI_SHAPE35";
            case CSLTemplate::SI_SHAPE_ANIMATION: 
                return "SI_SHAPE_ANIMATION";
            case CSLTemplate::SI_SPOT_LIGHT: 
                return "SI_SPOT_LIGHT";
            case CSLTemplate::SI_SURFACE_MESH: 
                return "SI_SURFACE_MESH";
            case CSLTemplate::SI_TEXTURE_2D: 
                return "SI_TEXTURE_2D";
            case CSLTemplate::SI_TRANSFORM: 
                return "SI_TRANSFORM";
            case CSLTemplate::SI_TRIANGLE_LIST: 
                return "SI_TRIANGLE_LIST";
            case CSLTemplate::SI_TRIANGLE_STRIP: 
                return "SI_TRIANGLE_STRIP";
            case CSLTemplate::SI_TRIANGLE_STRIP_LIST: 
                return "SI_TRIANGLE_STRIP_LIST";
            case CSLTemplate::SI_VISIBILITY: 
                return "SI_VISIBILITY";
            case CSLTemplate::XSI_ACTION: 
                return "XSI_ACTION";
            case CSLTemplate::XSI_ACTIONCLIP: 
                return "XSI_ACTIONCLIP";
            case CSLTemplate::XSI_CUSTOM_PARAM_INFO: 
                return "XSI_CUSTOM_PARAM_INFO";
            case CSLTemplate::XSI_EXTRAPOLATION: 
                return "XSI_EXTRAPOLATION";
            case CSLTemplate::XSI_FXOPERATOR: 
                return "XSI_FXOPERATOR";
            case CSLTemplate::XSI_FXTREE: 
                return "XSI_FXTREE";
            case CSLTemplate::XSI_IMAGE: 
                return "XSI_IMAGE";
            case CSLTemplate::XSI_IMAGE_DATA: 
                return "XSI_IMAGE_DATA";
            case CSLTemplate::XSI_IMAGE_FX: 
                return "XSI_IMAGE_FX";
            case CSLTemplate::XSI_IMAGE_LIBRARY: 
                return "XSI_IMAGE_LIBRARY";
            case CSLTemplate::XSI_MATERIAL: 
                return "XSI_MATERIAL";
            case CSLTemplate::XSI_MATERIAL_INFO: 
                return "XSI_MATERIAL_INFO";
            case CSLTemplate::XSI_MIXER: 
                return "XSI_MIXER";
            case CSLTemplate::XSI_NURBS_PROJECTION: 
                return "XSI_NURBS_PROJECTION";
            case CSLTemplate::XSI_NURBS_TRIM: 
                return "XSI_NURBS_TRIM";
            case CSLTemplate::XSI_SHADER: 
                return "XSI_SHADER";
            case CSLTemplate::XSI_SHADER_INSTANCE_DATA: 
                return "XSI_SHADER_INSTANCE_DATA";
            case CSLTemplate::XSI_STATIC_VALUES: 
                return "XSI_STATIC_VALUES";
            case CSLTemplate::XSI_TIMECONTROL: 
                return "XSI_TIMECONTROL";
            case CSLTemplate::XSI_TRACK: 
                return "XSI_TRACK";
            case CSLTemplate::XSI_USER_DATA: 
                return "XSI_USER_DATA";
            case CSLTemplate::XSI_USER_DATA_LIST: 
                return "XSI_USER_DATA_LIST";
            case CSLTemplate::XSI_SUB_COMPONENT_ATTRIBUTE_LIST: 
                return "XSI_SUB_COMPONENT_ATTRIBUTE_LIST";
            case CSLTemplate::XSI_TRIANGLE_STRIP_LIST: 
                return "XSI_TRIANGLE_STRIP_LIST";
            case CSLTemplate::XSI_TRIANGLE_LIST: 
                return "XSI_TRIANGLE_LIST";
            case CSLTemplate::XSI_POLYGON_LIST: 
                return "XSI_POLYGON_LIST";
            case CSLTemplate::XSI_VERTEX_LIST: 
                return "XSI_VERTEX_LIST";
            case CSLTemplate::XSI_SHAPE: 
                return "XSI_SHAPE";
            case CSLTemplate::XSI_MESH: 
                return "XSI_MESH";
            case CSLTemplate::XSI_SHAPE_ANIMATION: 
                return "XSI_SHAPE_ANIMATION";
            case CSLTemplate::XSI_CLUSTER_INFO: 
                return "XSI_CLUSTER_INFO";
            case CSLTemplate::XSI_USER_DATA_BLOB: 
                return "XSI_USER_DATA_BLOB";
            case CSLTemplate::XSI_TRANSFORM: 
                return "XSI_TRANSFORM";
            case CSLTemplate::XSI_LIMIT: 
                return "XSI_LIMIT";
            case CSLTemplate::XSI_BASEPOSE: 
                return "XSI_BASEPOSE";
            case CSLTemplate::COLLADA_MATERIAL: 
                return "COLLADA_MATERIAL";
            case CSLTemplate::COLLADA_EFFECT_LIBRARY: 
                return "COLLADA_EFFECT_LIBRARY";
            case CSLTemplate::COLLADA_EFFECT: 
                return "COLLADA_EFFECT";
            case CSLTemplate::COLLADA_EFFECT_COMMON_PROFILE: 
                return "COLLADA_EFFECT_COMMON_PROFILE";
            case CSLTemplate::COLLADA_EFFECT_CG_PROFILE: 
                return "COLLADA_EFFECT_CG_PROFILE";
            case CSLTemplate::COLLADA_EFFECT_COMMON_PARAM: 
                return "COLLADA_EFFECT_COMMON_PARAM";
            case CSLTemplate::COLLADA_EFFECT_COMMON_FLOAT: 
                return "COLLADA_EFFECT_COMMON_FLOAT";
            case CSLTemplate::COLLADA_EFFECT_COMMON_COLOR: 
                return "COLLADA_EFFECT_COMMON_COLOR";
            case CSLTemplate::COLLADA_EFFECT_COMMON_TEXTURE: 
                return "COLLADA_EFFECT_COMMON_TEXTURE";
            case CSLTemplate::COLLADA_EFFECT_COMMON_FLOAT_OR_PARAM: 
                return "COLLADA_EFFECT_COMMON_FLOAT_OR_PARAM";
            case CSLTemplate::COLLADA_EFFECT_COMMON_COLOR_OR_TEXTURE: 
                return "COLLADA_EFFECT_COMMON_COLOR_OR_TEXTURE";
            case CSLTemplate::COLLADA_EFFECT_COMMON_SHADER: 
                return "COLLADA_EFFECT_COMMON_SHADER";
            case CSLTemplate::COLLADA_EFFECT_SETPARAM: 
                return "COLLADA_EFFECT_SETPARAM";
            case CSLTemplate::XSI_IK_RESOLUTIONPLANE: 
                return "XSI_IK_RESOLUTIONPLANE";
            case CSLTemplate::XSI_CAMERA: 
                return "XSI_CAMERA";
            case CSLTemplate::XSI_POLYMATRICKS: 
                return "XSI_POLYMATRICKS";
            case CSLTemplate::XSI_TRANSLATE: 
                return "XSI_TRANSLATE";
            case CSLTemplate::XSI_ROTATE: 
                return "XSI_ROTATE";
            case CSLTemplate::XSI_SCALE: 
                return "XSI_SCALE";
            case CSLTemplate::XSI_SHEAR: 
                return "XSI_SHEAR";
            case CSLTemplate::XSI_LOOKAT: 
                return "XSI_LOOKAT";
            case CSLTemplate::XSI_MATRIX: 
                return "XSI_MATRIX";
            case CSLTemplate::XSI_SHAPEREFERENCE: 
                return "XSI_SHAPEREFERENCE";
            case CSLTemplate::XSI_ACTIONCLIPCONTAINER: 
                return "XSI_ACTIONCLIPCONTAINER";
            case CSLTemplate::XSI_INDEX_LIST: 
                return "XSI_INDEX_LIST";
            default: 
                return "unknown type";
        }
    }

    //---------------------------------------------------------------------------
    const char * GetTypeName(CSLTemplate::EFCurveType curveType)
    {
        switch (curveType)
        {
		    case CSLTemplate::SI_COLOR_R:            
                return "SI_COLOR_R";
		    case CSLTemplate::SI_COLOR_G:            
                return "SI_COLOR_G";
		    case CSLTemplate::SI_COLOR_B:            
                return "SI_COLOR_B";
		    case CSLTemplate::SI_POSITION_X:         
                return "SI_POSITION_X";
		    case CSLTemplate::SI_POSITION_Y:         
                return "SI_POSITION_Y";
		    case CSLTemplate::SI_POSITION_Z:         
                return "SI_POSITION_Z";
		    case CSLTemplate::SI_ROLL:               
                return "SI_ROLL";
		    case CSLTemplate::SI_FOV:                
                return "SI_FOV";
		    case CSLTemplate::SI_NEAR:               
                return "SI_NEAR";
		    case CSLTemplate::SI_FAR:                
                return "SI_FAR";
		    case CSLTemplate::SI_INTEREST_X:         
                return "SI_INTEREST_X";
		    case CSLTemplate::SI_INTEREST_Y:         
                return "SI_INTEREST_Y";
		    case CSLTemplate::SI_INTEREST_Z:         
                return "SI_INTEREST_Z";
		    case CSLTemplate::SI_CONE:               
                return "SI_CONE";
		    case CSLTemplate::SI_SPREAD:             
                return "SI_SPREAD";
		    case CSLTemplate::SI_ORIENTATION_X:      
                return "SI_ORIENTATION_X";
		    case CSLTemplate::SI_ORIENTATION_Y:      
                return "SI_ORIENTATION_Y";
		    case CSLTemplate::SI_ORIENTATION_Z:      
                return "SI_ORIENTATION_Z";
		    case CSLTemplate::SI_DIFFUSE_R:          
                return "SI_DIFFUSE_R";
		    case CSLTemplate::SI_DIFFUSE_G:          
                return "SI_DIFFUSE_G";
		    case CSLTemplate::SI_DIFFUSE_B:          
                return "SI_DIFFUSE_B";
		    case CSLTemplate::SI_DIFFUSE:            
                return "SI_DIFFUSE";
		    case CSLTemplate::SI_PARAMETER:          
                return "SI_PARAMETER";
		    case CSLTemplate::SI_POWER:              
                return "SI_POWER";
		    case CSLTemplate::SI_SPECULAR_R:         
                return "SI_SPECULAR_R";
		    case CSLTemplate::SI_SPECULAR_G:         
                return "SI_SPECULAR_G";
		    case CSLTemplate::SI_SPECULAR_B:         
                return "SI_SPECULAR_B";
		    case CSLTemplate::SI_EMISSIVE_R:         
                return "SI_EMISSIVE_R";
		    case CSLTemplate::SI_EMISSIVE_G:         
                return "SI_EMISSIVE_G";
		    case CSLTemplate::SI_EMISSIVE_B:         
                return "SI_EMISSIVE_B";
		    case CSLTemplate::SI_AMBIENT_R:          
                return "SI_AMBIENT_R";
		    case CSLTemplate::SI_AMBIENT_G:          
                return "SI_AMBIENT_G";
		    case CSLTemplate::SI_AMBIENT_B:          
                return "SI_AMBIENT_B";
		    case CSLTemplate::SI_SCALING_X:          
                return "SI_SCALING_X";
		    case CSLTemplate::SI_SCALING_Y:          
                return "SI_SCALING_Y";
		    case CSLTemplate::SI_SCALING_Z:          
                return "SI_SCALING_Z";
		    case CSLTemplate::SI_ROTATION_X:         
                return "SI_ROTATION_X";
		    case CSLTemplate::SI_ROTATION_Y:         
                return "SI_ROTATION_Y";
		    case CSLTemplate::SI_ROTATION_Z:         
                return "SI_ROTATION_Z";
		    case CSLTemplate::SI_TRANSLATION_X:      
                return "SI_TRANSLATION_X";
		    case CSLTemplate::SI_TRANSLATION_Y:      
                return "SI_TRANSLATION_Y";
		    case CSLTemplate::SI_TRANSLATION_Z:      
                return "SI_TRANSLATION_Z";
		    case CSLTemplate::SI_SHPANIM:            
                return "SI_SHPANIM";
		    case CSLTemplate::SI_NODEVIS:            
                return "SI_NODEVIS";
		    case CSLTemplate::SI_HUE:                
                return "SI_HUE";
		    case CSLTemplate::SI_GAIN:               
                return "SI_GAIN";
		    case CSLTemplate::SI_SATURATION:         
                return "SI_SATURATION";
		    case CSLTemplate::SI_BRIGHTNESS:         
                return "SI_BRIGHTNESS";
		    case CSLTemplate::SI_BLUR_RADIUS:        
                return "SI_BLUR_RADIUS";
		    case CSLTemplate::SI_BLUR_AMOUNT:        
                return "SI_BLUR_AMOUNT";
		    case CSLTemplate::SI_BLUR_ALPHA:         
                return "SI_BLUR_ALPHA";
		    case CSLTemplate::SI_SCALING_TYPE:       
                return "SI_SCALING_TYPE";
		    case CSLTemplate::SI_SCALE_X:            
                return "SI_SCALE_X";
		    case CSLTemplate::SI_SCALE_Y:            
                return "SI_SCALE_Y";
		    case CSLTemplate::SI_FLIP_X:             
                return "SI_FLIP_X";
		    case CSLTemplate::SI_FLIP_Y:             
                return "SI_FLIP_Y";
		    case CSLTemplate::SI_RGBA2GRAYSCALE:     
                return "SI_RGBA2GRAYSCALE";
		    case CSLTemplate::SI_BITS_PER_CHANNEL:   
                return "SI_BITS_PER_CHANNEL";
		    case CSLTemplate::SI_CROP_MIN_X:         
                return "SI_CROP_MIN_X";
		    case CSLTemplate::SI_CROP_MAX_X:         
                return "SI_CROP_MAX_X";
		    case CSLTemplate::SI_CROP_MIN_Y:         
                return "SI_CROP_MIN_Y";
		    case CSLTemplate::SI_CROP_MAX_Y:         
                return "SI_CROP_MAX_Y";
		    case CSLTemplate::SI_FALLOFF_ACTIVE:     
                return "SI_FALLOFF_ACTIVE";
		    case CSLTemplate::SI_FALLOFF_START:      
                return "SI_FALLOFF_START";
		    case CSLTemplate::SI_FALLOFF_END:        
                return "SI_FALLOFF_END";
		    case CSLTemplate::SI_SHADOWS_ENABLED:    
                return "SI_SHADOWS_ENABLED";
		    case CSLTemplate::SI_UMBRA:              
                return "SI_UMBRA";
		    case CSLTemplate::SI_LIGHT_AS_ENERGY:    
                return "SI_LIGHT_AS_ENERGY";
		    case CSLTemplate::SI_ENERGY_FACTOR:      
                return "SI_ENERGY_FACTOR";
		    case CSLTemplate::SI_INTENSITY:          
                return "SI_INTENSITY";
            default:                    
                return "unknown type";
        }
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // XSI helper functions.
    //---------------------------------------------------------------------------

    const char * GetMaterialTypeName(CSLBaseMaterial& in_Material)
    {
        return GetTypeName(in_Material.Type());
    }

    //---------------------------------------------------------------------------
    const char * GetModelTypeName(CSLModel& in_Model)
    {
        return GetTypeName(in_Model.GetPrimitiveType());
    }

    //---------------------------------------------------------------------------
    const char * GetTemplateTypeName(CSLTemplate& in_Template)
    {
        return GetTypeName(in_Template.Type());
    }

    //---------------------------------------------------------------------------
    const char * GetFCurveTypeName(CSLFCurve& in_FCurve)
    {
        return GetTypeName(in_FCurve.GetFCurveType());
    }

    //---------------------------------------------------------------------------
    bool IsVisible(CSLModel& in_Model)
    {
        CSLVisibility* visibility = in_Model.Visibility();
        if(visibility)
            return (visibility->GetVisibility() != 0);
        else
            return true;
    }

    //---------------------------------------------------------------------------
    bool CanBeVisible(CSLModel& in_Model)
    {
        const CSLTemplate::ETemplateType tmplType = in_Model.GetPrimitiveType();
        switch (tmplType)
        {
            // By default and for unknown thing, assume it can be visible.
            default:
                return true;
            case CSLTemplate::SI_NULL_OBJECT:
            case CSLTemplate::SI_FILE_INFO:
            case CSLTemplate::SI_MODEL:
            case CSLTemplate::SI_SCENE_INFO:
                return false;
        }
    }

    //---------------------------------------------------------------------------
}
