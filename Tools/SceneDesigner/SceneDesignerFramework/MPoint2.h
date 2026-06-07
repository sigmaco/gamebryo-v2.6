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
    public __gc class MPoint2
    {
    public:
        MPoint2();
        MPoint2(float fX, float fY);
        MPoint2(const NiPoint2& kPoint2);

        void SetData(const NiPoint2& kPoint2);
        void ToNiPoint2(NiPoint2& kPoint2);

        __property float get_X();
        __property void set_X(float fX);

        __property float get_Y();
        __property void set_Y(float fY);

        static MPoint2* Add(MPoint2* pmA, MPoint2* pmB);
        static MPoint2* Subtract(MPoint2* pmA, MPoint2* pmB);

        //System::Object overrides
        virtual bool Equals(Object* pmObj);

    private:
        float m_fX;
        float m_fY;
    };
}}}}
