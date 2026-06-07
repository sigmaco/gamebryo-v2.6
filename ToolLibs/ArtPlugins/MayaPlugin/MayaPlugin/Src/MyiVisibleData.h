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

#ifndef MYIVISIBLEDATA_H
#define MYIVISIBLEDATA_H

#include <NiStepBoolKey.h>
#include "MyiObject.h"

class MyiVisibleData : public MyiObject
{
    public:
        inline void GetVisibleData(NiStepBoolKey *&pVisibleKeys, int &iKeys);

        inline NiStepBoolKey* GetKeys();
        inline int GetNumKeys();

        MyiVisibleData( int iComponentID );
        ~MyiVisibleData( void );

    protected:
        bool FillNiKeys( int iComponentID );

        int m_iKeys;
        NiStepBoolKey *m_pNiKeys;
};

//---------------------------------------------------------------------------
inline void MyiVisibleData::GetVisibleData(NiStepBoolKey*& pVisibleKeys, 
    int &iKeys)
{
    pVisibleKeys = m_pNiKeys;
    iKeys = m_iKeys;
}
//---------------------------------------------------------------------------
inline NiStepBoolKey* MyiVisibleData::GetKeys()
{
    return m_pNiKeys;
}
//---------------------------------------------------------------------------
inline int MyiVisibleData::GetNumKeys()
{
    return m_iKeys;
}
//---------------------------------------------------------------------------

#endif
