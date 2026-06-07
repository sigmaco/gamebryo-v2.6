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

#ifndef MYIPOSDATA_H
#define MYIPOSDATA_H

#include <NiPosKey.h>
#include "MyiObject.h"

class MyiPosData : public MyiObject
{
    public:
        inline void GetPosData(NiPosKey *&pPosKeys, int &iKeys);
        MyiPosData( int iComponentID );

        ~MyiPosData( void );

    protected:
        bool FillNiKeys( int iComponentID );

        int m_iKeys;
        NiPosKey *m_pNiKeys;
};

//---------------------------------------------------------------------------
inline void MyiPosData::GetPosData(NiPosKey *&pPosKeys, int &iKeys)
{
    pPosKeys = m_pNiKeys;
    iKeys = m_iKeys;
}
//---------------------------------------------------------------------------

#endif
