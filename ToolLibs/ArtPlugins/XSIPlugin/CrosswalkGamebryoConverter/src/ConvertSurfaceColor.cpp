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

#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoHelpers.h"
#include "CrosswalkGamebryoMatHelpers.h"
#include "CrosswalkGamebryoMatMap.h"

#include "XSIShader.h"
#include "XSIMaterial.h"
#include "AnimatableType.h"

#include "NiMaterialProperty.h"
#include "NiSpecularProperty.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Material mental-ray surface shader color conversion.
    //
    // Here's the mapping between XSI materials and Gamebryo material property.
    // The mappings use the following conventions:
    //
    //   dotXSI shader entry -> Gamebryo function or class.
    //
    //   (color) means that the dotXSI entry is a color, usually represented
    //           as three entries in the form name.red, name.green, name.blue.
    //
    //   (boolean) used to enable or disable a feature in dotXSI.
    //
    //   surface:ambient (color) -----------------> NiMaterialProperty::SetAmbientColor().
    //   Note: ambient color is always in use, it doesn't have a flag.
    //
    //   surface:diffuse (color) -----------------> NiMaterialProperty::SetDiffuseColor().
    //   surface:diffuse_inuse (boolean) ---------> Enable / disable diffuse color.
    //
    //   surface:specular (color) ----------------> NiMaterialProperty::SetSpecularColor().
    //   surface:specular_inuse (color) ----------> Enable / disable specular color.
    //
    //   surface:incadescence (color) ------------> NiMaterialProperty::SetEmittance().
    //   surface:incand_inuse (color) ------------> Enable / disable emittance color.
    //   surface:inc_inten (float) ---------------> Intensity, multiplies the color values.
    //
    //   surface:shiny (float) -------------------> NiMaterialProperty::SetShineness().
    //
    //   surface:transparency (color) ------------> NiMaterialProperty::SetAlpha().
    //   Note: we use the average of the color R, G and B values. We warns if not equals.
    //
    //---------------------------------------------------------------------------

    //---------------------------------------------------------------------------
    // Convert simple constant shader colors.
    // Returns true if any color were converted.
    //---------------------------------------------------------------------------
    bool ConvertSurfaceColors(
        Context& io_Context,
        CSLXSIShader& in_Shader,
        NiMaterialProperty& io_MatProp)
    {
        bool generatedColors = false;

        // Take advantage in the regularity of color setting functions...
        typedef void (NiMaterialProperty::*ColorSettingFunction)(const NiColor &);
        struct ColorConversion
        {
            ColorSettingFunction SetColor;
            const char * colorParam;
            const char * inUseParam;
        };

        ColorConversion colorConv[] =
        {
            { &NiMaterialProperty::SetAmbientColor,  "ambient",       "!NONE!"         },
            { &NiMaterialProperty::SetDiffuseColor,  "diffuse",       "diffuse_inuse"  },
            { &NiMaterialProperty::SetSpecularColor, "specular",      "specular_inuse" },
            { &NiMaterialProperty::SetEmittance,     "incandescence", "incand_inuse"   },
            { 0, 0, 0 }
        };

        NiColor color;
        for (int i = 0; colorConv[i].colorParam; ++i)
        {
            if (GetBooleanParameter(in_Shader, colorConv[i].inUseParam, true))
            {
                if (FindColorParameter(io_Context, in_Shader, colorConv[i].colorParam, color))
                {
                    (io_MatProp.*colorConv[i].SetColor)(color);
                    generatedColors = true;
                }
            }
        }

        // Convert incandescence intensity by mutiplying the color values.
        float intensity;
        if (FindFloatParameter(in_Shader, "inc_inten", intensity))
        {
            color = io_MatProp.GetEmittance();
            color.r *= intensity;
            color.g *= intensity;
            color.b *= intensity;
            io_MatProp.SetEmittance(color);
        }

        return generatedColors;
    }

    //---------------------------------------------------------------------------
    // Convert the shininess.
    // Returns true if the shininess was set.
    //---------------------------------------------------------------------------
    bool ConvertSurfaceShineness(CSLXSIShader& in_Shader, NiMaterialProperty& io_MatProp)
    {
        float shiny;
        if (FindFloatParameter(in_Shader, "shiny", shiny))
        {
            io_MatProp.SetShineness(shiny);
            return true;
        }
        else
        {
            return false;
        }
    }

    //---------------------------------------------------------------------------
    // Convert the transparency.
    // Returns true if transparency was set.
    //---------------------------------------------------------------------------
    bool ConvertSurfaceTransparency(
        Context& io_Context,
        CSLXSIShader& in_Shader,
        NiMaterialProperty& io_MatProp)
    {
        // Should we use color alphas? Hard to say... whatever we choose it will be arbitrary.
        NiColor color;
        if (FindColorParameter(io_Context, in_Shader, "transparency", color))
        {
            if (color.r != color.g || color.r != color.b)
            {
                io_Context.Logf(
                    LOG_WARNING,
                    "Shader \"%s\" has non-uniform transparency, using average instead.",
                    in_Shader.Name().GetText());
            }
            io_MatProp.SetAlpha(1.0f - (color.r + color.g + color.b) / 3);
            return true;
        }
        else
        {
            return false;
        }
    }

    //---------------------------------------------------------------------------
    // Material real-time shader color conversion.
    //
    // Here's the mapping between XSI materials and Gamebryo material property.
    // The mappings use the following conventions:
    //
    //   dotXSI shader entry -> Gamebryo function or class.
    //
    //   (color) means that the dotXSI entry is a color, usually represented
    //           as three entries in the form name.red, name.green, name.blue.
    //
    //   (float) is a real number between 0 and 1.
    //
    //   RealTime:Ambient (color) ----> NiMaterialProperty::SetAmbientColor().
    //   RealTime:Diffuse (color) ----> NiMaterialProperty::SetDiffuseColor().
    //   RealTime:Specular (color) ---> NiMaterialProperty::SetSpecularColor().
    //   RealTime:Emittance(color) ---> NiMaterialProperty::SetEmittance().
    //   RealTime:Shininess(float) ---> NiMaterialProperty::SetShineness().
    //   RealTime:Alpha (float) ------> NiMaterialProperty::SetAlpha().
    //
    //---------------------------------------------------------------------------

    //---------------------------------------------------------------------------
    // Convert simple constant shader colors.
    // Returns true if any color were converted.
    //---------------------------------------------------------------------------
    bool ConvertRealtimeColors(
        Context& io_Context,
        CSLXSIShader& in_Shader,
        NiMaterialProperty& io_MatProp)
    {
        bool generatedColors = false;

        // Take advantage in the regularity of color setting functions...
        typedef void (NiMaterialProperty::*ColorSettingFunction)(const NiColor &);
        struct ColorConversion
        {
            ColorSettingFunction SetColor;
            const char * colorParam;
        };

        static const ColorConversion colorConv[] =
        {
            { &NiMaterialProperty::SetAmbientColor,  "Ambient"   },
            { &NiMaterialProperty::SetDiffuseColor,  "Diffuse"   },
            { &NiMaterialProperty::SetSpecularColor, "Specular"  },
            { &NiMaterialProperty::SetEmittance,     "Emittance" },
            { 0, 0 }
        };

        NiColor color;
        for (int i = 0; colorConv[i].colorParam; ++i)
        {
            if (FindColorParameter(io_Context, in_Shader, colorConv[i].colorParam, color))
            {
                (io_MatProp.*colorConv[i].SetColor)(color);
                generatedColors = true;
            }
        }

        return generatedColors;
    }

    //---------------------------------------------------------------------------
    // Convert the shininess.
    // Returns true if the shininess was set.
    //---------------------------------------------------------------------------
    bool ConvertRealtimeShineness(CSLXSIShader& in_Shader, NiMaterialProperty& io_MatProp)
    {
        float shiny;
        if (FindFloatParameter(in_Shader, "Shininess", shiny))
        {
            io_MatProp.SetShineness(shiny);
            return true;
        }
        else
        {
            return false;
        }
    }

    //---------------------------------------------------------------------------
    // Convert the transparency.
    // Returns true if transparency was set.
    //---------------------------------------------------------------------------
    bool ConvertRealtimeTransparency(
        Context& io_Context,
        CSLXSIShader& in_Shader,
        NiMaterialProperty& io_MatProp)
    {
        float alpha;
        if (FindFloatParameter(in_Shader, "Alpha", alpha))
        {
            io_MatProp.SetAlpha(alpha);
            return true;
        }
        else
        {
            return false;
        }
    }

    //---------------------------------------------------------------------------
    // Convert the shader colors to a Gamebryo material property.
    // Returns true on success. Doing nothing is considered successful.
    //---------------------------------------------------------------------------
    bool ConvertSurfaceColors(
        Context& io_Context,
        CSLXSIMaterial& in_Material)
    {
        if (CSLXSIShader* pkShader = FindShader(in_Material, "RealTime"))
        {
            NiMaterialPropertyPtr mat = NiNew NiMaterialProperty;

            bool validMaterialProperty = false;
            validMaterialProperty |= ConvertRealtimeColors(io_Context, *pkShader, *mat);
            validMaterialProperty |= ConvertRealtimeShineness(*pkShader, *mat);
            validMaterialProperty |= ConvertRealtimeTransparency(io_Context, *pkShader, *mat);

            if (validMaterialProperty)
                io_Context.GetMaterialMap().AddMaterialProperty(in_Material, *mat);

            // If the specualr color alpha is zero, then the specular color
            // is used and we must generate the specular property.
            float alpha = 1.0f;
            if (FindFloatSubParameter(*pkShader, "Specular", ".alpha", alpha) && alpha == 0.0f)
            {
                NiSpecularProperty* pSpecProp = NiNew NiSpecularProperty;
                pSpecProp->SetSpecular(true);
                io_Context.GetMaterialMap().AddSpecularProperty(in_Material, *pSpecProp);
            }
        }
        else if (CSLXSIShader* pkShader = FindShader(in_Material, "surface"))
        {
            io_Context.Logf(
                LOG_WARNING,
                "Didn't find a real-time shader in \"%s\" material,"
                " using surface shader instead.",
                in_Material.Name().GetText());

            NiMaterialPropertyPtr mat = NiNew NiMaterialProperty;

            bool validMaterialProperty = false;
            validMaterialProperty |= ConvertSurfaceColors(io_Context, *pkShader, *mat);
            validMaterialProperty |= ConvertSurfaceShineness(*pkShader, *mat);
            validMaterialProperty |= ConvertSurfaceTransparency(io_Context, *pkShader, *mat);

            if (validMaterialProperty)
                io_Context.GetMaterialMap().AddMaterialProperty(in_Material, *mat);
        }
        else
        {
            // Note: we warn about missing real-time shaders because we expect
            //       all objects to have one. Without one, the object will
            //       not have colors, which is suspicious.
            io_Context.Logf(
                LOG_WARNING,
                "Material \"%s\" does not have a real-time or surface shader.",
                in_Material.Name().GetText());
        }

        return true;
    }

    //---------------------------------------------------------------------------
}
