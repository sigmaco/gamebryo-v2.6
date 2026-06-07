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

#ifndef NSBD3D10OBJECTTABLE_H
#define NSBD3D10OBJECTTABLE_H

#include "NiD3D10BinaryShaderLibLibType.h"
#include <NiShaderAttributeDesc.h>

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10ObjectTable : public NiMemObject
{
public:
    class ObjectDesc : public NiMemObject
    {
    public:
        ObjectDesc(const char* pcName,
            NiShaderAttributeDesc::ObjectType eType, unsigned int uiIndex);
        ~ObjectDesc();

        inline const char* GetName() inline const;
        void SetName(const char* pcName);

        inline NiShaderAttributeDesc::ObjectType GetType() const;
        inline void SetType(NiShaderAttributeDesc::ObjectType eType);

        inline unsigned int GetIndex() const;
        inline void SetIndex(unsigned int uiIndex);

    private:
        char* m_pcName;
        NiShaderAttributeDesc::ObjectType m_eType;
        unsigned int m_uiIndex;
    };

    ~NSBD3D10ObjectTable();

    bool AddObject(const char* pcName,
        NiShaderAttributeDesc::ObjectType eType, unsigned int uiIndex);

    inline unsigned int GetObjectCount() const;
    ObjectDesc* GetFirstObject();
    ObjectDesc* GetNextObject();
    ObjectDesc* GetObjectByName(const char* pcName);

private:
    NiTListIterator m_kObjectListIter;
    NiTPointerList<ObjectDesc*> m_kObjectList;
};

#include "NSBD3D10ObjectTable.inl"

#endif  // #ifndef NSBD3D10OBJECTTABLE_H
