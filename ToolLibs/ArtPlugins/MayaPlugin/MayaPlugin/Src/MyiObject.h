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

#ifndef MYIOBJECT_H
#define MYIOBJECT_H


enum eGlobalConstantEnums
{
    NI_MAX_MAYA_OBJECT_NAME_LENGTH = 256
};

class MyiObject : public NiMemObject
{
protected:
    NiDeclareFlags(NiUInt8);
    
public:
    MyiObject();
    inline bool Succeeded() const;
    inline bool Cancelled() const;
    inline bool Error() const;

protected:
    enum Flags {
        MYIOBJ_SUCCESS = 0x01,
        MYIOBJ_CANCEL = 0x02,
        MYIOBJ_ERROR = 0x04
    };
    
    inline void SetStatus(const Flags eStatus);
};

//---------------------------------------------------------------------------
inline MyiObject::MyiObject()
{
    m_uFlags = MYIOBJ_ERROR;
}
//---------------------------------------------------------------------------
inline bool MyiObject::Succeeded() const
{
    return GetBit(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
inline bool MyiObject::Cancelled() const
{
    return GetBit(MYIOBJ_CANCEL);
}
//---------------------------------------------------------------------------
inline bool MyiObject::Error() const
{
    return GetBit(MYIOBJ_ERROR);
}
//---------------------------------------------------------------------------
inline void MyiObject::SetStatus(const MyiObject::Flags eStatus)
{
    m_uFlags = 0;
    SetBit(true, (NiUInt8)eStatus);
}
//---------------------------------------------------------------------------
#endif
