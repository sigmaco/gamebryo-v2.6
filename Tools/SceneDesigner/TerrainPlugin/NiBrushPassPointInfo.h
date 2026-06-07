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

#ifndef NIBRUSHPASSPOINTINFO_H
#define NIBRUSHPASSPOINTINFO_H

#include <NiTerrainComponent.h>

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{
    using namespace System;
    using namespace System::Windows::Forms;
    using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
    using namespace Emergent::Gamebryo::SceneDesigner::Framework;

    class NiBrushPassPointInfo: public NiRefObject
    {
    public: class FalloffData;
    public:

        /// Array of deformable points containing the inner points
        NiTLargePrimitiveArray<NiDeformablePoint*> m_kInnerPoints; 

        /// Array of falloff data defining the falloff points
        NiTLargeObjectArray<FalloffData> m_kOuterPoints;

        /// Point set that all contained points belong too
        NiDeformablePointSetPtr m_spPointSet;

    public:

        /** 
            Default constructor            
         */
        NiBrushPassPointInfo():
            m_kInnerPoints(0,1),
            m_kOuterPoints(0,1),
            m_spPointSet(0)
        {}

        /** 
            Defines falloff data            
         */
        class FalloffData : public NiRefObject
        {
        public:
            FalloffData():
                m_pkActual(0),
                m_fInnerBoundaryValue(0.0f),
                m_fOuterBoundaryValue(0.0f),
                m_fInterpolatedModifier(0.0f)
            {}
            
            explicit FalloffData(int i) :
                m_pkActual(0),
                m_fInnerBoundaryValue(0.0f),
                m_fOuterBoundaryValue(0.0f),
                m_fInterpolatedModifier(0.0f)
            {
                NI_UNUSED_ARG(i);
            }

            /// The actual deformable point that this data represents
            NiDeformablePoint* m_pkActual;
        
            /// The best inner radius point to use to find the inner boundary 
            /// value
            NiDeformablePoint* m_pkInnerBoundaryPoint;

            //float m_fScale;

            float m_fInterpolatedModifier;

            /// inner boundary value to be used in falloff interpolation
            float m_fInnerBoundaryValue;

            /// outer boundary value to be used in falloff interpolation
            float m_fOuterBoundaryValue;
        };
    };

    /** 
        Operator overload to allow comparisons between to AeBrushPassPointInfo
            objects
        
     */
    bool operator!=(const NiBrushPassPointInfo::FalloffData& kS1, 
        const NiBrushPassPointInfo::FalloffData& kS2);
    
    /** 
        Operator overload to allow comparisons between to AeBrushPassPointInfo
            objects
        
     */
    bool operator==(const NiBrushPassPointInfo::FalloffData& kS1, 
        const NiBrushPassPointInfo::FalloffData& kS2);
}
}
}
}

#endif
