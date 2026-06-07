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
    public __gc class MQuaternion
    {
    public:
        MQuaternion();
        MQuaternion(const NiQuaternion& kQuaternion);

        void SetData(const NiQuaternion& kQuaternion);
        void ToNiQuaternion(NiQuaternion& kQuaternion);

        __property float get_W();
        __property void set_W(float fW);

        __property float get_X();
        __property void set_X(float fX);

        __property float get_Y();
        __property void set_Y(float fY);

        __property float get_Z();
        __property void set_Z(float fZ);

        __property float get_XAngle();
        __property float get_YAngle();
        __property float get_ZAngle();
        void FromEulerAnglesXYZ(float fXAngle, float fYAngle, float fZAngle);

        //System::Object overrides
        virtual bool Equals(Object* pmObj);

    private:
        void ComputeEulerAnglesXYZ();

        float m_fW;
        float m_fX;
        float m_fY;
        float m_fZ;

        float m_fXAngle;
        float m_fYAngle;
        float m_fZAngle;
        bool m_bRecomputeEulerAngles;
    };
}}}}
