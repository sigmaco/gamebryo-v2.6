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

#ifndef MBRUSHELEMENTSCULPT_H
#define MBRUSHELEMENTSCULPT_H

#include "MBrushElement.h"

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    /** 
        Brush elements define what layer the brush operates upon.
     */
    public __gc class MBrushElementSculpt : public MBrushElement
    {
    
    public:

        /** 
            Default constructor
         */
        MBrushElementSculpt();

        MBrushElementSculpt(MBrushElementSculpt* pmToCopy);

        virtual MBrushElement* CloneOperation();

        virtual float GetValue(const NiDeformablePoint* pkPoint);
        virtual void SetValue(NiDeformablePoint* pkPoint,float fValue);
        virtual float GetValue(NiTerrainInteractor* pkTerrain,
            const NiPoint3& kWorldLocation);

        virtual void CommitPoints(NiTerrainInteractor* pkTerrain,
            NiDeformablePointSet* pkPoints, 
            const NiPoint3& kOrigin, const float& kRadius);
       
        virtual unsigned int MBrushElementSculpt::PreparePoints(
            const NiBound& kBound,
            NiTerrainInteractor* pkTerrain, 
            NiDeformablePointSet* pkPoints);

        virtual void GetPointSpacing(float& fSpacing);

    protected:

        virtual void Do_Dispose(bool bDisposing);
    };

    #include "MBrushElementSculpt.inl"
}
}
}
}

#endif