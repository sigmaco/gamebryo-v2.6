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

// Precompiled Header
#include "NiEntityPCH.h"

#include "NiEntityPropertyInterface.h"

NiFixedString NiEntityPropertyInterface::PT_FLOAT;
NiFixedString NiEntityPropertyInterface::PT_BOOL;
NiFixedString NiEntityPropertyInterface::PT_INT;
NiFixedString NiEntityPropertyInterface::PT_UINT;
NiFixedString NiEntityPropertyInterface::PT_SHORT;
NiFixedString NiEntityPropertyInterface::PT_USHORT;
NiFixedString NiEntityPropertyInterface::PT_STRING;
NiFixedString NiEntityPropertyInterface::PT_POINT2;
NiFixedString NiEntityPropertyInterface::PT_POINT3;
NiFixedString NiEntityPropertyInterface::PT_QUATERNION;
NiFixedString NiEntityPropertyInterface::PT_MATRIX3;
NiFixedString NiEntityPropertyInterface::PT_COLOR;
NiFixedString NiEntityPropertyInterface::PT_COLORA;
NiFixedString NiEntityPropertyInterface::PT_NIOBJECTPOINTER;
NiFixedString NiEntityPropertyInterface::PT_ENTITYPOINTER;

//---------------------------------------------------------------------------
void NiEntityPropertyInterface::_SDMInit()
{
    PT_FLOAT            = "Float";
    PT_BOOL             = "Boolean";
    PT_INT              = "Integer";
    PT_UINT             = "Unsigned Integer";
    PT_SHORT            = "Short";
    PT_USHORT           = "Unsigned Short";
    PT_STRING           = "String";
    PT_POINT2           = "Point2";
    PT_POINT3           = "Point3";
    PT_QUATERNION       = "Quaternion";
    PT_MATRIX3          = "Matrix3";
    PT_COLOR            = "Color (RGB)";
    PT_COLORA           = "Color (RGBA)";
    PT_NIOBJECTPOINTER  = "NiObject Pointer";
    PT_ENTITYPOINTER    = "Entity Pointer";
}
//---------------------------------------------------------------------------
void NiEntityPropertyInterface::_SDMShutdown()
{
    PT_FLOAT            = NULL;
    PT_BOOL             = NULL;
    PT_INT              = NULL;
    PT_UINT             = NULL;
    PT_SHORT            = NULL;
    PT_USHORT           = NULL;
    PT_STRING           = NULL;
    PT_POINT2           = NULL;
    PT_POINT3           = NULL;
    PT_QUATERNION       = NULL;
    PT_MATRIX3          = NULL;
    PT_COLOR            = NULL;
    PT_COLORA           = NULL;
    PT_NIOBJECTPOINTER  = NULL;
    PT_ENTITYPOINTER    = NULL;
}
//---------------------------------------------------------------------------
NiEntityPropertyInterface::NiEntityPropertyInterface() :
    m_kInstanceID(true)
{
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::IsAnimated() const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::IsAddPropertySupported()
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::AddProperty(
    const NiFixedString&, const NiFixedString&,
    const NiFixedString&, const NiFixedString&,
    const NiFixedString&)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::MakeCollection(const NiFixedString&, bool)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::RemoveProperty(const NiFixedString&)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    float&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    bool&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    int&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    unsigned int&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    short&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    unsigned short&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    NiFixedString&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    NiPoint2&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    NiPoint3&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    NiQuaternion&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    NiMatrix3&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    NiColor&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    NiColorA&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    NiObject*&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    NiEntityInterface*&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(const NiFixedString&,
    void*&, size_t&, unsigned int) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    float, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    bool, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    int, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    unsigned int, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    short, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    unsigned short, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    const NiFixedString&, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    const NiPoint2&, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    const NiPoint3&, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    const NiQuaternion&, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    const NiMatrix3&, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    const NiColor&, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    const NiColorA&, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    NiObject*, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    NiEntityInterface*, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(const NiFixedString&,
    const void*, size_t, unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
const NiUniqueID& NiEntityPropertyInterface::GetID() const
{
    return m_kInstanceID;
}
//---------------------------------------------------------------------------
void NiEntityPropertyInterface::SetID(const NiUniqueID& kID)
{
    NIASSERT(kID != NiUniqueID());
    m_kInstanceID = kID;
}
//---------------------------------------------------------------------------
const NiFixedString& NiEntityPropertyInterface::GetSourceFilename() const
{
    return m_kSourceFilename;
}
//---------------------------------------------------------------------------
void NiEntityPropertyInterface::SetSourceFilename(
    const NiFixedString& kSourceFilename)
{
    m_kSourceFilename = kSourceFilename;
}
//---------------------------------------------------------------------------
