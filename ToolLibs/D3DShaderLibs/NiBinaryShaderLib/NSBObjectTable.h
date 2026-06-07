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

#ifndef NSBOBJECTTABLE_H
#define NSBOBJECTTABLE_H

#include "NiBinaryShaderLibLibType.h"
#include <NiShaderAttributeDesc.h>

class NIBINARYSHADERLIB_ENTRY NSBObjectTable : public NiMemObject
{
public:
    class ObjectDesc : public NiMemObject
    {
    public:
        ObjectDesc(const char* pcName,
            NiShaderAttributeDesc::ObjectType eType, unsigned int uiIndex);
        ~ObjectDesc();

        inline const char* GetName() const;
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

    ~NSBObjectTable();

    bool AddObject(const char* pcName,
        NiShaderAttributeDesc::ObjectType eType, unsigned int uiIndex);

    inline unsigned int GetObjectCount() const;
    inline ObjectDesc* GetFirstObject();
    inline ObjectDesc* GetNextObject();
    ObjectDesc* GetObjectByName(const char* pcName);

private:
    NiTListIterator m_kObjectListIter;
    NiTPointerList<ObjectDesc*> m_kObjectList;
};

#include "NSBObjectTable.inl"

#endif  // #ifndef NSBOBJECTTABLE_H
