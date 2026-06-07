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
    public __gc class MPoint3
    {
    public:
        MPoint3();
        MPoint3(float fX, float fY, float fZ);
        MPoint3(const NiPoint3& kPoint3);

        void SetData(const NiPoint3& kPoint3);
        void ToNiPoint3(NiPoint3& kPoint3);

        __property float get_X();
        __property void set_X(float fX);

        __property float get_Y();
        __property void set_Y(float fY);

        __property float get_Z();
        __property void set_Z(float fZ);

        static MPoint3* Add(MPoint3* pmA, MPoint3* pmB);
        static MPoint3* Subtract(MPoint3* pmA, MPoint3* pmB);
        static MPoint3* Multiply(float fScalar, MPoint3* pmB);
        static float Dot(MPoint3* pmA, MPoint3* pmB);
        static MPoint3* Cross(MPoint3* pmA, MPoint3* pmB);
        static MPoint3* Unitize(MPoint3* pmA);
        static float Length(MPoint3* pmA);

        //System::Object overrides
        virtual bool Equals(Object* pmObj);

    private:
        float m_fX;
        float m_fY;
        float m_fZ;
    };
}}}}
