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

#ifndef XSI_GAMEBRYO_CONVERTER_ANIM_CURVES_H
#define XSI_GAMEBRYO_CONVERTER_ANIM_CURVES_H

#include "SLTypes.h"

#include "NiMainLibType.h"
#include "NiPoint3.h"

#include <map>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLTemplate;
class XSIEXPORT CSLFCurve;
class XSIEXPORT CSLXSITransform;

class NIMAIN_ENTRY NiObjectNET;

namespace epg
{
    class Context;
    class SceneInfo;
    class ActionDescriptor;
    enum DummyNodeType;

    //---------------------------------------------------------------------------
    //// Maximum number of animation curve in a curve group.
    //// For now, we have not seen anything beyond X, Y, Z.
    //---------------------------------------------------------------------------
    enum { MAX_ANIM_CURVES_COUNT = 3 };

    //---------------------------------------------------------------------------
    // Flags used to control details of animation curve conversion.
    //---------------------------------------------------------------------------
    enum AnimCurvesFlags
    {
        NO_ANIM_CURVES_FLAG         = 0,
        CURVE_MUST_BE_UNIFORM_FLAG  = 1 << 0,
        ALL_ANIM_CURVES_FLAGS       = 0xFFFF
    };

    //---------------------------------------------------------------------------
    //// Descriptor for a group of animation curves.
    ////
    //// @note willfully designed so that it can easily be statically initialized.
    //---------------------------------------------------------------------------
    struct AnimCurvesDescriptor
    {
        int count;
        const char* printableNames[MAX_ANIM_CURVES_COUNT];
        const char* xsiNames[MAX_ANIM_CURVES_COUNT];
        const float defaultValues[MAX_ANIM_CURVES_COUNT];
        float maxDelta;
        AnimCurvesFlags flags;
    };

    //---------------------------------------------------------------------------
    /// XSI supports animating various values: pivot, pivot compensation,
    /// neutral pose, etc. Each one uses different names for the animation curve.
    ///
    /// In addition, XSI animates each component of a transformation
    /// independently.
    ///
    /// On the other hand, Gamebryo uses a single animation object for all
    /// individual component of a given transform and does not support all
    /// the different parts of positioning; it is assumed that they get baked
    /// in a single node.
    ///
    /// We, on the other hand, to keep the code simple, create multiple dummy
    /// Gamebryo nodes corresponding to the XSI ways of doing things.
    ///
    /// The AnimCurves eases the transposition by doing the proper
    /// curve look-up based on the dummy node type and allows simple access
    /// to the independent curves of a single transform.
    ///
    /// The anim curve is made more generic to support any type of animation
    /// curves.
    //---------------------------------------------------------------------------

    class AnimCurves
    {
    public:
        /** The container of value arrays indexed by time.
        */
        typedef std::map<float, float *> TimedValues;

        /** Create the anim curves collection for the specified curves
            taken from the specified object.
        */
        AnimCurves(
            Context& io_Context,
            CSLTemplate& in_Tmpl,
            const char* in_ObjectName,
            const AnimCurvesDescriptor& in_CurvesDesc);

        /** Create the anim curves collection for the specified curves
            taken from the specified action descriptor.
        */
        AnimCurves(
            Context& io_Context,
            const ActionDescriptor& in_ActionDescriptor,
            const char* in_ObjectName,
            const AnimCurvesDescriptor& in_CurvesDesc);

        /** Check if animation are used at all.
            @return true if at least one animation is found.
        */
        bool HasAnimation() const;

        /** Verify that all the curve are compatible: linear curve with the same
            number of keys, and scaling is uniform.
        */
        bool IsValid() const;

        /** Retrieve the number of curves.
        */
        int GetCurveCount() const;

        /** Retrieves the curve values.
        */
        const TimedValues& GetTimedValues() const;

    private:
        void Init(
            Context& io_Context,
            CSLTemplate* in_pTmpl,
            const ActionDescriptor* in_pActionDescriptor,
            const char* in_ObjectName,
            const AnimCurvesDescriptor& in_CurvesDesc);

        const SceneInfo&            m_SceneInfo;
        const int                   m_CurveCount;
        TimedValues                 m_TimedValues;
        std::allocator<float>       m_ValueAllocator;
        bool                        m_IsValid;

        AnimCurves(const AnimCurves &);
        void operator=(const AnimCurves &);
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_ANIM_CURVES_H */
