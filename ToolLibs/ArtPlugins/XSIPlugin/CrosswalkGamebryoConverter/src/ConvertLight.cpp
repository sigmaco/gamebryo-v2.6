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
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoNodeMap.h"
#include "CrosswalkGamebryoEffectMap.h"
#include "CrosswalkGamebryoSettings.h"
#include "CrosswalkGamebryoAnimCurves.h"
#include "CrosswalkGamebryoLightingMap.h"
#include "CrosswalkGamebryoAnimMap.h"

#include "Ambience.h"
#include "Light.h"
#include "Pointlight.h"
#include "Spotlight.h"
#include "Directionallight.h"
#include "Infinitelight.h"
#include "Model.h"

#include "NiAmbientLight.h"
#include "NiPointLight.h"
#include "NiSpotLight.h"
#include "NiDirectionalLight.h"
#include "NiPoint3.h"
#include "NiNode.h"
#include "NiLightColorController.h"
#include "NiPoint3Interpolator.h"
#include "NiLinPosKey.h"
#include "NiLightDimmerController.h"
#include "NiFloatInterpolator.h"

//---------------------------------------------------------------------------
// Helper functions.
//---------------------------------------------------------------------------

namespace
{
    //---------------------------------------------------------------------------
    // Control if an active falloff in XSI is an error or not.
    //---------------------------------------------------------------------------
    enum LightFallofSupport
    {
        LIGHT_FALLOF_NOT_SUPPORTED  = 0,
        LIGHT_FALLOF_SUPPORTED      = 1
    };

    //---------------------------------------------------------------------------
    // Control where the light color is set in the Gamebryo light.
    //---------------------------------------------------------------------------
    enum LightColorType
    {
        LIGHT_NO_COLOR       = 0,
        LIGHT_AMBIENT_COLOR  = (1 << 0),
        LIGHT_DIFFUSE_COLOR  = (1 << 1),
        LIGHT_SPECULAR_COLOR = (1 << 2),
        LIGHT_ALL_COLORS     = (1 << 3) - 1
    };

    inline LightColorType operator|(LightColorType lhs, LightColorType rhs)
    {
        return LightColorType((int)lhs | (int)rhs);
    }

    inline LightColorType operator&(LightColorType lhs, LightColorType rhs)
    {
        return LightColorType((int)lhs & (int)rhs);
    }

    //---------------------------------------------------------------------------
    // Convert light color animation.
    //---------------------------------------------------------------------------

    bool ConvertLightColorCurves(
        epg::Context& io_Context,
        NiLight& io_Light,
        const char* in_LightName,
        LightColorType in_ColorType,
        epg::AnimCurves& in_Curves)
    {
        // Verify validity.
        if (!in_Curves.IsValid())
            return false;

        if (!in_Curves.HasAnimation())
            return true;

        typedef epg::AnimCurves::TimedValues TimedValues;
        const TimedValues& values = in_Curves.GetTimedValues();

        const int count = values.size();
        if (count <= 0)
            return true;

        // Create controller and interpolator.
        //
        // Note: because light color doesn't have an alpha value, the Gamebryo
        //       engine uses a Point3 interpolator instead of a color interpolator.
        NiLightColorControllerPtr spController = NiNew NiLightColorController;
        NiPoint3Interpolator* pInterpolator = NiNew NiPoint3Interpolator;
        spController->SetInterpolator(pInterpolator);
        if (in_ColorType & LIGHT_AMBIENT_COLOR)
            spController->SetAmbient(true);

        // Convert keys.
        //
        // Note: because light color doesn't have an alpha value, the Gamebryo
        //       engine uses PosKey instead of ColorKey.
        NiLinPosKey* pGBKeys = NiNew NiLinPosKey[count];
        TimedValues::const_iterator pos = values.begin();
        for (SI_Int i = 0; i < count; ++i, ++pos)
        {
            pGBKeys[i].SetTime(pos->first);
            pGBKeys[i].SetPos(NiPoint3(pos->second[0], pos->second[1], pos->second[2]));
        }

        // Note: ReplaceKeys() takes ownership of the keys.
        pInterpolator->ReplaceKeys(pGBKeys, count, NiAnimationKey::LINKEY);

        NiPosKey::FillDerivedValsFunction pDeriv;
        pDeriv = NiPosKey::GetFillDerivedFunction(NiAnimationKey::LINKEY);
        NIASSERT(pDeriv);
        (*pDeriv)(pGBKeys, count, sizeof(NiLinPosKey));

        // Finish setup and register controller with the light.
        spController->ResetTimeExtrema();
        pInterpolator->Collapse();
        spController->SetTarget(&io_Light);

        io_Light.SetSelectiveUpdate(true);
        io_Light.SetSelectiveUpdatePropertyControllers(true);
        io_Context.GetAnimationMap().AddAnimatedObject(io_Light);

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert light color to the proper light parameters.
    // Note: transparent light is treated as an absence of light.
    //---------------------------------------------------------------------------
    bool ConvertLightColor(
        epg::Context & io_Context,
        CSLTemplate& in_XSILight,
        const CSIBCColorf& in_Color,
        NiLight& io_Light,
        const char* in_LightName,
        const char* in_PropName,
        LightColorType in_ColorType)
    {
        // Note: ConvertColor() returns false when the color was fully transparent.
        //       We treat that as an absence of light.
        NiColor color;
        if (epg::ConvertColor(io_Context, in_Color, color, in_LightName, in_PropName))
        {
            NIASSERT((in_ColorType & LIGHT_ALL_COLORS) != LIGHT_NO_COLOR);
            NiColor black(0.0f, 0.0f, 0.0f);
            io_Light.SetAmbientColor( (in_ColorType & LIGHT_AMBIENT_COLOR)  ? color : black);
            io_Light.SetDiffuseColor( (in_ColorType & LIGHT_DIFFUSE_COLOR)  ? color : black);
            io_Light.SetSpecularColor((in_ColorType & LIGHT_SPECULAR_COLOR) ? color : black);

            // Convert color animation, if any.
            const epg::AnimCurvesDescriptor lightColorAnimCurves =
            {
                3,
                { "red light", "green light", "blue light" },
                { "red", "green", "blue" },
                { color.r, color.g, color.b },
                0.0f,
                epg::NO_ANIM_CURVES_FLAG
            };
            epg::AnimCurves colorAnim(
                io_Context,
                in_XSILight,
                in_LightName,
                lightColorAnimCurves);
            return ConvertLightColorCurves(
                io_Context, io_Light, in_LightName, in_ColorType, colorAnim);
        }
        else
        {
            return false;
        }
    }

    //---------------------------------------------------------------------------
    // Convert the light intensity.
    //---------------------------------------------------------------------------
    bool ConvertLightIntensity(
        epg::Context & io_Context,
        CSLLightInfo& in_LightInfo,
        NiLight& io_Light,
        const char* in_LightName)
    {
        float intensity = in_LightInfo.GetIntensity();
        io_Light.SetDimmer(intensity);

        // Convert intensity animation curve.
        const epg::AnimCurvesDescriptor lightIntensityAnimCurve =
        {
            1,
            { "intensity" },
            { "Intensity" },
            { intensity },
            0.0f,
            epg::NO_ANIM_CURVES_FLAG
        };
        epg::AnimCurves intensityAnim(
            io_Context,
            in_LightInfo,
            in_LightName,
            lightIntensityAnimCurve);
        if (!intensityAnim.IsValid())
            return false;
        if (!intensityAnim.HasAnimation())
            return true;
        NiFloatInterpolatorPtr spInterpolator = epg::ConvertFloatCurves(
            io_Context, in_LightName, intensityAnim, intensity);
        if (!spInterpolator)
            return false;

        // Create controller.
        //
        // Note: because light color doesn't have an alpha value, the Gamebryo
        //       engine uses a Point3 interpolator instead of a color interpolator.
        NiLightDimmerControllerPtr spController = NiNew NiLightDimmerController;
        spController->SetInterpolator(spInterpolator);

        // Finish setup and register controller with the light.
        spController->ResetTimeExtrema();
        spInterpolator->Collapse();
        spController->SetTarget(&io_Light);

        io_Light.SetSelectiveUpdate(true);
        io_Light.SetSelectiveUpdatePropertyControllers(true);
        io_Context.GetAnimationMap().AddAnimatedObject(io_Light);

        return true;
    }

    //---------------------------------------------------------------------------
    // Transform two world point into a local rotation.
    //---------------------------------------------------------------------------
    void ComputeLookAtRotation(
        NiAVObject& in_Object,
        const NiPoint3& in_LocalTarget,
        NiMatrix3& out_Matrix)
    {
        // Note: the following code was copied and adapted from the
        //       NiLookAtInterpolator::Update() function.

        NiPoint3 kAt = (in_LocalTarget + in_Object.GetWorldTranslate())
                     -  in_Object.GetWorldTranslate();
        if (kAt.SqrLength() < 0.001f)
        {
            // We have zero vector so don't update the transform
            out_Matrix.MakeIdentity();
        }
        else    
        {
            NiPoint3 kUp, kRight;
            float fDot;

            kAt.Unitize();

            if (((kAt.z < 1.001f) && (kAt.z > 0.999f)) ||
                ((kAt.z > -1.001f) && (kAt.z < -0.999f)))
            {
                // kUp & kAt are too close - use the Y axis as an alternate kUp
                kUp = NiPoint3::UNIT_Y;
                fDot = kAt.y;
            }
            else
            {
                kUp = NiPoint3::UNIT_Z;
                fDot = kAt.z;
            }


            // subtract off the component of 'kUp' in the 'kAt' direction
            kUp -= fDot*kAt;
            kUp.Unitize();

            // Note: GetFlip() is not needed for spot light.
            if (false)
            {
                kAt = -kAt;
            }

            kRight = kUp.Cross(kAt);

            // Spot light always use the X axis.
            switch (0)
            {
                case 0:
                    out_Matrix.SetCol(0, kAt);
                    out_Matrix.SetCol(1, kUp);
                    out_Matrix.SetCol(2, -kRight);
                    break;
                case 1:
                    out_Matrix.SetCol(0, kRight);
                    out_Matrix.SetCol(1, kAt);
                    out_Matrix.SetCol(2, -kUp);
                    break;
                case 2:
                    out_Matrix.SetCol(0, kRight);
                    out_Matrix.SetCol(1, kUp);
                    out_Matrix.SetCol(2, kAt);
                    break;
            }

            // At this point out_Matrix is the desired world rotation. The next
            // line converts it into the model space rotation necessary to
            // result in the desired world space rotation.
            NiMatrix3 kParentWorldRot;
            if (NiNode* pParent = in_Object.GetParent())
                kParentWorldRot = pParent->GetWorldRotate();
            else
                kParentWorldRot = NiMatrix3::IDENTITY;
            out_Matrix = kParentWorldRot.TransposeTimes(out_Matrix);
        }
    }

    //---------------------------------------------------------------------------
    // Convert spotlight orientation.
    //---------------------------------------------------------------------------
    void ConvertSpotlightOrientation(
        const CSIBCVector3D* in_ParentScale,
        const CSIBCVector3D& in_InterestPos,
        NiLight& io_Light)
    {
        NiPoint3 interestPoint;
        epg::ConvertTranslation(in_InterestPos, interestPoint, in_ParentScale);
        NiMatrix3 matrix;
        ComputeLookAtRotation(io_Light, interestPoint, matrix);
        io_Light.SetRotate(matrix);
    }

    //---------------------------------------------------------------------------
    // Convert normal light orientation.
    //---------------------------------------------------------------------------
    void ConvertLightOrientation(NiLight& io_Light)
    {
        // Note: Gamebryo looks down the X axis, while XSI seems to look down Z axis,
        //       so rotation 90 degrees around the Y axis.
        NiMatrix3 rotY90;
        rotY90.FromEulerAnglesZYX(
            epg::DegreeToRadian(0.0f),
            epg::DegreeToRadian(-90.0f),
            epg::DegreeToRadian(0.0f));
        io_Light.SetRotate(io_Light.GetRotate() * rotY90);
    }

    //---------------------------------------------------------------------------
    // Convert the light position.
    //---------------------------------------------------------------------------
    void ConvertLightPosition(
        epg::Context & io_Context,
        CSLModel& in_Model,
        CSLLight& in_Light,
        NiLight& io_Light)
    {
        epg::NodeMap& nodeMap = io_Context.GetNodeMap();
        CSLModel* parent = in_Model.Parent();
        const CSIBCVector3D* parentScale = parent ? nodeMap.FindModelScale(*parent) : NULL;

        NiPoint3 point;
        epg::ConvertTranslation(in_Light.GetPosition(), point, parentScale);
        io_Light.SetTranslate(point);

        switch (in_Light.Type())
        {
            case CSLLight::SI_SPOT_LIGHT:
            {
                CSLSpotLight& spot = (CSLSpotLight&) in_Light;
                CSIBCVector3D interestPos = spot.GetInterestPosition();
                if (interestPos != CSIBCVector3D(0.0f, 0.0f, 0.0f))
                {
                    ConvertSpotlightOrientation(parentScale, interestPos, io_Light);
                }
                else
                {
                    io_Context.Logf(
                        epg::LOG_WARNING,
                        "Spot light \"%s\" lacks an interest point"
                        " (probably because it was converted from another"
                        " light type) and its orientation will be incorrect.",
                        in_Light.Name().GetText());
                }
                break;
            }

            default:
            {
                io_Context.Logf(
                    epg::LOG_WARNING,
                    "Light \"%s\" has an unknown light type, treating as a generic light.",
                    in_Light.Name().GetText());
                // Fall-through, no break;
            }

            case CSLLight::SI_INFINITE_LIGHT:
            case CSLLight::SI_POINT_LIGHT:
            case CSLLight::SI_DIRECTIONAL_LIGHT:
            {
                ConvertLightOrientation(io_Light);
                break;
            }
        }
    }

    //---------------------------------------------------------------------------
    // Convert the XSI lightning property
    //---------------------------------------------------------------------------
    void ConvertLightingProperty(
        epg::Context & io_Context,
        CSLTemplate& in_Tmpl,
        NiLight& in_Light)
    {
        io_Context.GetEffectMap().AddEffect(in_Tmpl, in_Light);

        NiFixedString shadowTechnique;
        std::vector<NiFixedString> tags;
        if (epg::LoadLightingProperty(in_Tmpl, shadowTechnique, tags))
        {
            typedef std::vector<NiFixedString>::iterator iter;
            for (iter pos = tags.begin(); pos != tags.end(); ++pos)
                if (pos->GetLength() > 0)
                    io_Context.GetLightingMap().AddLight(*pos, in_Light, shadowTechnique);
        }
    }

    //---------------------------------------------------------------------------
    // Basic light conversion: position, connection, color, intensity,
    // affected object list.
    //---------------------------------------------------------------------------
    bool ConvertLight(
        epg::Context & io_Context,
        CSLModel& in_Model,
        CSLLight& in_Light,
        NiLight& io_Light,
        LightFallofSupport in_SupportFallof)
    {
        const bool visibleColor = ConvertLightColor(
            io_Context,
            in_Light,
            in_Light.GetColor(),
            io_Light,
            in_Light.Name().GetText(),
            "color",
            LIGHT_DIFFUSE_COLOR | LIGHT_SPECULAR_COLOR);
        if (!visibleColor)
            return false;

        if (!epg::ConvertCommonProperties(io_Context, in_Model, io_Light))
            return false;

        // Note: ConvertLightPosition() must be done after ConvertCommonProperties() because
        //       XSI stores the position in the light, not in the transform, so we need
        //       to overwrite whatever was done in ConvertCommonProperties().
        ConvertLightPosition(io_Context, in_Model, in_Light, io_Light);

        CSLLightInfo* lInfo = in_Light.LightInfo();
        if (lInfo)
        {
            if (!ConvertLightIntensity(io_Context, *lInfo, io_Light, in_Model.Name().GetText()))
                return false;

            if (!in_SupportFallof)
            {
                if (lInfo->GetFallofActive())
                {
                    io_Context.Logf(
                        epg::LOG_WARNING,
                        "Light falloff used in \"%s\" is not supported in this type of light.",
                        in_Model.Name().GetText());
                }
            }
        }

        ConvertLightingProperty(io_Context, in_Light, io_Light);

        return true;
    }

    //---------------------------------------------------------------------------
    // Retrieve the falloff exponent.
    //---------------------------------------------------------------------------
    float GetLightFalloffExponent(
        epg::Context & io_Context,
        CSLModel& in_Model,
        CSLLight& in_Light)
    {
        CSLLightInfo* lightInfo = in_Light.LightInfo();
        if (lightInfo)
        {
            if (lightInfo->GetFallofActive())
            {
                switch (lightInfo->GetFallofMode())
                {
                    case CSLLightInfo::SI_LINEAR:
                        return 1.0f;
                    case CSLLightInfo::SI_EXPONENT:
                        // Note: in XSI, the exponent is taken from a user-settable
                        // parameter. This parameter is not exported in dotXSI
                        // so we use a hard-coded quadratic fall-off.
                        io_Context.Logf(
                            epg::LOG_WARNING,
                            "Assuming quadratic fall-off for exponent fall-off in light \"%s\".",
                            in_Model.Name().GetText());
                        return 2.0f;
                    default:
                        io_Context.Logf(
                            epg::LOG_WARNING,
                            "Unknown falloff mode in light \"%s\".",
                            in_Model.Name().GetText());
                        break;
                }
            }
        }

        return 0.0f;
    }

    //---------------------------------------------------------------------------
    // Retrieve the end of the attenuation, if any.
    //---------------------------------------------------------------------------
    float GetLightAttenuationEnd(CSLLight& in_Light, const float in_Default)
    {
        CSLLightInfo* lightInfo = in_Light.LightInfo();
        return lightInfo ? lightInfo->GetEnd() : in_Default;
    }

    //---------------------------------------------------------------------------
    // Convert XSI falloff to the Gamebryo point-light attenuation.
    //---------------------------------------------------------------------------
    void ConvertAttenuation(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLLight& in_XSILight,
        NiPointLight& io_GBLight)
    {
        const float falloffExponent = GetLightFalloffExponent(io_Context, in_Model, in_XSILight);
        NIASSERT(falloffExponent == 0 || falloffExponent == 1 || falloffExponent == 2);

        // Note: the value of 4 was taken from the 3Dmax exporter, which we assume was
        //       arrived at by a highly scientific method.
        static const float ATTENUATION_NUMERATOR = 4.0f;
        const float attenuationEnd = GetLightAttenuationEnd(in_XSILight, ATTENUATION_NUMERATOR);
        if (0.0f == falloffExponent)
        {

            io_GBLight.SetConstantAttenuation(1.0f);
            io_GBLight.SetLinearAttenuation(0.0f);
            io_GBLight.SetQuadraticAttenuation(0.0f);
        }
        else if (1.0f == falloffExponent)
        {
            io_GBLight.SetConstantAttenuation(0.0f);
            io_GBLight.SetLinearAttenuation(ATTENUATION_NUMERATOR / attenuationEnd);
            io_GBLight.SetQuadraticAttenuation(0.0f);
        }
        else if (2.0f == falloffExponent)
        {
            io_GBLight.SetConstantAttenuation(0.0f);
            io_GBLight.SetLinearAttenuation(0.0f);
            io_GBLight.SetQuadraticAttenuation(ATTENUATION_NUMERATOR * ATTENUATION_NUMERATOR
                                            / attenuationEnd * attenuationEnd);
        }
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Convert lights.
    //---------------------------------------------------------------------------

    //---------------------------------------------------------------------------
    bool ConvertAmbientLight(Context & io_Context, CSLAmbience& in_Model)
    {
        if (!io_Context.GetSettings().GetExportLights())
            return true;

        NiAmbientLightPtr light = NiNew NiAmbientLight;

        const bool visibleColor = ConvertLightColor(
            io_Context,
            in_Model,
            in_Model.GetColor(),
            *light,
            "ambient light",
            "color",
            LIGHT_AMBIENT_COLOR);
        if (!visibleColor)
            return true;

        io_Context.GetNodeMap().AddRoot(*light);
        ConvertLightingProperty(io_Context, in_Model, *light);

        return true;
    }

    //---------------------------------------------------------------------------
    bool ConvertDirectionalLight(Context & io_Context, CSLModel& in_Model)
    {
        if (!io_Context.GetSettings().GetExportLights())
            return true;

        CSLDirectionalLight* lightXSI = static_cast<CSLDirectionalLight*>(in_Model.Primitive());
        if (!lightXSI)
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Invalid XSI directional light \"%s\".",
                in_Model.Name().GetText());
        }

        NiDirectionalLightPtr lightGB = NiNew NiDirectionalLight;
        if (!ConvertLight(io_Context, in_Model, *lightXSI, *lightGB, LIGHT_FALLOF_NOT_SUPPORTED))
        {
            // Note: if ConvertLight() returns false, the light color was invisible
            //       so return true and pretend the light was converted.
            return true;
        }

        NiMatrix3 rotation;
        if (!ConvertRotation(
            io_Context,
            in_Model,
            lightXSI->GetDirection(),
            CSLTemplate::SI_XYZ,
            rotation))
        {
            return false;
        }

        lightGB->SetRotate(rotation * lightGB->GetRotate());

        return true;
    }

    //---------------------------------------------------------------------------
    bool ConvertPointLight(Context & io_Context, CSLModel& in_Model)
    {
        if (!io_Context.GetSettings().GetExportLights())
            return true;

        CSLPointLight* lightXSI = static_cast<CSLPointLight*>(in_Model.Primitive());
        if (!lightXSI)
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Invalid XSI point light \"%s\".",
                in_Model.Name().GetText());
        }

        NiPointLightPtr lightGB = NiNew NiPointLight;
        if (!ConvertLight(io_Context, in_Model, *lightXSI, *lightGB, LIGHT_FALLOF_SUPPORTED))
        {
            // Note: if ConvertLight() returns false, the light color was invisible
            //       so return true and pretend the light was converted.
            return true;
        }

        ConvertAttenuation(io_Context, in_Model, *lightXSI, *lightGB);

        return true;
    }

    //---------------------------------------------------------------------------
    bool ConvertSpotLight(Context & io_Context, CSLModel& in_Model)
    {
        if (!io_Context.GetSettings().GetExportLights())
            return true;

        CSLSpotLight* lightXSI = static_cast<CSLSpotLight*>(in_Model.Primitive());
        if (!lightXSI)
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Invalid XSI spot light \"%s\".",
                in_Model.Name().GetText());
        }

        NiSpotLightPtr lightGB = NiNew NiSpotLight;
        if (!ConvertLight(io_Context, in_Model, *lightXSI, *lightGB, LIGHT_FALLOF_SUPPORTED))
        {
            // Note: if ConvertLight() returns false, the light color was invisible
            //       so return true and pretend the light was converted.
            return true;
        }

        ConvertAttenuation(io_Context, in_Model, *lightXSI, *lightGB);

        const float falloffExponent = GetLightFalloffExponent(io_Context, in_Model, *lightXSI);
        lightGB->SetSpotExponent(falloffExponent > 0.0f ? falloffExponent : 1.0f);

        // Note: the XSI and Gamebryo spotlight angles are not defined the same way.
        // First, the angles in XSI are the border-to-border angles while in Gamebryo
        // the angle is from the center. Thus XSI angles have to be divided by two.
        // In XSI, we have a cone angle for the main light code and a small spread angle
        // that is within the outer border of the cone. In Gamebryo, the spot angle is
        // the full cone and the inner cone is the angle of full intensity:
        //
        //   - Gamebryo spot angle = XSI cone / 2.
        //   - Gamebryo inner spot angle = (XSI cone - XSI spread) / 2.
        lightGB->SetSpotAngle(lightXSI->GetConeAngle() / 2);
        if (lightXSI->GetSpreadAngle() < lightXSI->GetConeAngle())
            lightGB->SetInnerSpotAngle((lightXSI->GetConeAngle()-lightXSI->GetSpreadAngle()) / 2);
        else
            lightGB->SetInnerSpotAngle(0.0f);

        return true;
    }

    //---------------------------------------------------------------------------
    bool ConvertInfiniteLight(Context & io_Context, CSLModel& in_Model)
    {
        if (!io_Context.GetSettings().GetExportLights())
            return true;

        // Note: XSI infinite light does not have an explicit direction.
        // XSI infinite light are actually exported as a directional light
        // in the dotXSI format.

        CSLInfiniteLight* lightXSI = static_cast<CSLInfiniteLight*>(in_Model.Primitive());
        if (!lightXSI)
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Invalid XSI infinite light \"%s\".",
                in_Model.Name().GetText());
        }

        NiDirectionalLightPtr lightGB = NiNew NiDirectionalLight;
        if (!ConvertLight(io_Context, in_Model, *lightXSI, *lightGB, LIGHT_FALLOF_NOT_SUPPORTED))
        {
            // Note: if ConvertLight() returns false, the light color was invisible
            //       so return true and pretend the light was converted.
            return true;
        }

        return true;
    }

    //---------------------------------------------------------------------------
}
