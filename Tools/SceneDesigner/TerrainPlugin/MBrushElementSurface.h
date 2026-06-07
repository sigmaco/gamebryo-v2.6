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

#ifndef MBRUSHELEMENTSURFACE_H
#define MBRUSHELEMENTSURFACE_H

#include "MBrushElement.h"

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    struct PointLocationKeyMapFunctors
    {
        static bool IsKeysEqual(const NiPoint3* pkKey1, 
            const NiPoint3* pkKey2);
        static unsigned int KeyToHashIndex(const NiPoint3* pkKey1, 
            unsigned int uiTableSize);
    };

    /** 
        Brush elements define what layer the brush operates upon.
     */
    public __gc class MBrushElementSurface : public MBrushElement
    {
    private:

        /// Surface associated with the element
        const NiSurface* m_pkCurSurface;

        System::Windows::Forms::ComboBox* m_pmPackageBox;

        System::Windows::Forms::ComboBox* m_pmSurfaceBox;

        System::Windows::Forms::PictureBox* m_pmPicture;

        NiTMap<const NiPoint3*, float, PointLocationKeyMapFunctors, 
            PointLocationKeyMapFunctors>* m_pkPointMap;

    public:

        bool m_bPreventUpdate;

        /** 
            Default constructor
         */
        MBrushElementSurface();

        MBrushElementSurface(MBrushElementSurface* pkToCopy);

        /** 
            Fetches the associated surface.
            
            @return returns the Surface associated with the element
         */
        const NiSurface* GetSurface();

        /** 
            Sets the surface.
            
            @param pkSurface The surface to be set towards
         */
        void SetSurface(const NiSurface* pkSurface);

        virtual MBrushElement* CloneOperation();
        
        virtual float GetValue(const NiDeformablePoint* pkPoint);
        virtual float GetSmoothedValue(NiTerrainInteractor* pkInteractor,
            NiDeformablePointSet* pkPointSet, 
            const NiDeformablePoint* pkPoint);
        virtual void SetValue(NiDeformablePoint* pkPoint,float fValue);
        virtual float GetValue(NiTerrainInteractor* pkTerrain, 
            const NiPoint3& kWorldLocation);
        
        virtual void CommitPoints(NiTerrainInteractor* pkTerrain, 
            NiDeformablePointSet* pkPoints, 
            const NiPoint3& kOrigin, const float& fRadius);
        
        virtual unsigned int PreparePoints(const NiBound& kBound, 
            NiTerrainInteractor* pkTerrain, 
            NiDeformablePointSet* pkPoints);

        virtual void Update(MBrushOperation* pmOperation);

        virtual void GetOperationFullPanel(
            System::Windows::Forms::Panel* pkPanel,
            bool bInList,
            MBrushOperation* pmOperation);
        
        virtual void ReadValues(MBrushElement* pmElement);

        virtual void GetPointSpacing(float& fSpacing);

        virtual void GetOptionInformation(
            System::Windows::Forms::Label* pmElementDesc);

        System::Void ComboBox_SelectedIndexChange(System::Object*  sender,
            System::EventArgs*  e);

    protected:
        virtual void SetIcon(NiSurface* pkSurface, MUCBrushElement* pmPanel);

        virtual void Do_Dispose(bool bDisposing);
    };

    #include "MBrushElementSurface.inl"
}
}
}
}

#endif