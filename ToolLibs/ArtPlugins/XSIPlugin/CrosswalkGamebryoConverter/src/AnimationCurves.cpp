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

#include "CrosswalkGamebryoAnimCurves.h"
#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoSceneInfo.h"
#include "CrosswalkGamebryoAnimMap.h"
#include "CrosswalkGamebryoSettings.h"

#include "FCurve.h"
#include "ActionFCurve.h"
#include "XSITransform.h"
#include "StaticValue.h"

#include "NiObjectNET.h"

namespace
{
    //---------------------------------------------------------------------------
    // Value used to flag a missing key frame in a curve compared to other curves.
    //
    // Note: the floating point overflow is volontary.
    //---------------------------------------------------------------------------
    #pragma warning(push)
    #pragma warning(disable : 4056)
    static const float gInvalidKeyFrameValue = -1.0e100;
    #pragma warning(pop)

    //---------------------------------------------------------------------------
    // Retrieve the curve name for the given index from the specified array
    // of names. Return "..." for bad indexes or null names.
    //---------------------------------------------------------------------------
    const char* GetCurveName(const char* const in_CurveUserNames[], int in_AxisIndex)
    {
        return in_CurveUserNames && in_CurveUserNames[in_AxisIndex]
             ? in_CurveUserNames[in_AxisIndex]
             : "...";
    }

    //---------------------------------------------------------------------------
    // Allocate the given number of values from the allocator.
    //---------------------------------------------------------------------------
    float* AllocateInvalidValues(std::allocator<float>& io_Allocator, int in_CurveCount)
    {
        float* values = io_Allocator.allocate(in_CurveCount);
        for (int i = 0; i < in_CurveCount; ++i)
            values[i] = gInvalidKeyFrameValue;
        return values;
    }

    //---------------------------------------------------------------------------
    // Insert a value in the time-value container.
    //---------------------------------------------------------------------------
    void InsertTimeValue(
        const float in_GamebryoTime,
        const float in_Value,
        int in_Axis,
        int in_AxisCount,
        epg::AnimCurves::TimedValues& out_Values,
        std::allocator<float>& io_Allocator)
    {
        // Make sure we can detect later on axis without value by
        // initializing them to an invalid value.
        if (out_Values.find(in_GamebryoTime) == out_Values.end())
            out_Values[in_GamebryoTime] = AllocateInvalidValues(io_Allocator, in_AxisCount);

        // Set the value for the current curve.
        out_Values[in_GamebryoTime][in_Axis] = in_Value;
    }

    //---------------------------------------------------------------------------
    // Get the XSI time of the curve key at the specified index.
    //---------------------------------------------------------------------------
    float GetCurveKeyTime(CSLBaseFCurve& in_Curve, int in_Index)
    {
        switch (in_Curve.GetInterpolationType())
        {
            case CSLBaseFCurve::SI_CONSTANT:
                NIASSERT(in_Curve.GetConstantKeyListPtr());
                return in_Curve.GetConstantKeyListPtr()[in_Index].m_fTime;

            case CSLBaseFCurve::SI_LINEAR:
                NIASSERT(in_Curve.GetLinearKeyListPtr());
                return in_Curve.GetLinearKeyListPtr()[in_Index].m_fTime;

            case CSLBaseFCurve::SI_HERMITE:
                NIASSERT(in_Curve.GetHermiteKeyListPtr());
                return in_Curve.GetHermiteKeyListPtr()[in_Index].m_fTime;

            case CSLBaseFCurve::SI_BEZIER:
                NIASSERT(in_Curve.GetBezierKeyListPtr());
                return in_Curve.GetBezierKeyListPtr()[in_Index].m_fTime;

            case CSLBaseFCurve::SI_CUBIC:
                NIASSERT(in_Curve.GetCubicKeyListPtr());
                return in_Curve.GetCubicKeyListPtr()[in_Index].m_fTime;

            default:
                NIASSERT(0 == "Invalid curve interpolation type");
                return 0.0f;
        }
    }

    //---------------------------------------------------------------------------
    // Transfer the values from the given XSI FCurve into our container.
    // \return true if all curve were of proper form.
    //---------------------------------------------------------------------------
    bool TransferCurveValues(
        epg::Context& io_Context,
        const char *in_ObjectName,
        const epg::AnimCurvesDescriptor& in_CurvesDesc,
        CSLBaseFCurve** in_Curves,
        const epg::ActionTimeline& in_Timeline,
        epg::AnimCurves::TimedValues& out_Values,
        std::allocator<float>& io_Allocator)
    {
        // Assume everything will be fine and make it false when needed.
        bool isValid = true;

        const epg::SceneInfo& sceneInfo = io_Context.GetSceneInfo();

        // Retrieve all the value of all the keyframes of all the curves.
        const int axisCount = in_CurvesDesc.count;
        for (int axis = 0; axis < axisCount; ++axis)
        {
            if (!in_Curves[axis])
                continue;

            CSLBaseFCurve& curve = in_Curves[axis][0];

            const int keyCount = curve.GetKeyCount();
            if (keyCount < 0)
                continue;

            switch (curve.GetInterpolationType())
            {
                case CSLBaseFCurve::SI_LINEAR:
                case CSLBaseFCurve::SI_CONSTANT:
                {
                    // Linear and constant curve can use the key and values directly
                    // without sub-sampling.
                    for (int keyIndex = 0; keyIndex < keyCount; ++keyIndex)
                    {
                        const float xsiTime = GetCurveKeyTime(curve, keyIndex);
                        if (SI_SUCCESS != curve.Evaluate(xsiTime))
                        {
                            isValid = io_Context.Logf(
                                epg::LOG_ERROR,
                                "Cannot evaluate the %s animation of \"%s\" at time %fS.",
                                GetCurveName(in_CurvesDesc.printableNames, axis),
                                in_ObjectName,
                                sceneInfo.ConvertTime(xsiTime));
                            continue;
                        }

                        float time = sceneInfo.ConvertTime(xsiTime - in_Timeline.xsiStartTime)
                                         + in_Timeline.startTime;
                        float value = curve.GetLastEvaluation();
                        InsertTimeValue(time, value, axis, axisCount, out_Values, io_Allocator);
                    }
                    break;
                }

                case CSLBaseFCurve::SI_HERMITE:
                case CSLBaseFCurve::SI_BEZIER:
                case CSLBaseFCurve::SI_CUBIC:
                {
                    // All cubic curves are resampled for now.

                    // Retrieve curve XSI time end-points.
                    const float startTime = GetCurveKeyTime(curve, 0);
                    const float endTime = GetCurveKeyTime(curve, keyCount - 1);

                    // Calculate the XSI time step size based on the frame rate of the scene,
                    // the requested frames-per-second sampling and the time format used in
                    // the scene.
                    const float samplingRate = io_Context.GetSettings().GetKeyframeSamplingRate();
                    const float unitsPerSecond = sceneInfo.IsTimingInSeconds()
                                               ? 1.0f : sceneInfo.GetFramesPerSecond();
                    const float frameStep = unitsPerSecond
                                          / (samplingRate > 0 ? samplingRate : 30.0f);

                    // Sample the cubic curve.
                    for (float xsiTime = startTime; xsiTime <= endTime; xsiTime += frameStep)
                    {
                        if (SI_SUCCESS != curve.Evaluate(xsiTime))
                        {
                            isValid = io_Context.Logf(
                                epg::LOG_ERROR,
                                "Cannot evaluate the %s animation of \"%s\" at time %fS.",
                                GetCurveName(in_CurvesDesc.printableNames, axis),
                                in_ObjectName,
                                sceneInfo.ConvertTime(xsiTime));
                            continue;
                        }

                        float time = sceneInfo.ConvertTime(xsiTime - in_Timeline.xsiStartTime)
                                   + in_Timeline.startTime;
                        float value = curve.GetLastEvaluation();
                        InsertTimeValue(time, value, axis, axisCount, out_Values, io_Allocator);
                    }
                    break;
                }

                default:
                {
                    isValid = io_Context.Logf(
                        epg::LOG_ERROR,
                        "Animation curve for \"%s\" in \"%s\" use unsupported interpolation.",
                        GetCurveName(in_CurvesDesc.printableNames, axis),
                        in_ObjectName);
                    break;
                }
            }
        }

        return isValid;
    }

    //---------------------------------------------------------------------------
    // Transfer the values from the given XSI static values into our container.
    // \return true if all values were of proper form.
    //---------------------------------------------------------------------------
    bool TransferStaticValues(
        epg::Context& io_Context,
        const epg::AnimCurvesDescriptor& in_CurvesDesc,
        CSLStaticValue** in_Values,
        const epg::ActionTimeline& in_Timeline,
        epg::AnimCurves::TimedValues& out_Values,
        std::allocator<float>& io_Allocator)
    {
        const epg::SceneInfo& sceneInfo = io_Context.GetSceneInfo();

        // Retrieve all the value of all the keyframes of all the curves.
        const int axisCount = in_CurvesDesc.count;
        for (int axis = 0; axis < axisCount; ++axis)
        {
            if (!in_Values[axis])
                continue;

            const float value = in_Values[axis]->GetValue();
            InsertTimeValue(
                in_Timeline.startTime,
                value,
                axis,
                axisCount,
                out_Values,
                io_Allocator);
            InsertTimeValue(
                in_Timeline.startTime + in_Timeline.duration,
                value,
                axis,
                axisCount,
                out_Values,
                io_Allocator);
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Divide large deltas (for example, rotation of more than 180 degrees).
    // \return true if some large deltas were divided.
    //---------------------------------------------------------------------------
    bool DivideLargeDeltas(
        epg::Context& io_Context,
        const char* in_ObjectName,
        const epg::AnimCurvesDescriptor& in_CurvesDesc,
        epg::AnimCurves::TimedValues& io_Values,
        std::allocator<float>& io_Allocator)
    {
        if (in_CurvesDesc.maxDelta == 0.0f)
            return false;

        if (io_Values.size() < 2)
            return false;

        // Note: we work by axis because we must skip invalid values and the number
        //       and location of invalid values are per-axis.
        for (int axis = 0; axis < in_CurvesDesc.count; ++axis)
        {
            int largeDeltaDivisionCount = 0;

            typedef epg::AnimCurves::TimedValues::iterator iter;
            const iter end = io_Values.end();
            for (iter pos = io_Values.begin(); pos != end; ++pos)
            {
                // Be careful not to divide invalid values.
                if (gInvalidKeyFrameValue == pos->second[axis])
                    continue;

                // Find next valid value, if any.
                iter next = pos;
                while (++next != end)
                    if (gInvalidKeyFrameValue != next->second[axis])
                        break;
                if (end == next)
                    break;

                // Now we have valid values... check their deltas.
                const float delta = ::abs(next->second[axis] - pos->second[axis]);
                if (delta > in_CurvesDesc.maxDelta)
                {
                    ++largeDeltaDivisionCount;

                    const float steps    = 1 + ::ceil(delta / in_CurvesDesc.maxDelta);
                    const float timeStep = (next->first - pos->first) / steps;
                    // Note: insertion doesn't invalidate iterator because we use a std::map.
                    for (float time = pos->first + timeStep; time < next->first; time += timeStep)
                    {
                        io_Values[time] = AllocateInvalidValues(io_Allocator,in_CurvesDesc.count);
                    }
                }
            }

            // Note: we return as soon as an axis has new key frames inserted
            //       to avoid having multiple axis insert new keyframes at slightly
            //       different place between the same existing keyframes, resulting
            //       in excessive and unnecessary large number of new key frames.
            if (largeDeltaDivisionCount > 0)
            {
                return io_Context.Logf(
                    epg::LOG_INFO,
                    "Divided %d large deltas for the %s animation curve of \"%s\".",
                    largeDeltaDivisionCount,
                    GetCurveName(in_CurvesDesc.printableNames, axis),
                    in_ObjectName);
            }
        }

        return false;
    }

    //---------------------------------------------------------------------------
    // Interpolate missing values.
    // \return true if some values were interpolated.
    //---------------------------------------------------------------------------
    bool InterpolateMissingValues(
        epg::Context& io_Context,
        const char* in_ObjectName,
        const epg::AnimCurvesDescriptor& in_CurvesDesc,
        epg::AnimCurves::TimedValues& io_Values)
    {
        bool interpolated = false;

        for (int axis = 0; axis < in_CurvesDesc.count; ++axis)
        {
            int interpolateCount = 0;

            typedef epg::AnimCurves::TimedValues::iterator iter;
            const iter end = io_Values.end();
            iter prevValid = end;
            iter nextValid = io_Values.begin();
            for (iter pos = io_Values.begin(); pos != end; ++pos)
            {
                // If we reached the value we used for the next valid value
                // then go looking for another next valid value.
                if (nextValid == pos)
                {
                    while (++nextValid != end && gInvalidKeyFrameValue == nextValid->second[axis])
                    {
                        // Keep looking for a valid value...
                    }
                }

                // Now do necessary interpolation if needed.
                if (gInvalidKeyFrameValue != pos->second[axis])
                {
                    // Good value, keep it as previous valid.
                    prevValid = pos;
                }
                else
                {
                    if (prevValid != end)
                    {
                        ++interpolateCount;

                        if (nextValid != end)
                        {
                            // Both values are valid, so interpolate:
                            //
                            //  Yp = y0 + (y1 - y0) * (Xp - x0) / (x1 - x0)

                            pos->second[axis] = prevValid->second[axis]
                                           + (nextValid->second[axis] - prevValid->second[axis])
                                           * (pos->first - prevValid->first)
                                           / (nextValid->first - prevValid->first);
                        }
                        else
                        {
                            // Previous is valid but not next: use previous value.
                            // We do not extrapolate since in XSI, before a curve starts
                            // the value stays put.
                            pos->second[axis] = prevValid->second[axis];
                        }
                    }
                    else
                    {
                        if (nextValid != end)
                        {
                            ++interpolateCount;

                            // Next is valid but not previous: use next value.
                            // We do not extrapolate since in XSI, once a curve ends
                            // the value stays put.
                            pos->second[axis] = nextValid->second[axis];
                        }
                        else
                        {
                            // No valid values, use zero. This basically means the curve
                            // was non-existant.
                            if (in_CurvesDesc.defaultValues)
                            {
                                pos->second[axis] = in_CurvesDesc.defaultValues[axis];
                            }
                            else
                            {
                                io_Context.Logf(
                                    epg::LOG_WARNING,
                                    "Non-existant animation curve \"%s\" was not given"
                                    " a default value of \"%s\", using zero.",
                                    GetCurveName(in_CurvesDesc.printableNames, axis),
                                    in_ObjectName);
                                pos->second[axis] = 0.0f;
                            }
                        }
                    }
                }
            }

            if (interpolateCount > 0)
            {
                io_Context.Logf(
                    epg::LOG_INFO,
                    "Interpolated %d value for the %s animation curves of \"%s\".",
                    interpolateCount,
                    GetCurveName(in_CurvesDesc.printableNames, axis),
                    in_ObjectName);

                interpolated = true;
            }
        }

        return interpolated;
    }

    //---------------------------------------------------------------------------
    // Verify uniform curve (for example, Gamebryo doesn't support non-uniform
    // scaling), but let a small imprecision leeway.
    // \return true if the curve were uniform.
    //---------------------------------------------------------------------------
    bool VerifyUniformity(
        epg::Context& io_Context,
        const char* in_ObjectName,
        const epg::AnimCurvesDescriptor& in_CurvesDesc,
        epg::AnimCurves::TimedValues& io_Values)
    {
        static const float precision = 10000.0f;

        if (0 == (in_CurvesDesc.flags & epg::CURVE_MUST_BE_UNIFORM_FLAG))
            return true;

        typedef epg::AnimCurves::TimedValues::iterator iter;
        const iter end = io_Values.end();
        for (iter pos = io_Values.begin(); pos != end; ++pos)
        {
            for (int axis = 1; axis < in_CurvesDesc.count; ++axis)
            {
                const float val0     = pos->second[axis];
                const float val1     = pos->second[axis-1];
                const float absDelta = ::abs(val0 - val1);
                const float relDelta = (val0 != 0.0f) ?  absDelta / ::abs(val0) : relDelta;
                const float error    = ::floor(relDelta * precision);
                if (error > 1.0f)
                {
                    return io_Context.Logf(
                        epg::LOG_ERROR,
                        "The \"%s\" of object \"%s\" is animated non-uniformly"
                        " (i.e the values in the X, Y and Z axis are not equal),"
                        " which is not supported.",
                        GetCurveName(in_CurvesDesc.printableNames, axis),
                        in_ObjectName);
                }
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Compute derived values. Return true if everything is valid.
    //---------------------------------------------------------------------------
    bool ComputeDerivedValues(
        epg::Context& io_Context,
        const char* in_ObjectName,
        const epg::AnimCurvesDescriptor& in_CurvesDesc,
        CSLBaseFCurve** in_Curves,
        CSLStaticValue** in_Values,
        const epg::ActionTimeline& in_ValueTimeline,
        epg::AnimCurves::TimedValues& io_Values,
        std::allocator<float>& io_Allocator)
    {
        io_Values.clear();
        io_Allocator = std::allocator<float>();

        if (!TransferCurveValues(
            io_Context,
            in_ObjectName,
            in_CurvesDesc,
            in_Curves,
            in_ValueTimeline,
            io_Values,
            io_Allocator))
        {
            return false;
        }

        if (!TransferStaticValues(
            io_Context,
            in_CurvesDesc,
            in_Values,
            in_ValueTimeline,
            io_Values,
            io_Allocator))
        {
            return false;
        }

        // Note: we interpolate values before dividing large deltas because
        //       interpolating the missing values might reduce the deltas
        //       between valid point below the large delta threshold.
        bool curvesWereModified = true;
        while (curvesWereModified)
        {
            curvesWereModified = InterpolateMissingValues(
                io_Context,
                in_ObjectName,
                in_CurvesDesc,
                io_Values);
            curvesWereModified |= DivideLargeDeltas(
                io_Context,
                in_ObjectName,
                in_CurvesDesc,
                io_Values,
                io_Allocator);
        }

        if (!VerifyUniformity(io_Context, in_ObjectName, in_CurvesDesc, io_Values))
            return false;

        return true;
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Create the anim curves collection for the specified dummy node type.
    //
    // Note: XSI angles are in degrees while Gamebryo are in radians...
    //       so the caller must re-convert that.
    //---------------------------------------------------------------------------
    AnimCurves::AnimCurves(
        Context& io_Context,
        CSLTemplate& in_Tmpl,
        const char* in_ObjectName,
        const AnimCurvesDescriptor& in_CurvesDesc)
            : m_SceneInfo(io_Context.GetSceneInfo())
            , m_CurveCount(in_CurvesDesc.count)
            , m_IsValid(false)
    {
        Init(io_Context, &in_Tmpl, 0, in_ObjectName, in_CurvesDesc);
    }

    //---------------------------------------------------------------------------
    // Create the anim curves for the specified curves taken from the action descriptor.
    //---------------------------------------------------------------------------
    AnimCurves::AnimCurves(
        Context& io_Context,
        const ActionDescriptor& in_ActionDescriptor,
        const char* in_ObjectName,
        const AnimCurvesDescriptor& in_CurvesDesc)
            : m_SceneInfo(io_Context.GetSceneInfo())
            , m_CurveCount(in_CurvesDesc.count)
            , m_IsValid(false)
    {
        Init(io_Context, 0, &in_ActionDescriptor, in_ObjectName, in_CurvesDesc);
    }

    //---------------------------------------------------------------------------
    // Initialize the animation curves.
    //---------------------------------------------------------------------------
    void AnimCurves::Init(
        Context& io_Context,
        CSLTemplate* in_pTmpl,
        const ActionDescriptor* in_pActionDescriptor,
        const char* in_ObjectName,
        const AnimCurvesDescriptor& in_CurvesDesc)
    {
        if (in_CurvesDesc.count > MAX_ANIM_CURVES_COUNT)
        {
            io_Context.Logf(
                LOG_ERROR,
                "More than %d curves requested for \"%s\", cannot convert animation.",
                MAX_ANIM_CURVES_COUNT,
                in_ObjectName);
            return;
        }

        CSLBaseFCurve*  curves[MAX_ANIM_CURVES_COUNT] = { 0 };
        CSLStaticValue* values[MAX_ANIM_CURVES_COUNT] = { 0 };
        ActionTimeline valueTimeline = { 0.0f, 0.0f };

        const char * const * const curveNames = in_CurvesDesc.xsiNames;
        for (int i = 0; i < in_CurvesDesc.count; ++i)
        {
            if (curveNames && curveNames[i] && curveNames[i][0])
            {
                if (in_pTmpl)
                {
                    // Note: Crosswalk is not const-correct... oh well.
                    SI_Char* name = (SI_Char*) curveNames[i];
                    curves[i] = in_pTmpl->GetParameterFCurve(name);
                }
                else if (in_pActionDescriptor)
                {
                    const char* name = curveNames[i];
                    curves[i] = in_pActionDescriptor->FindAnimCurve(name);
                    values[i] = in_pActionDescriptor->FindStaticValue(name);
                    valueTimeline = in_pActionDescriptor->GetTimeline();
                }
            }
        }

        m_IsValid = ComputeDerivedValues(
                        io_Context,
                        in_ObjectName,
                        in_CurvesDesc,
                        curves,
                        values,
                        valueTimeline,
                        m_TimedValues,
                        m_ValueAllocator);
    }

    //---------------------------------------------------------------------------
    // Check if animation are used at all.
    //---------------------------------------------------------------------------
    bool AnimCurves::HasAnimation() const
    {
        return m_TimedValues.size() > 0;
    }

    //---------------------------------------------------------------------------
    // Verify that all the curve are compatible.
    //---------------------------------------------------------------------------
    bool AnimCurves::IsValid() const
    {
        return m_IsValid;
    }

    //---------------------------------------------------------------------------
    // Retrieve the number of curves.
    //---------------------------------------------------------------------------
    int AnimCurves::GetCurveCount() const
    {
        return m_CurveCount;
    }

    //---------------------------------------------------------------------------
    // Retrieves the curve values.
    //---------------------------------------------------------------------------
    const AnimCurves::TimedValues& AnimCurves::GetTimedValues() const
    {
        return m_TimedValues;
    }

    //---------------------------------------------------------------------------
}
