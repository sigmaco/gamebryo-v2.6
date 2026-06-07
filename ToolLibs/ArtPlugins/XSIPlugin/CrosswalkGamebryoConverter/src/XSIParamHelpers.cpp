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
#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoSettings.h"

#include "Model.h"
#include "Template.h"
#include "CustomPSet.h"
#include "VariantParameter.h"
#include "XSIMaterial.h"
#include "XSIShader.h"
#include "ConnectionPoint.h"
#include "ShaderConnectionPoint.h"
#include "Constraint.h"
#include "ConstrainableType.h"

#include "NiColor.h"
#include "NiString.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Retrieve the custom propery set with the given name.
    //---------------------------------------------------------------------------
    CSLCustomPSet* FindCustomProperties(CSLTemplate& in_Tmpl, const char* in_Name)
    {
        if (!in_Name || !in_Name[0])
            return 0;

        NiString name(in_Name);

        const SI_Int propSetCount = in_Tmpl.GetCustomPSetCount();
        CSLCustomPSet** propSetArray = in_Tmpl.GetCustomPSetList();
        for (SI_Int i = 0 ; i < propSetCount; ++i)
        {
            CSLCustomPSet* propSet = propSetArray[i];
            if (propSet && name.EqualsNoCase(propSet->Name().GetText()))
            {
                return propSet;
            }
        }

        return 0;
    }

    //---------------------------------------------------------------------------
    // Retrieve the connection point with the given name.
    //---------------------------------------------------------------------------
    CSLConnectionPoint* FindConnectionPoint(CSLXSIMaterial& in_Mat, const char* in_Name)
    {
        if (!in_Name || !in_Name[0])
            return 0;

        NiString name(in_Name);

        const SI_Int count = in_Mat.GetConnectionPointCount();
        CSLConnectionPoint** connArray = in_Mat.GetConnectionPointList();
        for (SI_Int i = 0; i < count; ++i)
        {
            CSLConnectionPoint* conn = connArray[i];
            if (conn)
            {
                if (name.EqualsNoCase(conn->GetName()))
                {
                    return conn;
                }
            }
        }

        return NULL;
    }

    //---------------------------------------------------------------------------
    CSLShaderConnectionPoint* FindConnectionPoint(CSLXSIShader& in_Shader, const char* in_Name)
    {
        if (!in_Name || !in_Name[0])
            return 0;

        NiString name(in_Name);

        const SI_Int count = in_Shader.GetConnectionPointCount();
        CSLShaderConnectionPoint** connArray = in_Shader.GetConnectionPointList();
        for (SI_Int i = 0; i < count; ++i)
        {
            CSLShaderConnectionPoint* conn = connArray[i];
            if (conn)
            {
                if (name.EqualsNoCase(conn->GetName()))
                {
                    return conn;
                }
            }
        }

        return NULL;
    }

    //---------------------------------------------------------------------------
    // Retrieve the typed parameter if it has the specified type.
    //---------------------------------------------------------------------------
    bool GetTypedParameter(
        CdotXSIParam* in_pParam,
        int in_Type,
        SI_TinyVariant & out_Value)
    {
        if (in_pParam)
        {
            in_pParam->GetValue(&out_Value);
            if (out_Value.variantType == in_Type)
            {
                return true;
            }
        }

        return false;
    }

    //---------------------------------------------------------------------------
    // Retrieve the parameter with the given name a value.
    //---------------------------------------------------------------------------
    bool GetTypedParameter(
        CSLTemplate& in_Tmpl,
        const char * in_Name,
        int in_Type,
        SI_TinyVariant & out_Value)
    {
        // Note: ParameterFromName() is not const-correct...
        CSLAnimatableType* param = in_Tmpl.ParameterFromName((SI_Char*)in_Name);
        if (!param)
        {
            CSIBCString lowerName(in_Name);
            lowerName.LowerCase();
            param = in_Tmpl.ParameterFromName(lowerName.GetText());
        }

        if (param)
        {
            CdotXSIParam* dotXSIParam = param->ParameterReference();
            return GetTypedParameter(dotXSIParam, in_Type, out_Value);
        }

        // If not found, search parameter of the template.
        CdotXSITemplate* pTmpl = in_Tmpl.Template();
        if (pTmpl)
        {
            CdotXSIParams& params = pTmpl->Params();
            CdotXSIParam* dotXSIParam = params.Find(in_Name);
            return GetTypedParameter(dotXSIParam, in_Type, out_Value);
        }

        return false;
    }

    //---------------------------------------------------------------------------
    // Find the parameter with the given name and value.
    //---------------------------------------------------------------------------
    bool FindStringParameter(CSLTemplate& in_Tmpl, const char * in_Name, CSIBCString & out_Value)
    {
        SI_TinyVariant value;
        if (GetTypedParameter(in_Tmpl, in_Name, SI_VT_PCHAR, value))
        {
            out_Value = value.p_cVal;
            return true;
        }
        else
        {
            return false;
        }
    }

    //---------------------------------------------------------------------------
    bool FindBooleanParameter(CSLTemplate& in_Tmpl, const char * in_Name, bool & out_Value)
    {
        SI_TinyVariant value;
        if (GetTypedParameter(in_Tmpl, in_Name, SI_VT_BOOL, value))
        {
            out_Value = (value.boolVal != 0);
            return true;
        }
        else
        {
            return false;
        }
    }

    //---------------------------------------------------------------------------
    bool FindIntegerParameter(CSLTemplate& in_Tmpl, const char * in_Name, int & out_Value)
    {
        SI_TinyVariant value;
        if (GetTypedParameter(in_Tmpl, in_Name, SI_VT_INT, value))
        {
            out_Value = value.nVal;
            return true;
        }
        else
        {
            return false;
        }
    }

    //---------------------------------------------------------------------------
    bool FindLongParameter(CSLTemplate& in_Tmpl, const char * in_Name, long & out_Value)
    {
        SI_TinyVariant value;
        if (GetTypedParameter(in_Tmpl, in_Name, SI_VT_LONG, value))
        {
            out_Value = value.nVal;
            return true;
        }
        else
        {
            return false;
        }
    }

    //---------------------------------------------------------------------------
    bool FindFloatParameter(CSLTemplate& in_Tmpl, const char * in_Name, float & out_Value)
    {
        SI_TinyVariant value;
        if (GetTypedParameter(in_Tmpl, in_Name, SI_VT_FLOAT, value))
        {
            out_Value = value.fVal;
            return true;
        }
        else if (GetTypedParameter(in_Tmpl, in_Name, SI_VT_DOUBLE, value))
        {
            out_Value = (float)value.dVal;
            return true;
        }
        else
        {
            return false;
        }
    }

    //---------------------------------------------------------------------------
    bool FindFloatSubParameter(CSLTemplate& in_Tmpl, 
        const char * in_Name, 
        const char * in_SubName, 
        float& out_Value)
    {
        NIASSERT(in_Name);
        NIASSERT(in_SubName);

        CSIBCString paramName(in_Name);
        paramName.Concat(in_SubName);
        return FindFloatParameter(in_Tmpl, paramName.GetText(), out_Value);
    }

    //---------------------------------------------------------------------------
    bool FindColorParameter(Context & io_Context, 
        CSLTemplate& in_Tmpl, 
        const char * in_Name, 
        NiColor& out_Color)
    {
        // Verify alpha transparency. Full transparency is returned as no-value.
        float alpha;
        if (FindFloatSubParameter(in_Tmpl, in_Name, ".alpha", alpha))
        {
            if (alpha == 1.0f)
            {
                return false;
            }

            if (alpha != 0.0f)
            {
                io_Context.Logf(
                    LOG_WARNING,
                    "Object \"%s\" uses unsupported alpha transparency in \"%s\"",
                    in_Tmpl.Name().GetText(),
                    in_Name);
            }
        }

        if (!FindFloatSubParameter(in_Tmpl, in_Name, ".red",   out_Color.r))
            return false;
        if (!FindFloatSubParameter(in_Tmpl, in_Name, ".green", out_Color.g))
            return false;
        if (!FindFloatSubParameter(in_Tmpl, in_Name, ".blue",  out_Color.b))
            return false;

        return true;
    }

    //---------------------------------------------------------------------------
    // Retrieve the parameter with the given name and value.
    //---------------------------------------------------------------------------
    const char * GetStringParameter(
        CSLTemplate& in_Tmpl,
        const char * in_Name,
        const char * in_DefaultValue)
    {
        SI_TinyVariant value;
        return GetTypedParameter(in_Tmpl, in_Name, SI_VT_PCHAR, value)
             ? value.p_cVal
             : in_DefaultValue;
    }

    //---------------------------------------------------------------------------
    bool GetBooleanParameter(
        CSLTemplate& in_Tmpl,
        const char * in_Name,
        bool in_DefaultValue)
    {
        FindBooleanParameter(in_Tmpl, in_Name, in_DefaultValue);
        return in_DefaultValue;
    }

    //---------------------------------------------------------------------------
    int GetIntegerParameter(
        CSLTemplate& in_Tmpl,
        const char * in_Name,
        int in_DefaultValue)
    {
        FindIntegerParameter(in_Tmpl, in_Name, in_DefaultValue);
        return in_DefaultValue;
    }

    //---------------------------------------------------------------------------
    long GetLongParameter(
        CSLTemplate& in_Tmpl,
        const char * in_Name,
        long in_DefaultValue)
    {
        FindLongParameter(in_Tmpl, in_Name, in_DefaultValue);
        return in_DefaultValue;
    }

    //---------------------------------------------------------------------------
    float GetFloatParameter(
        CSLTemplate& in_Tmpl,
        const char * in_Name,
        float in_DefaultValue)
    {
        FindFloatParameter(in_Tmpl, in_Name, in_DefaultValue);
        return in_DefaultValue;
    }

    float GetFloatSubParameter(
        CSLTemplate& in_Tmpl,
        const char * in_Name,
        const char * in_SubName,
        float in_DefaultValue)
    {
        FindFloatSubParameter(in_Tmpl, in_Name, in_SubName, in_DefaultValue);
        return in_DefaultValue;
    }

    //---------------------------------------------------------------------------
    bool LoadSettings(
        CSLModel& in_Model,
        ConverterSettings& io_Settings,
        const char* in_PropertyName)
    {
        if (!in_PropertyName)
            in_PropertyName = XSI_GAMEBRYO_EXPORT_OPTIONS_NAME;

        CSLCustomPSet* propSet = FindCustomProperties(in_Model, in_PropertyName);
        if (!propSet)
        {
            // Crosswalk typically insert a dummy scene root,
            // so try finding the settings in the children.
            const SI_Int count = in_Model.GetChildrenCount();
            for (SI_Int i = 0; i < count; ++i)
            {
                CSLModel * child = in_Model.GetChildrenList()[i];
                if (child)
                {
                    if (LoadSettings(*child, io_Settings, in_PropertyName))
                    {
                        return true;
                    }
                }
            }

            return false;
        }
        else
        {
            io_Settings.SetGamebryoProcessingScript(
                GetStringParameter(*propSet, XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_NAME));
            io_Settings.SetGamebryoExportingScript(
                GetStringParameter(*propSet, XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_NAME));
            io_Settings.SetGamebryoViewingScript(
                GetStringParameter(*propSet, XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_NAME));
            io_Settings.SetGamebryoProcessingScriptEnc(
                GetStringParameter(*propSet, XSI_GAMEBRYO_PROCESSING_SCRIPT_ENC_SETTING_NAME));
            io_Settings.SetGamebryoExportingScriptEnc(
                GetStringParameter(*propSet, XSI_GAMEBRYO_EXPORTING_SCRIPT_ENC_SETTING_NAME));
            io_Settings.SetGamebryoViewingScriptEnc(
                GetStringParameter(*propSet, XSI_GAMEBRYO_VIEWING_SCRIPT_ENC_SETTING_NAME));
            // Note: XSI_GAMEBRYO_EXPORT_SELECTION_SETTING_NAME is only 
            // relevant to thedotXSI exporter, not the Gamebryo converter,
            // so there is not corresponding setting in the ConverterSettings class.
            io_Settings.SetExportCamera(
                GetBooleanParameter(*propSet, XSI_GAMEBRYO_EXPORT_CAMERAS_SETTING_NAME, true));
            io_Settings.SetExportLights(
                GetBooleanParameter(*propSet, XSI_GAMEBRYO_EXPORT_LIGHTS_SETTING_NAME, true));
            io_Settings.SetExportTextures(
                GetBooleanParameter(*propSet, XSI_GAMEBRYO_EXPORT_TEXTURES_SETTING_NAME, true));
            io_Settings.SetExportShapeAnimations(
                GetBooleanParameter(*propSet, 
                XSI_GAMEBRYO_EXPORT_SHAPE_ANIMS_SETTING_NAME, true));
            io_Settings.SetExportSkinnings(
                GetBooleanParameter(*propSet, XSI_GAMEBRYO_EXPORT_SKINNINGS_SETTING_NAME, true));
            io_Settings.SetLaunchGamebryoAssetViewer(
                GetBooleanParameter(*propSet, XSI_GAMEBRYO_LAUNCH_ASSET_VIEWER_SETTING_NAME));
            io_Settings.SetIgnoreAllErrors(
                GetBooleanParameter(*propSet, XSI_GAMEBRYO_IGNORE_ALL_ERRORS_SETTING_NAME));
            io_Settings.SetEmbedTextures(
                GetBooleanParameter(*propSet, XSI_GAMEBRYO_EMBED_TEXTURES_SETTING_NAME));
            io_Settings.SetTextureFormat((epg::TextureFormat)
                GetIntegerParameter(*propSet, XSI_GAMEBRYO_TEXTURE_FORMAT_SETTING_NAME));
            io_Settings.SetTextureCompression((epg::TextureCompression)
                GetIntegerParameter(*propSet, XSI_GAMEBRYO_TEXTURE_COMPRESSION_SETTING_NAME));
            io_Settings.SetKeyframeSamplingRate(
                GetFloatParameter(*propSet, XSI_GAMEBRYO_KEYFRAME_RATE_SETTING_NAME));
            io_Settings.SetLogFileName(
                GetStringParameter(*propSet, XSI_GAMEBRYO_LOG_FILENAME_SETTING_NAME));
            io_Settings.SetTargetPlatform((epg::TargetPlatform)
                GetLongParameter(*propSet, XSI_GAMEBRYO_TARGET_PLATFORM_SETTING_NAME));
            io_Settings.SetMeshProfile(
                GetStringParameter(*propSet, XSI_GAMEBRYO_MESH_PROFILE_SETTING_NAME));

            return true;
        }
    }

    //---------------------------------------------------------------------------
}
