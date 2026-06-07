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

// MyiRotData.h

#ifndef MYIROTDATA_H
#define MYIROTDATA_H

#include "MyiObject.h"
#include <NiEulerRotKey.h>
#include <NiBezFloatKey.h>

class MyiRotData : public MyiObject
{
    public:
        MyiRotData(int iComponentID);

        ~MyiRotData();

        inline NiEulerRotKey* GetRotData();

        inline NiRotKey* GetQuatData();
        inline int GetQuatKeyCount();

        bool MakeCurve(int iComponentID, int axis);

        bool IsQuaternion();

    protected:
        void CheckKeys(const char *pName);
        float FirstKeyTime(int iComponentID);

        int m_iKeys[4];
        NiFloatKey *m_pNiKeys[4];
        
        NiEulerRotKey *m_pEuler;
        NiRotKey* m_pQuaternion;

};
//---------------------------------------------------------------------------
inline NiEulerRotKey *MyiRotData::GetRotData()
{
    return m_pEuler;
}
//---------------------------------------------------------------------------
inline int MyiRotData::GetQuatKeyCount()
{
    return m_iKeys[3];
}
//---------------------------------------------------------------------------
inline NiRotKey* MyiRotData::GetQuatData()
{
    return m_pQuaternion;
}
//---------------------------------------------------------------------------

#endif
