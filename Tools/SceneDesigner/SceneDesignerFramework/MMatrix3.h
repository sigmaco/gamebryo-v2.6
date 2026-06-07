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

#pragma once

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    [SerializableAttribute]
    public __gc class MMatrix3
    {
    public:
        MMatrix3();
        MMatrix3(const NiMatrix3& kMatrix3);

        void SetData(const NiMatrix3& kMatrix3);
        void ToNiMatrix3(NiMatrix3& kMatrix3);

        __property float get_XAngle();
        __property float get_YAngle();
        __property float get_ZAngle();
        void FromEulerAnglesXYZ(float fXAngle, float fYAngle, float fZAngle);
        //System::Object overrides
        virtual bool Equals(Object* pmObj);

    private:
        void ComputeEulerAnglesXYZ();

        float m_afData __gc [];

        float m_fXAngle;
        float m_fYAngle;
        float m_fZAngle;
        bool m_bRecomputeEulerAngles;
    };
}}}}
